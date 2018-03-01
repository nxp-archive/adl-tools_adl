//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _INTBV_BASE
#define _INTBV_BASE 

#include <cassert>       // For assert()
#include <cstddef>       // For size_t
#include <cstring>       // For memset
#include <stdexcept>     // For invalid_argument, out_of_range, overflow_error
#include <iosfwd>        // stream forward declaration
#include <limits.h>      // For CHAR_BIT

#include "stringfwd.h"   // string forward declaration
#include "BasicTypes.h"  // various integer typedefs
#include "BitTwiddles.h" // various low-level bit manipulation functions

#ifndef __NO_RNUMBER__
#include "rnumber/RNumber.h"
namespace rnumber {
  class RNumber;
}
#endif

namespace adl {

  inline void intbv_dummy(const uint32_t *x) {}

  const size_t _INTBV_BITS_PER_WORD = 32;
  const size_t _INTBV_BYTES_PER_WORD = _INTBV_BITS_PER_WORD/8;

  const size_t BitsPerInt = sizeof(int)*8;

#define _INTBV_WORDS(__n)                                               \
  ((__n) < 1 ? 0 : ((__n) + _INTBV_BITS_PER_WORD - 1)/_INTBV_BITS_PER_WORD)


#define _WORD_ALIGNED(__n) ((_INTBV_WORDS((__n)))*(_INTBV_BITS_PER_WORD))

  // non-member functions defined in intbv.C (for i/o support)
  char readHex(std::istream& is,int xlen, uint32_t* x);
  char readBin(std::istream& is,int xlen, uint32_t* x  );
  char readDec(std::istream& is,int xlen, uint32_t* x );
  void readNumber(std::istream& is , int xlen, uint32_t* x);
  void readNumber(const std::string& s,int format, int xlen, uint32_t* x);

  void printHex(std::ostream& os,int xlen, const uint32_t* x);
  void printBin(std::ostream& os,int xlen, const uint32_t* x);
  void printDec(std::ostream& os,int xlen, const uint32_t* x);
  void writeNumber(std::ostream &os, int xlen, const uint32_t* x);

#ifndef __NO_RNUMBER__
  rnumber::RNumber createRNumber(int size,const uint32_t* x);
  void constructFromRNumber(uint32_t* v,size_t numwords,const rnumber::RNumber &x);
#endif

  std::ostream& printToOS( std::ostream& os,int xlen, const uint32_t* x);
  std::string str (int format,int xlen, const uint32_t* x);


  // Divide routine on word arrays.
  void divideWords(size_t Nw,uint32_t *res,const uint32_t *x,const uint32_t *y);

  // Multiply routine on word arrays.
  void multiplyWords ( size_t nw, uint32_t *res, const uint32_t *x, const uint32_t *y);

  // Add routine on word arrays.
  static inline void addWords(size_t nw,uint32_t *res,const uint32_t *x,const uint32_t *y)
  {
    unsigned cin = 0;
    unsigned a,b,c,sum;
    for (int i = nw-1; i >=0; i--) {
      sum = x[i] + y[i] + cin;
      a = x[i];
      b = y[i];
      c = sum;
      cin = ( ( a & b ) | ( ( a | b ) & ~c ) ) >> ( _INTBV_BITS_PER_WORD - 1 );
      // write back the result
      res[i] = sum;
    }
  }

  // Subtract routine on word arrays.
  static inline void subtractWords(size_t nw,uint32_t *res,const uint32_t *x,const uint32_t *y)
  {
    unsigned cin = 1;
    unsigned a,b,c,diff,inv;
            
    for (int i = nw-1; i >=0; i--)
      {
        inv = ~y[i];
        diff = x[i] + inv + cin;
        a = x[i];
        b = inv;
        c = diff;
        cin = ( ( a & b ) | ( ( a | b ) & ~c ) ) >> ( _INTBV_BITS_PER_WORD - 1 );
        res[i] = diff;
      }
  }
  // Helper class to zero out the unused trailing bits in the least significant word.
  template<size_t _Extrabits>
  struct sanitize
  {
    static void _do_sanitize(uint32_t& val)
    { val &= ~((~static_cast<uint32_t>(0)) << _Extrabits); }

    static void _do_sanitize(uint64_t& val)
    { val &= ~((~static_cast<uint64_t>(0)) << (_INTBV_BITS_PER_WORD + _Extrabits)); }

//     static void _do_sanitize(uint64_t& val)
//     { val &= ~((~static_cast<uint64_t>(0)) << (_INTBV_BITS_PER_WORD + _Extrabits));
//     }

  };

  template<>
  struct sanitize<0> { 
    static void _do_sanitize(uint32_t) {} 
  };


  // Base class, general case.  It is a class inveriant that _Nw will be nonnegative.
  template<size_t _Nw>
  struct intbv_base 
  {
    // This has to be 16-byte aligned for newer GCCs in order to use -O3, since
    // it tries to vectorize some simple loops like operator&.  Oh, and for some
    // reason, this alignment doesn't work on RHEL5 in 32-bit mode, so I'm just
    // disabling it for 32-bit right now.  This can be removed once we move to
    // RHEL6.
    uint32_t NotMSVC(Is64Bit(__attribute__((aligned (16)))))  _w[_Nw];
        
    intbv_base() { _reset(); }

    intbv_base(uint32_t val) 
    {
      _reset();
      _w[_Nw-1] = val;
    }
    
    intbv_base( uint64_t val)
    {
      if (_Nw < 2) {
        throw std::runtime_error("intbv_base::intbv_base(uint64_t) expected _Nw>=2");                
      }
            
      _reset();
      _w[_Nw-1] = val;
      _w[_Nw-2] = val >>  _INTBV_BITS_PER_WORD;
    }
    
    intbv_base( int32_t val)
    {
      _reset();
      _w[_Nw-1] = val;
    }

    intbv_base( int64_t val)
    {
      if (_Nw < 2) {
        throw std::runtime_error("intbv_base::intbv_base(uint64_t) expected _Nw>=2");                
      }
            
      _reset();
      _w[_Nw-1] = val;
      _w[_Nw-2] = val >>  _INTBV_BITS_PER_WORD;
    }
        
    intbv_base( const std::string& s) {}
    intbv_base( const std::string& s, int format) {}
    // ctor that takes a templated initialization functor
    template <typename R> 
    explicit intbv_base (unsigned, const R &r)
    {
      _reset();
      for (int i = _Nw-1; i >=0; i--)
        { _w[i] = r();  }
    }

    intbv_base<_Nw> operator= (const intbv_base<_Nw> &x)
    {
      for (int i = 0; i != _Nw; ++i) {
        _w[i] = x._w[i];
      }
      return *this;
    }

    template <size_t _Nw1>
    void _construct_from_smaller_sized (const intbv_base<_Nw1>& x)
    {
      size_t diff = _Nw-_Nw1;
      for (size_t i = 0; i != diff; ++i) {
        _w[i] = 0;
      }
      for (int i = _Nw1-1; i >=0; i--)
        _w[i+diff] =  x._getword(i); 
    }
    
