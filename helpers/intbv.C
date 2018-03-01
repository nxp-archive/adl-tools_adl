//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Implementation file for intbv:  Any non-templated stuff should go here.
//

#include <ostream>     // For ostream (operator<<)
#include <istream>     // For istream (operator>>)
#include <sstream>     // For stringstream
#include <iomanip>     // for setw
#include <string>
#include <iostream>

// We set this here so that the library will not have any RNumber dependencies.
// In reality, we may still be using RNumber, but this lets us get away with having
// only a single library.
#define __NO_RNUMBER__

#include "intbv.h"

using namespace std;

namespace adl {

  const int bits_per_word    = _INTBV_BITS_PER_WORD;
  const int bytes_per_word   = _INTBV_BYTES_PER_WORD;
  const int nibbles_per_word = _INTBV_BITS_PER_WORD/4;
  const int bits_per_nibble  = 4;

  const unsigned WordBytes = sizeof( unsigned );
  const unsigned HalfWordBits = _INTBV_BITS_PER_WORD / 2;
  const unsigned HalfWordRadix = ( (unsigned) 0x1 ) << HalfWordBits;
  const unsigned HalfWordMask  = HalfWordRadix - 1;
  const unsigned ByteBits  = 8;
  const unsigned ByteRadix = ( (unsigned) 0x1 ) << ByteBits;
  const unsigned ByteMask  = ByteRadix - 1;

  static inline uint32_t lowHalf( uint32_t n )
  {
    return n & HalfWordMask;
  }

  static inline uint32_t highHalf( uint32_t n )
  {
    return n >> HalfWordBits;
  }

  static inline uint32_t highLowConcat( uint32_t nh, uint32_t nl )
  {
    return ( nh << HalfWordBits ) | ( nl & HalfWordMask );
  }

  //
  // The multiply algorithm works as follows.
  //   Each number can be represented as a sum of 16-bit coefficients times a power of 2**16:
  //     a0 + a1*2**16 + a2*2**32 + a3*2**48 + ...
  //     b0 + b1*2**16 + b2*2**32 + b3*2**48 + ...
  //   The result of the multiplication can be written as:
  //     a0*b0 + (a0*b1 + a1*b0)*2**16 + (a0*b2+a1*b1+a2*b0)*2**32 + (a0*b3+a1*b2+a2*b1+a3*b0)*2**48 + ...
  //   Each of these multiplications can be performed by a native 32-bit multiply without
  //   losing any output bits, and each result word can be computed by adding the multiplication
  //   results appropriately so that carry-outs are propagated.
  //
  //   result words (order from least significant word on the right) as sums of products:
  //         res0      |        res1          ...
  //     ------------------------------------
  //     |    a0*b0   |
  //     ----------------------
  //            |     a0*b1    |
  //            |     a1*b0    |
  //             ---------------------
  //                    |    a0*b2    |
  //                    |    a1*b1    |
  //                    |    a2*b0    |
  //                     --------------------
  //                           |    a0*b3    |
  //                           |    a1*b2    |
  //                           |    a2*b1    |
  //                           |    a3*b0    |
  //                            -------------
  //
  // Following is pseudocode for the actual computation.  
  //    carry = 0
  //    for (i = 0; i < 2*wordCount; i += 2)
  //      lw = lowHalf (carry)
  //      hw = highHalf (carry)
  //      carry = 0
  //      for (j = 0; j <= i; j++)
  //        mult1 = a[j]*b[i-j]
  //        mult2 = a[j]*b[i-j+1]
  //        lw += lowHalf (mult1)
  //        hw += lowHalf (mult2) + highHalf (mult1)
  //        carry += highHalf (mult2)
  //      mult2 = a[i+1]*b[0]
  //      hw += lowHalf (mult2)
  //      carry += highHalf (mult2)
  //      hw += highHalf (lw)
  //      carry += highHalf (hw)
  //      result[i/2] = highLowConcat (lowHalf (hw), lowHalf (lw))
  //
  // Note1: This algorithm will not work on arbitrarily large numbers; at around
  //        32768 words, carryouts from the adds will be lost.
  // Note2: This is a big-endian representation, for a little-endian representation,
  //  just modify indexHalf to index at pos/2 and modify the result write back to
  //  write write at i/2.
 
