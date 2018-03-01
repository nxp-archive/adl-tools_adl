//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

// 
// Signed bit-vector.
//

#ifndef _SINTBV_H_
#define _SINTBV_H_
#include "intbv.h" 

namespace adl {

  template<size_t _Nb>
  class sintbv : public intbv<_Nb>
  {
  public:
    template <size_t> friend  class sintbv;
    typedef intbv<_Nb> Base;
  
    ////////////////////
    //
    //  Constructors
    //
    
    // All bits set to zero.
    sintbv() {};
    // Initial bits bitwise-copied from a single word (others set to zero).
    sintbv(uint32_t val) : Base(val) {};
    sintbv(uint64_t val) : Base(val) {};
    sintbv(int32_t val)  : Base(val) {};      
    sintbv(int64_t val)  : Base(val) {}
#ifdef IS_64BIT
    sintbv(unsigned long long val) : Base((uint64_t)val) {};
    sintbv(long long int val) : Base((int64_t)val) {};
#else
    sintbv(unsigned long val) : Base((uint32_t)val) {};
    sintbv(long val) : Base((int32_t)val) {};
#endif
    sintbv( const char* s) : Base(s) {
      if (is_negative(s)) {
	Base::negate();
      } 
    };
    sintbv( const std::string& s) : Base(s) {
      if (is_negative(s.c_str())) {
	Base::negate();
      } 
    };
    sintbv( const std::string& s, int format) : Base(s,format) {
      if (is_negative(s.c_str())) {
	Base::negate();
      }    
    };

    sintbv( const std::istream &is ) : Base(is) {};
# ifndef __NO_RNUMBER__
    sintbv( const rnumber::RNumber &x ) : Base(x) {
      if (x.getBit(0) &&_Nb > x.size() ) {
        Base::signExtend(_Nb - x.size());
      } 
    };
# endif
   
    template <size_t _Nb2>
    sintbv (const intbv<_Nb2> &x): Base(x) {
      if (_Nb > _Nb2) {
        Base::signExtend(_Nb - _Nb2);
      }   
    }
    
    explicit sintbv (const intbv<_Nb> &x): Base(x) {}
        
    template <size_t _Nb2>
    sintbv (const sintbv<_Nb2> &x): Base(x) {
      if (_Nb > _Nb2) {
        Base::signExtend(_Nb - _Nb2);
      }
    }
        
    sintbv (const sintbv<_Nb> &x): Base(x) {}
        
    template <typename R> 
      explicit sintbv (unsigned x,const R &r) : Base(x, r) {}  
    

    friend std::ostream& operator<<(std::ostream& os, const sintbv<_Nb> &x) 
    {
      std::ios_base::fmtflags flags =  os.flags();
      if (flags & std::ios_base::dec) {
      	sintbv<_Nb> tmp(x);
        if(x.get(0)) {
          tmp.negate() ;
          os << '-';
        } else if (flags & std::ios_base::showpos) {
          os << '+';
        }
        os << static_cast<Base>(tmp);
      } else {
        os << static_cast<Base>(x);
      }
      return os; 
    }
    
    //////////////////
    //
    //  Getters/Setters/Masks
    // 
   
       // to set a field
    sintbv<_Nb> &set (size_t start, size_t stop, const sintbv<_Nb>& x) 
    {
      Base::set(start,stop,x);
      return *this;
    }
    
    template <size_t _Nb2>
    sintbv<_Nb> &set (size_t start, size_t stop, const sintbv<_Nb2>& x) 
    {
      Base::set(start,stop,x);
      return *this;
    }
    
    template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
    sintbv<_Nb> &set (const sintbv<_Nb1> &start, const sintbv<_Nb2> &stop, const sintbv<_Nb3>& x) 
    { 
      Base::set(start,stop,x);
      return *this;
    }
    // sets the specified range to all ones.
    sintbv<_Nb> &mask (size_t start, size_t stop)
    { 
      Base::mask(start,stop);
      return *this;
    }
    
    template <size_t _Nb1,size_t _Nb2>
    sintbv<_Nb> &mask (const sintbv<_Nb1>& start, const sintbv<_Nb2> &stop)
    { 
      Base::mask(start,stop);
      return *this;
    }
    // sets the specified range to all zeroes.
    sintbv<_Nb> &mask_inv (size_t start, size_t stop)
    { 
      Base::mask_inv(start,stop);
      return *this;
    }    
    template <size_t _Nb1,size_t _Nb2>
    sintbv<_Nb> &mask_inv (const sintbv<_Nb1>& start, const sintbv<_Nb2>& stop)
    { 
      Base::mask_inv(start,stop);
      return *this;
    }     
    // to get a field/slice (returns same size sintbv w/ value right justified
    const sintbv<_Nb> operator () (size_t start, size_t stop) const 
    { 
      return Base::operator ()(start,stop);
    }    
    
    const sintbv<_Nb> get (size_t start, size_t stop) const
    { 
      return Base::get(start,stop);
    }     
    
    template <size_t _Nb1,size_t _Nb2>
    const sintbv<_Nb> get (const sintbv<_Nb1>& start, const sintbv<_Nb2>& stop) const
    { 
      return Base::get(start,stop);
    } 
        
    template <size_t _Nb1,size_t _Nb2>
    const sintbv<_Nb> operator () (const sintbv<_Nb1>& start, const sintbv<_Nb2>& stop) const
    { 
      return Base::operator()(start,stop);
    }  
       
    template <size_t _Nb2>
    const sintbv<_Nb2> get (size_t start, size_t stop) const
    { 
      return Base::template get<_Nb2>(start,stop);
    }     
    
    template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
    const sintbv<_Nb3> get (const sintbv<_Nb1>& start,const sintbv<_Nb2>& stop) const
    { 
      return Base::template get<_Nb3>(start,stop);
    }   
    
    bool get(size_t position) const 
    {
      return Base::get(position);
    }   
  
    sintbv<_Nb>& set() 
    {
      Base::set();
      return *this;
    }
    sintbv<_Nb>& set(size_t position, bool val = true) 
    {
      Base::set(position,val);
      return *this;    
    }
    
    template <size_t _Nb1>
    sintbv<_Nb>& set(const sintbv<_Nb1>& position, bool val = true)
    {
      Base::set(position,val);
      return *this;    
    } 
       
    template <size_t _Nb1>
    sintbv<_Nb>& set(size_t position,const sintbv<_Nb1>& value)
    {
      Base::set(position,value);
      return *this;    
    } 
       