    void _construct_from_smaller_sized (const intbv_base<0>& x) {}
    void _construct_from_smaller_sized (const intbv_base<1>& x);
    void _construct_from_larger_sized  (const intbv_base<0>& x) {assert(0);}
    void _construct_from_larger_sized  (const intbv_base<1>& x) {assert(0);}
    
    template <size_t _Nw1>
    void _construct_from_larger_sized (const intbv_base<_Nw1>& x)
    {
      size_t diff = _Nw1-_Nw;
      for (int i = _Nw-1; i >=0; i--) {   
        _w[i] = x._getword(i+diff);
      }
    }

    static size_t _whichbit(size_t nb,size_t pos )
    {  return  ((nb - 1 - pos) % _INTBV_BITS_PER_WORD); }
    
    static uint32_t _maskbit(size_t nb,size_t pos )
    { 
      uint32_t wb = _whichbit(nb,pos);
      return (static_cast<uint32_t>(1)) << wb; 
    }

    static uint32_t _maskwordbit(size_t nb,size_t pos )
    { return _maskbit(nb,pos); }

    static size_t _whichword(size_t nb,size_t pos )
    { 
      size_t leading_bits = nb % _INTBV_BITS_PER_WORD;
            
      if ((leading_bits) && (pos <= leading_bits - 1))
        return 0;
      else if (leading_bits)
        return ((pos - leading_bits)/ _INTBV_BITS_PER_WORD + 1);    
      else 
        return (pos / _INTBV_BITS_PER_WORD); 
    }
    
    static size_t _whichbyte(size_t nb,size_t pos )
    { return ((pos % _INTBV_BITS_PER_WORD) / CHAR_BIT); }

    uint32_t _getword(size_t i) const
    { return _w[i]; }
      
    uint32_t _getword(size_t nb,size_t pos) const
    { return _w[_whichword(nb,pos)]; }

    void _setword(size_t nb,size_t pos,uint32_t w)
    { _w[_whichword(nb,pos)] = w; }

    void _subtract(const intbv_base<_Nw>& x)
    {  
      subtractWords(_Nw,_w,_w,x._w);
    }
    
    void _add (const intbv_base<_Nw>& x)
    {
      addWords(_Nw,_w,_w,x._w);
    }

    // TBD: Check if we really need all this "negate()"s
    void _multiplySigned (unsigned x)
    {
      bool negative_y = _w[0];
      bool negative_x = false;
      intbv_base<_Nw> tmp_x (x);
      if (negative_y) {
        // convert to unsigned
        _negate();
      }
      if (negative_x) {
        // convert to unsigned
        tmp_x._negate();
      }
      // perform an unsigned multiply
      _multiply(tmp_x);
      // convert back to signed
      if (negative_x != negative_y) {
        _negate();
      }
    }
  
    void _multiplySigned (const intbv_base<_Nw>& x)
    {
      bool negative_y = _w[0];
      bool negative_x = x._w[0];
      intbv_base<_Nw> tmp_x (x);
      if (negative_y) {
        // convert to unsigned
        _negate();
      }
      if (negative_x) {
        // convert to unsigned
        tmp_x._negate();
      }
      // perform an unsigned multiply
      _multiply(tmp_x);
      // convert back to signed
      if (negative_x != negative_y) {
        _negate();
      }
     
    }

    void _divideSigned (const intbv_base<_Nw>& x)
    {
     
      // If "_w"'s type has been changed change here for "msb" too. 
      assert (_INTBV_BITS_PER_WORD == 32);   
      static uint32_t msb = 1 << (_INTBV_BITS_PER_WORD - 1);  
      
      bool negative_y =   _w[0] & msb;
      bool negative_x = x._w[0] & msb;
      
      intbv_base<_Nw> tmp_x (x);
      
      if (negative_y) {
        // convert to unsigned
        _negate();
      }
      if (negative_x) {
        // convert to unsigned
        tmp_x._negate();
      }
      // perform an unsigned divide
      _divide(tmp_x);
      // convert back to signed
      if (negative_x != negative_y) {
        _negate();
      }
    }

    void _multiply (const intbv_base<_Nw>& x)
    {
            
      intbv_base<_Nw> zero (0);
      intbv_base<_Nw> one (1);
      if ((_is_equal(zero)) || (x._is_equal(zero)) ) {
        // if either operand is zero, result is zero
        *this = zero;
        return;
      }
      else if ( x._is_equal(one) ) { 
        // this*1 = this, do nothing
        return; 
      }
      else if ( _is_equal(one) ) {
        // 1*x = x
        *this = x;
        return;
      }
      intbv_base<_Nw> result (0);
      multiplyWords( _Nw, result._w, _w, x._w);
      *this = result;
      return;   
    }
    
    void _divide (const intbv_base<_Nw>& x)
    {    
      intbv_base<_Nw> zero (0);
      intbv_base<_Nw> one (1);
      if (_is_equal(zero)) {
        // 0/x = 0
        *this = zero;
        return;
      }
      	 else if ( x._is_equal(zero) ) {
           throw std::runtime_error ("ERROR8: attempt to divide by zero.");
       }
            
      else if ( x._is_equal(one) ) { 
        // this/1 = this, do nothing
        return;
      }
            
      intbv_base<_Nw> q (0);
      intbv_base<_Nw> r (0);
      q = divideInternal( *this,x,r);
      *this = q;
      return;   
    }
    
    void _and(const intbv_base<_Nw>& x)
    {
      for (int i = _Nw-1; i >=0; i--) 
        _w[i] &= x._w[i];
    }
    
    void _or(const intbv_base<_Nw>& x)
    {        
      for (int i = _Nw-1; i >=0; i--)
        _w[i] |= x._w[i];
    }
    
    void _xor(const intbv_base<_Nw>& x)
    {
      for (int i = _Nw-1; i >=0; i--)
        _w[i] ^= x._w[i];
    }
    
    void _left_rotate (size_t shift,size_t nb,intbv_base<_Nw> &res) const;
    
    void _right_rotate(size_t shift,size_t nb,intbv_base<_Nw> &res) const;

    void _left_shift(size_t shift);
  
    void _right_shift(size_t shift);

    void _swap_bytes(intbv_base<_Nw> &res) const 
    {
      for(size_t i=0; i < _Nw; ++i) 
        res._w[i] = swap_uint32(_w[_Nw-i-1]);
    } 

    void _swap_words(intbv_base<_Nw> &res,unsigned adjust) const 
    {
      for(size_t i=0; i < _Nw; ++i) {
        res._w[i] = _w[_Nw-i-1];
      }
      // The new bottom word needs to be shifted to the left in order to
      // accomodate the right shift which will right-justify the result.
      res._w[_Nw-1] <<= adjust;
    }

    void _invert()
    {
      for (int i = _Nw-1; i >=0; i--)
        _w[i] = ~_w[i];
    }
    