  static inline uint32_t indexHalf (unsigned nw, const uint32_t *x, unsigned pos)
  {
    assert (pos/2 <= nw-1);
    // returns the correct half word
    if (pos%2)
      return highHalf(x[nw-1-pos/2]);
    else
      return lowHalf (x[nw-1-pos/2]);
  }

  void multiplyWords ( size_t nw,uint32_t *result,const uint32_t *a,const uint32_t *b)
  {
    uint32_t i,j,lw,hw,mult1,mult2;
    uint32_t carry = 0;

    for (i = 0; i < 2*nw; i += 2){
      lw = lowHalf (carry);
      hw = highHalf (carry);
      carry = 0;
      for (j = 0; j <= i; j++) {
        //std::cout<<"a["<<j<<"] b["<<i-j<<"]"<<std::endl;
        mult1 = indexHalf(nw,a,j)*indexHalf(nw,b,i-j);
        //std::cout<<"a["<<j<<"] b["<<i-j+1<<"]"<<std::endl;
        mult2 = indexHalf(nw,a,j)*indexHalf(nw,b,i-j+1);
        lw += lowHalf (mult1);
        hw += lowHalf (mult2) + highHalf (mult1);
        carry += highHalf (mult2);
      }
      //std::cout<<"a["<<i+1<<"] b[0]"<<std::endl;
      mult2 = indexHalf(nw,a,i+1)*indexHalf(nw,b,0);
      hw += lowHalf (mult2);
      carry += highHalf (mult2);
      hw += highHalf (lw);
      carry += highHalf (hw);
      result[nw-1-i/2] = highLowConcat (lowHalf (hw), lowHalf (lw));
    }
  }

  // Return the number of bytes in the number excluding leading zeros.
  static inline unsigned int skipLeadingZeroBytes( const unsigned char* n, unsigned int nLen )
  {
    assert( nLen != 0 );
    const unsigned char* p = n;
    while ( ( --nLen > 0 ) && ( !*p ) )
      p++;
    return ( ++nLen );
  }

  // Convert word-based number representation to byte-based representation.
  static inline void convertToBytes( const uint32_t * wordBuffer,
                                     unsigned char* byteBuffer,
                                     unsigned int wordCount )
  {  
    for ( unsigned i = 0; i < wordCount ; i++)
      {
        *(byteBuffer++) = ( wordBuffer[i] >> ( 3 * ByteBits ) ) & ByteMask;
        *(byteBuffer++) = ( wordBuffer[i] >> ( 2 * ByteBits ) ) & ByteMask;
        *(byteBuffer++) = ( wordBuffer[i] >> ByteBits ) & ByteMask;
        *(byteBuffer++) = wordBuffer[i] & ByteMask;
      }
  }

  static inline void convertFromBytes (uint32_t * wordBuffer,
                                       const unsigned char* byteBuffer,
                                       unsigned int wordCount,unsigned int byteCount)
  {
    unsigned int shift = 0;
    unsigned int wc = wordCount-1;

    for (unsigned i = 0; i != wordCount; ++i) {
      wordBuffer[i] = 0;
    }
                                                                                                                                      
    for ( int i = byteCount; i >= 0; i-- )
      {
        assert (wc < wordCount);
        wordBuffer[wc] |= byteBuffer[i] << (shift * ByteBits);
        shift++;
        // every 4 bytes, increment wordCount
        if (shift%4==0)
          wc--;
      }
                                                                                                                                      
  }

