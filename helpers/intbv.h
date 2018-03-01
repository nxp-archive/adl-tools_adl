//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Bit vector template class.  
// Much of this code is from bitset from the GNU ISO C++ Library. 
//

#ifndef _INTBV
#define _INTBV 

#include <boost/static_assert.hpp>

#include "intbv_base.h"

// IO enums.
namespace intbv_io {

  // Format values used for string manipulation.
  enum Format {
    iios    = 0x01,
    idec    = 0x02,  // Decimal radix.
    ihex    = 0x04,  // Hex radix.
    ibin    = 0x08,  // Binary radix.
    iprefix = 0x10,  // Show radix prefix, e.g. 0x for hex.
    iupper  = 0x20,  // Use upper-case letters for hex digits.
    ilast   = 0x20,
  };
}

#ifndef __NO_RNUMBER__
//  The intbv class will work with RNumber, a dynamic, arbitrary-precision
//  integer class.
namespace rnumber {
  class RNumber;
}
#endif

//
//  The intbv class represents a fixed-size sequence of bits.
//
//  The template argument, Nb, may be any non-negative number,
//  specifying the number of bits (e.g., "0", "12", "1024*1024").
//
//  In the general unoptimized case, storage is allocated in word-sized
//  blocks.  Let B be the number of bits in a word, then (Nb+(B-1))/B
//  words will be used for storage.  B - Nb%B bits are unused.  (They are
//  the high-order bits in the highest word.)  It is a class invariant
//  that those unused bits are always zero.
//
//  You will see comments regarding endianness in the code.  The divide
//   algorithm operates on a byte array but this is copied into a 
//   separate buffer so that it will work on a little-endian machine (x86,sun)
//   or a big-endian machine (ppc).
//   The representation of the value is big-endian in that 0 represents the
//   most significant part of the value.  ex:
//
//  intbv w[0]  most significant word
//        w[1]  next most significant word
//              ...
//        w[_Nw-1] the least significant word
//
//        0x12345678deadbeef would be stored as:
//            w[0] = 0x12345678
//            w[1] = 0xdeadbeef
//               msb = w[0] bit 0
//               lsb = w[_Nw-1] bit 31
//
//  Notes on switching from big to little endian representations:
//  If you wanted to change this representation to a little-endian representation,
//  you would need to change the loops from _Nw-1->0 to 0->_Nw-1.  As well as any
//  code that does bit manipulation: left_shift,right_shift,set/get field,set/get bit.
//  MultiplyInternal is commented with what you would need and divide is straight-
//  forward just copy to/from the bytearrays appropriately.  Most of this code was
//  originally written using a little-endian representation and it's checked in to
//  cvs so you could go back and refer to that as well.  
//
//  Most of the actual code isn't contained in intbv<> itself, but in the
//  base class intbv_base.  The base class works with whole words, not with
//  individual bits.  This allows us to specialize intbv_base for the
//  important special case where the intbv is only a single word.
//
//  Extra confusion can result due to the fact that the storage for
//  intbv_base is a regular array, and is indexed as such.  This is
//  carefully encapsulated.
//

// We have to deal with broken template issues here.  MSVC2003 doesn't handle
// this expression properly and issues an internal compiler error.  So, for that
// compiler, we default to only using the left operand.  This means that on that
// compiler, the behavior *will* be subtly different.  Watch out!  You can
// define BROKEN_TEMPLATES on an other platform in order to test this behavior.
// To be on the safe side, a compile-time assertion is used to guarantee that,
// for any binary operation, the left's width must be greater than the right's
// width.  This feature can be turned off by defining NO_ARG_CHECK.
#if ((_MSC_VER && (_MSC_VER < 1400)) || defined(BROKEN_TEMPLATES))
# define MAX(a,b) a
# define MIN(a,b) a
# define ArithArgCheck(a,b) BOOST_STATIC_ASSERT( a >= b )
# define CmpArgCheck(a,b) BOOST_STATIC_ASSERT( a >= b )
# ifdef _MSC_VER
#  pragma warning("Broken templates are enabled:  The result of a binary intbv operation will have the width of the left argument.")
# else
#  warning "Broken templates are enabled:  The result of a binary intbv operation will have the width of the left argument."
# endif
#else
# define MAX(a,b) (((a) > (b)) ? (a) : (b) )
# define MIN(a,b) (((a) < (b)) ? (a) : (b) )
# define ArithArgCheck(a,b)
# define CmpArgCheck(a,b)
#endif

