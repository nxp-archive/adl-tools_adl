//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

// 
// Bit-vector class with an undefined mask.  This implements the same interface
// as intbv, but has an accompanying mask for identifying which portions are to
// be considered undefined (1 means undefined, 0 means defined).  In general,
// arithmetic operations, if either operand has a mask that is set, result in a
// fully set mask for the result.  This may be refined further in the future.
//

#ifndef _BITS_H_
#define _BITS_H_

#include "intbv.h"

namespace adl {

  template<size_t _Nb>
  class bits : public intbv<_Nb>
  {
    template <size_t> friend  class bits;

    typedef bits<_Nb>  This;
    typedef intbv<_Nb> Base;

  public:
    // constructors
    // All bits set to zero.
    bits() {};
    // Initial bits bitwise-copied from a single word (others set to zero).
#ifdef IS_64BIT
    bits(unsigned long long val) : Base(val) {};
    bits(long long int val) : Base(val) {};
    
#else
    bits(unsigned long val) : Base(val) {};
    bits(long val) : Base(val) {};
#endif
    bits(uint64_t val) : Base(val) {};
    bits(int64_t val) : Base(val) {};
     bits(uint32_t val) : Base(val) {};
    bits(int32_t val) : Base(val) {};
    // Construct from string.  Where format is explicitly set, the input should
    // contain a prefix, e.g. 0x...
    bits( const char* s) : Base(s) {};
    bits( const std::string& s) : Base(s) {};
    bits( const std::string& s, int format) : Base(s,format) {};
    bits( const std::istream &is ) : Base(is) {};
    bits( const rnumber::RNumber &x ) : Base(x) {};
# ifndef NO_PROPAGATE
    // Copy (same size)
    bits (const bits <_Nb> &x) : Base (x)
                               , _mask(x._mask) 
    {};  
# endif
    // Create from intbv (same size)
    bits (const intbv <_Nb> x) : Base (x) {};    
    
    // Copy (diff size)
    template <size_t _Nb2>
    bits (const bits<_Nb2> &x) : Base(x)
# ifndef NO_PROPAGATE
                               , _mask(x._mask) 
# endif
    {};

    // Copy from intbv (diff size)
    template <size_t _Nb2>
    bits (const intbv<_Nb2> &x) : Base(x) {};
    
    // operator= (diff size)
    template <size_t _Nb2>
    bits<_Nb> &operator= (const bits <_Nb2> &x);
    
    // ctor that takes a templated initialization functor
    template <typename R> 
    explicit bits (unsigned x,const R &r) : Base (x, r) {};

    // Set the mask to register that a portion is undefined.
    void setUndefined(unsigned pos);
    void setUndefined(unsigned start,unsigned stop);

    // True if anything is undefined in the specified range.
    bool isUndefined(unsigned start,unsigned stop) const;
    // True if there is any undefined.
    bool isUndefined() const;

    // Operations on 'bits'.

    // Set a field to the value x.
    bits<_Nb> &set (unsigned start, unsigned stop, const bits<_Nb>& x);
    template <size_t _Nb2>
    bits<_Nb> &set (unsigned start, unsigned stop, const bits<_Nb2>& x);
    template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
    bits<_Nb> &set (const bits<_Nb1>&, const bits<_Nb2>&, const bits<_Nb3>& x);
    // sets the specified range to all ones.
    bits<_Nb> &mask (unsigned start, unsigned stop);
    template <size_t _Nb1,size_t _Nb2>
    bits<_Nb> &mask (const bits<_Nb1>& start, const bits<_Nb2>& stop);
    // sets the specified range to all zeroes.
    bits<_Nb> &mask_inv (unsigned start, unsigned stop);
    template <size_t _Nb1,size_t _Nb2>
    bits<_Nb> &mask_inv (const bits<_Nb1>& start, const bits<_Nb2>& stop);
    // Sets every bit to true.
    bits<_Nb>& set();
    bits<_Nb>& set(size_t position, bool val = true);
    template <size_t _Nb1>
    bits<_Nb>& set(const bits<_Nb1>& position, bool val = true);
    template <size_t _Nb1,size_t _Nb2>
    bits<_Nb>& set(const bits<_Nb1>& position,const bits<_Nb2>& value);
    // Sets every bit to false.
    bits<_Nb>& reset();
    bits<_Nb>& reset(size_t position);
    // Toggles every bit to its opposite value.
    bits<_Nb>& invert();
    // Two's complement
    bits<_Nb>& negate();
    // Toggles a given bit to its opposite value.
    bits<_Nb>& invert(size_t position);
    // See the no-argument invert().
    bits<_Nb> operator~() const { return bits<_Nb>(*this).invert(); }