  static void doDivide (unsigned char* xb, unsigned int xlen, unsigned char* yb, unsigned int ylen, unsigned char* q )
  {
    assert (xlen >= ylen);
    
    uint32_t qk;    // qk is a guess for q[k] such that q[k] = qk or qk - 1.
    uint32_t x3;    // most significant 3 bytes of x
    uint32_t y2;    // most significant 2 bytes of y.
    xb[-1] = 0;
    
    y2 = ( yb[0] << ByteBits ) + yb[1];
    
    // This represents dividend[i-1], which starts at 0 for when i == 0.
    uint32_t first_byte = 0;
    // Find each q[k]. qk is a guess for q[k] such that q[k] = qk or qk - 1.
    // Find qk by just using 2 bytes of y and 3 bytes of x.
    for ( uint32_t k = 0; k <= xlen - ylen; k++ ) {
      x3 = ( first_byte << ( 2 * ByteBits ) ) + ( xb[k] << ByteBits ) + xb[k + 1];
      qk = x3 / y2;
        
      // qk cannot be larger than the largest value in ByteRadix.
      if ( qk >= ByteRadix )
        qk = ByteRadix - 1;
        
      // Determine if q[k] = qk or qk - 1.
      if ( qk ) {
        int carry;
        int carry2;
        int diff;
            
        // adjust dividend based on divisor byte:
        //   x = x - y * qk;
        carry = 0;
            
        for ( int i = ylen - 1; i >= 0; i-- ) {
          carry += yb[i] * qk;
          diff = ( xb[k+i] + ByteRadix ) - ( carry & ByteMask );
          xb[k+i] = (unsigned char) ( diff & ByteMask );
          carry = ( carry >> ByteBits ) + ( 1 - ( diff >> ByteBits ) );
        }
            
        if ( carry ) {
          // extend the carry out to the most significant byte.
          carry = ( first_byte + ByteRadix ) - carry;
          first_byte = (unsigned char) ( carry & ByteMask );
          carry = 1 - ( carry >> ByteBits );
                
          if ( carry ) {
            // qk was one too large, so decrement it.
            --qk;
            // re-adjust dividend for correct divisor byte:
            //    x = x - y * (qk - 1) 
            //      = x - y * qk + y 
            //      = x_above + y.
            carry2 = 0;
                    
            for ( int i = ylen - 1; i >= 0; i-- ) {
              carry2 += xb[k+i] + yb[i];
              xb[k+i] = (unsigned char) ( carry2 & ByteMask );
              carry2 >>= ByteBits;
            }
                    
            if ( carry2 )
              first_byte = (unsigned char) ( ( first_byte + 1 ) & ByteMask );
          }
        }
      }
      q[k] = (unsigned char) qk;
      first_byte = xb[k];
    }
  }

  void divideWords(size_t Nw,uint32_t *res,const uint32_t *n1,const uint32_t *n2)
  {
    // Convert word-based representation to byte-based representation.  A separate
    // buffer is used so that it will work correctly on a little endian architecture.
    DeclArray(unsigned char,y,Nw * sizeof( uint32_t ) + 1);
    DeclArray(unsigned char,x,Nw * sizeof( uint32_t ) + 1);
    DeclArray(unsigned char,q,Nw * 2);

    for (unsigned int i = 0; i < Nw*2; i++)
      q[i] = 0;

    convertToBytes( n1, x, Nw );
    convertToBytes( n2, y, Nw );
    
    // Skip leading zeros in both numbers.
    unsigned int xlen = skipLeadingZeroBytes( x, Nw * sizeof( uint32_t ) );
    unsigned int ylen = skipLeadingZeroBytes( y, Nw * sizeof( uint32_t ) );  
    unsigned char* xb = x + Nw * sizeof( uint32_t ) - xlen;
    unsigned char* yb = y + Nw * sizeof( uint32_t ) - ylen;
    
    yb[ylen] = 0;   // in case ylen < 2
                                                                                                                                           
    if ( xlen <= WordBytes && ylen <= WordBytes )
      { // native divide
        res[Nw-1] =  n1[Nw-1] /  n2[Nw-1];
        //      rem[Nw-1] =  n1[Nw-1] - ( res[Nw-1] ) * n2[Nw-1];
      }
    else if (xlen < ylen)
      { // do nothing, we will just return 0 
      }
    else {
      doDivide( xb, xlen, yb, ylen, q );
      // Convert byte-based result to word-based result.
      convertFromBytes( res, q, Nw, xlen-ylen );
    }
  }

