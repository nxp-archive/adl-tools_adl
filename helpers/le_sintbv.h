//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

// 
// Bit-vector with big-endian bit numbering
//

#ifndef _LE_SINTBV_H_
#define _LE_SINTBV_H_
#include "sintbv.h"
#include "le_intbv.h"

namespace adl {

  template<size_t _Nb>
  class le_sintbv : public sintbv<_Nb>
  {
  public:
    template <size_t> friend  class le_sintbv;
    typedef sintbv<_Nb> Base;
    
    // constructors
    // All bits set to zero.
    le_sintbv() {};
    // Initial bits bitwise-copied from a single word (others set to zero).
    le_sintbv(uint32_t val) : Base(val) {};
    le_sintbv(uint64_t val) : Base(val) {};
    le_sintbv(int32_t val)  : Base(val) {};
    le_sintbv(int64_t val)  : Base(val) {};
#ifdef IS_64BIT
    le_sintbv(unsigned long long val) : Base((uint64_t)val) {};
    le_sintbv(long long int val) : Base((int64_t)val) {};
#else
    le_sintbv(unsigned long val) : Base((uint32_t)val) {};
    le_sintbv(long val) : Base((int32_t)val) {};
#endif
    le_sintbv( const char* s) : Base(s) {};
    le_sintbv( const std::string& s) : Base(s) {};
    le_sintbv( const std::string& s, int format) : Base(s,format) {};
    le_sintbv( const std::istream &is ) : Base(is) {};
# ifndef __NO_RNUMBER__
    le_sintbv( const rnumber::RNumber &x ) : Base(x) {};
# endif
   
    template <size_t _Nb2>
    le_sintbv (const sintbv<_Nb2> &x): Base(x) {}    
    
    le_sintbv (const sintbv<_Nb> &x): Base(x) {}

    template <size_t _Nb2>
    le_sintbv (const intbv<_Nb2> &x): Base(x) {}    
    
    le_sintbv (const intbv<_Nb> &x): Base(x) {}        
    
    template <size_t _Nb2>
    le_sintbv (const le_intbv<_Nb2> &x): Base(x) {}    
    
    le_sintbv (const le_intbv<_Nb> &x): Base(x) {}
    
    template <size_t _Nb2>
    le_sintbv (const le_sintbv<_Nb2> &x): Base(x) {}
        
    le_sintbv (const le_sintbv<_Nb> &x): Base(x) {}
        
    // ctor that takes a templated initialization functor
    template <typename R> 
      explicit le_sintbv (unsigned x,const R &r) : Base(x, r) {}
        
    //////////////////
    //
    //  Getters/Setters/Masks
    //   
    
    
    // to set a field
    le_sintbv<_Nb> &set (size_t start, size_t stop, const le_sintbv<_Nb>& x) {
      assert(start >= stop);
      Base::set(be2le(start),be2le(stop),x);
      return *this;
    }
    
    template <size_t _Nb2>
    le_sintbv<_Nb> &set (size_t start, size_t stop, const le_sintbv<_Nb2>& x) {
      assert(start >= stop);
      Base::set(be2le(start),be2le(stop),x);
      return *this;
    }    
    
    template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
    le_sintbv<_Nb> &set (const intbv<_Nb1> &start, const intbv<_Nb2> &stop, const le_sintbv<_Nb3>& x) {
      set(start.uint32(),stop.uint32(),x);
      return *this;      
      
    }
  
    // sets the specified range to all ones.    
    le_sintbv<_Nb> &mask (size_t start, size_t stop) {
      assert(start >= stop);
      Base::mask(be2le(start),be2le(stop));
      return *this;
    }
     
    template <size_t _Nb1,size_t _Nb2>
    le_sintbv<_Nb> &mask (const intbv<_Nb1>& start, const intbv<_Nb2> &stop){
      assert(start >= stop);
      Base::mask(be2le(start),be2le(stop));
      return *this;
    }       

    // sets the specified range to all zeroes. 
    le_sintbv<_Nb> &mask_inv (size_t start, size_t stop) {
      assert(start >= stop);
      Base::mask_inv(be2le(start),be2le(stop));
      return *this; 
    }
    
    template <size_t _Nb1,size_t _Nb2>
    le_sintbv<_Nb> &mask_inv (const intbv<_Nb1>& start, const intbv<_Nb2>& stop) {
      Base::mask_inv(be2le(start),be2le(stop));
      return *this;      
    }  
      
    // to get a field/slice (returns same size intbv w/ value right justified
    const le_sintbv<_Nb> operator () (size_t start, size_t stop) const {
      assert(start >= stop);
      return Base::get(be2le(start),be2le(stop));    
    }
  
    const le_sintbv<_Nb> get (size_t start, size_t stop) const {
      assert(start >= stop);
      return Base::get(be2le(start),be2le(stop));     
    }
 
    template <size_t _Nb1,size_t _Nb2>
    const le_sintbv<_Nb> get (const intbv<_Nb1>& start, const intbv<_Nb2>& stop) const {
      return Base::get(be2le(start),be2le(stop));
    }      
      
    template <size_t _Nb1,size_t _Nb2>
    const le_sintbv<_Nb> operator () (const intbv<_Nb1>& start, const intbv<_Nb2>& stop) const {      
      return Base::get(be2le(start),be2le(stop)); 
    }  
        
    template <size_t _Nb2>
    const le_sintbv<_Nb2> get (size_t start, size_t stop) const {      
      return Base::template get<_Nb2>(be2le(start),be2le(stop));       
    }
    
    template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
    const le_sintbv<_Nb3> get (const intbv<_Nb1>& start,const intbv<_Nb2>& stop) const {      
      return Base::template get<_Nb3>(be2le(start),be2le(stop));      
    } 
 