    void _negate()
    {
      _invert();
      intbv_base<_Nw> one (1);
      _add(one);
    }
    
    void _getField(size_t nb,size_t start, size_t stop, intbv_base<_Nw> &res) const
    {
      start = start % nb;
      stop = stop % nb;
      assert (stop >= start);
      assert (stop >= start);
      assert (start >= 0);
      res._left_shift(_Nw * _INTBV_BITS_PER_WORD - nb + start);
      res._right_shift((_Nw * _INTBV_BITS_PER_WORD - nb + start) + (nb -1 - stop));
    }
    
    void _setField(size_t nb,size_t start, size_t stop, const intbv_base<_Nw>& x)
    {
      // fix me: inefficient, figure out which words are affected and only
      //   shift those about.
      start = start % nb;
      stop = stop % nb;
      assert (stop >= start);
      assert (stop >= 0);
      assert (start >= 0);

      intbv_base<_Nw> mask (0);
      intbv_base<_Nw> x_copy (x);
      mask._set();
      mask._left_shift(_INTBV_BITS_PER_WORD * _Nw - nb + start);
      mask._right_shift( (_INTBV_BITS_PER_WORD * _Nw - nb + start) + (nb -1 - stop));
      mask._left_shift(nb -1 - stop);
      intbv_base<_Nw> mask_inv (mask);
      mask_inv._invert();
            
      // zero out field in original value
      _and(mask_inv);
            
      // or in the field
      x_copy._left_shift(nb-1-stop);
      x_copy._and(mask);
      _or(x_copy);
    }
    
    std::string _str (int nb, int format=0) const { return str(format,nb,_w); }
    std::ostream& _printToOS( std::ostream& os,int nb) const
    { return printToOS (os, nb,_w); }
    
    void _readNumber (const std::string& s, int nb, int format=0) {
      readNumber(s,format,nb,_w);
    }
    
    void _readNumber (std::istream& is,int nb) { 
      readNumber(is,nb,_w);
    }

    void _writeNumber (std::ostream& os,int nb) const { 
      writeNumber(os,nb,_w);
    }

    void _dummy () const {
      intbv_dummy(_w);
    }

# ifndef __NO_RNUMBER__
    rnumber::RNumber _createRNumber (int nb) const {
      return createRNumber(nb,_w);
    }

    void _constructFromRNumber(const rnumber::RNumber &x) {
      constructFromRNumber(_w,_Nw,x);
    }
# endif
    
    void _set()
    {
      for (int i = _Nw-1; i >=0; i--)
        _w[i] = ~static_cast<uint32_t>(0);
    }
    
    void _reset() { memset(_w, 0, _Nw * sizeof(uint32_t)); }
    
    bool _is_equal(const intbv_base<_Nw>& x) const
    {
      for (int i = _Nw-1; i >=0; i--)
        {
          if (_w[i] != x._w[i]){
            return false;
          }
        }
      return true;
    }
    
    bool _less_than(const intbv_base<_Nw>& x) const
    {
      for (size_t i = 0; i < _Nw; i++) {
        if  (_w[i] < x._w[i]) 
          return true;
        else if  (_w[i] > x._w[i]) 
          return false;
      }
      return false;
    }
    
    bool _less_than_equal(const intbv_base<_Nw>& x) const
    {
      unsigned eq = 0;
            
      for (size_t i = 0; i < _Nw; i++) {       
        if  (_w[i] < x._w[i]) 
          return true;
        else if  (_w[i] > x._w[i]) 
          return false;
        else if  (_w[i] == x._w[i]) 
          eq++;
      }
      if (eq == _Nw) return true;
            
      return false;
    }
    
    bool _signed_less_than(const intbv_base<_Nw>& x,size_t nb) const
    {
      bool sign1 = _msb(nb);
      bool sign2 = x._msb(nb);
            
      if ( sign1 == 1 && sign2 == 0 ){
        return true;
      }
      if ( sign1 == 0 && sign2 == 1 ) {
        return false;
      }
      return _less_than(x);
    }
    
    bool _signed_less_than_equal(const intbv_base<_Nw>& x,size_t nb) const
    {
      return _signed_less_than(x,nb) || _is_equal(x);
    }
    
    bool _greater_than(const intbv_base<_Nw>& x) const
    {
      for (size_t i = 0; i < _Nw; i++) {
                
        if  (_w[i] > x._w[i]) 
          return true;
        else if  (_w[i] < x._w[i]) 
          return false;
      }
      return false;
    }
    
    bool _greater_than_equal(const intbv_base<_Nw>& x) const
    {
      unsigned eq = 0;
      for (size_t i = 0; i < _Nw; i++) {
        if  (_w[i] > x._w[i]) 
          return true;
        else if  (_w[i] < x._w[i]) 
          return false;
        else if  (_w[i] == x._w[i]) 
          eq++;
      }
      if (eq == _Nw) return true;
            
      return false;
    }
    
    bool _signed_greater_than(const intbv_base<_Nw>& x,size_t nb) const
    { 
      bool sign1 = _msb(nb);
      bool sign2 = x._msb(nb);
      if ( sign1 == 1 && sign2 == 0 ) {
        return false;
      }
      if ( sign1 == 0 && sign2 == 1 ) {
        return true;
      }
      return _greater_than(x);
    }
    
    bool _signed_greater_than_equal(const intbv_base<_Nw>& x,size_t nb) const
    {
      return _signed_greater_than(x,nb) || _is_equal(x);            
    }
    
    bool _msb(size_t nb) const
    {
      return _w[0] & _maskwordbit(nb,0);
    }
    
    bool _is_any() const
    {
      for (int i = _Nw-1; i >=0; i--)
        {
          if (_w[i] != static_cast<uint32_t>(0))
            return true;
        }
      return false;
    }
    
    size_t _count_ones() const
    {
      size_t result = 0;
      for (int i = _Nw-1; i >=0; i--)
        result += count_ones(_w[i]);
      return result;
    }

   size_t _count_leading_zeros(size_t nb) const
    {
      size_t result = 0;
      for (int i = 0; i != _Nw; ++i) {
        result += count_leading_zeros(_w[i]);
        if (_w[i]) break;
      }

      return result - ( (_Nw*sizeof(uint32_t)*8) - nb );
    }
    
    uint32_t _to_ulong() const;
    uint64_t _to_ulonglong() const;
    
    // find first "on" bit
    size_t _find_first(size_t __not_found) const;                                                                                                                        
    // find the next "on" bit that follows "prev"
    size_t _find_next(size_t prev, size_t __not_found) const;

    template <size_t nb>
    void _sanitize()
    { sanitize<nb%_INTBV_BITS_PER_WORD>::_do_sanitize(_w[0]); }
  };

  template<size_t _Nw>
  void intbv_base<_Nw>::_left_rotate(size_t shift, size_t nb,intbv_base<_Nw> &res) const
  {
    intbv_base<_Nw> mask (0);
    mask._set();
    mask._left_shift(nb-shift);
    intbv_base<_Nw> tmp (*this);
    tmp._and(mask);
    res._left_shift(shift);
    tmp._right_shift(nb-shift);
    res._or(tmp);
  }