  //
  // Miscellaneous routines for dealing with word arrays.
  //

  inline bool isZero(const uint32_t *x,unsigned nw)
  {
    for (unsigned i = 0; i != nw; ++i) {
      if (x[i]) {
        return false;
      }
    }
    return true;
  }

  inline void initZero(unsigned nw,uint32_t *x)
  {
    memset(x,0,sizeof(uint32_t)*nw);
  }

  inline void setLowWord(unsigned nw,uint32_t *x,uint32_t value)
  {
    x[nw-1] = value;
  }

  inline void initLowWord(unsigned nw,uint32_t *x,uint32_t value)
  {
    memset(x,0,sizeof(uint32_t)*nw);
    setLowWord(nw,x,value);
  }

  inline void copyWords(unsigned nw,uint32_t *dest,const uint32_t *src)
  {
    memcpy(dest,src,sizeof(uint32_t)*nw);
  }

                                                                                                                                                 
  // Read in a hex value.
  char readHex( std::istream& is,int size, uint32_t * x)
  {    
    char c;  
    int num_words = _INTBV_WORDS(size);
    
    // we don't know the size of the input stream, so we use a temporary byte buffer
    std::string s;
    
     
    while ( 1 ){
      c = is.get();
      if ( isdigit( c ) ){
        s.push_back( c - '0' );
      }
      else if ( ( c = toupper( c ) ) >= 'A' && c <= 'F' ) {
        s.push_back( ( c - 'A' ) + 0xa );
      }
      else if ( c == '_' ) {
        // skip
      } else {
        is.putback(c);
        break;
      }
    }
    
    // if the string is too short, right justify
    // if the string was too long, truncate
    int w = num_words - 1;

    // iterate over the string in reverse, copying in from the least significant nibble to
    //  the most significant nibble into our intbv.  Terminate when we reach the 
    //  end of our intbv even if there is more left in the string.
    int nibble = 0;
    int num;
    for (int i = s.length() - 1; i >= 0; i--)
      {
        num = s[i];
        num <<= nibble * bits_per_nibble;
        x[w] |= num;
        nibble++;
        if (!(nibble%nibbles_per_word))
          {
            if (!w)
              break;
            w--;
            nibble = 0;
          }
      }
    
    return c;
  }

  // Read in a binary value.
  char readBin(  std::istream& is,int size, uint32_t * x  )
  {
    char c;
    int bit = 0;
    int num_words = _INTBV_WORDS(size);
    int i = 0;
    
    while ( 1 ) {
      c = is.get();
      if (bit == bits_per_word) {
        // advance to the next word
        bit = 0;
        i++;
      }
      if ( c == '0' ) {   
        if (i > num_words-1)
          return c;
        x[i] <<= 1;
        bit++;
      }
      else if ( c == '1' ) {
        if (i > num_words-1)
          return c;
        x[i] <<= 1;
        x[i] |= 1;
        bit++;
      }
      else if ( c == '_' ) {
        // skip
      } else {
        is.putback(c);
        return c;
      }
    }
  }

  // Read in a decimal value.
  char readDec(  std::istream& is,int size, uint32_t * x )
  {
    char c;
        
    int nw = _INTBV_WORDS(size);
    if (nw == 1) {
      // Use native reading.
      is >> *x;
      c = is.get();
      is.putback(c);
      return c;
    } else {

      assert(nw);
    
      // This represents the number 10.
      DeclArray(uint32_t,ten,nw);
      initLowWord(nw,ten,10);

      // This is a temporary used for addition.
      DeclArray(uint32_t,tmp,nw);
      initZero(nw,tmp);

      // This is our running result.
      initZero(nw,x);

      while ( 1 ) {
        c = is.get();
       
        if ( isdigit( c ) ) {
          multiplyWords(nw,x,x,ten);
          setLowWord(nw,tmp,(c - '0'));
          addWords(nw,x,x,tmp);
        }
        else if ( c == '_' ) {
          // skip
        } else {
          is.putback(c);
          return c;
        }
      }
    }
  }