#ifdef NO_ARITH_ARG_CHECK
# undef ArithArgCheck
# define ArithArgCheck(a,b)
#endif

#ifdef NO_CMP_ARG_CHECK
# undef CmpArgCheck
# define CmpArgCheck(a,b)
#endif

namespace adl {

  enum Unsafe_t { UnsafeSet = 0 };

  template<size_t _Nb>
  class intbv : private intbv_base<_INTBV_WORDS(_Nb)>
  {
  public:
    enum { Size = _Nb };
    template <size_t> friend class intbv;
    
  private:
    typedef intbv_base<_INTBV_WORDS(_Nb)> _Base;

    
  public:
    // constructors
    // All bits set to zero.
    intbv() {};
    // Initial bits bitwise-copied from a single word (others set to zero).
    intbv(int32_t val) : _Base(val) {       
      _sanitize(); 
      if (val < 0 && _Nb > 32) {
        signExtend(_Nb - 32);
      }
      
    };
      intbv(int64_t val) : _Base(val) { 
        _sanitize(); 
        if (val < 0 && _Nb > 64) {
          signExtend(_Nb - 64);
        }
      };
        intbv(uint32_t val) : _Base(val) {_sanitize(); };
          intbv(uint32_t val,Unsafe_t) : _Base(val) {};
          intbv(uint64_t val) : _Base(val) { _sanitize(); };
#ifdef IS_64BIT
            // This constructor is only necessary for 64-bit builds, due to
            // differences in how these integer types are defined.  This allows
            // the user to specify a ULL numerical constant and have it work w/o
            // compile errors.
            intbv(unsigned long long val) : _Base((uint64_t)val) { _sanitize(); };
            intbv(long long int val) : _Base((int64_t)val) { _sanitize(); };
#elif !defined(__CYGWIN__)
                // This constructor is only necessary for 32-bit builds, due to
                // differences in how these integer types are defined.  This allows
                // the user to specify an unsigned value and have it treated correctly.
                intbv(unsigned long val) : _Base((uint32_t)val) { _sanitize(); };
                intbv(long val) : _Base((int32_t)val) { _sanitize(); };
#endif
                  // Construct from string.  Where format is explicitly set, the input should
                  // contain a prefix, e.g. 0x...
                  intbv( const char* s);
                  intbv( const std::string& s);
                  intbv( const std::string& s, int format);
                  intbv( const std::istream &is );
# ifndef __NO_RNUMBER__
                  intbv( const rnumber::RNumber &x );
# endif
                  // copy (same size) purposefully excluded b/c for some reason g++ adds extra code
                  // if you have it.  Don't know why and it should be unnecessary- the default behavior
                  // is accurate.

                  // copy (diff size)
                  template <size_t _Nb2>
                    intbv (const intbv<_Nb2> &x);

                  // operator= (diff size)
                  template <size_t _Nb2>
                    intbv<_Nb> &operator= (const intbv <_Nb2> &x);

                  // ctor that takes a templated initialization functor
                  template <typename R> 
                    explicit intbv (unsigned x,const R &r) : _Base (x, r) { _sanitize();}

                    // These are here for compatibility w/the bits class.  Since we don't
                    // actually track undefined results, they're just stubbed out.

                    // Set the mask to register that a portion is undefined.
                    void setUndefined(size_t pos) {};
                    void setUndefined(size_t start,size_t stop) {};

                    // True if anything is undefined in the specified range.
                    bool isUndefined(size_t start,size_t stop) const { return false; };
                    // True if there is any undefined.
                    bool isUndefined() const { return false; };
        
  public:

                    // to set a field
                    intbv<_Nb> &set (size_t start, size_t stop, const intbv<_Nb>& x);
                    template <size_t _Nb2>
                      intbv<_Nb> &set (size_t start, size_t stop, const intbv<_Nb2>& x);
                    template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
                      intbv<_Nb> &set (const intbv<_Nb1> &start, const intbv<_Nb2> &stop, const intbv<_Nb3>& x);

                    // Sets the specified range to all ones.  If start > stop, the masking operation loops around.
                    intbv<_Nb> &mask (size_t start, size_t stop);
                    template <size_t _Nb1,size_t _Nb2>
                      intbv<_Nb> &mask (const intbv<_Nb1>& start, const intbv<_Nb2> &stop);
                    // Sets the specified range to all zeroes.
                    intbv<_Nb> &mask_inv (size_t start, size_t stop);
                    template <size_t _Nb1,size_t _Nb2>
                      intbv<_Nb> &mask_inv (const intbv<_Nb1>& start, const intbv<_Nb2>& stop);

                    // This sets the specified range to all ones, where the range must have start < stop.
                    intbv<_Nb> &simple_mask (size_t start, size_t stop);
                    template <size_t _Nb1,size_t _Nb2>
                      inline intbv<_Nb> &simple_mask (const intbv<_Nb1>& start, const intbv<_Nb2>& stop);

                    // This sets the specified range to all zeros, where the range must have start < stop.
                    intbv<_Nb> &simple_mask_inv (size_t start, size_t stop);
                    template <size_t _Nb1,size_t _Nb2>
                      intbv<_Nb> &simple_mask_inv (const intbv<_Nb1>& start, const intbv<_Nb2>& stop);

                    // to get a field/slice (returns same size intbv w/ value right justified
                    const intbv<_Nb> operator () (size_t start, size_t stop) const;
                    const intbv<_Nb> get (size_t start, size_t stop) const;
                    template <size_t _Nb1,size_t _Nb2>
                      const intbv<_Nb> get (const intbv<_Nb1>& start, const intbv<_Nb2>& stop) const;
                    template <size_t _Nb1,size_t _Nb2>
                      const intbv<_Nb> operator () (const intbv<_Nb1>& start, const intbv<_Nb2>& stop) const;
                    template <size_t _Nb2>
                      const intbv<_Nb2> get (size_t start, size_t stop) const;
                    template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
                      const intbv<_Nb3> get (const intbv<_Nb1>& start,const intbv<_Nb2>& stop) const;
    
                    // arithmetic operations on intbvs.
                    intbv<_Nb>& operator%=(const intbv<_Nb>& rhs);
                    intbv<_Nb>& operator%=(int64_t rhs);
                    intbv<_Nb>& operator%=(uint64_t rhs);
                    intbv<_Nb>& operator%=(int32_t rhs);
                    intbv<_Nb>& operator%=(uint32_t rhs);
                    template <size_t _Nb2>
                      intbv<_Nb>& operator%=(const intbv<_Nb2>& rhs); // diff sized, special cased
             
                    intbv<_Nb>& operator*=(const intbv<_Nb>& rhs); //unsigned multiply
                    intbv<_Nb>& operator*=(uint64_t rhs); //unsigned multiply
                    intbv<_Nb>& operator*=(int32_t rhs); //unsigned multiply
                    intbv<_Nb>& operator*=(int64_t rhs); //unsigned multiply
                    intbv<_Nb>& operator*=(uint32_t rhs); //unsigned multiply
             
                    intbv<_Nb>& signedMultiplyEqual(const intbv<_Nb>& rhs); // signed multiply
                    intbv<_Nb>& signedMultiplyEqual(uint64_t rhs); //signed multiply
              
                    intbv<_Nb>& signedDivideEqual(const intbv<_Nb>& rhs); // signed divide
                    intbv<_Nb>& divideSignedAligned(const intbv<_Nb>& rhs); // word aligned divide
                    template <size_t _Nb2> 
                      intbv<_Nb>& signedDivideEqual(const intbv<_Nb2>& rhs);
                    intbv<MAX(_Nb,64)>& signedDivideEqual(uint64_t rhs); //signed divide
                    intbv<MAX(_Nb,32)>& signedDivideEqual(uint32_t rhs); //signed divide