  template<size_t _Nw>
  void intbv_base<_Nw>::_right_rotate(size_t shift,size_t nb,intbv_base<_Nw> &res) const
  {
    intbv_base<_Nw> mask (0);
    mask._set();
    mask._right_shift(nb-shift);
    intbv_base<_Nw> tmp (*this);
    tmp._and(mask);
    res._right_shift(shift);
    tmp._left_shift(nb-shift);
    res._or (tmp);
  }


  template<size_t _Nw>
  void intbv_base<_Nw>::_right_shift(size_t shift)
  {
    if (EXPECT(shift != 0, true))
      {
        const size_t wshift = shift / _INTBV_BITS_PER_WORD;
        const size_t offset = shift % _INTBV_BITS_PER_WORD;
        const size_t limit = _Nw - wshift - 1;
        
        if (offset == 0)
          for (int n = limit; n >= 0; --n)
            _w[n+wshift] = _w[n];
        else
          {
            int n = limit;
            const size_t sub_offset = _INTBV_BITS_PER_WORD - offset;
            for (; n > 0; --n)
              _w[n+wshift] = (_w[n] >> offset) |
                (_w[n - 1] << sub_offset);
            _w[n+wshift] = _w[0] >> offset;
          }
        std::fill(_w , _w + wshift ,static_cast<uint32_t>(0));
      }
  }

  template<size_t _Nw>
  void intbv_base<_Nw>::_left_shift(size_t shift)
  {
    if (EXPECT(shift != 0, true))
      {
        const size_t wshift = shift / _INTBV_BITS_PER_WORD;
        const size_t offset = shift % _INTBV_BITS_PER_WORD;
        const size_t limit = _Nw - wshift - 1;
        
        if (offset == 0)
          for (size_t n = 0; n <= limit; ++n)
            _w[n] = _w[n + wshift];
        else
          {
            const size_t sub_offset = _INTBV_BITS_PER_WORD - offset;
            for (size_t n = 0; n < limit; ++n)
              _w[n] = (_w[n + wshift] << offset) |
                (_w[n + wshift + 1] >> sub_offset);
            _w[limit] = _w[limit+wshift] << offset;
          }
        std::fill(_w + limit + 1, _w + _Nw, static_cast<uint32_t>(0));
      }
  }

  template<size_t _Nw>
  uint32_t intbv_base<_Nw>::_to_ulong() const
  {
    return _w[_Nw-1];
  }

  // Since we have a specialization on single words, we should never
  // have a case where the generic intbv_base is just a single word
  // in size, so it should be okay to always return the last two words.
  template<size_t _Nw>
  uint64_t intbv_base<_Nw>::_to_ulonglong() const
  {
    uint64_t r = _w[_Nw-2];
    return (r << 32) | _w[_Nw-1];
  }

  template<size_t _Nw>
  size_t intbv_base<_Nw>::_find_first(size_t __not_found) const
  {
    for (int i = _Nw-1; i >=0; i--)
      {
        uint32_t thisword = _w[i];
        if (thisword != static_cast<uint32_t>(0))
          return i * _INTBV_BITS_PER_WORD
            + count_trailing_zeros(thisword);
      }
    // not found, so return an indication of failure.
    return __not_found;
  }
  
  template<size_t _Nw>
  size_t intbv_base<_Nw>::_find_next(size_t prev, size_t __not_found) const
  {
    // make bound inclusive
    ++prev;
    
    // check out of bounds
    if (prev >= _Nw * _INTBV_BITS_PER_WORD)
      return __not_found;
    
    // search first word
    size_t i = _whichword(_Nw*8,prev);
    uint32_t thisword = _w[i];
    
    // mask off bits below bound
    thisword &= (~static_cast<uint32_t>(0)) << _whichbit(_Nw*8,prev);
    
    if (thisword != static_cast<uint32_t>(0))
      return i * _INTBV_BITS_PER_WORD
        + count_trailing_zeros(thisword);
    
    // check subsequent words
    i++;
    for ( ; i < _Nw; i++ )
      {
        thisword = _w[i];
        if (thisword != static_cast<uint32_t>(0))
          return i * _INTBV_BITS_PER_WORD
            + count_trailing_zeros(thisword);
      }
    // not found, so return an indication of failure.
    return __not_found;
  } // end _find_next




  // ======== Base class, specialization for no storage (zero-length %intbv).
  template<>
  struct intbv_base<0>
  {
    // this class is just a stub, it throws a runtime error if you try to instantiate it.
    intbv_base() {}
    intbv_base(uint32_t) {}
    intbv_base(int32_t) {}
    intbv_base(const std::string&) {}
    intbv_base(const std::string&, int) {}
    template <size_t _Nw1>
    void _construct_from_smaller_sized (const intbv_base<_Nw1>& x) {}
    template <size_t _Nw1>
    void _construct_from_larger_sized (const intbv_base<_Nw1>& x) {}
    template <typename R> explicit intbv_base (unsigned,unsigned,const R &r){}
    static size_t _whichbit(size_t ) {  return  0;}
    static uint32_t _maskbit(size_t ) { return 0; }
    static uint32_t _maskwordbit(size_t ) { return 0; }
    static uint32_t _maskwordbit(size_t,size_t) { return 0; };
    static size_t _whichword(size_t ) { return 0;}
    static size_t _whichbyte(size_t ) { return 0; }
    uint32_t _getword(size_t) const {return 0;}
    uint32_t _getword(size_t,size_t) const { return 0; };
    void _multiply (const intbv_base<0>&){}
    void _multiplySigned (const intbv_base<0>&){}
    void _multiplySigned (unsigned){}
    void _divideSigned (unsigned){}
    void _divide (const intbv_base<0>&){}
    void _divideSigned (const intbv_base<0>&){}
    void _subtract(const intbv_base<0>&) {}
    void _add(const intbv_base<0>&) {}
    void _and(const intbv_base<0>&) {}
    void _or(const intbv_base<0>&)  {}
    void _xor(const intbv_base<0>&) {}
    void _left_rotate(size_t,size_t,intbv_base<0> &) const {}
    void _right_rotate(size_t,size_t,intbv_base<0> &) const {}
    void _left_shift(size_t) {}
    void _right_shift(size_t) {}
    void _swap_bytes(intbv_base<0> &)  const {}
    void _swap_words(intbv_base<0> &,unsigned)  const {}
    void _invert() {}
    void _negate() {}
    void _setField(size_t,size_t, size_t, const intbv_base<0>&) {}
    void _getField(size_t,size_t, size_t, const intbv_base<0>&) const {}
    void _set() {}
    void _reset() {}
    bool _less_than(const intbv_base<0>&) const { return true; }
    bool _greater_than(const intbv_base<0>&) const { return true; }
    bool _less_than_equal(const intbv_base<0>&) const { return true; }
    bool _greater_than_equal(const intbv_base<0>&) const { return true; }
    bool _signed_less_than(const intbv_base<0>&,size_t) const { return true; }
    bool _signed_greater_than(const intbv_base<0>&,size_t) const { return true; }
    bool _signed_less_than_equal(const intbv_base<0>&,size_t) const { return true; }
    bool _signed_greater_than_equal(const intbv_base<0>&,size_t) const { return true; }
    bool _is_equal(const intbv_base<0>&) const { return true; }
    bool _msb(size_t) const { return false;}
    bool _is_any() const { return false; }
    size_t _count_ones() const { return 0; }
    size_t _count_leading_zeros(size_t) const { return 0; }
    uint32_t _to_ulong() const { return 0; }
    uint64_t _to_ulonglong() const { return 0; }
    size_t _find_first(size_t) const { return 0; }
    size_t _find_next(size_t) const { return 0; }
    std::string _str (int, int = 0 ) const { std::string s; return s;}
    std::ostream& _printToOS( std::ostream&os, int) const {return os;}
    void _readNumber(std::istream&,int,int = 0){}
    void _writeNumber(std::ostream&,int) const{ }
    void _dummy () const { }
# ifndef __NO_RNUMBER__
    rnumber::RNumber _createRNumber(int) const { assert(0); return 0; }
    void _constructFromRNumber(const rnumber::RNumber &x) { }
# endif
    template <size_t nb>
    void _sanitize() {}
  };