  // General input support for hexadecimal, binary and decimal (the default)
  // numbers.  Only decimal numbers may include a unary plus or minus.
  // We absorb characters until we see a bad one (non-numeric and not '_'), then
  // we quit.
  void readNumberUnspecifiedFormat( std::istream& is , int size, uint32_t * x)
  {
    bool negative  = false;
    bool decimal = true;

    // Absorb any prior white-space and get the first char.
    char c = is.get();
    
    if ( c == '-' ) {
      // A unary minus; thus, we have a decimal, negated number.
      negative = true;
    }
    else if ( c == '+' ) {
      // A unary plus; thus, we have a decimal number.
    }
    else if ( c == '0' ) {
      // Octal numbers are *not* supported; thus, the next char is needed to
      // determine the base of the numeric.
      c = is.get();
        
      if ( c == 'x' ) {
        // Hex mode.
        decimal = false;
        readHex(is,size,x);
      }
      else if ( c == 'b' ) {
        // Binary mode.
        decimal = false;
        readBin(is,size,x);
      }
      else {
        // Not a recognized prefix, so put the character back.
        is.putback ( c );
        is.putback ( '0' );
      }
    }
    else {
      // Not an indicator character, so put it back.
      is.putback( c );
    }
    
    // If all else fails, we assume decimal notation.
    if ( decimal ) {
      readDec(is,size,x);
      // If the decimal number was prepended with a unary minus, negate it.
      if ( negative ) {
        // fix me: negate();
      }
    }
  }  

  void readNumber(std::istream& is,int size, uint32_t * x)
  {
    std::ios_base::fmtflags flags =  is.flags();

    if (flags & std::ios_base::hex) 
      readHex (is,size,x);
    else if (flags & std::ios_base::dec) 
      readDec (is,size,x);
    else if (flags & std::ios_base::oct)
      readBin (is,size,x);
    else 
      readNumberUnspecifiedFormat(is,size,x);      
  }

  void readNumber(const std::string& s,int format,int size, uint32_t * x)
  {
    using namespace intbv_io;

    std::istringstream ss (s); 
    // set the flags
    if (format & ihex)         
      ss.setf(std::ios_base::hex);
    else if (format & idec)
      ss.setf(std::ios_base::dec);
    else if (format & ibin)
      ss.setf(std::ios_base::oct);
    else
      ss.unsetf(std::ios_base::dec);
  
    readNumber(ss,size,x);
  }
     
  void printDec(std::ostream& os,int size, const uint32_t * x)
  {    
    // Set hex format and save old flags.
    std::ios::fmtflags old_options = os.flags( ( os.flags() & ~std::ios::basefield ) |
                                               std::ios::dec );
    
    int nw = _INTBV_WORDS(size);
    if (nw == 1) {
      // Size is one word or less, so use built-in decimal printing.
      os << *x;
    } else {
      // Larger than one word, so we have to do this one digit at a time.

      assert(nw);
      
      // This represents the number 10.
      DeclArray(uint32_t,ten,nw);
      initLowWord(nw,ten,10);

      // This is our quotient.
      DeclArray(uint32_t,quotient,nw);
      initZero(nw,quotient);

      // Temporary for determining text digit.
      DeclArray(uint32_t,tmp,nw);
      initZero(nw,tmp);

      // This is our temporary- when it's zero, we're done.
      DeclArray(uint32_t,dividend,nw);
      copyWords(nw,dividend,x);

      // This is our output buffer- it stores the character sequence
      // representing the number in base-10.
      const int len = (size+1)/2;
      DeclArray(char,buf,len);
      char *p = &buf[len-1];
      *p = 0;

      // Loop until we have a zero result.  Each iteration, we divide by
      // ten and use the remainder as our next digit in the sequence.
      while (!isZero(dividend,nw)) {
        divideWords(nw,quotient,dividend,ten);
        multiplyWords(nw,tmp,quotient,ten);
        subtractWords(nw,tmp,dividend,tmp);
        *(--p) = '0' + tmp[nw-1];
        copyWords(nw,dividend,quotient);
      }

      os << p;
    
    }

    os.flags( old_options );
  }