    // To get a field/slice (returns same size bits w/ value right justified
    bits<_Nb> operator () (unsigned start, unsigned stop) const;
    bits<_Nb> get (unsigned start, unsigned stop) const;
    template <size_t _Nb1,size_t _Nb2>
    bits<_Nb> get (const bits<_Nb1>& start, const bits<_Nb2>& stop) const;
    template <size_t _Nb1,size_t _Nb2>
    bits<_Nb> operator () (const bits<_Nb1>& start, const bits<_Nb2>& stop) const;
    template <size_t _Nb1>
    bits<_Nb1> get (unsigned start, unsigned stop) const;
    template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
    bits<_Nb1> get (const bits<_Nb2>& start,const bits<_Nb3>& stop) const;

    // Array-indexing support.
    using Base::operator[];
    using Base::operator();

    // single-bit set, reset, invert, and getBit (no range checking)
    bits<_Nb>& _Unchecked_set(size_t pos);
    bits<_Nb>& _Unchecked_set(size_t pos, int val);  
    bits<_Nb>& _Unchecked_reset(size_t pos);
    bits<_Nb>& _Unchecked_invert(size_t pos);
    using Base::_Unchecked_test;
    // Tests the value of a bit.
    bool get(size_t position) const { return Base::get(position); };
    // Tests whether any of the bits are on.
    using Base::any;
    // Test if all are off.
    using Base::none;
    
    // arithmetic operations on bits.
    bits<_Nb>& operator%=(const bits<_Nb> &rhs);
    bits<_Nb>& operator%=(uint64_t rhs);
    bits<_Nb>& operator%=(int32_t rhs);
    bits<_Nb>& operator%=(int64_t rhs);
    bits<_Nb>& operator%=(uint32_t rhs);
    template <size_t _Nb2>
    bits<_Nb>& operator%=(const bits<_Nb2>& rhs); // diff sized, special cased
    bits<_Nb>& operator*=(const bits<_Nb> &rhs);
    bits<_Nb>& operator*=(uint64_t rhs);
    bits<_Nb>& operator*=(int32_t rhs);
    bits<_Nb>& operator*=(int64_t rhs);
    bits<_Nb>& operator*=(uint32_t rhs);
    bits<_Nb>& signedMultiplyEqual(const bits<_Nb> &rhs);
    bits<_Nb>& signedMultiplyEqual(uint64_t rhs);
    bits<_Nb>& operator/=(const bits<_Nb> &rhs);
    template <size_t _Nb2>
    bits<_Nb>& operator/=(const bits<_Nb2>& rhs); // diff sized, special cased
    bits<_Nb>& operator/=(uint64_t rhs);
    bits<_Nb>& operator/=(int32_t rhs);
    bits<_Nb>& operator/=(int64_t rhs);
    bits<_Nb>& operator/=(uint32_t rhs);

    bits<MAX(_Nb,64)>& signedDivideEqual(uint64_t rhs);
    bits<MAX(_Nb,32)>& signedDivideEqual(uint32_t rhs);
    bits<_Nb>& signedDivideEqual(const bits<_Nb> &rhs);
    template <size_t _Nb2> 
    bits<_Nb>& signedDivideEqual(const bits<_Nb2>& rhs); 
    
    bits<_Nb>& operator-=(const bits<_Nb> &rhs);
    bits<_Nb>& operator-=(uint64_t rhs);
    bits<_Nb>& operator-=(int32_t rhs);
    bits<_Nb>& operator-=(int64_t rhs);
    bits<_Nb>& operator-=(uint32_t rhs);
    bits<_Nb>& operator+=(const bits<_Nb> &rhs);
    bits<_Nb>& operator+=(uint64_t rhs);
    bits<_Nb>& operator+=(int32_t rhs);
    bits<_Nb>& operator+=(int64_t rhs);
    bits<_Nb>& operator+=(uint32_t rhs);
  