  // =========== Base class, specialization for a single word.
  template<>
  struct intbv_base<1>
  {
    uint32_t _w;
    
    intbv_base() : _w(0) {}
    intbv_base(uint32_t val) : _w(val) {}
    intbv_base(uint64_t val) : _w(val) {}
    intbv_base(int32_t val) : _w(val) {}
    intbv_base(int64_t val) : _w(val) {}
    intbv_base( const std::string&)  {}
    intbv_base( const std::string&, int) {}
    
    // ctor that takes a templated initialization functor
    template <typename R> 
    explicit intbv_base (unsigned , const R &r){ _w = r(); }
    
    // assignment from integer
    intbv_base<1>& operator=(int val) { _w = val; return *this;}

    // assignment from same type- necessary so that LLVM can do proper
    // type-based alias analysis and know that this is just an integer
    // assignment.
    intbv_base<1>& operator=(const intbv_base<1> &x) { _w = x._w; return *this;}

    void _construct_from_smaller_sized (const intbv_base<0>&) {}
    // construct ourself where we are of size 1:32 _bits_ from a smaller of size 1:32 _bits_
    void _construct_from_smaller_sized (intbv_base<1> x) {_w = x._w;}
    // construct ourself where we are of size 1:32 _bits_ from a larger of size 1:32 _bits_
    void _construct_from_larger_sized (intbv_base<1> x) { _w = x._w;}
    void _construct_from_larger_sized (const intbv_base<0>& ) {assert(0);}
    // general cases...
    template <size_t _Nw1>
    void _construct_from_larger_sized (const intbv_base<_Nw1>& x) { _w = x._getword(_Nw1-1);}  


    template <size_t _Nw1>
    void _construct_from_smaller_sized (const intbv_base<_Nw1>& ) {assert(0);}
    
    static size_t _whichbit(size_t nb,size_t pos )
    {  return  ((nb - 1 - pos) % _INTBV_BITS_PER_WORD); }
    
    static uint32_t _maskbit(size_t nb,size_t pos )
    { return (static_cast<uint32_t>(1)) <<_whichbit(nb,pos); }

    static uint32_t _maskwordbit(size_t nb,size_t pos )
    { return (static_cast<uint32_t>(1)) <<_whichbit(nb,pos); }

    static size_t _whichword(size_t,size_t pos )
    { return pos/_INTBV_BITS_PER_WORD; }
    
    static size_t _whichbyte(size_t, size_t pos )
    { return ((pos % _INTBV_BITS_PER_WORD) / CHAR_BIT); }
        
    void _setword(size_t,size_t,uint32_t w)  { _w = w; }

    uint32_t _getword(size_t) { return _w; }
    
    uint32_t _getword(size_t,size_t) const { return _w; }
    
    void _multiply (const intbv_base<1>& x){_w *= x._w;}
    void _multiplySigned (unsigned x){_w = (int32_t)_w * x;}
    void _multiplySigned (const intbv_base<1>& x){_w = (int32_t)_w * (int32_t)x._w;}
    void _divideSigned (const intbv_base<1>& x){
	 if (! x._w) {
          // throw std::runtime_error ("ERROR1: attempt to divide by zero.");
         } else
      _w = (int32_t)_w / (int32_t)x._w;

    }
    void _divideSigned (unsigned x){
        if (! x) {
          throw std::runtime_error ("ERROR2: attempt to divide by zero.");
        }
      _w = (int32_t)_w / (int32_t)x;
    }
    void _divide (const intbv_base<1>& x){
        if (! x._w) {
          throw std::runtime_error ("ERROR3: attempt to divide by zero.");
        } 
      _w = _w / x._w;
    }
    
    void _subtract(const intbv_base<1>& x) { _w -= x._w; }
    
    void _add(const intbv_base<1>& x) { _w += x._w; }
    
    void _and(const intbv_base<1>& x) { _w &= x._w; }
    
    void _or(const intbv_base<1>& x)  { _w |= x._w; }
    
    void _xor(const intbv_base<1>& x) { _w ^= x._w; }
    
    void _left_rotate(size_t shift,size_t nb,intbv_base<1> &res) const { 
      uint32_t mask = ((uint32_t)-1) << (nb - shift);
      uint32_t tmp = _w & mask;
      res._w =  (res._w << shift) | (tmp >> (nb - shift));
    }
    
    void _right_rotate(size_t shift,size_t nb,intbv_base<1> &res) const {
      uint32_t mask = ((uint32_t)-1) >> (nb - shift);
      uint32_t tmp = _w & mask;
      res._w =  (res._w >> shift) | (tmp << (nb - shift));   
    }
    
    void _left_shift(size_t shift) { _w <<= shift; }
    
    void _right_shift(size_t shift) { _w >>= shift; }
    
    void _swap_bytes(intbv_base<1> &x) const 
    {
      x._w =swap_uint32(_w);
    }

    void _swap_words(intbv_base<1> &x,unsigned) const 
    {
      x._w = _w;
    }

    void _invert() { _w = ~_w; }

    void _negate() { _w = ~_w; _w += 1; }
    
    void _getField(size_t nb,size_t start, size_t stop, intbv_base<1> &res) const
    {
      assert (stop >= start);
      res._w = _w;
      res._w  <<= _INTBV_BITS_PER_WORD - nb + start;
      res._w >>= (_INTBV_BITS_PER_WORD - nb + start) + (nb -1 - stop);
    }
    