     // single-bit set, reset, invert, and getBit (no range checking)
    le_sintbv<_Nb>& _Unchecked_set(size_t pos) {
      Base::_Unchecked_set(be2le(pos));
      return *this;    
    }
    
    le_sintbv<_Nb>& _Unchecked_set(size_t pos, int val) {
      Base::_Unchecked_set(be2le(pos),val);
      return *this;     
    };      

    le_sintbv<_Nb>& _Unchecked_reset(size_t pos) {
      Base::_Unchecked_reset(be2le(pos));
      return *this;    
    }
    
    le_sintbv<_Nb>& _Unchecked_invert(size_t pos) {
      Base::_Unchecked_invert(be2le(pos));
      return *this;      
    }   
    
    bool _Unchecked_test(size_t pos) const {    
      return Base::_Unchecked_invert(be2le(pos));    
    }
    
    // Set, reset, and invert.  
    // Sets every bit to true.
    le_sintbv<_Nb>& set()
    {
      Base::set();
      return *this;
    }
    
    le_sintbv<_Nb>& set(size_t position, bool val = true) {
      assert(position < _Nb);
      Base::set(be2le(position),val);
      return *this;    
    }

    template <size_t _Nb1>
    le_sintbv<_Nb>& set(const le_sintbv<_Nb1>& position, bool val = true) {
      Base::set(be2le(position), val);
      return *this;    
    }
        
    template <size_t _Nb1>
    le_sintbv<_Nb>& set(size_t position,const le_sintbv<_Nb1>& value) {
      Base::set(be2le(position), value);
      return *this;
    }   
    
    template <size_t _Nb1,size_t _Nb2>
    le_sintbv<_Nb>& set(const le_sintbv<_Nb1>& position,const le_sintbv<_Nb2>& value) {
      Base::set(be2le(position), value);
      return *this;
    }
    
    using Base::reset;
    // Sets every bit to false.
    le_sintbv<_Nb>& reset(size_t position) {
      assert(position < _Nb);
      Base::reset(be2le(position));
      return *this;     
    }       
    
    // Toggles every bit to its opposite value.
    le_sintbv<_Nb>& invert() {
      Base::invert();
      return *this;
    }
    
    // Two's complement
    le_sintbv<_Nb>& negate() {
      Base::negate();
      return *this;
    }

    // Unary minus
    const le_sintbv<_Nb> operator-() const {
      return Base::operator-();
    }
    
    
    // Toggles a given bit to its opposite value.
    le_sintbv<_Nb>& invert(size_t position) {
      assert(position < _Nb);
      Base::_Unchecked_invert(be2le(position));
      return *this;
    }
     
    //    Array-indexing support.
    const le_intbv<1> operator[](size_t position) const {
      return Base::operator[](be2le(position));
    }
    
    template <size_t _Nb2>
    const le_intbv<1> operator[](const le_sintbv<_Nb2>& position) const { 
      return Base::operator[](be2le(position)); 
    }

    const le_intbv<1> operator()(size_t position) const { 
      return Base::operator()(be2le(position)); 
    }
    