                    intbv<_Nb>& operator/=(const intbv<_Nb>& rhs);
                    template <size_t _Nb2>
                      intbv<_Nb>& operator/=(const intbv<_Nb2>& rhs); // diff sized, special cased
                    intbv<_Nb>& operator/=(uint64_t rhs);
                    intbv<_Nb>& operator/=(int32_t rhs);
                    intbv<_Nb>& operator/=(int64_t rhs);
                    intbv<_Nb>& operator/=(uint32_t rhs);
          
                    intbv<_Nb>& operator-=(const intbv<_Nb>& rhs);
                    intbv<_Nb>& operator-=(uint64_t rhs);
                    intbv<_Nb>& operator-=(int64_t rhs);
                    intbv<_Nb>& operator-=(uint32_t rhs);
                    intbv<_Nb>& operator-=(int32_t rhs);
            
                    intbv<_Nb>& operator+=(const intbv<_Nb>& rhs);
                    intbv<_Nb>& operator+=(uint64_t rhs);
                    intbv<_Nb>& operator+=(int64_t rhs);
                    intbv<_Nb>& operator+=(uint32_t rhs);
                    intbv<_Nb>& operator+=(int32_t rhs);
    
                    // logical operations on intbvs.
                    intbv<_Nb>& operator&=(const intbv<_Nb>& rhs);
                    intbv<_Nb>& operator|=(const intbv<_Nb>& rhs);
                    intbv<_Nb>& operator^=(const intbv<_Nb>& rhs);
    
                    // shift/rotate operations on intbvs.     
                    intbv<_Nb>& operator<<=(size_t position);
                    template <size_t _Nb2>
                      intbv<_Nb>& operator<<=(const intbv<_Nb2>& position);
                    intbv<_Nb>& operator>>=(size_t position);
                    template <size_t _Nb2>
                      intbv<_Nb>& operator>>=(const intbv<_Nb2>& position);
                    intbv<_Nb>& signedRightShiftEqual(size_t position);
                    template <size_t _Nb2>
                      intbv<_Nb>& signedRightShiftEqual(const intbv<_Nb2>& position);


                    template <size_t _Nb2>
                      intbv<_Nb> left_rotate(const intbv<_Nb2>& position) const;
                    intbv<_Nb> left_rotate (size_t position) const;

                    template <size_t _Nb2>
                      intbv<_Nb> right_rotate(const intbv<_Nb2>& position) const;
                    intbv<_Nb> right_rotate (size_t position) const;


                    intbv<_Nb> swap_bytes() const;
                    intbv<_Nb> swap_words() const;

                    // single-bit set, reset, invert, and getBit (no range checking)
                    intbv<_Nb>& _Unchecked_set(size_t pos);
                    intbv<_Nb>& _Unchecked_set(size_t pos, int val);  
                    intbv<_Nb>& _Unchecked_reset(size_t pos);
                    intbv<_Nb>& _Unchecked_invert(size_t pos);
                    bool _Unchecked_test(size_t pos) const;
    
                    // Set, reset, and invert.  
                    // Sets every bit to true.
                    intbv<_Nb>& set();
                    intbv<_Nb>& set(size_t position, bool val = true);
                    template <size_t _Nb1>
                      intbv<_Nb>& set(const intbv<_Nb1>& position, bool val = true);
                    template <size_t _Nb1>
                      intbv<_Nb>& set(size_t position,const intbv<_Nb1>& value);
                    template <size_t _Nb1,size_t _Nb2>
                      intbv<_Nb>& set(const intbv<_Nb1>& position,const intbv<_Nb2>& value);
                    // Sets every bit to false.
                    intbv<_Nb>& reset();
                    intbv<_Nb>& reset(size_t position);
                    // Toggles every bit to its opposite value.
                    intbv<_Nb>& invert();
                    // Two's complement
                    intbv<_Nb>& negate();
                    // Toggles a given bit to its opposite value.
                    intbv<_Nb>& invert(size_t position);
                    // See the no-argument invert().
                    intbv<_Nb> operator~() const { return intbv<_Nb>(*this).invert(); }
    