  void printBin(std::ostream& os,int size, const uint32_t * x) 
  {
    std::ios::fmtflags flags =  os.flags();    
    if (flags && std::ios::showbase) {
      os << "0b";
    }
    int num_words = _INTBV_WORDS(size);
    for (int i = 0; i < num_words; i++) {
      unsigned bitmask = 0x80000000;
      for(int j=0; j < bits_per_word; ++j) 
        { 
          if((x[i] & bitmask) == 0) 
            os<<'0';  
          else 
            os<<'1'; 
          bitmask >>= 1;
        }            
    }
  }   
  
  void printHex(std::ostream& os,int size, const uint32_t * x) 
  {
    // Set hex format and save old flags.
    std::ios::fmtflags old_options = os.flags( ( os.flags() & ~std::ios::basefield ) |
                                               std::ios::hex );
    // Hexadecimal format.
    unsigned int width = os.width();
    int num_words = _INTBV_WORDS(size);
    if (old_options & std::ios::showbase) {
      os << "0x";
    }    
    int i = 0; 
    os << noshowbase;
    // write out the most significant word
    if ( width > bits_per_word / 4 || ( width == 0 && size > 1 )) {
      uint32_t n = ( ( size % bits_per_word ) + 3 ) / 4;
      char oldFill = os.fill( '0' );
      if ( n == 0 )
        n = 8;
      os << std::setw( n ) << x[i];;
      os.fill( oldFill );
    } else {
      os << x[i];
    }

    i++;
    
    // write out the rest of the words
    char oldFill = os.fill( '0' );
    //int startBit =  size - (size %bits_per_word);
    //if (startBit == size) 
    //startBit -= bits_per_word;
    //startBit -= 1;
    for ( ;  i < num_words; i++) {
      os << std::setw( bits_per_word / 4 );
      os << x[i];
    }
    
    os.fill( oldFill );
    os.flags( old_options );
  }

  std::ostream& printToOS( std::ostream& os,int size, const uint32_t * x) 
  {
    std::ios_base::fmtflags flags =  os.flags();
    if (flags & std::ios_base::hex) {
      printHex (os,size,x);
    }
    else if (flags & std::ios_base::oct) {
      printBin (os,size,x);
    } 
    else {
      printDec(os,size,x);  
    }
    return os;
  }

  // copy to string
  std::string str (int format,int size, const uint32_t * x)
  {    
    using namespace intbv_io;

    std::ostringstream ss;

    if (format & iprefix) {
      ss.setf(std::ios_base::showbase);
    }
 
    if (format & iupper) {
      ss.setf(std::ios_base::uppercase);
    }
  
    if (format & ibin) {
      ss.setf(std::ios_base::oct);
    }
    else if (format & idec) {
      ss.setf(std::ios_base::dec);
    }
    else {   
      ss.setf(std::ios_base::hex);
    }

    printToOS(ss,size,x);    
    return ss.str();
  }

  // Prints the number to a given stream using the radix specified by the stream.
  // A prefix for the base is printed if the prefix flag is true.
  void writeNumber(std::ostream &os, int size, const uint32_t * x) 
  {
    unsigned int flags = os.flags() & std::ios::basefield;

    switch (flags) {
    case std::ios::hex:
      printHex( os,size,x );
      break;
    case std::ios::dec:
      printDec( os,size,x );
      break;
    default:
      printBin( os, size,x );
      break;
    }
  }

}