    void _setField(size_t nb,size_t start, size_t stop, const intbv_base<1>& x) ATTRIBUTE_INLINE
    { 
      assert (stop >= start);
      intbv_base<1> x_copy (x);
      uint32_t m = 0xffffffff;
      m <<= _INTBV_BITS_PER_WORD - nb + start;
      m >>= (_INTBV_BITS_PER_WORD - nb + start) + (nb -1 - stop);
      m <<= nb -1 - stop;
            
      // zero out field in original value
      intbv_base<1> mask (m);
      intbv_base<1> mask_inv (~m);
      _and(mask_inv);
      // or in the field
      x_copy._left_shift(nb-1-stop);
      x_copy._and(mask);
      _or(x_copy);
    }                  
    
    void _set() { _w = ~static_cast<uint32_t>(0); }
    
    void _reset() { _w = 0; }
    bool _msb(size_t nb) const { return _w & _maskbit(nb,0); }
    bool _less_than(const intbv_base<1>& x) const { return _w < x._w; }
    bool _greater_than(const intbv_base<1>& x) const { return _w > x._w; }
    bool _less_than_equal(const intbv_base<1>& x) const { return _w <= x._w; }
    bool _greater_than_equal(const intbv_base<1>& x) const {return _w >= x._w;  }
    
    bool _signed_less_than(const intbv_base<1>& x,size_t nb) const { 
      bool sign1 = _msb(nb);
      bool sign2 = x._msb(nb);
        
      if ( sign1 == 1 && sign2 == 0 ){
        return true;
      }
      if ( sign1 == 0 && sign2 == 1 ) {
        return false;
      }
      return _less_than(x);
    }
    
    bool _signed_greater_than(const intbv_base<1>& x,size_t nb) const
    { 
      bool sign1 = _msb(nb);
      bool sign2 = x._msb(nb);
      if ( sign1 == 1 && sign2 == 0 ) {
        return false;
      }
      if ( sign1 == 0 && sign2 == 1 ) {
        return true;
      }
      return _greater_than(x);
    }
    
    bool _signed_greater_than_equal(const intbv_base<1>& x,size_t nb) const {
      return _signed_greater_than(x,nb) || _is_equal(x); 
    }
    bool _signed_less_than_equal(const intbv_base<1>& x,size_t nb) const {
      return _signed_less_than(x,nb) || _is_equal(x); 
    }
    bool _is_equal(const intbv_base<1>& x) const { return _w == x._w; }
    
    bool _is_any() const { return _w != 0; }
    
    size_t _count_ones() const { return count_ones(_w); }

    size_t _count_leading_zeros(size_t nb) const { 
      size_t result = (_w == 0) ? (sizeof(_w)*8) : count_leading_zeros(_w);
      return result - ( sizeof(_w)*8 - nb ); 
    }
    
    uint32_t _to_ulong() const { return _w; }
    uint64_t _to_ulonglong() const { return _w; }
    
    size_t _find_first(size_t not_found) const
    {
      if (_w != 0)
        return count_trailing_zeros(_w);
      else
        return not_found;
    }    
    
    // find the next "on" bit that follows "prev"
    size_t _find_next(size_t prev, size_t not_found) const
    {
      ++prev;
      if (prev >= ((size_t) _INTBV_BITS_PER_WORD))
        return not_found;
            
      uint32_t x = _w >> prev;
      if (x != 0)
        return count_trailing_zeros(x) + prev;
      else
        return not_found;
    }
    
    std::string _str (int nb, int format=0) const { return str(format,nb,&_w); }
    
    std::ostream& _printToOS( std::ostream& os,int nb) const
    { return printToOS (os, nb,&_w); }
    
    void _readNumber (const std::string& s, int nb, int format=0) 
    { readNumber(s,format,nb,&_w);}
    void _readNumber(std::istream& is,int nb)
    { readNumber(is,nb,&_w);}
    void _writeNumber(std::ostream& os,int nb) const
    { writeNumber(os,nb,&_w);}
    void _dummy () const {
      intbv_dummy(&_w);
    }
# ifndef __NO_RNUMBER__
    rnumber::RNumber _createRNumber(int nb) const
    { return createRNumber(nb,&_w); }
    void _constructFromRNumber(const rnumber::RNumber &x)
    { constructFromRNumber(&_w,1,x); }
# endif
    template <size_t nb>
    void _sanitize()
    { sanitize<nb%_INTBV_BITS_PER_WORD>::_do_sanitize(_w); }
  };

  // =========== Base class, specialization for two words (64 bit).
  template<>
  struct intbv_base<2>
  {
    uint64_t _w;
  	
    intbv_base() : _w(0) {}
    // construct from 32 bit signed or unsigned
    intbv_base(uint32_t val) : _w(val) {}
    intbv_base(int32_t val) : _w(val) {}
    // construct from 64 bit signed or unsigned
    intbv_base(int64_t val) : _w(val) {}
    intbv_base(uint64_t val) : _w(val) {}

    intbv_base( const std::string&)  {}
    intbv_base( const std::string&, int) {}
  
    // ctor that takes a templated initialization functor
    template <typename R> 
    explicit intbv_base (unsigned , const R &r){ _w = r(); }
  
    // assignment from integer
    intbv_base<2>& operator=(int32_t val) { _w = val; return *this;}
    intbv_base<2>& operator=(int64_t val) { _w = val; return *this;}

    // assignment from same type- necessary so that LLVM can do proper
    // type-based alias analysis and know that this is just an integer
    // assignment.
    intbv_base<2>& operator=(const intbv_base<2> &x) { _w = x._w; return *this;}

    // construct ourself where we are of size 1:64 _bits_ from a smaller of size 1:64 _bits_
    void _construct_from_smaller_sized (const intbv_base<0>&) {}
    void _construct_from_smaller_sized (const intbv_base<1> x) {_w = x._w;}
    void _construct_from_smaller_sized (const intbv_base<2> x) {_w = x._w;}
    template <size_t _Nw1>
    void _construct_from_smaller_sized (const intbv_base<_Nw1> x) { assert(0);}

    // construct ourself where we are of size 1:64 _bits_ from a larger of size 1:364 _bits_
    void _construct_from_larger_sized (const intbv_base<0>&) {assert(0);}
    void _construct_from_larger_sized (const intbv_base<1>) { assert(0);}
    void _construct_from_larger_sized (intbv_base<2> x) { _w =  x._w;}
    // general case...
    template <size_t _Nw1>
    void _construct_from_larger_sized (const intbv_base<_Nw1>& x) {  
      _w =  x._w[_Nw1-2]; 
      _w = _w << _INTBV_BITS_PER_WORD | x._w[_Nw1-1];}

  
    static size_t _whichbit(size_t nb,size_t pos )
    {  return  ((nb - 1 - pos) % (_INTBV_BITS_PER_WORD*2)); }

    static size_t _whichwordbit(size_t nb,size_t pos )
    {  return  ((nb - 1 - pos) % (_INTBV_BITS_PER_WORD)); }
 