    template <size_t _Nb1,size_t _Nb2>
    sintbv<_Nb>& set(const sintbv<_Nb1>& position,const sintbv<_Nb2>& value)
    {
      Base::set(position,value);
      return *this;    
    }
    
    sintbv<_Nb>& reset()
    {
      Base::reset();
      return *this;    
    }    
    sintbv<_Nb>& reset(size_t position)
    {
      Base::reset(position);
      return *this;    
    }  
  
    const intbv<1> operator[](size_t position) const 
    { 
      return Base::operator[](position); 
    }
    template <size_t _Nb2>
    const intbv<1> operator[](const sintbv<_Nb2>& position) const 
    { 
      return Base::operator[](position);  
    }

    const intbv<1> operator()(size_t position) const 
    { 
      return Base::operator()(position); 
    }

    template <size_t _Nb2>
    const intbv<1> operator()(const sintbv<_Nb2>& position) const 
    { 
      return Base::operator()(position); 
    }

    sintbv<_Nb>& _Unchecked_set(size_t pos) 
    {
      Base::_Unchecked_set(pos);
      return *this;
    }

    sintbv<_Nb>& _Unchecked_set(size_t pos, int val)
    {
      Base::_Unchecked_set(pos,val);
      return *this;
    }

    sintbv<_Nb>& _Unchecked_reset(size_t pos)
    {
      Base::_Unchecked_reset(pos);
      return *this;   
    }

    sintbv<_Nb>& _Unchecked_invert(size_t pos)
    {
      Base::_Unchecked_invert(pos);
      return *this;    
    }
    bool _Unchecked_test(size_t pos) const 
    {
      return Base::_Unchecked_test(pos);
    }

    bool any() const 
    { 
      return Base::any(); 
    }

    bool none() const 
    { 
      return Base::none(); 
    }



    /////////////////////
    //
    //  Bit Manipulations
    //


    // Toggles every bit to its opposite value.
    sintbv<_Nb>& invert()
    {
      Base::invert();
      return *this;    
    }
    // Two's complement
    sintbv<_Nb>& negate()
    {
      Base::negate();
      return *this;     
    }
    // Toggles a given bit to its opposite value.
    sintbv<_Nb>& invert(size_t position)
    {
      Base::invert(position);
      return *this;     
    }
    // See the no-argument invert().
    sintbv<_Nb> operator~() const 
    { 
      return sintbv<_Nb>(*this).invert(); 
    }

    sintbv<_Nb> swap_bytes() const {
      return Base::swap_bytes();
    }

    sintbv<_Nb> swap_words() const {
      return Base::swap_words();
    }


     // shift/rotate operations on sintbvs.     
    sintbv<_Nb>& operator<<=(size_t position) {
      Base::operator<<=(position);
      return *this;
    }

    template <size_t _Nb2>
    sintbv<_Nb>& operator<<=(const sintbv<_Nb2>& position)
    {
      Base::operator<<=(position);
      return *this;   
    }

    sintbv<_Nb>& operator>>=(size_t position)
    {
      Base::signedRightShiftEqual(position);
      return *this;
    }

    template <size_t _Nb2>
    sintbv<_Nb>& operator>>=(const sintbv<_Nb2>& position)
    {
      Base::signedRightShiftEqual(position);
      return *this;
    }

    template <size_t _Nb2>
    sintbv<_Nb> left_rotate(const sintbv<_Nb2>& position) const {
      return Base::left_rotate(position);
    }

    sintbv<_Nb> left_rotate (size_t position) const
    {
      return Base::left_rotate(position);
    }

    template <size_t _Nb2>
    sintbv<_Nb> right_rotate(const sintbv<_Nb2>& position) const
    {   
      return Base::right_rotate(position);
    }   

    sintbv<_Nb> right_rotate (size_t position) const 
    {
      return Base::right_rotate(position);
    }

    sintbv<_Nb> operator<<(size_t position) const
    { 
      return Base::operator<<(position); 
    }

    template<size_t _Nb2>
    sintbv<_Nb> operator<<(const sintbv<_Nb2>& position) const
    { 
      return Base::operator<<(position); 
    }

    template<size_t _Nb2>
    sintbv<_Nb> operator>>(const sintbv<_Nb2>& position) const
    { 
      return Base::signedRightShift(position); 
    }

    sintbv<_Nb> operator>>(size_t position) const
    { 
      return Base::signedRightShift(position); 
    }    
    
    /////////////////
    //
    // Subtraction
    //    
    

    // same size
    friend sintbv<_Nb> operator-(const sintbv<_Nb>& x, const sintbv<_Nb>& y)
    {
      sintbv<_Nb> result(x);
      result -= y;
      return result;
    }

    // diff size
    template <size_t _Nb1>
    friend sintbv< MAX(_Nb,_Nb1)> operator-(const sintbv<_Nb>& x, const sintbv<_Nb1>& y)
    {
      sintbv <MAX(_Nb,_Nb1) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb1) > tmp_y (y);
      tmp_x -= tmp_y;
      return tmp_x;
    }
     