                    //    Array-indexing support.
                    const intbv<1> operator[](size_t position) const { return _Unchecked_test(position); }
                    template <size_t _Nb2>
                      const intbv<1> operator[](const intbv<_Nb2>& position) const { return operator[](position.uint32()); }

                    const intbv<1> operator()(size_t position) const { return _Unchecked_test(position); }
                    template <size_t _Nb2>
                      const intbv<1> operator()(const intbv<_Nb2>& position) const { return operator()(position.uint32()); };
    
                    // Returns the lower 32-bits of the intbv (or the entire thing if it's smaller
                    // than that).
                    uint32_t uint32 () const 
                    { return _Base::_to_ulong(); }

                    // Returns the lower 64-bits (or the entire thing) of the intbv.
                    uint64_t uint64() const
                    { return _Base::_to_ulonglong(); }
  
                    // Returns the sign-extended lower 64-bits (or the entire thing) of the intbv.
                    int64_t int64() const
                    { 
                      // we lose a bit of precision for the sign bit
                      if (_Nb > 1) {
                        intbv<64> tmp = signExtend<64>(); 
                        return tmp.uint64();
                      }
                      return uint64(); 
                    }

                    // Returns the sign-extended lower 32-bits (or the entire thing) of the intbv.
                    int32_t int32() const
                    {
                      // we lose a bit of precision for the sign bit
                      if (_Nb > 1) {
                        intbv<32> tmp = signExtend<32>(); 
                        return tmp.uint32();
                      }
                      return uint32(); 
                    }

# ifndef __NO_RNUMBER__
                    // Returns an RNumber of the same size as this object.
                    rnumber::RNumber rnum() const { return _Base::_createRNumber(_Nb); };
# endif

                    // Returns the number of bits which are set.
                    size_t count_ones() const { return _Base::_count_ones(); }
                    // Returns the number of leading zeros.
                    size_t count_leading_zeros() const { return _Base::_count_leading_zeros(_Nb); }
                    // Returns the total number of bits.
                    size_t size() const { return _Nb; }
    
                    // These comparisons for equality/inequality are bitwise (for same sized intbvs)
                    // == 
                    bool operator==(const intbv<_Nb>& rhs) const
                    { return _Base::_is_equal(rhs); }
	     
                    bool operator==(int32_t rhs) const
                    { return operator==(static_cast<intbv<32> >(rhs)); }
              
                    bool operator==(uint32_t rhs) const
                    { return operator==(static_cast<intbv<32> >(rhs)); }              
	     
                    bool operator==(uint64_t rhs) const
                    { return operator==(static_cast<intbv<64> >(rhs)); }      
	     
                    bool operator==(int64_t rhs) const
                    { return operator==(static_cast<intbv<64> >(rhs)); }      
                    // !=
                    bool operator!=(const intbv<_Nb>& rhs) const
                    { return !_Base::_is_equal(rhs);}
	      	      	     
                    bool operator!=(int32_t rhs) const
                    { return operator!=(static_cast<intbv<32> >(rhs)); }
              
                    bool operator!=(uint32_t rhs) const
                    { return operator!=(static_cast<intbv<32> >(rhs)); }              
	     
                    bool operator!=(uint64_t rhs) const
                    { return operator!=(static_cast<intbv<64> >(rhs)); }  
	     
                    bool operator!=(int64_t rhs) const
                    { return operator!=(static_cast<intbv<64> >(rhs)); }  
	        	      
                    // <=
                    bool operator<=(const intbv<_Nb>& rhs) const
                    { return _Base::_less_than_equal(rhs); }
                               
                    bool operator<=(int32_t rhs) const
                    { return operator<=(static_cast<intbv<32> >(rhs)); }
              
