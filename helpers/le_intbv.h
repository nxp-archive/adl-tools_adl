//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

// 
// Bit-vector with big-endian bit numbering
//

#ifndef _LE_INTBV_H_
#define _LE_INTBV_H_
#include "intbv.h" 

namespace adl {

  template<size_t _Nb>
  class le_intbv : public intbv<_Nb>
  {
  public:
    template <size_t> friend  class le_intbv;
    typedef intbv<_Nb> Base;
    typedef le_intbv<_Nb> This;
    
    // constructors
    // All le_intbv set to zero.
    le_intbv() {};
    // Initial bits bitwise-copied from a single word (others set to zero).
    le_intbv(uint32_t val) : Base(val) {};
    le_intbv(uint64_t val) : Base(val) {};
    le_intbv(int32_t val)  : Base(val) {};
    le_intbv(int64_t val)  : Base(val) {};
#ifdef IS_64BIT
    le_intbv(unsigned long long val) : Base((uint64_t)val) {};
    le_intbv(long long int val) : Base((int64_t)val) {};
#else
    le_intbv(unsigned long val) : Base((uint32_t)val) {};
    le_intbv(long val) : Base((int32_t)val) {};
#endif
    le_intbv( const char* s) : Base(s) {};
    le_intbv( const std::string& s) : Base(s) {};
    le_intbv( const std::string& s, int format) : Base(s,format) {};
    le_intbv( const std::istream &is ) : Base(is) {};
# ifndef __NO_RNUMBER__
    le_intbv( const rnumber::RNumber &x ) : Base(x) {};
# endif

    template <size_t _Nb2>
      le_intbv (const intbv<_Nb2> &x): Base(x) {}    
    
    le_intbv (const intbv<_Nb> &x): Base(x) {}
 
    template <size_t _Nb2>
      le_intbv (const le_intbv<_Nb2> &x): Base(x) {}
        
    le_intbv (const le_intbv<_Nb> &x): Base(x) {}
        
    // ctor that takes a templated initialization functor
    template <typename R> 
      explicit le_intbv (unsigned x,const R &r) : Base(x, r) {}
        
    //////////////////
    //
    //  Getters/Setters/Masks
    //   
    
    
    // to set a field
    le_intbv<_Nb> &set (size_t start, size_t stop, const le_intbv<_Nb>& x) {
      assert(start >= stop);
      Base::set(be2le(start),be2le(stop),x);
      return *this;
    }
    
    template <size_t _Nb2>
    le_intbv<_Nb> &set (size_t start, size_t stop, const le_intbv<_Nb2>& x) {
      assert(start >= stop);
      Base::set(be2le(start),be2le(stop),x);
      return *this;
    }    
    
    template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
    le_intbv<_Nb> &set (const intbv<_Nb1> &start, const intbv<_Nb2> &stop, const le_intbv<_Nb3>& x) {
      set(start.uint32(),stop.uint32(),x);
      return *this;      
      
    }
  
    // sets the specified range to all ones.    
    le_intbv<_Nb> &mask (size_t start, size_t stop) {
      assert(start >= stop);
      Base::mask(be2le(start),be2le(stop));
      return *this;
    }
     
    template <size_t _Nb1,size_t _Nb2>
    le_intbv<_Nb> &mask (const intbv<_Nb1>& start, const intbv<_Nb2> &stop){
      assert(start >= stop);
      Base::mask(be2le(start),be2le(stop));
      return *this;
    }       

    // sets the specified range to all zeroes. 
    le_intbv<_Nb> &mask_inv (size_t start, size_t stop) {
      assert(start >= stop);
      Base::mask_inv(be2le(start),be2le(stop));
      return *this; 
    }
    
    template <size_t _Nb1,size_t _Nb2>
    le_intbv<_Nb> &mask_inv (const intbv<_Nb1>& start, const intbv<_Nb2>& stop) {
      Base::mask_inv(be2le(start),be2le(stop));
      return *this;      
    }  
      
    // to get a field/slice (returns same size intbv w/ value right justified
    const le_intbv<_Nb> operator () (size_t start, size_t stop) const {
      assert(start >= stop);
      return Base::get(be2le(start),be2le(stop));    
    }
  
    const le_intbv<_Nb> get (size_t start, size_t stop) const {
      assert(start >= stop);
      return Base::get(be2le(start),be2le(stop));     
    }
 
    template <size_t _Nb1,size_t _Nb2>
    const le_intbv<_Nb> get (const intbv<_Nb1>& start, const intbv<_Nb2>& stop) const {
      return Base::get(be2le(start),be2le(stop));
    }      
      
    template <size_t _Nb1,size_t _Nb2>
    const le_intbv<_Nb> operator () (const intbv<_Nb1>& start, const intbv<_Nb2>& stop) const {      
      return Base::get(be2le(start),be2le(stop)); 
    }  
        