    // logical operations on bitss.
    bits<_Nb>& operator&=(const bits<_Nb> &rhs);
    bits<_Nb>& operator|=(const bits<_Nb> &rhs);
    bits<_Nb>& operator^=(const bits<_Nb> &rhs);
  
    // shift/rotate operations on bitss.     
    bits<_Nb>& operator<<=(size_t position);
    bits<_Nb>& operator>>=(size_t position);
    bits<_Nb>& signedRightShiftEqual(size_t position);
           
    template<size_t _Nb2>
    bits<_Nb> operator<<(const bits<_Nb2>& position) const;
    bits<_Nb> operator<<(size_t position) const;

    template<size_t _Nb2>
    bits<_Nb> operator>>(const bits<_Nb2>& position) const;
    bits<_Nb> operator>>(size_t position) const;

    template<size_t _Nb2>
    bits<_Nb> left_rotate(const bits<_Nb2>& position) const;
    bits<_Nb> left_rotate (size_t position) const;

    template<size_t _Nb2>
    bits<_Nb> right_rotate(const bits<_Nb2>& position) const;
    bits<_Nb> right_rotate(size_t position) const;
    

    bits<_Nb> swap_bytes() const;
    bits<_Nb> swap_words() const;

    // Retuns a numerical interpretation.
    using Base::uint32;
    using Base::uint64;
    using Base::rnum;
  
    // Returns the number of bits which are set.
    using Base::count_ones;
    // Returns the number of leading zeros.
    using Base::count_leading_zeros;
    // Returns the total number of bits.
    using Base::size;
  
    // These comparisons for equality/inequality are bitwise (for same sized bits)
    // and only look at the value, not the mask.
    bool operator==(const bits<_Nb> &rhs) const;    
    bool operator==(int32_t rhs) const;    
    bool operator==(uint32_t rhs) const;    
    bool operator==(uint64_t rhs) const;    
    bool operator==(int64_t rhs) const;    
    bool operator!=(const bits<_Nb> &rhs) const;
    bool operator!=(int32_t rhs) const;    
    bool operator!=(uint32_t rhs) const;    
    bool operator!=(uint64_t rhs) const;    
    bool operator!=(int64_t rhs) const;    
    bool operator<=(const bits<_Nb> &rhs) const;
    bool operator<=(int32_t rhs) const;    
    bool operator<=(uint32_t rhs) const;    
    bool operator<=(uint64_t rhs) const;    
    bool operator<=(int64_t rhs) const;    
    bool operator>=(const bits<_Nb> &rhs) const;
    bool operator>=(int32_t rhs) const;    
    bool operator>=(uint32_t rhs) const;    
    bool operator>=(uint64_t rhs) const;    
    bool operator>=(int64_t rhs) const;    
    bool operator< (const bits<_Nb> &rhs) const;
    bool operator< (int32_t rhs) const;    
    bool operator< (uint32_t rhs) const;    
    bool operator< (uint64_t rhs) const;    
    bool operator< (int64_t rhs) const;    
    bool operator> (const bits<_Nb> &rhs) const;
    bool operator> (int32_t rhs) const;    
    bool operator> (uint32_t rhs) const;    
    bool operator> (uint64_t rhs) const;    
    bool operator> (int64_t rhs) const;    
    template <size_t _Nb2> bool operator==(const bits<_Nb2> &rhs) const;
    template <size_t _Nb2> bool operator!=(const bits<_Nb2> &rhs) const;
    template <size_t _Nb2> bool operator<=(const bits<_Nb2> &rhs) const;
    template <size_t _Nb2> bool operator>=(const bits<_Nb2> &rhs) const;
    template <size_t _Nb2> bool operator< (const bits<_Nb2> &rhs) const;
    template <size_t _Nb2> bool operator> (const bits<_Nb2> &rhs) const;
#ifdef IS_64BIT
    bool operator==(long long unsigned int rhs) const;
    bool operator==(long long int rhs) const;
    bool operator!=(long long unsigned int rhs) const;
    bool operator!=(long long int rhs) const;
    bool operator> (long long unsigned int rhs) const;    
    bool operator> (long long int rhs) const;  
    bool operator< (long long unsigned int rhs) const;    
    bool operator< (long long int rhs) const;  
    bool operator>=(long long unsigned int rhs) const;    
    bool operator>=(long long int rhs) const;  
    bool operator<=(long long unsigned int rhs) const;    
    bool operator<=(long long int rhs) const;    
#endif              