    // with unsigned 
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator-(const sintbv<_Nb>& x, const intbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2)> tmp_x (x);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (y);
      tmp_x -= tmp_y;
      return tmp_x;
    }
    
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator-(const intbv<_Nb2>& x, const sintbv<_Nb>& y)
    {
      sintbv <MAX(_Nb,_Nb2)> tmp_x (x);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (y);
      tmp_x -= tmp_y;
      return tmp_x;
    } 
    
    friend intbv<MAX(_Nb,32)> operator-(const sintbv<_Nb>& x,uint32_t y) {      
      return (x-static_cast<sintbv<32> >(y));
    }
    friend intbv<MAX(_Nb,32)> operator-(uint32_t x,const sintbv<_Nb>& y) {      
      return (static_cast<sintbv<32> >(x)-y);
    }
    friend sintbv<MAX(_Nb,32)> operator-(const sintbv<_Nb>& x,int32_t y) {      
      return x-static_cast<sintbv<32> >(y);
    }
    friend sintbv<MAX(_Nb,32)> operator-(int32_t x,const sintbv<_Nb>& y) {      
      return (static_cast<sintbv<32> >(x)-y);
    }    
    friend intbv<MAX(_Nb,64)> operator-(const sintbv<_Nb>& x,uint64_t y) {      
      return x-static_cast<sintbv<64> >(y);
    }
    friend intbv<MAX(_Nb,64)> operator-(uint64_t x,const sintbv<_Nb>& y) {      
      return (static_cast<sintbv<64> >(x)-y);
    }    
    friend sintbv<MAX(_Nb,64)> operator-(const sintbv<_Nb>& x,int64_t y) {      
      return x-static_cast<sintbv<64> >(y);
    }
    friend sintbv<MAX(_Nb,64)> operator-(int64_t x,const sintbv<_Nb>& y) {      
      return (static_cast<sintbv<64> >(x)-y);
    }

    // Unary minus
    const sintbv<_Nb> operator-() const {
      sintbv<_Nb> res( *this );
      res.negate();
      return res;
    }
        
    ////////////////////
    //
    // Addition
    //

    // same size
    friend sintbv<_Nb> operator+(const sintbv<_Nb>& x, const sintbv<_Nb>& y)
    {
      sintbv<_Nb> result(x);
      result += y;
      return result;
    }
    
    // diff size
    template<size_t _Nb2>
    friend sintbv<MAX(_Nb,_Nb2)> operator+(const sintbv<_Nb>& x, const sintbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2)> tmp_x (x);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (y);
      tmp_x += tmp_y;
      return tmp_x;
    }
  
    // with unsigned
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator+(const sintbv<_Nb>& x, const intbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2)> tmp_x (x);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (y);
      tmp_x += tmp_y;
      return tmp_x;
    }
    
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator+(const intbv<_Nb2>& x, const sintbv<_Nb>& y)
    {
      sintbv <MAX(_Nb,_Nb2)> tmp_x (x);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (y);
      tmp_x += tmp_y;
      return tmp_x;
    }

    friend intbv<MAX(_Nb,32)> operator+(const sintbv<_Nb>& x,uint32_t y) {      
      return operator+(x,static_cast<sintbv<32> >(y));
    }
    friend intbv<MAX(_Nb,32)> operator+(uint32_t x,const sintbv<_Nb>& y) {      
      return operator+(static_cast<sintbv<32> >(x),y);
    }
    friend sintbv<MAX(_Nb,32)> operator+(const sintbv<_Nb>& x,int32_t y) {      
      return operator+(x,static_cast<sintbv<32> >(y));
    }
    friend sintbv<MAX(_Nb,32)> operator+(int32_t x,const sintbv<_Nb>& y) {      
      return operator+(static_cast<sintbv<32> >(x),y);
    }    
    friend intbv<MAX(_Nb,64)> operator+(const sintbv<_Nb>& x,uint64_t y) {      
      return operator+(x,static_cast<sintbv<64> >(y));
    }
    friend intbv<MAX(_Nb,64)> operator+(uint64_t x,const sintbv<_Nb>& y) {      
      return operator+(static_cast<sintbv<64> >(x),y);
    }    
    friend sintbv<MAX(_Nb,64)> operator+(const sintbv<_Nb>& x,int64_t y) {      
      return operator+(x,static_cast<sintbv<64> >(y));
    }
    friend sintbv<MAX(_Nb,64)> operator+(int64_t x,const sintbv<_Nb>& y) {      
      return operator+(static_cast<sintbv<64> >(x),y);
    }

    
    /////////////////////
    //
    //  Comparisons
    //

    // ==
    bool operator==(const sintbv<_Nb>& rhs) const { 
      return Base::operator==(rhs); 
    }
    bool operator==(int32_t rhs) const { 
      return operator==(static_cast<sintbv<32> >(rhs)); 
    }
    bool operator==(uint32_t rhs) const { 
      return operator==(static_cast<sintbv<32> >(rhs)); 
    }              
    bool operator==(uint64_t rhs) const { 
      return operator==(static_cast<sintbv<64> >(rhs)); 
    }
    bool operator==(int64_t rhs) const { 
      return operator==(static_cast<sintbv<64> >(rhs)); 
    }
    // !=
    bool operator!=(const sintbv<_Nb>& rhs) const { 
      return Base::operator!=(rhs);
    }
    bool operator!=(int32_t rhs) const { 
      return operator!=(static_cast<sintbv<32> >(rhs)); 
    }
    bool operator!=(uint32_t rhs) const { 
      return operator!=(static_cast<sintbv<32> >(rhs)); 
    }              
    bool operator!=(uint64_t rhs) const { 
      return operator!=(static_cast<sintbv<64> >(rhs)); 
    }
    bool operator!=(int64_t rhs) const { 
      return operator!=(static_cast<sintbv<64> >(rhs)); 
    }
    
    template <size_t _Nb2>
    bool operator==(const sintbv<_Nb2>& rhs) const {
      sintbv <MAX(_Nb,_Nb2)> tmp_x (*this);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (rhs);
      return tmp_x == tmp_y;
    }

    template <size_t _Nb2>
    bool operator!=(const sintbv<_Nb2>& rhs) const { 
      sintbv <MAX(_Nb,_Nb2)> tmp_x (*this);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (rhs);      
      return tmp_x != tmp_y;
    }    
    // <=
    bool operator<=(const sintbv<_Nb>& rhs) const {
      return this->signedLE(rhs);
    }

    template <size_t _Nb2>
    bool operator<=(const sintbv<_Nb2>& rhs) const { 
      sintbv <MAX(_Nb,_Nb2)> tmp_x (*this);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (rhs);
      return tmp_x.signedLE(tmp_y);
    }

    bool operator<=(int32_t rhs) const { 
      return operator<=(static_cast<sintbv<32> >(rhs)); 
    }
    bool operator<=(uint32_t rhs) const { 
      return operator<=(static_cast<sintbv<32> >(rhs)); 
    }              
    bool operator<=(uint64_t rhs) const { 
      return operator<=(static_cast<sintbv<64> >(rhs)); 
    }
    bool operator<=(int64_t rhs) const { 
      return operator<=(static_cast<sintbv<64> >(rhs)); 
    }
    // >=
    bool operator>=(const sintbv<_Nb>& rhs) const
    { 
      return this->signedGE(rhs);
    }

    template <size_t _Nb2>
    bool operator>=(const sintbv<_Nb2>& rhs) const
    { 
      sintbv <MAX(_Nb,_Nb2)> tmp_x (*this);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (rhs);
      return tmp_x.signedGE(tmp_y);
    }
    bool operator>=(int32_t rhs) const { 
      return operator>=(static_cast<sintbv<32> >(rhs)); 
    }
    bool operator>=(uint32_t rhs) const { 
      return operator>=(static_cast<sintbv<32> >(rhs)); 
    }              
    bool operator>=(uint64_t rhs) const { 
      return operator>=(static_cast<sintbv<64> >(rhs)); 
    }
    bool operator>=(int64_t rhs) const { 
      return operator>=(static_cast<sintbv<64> >(rhs)); 
    }
    // <
    bool operator<(const sintbv<_Nb>& rhs) const
    { 
      return this->signedLT(rhs);
    }
    
    template <size_t _Nb2>
    bool operator<(const sintbv<_Nb2>& rhs) const
    { 
      sintbv <MAX(_Nb,_Nb2)> tmp_x (*this);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (rhs);
      return tmp_x.signedLT(tmp_y);;
    }
    bool operator<(int32_t rhs) const { 
      return operator<(static_cast<sintbv<32> >(rhs)); 
    }
    bool operator<(uint32_t rhs) const { 
      return operator<(static_cast<sintbv<32> >(rhs)); 
    }              
    bool operator<(uint64_t rhs) const { 
      return operator<(static_cast<sintbv<64> >(rhs)); 
    }
    bool operator<(int64_t rhs) const { 
      return operator<(static_cast<sintbv<64> >(rhs)); 
    }
    // >
    bool operator>(const sintbv<_Nb>& rhs) const
    { 
      return Base::signedGT(rhs);
    }
    
    template <size_t _Nb2>
    bool operator>(const sintbv<_Nb2>& rhs) const
    { 
      sintbv <MAX(_Nb,_Nb2)> tmp_x (*this);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (rhs);
      return tmp_x.signedGT(tmp_y);
    }
    bool operator>(int32_t rhs) const { 
      return operator>(static_cast<sintbv<32> >(rhs)); 
    }
    bool operator>(uint32_t rhs) const { 
      return operator>(static_cast<sintbv<32> >(rhs)); 
    }              
    bool operator>(uint64_t rhs) const { 
      return operator>(static_cast<sintbv<64> >(rhs)); 
    }
    bool operator>(int64_t rhs) const { 
      return operator>(static_cast<sintbv<64> >(rhs)); 
    }

    ///////////////////////
    //
    //   Multiplication
    //

    // same size
    friend sintbv<_Nb> operator*(const sintbv<_Nb>& x, const sintbv<_Nb>& y)
    {
      sintbv <_Nb > tmp_x (x);
      sintbv <_Nb > tmp_y (y);
      return signedMultiply(tmp_x,tmp_y);
    }

    // diff size
    template<size_t _Nb2>
    friend sintbv<MAX(_Nb,_Nb2)> operator*(const sintbv<_Nb>& x, const sintbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb2) > tmp_y (y);
      return signedMultiply(tmp_x,tmp_y);
    }

    // with unsigned
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator*(const sintbv<_Nb>& x, const intbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2)> tmp_x (x);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (y);
      tmp_x *= tmp_y;
      return tmp_x;
    }
    
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator*(const intbv<_Nb2>& x, const sintbv<_Nb>& y)
    {
      sintbv <MAX(_Nb,_Nb2)> tmp_x (x);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (y);
      tmp_x *= tmp_y;
      return tmp_x;
    }
    
    friend intbv<MAX(_Nb,32)> operator*(const sintbv<_Nb>& x,uint32_t y) {      
      return operator*(x,static_cast<sintbv<32> >(y));
    }
    friend intbv<MAX(_Nb,32)> operator*(uint32_t x,const sintbv<_Nb>& y) {      
      return operator*(static_cast<sintbv<32> >(x),y);
    }
    friend sintbv<MAX(_Nb,32)> operator*(const sintbv<_Nb>& x,int32_t y) {      
      return operator*(x,static_cast<sintbv<32> >(y));
    }
    friend sintbv<MAX(_Nb,32)> operator*(int32_t x,const sintbv<_Nb>& y) {      
      return operator*(static_cast<sintbv<32> >(x),y);
    }    
    friend intbv<MAX(_Nb,64)> operator*(const sintbv<_Nb>& x,uint64_t y) {      
      return operator*(x,static_cast<sintbv<64> >(y));
    }
    friend intbv<MAX(_Nb,64)> operator*(uint64_t x,const sintbv<_Nb>& y) {      
      return operator*(static_cast<sintbv<64> >(x),y);
    }    
    friend sintbv<MAX(_Nb,64)> operator*(const sintbv<_Nb>& x,int64_t y) {      
      return operator*(x,static_cast<sintbv<64> >(y));
    }
    friend sintbv<MAX(_Nb,64)> operator*(int64_t x,const sintbv<_Nb>& y) {      
      return operator*(static_cast<sintbv<64> >(x),y);
    }
    
    ///////////////////////
    //
    //   Division
    //
    
    // same size
    friend sintbv<_Nb> operator/(const sintbv<_Nb>& x, const sintbv<_Nb>& y)
    {
      sintbv <_Nb > tmp_x (x);
      sintbv <_Nb > tmp_y (y);
      return signedDivide(x,y);
    }

    // diff size
    template<size_t _Nb2>
    friend sintbv<MAX(_Nb,_Nb2)> operator/(const sintbv<_Nb>& x, const sintbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb2) > tmp_y (y);
      return signedDivide(tmp_x,tmp_y);
    }

    friend intbv<MAX(_Nb,32)> operator/(const sintbv<_Nb>& x,uint32_t y) {      
      return operator/(x,static_cast<sintbv<32> >(y));
    }
    friend intbv<MAX(_Nb,32)> operator/(uint32_t x,const sintbv<_Nb>& y) {      
      return operator/(static_cast<sintbv<32> >(x),y);
    }
    friend sintbv<MAX(_Nb,32)> operator/(const sintbv<_Nb>& x,int32_t y) {      
      return operator/(x,static_cast<sintbv<32> >(y));
    }
    friend sintbv<MAX(_Nb,32)> operator/(int32_t x,const sintbv<_Nb>& y) {      
      return operator/(static_cast<sintbv<32> >(x),y);
    }    
    friend intbv<MAX(_Nb,64)> operator/(const sintbv<_Nb>& x,uint64_t y) {      
      return operator/(x,static_cast<sintbv<64> >(y));
    }
    friend intbv<MAX(_Nb,64)> operator/(uint64_t x,const sintbv<_Nb>& y) {      
      return operator/(static_cast<sintbv<64> >(x),y);
    }    
    friend sintbv<MAX(_Nb,64)> operator/(const sintbv<_Nb>& x,int64_t y) {      
      return operator/(x,static_cast<sintbv<64> >(y));
    }
    friend sintbv<MAX(_Nb,64)> operator/(int64_t x,const sintbv<_Nb>& y) {      
      return operator/(static_cast<sintbv<64> >(x),y);
    }
    
    // with unsigned
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator/(const sintbv<_Nb>& x, const intbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2)> tmp_x (x);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (y);
      return signedDivide(tmp_x,tmp_y);
    }
    
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator/(const intbv<_Nb2>& x, const sintbv<_Nb>& y)
    {
      sintbv <MAX(_Nb,_Nb2)> tmp_x (x);
      sintbv <MAX(_Nb,_Nb2)> tmp_y (y);
      return signedDivide(tmp_x,tmp_y);
    }
 
    sintbv<_Nb>& operator/=(int32_t rhs) {
      *this = *this/rhs;
      return *this;
    }    
        
    sintbv<_Nb>& operator/=(int64_t rhs) {
      *this = *this/rhs;
      return *this;
    }
    
    sintbv<_Nb>& operator/=(const sintbv<_Nb>& rhs) {
      Base::signedDivideEqual(rhs);
      return *this;
    }   
    
    template <size_t _Nb2>
    sintbv<_Nb>& operator/=(const intbv<_Nb2>& rhs) {     
      Base::signedDivideEqual(rhs);
      return *this;    
    }  

    
    //////////////
    //
    //   Modulus
    //
    
    friend sintbv<_Nb> operator%(const sintbv<_Nb>& x, const sintbv<_Nb>& y)
    {
      sintbv<MAX(_Nb,32)> tmp_x(x);
      sintbv<MAX(_Nb,32)> tmp_y(y);
      return signedMod(tmp_x,tmp_y);
    }

    template<size_t _Nb2>
    friend sintbv<MAX(_Nb,_Nb2)> operator%(const sintbv<_Nb>& x, const sintbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb2) > tmp_y (y);
      return signedMod(tmp_x,tmp_y);
    }

    friend intbv<MAX(_Nb,32)> operator%(const sintbv<_Nb>& x,uint32_t y) {      
      return operator%(x,static_cast<sintbv<32> >(y));
    }
    friend intbv<MAX(_Nb,32)> operator%(uint32_t x,const sintbv<_Nb>& y) {      
      return operator%(static_cast<sintbv<32> >(x),y);
    }
    friend sintbv<MAX(_Nb,32)> operator%(const sintbv<_Nb>& x,int32_t y) {      
      return operator%(x,static_cast<sintbv<32> >(y));
    }
    friend sintbv<MAX(_Nb,32)> operator%(int32_t x,const sintbv<_Nb>& y) {      
      return operator%(static_cast<sintbv<32> >(x),y);
    }    
    friend intbv<MAX(_Nb,64)> operator%(const sintbv<_Nb>& x,uint64_t y) {      
      return operator%(x,static_cast<sintbv<64> >(y));
    }
    friend intbv<MAX(_Nb,64)> operator%(uint64_t x,const sintbv<_Nb>& y) {      
      return operator%(static_cast<sintbv<64> >(x),y);
    }    
    friend sintbv<MAX(_Nb,64)> operator%(const sintbv<_Nb>& x,int64_t y) {      
      return operator%(x,static_cast<sintbv<64> >(y));
    }
    friend sintbv<MAX(_Nb,64)> operator%(int64_t x,const sintbv<_Nb>& y) {      
      return operator%(static_cast<sintbv<64> >(x),y);
    }

    
    sintbv<_Nb>& operator%=(const sintbv<_Nb>& rhs) {
      *this =  (*this)%rhs;
      return *this;
    }

    template <size_t _Nb2>
    sintbv<_Nb>& operator%=(const sintbv<_Nb2>& rhs){
      *this =  (*this)%rhs;
      return *this;
    }      
  
    template<size_t _Nb2>
    friend sintbv<MAX(_Nb,_Nb2)> operator%(const sintbv<_Nb>& x, const intbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2)> tmp_x(x);
      sintbv <MAX(_Nb,_Nb2)> tmp_y(y);
      return signedMod(tmp_x,tmp_y);
    }
    
    template<size_t _Nb2>
    friend sintbv<MAX(_Nb,_Nb2)> operator%(const intbv<_Nb2>& x, const sintbv<_Nb>& y)
    {
      sintbv <MAX(_Nb,_Nb2)> tmp_x(x);
      sintbv <MAX(_Nb,_Nb2)> tmp_y(y);
#     ifdef IS_64BIT
      // Hack for compiler aliasing bug.
      tmp_y.dummy();
#     endif
      return signedMod(tmp_x,tmp_y);
    } 
         
    ////////////////////////////////////////
    //
    //     Logical
    //  
      
    sintbv<_Nb>& operator&=(const sintbv<_Nb>& rhs)
    {
      Base::operator&=(rhs); 
      return *this;
    }

    sintbv<_Nb>& operator|=(const sintbv<_Nb>& rhs)
    {
      Base::operator|=(rhs); 
      return *this;
    }

    sintbv<_Nb>& operator^=(const sintbv<_Nb>& rhs)
    {
      Base::operator^=(rhs); 
      return *this;
    }   
      
  
    friend sintbv<_Nb> operator&(const sintbv<_Nb>& x, const sintbv<_Nb>& y)
    {
      sintbv<_Nb> result(x);
      result &= y;
      return result;
    }
 
    template<size_t _Nb2>
    friend sintbv<MAX(_Nb,_Nb2)> operator&(const sintbv<_Nb>& x, const sintbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb2) > tmp_y (y);
      tmp_x &= tmp_y;
      return tmp_x;
    } 
 
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator&(const intbv<_Nb>& x, const sintbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb2) > tmp_y (y);
      tmp_x &= tmp_y;
      return tmp_x;
    }      
 
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator&(const sintbv<_Nb>& x, const intbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb2) > tmp_y (y);
      tmp_x &= tmp_y;
      return tmp_x;
    }
    
    friend intbv<MAX(_Nb,32)> operator&(const sintbv<_Nb>& x,uint32_t y) {      
      return operator&(x,static_cast<sintbv<32> >(y));
    }
    friend intbv<MAX(_Nb,32)> operator&(uint32_t x,const sintbv<_Nb>& y) {      
      return operator&(static_cast<sintbv<32> >(x),y);
    }
    friend sintbv<MAX(_Nb,32)> operator&(const sintbv<_Nb>& x,int32_t y) {      
      return operator&(x,static_cast<sintbv<32> >(y));
    }
    friend sintbv<MAX(_Nb,32)> operator&(int32_t x,const sintbv<_Nb>& y) {      
      return operator&(static_cast<sintbv<32> >(x),y);
    }    
    friend intbv<MAX(_Nb,64)> operator&(const sintbv<_Nb>& x,uint64_t y) {      
      return operator&(x,static_cast<sintbv<64> >(y));
    }
    friend intbv<MAX(_Nb,64)> operator&(uint64_t x,const sintbv<_Nb>& y) {      
      return operator&(static_cast<sintbv<64> >(x),y);
    }    
    friend sintbv<MAX(_Nb,64)> operator&(const sintbv<_Nb>& x,int64_t y) {      
      return operator&(x,static_cast<sintbv<64> >(y));
    }
    friend sintbv<MAX(_Nb,64)> operator&(int64_t x,const sintbv<_Nb>& y) {      
      return operator&(static_cast<sintbv<64> >(x),y);
    }

    friend sintbv<_Nb> operator^(const sintbv<_Nb>& x, const sintbv<_Nb>& y)
    {
      sintbv<_Nb> result(x);
      result ^= y;
      return result;
    }
    
    template<size_t _Nb2>
    friend sintbv<MAX(_Nb,_Nb2)> operator^(const sintbv<_Nb>& x, const sintbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb2) > tmp_y (y);
      tmp_x ^= tmp_y;
      return tmp_x;
    }
 
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator^(const intbv<_Nb>& x, const sintbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb2) > tmp_y (y);
      tmp_x ^= tmp_y;
      return tmp_x;
    }      
 
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator^(const sintbv<_Nb>& x, const intbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb2) > tmp_y (y);
      tmp_x ^= tmp_y;
      return tmp_x;
    } 
  
    friend intbv<MAX(_Nb,32)> operator^(const sintbv<_Nb>& x,uint32_t y) {      
      return operator^(x,static_cast<sintbv<32> >(y));
    }
    friend intbv<MAX(_Nb,32)> operator^(uint32_t x,const sintbv<_Nb>& y) {      
      return operator^(static_cast<sintbv<32> >(x),y);
    }
    friend sintbv<MAX(_Nb,32)> operator^(const sintbv<_Nb>& x,int32_t y) {      
      return operator^(x,static_cast<sintbv<32> >(y));
    }
    friend sintbv<MAX(_Nb,32)> operator^(int32_t x,const sintbv<_Nb>& y) {      
      return operator^(static_cast<sintbv<32> >(x),y);
    }    
    friend intbv<MAX(_Nb,64)> operator^(const sintbv<_Nb>& x,uint64_t y) {      
      return operator^(x,static_cast<sintbv<64> >(y));
    }
    friend intbv<MAX(_Nb,64)> operator^(uint64_t x,const sintbv<_Nb>& y) {      
      return operator^(static_cast<sintbv<64> >(x),y);
    }    
    friend sintbv<MAX(_Nb,64)> operator^(const sintbv<_Nb>& x,int64_t y) {      
      return operator^(x,static_cast<sintbv<64> >(y));
    }
    friend sintbv<MAX(_Nb,64)> operator^(int64_t x,const sintbv<_Nb>& y) {      
      return operator^(static_cast<sintbv<64> >(x),y);
    }
  
    friend sintbv<_Nb> operator|(const sintbv<_Nb>& x, const sintbv<_Nb>& y)
    {
      sintbv<_Nb> result(x);
      result |= y;
      return result;
    }

    template<size_t _Nb2>
    friend sintbv<MAX(_Nb,_Nb2)> operator|(const sintbv<_Nb>& x, const sintbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb2) > tmp_y (y);
      tmp_x |= tmp_y;
      return tmp_x;
    } 
    
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator|(const intbv<_Nb>& x, const sintbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb2) > tmp_y (y);
      tmp_x |= tmp_y;
      return tmp_x;
    }      
 
    template<size_t _Nb2>
    friend intbv<MAX(_Nb,_Nb2)> operator|(const sintbv<_Nb>& x, const intbv<_Nb2>& y)
    {
      sintbv <MAX(_Nb,_Nb2) > tmp_x (x);
      sintbv <MAX(_Nb,_Nb2) > tmp_y (y);
      tmp_x |= tmp_y;
      return tmp_x;
    }
      
    friend intbv<MAX(_Nb,32)> operator|(const sintbv<_Nb>& x,uint32_t y) {      
      return operator|(x,static_cast<sintbv<32> >(y));
    }
    friend intbv<MAX(_Nb,32)> operator|(uint32_t x,const sintbv<_Nb>& y) {      
      return operator|(static_cast<sintbv<32> >(x),y);
    }
    friend sintbv<MAX(_Nb,32)> operator|(const sintbv<_Nb>& x,int32_t y) {      
      return operator|(x,static_cast<sintbv<32> >(y));
    }
    friend sintbv<MAX(_Nb,32)> operator|(int32_t x,const sintbv<_Nb>& y) {      
      return operator|(static_cast<sintbv<32> >(x),y);
    }    
    friend intbv<MAX(_Nb,64)> operator|(const sintbv<_Nb>& x,uint64_t y) {      
      return operator|(x,static_cast<sintbv<64> >(y));
    }
    friend intbv<MAX(_Nb,64)> operator|(uint64_t x,const sintbv<_Nb>& y) {      
      return operator|(static_cast<sintbv<64> >(x),y);
    }    
    friend sintbv<MAX(_Nb,64)> operator|(const sintbv<_Nb>& x,int64_t y) {      
      return operator|(x,static_cast<sintbv<64> >(y));
    }
    friend sintbv<MAX(_Nb,64)> operator|(int64_t x,const sintbv<_Nb>& y) {      
      return operator|(static_cast<sintbv<64> >(x),y);
    }
    
    //////////////////////
    //
    //  Other
    //
 
 
    uint32_t uint32() const 
    { 
      return Base::uint32(); 
    }

    // Returns the lower 64-bits (or the entire thing) of the intbv.
    uint64_t uint64() const
    { 
      return Base::uint64(); 
    }

    // Returns the sign-extended lower 64-bits (or the entire thing) of the intbv.
    int64_t int64() const
    { 
      return Base::int64();
    }

    // Returns the sign-extended lower 32-bits (or the entire thing) of the intbv.
    int32_t int32() const
    {
      return Base::int32();
    }