    template <size_t _Nb2>
    const le_intbv<_Nb2> get (size_t start, size_t stop) const {      
      return Base::template get<_Nb2>(be2le(start),be2le(stop));       
    }
    
    template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
    const le_intbv<_Nb3> get (const intbv<_Nb1>& start,const intbv<_Nb2>& stop) const {      
      return Base::template get<_Nb3>(be2le(start),be2le(stop));      
    } 
 
     // single-bit set, reset, invert, and getBit (no range checking)
    le_intbv<_Nb>& _Unchecked_set(size_t pos) {
      Base::_Unchecked_set(be2le(pos));
      return *this;    
    }
    
    le_intbv<_Nb>& _Unchecked_set(size_t pos, int val) {
      Base::_Unchecked_set(be2le(pos),val);
      return *this;     
    };      

    le_intbv<_Nb>& _Unchecked_reset(size_t pos) {
      Base::_Unchecked_reset(be2le(pos));
      return *this;    
    }
    
    le_intbv<_Nb>& _Unchecked_invert(size_t pos) {
      Base::_Unchecked_invert(be2le(pos));
      return *this;      
    }   
    
    bool _Unchecked_test(size_t pos) const {    
      return Base::_Unchecked_invert(be2le(pos));    
    }
    
    // Set, reset, and invert.  
    // Sets every bit to true.
    le_intbv<_Nb>& set()
    {
      Base::set();
      return *this;
    }
    
    le_intbv<_Nb>& set(size_t position, bool val = true) {
      assert(position < _Nb);
      Base::set(be2le(position),val);
      return *this;    
    }

    template <size_t _Nb1>
    le_intbv<_Nb>& set(const le_intbv<_Nb1>& position, bool val = true) {
      Base::set(be2le(position), val);
      return *this;    
    }
        
    template <size_t _Nb1>
    le_intbv<_Nb>& set(size_t position,const le_intbv<_Nb1>& value) {
      Base::set(be2le(position), value);
      return *this;
    }   
    
    template <size_t _Nb1,size_t _Nb2>
    le_intbv<_Nb>& set(const le_intbv<_Nb1>& position,const le_intbv<_Nb2>& value) {
      Base::set(be2le(position), value);
      return *this;
    }
    
    using Base::reset;
    // Sets every bit to false.
    le_intbv<_Nb>& reset(size_t position) {
      assert(position < _Nb);
      Base::reset(be2le(position));
      return *this;     
    }       
    
    // Toggles every bit to its opposite value.
    le_intbv<_Nb>& invert() {
      Base::invert();
      return *this;
    }
    
    // Two's complement
    le_intbv<_Nb>& negate() {
      Base::negate();
      return *this;
    }
    
    // Toggles a given bit to its opposite value.
    le_intbv<_Nb>& invert(size_t position) {
      assert(position < _Nb);
      Base::_Unchecked_invert(be2le(position));
      return *this;
    }
     
    //    Array-indexing support.
    const le_intbv<1> operator[](size_t position) const {
      return Base::operator[](be2le(position));
    }
    
    template <size_t _Nb2>
    const le_intbv<1> operator[](const le_intbv<_Nb2>& position) const { 
      return Base::operator[](be2le(position)); 
    }

    const le_intbv<1> operator()(size_t position) const { 
      return Base::operator()(be2le(position)); 
    }
    
    template <size_t _Nb2>
    const le_intbv<1> operator()(const le_intbv<_Nb2>& position) const {
      return Base::operator()(be2le(position)); 
    };

    //   Tests the value of a bit.
    bool get(size_t position) const {
      assert(position < _Nb);
      return Base::get(be2le(position));
    }

    //    Finds the index of the first "on" bit.
    size_t _find_first() const
    { return Base::_find_last(_Nb); }
 
    template <size_t _Nb2> 
    le_intbv<_Nb2> signExtend() const {
      le_intbv<_Nb2> x = *this;
      if (_Nb2 > _Nb) {
        if (get(_Nb-1)) {
          x.mask(_Nb2-1,_Nb);
        } else {
	}
      }
      return x;
    }
    void signExtend(size_t bit) {
      Base::signExtend(be2le(bit));
    }
   
    /////////////
    //
    // Subtraction
    //   
    
    friend le_intbv<_Nb> operator-(const le_intbv<_Nb>& x, const le_intbv<_Nb>& y)
    {
      return  operator-(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb> >(y));
    }

    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator-(const le_intbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator-(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb2> >(y));
    }