    static uint64_t _maskbit(size_t nb,size_t pos )
    { return (static_cast<uint64_t>(1)) <<_whichbit(nb,pos); }

    static uint32_t _maskwordbit(size_t nb,size_t pos )
    { 
      uint32_t wb = _whichwordbit(nb,pos);
      uint32_t mb = (static_cast<uint32_t>(1)) << wb;
      return mb; 
    }
 
    static size_t _whichword(size_t,size_t pos )
    { return pos/_INTBV_BITS_PER_WORD; }
  
    static size_t _whichbyte(size_t, size_t pos )
    { return ((pos % _INTBV_BITS_PER_WORD) / CHAR_BIT); }
  

    uint32_t _getword(size_t i) const {
      if (i == 0) 
        return( _w >> 32);
      return (_w);
    }

    uint32_t _getword(size_t nb,size_t pos) const { 
      if (pos < (nb - _INTBV_BITS_PER_WORD))
        return _w >> 32;
      return _w;
    }


    void _setword(size_t nb,size_t pos,uint32_t w) { 
      uint64_t t = w;
      uint64_t m = (uint64_t)-1;
      if (pos < (nb - _INTBV_BITS_PER_WORD)) {
        t <<= _INTBV_BITS_PER_WORD;
        m >>= _INTBV_BITS_PER_WORD;
      }else {
        m <<= _INTBV_BITS_PER_WORD;
      }
      _w = (_w & m) | t;
    }

    void _multiply (const intbv_base<1>& x){_w *= x._w;}
    void _multiply (const intbv_base<2>& x){_w *= x._w;}

    void _multiplySigned (unsigned x){ _w = (int64_t)_w * x;}
    void _multiplySigned (const intbv_base<1>& x){ _w = (int64_t)_w * (int32_t)x._w;}
    void _multiplySigned (intbv_base<2> x){ 
      int64_t t1 = _w;
      int64_t t2 = x._w;
      _w = t1 * t2;
    }
    void _divideSigned (unsigned x){
      if (! x) {
        throw std::runtime_error ("ERROR4: attempt to divide by zero.");
      }
      _w = (int64_t)_w / (int32_t)x;
    }
    void _divideSigned (const intbv_base<1>& x){ 
      if (! x._w) {
        throw std::runtime_error ("ERROR5: attempt to divide by zero.");
      }
      _w = (int64_t)_w / (int32_t)x._w;
    }
    void _divideSigned (const intbv_base<2>& x){ 
      if (! x._w) {
       throw std::runtime_error ("ERROR6: attempt to divide by zero.");
      }
      _w = (int64_t)_w / (int64_t)x._w;
    }
    void _divide (const intbv_base<1>& x){
      if (! x._w) {
        throw std::runtime_error ("ERROR: attempt to divide by zero.");
      }
      _w = _w / x._w;
    }

    void _divide (const intbv_base<2>& x){
      if (! x._w) {
       throw std::runtime_error ("ERROR7: attempt to divide by zero.");
      }  
      _w = _w / x._w;
    } 
    void _subtract(const intbv_base<1>& x) { _w -= x._w; }
    void _subtract(const intbv_base<2>& x) { _w -= x._w; }   
    void _add(const intbv_base<1>& x) { _w += x._w; }
    void _add(const intbv_base<2>& x) { _w += x._w; }
    void _and(const intbv_base<1>& x) { _w &= x._w; }
    void _and(const intbv_base<2>& x) { _w &= x._w; } 
    void _or(const intbv_base<1>& x)  { _w |= x._w; }
    void _or(const intbv_base<2>& x)  { _w |= x._w; }  
    void _xor(const intbv_base<1>& x) { _w ^= x._w; }
    void _xor(const intbv_base<2>& x) { _w ^= x._w; }
    void _left_rotate(size_t shift,size_t nb,intbv_base<2> &res) const { 
      uint64_t mask = ((uint64_t)-1) << (nb - shift);
      uint64_t tmp = _w & mask;
      res._w =  (res._w << shift) | (tmp >> (nb - shift));
    }
    void _right_rotate(size_t shift,size_t nb,intbv_base<2> &res) const {
      uint64_t mask = ((uint64_t)-1) >> (nb - shift);
      uint64_t tmp = _w & mask;
      res._w =  (res._w >> shift) | (tmp << (nb - shift));   
    }
  
    void _left_shift(size_t shift) { _w <<= shift; }
  
    void _right_shift(size_t shift) { _w >>= shift; }

    void _swap_bytes(intbv_base<2> &x) const 
    {
      x._w = swap_uint64(_w);                                                    
    }

    void _swap_words(intbv_base<2> &x,unsigned adjust) const 
    {
      x._w = ((_w << adjust) >> 32) | (_w << 32);
    }

    void _invert() { _w = ~_w; }

    void _negate() { _w = ~_w; _w += 1; }
  
    void _getField(size_t nb,size_t start, size_t stop, intbv_base<2> &res) const
    {
      start = start % nb;
      stop = stop % nb;
      assert (stop >= start);
      res._w = _w;
      res._w  <<= _INTBV_BITS_PER_WORD * 2 - nb + start;
      res._w >>= (_INTBV_BITS_PER_WORD * 2 - nb + start) + (nb -1 - stop);
    }
  
    union Words {
      uint64_t d;
      struct {
        uint32_t h;
        uint32_t l;
      } w;
    };

    void _setField(size_t nb,size_t start, size_t stop, const intbv_base<2>& x) ATTRIBUTE_INLINE
    {    
      start = start % nb;
      stop = stop % nb;

#if 0
      if ( (nb-1-stop) == 32 && (stop-start+1) == 32 ) {
        Words t;
        t.d= _w;
#       ifdef WORDS_BIGENDIAN
        t.w.h = x._w;
#       else
        t.w.l = x._w;
#       endif
        _w = t.d;
        return;
      } else if ( (nb-1-stop) == 0 && (stop-start+1) == 32  ) {
        Words t;
        t.d = _w;
#       ifdef WORDS_BIGENDIAN
        t.w.l = x._w;
#       else
        t.w.h = x._w;
#       endif
        _w = t.d;
        return;
      }
#endif
      _setFieldInternal(nb,start,stop,x);
    }

    void _setFieldInternal(size_t nb,size_t start, size_t stop, const intbv_base<2>& x) 
    {
      assert (stop >= start);
  
      intbv_base<2> x_copy (x);
      uint64_t m = ~0;
      m <<= _INTBV_BITS_PER_WORD *2 - nb + start;
      m >>= (_INTBV_BITS_PER_WORD *2 - nb + start) + (nb -1 - stop);
      m <<= nb -1 - stop;
          
      // zero out field in original value
      intbv_base<2> mask (m);
      intbv_base<2> mask_inv (~m);
      _and(mask_inv);
      // or in the field
      x_copy._left_shift(nb-1-stop);
      x_copy._and(mask);
      _or(x_copy);
    }
  
    void _set() { _w = ~static_cast<uint64_t>(0); }
  