                    bool operator<=(uint32_t rhs) const
                    { return operator<=(static_cast<intbv<32> >(rhs)); }              
	     
                    bool operator<=(uint64_t rhs) const
                    { return operator<=(static_cast<intbv<64> >(rhs)); } 
	     
                    bool operator<=(int64_t rhs) const
                    { return operator<=(static_cast<intbv<64> >(rhs)); } 
	      
                    // >=
                    bool operator>=(const intbv<_Nb>& rhs) const
                    { return _Base::_greater_than_equal(rhs); }
                     
                    bool operator>=(int32_t rhs) const
                    { return operator>=(static_cast<intbv<32> >(rhs)); }
              
                    bool operator>=(uint32_t rhs) const
                    { return operator>=(static_cast<intbv<32> >(rhs)); }              
	     
                    bool operator>=(uint64_t rhs) const
                    { return operator>=(static_cast<intbv<64> >(rhs)); } 
	     
                    bool operator>=(int64_t rhs) const
                    { return operator>=(static_cast<intbv<64> >(rhs)); } 
    
                    // <
                    bool operator<(const intbv<_Nb>& rhs) const
                    { return _Base::_less_than(rhs); }
              
                    bool operator<(int32_t rhs) const
                    { return operator<(static_cast<intbv<32> >(rhs)); }
              
                    bool operator<(uint32_t rhs) const
                    { return operator<(static_cast<intbv<32> >(rhs)); }              
	     
                    bool operator<(uint64_t rhs) const
                    { return operator<(static_cast<intbv<64> >(rhs)); }    
	     
                    bool operator<(int64_t rhs) const
                    { return operator<(static_cast<intbv<64> >(rhs)); }    
    
                    // >
                    bool operator>(const intbv<_Nb>& rhs) const
                    { return _Base::_greater_than(rhs); }
	                                     
                    bool operator>(int32_t rhs) const
                    { return operator>(static_cast<intbv<32> >(rhs)); }
              
                    bool operator>(uint32_t rhs) const
                    { return operator>(static_cast<intbv<32> >(rhs)); }              
	     
                    bool operator>(uint64_t rhs) const
                    { return operator>(static_cast<intbv<64> >(rhs)); } 
	     
                    bool operator>(int64_t rhs) const
                    { return operator>(static_cast<intbv<64> >(rhs)); } 

                    // comparing same sized intbv's
                    bool signedLE (const intbv<_Nb>& rhs) const
                    { return _Base::_signed_less_than_equal(rhs,_Nb); }

                    bool signedLT (const intbv<_Nb>& rhs) const
                    { return _Base::_signed_less_than(rhs,_Nb); }

                    bool signedGT (const intbv<_Nb>& rhs) const
                    { return _Base::_signed_greater_than(rhs,_Nb); }

                    bool signedGE (const intbv<_Nb>& rhs) const
                    { return _Base::_signed_greater_than_equal(rhs,_Nb); }
    
                    // comparing diff sized intbv's
                    template <size_t _Nb2>
                      bool signedLE (const intbv<_Nb2>& rhs) const;
   
                    template <size_t _Nb2>
                      bool signedLT (const intbv<_Nb2>& rhs) const;
    
                    template <size_t _Nb2>
                      bool signedGT (const intbv<_Nb2>& rhs) const;
    
                    template <size_t _Nb2>
                      bool signedGE (const intbv<_Nb2>& rhs) const;

                    template <size_t _Nb2> 
                      intbv<_Nb2> signExtend() const;
                    void signExtend(size_t bit);  

                    // These comparisons for equality/inequality are bitwise (for diff sized intbvs)    
                    template <size_t _Nb2>
                      bool operator==(const intbv<_Nb2>& rhs) const;
    
                    template <size_t _Nb2>
                      bool operator!=(const intbv<_Nb2>& rhs) const;
    
                    template <size_t _Nb2>
                      bool operator<=(const intbv<_Nb2>& rhs) const;
    
                    template <size_t _Nb2>
                      bool operator>=(const intbv<_Nb2>& rhs) const;