    friend le_intbv<MAX(_Nb,64)> operator-(const le_intbv<_Nb>& x, uint64_t y)
    {
      return operator-(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,64)> operator-(uint64_t x,const le_intbv<_Nb>& y)
    {
      return operator-(x,static_cast<intbv<_Nb> >(y));
    }

    friend le_intbv<MAX(_Nb,32)> operator-(const le_intbv<_Nb>& x, uint32_t y)
    {
      return operator-(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,32)> operator-(uint32_t x,const le_intbv<_Nb>& y)
    {
      return operator-(x,static_cast<intbv<_Nb> >(y));
    }

    friend le_intbv<MAX(_Nb,32)> operator-(const le_intbv<_Nb>& x, int32_t y)
    {
      return operator-(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,32)> operator-(int32_t x,const le_intbv<_Nb>& y)
    {
      return operator-(x,static_cast<intbv<_Nb> >(y));
    }
   
    le_intbv<_Nb>& operator-=(uint64_t rhs)
    {
      Base::operator-=(rhs);
      return *this;
    }

    le_intbv<_Nb>& operator-=(int32_t rhs)
    {
      Base::operator-=(rhs);
      return *this;
    }

    le_intbv<_Nb>& operator-=(const le_intbv<_Nb> &rhs)
    {
      Base::operator-=(rhs);
      return *this;
    }
     
    ////////////////////
    //
    // Addition
    //

    friend le_intbv<_Nb> operator+(const le_intbv<_Nb>& x, const le_intbv<_Nb>& y)
    {
      return operator+(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb> >(y));
    }
    
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator+(const le_intbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator+(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb2> >(y));
    }

    friend le_intbv<MAX(_Nb,64)> operator+(const le_intbv<_Nb>& x, uint64_t y)
    {
      return operator+(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,64)> operator+(uint64_t x,const le_intbv<_Nb>& y)
    {
      return operator+(x,static_cast<intbv<_Nb> >(y));
    }
    
    friend le_intbv<MAX(_Nb,32)> operator+(const le_intbv<_Nb>& x, uint32_t y)
    {
      return operator+(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,32)> operator+(uint32_t x,const le_intbv<_Nb>& y)
    {
      return operator+(x,static_cast<intbv<_Nb> >(y));
    }
    
    friend le_intbv<MAX(_Nb,32)> operator+(const le_intbv<_Nb>& x, int32_t y)
    {
      return operator+(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,32)> operator+(int32_t x,const le_intbv<_Nb>& y)
    {
      return operator+(x,static_cast<intbv<_Nb> >(y));
    }
    

    le_intbv<_Nb>& operator+=(uint64_t rhs)
    {
      Base::operator+=(rhs);
      return *this;
    }

    le_intbv<_Nb>& operator+=(int32_t rhs)
    {
      Base::operator+=(rhs);
      return *this;
    }

    le_intbv<_Nb>& operator+=(const le_intbv<_Nb> &rhs)
    {
      Base::operator+=(rhs);
      return *this;
    }   
  
    /////////////////////
    //
    //  Comparisons
    //
    
    // == 
    bool operator==(const le_intbv<_Nb>& rhs) const { 
      return Base::operator==(rhs); 
    }
    bool operator==(int32_t rhs) const { 
      return operator==(static_cast<le_intbv<32> >(rhs));  
    }
    bool operator==(uint32_t rhs) const { 
      return operator==(static_cast<le_intbv<32> >(rhs));  
    }
    bool operator==(uint64_t rhs) const { 
      return operator==(static_cast<le_intbv<64> >(rhs));  
    }
    bool operator==(int64_t rhs) const { 
      return operator==(static_cast<le_intbv<64> >(rhs));  
    }
   
    template <size_t _Nb2>
    bool operator==(const le_intbv<_Nb2>& rhs) const {
      return Base::operator==(rhs);
    }    
    // !=    
    bool operator!=(const le_intbv<_Nb>& rhs) const { 
      return Base::operator!=(rhs);
    }
    bool operator!=(int32_t rhs) const { 
      return operator!=(static_cast<le_intbv<32> >(rhs));  
    }
    bool operator!=(uint32_t rhs) const { 
      return operator!=(static_cast<le_intbv<32> >(rhs));  
    }
    bool operator!=(uint64_t rhs) const { 
      return operator!=(static_cast<le_intbv<64> >(rhs));  
    }
    bool operator!=(int64_t rhs) const { 
      return operator!=(static_cast<le_intbv<64> >(rhs));  
    }
    template <size_t _Nb2>
    bool operator!=(const le_intbv<_Nb2>& rhs) const { 
      return Base::operator!=(rhs);
    }    
      
    // <=  
      
    template <size_t _Nb2>
    bool operator<=(const le_intbv<_Nb2>& rhs) const { 
      return Base::operator<=(rhs);
    }
    bool operator<=(const le_intbv<_Nb>& rhs) const { 
      return Base::operator<=(rhs);
    }
    bool operator<=(int32_t rhs) const { 
      return operator<=(static_cast<le_intbv<32> >(rhs));  
    }
    bool operator<=(uint32_t rhs) const { 
      return operator<=(static_cast<le_intbv<32> >(rhs));  
    }
    bool operator<=(uint64_t rhs) const { 
      return operator<=(static_cast<le_intbv<64> >(rhs));  
    }
    bool operator<=(int64_t rhs) const { 
      return operator<=(static_cast<le_intbv<64> >(rhs));  
    }
 
    // >=
    template <size_t _Nb2>
    bool operator>=(const le_intbv<_Nb2>& rhs) const { 
      return Base::operator>=(rhs);
    }
    bool operator>=(const le_intbv<_Nb>& rhs) const { 
      return Base::operator>=(rhs);
    }
    bool operator>=(int32_t rhs) const { 
      return operator>=(static_cast<le_intbv<32> >(rhs));  
    }
    bool operator>=(uint32_t rhs) const { 
      return operator>=(static_cast<le_intbv<32> >(rhs));  
    }
    bool operator>=(uint64_t rhs) const { 
      return operator>=(static_cast<le_intbv<64> >(rhs));  
    }
    bool operator>=(int64_t rhs) const { 
      return operator>=(static_cast<le_intbv<64> >(rhs));  
    }
    // <
    template <size_t _Nb2>
    bool operator<(const le_intbv<_Nb2>& rhs) const { 
      return Base::operator<(rhs);
    }
    bool operator<(const le_intbv<_Nb>& rhs) const { 
      return Base::operator<(rhs);
    }
    bool operator<(int32_t rhs) const { 
      return operator<(static_cast<le_intbv<32> >(rhs));  
    }
    bool operator<(uint32_t rhs) const { 
      return operator<(static_cast<le_intbv<32> >(rhs));  
    }
    bool operator<(uint64_t rhs) const { 
      return operator<(static_cast<le_intbv<64> >(rhs));  
    }
    bool operator<(int64_t rhs) const { 
      return operator<(static_cast<le_intbv<64> >(rhs));  
    }
    // >
    template <size_t _Nb2>
    bool operator>(const le_intbv<_Nb2>& rhs) const { 
      return Base::operator>(rhs);
    }
    bool operator>(const le_intbv<_Nb>& rhs) const { 
      return Base::operator>(rhs);
    }
    bool operator>(int32_t rhs) const { 
      return operator>(static_cast<le_intbv<32> >(rhs));  
    }
    bool operator>(uint32_t rhs) const { 
      return operator>(static_cast<le_intbv<32> >(rhs));  
    }
    bool operator>(uint64_t rhs) const { 
      return operator>(static_cast<le_intbv<64> >(rhs));  
    }
    bool operator>(int64_t rhs) const { 
      return operator>(static_cast<le_intbv<64> >(rhs));  
    }
#ifdef IS_64BIT
    bool operator==(long long unsigned rhs) const { 
      return operator==(static_cast<le_intbv<64> >(rhs)); 
    }
    bool operator!=(long long unsigned rhs) const { 
      return operator!=(static_cast<le_intbv<64> >(rhs)); 
    }
#endif              
 
    ///////////////////////
    //
    //   Multiplication
    //

    friend le_intbv<_Nb> operator*(const le_intbv<_Nb>& x, const le_intbv<_Nb>& y)
    {
      return operator*(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb> >(y));
    }

    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator*(const le_intbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator*(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb2> >(y));
    }

    friend le_intbv<MAX(_Nb,64)> operator*(const le_intbv<_Nb>& x, uint64_t y)
    {
      return operator*(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,64)> operator*(uint64_t x,const le_intbv<_Nb>& y)
    {
      return operator*(x,static_cast<intbv<_Nb> >(y));
    }

    friend le_intbv<MAX(_Nb,32)> operator*(const le_intbv<_Nb>& x, uint32_t y)
    {
      return operator*(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,32)> operator*(uint32_t x,const le_intbv<_Nb>& y)
    {
      return operator*(x,static_cast<intbv<_Nb> >(y));
    }

    friend le_intbv<MAX(_Nb,32)> operator*(const le_intbv<_Nb>& x, int32_t y)
    {
      return operator*(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,32)> operator*(int32_t x,const le_intbv<_Nb>& y)
    {
      return operator*(x,static_cast<intbv<_Nb> >(y));
    }
 
    le_intbv<_Nb>& operator*=(uint64_t rhs)
    {
      Base::operator*=(rhs);
      return *this;
    }

    le_intbv<_Nb>& operator*=(int32_t rhs)
    {
      Base::operator*=(rhs);
      return *this;
    }

    le_intbv<_Nb>& operator*=(const le_intbv<_Nb> &rhs)
    {
      Base::operator*=(rhs);
      return *this;
    }
 
 
 
 
 
  
    ///////////////////////
    //
    //   Division
    //
    friend le_intbv<_Nb> operator/(const le_intbv<_Nb>& x, const le_intbv<_Nb>& y)
    {
      return operator/(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb> >(y));
    }

    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator/(const le_intbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator/(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb2> >(y));
    }

    friend le_intbv<MAX(_Nb,64)> operator/(const le_intbv<_Nb>& x, uint64_t y)
    {
      return operator/(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,64)> operator/(uint64_t x,const le_intbv<_Nb>& y)
    {
      return operator/(x,static_cast<intbv<_Nb> >(y));;
    }
 
    friend le_intbv<MAX(_Nb,32)> operator/(const le_intbv<_Nb>& x, uint32_t y)
    {
      return operator/(static_cast<intbv<_Nb> >(x),y);
    }
    
    friend le_intbv<MAX(_Nb,32)> operator/(uint32_t x,const le_intbv<_Nb>& y)
    {
      return operator/(x,static_cast<intbv<_Nb> >(y));
    }

    friend le_intbv<MAX(_Nb,32)> operator/(const le_intbv<_Nb>& x, int32_t y)
    {
      return operator/(static_cast<intbv<_Nb> >(x),(uint32_t)y);
    }

    friend le_intbv<MAX(_Nb,32)> operator/(int32_t x,const le_intbv<_Nb>& y)
    {
      return operator/(x,static_cast<intbv<_Nb> >(y));
    }    
    
   /* le_intbv<_Nb>& signedDivideEqual(const le_intbv<_Nb>& rhs) {
      Base::signedDivideEqual(rhs);
      return *this;
    } 
    le_intbv<_Nb>& signedDivideEqual(uint64_t rhs) {
      Base::signedDivideEqual(rhs);
      return *this;    
    }   
    friend le_intbv<_Nb> signedDivide(const le_intbv<_Nb> &x,const le_intbv<_Nb> &y) {
      return static_cast<This>(Base::signedDivide(static_cast<Base>(x), static_cast<Base>(y)));
    }
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> signedDivide(const le_intbv<_Nb>& x, const le_intbv<_Nb2>& y) {
      return static_cast<le_intbv<MAX(_Nb,_Nb2)> >(Base::signedDivide(static_cast<le_intbv<_Nb> >(x), static_cast<le_intbv<_Nb2> >(y)));
    }    
    friend le_intbv<MAX(_Nb,64)> signedDivide(const le_intbv<_Nb>& x, uint64_t y) {
      return static_cast<le_intbv<MAX(_Nb,64)> >(Base::signedDivide(static_cast<le_intbv<MAX(_Nb,64)> >(x), static_cast<le_intbv<MAX(_Nb,64)> >(y)));
    }
    friend le_intbv<MAX(_Nb,64)> signedDivide(uint64_t x, const le_intbv<_Nb>& y) {
      return static_cast<le_intbv<MAX(_Nb,64)> >(Base::signedDivide(static_cast<le_intbv<MAX(_Nb,64)> >(x), static_cast<le_intbv<MAX(_Nb,64)> >(y)));
    }
        
    */
    le_intbv<_Nb>& operator/=(uint64_t rhs)
    {
      Base::operator/=(rhs);
      return *this;
    }

    le_intbv<_Nb>& operator/=(int32_t rhs)
    {
      Base::operator/=(rhs);
      return *this;
    }

    le_intbv<_Nb>& operator/=(const le_intbv<_Nb> &rhs)
    {
      Base::operator/=(rhs);
      return *this;
    }  
  
    template <size_t _Nb2>
    le_intbv<_Nb>& operator/=(const le_intbv<_Nb2>& rhs){
      Base::operator/=(rhs);
      return *this;
    } 
  
  
    //////////////
    //
    //   Modulus
    //
    
    friend le_intbv<_Nb> operator%(const le_intbv<_Nb>& x, const le_intbv<_Nb>& y)
    {
      return operator%(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb> >(y));
    }

    friend le_intbv<MAX(_Nb,64)> operator%(const le_intbv<_Nb>& x, uint64_t y)
    {
      return operator%(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,64)> operator%(uint64_t x,const le_intbv<_Nb>& y)
    {
      return operator%(x,static_cast<intbv<_Nb> >(y));
    }

    friend le_intbv<MAX(_Nb,32)> operator%(const le_intbv<_Nb>& x, uint32_t y)
    {
      return operator%(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,32)> operator%(uint32_t x,const le_intbv<_Nb>& y)
    {
      return operator%(x,static_cast<intbv<_Nb> >(y));
    }

    friend le_intbv<MAX(_Nb,32)> operator%(const le_intbv<_Nb>& x, int32_t y)
    {
      return operator%(static_cast<intbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,32)> operator%(int32_t x,const le_intbv<_Nb>& y)
    {
      return operator%(x,static_cast<intbv<_Nb> >(y));
    }

    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator%(const le_intbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator%(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb2> >(y));
    }
    
    le_intbv<_Nb>& operator%=(const le_intbv<_Nb>& rhs) {
      Base::operator%=(rhs);
      return *this;
    }

    template <size_t _Nb2>
    le_intbv<_Nb>& operator%=(const le_intbv<_Nb2>& rhs){
      Base::operator%=(rhs);
      return *this;
    }      
   
    le_intbv<_Nb>& operator%=(uint64_t rhs)
    {
      Base::operator%=(rhs);
      return *this;
    }

    le_intbv<_Nb>& operator%=(int32_t rhs)
    {
      Base::operator%=(rhs);
      return *this;
    }


    
    ////////////////////////////////////////
    //
    //     Logical
    //  
      
    le_intbv<_Nb>& operator&=(const le_intbv<_Nb>& rhs)
    {
      Base::operator&=(rhs); 
      return *this;
    }

    le_intbv<_Nb>& operator|=(const le_intbv<_Nb>& rhs)
    {
      Base::operator|=(rhs); 
      return *this;
    }

    le_intbv<_Nb>& operator^=(const le_intbv<_Nb>& rhs)
    {
      Base::operator^=(rhs); 
      return *this;
    }   
      
  
    friend le_intbv<_Nb> operator&(const le_intbv<_Nb>& x, const le_intbv<_Nb>& y)
    {
      return operator&(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb> >(y));
    }
 ////////////////// 
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator&(const le_intbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator&(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb2> >(y));
    } 
    friend le_intbv<MAX(_Nb,64)> operator&(const le_intbv<_Nb>& x,uint64_t y)
    {
      return operator&(static_cast<intbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,64)> operator&(uint64_t x, const le_intbv<_Nb>& y)
    {
      return operator&(x,static_cast<intbv<_Nb> >(y));
    }  
    
    friend le_intbv<MAX(_Nb,32)> operator&(const le_intbv<_Nb>& x,int32_t y)
    {
      return operator&(static_cast<intbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,32)> operator&(int32_t x, const le_intbv<_Nb>& y)
    {
      return operator&(x,static_cast<intbv<_Nb> >(y));
    } 
   
    friend le_intbv<MAX(_Nb,32)> operator&(const le_intbv<_Nb>& x,uint32_t y)
    {
      return operator&(static_cast<intbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,32)> operator&(uint32_t x, const le_intbv<_Nb>& y)
    {
      return operator&(x,static_cast<intbv<_Nb> >(y));
    }   
     
    friend le_intbv<MAX(_Nb,64)> operator&(const le_intbv<_Nb>& x,int64_t y)
    {
      return operator&(static_cast<intbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,64)> operator&(int64_t x, const le_intbv<_Nb>& y)
    {
      return operator&(x,static_cast<intbv<_Nb> >(y));
    }

////////////////////////
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator^(const le_intbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator^(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb2> >(y));
    } 
  
    friend le_intbv<MAX(_Nb,32)> operator^(const le_intbv<_Nb>& x,int32_t y)
    {
      return operator^(static_cast<intbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,32)> operator^(int32_t x, const le_intbv<_Nb>& y)
    {
      return operator^(x,static_cast<intbv<_Nb> >(y));
    } 
   
    friend le_intbv<MAX(_Nb,32)> operator^(const le_intbv<_Nb>& x,uint32_t y)
    {
      return operator^(static_cast<intbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,32)> operator^(uint32_t x, const le_intbv<_Nb>& y)
    {
      return operator^(x,static_cast<intbv<_Nb> >(y));
    }  
    friend le_intbv<MAX(_Nb,64)> operator^(const le_intbv<_Nb>& x,uint64_t y)
    {
      return operator^(static_cast<intbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,64)> operator^(uint64_t x, const le_intbv<_Nb>& y)
    {
      return operator^(x,static_cast<intbv<_Nb> >(y));
    }   
  
    // |
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator|(const le_intbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator|(static_cast<intbv<_Nb> >(x),static_cast<intbv<_Nb2> >(y));
    } 
  
    friend le_intbv<MAX(_Nb,32)> operator|(const le_intbv<_Nb>& x,int32_t y)
    {
      return operator|(static_cast<intbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,32)> operator|(int32_t x, const le_intbv<_Nb>& y)
    {
      return operator|(x,static_cast<intbv<_Nb> >(y));
    } 
   
    friend le_intbv<MAX(_Nb,32)> operator|(const le_intbv<_Nb>& x,uint32_t y)
    {
      return operator|(static_cast<intbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,32)> operator|(uint32_t x, const le_intbv<_Nb>& y)
    {
      return operator|(x,static_cast<intbv<_Nb> >(y));
    } 
 
    friend le_intbv<MAX(_Nb,64)> operator|(const le_intbv<_Nb>& x,uint64_t y)
    {
      return operator|(x,static_cast<intbv<_Nb> >(y));
    }   
  
    friend le_intbv<MAX(_Nb,64)> operator|(uint64_t x, const le_intbv<_Nb>& y)
    {
      return operator|(x,static_cast<intbv<_Nb> >(y));
    } 
 
    le_intbv<_Nb> operator~() const { return le_intbv<_Nb>(*this).invert(); }
    
    le_intbv<_Nb> swap_bytes() const {
      return static_cast<le_intbv<_Nb> >(Base::swap_bytes());
    }

    le_intbv<_Nb> swap_words() const {
      return static_cast<le_intbv<_Nb> >(Base::swap_words());
    }


    ////////////////////
    //
    //  Shift/Rotate
    //    
    
    le_intbv<_Nb>& operator<<=(size_t position) {
      Base::operator<<=(position);
      return *this;
    }

    template <size_t _Nb2>
    le_intbv<_Nb>& operator<<=(const le_intbv<_Nb2>& position)
    {
      Base::operator<<=(position);
      return *this;   
    }

    le_intbv<_Nb>& operator>>=(size_t position)
    {
      Base::operator>>=(position);
      return *this;
    }

    template <size_t _Nb2>
    le_intbv<_Nb>& operator>>=(const le_intbv<_Nb2>& position)
    {
      Base::operator>>=(position);
      return *this;
    }

    template <size_t _Nb2>
    le_intbv<_Nb> left_rotate(const le_intbv<_Nb2>& position) const {
      return Base::left_rotate(position);
    }

    le_intbv<_Nb> left_rotate (size_t position) const
    {
      return Base::left_rotate(position);
    }

    template <size_t _Nb2>
    le_intbv<_Nb> right_rotate(const le_intbv<_Nb2>& position) const
    {   
      return Base::right_rotate(position);
    }   

    le_intbv<_Nb> right_rotate (size_t position) const 
    {
      return Base::right_rotate(position);
    }

    le_intbv<_Nb> operator<<(size_t position) const
    { 
      return Base::operator<<(position); 
    }

    template<size_t _Nb2>
    le_intbv<_Nb> operator<<(const le_intbv<_Nb2>& position) const
    { 
      return Base::operator<<(position); 
    }

    template<size_t _Nb2>
    le_intbv<_Nb> operator>>(const le_intbv<_Nb2>& position) const
    { 
      return Base::operator>>(position); 
    }

    le_intbv<_Nb> operator>>(size_t position) const
    { 
      return Base::operator>>(position); 
    }    

 
 
// allows to use "ull" after numeric constant 
#ifdef IS_64BIT
    friend le_intbv<MAX(_Nb,64)> operator&(const le_intbv<_Nb>& x,unsigned long long y) {
      return operator&(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator&(unsigned long long  x, const le_intbv<_Nb>& y) {
      return operator&(x,static_cast<intbv<_Nb> >(y));
    } 
    friend le_intbv<MAX(_Nb,64)> operator&(const le_intbv<_Nb>& x,long long y) {
      return operator&(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator&(long long  x, const le_intbv<_Nb>& y) {
      return operator&(x,static_cast<intbv<_Nb> >(y));
    } 

    friend le_intbv<MAX(_Nb,64)> operator^(const le_intbv<_Nb>& x,unsigned long long y) {
      return operator^(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator^(unsigned long long  x, const le_intbv<_Nb>& y) {
      return operator^(x,static_cast<intbv<_Nb> >(y));
    } 
    friend le_intbv<MAX(_Nb,64)> operator^(const le_intbv<_Nb>& x,long long y) {
      return operator^(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator^(long long  x, const le_intbv<_Nb>& y) {
      return operator^(x,static_cast<intbv<_Nb> >(y));
    } 

    friend le_intbv<MAX(_Nb,64)> operator|(const le_intbv<_Nb>& x,unsigned long long y) {
      return operator|(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator|(unsigned long long  x, const le_intbv<_Nb>& y) {
      return operator|(x,static_cast<intbv<_Nb> >(y));
    } 
    friend le_intbv<MAX(_Nb,64)> operator|(const le_intbv<_Nb>& x,long long y) {
      return operator|(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator|(long long  x, const le_intbv<_Nb>& y) {
      return operator|(x,static_cast<intbv<_Nb> >(y));
    } 

    friend le_intbv<MAX(_Nb,64)> operator+(const le_intbv<_Nb>& x,unsigned long long y) {
      return operator+(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator+(unsigned long long  x, const le_intbv<_Nb>& y) {
      return operator+(x,static_cast<intbv<_Nb> >(y));
    } 
    friend le_intbv<MAX(_Nb,64)> operator+(const le_intbv<_Nb>& x,long long y) {
      return operator+(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator+(long long  x, const le_intbv<_Nb>& y) {
      return operator+(x,static_cast<intbv<_Nb> >(y));
    } 

    friend le_intbv<MAX(_Nb,64)> operator-(const le_intbv<_Nb>& x,unsigned long long y) {
      return operator-(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator-(unsigned long long  x, const le_intbv<_Nb>& y) {
      return operator-(x,static_cast<intbv<_Nb> >(y));
    } 
    friend le_intbv<MAX(_Nb,64)> operator-(const le_intbv<_Nb>& x,long long y) {
      return operator-(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator-(long long  x, const le_intbv<_Nb>& y) {
      return operator-(x,static_cast<intbv<_Nb> >(y));
    } 

    friend le_intbv<MAX(_Nb,64)> operator*(const le_intbv<_Nb>& x,unsigned long long y) {
      return operator*(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator*(unsigned long long  x, const le_intbv<_Nb>& y) {
      return operator*(x,static_cast<intbv<_Nb> >(y));
    } 
    friend le_intbv<MAX(_Nb,64)> operator*(const le_intbv<_Nb>& x,long long y) {
      return operator*(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator*(long long  x, const le_intbv<_Nb>& y) {
      return operator*(x,static_cast<intbv<_Nb> >(y));
    } 

    friend le_intbv<MAX(_Nb,64)> operator/(const le_intbv<_Nb>& x,unsigned long long y) {
      return operator/(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator/(unsigned long long  x, const le_intbv<_Nb>& y) {
      return operator/(x,static_cast<intbv<_Nb> >(y));
    } 
    friend le_intbv<MAX(_Nb,64)> operator/(const le_intbv<_Nb>& x,long long y) {
      return operator/(static_cast<intbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator/(long long  x, const le_intbv<_Nb>& y) {
      return operator/(x,static_cast<intbv<_Nb> >(y));
    } 

   
#endif

  //
  // Concatenation.
  //

  template <size_t _Nb2>
  friend le_intbv< _Nb + _Nb2> concat (const le_intbv<_Nb>& x, const le_intbv<_Nb2>& y)
  {
    return concat(static_cast<intbv<_Nb> >(x),
                  static_cast<intbv<_Nb2> >(y));
  }

  friend le_intbv< _Nb + _INTBV_BITS_PER_WORD> concat (const le_intbv<_Nb>& x, unsigned y)
  {
   le_intbv<_INTBV_BITS_PER_WORD> tmp_y (y);
   return concat(x,y); 
  }

  template <size_t _Nb2, size_t _Nb3>
  friend le_intbv<_Nb+_Nb2+_Nb3> concat (const le_intbv<_Nb>& x, const le_intbv<_Nb2>& y,const  le_intbv<_Nb3>& z)
  {

    return concat(static_cast<intbv<_Nb> >(x),
                  static_cast<intbv<_Nb2> >(y),
                  static_cast<intbv<_Nb3> >(z)
                  );
  }

  template <size_t _Nb2, size_t _Nb3,size_t _Nb4>
  friend le_intbv<_Nb+_Nb2+_Nb3+_Nb4> concat (const le_intbv<_Nb>& w, const le_intbv<_Nb2>& x,const le_intbv<_Nb3>& y,const le_intbv<_Nb4>& z)
  {
    return concat(static_cast<intbv<_Nb> >(w),
                  static_cast<intbv<_Nb2> >(x),
                  static_cast<intbv<_Nb3> >(y),
                  static_cast<intbv<_Nb4> >(z)
                  );
  }

  template <size_t _Nb2, size_t _Nb3,size_t _Nb4,size_t _Nb5>
  friend le_intbv<_Nb+_Nb2+_Nb3+_Nb4+_Nb5> concat (const le_intbv<_Nb>& v,const  le_intbv<_Nb2>& w,const  le_intbv<_Nb3>& x,const le_intbv<_Nb4>& y,const le_intbv<_Nb5>& z)
  {
    return concat(static_cast<intbv<_Nb> >(v),
                  static_cast<intbv<_Nb2> >(w),
                  static_cast<intbv<_Nb3> >(x),
                  static_cast<intbv<_Nb4> >(y),
                  static_cast<intbv<_Nb5> >(z)
                  );
  }

  template <size_t _Nb2, size_t _Nb3,size_t _Nb4,size_t _Nb5,size_t _Nb6>
  friend le_intbv<_Nb+_Nb2+_Nb3+_Nb4+_Nb5+_Nb6> concat (const le_intbv<_Nb>& u,const  le_intbv<_Nb2>& v, const le_intbv<_Nb3>& w,const le_intbv<_Nb4>& x,const le_intbv<_Nb5>& y, const le_intbv<_Nb6>& z)
  {
    return concat(static_cast<intbv<_Nb> >(u),
                  static_cast<intbv<_Nb2> >(v),
                  static_cast<intbv<_Nb3> >(w),
                  static_cast<intbv<_Nb4> >(x),
                  static_cast<intbv<_Nb5> >(y),
                  static_cast<intbv<_Nb6> >(z)
                  );
  }    
    
  private:
    template <size_t _Nb1>
    size_t be2le(intbv<_Nb1> pos) const {
      return _Nb - pos.uint32() - 1;
    }    
    
    size_t be2le(unsigned pos) const {
      return _Nb - pos - 1;
    }

  };
  
  
};

#endif