# ifndef __NO_RNUMBER__
    // Returns an RNumber of the same size as this object.
    rnumber::RNumber rnum() const 
    {
      return Base::rnum(); 
    };
# endif

    // Returns the number of bits which are set.
    size_t count() const 
    { 
      return Base::count(); 
    }
    
    // Returns the total number of bits.
    size_t size() const 
    { 
      return  Base::size();
    }




// allows to use "ull" after numeric constant 
#ifdef IS_64BIT
    friend intbv<MAX(_Nb,64)> operator+(const sintbv<_Nb>& x,unsigned long long int y) {
      return operator+(x,static_cast<sintbv<64> >(y));
    } 
    friend intbv<MAX(_Nb,64)> operator+(unsigned long long int x, const sintbv<_Nb>& y) {
      return operator+(static_cast<sintbv<64> >(x),y);
    } 

    friend sintbv<MAX(_Nb,64)> operator+(const sintbv<_Nb>& x,long long int y) {
      return operator+(x,static_cast<sintbv<64> >(y));
    } 
    friend sintbv<MAX(_Nb,64)> operator+(long long int x, const sintbv<_Nb>& y) {
      return operator+(static_cast<sintbv<64> >(x),y);
    } 

    friend intbv<MAX(_Nb,64)> operator-(const sintbv<_Nb>& x,unsigned long long int y) {
      return (x - static_cast<sintbv<64> >(y));
    } 
    friend intbv<MAX(_Nb,64)> operator-(unsigned long long int x, const sintbv<_Nb>& y) {
      return (static_cast<sintbv<64> >(x)-y);
    } 

    friend sintbv<MAX(_Nb,64)> operator-(const sintbv<_Nb>& x,long long int y) {
      return  (x - static_cast<sintbv<64> >(y));
    } 
    friend sintbv<MAX(_Nb,64)> operator-(long long int x, const sintbv<_Nb>& y) {
      return (static_cast<sintbv<64> >(x)-y);
    } 

    friend intbv<MAX(_Nb,64)> operator%(const sintbv<_Nb>& x,unsigned long long int y) {
      return operator%(x,static_cast<sintbv<64> >(y));
    } 
    friend intbv<MAX(_Nb,64)> operator%(unsigned long long int x, const sintbv<_Nb>& y) {
      return operator%(static_cast<sintbv<64> >(x),y);
    } 

    friend sintbv<MAX(_Nb,64)> operator%(const sintbv<_Nb>& x,long long int y) {
      return operator%(x,static_cast<sintbv<64> >(y));
    } 
    friend sintbv<MAX(_Nb,64)> operator%(long long int x, const sintbv<_Nb>& y) {
      return operator%(static_cast<sintbv<64> >(x),y);
    } 

    friend intbv<MAX(_Nb,64)> operator*(const sintbv<_Nb>& x,unsigned long long int y) {
      return operator*(x,static_cast<sintbv<64> >(y));
    } 
    friend intbv<MAX(_Nb,64)> operator*(unsigned long long int x, const sintbv<_Nb>& y) {
      return operator*(static_cast<sintbv<64> >(x),y);
    } 

    friend sintbv<MAX(_Nb,64)> operator*(const sintbv<_Nb>& x,long long int y) {
      return operator*(x,static_cast<sintbv<64> >(y));
    } 
    friend sintbv<MAX(_Nb,64)> operator*(long long int x, const sintbv<_Nb>& y) {
      return operator*(static_cast<sintbv<64> >(x),y);
    } 

    friend intbv<MAX(_Nb,64)> operator/(const sintbv<_Nb>& x,unsigned long long int y) {
      return operator/(x,static_cast<sintbv<64> >(y));
    } 
    friend intbv<MAX(_Nb,64)> operator/(unsigned long long int x, const sintbv<_Nb>& y) {
      return operator/(static_cast<sintbv<64> >(x),y);
    } 

    friend sintbv<MAX(_Nb,64)> operator/(const sintbv<_Nb>& x,long long int y) {
      return operator/(x,static_cast<sintbv<64> >(y));
    } 
    friend sintbv<MAX(_Nb,64)> operator/(long long int x, const sintbv<_Nb>& y) {
      return operator/(static_cast<sintbv<64> >(x),y);
    } 

    friend intbv<MAX(_Nb,64)> operator&(const sintbv<_Nb>& x,unsigned long long int y) {
      return operator&(x,static_cast<sintbv<64> >(y));
    } 
    friend intbv<MAX(_Nb,64)> operator&(unsigned long long int x, const sintbv<_Nb>& y) {
      return operator&(static_cast<sintbv<64> >(x),y);
    } 

    friend sintbv<MAX(_Nb,64)> operator&(const sintbv<_Nb>& x,long long int y) {
      return operator&(x,static_cast<sintbv<64> >(y));
    } 
    friend sintbv<MAX(_Nb,64)> operator&(long long int x, const sintbv<_Nb>& y) {
      return operator&(static_cast<sintbv<64> >(x),y);
    } 

    friend intbv<MAX(_Nb,64)> operator|(const sintbv<_Nb>& x,unsigned long long int y) {
      return operator|(x,static_cast<sintbv<64> >(y));
    } 
    friend intbv<MAX(_Nb,64)> operator|(unsigned long long int x, const sintbv<_Nb>& y) {
      return operator|(static_cast<sintbv<64> >(x),y);
    } 

    friend sintbv<MAX(_Nb,64)> operator|(const sintbv<_Nb>& x,long long int y) {
      return operator|(x,static_cast<sintbv<64> >(y));
    } 
    friend sintbv<MAX(_Nb,64)> operator|(long long int x, const sintbv<_Nb>& y) {
      return operator|(static_cast<sintbv<64> >(x),y);
    } 


    friend intbv<MAX(_Nb,64)> operator^(const sintbv<_Nb>& x,unsigned long long int y) {
      return operator^(x,static_cast<sintbv<64> >(y));
    } 
    friend intbv<MAX(_Nb,64)> operator^(unsigned long long int x, const sintbv<_Nb>& y) {
      return operator^(static_cast<sintbv<64> >(x),y);
    } 

    friend sintbv<MAX(_Nb,64)> operator^(const sintbv<_Nb>& x,long long int y) {
      return operator^(x,static_cast<sintbv<64> >(y));
    } 
    friend sintbv<MAX(_Nb,64)> operator^(long long int x, const sintbv<_Nb>& y) {
      return operator^(static_cast<sintbv<64> >(x),y);
    } 
    
    bool operator==(unsigned long long  rhs) const
    { return operator==(static_cast<sintbv<64> >(rhs)); }
    bool operator!=(unsigned long long  rhs) const
    { return operator!=(static_cast<sintbv<64> >(rhs)); }

    bool operator==(long long int rhs) const
    { return operator==(static_cast<sintbv<64> >(rhs)); }
    bool operator!=(long long int rhs) const
    { return operator!=(static_cast<sintbv<64> >(rhs)); }

    bool operator<(long long int rhs) const
    { return operator<(static_cast<sintbv<64> >(rhs)); }    
    bool operator<(unsigned long long int rhs) const
    { return operator<(static_cast<sintbv<64> >(rhs)); }    

    bool operator>(long long int rhs) const
    { return operator>(static_cast<sintbv<64> >(rhs)); }    
    bool operator>(unsigned long long int rhs) const
    { return operator>(static_cast<sintbv<64> >(rhs)); }    

    bool operator<=(long long int rhs) const
    { return operator<=(static_cast<sintbv<64> >(rhs)); }    
    bool operator<=(unsigned long long int rhs) const
    { return operator<=(static_cast<sintbv<64> >(rhs)); }    

    bool operator>=(long long int rhs) const
    { return operator>=(static_cast<sintbv<64> >(rhs)); }    
    bool operator>=(unsigned long long int rhs) const
    { return operator>=(static_cast<sintbv<64> >(rhs)); }    