    void _reset() { _w = 0; }
    bool _msb(size_t nb) const { return _w & _maskbit(nb,0); }
    bool _less_than(const intbv_base<1>& x) const { return _w < x._w; }
    bool _less_than(const intbv_base<2>& x) const { return _w < x._w; }
    bool _greater_than(const intbv_base<1>& x) const { return _w > x._w; }
    bool _greater_than(const intbv_base<2>& x) const { return _w > x._w; }
    bool _less_than_equal(const intbv_base<1>& x) const { return _w <= x._w; }
    bool _less_than_equal(const intbv_base<2>& x) const { return _w <= x._w; }
    bool _greater_than_equal(const intbv_base<1>& x) const {return _w >= x._w;  }
    bool _greater_than_equal(const intbv_base<2>& x) const {return _w >= x._w;  }   
    bool _signed_less_than(const intbv_base<1>& x,size_t nb) const { 
      bool sign1 = _msb(nb);
      bool sign2 = x._msb(nb);
  
      if ( sign1 == 1 && sign2 == 0 ){
        return true;
      }
      if ( sign1 == 0 && sign2 == 1 ) {
        return false;
      }
      return _less_than(x);
    }
    bool _signed_less_than(const intbv_base<2>& x,size_t nb) const { 
      bool sign1 = _msb(nb);
      bool sign2 = x._msb(nb);
  
      if ( sign1 == 1 && sign2 == 0 ){
        return true;
      }
      if ( sign1 == 0 && sign2 == 1 ) {
        return false;
      }
      return _less_than(x);
    } 
    bool _signed_greater_than(const intbv_base<1>& x,size_t nb) const
    { 
      bool sign1 = _msb(nb);
      bool sign2 = x._msb(nb);
      if ( sign1 == 1 && sign2 == 0 ) {
        return false;
      }
      if ( sign1 == 0 && sign2 == 1 ) {
        return true;
      }
      return _greater_than(x);
    }
    bool _signed_greater_than(const intbv_base<2>& x,size_t nb) const
    { 
      bool sign1 = _msb(nb);
      bool sign2 = x._msb(nb);
      if ( sign1 == 1 && sign2 == 0 ) {
        return false;
      }
      if ( sign1 == 0 && sign2 == 1 ) {
        return true;
      }
      return _greater_than(x);
    }  
    bool _signed_greater_than_equal(const intbv_base<1>& x,size_t nb) const {
      return _signed_greater_than(x,nb) || _is_equal(x); 
    }
    bool _signed_greater_than_equal(const intbv_base<2>& x,size_t nb) const {
      return _signed_greater_than(x,nb) || _is_equal(x); 
    }
    bool _signed_less_than_equal(const intbv_base<1>& x,size_t nb) const {
      return _signed_less_than(x,nb) || _is_equal(x); 
    }
    bool _signed_less_than_equal(const intbv_base<2>& x,size_t nb) const {
      return _signed_less_than(x,nb) || _is_equal(x); 
    }
    bool _is_equal(const intbv_base<1>& x) const { return _w == x._w; }
    bool _is_equal(const intbv_base<2>& x) const { return _w == x._w; } 
    bool _is_any() const { return _w != 0; }
  
    size_t _count_ones() const { return count_ones(_w); }

    size_t _count_leading_zeros(size_t nb) const { 
      size_t result = (_w == 0) ? (sizeof(_w)*8) : count_leading_zeros(_w);
      return result - ( sizeof(_w)*8 - nb ); 
    }
  
    uint64_t _to_ulong() const { return _w; }
    uint64_t _to_ulonglong() const { return _w; }


    size_t _find_first(size_t not_found) const
    {
      if (_w != 0)
        return count_trailing_zeros(_w);
      else
        return not_found;
    }
  
    // find the next "on" bit that follows "prev"
    size_t _find_next(size_t prev, size_t not_found) const
    {
      ++prev;
      if (prev >= ((size_t) _INTBV_BITS_PER_WORD))
        return not_found;
          
      uint32_t x = _w >> prev;
      if (x != 0)
        return count_trailing_zeros(x) + prev;
      else
        return not_found;
    }
  
    std::string _str (int nb, int format=0) const 
    { 
      uint32_t tmp[2];
      tmp[1] = _w;
      tmp[0] = _w >> _INTBV_BITS_PER_WORD;
      return str (format,nb,tmp);
    }
  
    std::ostream& _printToOS( std::ostream& os,int nb) const
    { 
      uint32_t tmp[2];
      tmp[1] = _w;
      tmp[0] = _w >> _INTBV_BITS_PER_WORD;
      return printToOS (os,nb,tmp);
    }
  
    void _readNumber (const std::string& s, int nb, int format=0) 
    { 
      uint32_t tmp[2];
      tmp[0] = 0;
      tmp[1] = 0;
      readNumber(s,format,nb,tmp);
      _w = tmp[0];
      _w = _w << _INTBV_BITS_PER_WORD | tmp[1];
    }
    void _readNumber(std::istream& is,int nb)
    { 
      uint32_t tmp[2];
      tmp[0] = 0;
      tmp[1] = 0;
      readNumber(is,nb,tmp);
      _w = tmp[0];
      _w = _w << _INTBV_BITS_PER_WORD | tmp[1];
    }
    void _writeNumber(std::ostream& os,int nb) const
    { 
      uint32_t tmp[2];
      tmp[1] = _w;
      tmp[0] = _w >> _INTBV_BITS_PER_WORD;
      writeNumber(os,nb,tmp);
    }

    void _dummy () const {
      uint32_t tmp = 0;
      intbv_dummy((uint32_t*)&tmp);
    }

#ifndef __NO_RNUMBER__
    rnumber::RNumber _createRNumber(int nb) const
    {
      uint32_t tmp[2];
      tmp[1] = _w;
      tmp[0] = _w >> _INTBV_BITS_PER_WORD;
      return createRNumber(nb,tmp);
    }

    void _constructFromRNumber(const rnumber::RNumber &x)
    {
      uint32_t tmp[2];
      constructFromRNumber(tmp,2,x);
      _w = tmp[0];
      _w = _w << _INTBV_BITS_PER_WORD | tmp[1];
    }
#endif

    template <size_t nb>
    void _sanitize()
    { sanitize<nb%_INTBV_BITS_PER_WORD>::_do_sanitize(_w); }
  };

  // non-inline methods
  template <size_t _Nw>
  void intbv_base<_Nw>::_construct_from_smaller_sized (const intbv_base<1>& x) { 
    for (int i = 0; i != _Nw-1; ++i) {
      _w[i] = 0;
    }
    _w[_Nw-1] = x._w;
  }




  template <size_t _Nw>
  intbv_base<_Nw> divideInternal ( const intbv_base<_Nw>& n1, const intbv_base<_Nw>& n2, intbv_base<_Nw> &rem)
  {
    intbv_base<_Nw> res;
    divideWords(_Nw,res._w,n1._w,n2._w);
    return res;
  }

}

#endif /* _BASE_INTBV */