                    template <size_t _Nb2>
                      bool operator<(const intbv<_Nb2>& rhs) const;
    
                    template <size_t _Nb2>
                      bool operator>(const intbv<_Nb2>& rhs) const;
    
#ifdef IS_64BIT
                    bool operator==(unsigned long long  rhs) const
                    { return operator==(static_cast<intbv<64> >(rhs)); }
                    bool operator!=(unsigned long long  rhs) const
                    { return operator!=(static_cast<intbv<64> >(rhs)); }

                    bool operator==(long long int rhs) const
                    { return operator==(static_cast<intbv<64> >(rhs)); }
                    bool operator!=(long long int rhs) const
                    { return operator!=(static_cast<intbv<64> >(rhs)); }

                    bool operator<(long long int rhs) const
                    { return operator<(static_cast<intbv<64> >(rhs)); }    
                    bool operator<(unsigned long long int rhs) const
                    { return operator<(static_cast<intbv<64> >(rhs)); }    

                    bool operator>(long long int rhs) const
                    { return operator>(static_cast<intbv<64> >(rhs)); }    
                    bool operator>(unsigned long long int rhs) const
                    { return operator>(static_cast<intbv<64> >(rhs)); }    

                    bool operator<=(long long int rhs) const
                    { return operator<=(static_cast<intbv<64> >(rhs)); }    
                    bool operator<=(unsigned long long int rhs) const
                    { return operator<=(static_cast<intbv<64> >(rhs)); }    

                    bool operator>=(long long int rhs) const
                    { return operator>=(static_cast<intbv<64> >(rhs)); }    
                    bool operator>=(unsigned long long int rhs) const
                    { return operator>=(static_cast<intbv<64> >(rhs)); }    

         
                    intbv<_Nb>& operator+=(long long unsigned rhs);
                    intbv<_Nb>& operator-=(long long unsigned rhs);
                    intbv<_Nb>& operator/=(long long unsigned rhs);
                    intbv<_Nb>& operator*=(long long unsigned rhs);
                    intbv<_Nb>& operator%=(long long unsigned rhs);
#endif

        

                    //   Tests the value of a bit.
                    bool get(size_t position) const;
    
                    //   Tests whether any of the bits are on.
                    bool any() const { return _Base::_is_any(); }
    
                    bool none() const { return !_Base::_is_any(); }
    

                    intbv<_Nb> operator<<(size_t position) const
                    { return intbv<_Nb>(*this) <<= position; }

                    template<size_t _Nb2>
                      intbv<_Nb> operator<<(const intbv<_Nb2>& position) const
                      { return intbv<_Nb>(*this) <<= position.uint32(); }

                    template<size_t _Nb2>
                      intbv<_Nb> operator>>(const intbv<_Nb2>& position) const
                      { return intbv<_Nb>(*this) >>= position.uint32(); }

                    intbv<_Nb> operator>>(size_t position) const
                    { return intbv<_Nb>(*this) >>= position; }
              
                    template<size_t _Nb2>
                      intbv<_Nb> signedRightShift (const intbv<_Nb2>& position) const
                      { return intbv<_Nb>(*this).signedRightShiftEqual(position.uint32()); }
              
                    intbv<_Nb> signedRightShift (size_t position) const
                    { return intbv<_Nb>(*this).signedRightShiftEqual(position); }

                    //    Finds the index of the first "on" bit.
                    size_t _Find_first() const
                    { return _Base::_find_first(_Nb); }
             
                    // string manipulation
             
                    std::string str (int format = 0) const;
                    std::ostream& printToOS( std::ostream& os ) const;
                    void readNumber (std::istream& is) { _Base::_readNumber(_Nb,is); }
                    void writeNumber (std::ostream& os) const {_Base::_writeNumber(os,_Nb);}
                    void dummy() const { _Base::_dummy(); };
  private:

                    void _sanitize()
                    { _Base::template _sanitize<_Nb>(); }

  };

}

#include "intbv_impl.h"

#endif /* _INTBV */