#endif

  protected:
    bool is_negative(const char* s) {
      const char* p = s;
      bool negative = false;
      while (!(negative = (*p == '-')) && !isalnum(*p) && (p++ != '\0')) {;};
      return negative;
    }

  };//sintbv

  /////////////////////////
  //
  // Signed multiplication.  These takes two unsigned bit vectors and returns a
  // signed bit vector as a result.
  //

  // Same size
  template<size_t _Nb>
  inline sintbv<_Nb> sbitsSignedMultiply(const intbv<_Nb> &x,const intbv<_Nb> &y)
  {
    return signedMultiply(x,y);
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline sintbv<MAX(_Nb1,_Nb2)> sbitsSignedMultiply(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    return signedMultiply(x,y);
  }
  
  // uint64_t
  template<size_t _Nb>
  inline sintbv<MAX(_Nb,64)> sbitsSignedMultiply(const intbv<_Nb> &x, uint64_t y)
  {
    return signedMultiply(x,y);
  }

  template<size_t _Nb>
  inline sintbv<MAX(_Nb,64)> sbitsSignedMultiply(uint64_t x,const intbv<_Nb> &y)
  {
    return signedMultiply(x,y);
  }
  
  // uint32_t
  template<size_t _Nb>
  inline sintbv<MAX(_Nb,32)> sbitsSignedMultiply(const intbv<_Nb>& x, uint32_t y)
  {
    return signedMultiply(x,y);
  }

  template<size_t _Nb>
  inline sintbv<MAX(_Nb,32)> sbitsSignedMultiply(uint32_t x,const intbv<_Nb>& y)
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
  inline sintbv<_Nb> sbitsSignedDivide(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    return signedDivide(x,y);
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline sintbv<MAX(_Nb1,_Nb2)> sbitsSignedDivide(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    return signedDivide(x,y);
  }
  
  // uint64_t
  template<size_t _Nb>
  inline sintbv<MAX(_Nb,64)> sbitsSignedDivide(const intbv<_Nb>& x, uint64_t y)
  {
    return signedDivide(x,y);
  }

  template<size_t _Nb>
  inline sintbv<MAX(_Nb,64)> sbitsSignedDivide(uint64_t x,const intbv<_Nb>& y)
  {
    return signedDivide(x,y);
  }

  // uint32_t
  template<size_t _Nb>
  inline sintbv<MAX(_Nb,32)> sbitsSignedDivide(const intbv<_Nb>& x, uint32_t y)
  {
    return signedDivide(x,y);
  }

  template<size_t _Nb>
  inline sintbv<MAX(_Nb,32)> sbitsSignedDivide(uint32_t x,const intbv<_Nb>& y)
  {
    return signedDivide(x,y);
  }  
  
  // int32_t
  template<size_t _Nb>
  inline sintbv<MAX(_Nb,32)> sbitsSignedDivide(const intbv<_Nb>& x, int32_t y)
  {
    return signedDivide(x,y);
  }

  template<size_t _Nb>
  inline sintbv<MAX(_Nb,32)> sbitsSignedDivide(int32_t x,const intbv<_Nb>& y)
  {
    return signedDivide(x,y);
  }

  //
  // Signed modulo
  //

  /////////////////////////////////
  //
  // Signed division.  These takes two unsigned bit vectors and returns a
  // signed bit vector as a result.
  //
    
  // Same size
  template<size_t _Nb>
  inline sintbv<_Nb> sbitsSignedMod(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    return signedMod(x,y);
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline sintbv<MAX(_Nb1,_Nb2)> sbitsSignedMod(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    return signedMod(x,y);
  }
  
  // uint64_t
  template<size_t _Nb>
  inline sintbv<MAX(_Nb,64)> sbitsSignedMod(const intbv<_Nb>& x, uint64_t y)
  {
    return signedMod(x,y);
  }

  template<size_t _Nb>
  inline sintbv<MAX(_Nb,64)> sbitsSignedMod(uint64_t x,const intbv<_Nb>& y)
  {
    return signedMod(x,y);
  }

  // uint32_t
  template<size_t _Nb>
  inline sintbv<MAX(_Nb,32)> sbitsSignedMod(const intbv<_Nb>& x, uint32_t y)
  {
    return signedMod(x,y);
  }

  template<size_t _Nb>
  inline sintbv<MAX(_Nb,32)> sbitsSignedMod(uint32_t x,const intbv<_Nb>& y)
  {
    return signedMod(x,y);
  }  
  
  // int32_t
  template<size_t _Nb>
  inline sintbv<MAX(_Nb,32)> sbitsSignedMod(const intbv<_Nb>& x, int32_t y)
  {
    return signedMod(x,y);
  }

  template<size_t _Nb>
  inline sintbv<MAX(_Nb,32)> sbitsSignedMod(int32_t x,const intbv<_Nb>& y)
  {
    return signedMod(x,y);
  }  

}; //adl  

#endif