    template <size_t _Nb2>
    const le_intbv<1> operator()(const le_sintbv<_Nb2>& position) const {
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
    le_sintbv<_Nb2> signExtend() const {
      le_sintbv<_Nb2> x = *this;
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
    
    friend le_sintbv<_Nb> operator-(const le_sintbv<_Nb>& x, const le_sintbv<_Nb>& y)
    {
      return  static_cast<le_sintbv<_Nb> >(static_cast<sintbv<_Nb> >(x) - static_cast<sintbv<_Nb> >(y));
    }

    template<size_t _Nb2>
    friend le_sintbv<MAX(_Nb,_Nb2)> operator-(const le_sintbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return (static_cast<sintbv<_Nb> >(x) - static_cast<sintbv<_Nb2> >(y));
    }

    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator-(const le_intbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return (static_cast<sintbv<_Nb> >(x) - static_cast<sintbv<_Nb2> >(y));
    }
    
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator-(const le_sintbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return (static_cast<sintbv<_Nb> >(x) - static_cast<sintbv<_Nb2> >(y));
    }
     
    friend le_intbv<MAX(_Nb,64)> operator-(const le_sintbv<_Nb>& x, uint64_t y)
    {
      return static_cast<sintbv<_Nb> >(x)-y;
    }

    friend le_intbv<MAX(_Nb,64)> operator-(uint64_t x,const le_sintbv<_Nb>& y)
    {
      return x-static_cast<sintbv<_Nb> >(y);
    }

    friend le_intbv<MAX(_Nb,32)> operator-(const le_sintbv<_Nb>& x, uint32_t y)
    {
      return (static_cast<sintbv<_Nb> >(x)-y);
    }

    friend le_intbv<MAX(_Nb,32)> operator-(uint32_t x,const le_sintbv<_Nb>& y)
    {
      return (x - static_cast<sintbv<_Nb> >(y));
    }

    friend le_sintbv<MAX(_Nb,32)> operator-(const le_sintbv<_Nb>& x, int32_t y)
    {
      return (static_cast<sintbv<_Nb> >(x)-y);
    }

    friend le_sintbv<MAX(_Nb,32)> operator-(int32_t x,const le_sintbv<_Nb>& y)
    {
      return (x - static_cast<sintbv<_Nb> >(y));
    }

    friend le_sintbv<MAX(_Nb,64)> operator-(const le_sintbv<_Nb>& x, int64_t y)
    {
      return (static_cast<sintbv<_Nb> >(x)-y);
    }

    friend le_sintbv<MAX(_Nb,64)> operator-(int64_t x,const le_sintbv<_Nb>& y)
    {
      return (x - static_cast<sintbv<_Nb> >(y));
    }
   
    le_sintbv<_Nb>& operator-=(uint64_t rhs)
    {
      Base::operator-=(rhs);
      return *this;
    }

    le_sintbv<_Nb>& operator-=(int32_t rhs)
    {
      Base::operator-=(rhs);
      return *this;
    }

    le_sintbv<_Nb>& operator-=(const le_sintbv<_Nb> &rhs)
    {
      Base::operator-=(rhs);
      return *this;
    }

    
    ////////////////////
    //
    // Addition
    //

    friend le_sintbv<_Nb> operator+(const le_sintbv<_Nb>& x, const le_sintbv<_Nb>& y)
    {
      return operator+(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb> >(y));
    }
    
    template<size_t _Nb2>
    friend le_sintbv<MAX(_Nb,_Nb2)> operator+(const le_sintbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator+(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }

    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator+(const le_intbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator+(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }
    
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator+(const le_sintbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator+(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }     
    
    friend le_intbv<MAX(_Nb,64)> operator+(const le_sintbv<_Nb>& x, uint64_t y)
    {
      return operator+(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,64)> operator+(uint64_t x,const le_sintbv<_Nb>& y)
    {
      return operator+(x,static_cast<sintbv<_Nb> >(y));
    }
    
    friend le_intbv<MAX(_Nb,32)> operator+(const le_sintbv<_Nb>& x, uint32_t y)
    {
      return operator+(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,32)> operator+(uint32_t x,const le_sintbv<_Nb>& y)
    {
      return operator+(x,static_cast<sintbv<_Nb> >(y));
    }
    
    friend le_sintbv<MAX(_Nb,32)> operator+(const le_sintbv<_Nb>& x, int32_t y)
    {
      return operator+(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_sintbv<MAX(_Nb,32)> operator+(int32_t x,const le_sintbv<_Nb>& y)
    {
      return operator+(x,static_cast<sintbv<_Nb> >(y));
    }

    friend le_sintbv<MAX(_Nb,64)> operator+(const le_sintbv<_Nb>& x, int64_t y)
    {
      return operator+(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_sintbv<MAX(_Nb,64)> operator+(int64_t x,const le_sintbv<_Nb>& y)
    {
      return operator+(x,static_cast<sintbv<_Nb> >(y));
    }

    le_sintbv<_Nb>& operator+=(uint64_t rhs)
    {
      Base::operator+=(rhs);
      return *this;
    }

    le_sintbv<_Nb>& operator+=(int32_t rhs)
    {
      Base::operator+=(rhs);
      return *this;
    }

    le_sintbv<_Nb>& operator+=(const le_sintbv<_Nb> &rhs)
    {
      Base::operator+=(rhs);
      return *this;
    }   
  
    /////////////////////
    //
    //  Comparisons
    //

    // == 
    bool operator==(const le_sintbv<_Nb>& rhs) const { 
      return Base::operator==(rhs); 
    }
    template <size_t _Nb2>
    bool operator==(const le_sintbv<_Nb2>& rhs) const {
      return Base::operator==(rhs);
    }
    bool operator==(int32_t rhs) const { 
      return operator==(static_cast<le_sintbv<32> >(rhs));  
    }
    bool operator==(uint32_t rhs) const { 
      return operator==(static_cast<le_sintbv<32> >(rhs));  
    }
    bool operator==(uint64_t rhs) const { 
      return operator==(static_cast<le_sintbv<64> >(rhs));  
    }
    bool operator==(int64_t rhs) const { 
      return operator==(static_cast<le_sintbv<64> >(rhs));  
    }
    // !=
    template <size_t _Nb2>
    bool operator!=(const le_sintbv<_Nb2>& rhs) const { 
      return Base::operator!=(rhs);
    }    
    bool operator!=(const le_sintbv<_Nb>& rhs) const { 
      return Base::operator!=(rhs);
    }
    bool operator!=(int32_t rhs) const { 
      return operator!=(static_cast<le_sintbv<32> >(rhs));  
    }
    bool operator!=(uint32_t rhs) const { 
      return operator!=(static_cast<le_sintbv<32> >(rhs));  
    }
    bool operator!=(uint64_t rhs) const { 
      return operator!=(static_cast<le_sintbv<64> >(rhs));  
    }
    bool operator!=(int64_t rhs) const { 
      return operator!=(static_cast<le_sintbv<64> >(rhs));  
    }
    
    // <=
    template <size_t _Nb2>
    bool operator<=(const le_sintbv<_Nb2>& rhs) const { 
      return Base::operator<=(rhs);
    }
    bool operator<=(const le_sintbv<_Nb>& rhs) const { 
      return Base::operator<=(rhs);
    }
    bool operator<=(int32_t rhs) const { 
      return operator<=(static_cast<le_sintbv<32> >(rhs));  
    }
    bool operator<=(uint32_t rhs) const { 
      return operator<=(static_cast<le_sintbv<32> >(rhs));  
    }
    bool operator<=(uint64_t rhs) const { 
      return operator<=(static_cast<le_sintbv<64> >(rhs));  
    }
    bool operator<=(int64_t rhs) const { 
      return operator<=(static_cast<le_sintbv<64> >(rhs));  
    }

    // >=
    template <size_t _Nb2>
    bool operator>=(const le_sintbv<_Nb2>& rhs) const { 
      return Base::operator>=(rhs);
    }
    bool operator>=(const le_sintbv<_Nb>& rhs) const { 
      return Base::operator>=(rhs);
    }
    bool operator>=(int32_t rhs) const { 
      return operator>=(static_cast<le_sintbv<32> >(rhs));  
    }
    bool operator>=(uint32_t rhs) const { 
      return operator>=(static_cast<le_sintbv<32> >(rhs));  
    }
    bool operator>=(uint64_t rhs) const { 
      return operator>=(static_cast<le_sintbv<64> >(rhs));  
    }
    bool operator>=(int64_t rhs) const { 
      return operator>=(static_cast<le_sintbv<64> >(rhs));  
    }
    // <
    template <size_t _Nb2>
    bool operator<(const le_sintbv<_Nb2>& rhs) const { 
      return Base::operator<(rhs);
    }
    bool operator<(const le_sintbv<_Nb>& rhs) const { 
      return Base::operator<(rhs);
    }
    bool operator<(int32_t rhs) const { 
      return operator<(static_cast<le_sintbv<32> >(rhs));  
    }
    bool operator<(uint32_t rhs) const { 
      return operator<(static_cast<le_sintbv<32> >(rhs));  
    }
    bool operator<(uint64_t rhs) const { 
      return operator<(static_cast<le_sintbv<64> >(rhs));  
    }
    bool operator<(int64_t rhs) const { 
      return operator<(static_cast<le_sintbv<64> >(rhs));  
    }
    // >
    template <size_t _Nb2>
    bool operator>(const le_sintbv<_Nb2>& rhs) const { 
      return Base::operator>(rhs);
    }
    bool operator>(const le_sintbv<_Nb>& rhs) const { 
      return Base::operator>(rhs);
    }
    bool operator>(int32_t rhs) const { 
      return operator>(static_cast<le_sintbv<32> >(rhs));  
    }
    bool operator>(uint32_t rhs) const { 
      return operator>(static_cast<le_sintbv<32> >(rhs));  
    }
    bool operator>(uint64_t rhs) const { 
      return operator>(static_cast<le_sintbv<64> >(rhs));  
    }
    bool operator>(int64_t rhs) const { 
      return operator>(static_cast<le_sintbv<64> >(rhs));  
    }

    ///////////////////////
    //
    //   Multiplication
    //

    friend le_sintbv<_Nb> operator*(const le_sintbv<_Nb>& x, const le_sintbv<_Nb>& y)
    {
      return operator*(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb> >(y));
    }

    template<size_t _Nb2>
    friend le_sintbv<MAX(_Nb,_Nb2)> operator*(const le_sintbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator*(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }

    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator*(const le_intbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator*(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }
    
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator*(const le_sintbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator*(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }   
    
    friend le_intbv<MAX(_Nb,64)> operator*(const le_sintbv<_Nb>& x, uint64_t y)
    {
      return operator*(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,64)> operator*(uint64_t x,const le_sintbv<_Nb>& y)
    {
      return operator*(x,static_cast<sintbv<_Nb> >(y));
    }

    friend le_intbv<MAX(_Nb,32)> operator*(const le_sintbv<_Nb>& x, uint32_t y)
    {
      return operator*(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,32)> operator*(uint32_t x,const le_sintbv<_Nb>& y)
    {
      return operator*(x,static_cast<sintbv<_Nb> >(y));
    }

    friend le_sintbv<MAX(_Nb,32)> operator*(const le_sintbv<_Nb>& x, int32_t y)
    {
      return operator*(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_sintbv<MAX(_Nb,32)> operator*(int32_t x,const le_sintbv<_Nb>& y)
    {
      return operator*(x,static_cast<sintbv<_Nb> >(y));
    }

    friend le_sintbv<MAX(_Nb,64)> operator*(const le_sintbv<_Nb>& x, int64_t y)
    {
      return operator*(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_sintbv<MAX(_Nb,64)> operator*(int64_t x,const le_sintbv<_Nb>& y)
    {
      return operator*(x,static_cast<sintbv<_Nb> >(y));
    }
 
    le_sintbv<_Nb>& operator*=(uint64_t rhs)
    {
      Base::operator*=(rhs);
      return *this;
    }

    le_sintbv<_Nb>& operator*=(int32_t rhs)
    {
      Base::operator*=(rhs);
      return *this;
    }

    le_sintbv<_Nb>& operator*=(const le_sintbv<_Nb> &rhs)
    {
      Base::operator*=(rhs);
      return *this;
    }
 
 
 
 
 
  
    ///////////////////////
    //
    //   Division
    //
    friend le_sintbv<_Nb> operator/(const le_sintbv<_Nb>& x, const le_sintbv<_Nb>& y)
    {
      return operator/(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb> >(y));
    }

    template<size_t _Nb2>
    friend le_sintbv<MAX(_Nb,_Nb2)> operator/(const le_sintbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator/(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }

    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator/(const le_intbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator/(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }
    
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator/(const le_sintbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator/(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }   
    
    friend le_intbv<MAX(_Nb,64)> operator/(const le_sintbv<_Nb>& x, uint64_t y)
    {
      return operator/(static_cast<sintbv<_Nb> >(x),(int64_t)y);
    }

    friend le_intbv<MAX(_Nb,64)> operator/(uint64_t x,const le_sintbv<_Nb>& y)
    {
      return operator/((int64_t)x,static_cast<sintbv<_Nb> >(y));;
    }
 
    friend le_intbv<MAX(_Nb,32)> operator/(const le_sintbv<_Nb>& x, uint32_t y)
    {
      return operator/(static_cast<sintbv<_Nb> >(x),(int32_t)y);
    }
    
    friend le_intbv<MAX(_Nb,32)> operator/(uint32_t x,const le_sintbv<_Nb>& y)
    {
      return operator/((int32_t)x,static_cast<sintbv<_Nb> >(y));
    }

    friend le_sintbv<MAX(_Nb,32)> operator/(const le_sintbv<_Nb>& x, int32_t y)
    {
      return operator/(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_sintbv<MAX(_Nb,32)> operator/(int32_t x,const le_sintbv<_Nb>& y)
    {
      return operator/(x,static_cast<sintbv<_Nb> >(y));
    }    

    friend le_sintbv<MAX(_Nb,64)> operator/(const le_sintbv<_Nb>& x, int64_t y)
    {
      return operator/(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_sintbv<MAX(_Nb,64)> operator/(int64_t x,const le_sintbv<_Nb>& y)
    {
      return operator/(x,static_cast<sintbv<_Nb> >(y));
    }    
    
    le_sintbv<_Nb>& operator/=(uint64_t rhs)
    {
      Base::operator/=(rhs);
      return *this;
    }

    le_sintbv<_Nb>& operator/=(int32_t rhs)
    {
      Base::operator/=(rhs);
      return *this;
    }

    le_sintbv<_Nb>& operator/=(const le_sintbv<_Nb> &rhs)
    {
      Base::operator/=(rhs);
      return *this;
    }  
  
    template <size_t _Nb2>
    le_sintbv<_Nb>& operator/=(const le_sintbv<_Nb2>& rhs){
      Base::operator/=(rhs);
      return *this;
    } 
  
  
    //////////////
    //
    //   Modulus
    //
    
    friend le_sintbv<_Nb> operator%(const le_sintbv<_Nb>& x, const le_sintbv<_Nb>& y)
    {
      return operator%(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb> >(y));
    }
    
    template<size_t _Nb2>
    friend le_sintbv<MAX(_Nb,_Nb2)> operator%(const le_intbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator%(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }
    
    template<size_t _Nb2>
    friend le_sintbv<MAX(_Nb,_Nb2)> operator%(const le_sintbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator%(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }
 
    friend le_intbv<MAX(_Nb,64)> operator%(const le_sintbv<_Nb>& x, uint64_t y)
    {
      return operator%(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,64)> operator%(uint64_t x,const le_sintbv<_Nb>& y)
    {
      return operator%(x,static_cast<sintbv<_Nb> >(y));
    }

    friend le_intbv<MAX(_Nb,32)> operator%(const le_sintbv<_Nb>& x, uint32_t y)
    {
      return operator%(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_intbv<MAX(_Nb,32)> operator%(uint32_t x,const le_sintbv<_Nb>& y)
    {
      return operator%(x,static_cast<sintbv<_Nb> >(y));
    }

    friend le_sintbv<MAX(_Nb,32)> operator%(const le_sintbv<_Nb>& x, int32_t y)
    {
      return operator%(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_sintbv<MAX(_Nb,32)> operator%(int32_t x,const le_sintbv<_Nb>& y)
    {
      return operator%(x,static_cast<sintbv<_Nb> >(y));
    }

    friend le_sintbv<MAX(_Nb,64)> operator%(const le_sintbv<_Nb>& x, int64_t y)
    {
      return operator%(static_cast<sintbv<_Nb> >(x),y);
    }

    friend le_sintbv<MAX(_Nb,64)> operator%(int64_t x,const le_sintbv<_Nb>& y)
    {
      return operator%(x,static_cast<sintbv<_Nb> >(y));
    }

    template<size_t _Nb2>
    friend le_sintbv<MAX(_Nb,_Nb2)> operator%(const le_sintbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator%(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }
    
    le_sintbv<_Nb>& operator%=(const le_sintbv<_Nb>& rhs) {
      Base::operator%=(rhs);
      return *this;
    }

    template <size_t _Nb2>
    le_sintbv<_Nb>& operator%=(const le_sintbv<_Nb2>& rhs){
      Base::operator%=(rhs);
      return *this;
    }      
   
    le_sintbv<_Nb>& operator%=(uint64_t rhs)
    {
      Base::operator%=(rhs);
      return *this;
    }

    le_sintbv<_Nb>& operator%=(int32_t rhs)
    {
      Base::operator%=(rhs);
      return *this;
    }


    
    ////////////////////////////////////////
    //
    //     Logical
    //  
      
    le_sintbv<_Nb>& operator&=(const le_sintbv<_Nb>& rhs)
    {
      Base::operator&=(rhs); 
      return *this;
    }

    le_sintbv<_Nb>& operator|=(const le_sintbv<_Nb>& rhs)
    {
      Base::operator|=(rhs); 
      return *this;
    }

    le_sintbv<_Nb>& operator^=(const le_sintbv<_Nb>& rhs)
    {
      Base::operator^=(rhs); 
      return *this;
    }   
      
  
    friend le_sintbv<_Nb> operator&(const le_sintbv<_Nb>& x, const le_sintbv<_Nb>& y)
    {
      return operator&(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb> >(y));
    }
  
    template<size_t _Nb2>
    friend le_sintbv<MAX(_Nb,_Nb2)> operator&(const le_sintbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator&(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    } 
  
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator&(const le_sintbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator&(static_cast<sintbv<_Nb> >(x),static_cast<intbv<_Nb2> >(y));
    } 
      
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator&(const le_intbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator&(static_cast<intbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }  
 
    friend le_sintbv<MAX(_Nb,32)> operator&(const le_sintbv<_Nb>& x,int32_t y)
    {
      return operator&(static_cast<sintbv<_Nb> >(x),y);
    }   
  
    friend le_sintbv<MAX(_Nb,32)> operator&(int32_t x, const le_sintbv<_Nb>& y)
    {
      return operator&(x,static_cast<sintbv<_Nb> >(y));
    } 

    friend le_sintbv<MAX(_Nb,64)> operator&(const le_sintbv<_Nb>& x,int64_t y)
    {
      return operator&(static_cast<sintbv<_Nb> >(x),y);
    }   
  
    friend le_sintbv<MAX(_Nb,64)> operator&(int64_t x, const le_sintbv<_Nb>& y)
    {
      return operator&(x,static_cast<sintbv<_Nb> >(y));
    } 
     
    friend le_intbv<MAX(_Nb,32)> operator&(const le_sintbv<_Nb>& x,uint32_t y)
    {
      return operator&(static_cast<sintbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,32)> operator&(uint32_t x, const le_sintbv<_Nb>& y)
    {
      return operator&(x,static_cast<sintbv<_Nb> >(y));
    } 
  
    friend le_intbv<MAX(_Nb,64)> operator&(const le_sintbv<_Nb>& x,uint64_t y)
    {
      return operator&(static_cast<sintbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,64)> operator&(uint64_t x, const le_sintbv<_Nb>& y)
    {
      return operator&(x,static_cast<sintbv<_Nb> >(y));
    }    
////////////////////////
    friend le_sintbv<_Nb> operator^(const le_sintbv<_Nb>& x, const le_sintbv<_Nb>& y)
    {
      return operator^(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb> >(y));
    }

    template<size_t _Nb2>
    friend le_sintbv<MAX(_Nb,_Nb2)> operator^(const le_sintbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator^(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }    
    
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator^(const le_sintbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator^(static_cast<sintbv<_Nb> >(x),static_cast<intbv<_Nb2> >(y));
    } 
      
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator^(const le_intbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator^(static_cast<intbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }
   
    friend le_sintbv<MAX(_Nb,32)> operator^(const le_sintbv<_Nb>& x,int32_t y)
    {
      return operator^(static_cast<sintbv<_Nb> >(x),y);
    }
        
    friend le_sintbv<MAX(_Nb,32)> operator^(int32_t x, const le_sintbv<_Nb>& y)
    {
      return operator^(x,static_cast<sintbv<_Nb> >(y));
    } 

    friend le_sintbv<MAX(_Nb,64)> operator^(const le_sintbv<_Nb>& x,int64_t y)
    {
      return operator^(static_cast<sintbv<_Nb> >(x),y);
    }
        
    friend le_sintbv<MAX(_Nb,64)> operator^(int64_t x, const le_sintbv<_Nb>& y)
    {
      return operator^(x,static_cast<sintbv<_Nb> >(y));
    } 
 
    friend le_intbv<MAX(_Nb,32)> operator^(const le_sintbv<_Nb>& x,uint32_t y)
    {
      return operator^(static_cast<sintbv<_Nb> >(x),y);
    }
        
    friend le_intbv<MAX(_Nb,32)> operator^(uint32_t x, const le_sintbv<_Nb>& y)
    {
      return operator^(x,static_cast<sintbv<_Nb> >(y));
    } 
  
    friend le_intbv<MAX(_Nb,64)> operator^(const le_sintbv<_Nb>& x,uint64_t y)
    {
      return operator^(static_cast<sintbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,64)> operator^(uint64_t x, const le_sintbv<_Nb>& y)
    {
      return operator^(x,static_cast<sintbv<_Nb> >(y));
    }   
    
    friend le_sintbv<_Nb> operator|(const le_sintbv<_Nb>& x, const le_sintbv<_Nb>& y)
    {
      return operator|(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb> >(y));
    }
  
    template<size_t _Nb2>
    friend le_sintbv<MAX(_Nb,_Nb2)> operator|(const le_sintbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator|(static_cast<sintbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    } 
    
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator|(const le_sintbv<_Nb>& x, const le_intbv<_Nb2>& y)
    {
      return operator|(static_cast<sintbv<_Nb> >(x),static_cast<intbv<_Nb2> >(y));
    } 
      
    template<size_t _Nb2>
    friend le_intbv<MAX(_Nb,_Nb2)> operator|(const le_intbv<_Nb>& x, const le_sintbv<_Nb2>& y)
    {
      return operator|(static_cast<intbv<_Nb> >(x),static_cast<sintbv<_Nb2> >(y));
    }  
   
    friend le_sintbv<MAX(_Nb,32)> operator|(const le_sintbv<_Nb>& x,int32_t y)
    {
      return operator|(static_cast<sintbv<_Nb> >(x),y);
    }   
  
    friend le_sintbv<MAX(_Nb,32)> operator|(int32_t x, const le_sintbv<_Nb>& y)
    {
      return operator|(x,static_cast<sintbv<_Nb> >(y));
    }

    friend le_sintbv<MAX(_Nb,64)> operator|(const le_sintbv<_Nb>& x,int64_t y)
    {
      return operator|(static_cast<sintbv<_Nb> >(x),y);
    }   
  
    friend le_sintbv<MAX(_Nb,64)> operator|(int64_t x, const le_sintbv<_Nb>& y)
    {
      return operator|(x,static_cast<sintbv<_Nb> >(y));
    }
     
    friend le_intbv<MAX(_Nb,32)> operator|(const le_sintbv<_Nb>& x,uint32_t y)
    {
      return operator|(static_cast<sintbv<_Nb> >(x),y);
    }   
  
    friend le_intbv<MAX(_Nb,32)> operator|(uint32_t x, const le_sintbv<_Nb>& y)
    {
      return operator|(x,static_cast<sintbv<_Nb> >(y));
    } 
  
    friend le_intbv<MAX(_Nb,64)> operator|(const le_sintbv<_Nb>& x,uint64_t y)
    {
      return operator|(x,static_cast<sintbv<_Nb> >(y));
    }   
  
    friend le_intbv<MAX(_Nb,64)> operator|(uint64_t x, const le_sintbv<_Nb>& y)
    {
      return operator|(x,static_cast<sintbv<_Nb> >(y));
    } 
 
    le_sintbv<_Nb> operator~() const { return le_sintbv<_Nb>(*this).invert(); }
    
    le_sintbv<_Nb> swap_bytes() const {
      return Base::swap_bytes();
    }

    le_sintbv<_Nb> swap_words() const {
      return Base::swap_words();
    }
    
    ///////////////////////////////
    //
    // Shift/Rotate
    //    
    
    
    le_sintbv<_Nb>& operator<<=(size_t position) {
      Base::operator<<=(position);
      return *this;
    }

    template <size_t _Nb2>
    le_sintbv<_Nb>& operator<<=(const le_sintbv<_Nb2>& position)
    {
      Base::operator<<=(position);
      return *this;   
    }

    le_sintbv<_Nb>& operator>>=(size_t position)
    {
      Base::signedRightShiftEqual(position);
      return *this;
    }

    template <size_t _Nb2>
    le_sintbv<_Nb>& operator>>=(const le_sintbv<_Nb2>& position)
    {
      Base::signedRightShiftEqual(position);
      return *this;
    }

    template <size_t _Nb2>
    le_sintbv<_Nb> left_rotate(const le_sintbv<_Nb2>& position) const {
      return Base::left_rotate(position);
    }

    le_sintbv<_Nb> left_rotate (size_t position) const
    {
      return Base::left_rotate(position);
    }

    template <size_t _Nb2>
    le_sintbv<_Nb> right_rotate(const le_sintbv<_Nb2>& position) const
    {   
      return Base::right_rotate(position);
    }   

    le_sintbv<_Nb> right_rotate (size_t position) const 
    {
      return Base::right_rotate(position);
    }

    le_sintbv<_Nb> operator<<(size_t position) const
    { 
      return Base::operator<<(position); 
    }

    template<size_t _Nb2>
    le_sintbv<_Nb> operator<<(const le_sintbv<_Nb2>& position) const
    { 
      return Base::operator<<(position); 
    }

    template<size_t _Nb2>
    le_sintbv<_Nb> operator>>(const le_sintbv<_Nb2>& position) const
    { 
      return Base::signedRightShift(position); 
    }

    le_sintbv<_Nb> operator>>(size_t position) const
    { 
      return Base::signedRightShift(position); 
    }      
// allows to use "ull" after numeric constant 
#ifdef IS_64BIT
    bool operator==(long long unsigned rhs) const { 
      return operator==(static_cast<le_sintbv<64> >(rhs)); 
    }
    bool operator!=(long long unsigned rhs) const { 
      return operator!=(static_cast<le_sintbv<64> >(rhs)); 
    }
    bool operator==(long long int rhs) const { 
      return operator==(static_cast<le_sintbv<64> >(rhs)); 
    }
    bool operator!=(long long int rhs) const { 
      return operator!=(static_cast<le_sintbv<64> >(rhs)); 
    }              
    friend le_intbv<MAX(_Nb,64)> operator&(const le_sintbv<_Nb>& x,unsigned long long y) {
      return operator&(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator&(unsigned long long  x, const le_sintbv<_Nb>& y) {
      return operator&(x,static_cast<le_sintbv<_Nb> >(y));
    } 
    friend le_sintbv<MAX(_Nb,64)> operator&(const le_sintbv<_Nb>& x,long long y) {
      return operator&(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_sintbv<MAX(_Nb,64)> operator&(long long  x, const le_sintbv<_Nb>& y) {
      return operator&(x,static_cast<le_sintbv<_Nb> >(y));
    } 

    friend le_intbv<MAX(_Nb,64)> operator^(const le_sintbv<_Nb>& x,unsigned long long y) {
      return operator^(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator^(unsigned long long  x, const le_sintbv<_Nb>& y) {
      return operator^(x,static_cast<le_sintbv<_Nb> >(y));
    } 
    friend le_sintbv<MAX(_Nb,64)> operator^(const le_sintbv<_Nb>& x,long long y) {
      return operator^(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_sintbv<MAX(_Nb,64)> operator^(long long  x, const le_sintbv<_Nb>& y) {
      return operator^(x,static_cast<le_sintbv<_Nb> >(y));
    } 

    friend le_intbv<MAX(_Nb,64)> operator|(const le_sintbv<_Nb>& x,unsigned long long y) {
      return operator|(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator|(unsigned long long  x, const le_sintbv<_Nb>& y) {
      return operator|(x,static_cast<le_sintbv<_Nb> >(y));
    } 
    friend le_sintbv<MAX(_Nb,64)> operator|(const le_sintbv<_Nb>& x,long long y) {
      return operator|(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_sintbv<MAX(_Nb,64)> operator|(long long  x, const le_sintbv<_Nb>& y) {
      return operator|(x,static_cast<le_sintbv<_Nb> >(y));
    } 

    friend le_intbv<MAX(_Nb,64)> operator+(const le_sintbv<_Nb>& x,unsigned long long y) {
      return operator+(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator+(unsigned long long  x, const le_sintbv<_Nb>& y) {
      return operator+(x,static_cast<le_sintbv<_Nb> >(y));
    } 
    friend le_sintbv<MAX(_Nb,64)> operator+(const le_sintbv<_Nb>& x,long long y) {
      return operator+(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_sintbv<MAX(_Nb,64)> operator+(long long  x, const le_sintbv<_Nb>& y) {
      return operator+(x,static_cast<le_sintbv<_Nb> >(y));
    } 

    friend le_intbv<MAX(_Nb,64)> operator-(const le_sintbv<_Nb>& x,unsigned long long y) {
      return operator-(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator-(unsigned long long  x, const le_sintbv<_Nb>& y) {
      return operator-(x,static_cast<le_sintbv<_Nb> >(y));
    } 
    friend le_sintbv<MAX(_Nb,64)> operator-(const le_sintbv<_Nb>& x,long long y) {
      return operator-(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_sintbv<MAX(_Nb,64)> operator-(long long  x, const le_sintbv<_Nb>& y) {
      return operator-(x,static_cast<le_sintbv<_Nb> >(y));
    } 

    friend le_intbv<MAX(_Nb,64)> operator*(const le_sintbv<_Nb>& x,unsigned long long y) {
      return operator*(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator*(unsigned long long  x, const le_sintbv<_Nb>& y) {
      return operator*(x,static_cast<le_sintbv<_Nb> >(y));
    } 
    friend le_sintbv<MAX(_Nb,64)> operator*(const le_sintbv<_Nb>& x,long long y) {
      return operator*(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_sintbv<MAX(_Nb,64)> operator*(long long  x, const le_sintbv<_Nb>& y) {
      return operator*(x,static_cast<le_sintbv<_Nb> >(y));
    } 

    friend le_intbv<MAX(_Nb,64)> operator/(const le_sintbv<_Nb>& x,unsigned long long y) {
      return operator/(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_intbv<MAX(_Nb,64)> operator/(unsigned long long  x, const le_sintbv<_Nb>& y) {
      return operator/(x,static_cast<le_sintbv<_Nb> >(y));
    } 
    friend le_sintbv<MAX(_Nb,64)> operator/(const le_sintbv<_Nb>& x,long long y) {
      return operator/(static_cast<le_sintbv<_Nb> >(x),y);
    }     
    friend le_sintbv<MAX(_Nb,64)> operator/(long long  x, const le_sintbv<_Nb>& y) {
      return operator/(x,static_cast<le_sintbv<_Nb> >(y));
    } 
    
#endif    
    
   private:
    template <size_t _Nb1>
    size_t be2le(intbv<_Nb1> pos) const {
      return _Nb - pos.uint32() - 1;
    }    
    
    size_t be2le(unsigned pos) const {
      return _Nb - pos - 1;
    }

  };


  /////////////////////////
  //
  // Signed multiplication.  These takes two unsigned bit vectors and returns a
  // signed bit vector as a result.
  //


  // Same size
  template<size_t _Nb>
  inline le_sintbv<_Nb> le_sbitsSignedMultiply(const intbv<_Nb> &x,const intbv<_Nb> &y)
  {
    return signedMultiply(x,y);
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline le_sintbv<MAX(_Nb1,_Nb2)> le_sbitsSignedMultiply(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    return signedMultiply(x,y);
  }
  
  // uint64_t
  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,64)> le_sbitsSignedMultiply(const intbv<_Nb> &x, uint64_t y)
  {
    return signedMultiply(x,y);
  }

  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,64)> le_sbitsSignedMultiply(uint64_t x,const intbv<_Nb> &y)
  {
    return signedMultiply(x,y);
  }
  
  // uint32_t
  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,32)> le_sbitsSignedMultiply(const intbv<_Nb>& x, uint32_t y)
  {
    return signedMultiply(x,y);
  }

  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,32)> le_sbitsSignedMultiply(uint32_t x,const intbv<_Nb>& y)
  {
    return signedMultiply(x,y);
  }

  /////////////////////////////////
  //
  // Signed division.  These takes two unsigned bit vectors and returns a
  // signed bit vector as a result.
  //
    
  // Same size
  template<size_t _Nb>
  inline le_sintbv<_Nb> le_sbitsSignedDivide(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    return signedDivide(x,y);
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline le_sintbv<MAX(_Nb1,_Nb2)> le_sbitsSignedDivide(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    return signedDivide(x,y);
  }
  
  // uint64_t
  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,64)> le_sbitsSignedDivide(const intbv<_Nb>& x, uint64_t y)
  {
    return signedDivide(x,y);
  }

  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,64)> le_sbitsSignedDivide(uint64_t x,const intbv<_Nb>& y)
  {
    return signedDivide(x,y);
  }

  // uint32_t
  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,32)> le_sbitsSignedDivide(const intbv<_Nb>& x, uint32_t y)
  {
    return signedDivide(x,y);
  }

  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,32)> le_sbitsSignedDivide(uint32_t x,const intbv<_Nb>& y)
  {
    return signedDivide(x,y);
  }  
  
  // int32_t
  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,32)> le_sbitsSignedDivide(const intbv<_Nb>& x, int32_t y)
  {
    return signedDivide(x,y);
  }

  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,32)> le_sbitsSignedDivide(int32_t x,const intbv<_Nb>& y)
  {
    return signedDivide(x,y);
  }

  /////////////////////////////////
  //
  // Signed modulo.
  //
    
  // Same size
  template<size_t _Nb>
  inline le_sintbv<_Nb> le_sbitsSignedMod(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    return signedMod(x,y);
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline le_sintbv<MAX(_Nb1,_Nb2)> le_sbitsSignedMod(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    return signedMod(x,y);
  }
  
  // uint64_t
  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,64)> le_sbitsSignedMod(const intbv<_Nb>& x, uint64_t y)
  {
    return signedMod(x,y);
  }

  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,64)> le_sbitsSignedMod(uint64_t x,const intbv<_Nb>& y)
  {
    return signedMod(x,y);
  }

  // uint32_t
  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,32)> le_sbitsSignedMod(const intbv<_Nb>& x, uint32_t y)
  {
    return signedMod(x,y);
  }

  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,32)> le_sbitsSignedMod(uint32_t x,const intbv<_Nb>& y)
  {
    return signedMod(x,y);
  }  
  
  // int32_t
  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,32)> le_sbitsSignedMod(const intbv<_Nb>& x, int32_t y)
  {
    return signedMod(x,y);
  }

  template<size_t _Nb>
  inline le_sintbv<MAX(_Nb,32)> le_sbitsSignedMod(int32_t x,const intbv<_Nb>& y)
  {
    return signedMod(x,y);
  }  

};

#endif