    // Signed comparisons.
    bool signedLE (const bits<_Nb> &rhs) const;
    bool signedLT (const bits<_Nb> &rhs) const;
    bool signedGT (const bits<_Nb> &rhs) const;
    bool signedGE (const bits<_Nb> &rhs) const;
    template <size_t _Nb2> bool signedLE (const bits<_Nb2> &rhs) const;
    template <size_t _Nb2> bool signedLT (const bits<_Nb2> &rhs) const;
    template <size_t _Nb2> bool signedGT (const bits<_Nb2> &rhs) const;    
    template <size_t _Nb2> bool signedGE (const bits<_Nb2> &rhs) const;

    // Sign extension.
    template <size_t _Nb2> bits<_Nb2> signExtend() const;
    using Base::signExtend;

    // Finds the index of the first "on" bit.
    using Base::_Find_first;
      
    // String manipulation
    using Base::str;
    using Base::printToOS;
    using Base::readNumber;
    using Base::writeNumber;

    // Binary operators must be friends due to private inheritence.
    template<size_t Nb> friend bits<Nb> operator-(const bits<Nb>& x, const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator-(const bits<Nb>& x, uint64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator-(uint64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator-(const bits<Nb>& x, int64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator-(int64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator-(const bits<Nb>& x, int32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator-(int32_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator-(const bits<Nb>& x, uint32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator-(uint32_t x,const bits<Nb>& y);
    template<size_t Nb1, size_t Nb2> friend bits<MAX(Nb1,Nb2)> operator-(const bits<Nb1>& x, const bits<Nb2>& y);

    template<size_t Nb> friend bits<Nb> operator+(const bits<Nb>& x, const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator+(const bits<Nb>& x, int64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator+(int64_t x,const bits<Nb>& y);  
    template<size_t Nb> friend bits<MAX(Nb,64)> operator+(const bits<Nb>& x, uint64_t y);    
    template<size_t Nb> friend bits<MAX(Nb,64)> operator+(uint64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator+(const bits<Nb>& x, int32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator+(int32_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator+(const bits<Nb>& x, uint32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator+(uint32_t x,const bits<Nb>& y);
  
    template<size_t Nb1, size_t Nb2> friend bits<MAX(Nb1,Nb2)> operator+(const bits<Nb1>& x, const bits<Nb2>& y);

    template<size_t Nb> friend bits<Nb> operator*(const bits<Nb>& x, const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator*(const bits<Nb>& x, uint64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator*(uint64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator*(const bits<Nb>& x, int64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator*(int64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator*(const bits<Nb>& x, int32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator*(int32_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator*(const bits<Nb>& x, uint32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator*(uint32_t x,const bits<Nb>& y);
    template<size_t Nb1, size_t Nb2> friend bits<MAX(Nb1,Nb2)> operator*(const bits<Nb1>& x, const bits<Nb2>& y);

 
    template<size_t Nb> friend bits<Nb> signedMultiply(const bits<Nb>& x, const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> signedMultiply(const bits<Nb>& x, uint64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> signedMultiply(uint64_t x,const bits<Nb>& y);
    template<size_t Nb1, size_t Nb2> friend bits<MAX(Nb1,Nb2)> signedMultiply(const bits<Nb1>& x, const bits<Nb2>& y);

    template<size_t Nb> friend bits<Nb> operator/(const bits<Nb>& x, const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator/(const bits<Nb>& x, int64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator/(int64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator/(const bits<Nb>& x, uint64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator/(uint64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator/(const bits<Nb>& x, int32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator/(int32_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator/(const bits<Nb>& x, uint32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator/(uint32_t x,const bits<Nb>& y);
    template<size_t Nb1, size_t Nb2> friend bits<MAX(Nb1,Nb2)> operator/(const bits<Nb1>& x, const bits<Nb2>& y);

    template<size_t Nb> friend bits<Nb> signedDivide(const bits<Nb>& x, const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> signedDivide(const bits<Nb>& x, uint64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> signedDivide(uint64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> signedDivide(const bits<Nb>& x, uint32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> signedDivide(uint32_t x,const bits<Nb>& y);    
    template<size_t Nb1, size_t Nb2> friend bits<MAX(Nb1,Nb2)> signedDivide(const bits<Nb1>& x, const bits<Nb2>& y);
  
    template<size_t Nb> friend bits<Nb> operator%(const bits<Nb>& x, const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator%(const bits<Nb>& x, int64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator%(int64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator%(const bits<Nb>& x, uint64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator%(uint64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator%(const bits<Nb>& x, int32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator%(int32_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator%(const bits<Nb>& x, uint32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator%(uint32_t x,const bits<Nb>& y);
    template<size_t Nb1, size_t Nb2> friend bits<MAX(Nb1,Nb2)> operator%(const bits<Nb1>& x, const bits<Nb2>& y);

    template<size_t Nb> friend bits<Nb> operator&(const bits<Nb>& x, const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator&(const bits<Nb>& x, uint64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator&(uint64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator&(const bits<Nb>& x, int64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator&(int64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator&(const bits<Nb>& x, uint32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator&(uint32_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator&(const bits<Nb>& x, int32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator&(int32_t x,const bits<Nb>& y);
    template<size_t Nb1, size_t Nb2> friend bits<MAX(Nb1,Nb2)> operator&(const bits<Nb1>& x, const bits<Nb2>& y);

    template<size_t Nb> friend bits<Nb> operator|(const bits<Nb>& x, const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator|(const bits<Nb>& x, uint64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator|(uint64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator|(const bits<Nb>& x, int64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator|(int64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator|(const bits<Nb>& x, uint32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator|(uint32_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator|(const bits<Nb>& x, int32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator|(int32_t x,const bits<Nb>& y);
    template<size_t Nb1, size_t Nb2> friend bits<MAX(Nb1,Nb2)> operator|(const bits<Nb1>& x, const bits<Nb2>& y);

    template<size_t Nb> friend bits<Nb> operator^(const bits<Nb>& x, const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator^(const bits<Nb>& x, uint64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator^(uint64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator^(const bits<Nb>& x, int64_t y);
    template<size_t Nb> friend bits<MAX(Nb,64)> operator^(int64_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator^(const bits<Nb>& x, uint32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator^(uint32_t x,const bits<Nb>& y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator^(const bits<Nb>& x, int32_t y);
    template<size_t Nb> friend bits<MAX(Nb,32)> operator^(int32_t x,const bits<Nb>& y);
    template<size_t Nb1, size_t Nb2> friend bits<MAX(Nb1,Nb2)> operator^(const bits<Nb1>& x, const bits<Nb2>& y);

    template <size_t _Nb1, size_t _Nb2> friend bits< _Nb1 + _Nb2> concat (const bits<_Nb1>& x, const bits<_Nb2>& y);
    template <size_t _Nb1, size_t _Nb2, size_t _Nb3> friend bits<_Nb1+_Nb2+_Nb3> concat (const bits<_Nb1>& x, const bits<_Nb2>& y, const bits<_Nb3>& z);
    template <size_t _Nb1, size_t _Nb2, size_t _Nb3,size_t _Nb4> friend bits<_Nb1+_Nb2+_Nb3+_Nb4> concat (const bits<_Nb1>& w, const bits<_Nb2>& x, const bits<_Nb3>& y,const bits<_Nb4>& z);
    template <size_t _Nb1, size_t _Nb2, size_t _Nb3,size_t _Nb4,size_t _Nb5> friend bits<_Nb1+_Nb2+_Nb3+_Nb4+_Nb5> concat (const bits<_Nb1>& v, const bits<_Nb2>& w, const bits<_Nb3>& x,const bits<_Nb4>& y,const bits<_Nb5>& z);
    template <size_t _Nb1, size_t _Nb2, size_t _Nb3,size_t _Nb4,size_t _Nb5,size_t _Nb6> friend bits<_Nb1+_Nb2+_Nb3+_Nb4+_Nb5+_Nb6> concat (const bits<_Nb1>& u, const bits<_Nb2>& v, const bits<_Nb3>& w,const bits<_Nb4>& x,const bits<_Nb5>& y, const bits<_Nb6>& z);

    template <size_t Nb1, size_t Nb2> friend bits<MAX(Nb1,Nb2)> Carry (const bits<Nb1>& a,const bits<Nb2>& b, const bits<1>& cin);
    template <size_t Nb1> friend bits<Nb1> Carry (const bits<Nb1>& a,unsigned b, const bits<1>& cin);

    template <size_t Nb> friend std::istream& operator>>( std::istream& is, bits<Nb> &x);
    template <size_t Nb> friend std::ostream& operator<<( std::ostream& os, const bits<Nb> &x);


#ifdef IS_64BIT
  template <size_t Nb> friend bits<MAX(Nb,64)> operator+(const bits<Nb>& x,unsigned long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator+(unsigned long long int x,const bits<Nb>& y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator+(const bits<Nb>& x,long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator+(long long int x, const bits<Nb>& y);
  
  template <size_t Nb> friend bits<MAX(Nb,64)> operator-(const bits<Nb>& x,unsigned long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator-(unsigned long long int x,const bits<Nb>& y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator-(const bits<Nb>& x,long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator-(long long int x, const bits<Nb>& y);

  template <size_t Nb> friend bits<MAX(Nb,64)> operator*(const bits<Nb>& x,unsigned long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator*(unsigned long long int x,const bits<Nb>& y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator*(const bits<Nb>& x,long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator*(long long int x, const bits<Nb>& y);

  template <size_t Nb> friend bits<MAX(Nb,64)> operator/(const bits<Nb>& x,unsigned long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator/(unsigned long long int x,const bits<Nb>& y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator/(const bits<Nb>& x,long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator/(long long int x, const bits<Nb>& y);

  template <size_t Nb> friend bits<MAX(Nb,64)> operator%(const bits<Nb>& x,unsigned long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator%(unsigned long long int x,const bits<Nb>& y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator%(const bits<Nb>& x,long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator%(long long int x, const bits<Nb>& y);

  template <size_t Nb> friend bits<MAX(Nb,64)> operator&(const bits<Nb>& x,unsigned long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator&(unsigned long long int x,const bits<Nb>& y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator&(const bits<Nb>& x,long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator&(long long int x, const bits<Nb>& y);

  template <size_t Nb> friend bits<MAX(Nb,64)> operator|(const bits<Nb>& x,unsigned long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator|(unsigned long long int x,const bits<Nb>& y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator|(const bits<Nb>& x,long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator|(long long int x, const bits<Nb>& y);

  template <size_t Nb> friend bits<MAX(Nb,64)> operator^(const bits<Nb>& x,unsigned long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator^(unsigned long long int x,const bits<Nb>& y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator^(const bits<Nb>& x,long long int y);
  template <size_t Nb> friend bits<MAX(Nb,64)> operator^(long long int x, const bits<Nb>& y);

#endif




  protected:

# ifndef NO_PROPAGATE
    template <size_t _Nb2>
    void propagate_undefined(const bits<_Nb2>& x);
    template<size_t _Nb1,size_t _Nb2>
    void propagate_undefined(const bits<_Nb1>& x,const bits<_Nb2>& y);
    template<size_t _Nb1,size_t _Nb2,size_t _Nb3>
    void propagate_undefined(const bits<_Nb1>& x,const bits<_Nb2>& y,const bits<_Nb3>& z);
    template <size_t _Nb2>
    void union_undefined(const bits<_Nb2>& x);
    template<size_t _Nb1,size_t _Nb2>
    void union_undefined(const bits<_Nb1>& x,const bits<_Nb2>& y);

    Base _mask;
# endif

  };

}

#include "bits_impl.h"


#endif
