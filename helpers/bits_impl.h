//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Implementation file for the bits class.
//

//
// Undefined value propagation strategy: Currently, for arithmetic operations,
// if either operand has undefined bits, the result is fully undefined.  This
// can be refined in the future, but I think it's good enough for now.
//
// For bitwise operations, we use the union of the mask, since we know that
// the undefined bits cannot propagate.
//
// For concatenation, we concatenate the masks together.
//

namespace adl {

#ifndef NO_PROPAGATE

  // Single element:  If undef, result is fully undefined.
  template<size_t _Nb> template<size_t _Nb2>
  inline void bits<_Nb>::propagate_undefined(const bits<_Nb2>& x)
  {
    if (x._mask.any() || _mask.any()) {
      _mask.set();
    }
  }

  template<size_t _Nb> template<size_t _Nb1,size_t _Nb2>
  inline void bits<_Nb>::propagate_undefined(const bits<_Nb1>& x,const bits<_Nb2>& y)
  {
    if (x._mask.any() || y._mask.any()) {
      _mask.set();
    }
  }

  template<size_t _Nb> template<size_t _Nb1,size_t _Nb2,size_t _Nb3>
  inline void bits<_Nb>::propagate_undefined(const bits<_Nb1> &x,const bits<_Nb2> &y,const bits<_Nb3> &z)
  {
    if (x._mask.any() || y._mask.any() || z._mask.any()) {
      _mask.set();
    }
  }

  // Single element:  If undef, result is fully undefined.
  template<size_t _Nb> template<size_t _Nb2>
  inline void bits<_Nb>::union_undefined(const bits<_Nb2>& x)
  {
    _mask |= x._mask;
  }

  template<size_t _Nb> template<size_t _Nb1,size_t _Nb2>
  inline void bits<_Nb>::union_undefined( const bits<_Nb1>& x,const bits<_Nb2>& y)
  {
    _mask = x._mask | y._mask;
  }

# endif

  //
  // Undefined query/set functions.
  //

  template <size_t _Nb> template <size_t _Nb2>
  inline bits<_Nb> &bits<_Nb>::operator= (const bits <_Nb2> &x)
  {
    Base::operator=(x);
# ifndef NO_PROPAGATE
    _mask = x._mask;
# endif
    return *this;
  }

  template <size_t _Nb>
  inline void bits<_Nb>::setUndefined(unsigned pos)
  {
# ifndef NO_PROPAGATE
    _mask.set(pos);
# endif
  }

  template <size_t _Nb>
  inline void bits<_Nb>::setUndefined(unsigned start,unsigned stop)
  {
# ifndef NO_PROPAGATE
    _mask.mask(start,stop);
# endif
  }

  template <size_t _Nb>
  inline bool bits<_Nb>::isUndefined() const
  {
# ifndef NO_PROPAGATE
    return _mask.any();
# else
    return false;
# endif
  }

  template <size_t _Nb>
  inline bool bits<_Nb>::isUndefined(unsigned start,unsigned stop) const
  {
# ifndef NO_PROPAGATE
    return _mask(start,stop).any();
# else
    return false;
# endif
  }

  //
  // Set member functions/operations.
  //

  template <size_t _Nb>
  inline bits<_Nb> &bits<_Nb>::set (unsigned start, unsigned stop, const bits<_Nb>& x)
  {
    Base::set(start,stop,x);
# ifndef NO_PROPAGATE
    _mask.set(start,stop,x._mask);
# endif
    return *this;
  }

  template <size_t _Nb> template <size_t _Nb2>
  inline bits<_Nb> &bits<_Nb>::set (unsigned start, unsigned stop, const bits<_Nb2>& x)
  {
    Base::set(start,stop,x);
# ifndef NO_PROPAGATE
    _mask.set(start,stop,x._mask);
# endif
    return *this;
  }

  template <size_t _Nb> template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
  inline bits<_Nb> &bits<_Nb>::set (const bits<_Nb1>& start, const bits<_Nb2>& stop, const bits<_Nb3>& x)
  {
    Base::set(start,stop,x);
# ifndef NO_PROPAGATE
    _mask.set(start,stop,x._mask);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb> &bits<_Nb>::mask (unsigned start, unsigned stop)
  {
    Base::mask(start,stop);
    return *this;
  }

  template <size_t _Nb> template <size_t _Nb1,size_t _Nb2>
  inline bits<_Nb> &bits<_Nb>::mask(const bits<_Nb1>& start, const bits<_Nb2>& stop)
  {
    Base::mask(start,stop);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb> &bits<_Nb>::mask_inv (unsigned start, unsigned stop)
  {
    Base::mask_inv(start,stop);
    return *this;
  }

  template <size_t _Nb> template <size_t _Nb1,size_t _Nb2>
  inline bits<_Nb> &bits<_Nb>::mask_inv (const bits<_Nb1>& start, const bits<_Nb2>& stop)
  {
    Base::mask_inv(start,stop);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::_Unchecked_set(size_t pos)
  {
    Base::_Unchecked_set(pos);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::_Unchecked_set(size_t pos, int val)
  {
    Base::_Unchecked_set(pos,val);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::_Unchecked_reset(size_t pos)
  {
    Base::_Unchecked_reset(pos);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::_Unchecked_invert(size_t pos)
  {
    Base::_Unchecked_invert(pos);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::set()
  {
    Base::set();
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::set(size_t position, bool val)
  {
    Base::set(position,val);
    return *this;
  }

  template <size_t _Nb> template <size_t _Nb1>
  inline bits<_Nb>& bits<_Nb>::set(const bits<_Nb1>& position, bool val)
  {
    return set(position.uint32(),val);
  }

  template <size_t _Nb> template <size_t _Nb1, size_t _Nb2>
  inline bits<_Nb>& bits<_Nb>::set(const bits<_Nb1>& position,const bits<_Nb2>& val)
  {
    return set(position.uint32(),val[_Nb2-1].uint32());
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::reset()
  {
    Base::reset();
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::reset(size_t position)
  {
    Base::reset(position);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::invert()
  {
    Base::invert();
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::negate()
  {
    Base::negate();
    return *this;  
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::invert(size_t position)
  {
    Base::invert(position);
    return *this;
  }

  //
  // Get/accessor member functions.
  // 

  // gets a field/slice (returns same size intbv w/ value right justified
  template <size_t _Nb>
  inline bits<_Nb> bits<_Nb>::operator () (unsigned start, unsigned stop) const
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res =  Base::get(start,stop);
    res._mask = _mask.get(start,stop);
    return res;
# else
    return Base::get(start,stop); 
# endif 
  }

  template <size_t _Nb>
  inline bits<_Nb> bits<_Nb>::get (unsigned start, unsigned stop) const
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = Base::get(start,stop);
    res._mask = _mask(start,stop);
    return res;
# else
    return Base::get(start,stop);
# endif
  }

  template <size_t _Nb> template <size_t _Nb1,size_t _Nb2>
  inline bits<_Nb> bits<_Nb>::operator () (const bits<_Nb1>& start, const bits<_Nb2>& stop) const
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = Base::get(start,stop);
    res._mask = _mask.get(start,stop);
    return res;
# else
    return Base::get(start,stop);
# endif
  }

  template <size_t _Nb> template <size_t _Nb1,size_t _Nb2>
  inline bits<_Nb> bits<_Nb>::get (const bits<_Nb1>& start, const bits<_Nb2>& stop) const
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = Base::get(start,stop);
    res._mask = _mask(start,stop);
    return res;
# else
    return Base::get(start,stop);
# endif
  }

  //  gets a field/slice (returns given size bits w/ value right justified
  template <size_t _Nb> template <size_t _Nb1>
  inline bits<_Nb1> bits<_Nb>::get (unsigned start,unsigned stop) const
  {
# ifndef NO_PROPAGATE
    bits<_Nb1> res = Base::template get<_Nb1>(start,stop);
    res._mask = _mask.template get<_Nb1>(start,stop);
    return res;
# else
    return Base::template get<_Nb1>(start,stop);
# endif
  }

  template <size_t _Nb> template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
  inline bits<_Nb1> bits<_Nb>::get (const bits<_Nb2>& start,const bits<_Nb3>& stop) const
  {
# ifndef NO_PROPAGATE
    bits<_Nb1> res = Base::get<_Nb1>(start,stop);
    res._mask = _mask.get<_Nb1>(start,stop);
    return res;
# else
    return Base::get<_Nb1>(start,stop);
# endif
  }

  //
  // Unary arithmetic operations on bits.
  //
  // == 
  template <size_t _Nb>
  inline bool bits<_Nb>::operator==(const bits<_Nb> &rhs) const
  { 
    return Base::operator==(rhs); 
  }

  template <size_t _Nb>
  bool bits<_Nb>::operator==(int32_t rhs) const {
    return operator==(static_cast<bits<32> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator==(uint32_t rhs) const {
    return operator==(static_cast<bits<32> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator==(uint64_t rhs) const {
    return operator==(static_cast<bits<64> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator==(int64_t rhs) const {
    return operator==(static_cast<bits<64> >(rhs)); 
  }  

  // !=
  template <size_t _Nb>
  inline bool bits<_Nb>::operator!=(const bits<_Nb> &rhs) const
  { 
    return Base::operator!=(rhs); 
  }

  template <size_t _Nb>
  bool bits<_Nb>::operator!=(int32_t rhs) const {
    return operator!=(static_cast<bits<32> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator!=(uint32_t rhs) const {
    return operator!=(static_cast<bits<32> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator!=(uint64_t rhs) const {
    return operator!=(static_cast<bits<64> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator!=(int64_t rhs) const {
    return operator!=(static_cast<bits<64> >(rhs)); 
  }  

  // <=
  template <size_t _Nb>
  inline bool bits<_Nb>::operator<=(const bits<_Nb> &rhs) const
  { 
    return Base::operator<=(rhs); 
  }

  template <size_t _Nb>
  bool bits<_Nb>::operator<=(int32_t rhs) const {
    return operator<=(static_cast<bits<32> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator<=(uint32_t rhs) const {
    return operator<=(static_cast<bits<32> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator<=(uint64_t rhs) const {
    return operator<=(static_cast<bits<64> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator<=(int64_t rhs) const {
    return operator<=(static_cast<bits<64> >(rhs)); 
  }  
  
  // >= 
  template <size_t _Nb>
  inline bool bits<_Nb>::operator>=(const bits<_Nb> &rhs) const
  { 
    return Base::operator>=(rhs); 
  }

  template <size_t _Nb>
  bool bits<_Nb>::operator>=(int32_t rhs) const {
    return operator>=(static_cast<bits<32> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator>=(uint32_t rhs) const {
    return operator>=(static_cast<bits<32> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator>=(uint64_t rhs) const {
    return operator>=(static_cast<bits<64> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator>=(int64_t rhs) const {
    return operator>=(static_cast<bits<64> >(rhs)); 
  }  
  
  // < 
  template <size_t _Nb>
  inline bool bits<_Nb>::operator<(const bits<_Nb> &rhs) const
  { 
    return Base::operator<(rhs); 
  }

  template <size_t _Nb>
  bool bits<_Nb>::operator<(int32_t rhs) const {
    return operator<(static_cast<bits<32> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator<(uint32_t rhs) const {
    return operator<(static_cast<bits<32> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator<(uint64_t rhs) const {
    return operator<(static_cast<bits<64> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator<(int64_t rhs) const {
    return operator<(static_cast<bits<64> >(rhs)); 
  }  
  
  // >
  template <size_t _Nb>
  inline bool bits<_Nb>::operator>(const bits<_Nb> &rhs) const
  { 
    return Base::operator>(rhs); 
  }

  template <size_t _Nb>
  bool bits<_Nb>::operator>(int32_t rhs) const {
    return operator>(static_cast<bits<32> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator>(uint32_t rhs) const {
    return operator>(static_cast<bits<32> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator>(uint64_t rhs) const {
    return operator>(static_cast<bits<64> >(rhs)); 
  }  
  
  template <size_t _Nb>
  bool bits<_Nb>::operator>(int64_t rhs) const {
    return operator>(static_cast<bits<64> >(rhs)); 
  }  

  // diff size
  template <size_t _Nb>
  template <size_t _Nb2>
  inline bool bits<_Nb>::operator==(const bits<_Nb2> & rhs) const
  { 
    return Base::operator==(rhs); 
  }
    
  template <size_t _Nb>
  template <size_t _Nb2>
  inline bool bits<_Nb>::operator!=(const bits<_Nb2> & rhs) const
  { 
    return Base::operator!=(rhs); 
  }
    
  template <size_t _Nb>
  template <size_t _Nb2>
  inline bool bits<_Nb>::operator<=(const bits<_Nb2> & rhs) const
  { 
    return Base::operator<=(rhs); 
  }
    
  template <size_t _Nb>
  template <size_t _Nb2>
  inline bool bits<_Nb>::operator>=(const bits<_Nb2> & rhs) const
  { 
    return Base::operator>=(rhs); 
  }
    
  template <size_t _Nb>
  template <size_t _Nb2>
  inline bool bits<_Nb>::operator<(const bits<_Nb2> & rhs) const
  { 
    return Base::operator<(rhs); 
  }
    
  template <size_t _Nb>
  template <size_t _Nb2>
  inline bool bits<_Nb>::operator>(const bits<_Nb2> & rhs) const
  { 
    return Base::operator>(rhs); 
  }



    
#ifdef IS_64BIT
  // +
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator+(const bits<_Nb>& x,unsigned long long int y) {
    return operator+(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator+(unsigned long long int x,const bits<_Nb>& y) {
    return operator+(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator+(const bits<_Nb>& x,long long int y) {
    return operator+(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator+(long long int x,const bits<_Nb>& y) {
    return operator+(static_cast<bits<64> >(x),y);
  }
 
  // -
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator-(const bits<_Nb>& x,unsigned long long int y) {
    return operator-(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator-(unsigned long long int x,const bits<_Nb>& y) {
    return operator-(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator-(const bits<_Nb>& x,long long int y) {
    return operator-(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator-(long long int x,const bits<_Nb>& y) {
    return operator-(static_cast<bits<64> >(x),y);
  }
  
  // *
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator*(const bits<_Nb>& x,unsigned long long int y) {
    return operator*(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator*(unsigned long long int x,const bits<_Nb>& y) {
    return operator*(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator*(const bits<_Nb>& x,long long int y) {
    return operator*(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator*(long long int x,const bits<_Nb>& y) {
    return operator*(static_cast<bits<64> >(x),y);
  }

  // /
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator/(const bits<_Nb>& x,unsigned long long int y) {
    return operator/(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator/(unsigned long long int x,const bits<_Nb>& y) {
    return operator/(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator/(const bits<_Nb>& x,long long int y) {
    return operator/(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator/(long long int x,const bits<_Nb>& y) {
    return operator/(static_cast<bits<64> >(x),y);
  }

  // %
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator%(const bits<_Nb>& x,unsigned long long int y) {
    return operator%(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator%(unsigned long long int x,const bits<_Nb>& y) {
    return operator%(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator%(const bits<_Nb>& x,long long int y) {
    return operator%(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator%(long long int x,const bits<_Nb>& y) {
    return operator%(static_cast<bits<64> >(x),y);
  }

  // == 
  template <size_t _Nb>
  bool bits<_Nb>::operator==(long long unsigned int rhs) const  { 
    return operator==(static_cast<bits<64> >(rhs)); 
  }
  template <size_t _Nb>
  bool bits<_Nb>::operator==(long long int rhs) const  { 
    return operator==(static_cast<bits<64> >(rhs)); 
  }
  // !=
  template <size_t _Nb>
  bool bits<_Nb>::operator!=(long long unsigned int rhs) const  { 
    return operator!=(static_cast<bits<64> >(rhs)); 
  }  
  template <size_t _Nb>
  bool bits<_Nb>::operator!=(long long int rhs) const  { 
    return operator!=(static_cast<bits<64> >(rhs)); 
  }  
  
  // >
  template <size_t _Nb>
  bool bits<_Nb>::operator>(long long unsigned int rhs) const  { 
    return operator>(static_cast<bits<64> >(rhs)); 
  }  
  template <size_t _Nb>
  bool bits<_Nb>::operator>(long long int rhs) const  { 
    return operator>(static_cast<bits<64> >(rhs)); 
  }  
  // <
  template <size_t _Nb>
  bool bits<_Nb>::operator<(long long unsigned int rhs) const  { 
    return operator<(static_cast<bits<64> >(rhs)); 
  }  
  template <size_t _Nb>
  bool bits<_Nb>::operator<(long long int rhs) const  { 
    return operator<(static_cast<bits<64> >(rhs)); 
  }  
  // >=
  template <size_t _Nb>
  bool bits<_Nb>::operator>=(long long unsigned int rhs) const  { 
    return operator>=(static_cast<bits<64> >(rhs)); 
  }  
  template <size_t _Nb>
  bool bits<_Nb>::operator>=(long long int rhs) const  { 
    return operator>=(static_cast<bits<64> >(rhs)); 
  }  
  // <=
  template <size_t _Nb>
  bool bits<_Nb>::operator<=(long long unsigned int rhs) const  { 
    return operator<=(static_cast<bits<64> >(rhs)); 
  }  
  template <size_t _Nb>
  bool bits<_Nb>::operator<=(long long int rhs) const  { 
    return operator<=(static_cast<bits<64> >(rhs)); 
  }  
 
  // &
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator&(const bits<_Nb>& x,unsigned long long int y) {
    return operator&(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator&(unsigned long long int x,const bits<_Nb>& y) {
    return operator&(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator&(const bits<_Nb>& x,long long int y) {
    return operator&(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator&(long long int x,const bits<_Nb>& y) {
    return operator&(static_cast<bits<64> >(x),y);
  }
  // |
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator|(const bits<_Nb>& x,unsigned long long int y) {
    return operator|(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator|(unsigned long long int x,const bits<_Nb>& y) {
    return operator|(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator|(const bits<_Nb>& x,long long int y) {
    return operator|(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator|(long long int x,const bits<_Nb>& y) {
    return operator|(static_cast<bits<64> >(x),y);
  }
  // ^
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator^(const bits<_Nb>& x,unsigned long long int y) {
    return operator^(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator^(unsigned long long int x,const bits<_Nb>& y) {
    return operator^(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator^(const bits<_Nb>& x,long long int y) {
    return operator^(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator^(long long int x,const bits<_Nb>& y) {
    return operator^(static_cast<bits<64> >(x),y);
  }
 
  
#endif              











  // comparing same sized bits's
  template <size_t _Nb>
  inline bool bits<_Nb>::signedLE (const bits<_Nb> &rhs) const
  { 
    return Base::signedLE(rhs); 
  }

  template <size_t _Nb>
  inline bool bits<_Nb>::signedLT (const bits<_Nb> &rhs) const
  { 
    return Base::signedLT(rhs); 
  }

  template <size_t _Nb>
  inline bool bits<_Nb>::signedGT (const bits<_Nb> &rhs) const
  { 
    return Base::signedGT(rhs); 
  }

  template <size_t _Nb>
  inline bool bits<_Nb>::signedGE (const bits<_Nb> &rhs) const
  { 
    return Base::signedGE(rhs); 
  }
    
  // comparing diff sized bits's
  template <size_t _Nb>
  template <size_t _Nb2>
  inline bool bits<_Nb>::signedLE (const bits<_Nb2> & rhs) const
  {
    return Base::signedLE(rhs);
  }
   
  template <size_t _Nb>
  template <size_t _Nb2>
  inline bool bits<_Nb>::signedLT (const bits<_Nb2> & rhs) const
  {
    return Base::signedLT(rhs);
  }
    
  template <size_t _Nb>
  template <size_t _Nb2>
  inline bool bits<_Nb>::signedGT (const bits<_Nb2> & rhs) const
  {
    return Base::signedGT(rhs);
  }
    
  template <size_t _Nb>
  template <size_t _Nb2>
  inline bool bits<_Nb>::signedGE (const bits<_Nb2> & rhs) const
  {
    return Base::signedGE(rhs);
  }

  template <size_t _Nb> template <size_t _Nb2>
  inline bits<_Nb2> bits<_Nb>::signExtend() const
  {
    bits<_Nb2> res(Base::template signExtend<_Nb2>());
# ifndef NO_PROPAGATE
    res._mask = _mask;
# endif
    return res;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator-=(uint64_t rhs)
  {
    Base::operator-=(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator-=(int32_t rhs)
  {
    Base::operator-=(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator-=(int64_t rhs)
  {
    Base::operator-=(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator-=(uint32_t rhs)
  {
    Base::operator-=(rhs);
    return *this;
  }
  
  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator-=(const bits<_Nb> &rhs)
  {
    Base::operator-=(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator+=(uint64_t rhs)
  {
    Base::operator+=(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator+=(int32_t rhs)
  {
    Base::operator+=(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator+=(int64_t rhs)
  {
    Base::operator+=(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator+=(uint32_t rhs)
  {
    Base::operator+=(rhs);
    return *this;
  }
  
  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator+=(const bits<_Nb> &rhs)
  {
    Base::operator+=(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator*=(uint64_t rhs)
  {
    Base::operator*=(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }
 
  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator*=(int32_t rhs)
  {
    Base::operator*=(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator*=(int64_t rhs)
  {
    Base::operator*=(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }
 
  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator*=(uint32_t rhs)
  {
    Base::operator*=(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }
 
  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator*=(const bits<_Nb> &rhs)
  {
    Base::operator*=(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }
  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::signedMultiplyEqual(uint64_t rhs)
  {
    Base::signedMultiplyEqual(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::signedMultiplyEqual(const bits<_Nb> &rhs)
  {
    Base::signedMultiplyEqual(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator/=(const bits<_Nb> &rhs)
  {
    Base::operator/=(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator/=(uint64_t rhs)
  {
    Base::operator/=(rhs);
    return *this;
  }


  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator/=(int32_t rhs)
  {
    Base::operator/=(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator/=(int64_t rhs)
  {
    Base::operator/=(rhs);
    return *this;
  }


  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator/=(uint32_t rhs)
  {
    Base::operator/=(rhs);
    return *this;
  }
  
  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::signedDivideEqual(const bits<_Nb> &rhs)
  {
    Base::signedDivideEqual(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }
  
  template <size_t _Nb> template<size_t _Nb2>
  inline bits<_Nb>& bits<_Nb>::signedDivideEqual(const bits<_Nb2>& rhs) 
  {
    Base::signedDivideEqual(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }



  template <size_t _Nb>
  inline bits<MAX(_Nb,64)>& bits<_Nb>::signedDivideEqual(uint64_t rhs)
  {
    Base::signedDivideEqual(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline bits<MAX(_Nb,32)>& bits<_Nb>::signedDivideEqual(uint32_t rhs)
  {
    Base::signedDivideEqual(rhs);
    return *this;
  }
  
  template <size_t _Nb> template<size_t _Nb2>
  inline bits<_Nb>& bits<_Nb>::operator/=(const bits<_Nb2> & rhs)
  {
    Base::operator/=(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }

  template <size_t _Nb> template<size_t _Nb2>
  inline bits<_Nb>& bits<_Nb>::operator%=(const bits<_Nb2> & rhs)
  {
    Base::operator%=(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator%=(const bits<_Nb> &rhs)
  {
    Base::operator%=(rhs);
# ifndef NO_PROPAGATE
    propagate_undefined(rhs);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator%=(uint64_t rhs)
  {
    Base::operator%=(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator%=(int32_t rhs)
  {
    Base::operator%=(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator%=(int64_t rhs)
  {
    Base::operator%=(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator%=(uint32_t rhs)
  {
    Base::operator%=(rhs);
    return *this;
  }
  
  // bitwise (logical) operations on bits's
  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator&=(const bits<_Nb> &rhs)
  {
    Base::operator&=(rhs);
# ifndef NO_PROPAGATE
    union_undefined(rhs);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator|=(const bits<_Nb> &rhs)
  {
    Base::operator|=(rhs);
# ifndef NO_PROPAGATE
    union_undefined(rhs);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator^=(const bits<_Nb> &rhs)
  {
    Base::operator^=(rhs);
# ifndef NO_PROPAGATE
    union_undefined(rhs);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator<<=(size_t position)
  {
    Base::operator<<=(position);
# ifndef NO_PROPAGATE
    _mask.operator<<=(position);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::operator>>=(size_t position)
  {
    Base::operator>>=(position);
# ifndef NO_PROPAGATE
    _mask.operator>>=(position);
# endif
    return *this;
  }

  template <size_t _Nb>
  inline bits<_Nb>& bits<_Nb>::signedRightShiftEqual(size_t position)
  {
    Base::signedRightShiftEqual(position);
# ifndef NO_PROPAGATE
    _mask.signedRightShiftEqual(position);
# endif
    return *this;
  }
 
  template <size_t _Nb> template<size_t _Nb2>
  inline bits<_Nb> bits<_Nb>::operator<<(const bits<_Nb2>& position) const
  {
    return operator<<(position.uint32());
  }


  template <size_t _Nb>
  inline bits<_Nb> bits<_Nb>::operator<<(size_t position) const
  { 
    return bits<_Nb>(*this) <<= position; 
  }

  template <size_t _Nb> template<size_t _Nb2>
  inline bits<_Nb> bits<_Nb>::operator>>(const bits<_Nb2>& position) const
  {
    return operator>>(position.uint32());  
  }
    
  template <size_t _Nb>
  inline bits<_Nb> bits<_Nb>::operator>>(size_t position) const
  { 
    return bits<_Nb>(*this) >>= position; 
  }

  template <size_t _Nb> template<size_t _Nb2>
  inline bits<_Nb> bits<_Nb>::left_rotate(const bits<_Nb2>& position) const
  {
    return left_rotate(position.uint32());  
  }

  template <size_t _Nb>
  inline bits<_Nb> bits<_Nb>::left_rotate (size_t position) const
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = Base::left_rotate(position);
    res._mask.left_rotate(position);
    return res;
# else
    return Base::left_rotate(position);
# endif
  }

  template <size_t _Nb> template<size_t _Nb2>
  inline bits<_Nb> bits<_Nb>::right_rotate(const bits<_Nb2>& position) const
  {
    return right_rotate(position.uint32());  
  }

  template <size_t _Nb>
  inline bits<_Nb> bits<_Nb>::right_rotate(size_t position) const
  {   
# ifndef NO_PROPAGATE
    bits<_Nb> res = Base::right_rotate(position);
    res._mask.right_rotate(position);
    return res;
# else
    return Base::right_rotate(position);
# endif
  }

  template <size_t _Nb>
  inline bits<_Nb> bits<_Nb>::swap_bytes() const
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = Base::swap_bytes();
    res._mask.swap_bytes();
    return res;
# else
    return Base::swap_bytes();
# endif
  }

  template <size_t _Nb>
  inline bits<_Nb> bits<_Nb>::swap_words() const
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = Base::swap_words();
    res._mask.swap_words();
    return res;
# else
    return Base::swap_words();
# endif
  }

  //
  // Carry generation
  //
  template <size_t Nb1> 
  inline bits<Nb1> Carry (const bits<Nb1>& a,unsigned b, const bits<1>&cin)
  {
    bits<Nb1> tmpb (b);
    return Carry (a,tmpb,cin);
  }

  template <size_t Nb1, size_t Nb2> 
  inline bits<MAX(Nb1,Nb2)> Carry (const bits<Nb1>& a,const bits<Nb2>& b, const bits<1>&cin)
  {
# ifndef NO_PROPAGATE
    bits<MAX(Nb1,Nb2)> r = Carry(static_cast<intbv<Nb1> >(a),
                                 static_cast<intbv<Nb2> >(b),
                                 static_cast<intbv<1>   >(cin));
    r.propagate_undefined(a,b,cin);
    return r;
# else
    return Carry(static_cast<intbv<Nb1> >(a),
                 static_cast<intbv<Nb2> >(b),
                 static_cast<intbv<1>   >(cin));
# endif
  }

  //
  // Concatenation.
  //

  template <size_t _Nb1, size_t _Nb2>
  inline bits< _Nb1 + _Nb2> concat (const bits<_Nb1>& x, const bits<_Nb2>& y)
  {
# ifndef NO_PROPAGATE  
    bits< _Nb1 + _Nb2> res = concat(static_cast<intbv<_Nb1> >(x),
                                    static_cast<intbv<_Nb2> >(y));
    res._mask = concat(x._mask,y._mask);
    return res;
# else
    return concat(static_cast<intbv<_Nb1> >(x),
                  static_cast<intbv<_Nb2> >(y));
# endif
  }

  template <size_t _Nb1>
  inline bits< _Nb1 + _INTBV_BITS_PER_WORD> concat (const bits<_Nb1>& x, unsigned y)
  {
   bits<_INTBV_BITS_PER_WORD> tmp_y (y);
   return concat(x,y); 
  }

  template <size_t _Nb1, size_t _Nb2, size_t _Nb3>
  inline bits<_Nb1+_Nb2+_Nb3> concat (const bits<_Nb1>& x, const bits<_Nb2>& y,const  bits<_Nb3>& z)
  {
# ifndef NO_PROPAGATE
    bits< _Nb1 + _Nb2 + _Nb3> res = concat(static_cast<intbv<_Nb1> >(x),
                                           static_cast<intbv<_Nb2> >(y),
                                           static_cast<intbv<_Nb3> >(z)
                                           );
    res._mask = concat(x._mask,y._mask,z._mask);
    return res;
# else
    return concat(static_cast<intbv<_Nb1> >(x),
                  static_cast<intbv<_Nb2> >(y),
                  static_cast<intbv<_Nb3> >(z)
                  );
# endif
  }

  template <size_t _Nb1, size_t _Nb2, size_t _Nb3,size_t _Nb4>
  inline bits<_Nb1+_Nb2+_Nb3+_Nb4> concat (const bits<_Nb1>& w, const bits<_Nb2>& x,const bits<_Nb3>& y,const bits<_Nb4>& z)
  {
# ifndef NO_PROPAGATE
    bits< _Nb1 + _Nb2 + _Nb3 + _Nb4> res = concat(static_cast<intbv<_Nb1> >(w),
                                                  static_cast<intbv<_Nb2> >(x),
                                                  static_cast<intbv<_Nb3> >(y),
                                                  static_cast<intbv<_Nb4> >(z)
                                                  );
    res._mask = concat(w._mask,x._mask,y._mask,z._mask);
    return res;
# else
    return concat(static_cast<intbv<_Nb1> >(w),
                  static_cast<intbv<_Nb2> >(x),
                  static_cast<intbv<_Nb3> >(y),
                  static_cast<intbv<_Nb4> >(z)
                  );
# endif
  }

  template <size_t _Nb1, size_t _Nb2, size_t _Nb3,size_t _Nb4,size_t _Nb5>
  inline bits<_Nb1+_Nb2+_Nb3+_Nb4+_Nb5> concat (const bits<_Nb1>& v,const  bits<_Nb2>& w,const  bits<_Nb3>& x,const bits<_Nb4>& y,const bits<_Nb5>& z)
  {
# ifndef NO_PROPAGATE
    bits< _Nb1 + _Nb2 + _Nb3 + _Nb4 + _Nb5> res = concat(static_cast<intbv<_Nb1> >(v),
                                                         static_cast<intbv<_Nb2> >(w),
                                                         static_cast<intbv<_Nb3> >(x),
                                                         static_cast<intbv<_Nb4> >(y),
                                                         static_cast<intbv<_Nb5> >(z)
                                                         );
    res._mask = concat(v._mask,w._mask,x._mask,y._mask,z._mask);
    return res;
# else
    return concat(static_cast<intbv<_Nb1> >(v),
                  static_cast<intbv<_Nb2> >(w),
                  static_cast<intbv<_Nb3> >(x),
                  static_cast<intbv<_Nb4> >(y),
                  static_cast<intbv<_Nb5> >(z)
                  );
# endif
  }

  template <size_t _Nb1, size_t _Nb2, size_t _Nb3,size_t _Nb4,size_t _Nb5,size_t _Nb6>
  inline bits<_Nb1+_Nb2+_Nb3+_Nb4+_Nb5+_Nb6> concat (const bits<_Nb1>& u,const  bits<_Nb2>& v, const bits<_Nb3>& w,const bits<_Nb4>& x,const bits<_Nb5>& y, const bits<_Nb6>& z)
  {
# ifndef NO_PROPAGATE
    bits< _Nb1 + _Nb2 + _Nb3 + _Nb4 + _Nb5 + _Nb6> res = concat(static_cast<intbv<_Nb1> >(u),
                                                                static_cast<intbv<_Nb2> >(v),
                                                                static_cast<intbv<_Nb3> >(w),
                                                                static_cast<intbv<_Nb4> >(x),
                                                                static_cast<intbv<_Nb5> >(y),
                                                                static_cast<intbv<_Nb6> >(z)
                                                                );
    res._mask = concat(u._mask,v._mask,w._mask,x._mask,y._mask,z._mask);
    return res;
# else
    return concat(static_cast<intbv<_Nb1> >(u),
                  static_cast<intbv<_Nb2> >(v),
                  static_cast<intbv<_Nb3> >(w),
                  static_cast<intbv<_Nb4> >(x),
                  static_cast<intbv<_Nb5> >(y),
                  static_cast<intbv<_Nb6> >(z)
                  );
# endif
  }

  //
  // Binary operators for 'bits'.
  //

  // Operator -

  template<size_t _Nb>
  inline bits<_Nb> operator-(const bits<_Nb>& x, const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = operator-(static_cast<intbv<_Nb> >(x),
                              static_cast<intbv<_Nb> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return operator-(static_cast<intbv<_Nb> >(x),
                     static_cast<intbv<_Nb> >(y));
# endif
  }

  template<size_t _Nb1, size_t _Nb2>
  inline bits<MAX(_Nb1,_Nb2)> operator-(const bits<_Nb1>& x, const bits<_Nb2>& y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb1,_Nb2)> res = operator-(static_cast<intbv<_Nb1> >(x),
                                         static_cast<intbv<_Nb2> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return operator-(static_cast<intbv<_Nb1> >(x),
                     static_cast<intbv<_Nb2> >(y));
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator-(uint64_t x, const bits<_Nb>& y) {
    return operator-(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator-(const bits<_Nb>& x,uint64_t y) {
    return operator-(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator-(int64_t x, const bits<_Nb>& y) {
    return operator-(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator-(const bits<_Nb>& x,int64_t y) {
    return operator-(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator-(uint32_t x, const bits<_Nb>& y) {
    return operator-(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator-(const bits<_Nb>& x,uint32_t y) {
    return operator-(x,static_cast<bits<32> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator-(int32_t x, const bits<_Nb>& y) {
    return operator-(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator-(const bits<_Nb>& x,int32_t y) {
    return operator-(x,static_cast<bits<32> >(y));
  }

  // Operator +

  template<size_t _Nb>
  inline bits<_Nb> operator+(const bits<_Nb>& x, const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = operator+(static_cast<intbv<_Nb> >(x),
                              static_cast<intbv<_Nb> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return operator+(static_cast<intbv<_Nb> >(x),
                     static_cast<intbv<_Nb> >(y));
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator+(uint64_t x, const bits<_Nb>& y) {
    return operator+(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator+(const bits<_Nb>& x,uint64_t y) {
    return operator+(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator+(int64_t x, const bits<_Nb>& y) {
    return operator+(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator+(const bits<_Nb>& x,int64_t y) {
    return operator+(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator+(uint32_t x, const bits<_Nb>& y) {
    return operator+(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator+(const bits<_Nb>& x,uint32_t y) {
    return operator+(x,static_cast<bits<32> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator+(int32_t x, const bits<_Nb>& y) {
    return operator+(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator+(const bits<_Nb>& x,int32_t y) {
    return operator+(x,static_cast<bits<32> >(y));
  }
 

  template<size_t _Nb1, size_t _Nb2>
  inline bits<MAX(_Nb1,_Nb2)> operator+(const bits<_Nb1>& x, const bits<_Nb2>& y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb1,_Nb2)> res = operator+(static_cast<intbv<_Nb1> >(x),
                                         static_cast<intbv<_Nb2> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return operator+(static_cast<intbv<_Nb1> >(x),
                     static_cast<intbv<_Nb2> >(y));
# endif
  }

  // Operator *

  template<size_t _Nb>
  inline bits<_Nb> operator*(const bits<_Nb>& x, const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = operator*(static_cast<intbv<_Nb> >(x),
                              static_cast<intbv<_Nb> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return operator*(static_cast<intbv<_Nb> >(x),
                     static_cast<intbv<_Nb> >(y));
# endif
  }

  template<size_t _Nb1, size_t _Nb2>
  inline bits<MAX(_Nb1,_Nb2)> operator*(const bits<_Nb1>& x, const bits<_Nb2>& y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb1,_Nb2)> res = operator*(static_cast<intbv<_Nb1> >(x),
                                         static_cast<intbv<_Nb2> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return operator*(static_cast<intbv<_Nb1> >(x),
                     static_cast<intbv<_Nb2> >(y));
# endif
  }
 
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator*(uint64_t x, const bits<_Nb>& y) {
    return operator*(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator*(const bits<_Nb>& x,uint64_t y) {
    return operator*(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator*(int64_t x, const bits<_Nb>& y) {
    return operator*(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator*(const bits<_Nb>& x,int64_t y) {
    return operator*(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator*(uint32_t x, const bits<_Nb>& y) {
    return operator*(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator*(const bits<_Nb>& x,uint32_t y) {
    return operator*(x,static_cast<bits<32> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator*(int32_t x, const bits<_Nb>& y) {
    return operator*(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator*(const bits<_Nb>& x,int32_t y) {
    return operator*(x,static_cast<bits<32> >(y));
  } 
 
  // Operator signedMultiply

  template<size_t _Nb>
  inline bits<_Nb> signedMultiply(const bits<_Nb>& x, const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = signedMultiply(static_cast<intbv<_Nb> >(x),
                                   static_cast<intbv<_Nb> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return signedMultiply(static_cast<intbv<_Nb> >(x),
                          static_cast<intbv<_Nb> >(y));
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> signedMultiply(const bits<_Nb>& x, uint64_t y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb,64)> res = signedMultiply(static_cast<intbv<_Nb> >(x),y);
    res.propagate_undefined(x);
    return res;
# else
    return signedMultiply(static_cast<intbv<_Nb> >(x),y);
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> signedMultiply(uint64_t x,const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb,64)> res = signedMultiply(x,static_cast<intbv<_Nb> >(y));
    res.propagate_undefined(y);
    return res;
# else
    return signedMultiply(x,static_cast<intbv<_Nb> >(y));
# endif
  }

  template<size_t _Nb1, size_t _Nb2>
  inline bits<MAX(_Nb1,_Nb2)> signedMultiply(const bits<_Nb1>& x, const bits<_Nb2>& y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb1,_Nb2)> res = signedMultiply(static_cast<intbv<_Nb1> >(x),
                                              static_cast<intbv<_Nb2> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return signedMultiply(static_cast<intbv<_Nb1> >(x),
                          static_cast<intbv<_Nb2> >(y));
# endif
  }

  // Operator /

  template<size_t _Nb>
  inline bits<_Nb> operator/(const bits<_Nb>& x, const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = operator/(static_cast<intbv<_Nb> >(x),
                              static_cast<intbv<_Nb> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return operator/(static_cast<intbv<_Nb> >(x),
                     static_cast<intbv<_Nb> >(y));
# endif
  }

  template<size_t _Nb1, size_t _Nb2>
  inline bits<MAX(_Nb1,_Nb2)> operator/(const bits<_Nb1>& x, const bits<_Nb2>& y)
  {
# ifndef NO_PROPAGATE
#   ifdef IS_64BIT
      // Hack for compiler aliasing bug.
      y.dummy();
#   endif
    bits<MAX(_Nb1,_Nb2)> res = operator/(static_cast<intbv<_Nb1> >(x),
                                         static_cast<intbv<_Nb2> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return operator/(static_cast<intbv<_Nb1> >(x),
                     static_cast<intbv<_Nb2> >(y));
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator/(uint64_t x, const bits<_Nb>& y) {
    return operator/(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator/(const bits<_Nb>& x,uint64_t y) {
    return operator/(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator/(int64_t x, const bits<_Nb>& y) {
    return operator/(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator/(const bits<_Nb>& x,int64_t y) {
    return operator/(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator/(uint32_t x, const bits<_Nb>& y) {
    return operator/(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator/(const bits<_Nb>& x,uint32_t y) {
    return operator/(x,static_cast<bits<32> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator/(int32_t x, const bits<_Nb>& y) {
    return operator/(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator/(const bits<_Nb>& x,int32_t y) {
    return operator/(x,static_cast<bits<32> >(y));
  }

  // Operator signedDivide

  template<size_t _Nb>
  inline bits<_Nb> signedDivide(const bits<_Nb>& x, const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = signedDivide(static_cast<intbv<_Nb> >(x),
                                 static_cast<intbv<_Nb> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return signedDivide(static_cast<intbv<_Nb> >(x),
                        static_cast<intbv<_Nb> >(y));
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> signedDivide(const bits<_Nb>& x, uint64_t y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb,64)> res = signedDivide(static_cast<intbv<_Nb> >(x),y);
    res.propagate_undefined(x);
    return res;
# else
    return signedDivide(static_cast<intbv<_Nb> >(x),y);
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> signedDivide(uint32_t x,const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb,32)> res = signedDivide(x,static_cast<intbv<_Nb> >(y));
    res.propagate_undefined(y);
    return res;
# else
    return signedDivide(x,static_cast<intbv<_Nb> >(y));
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> signedDivide(const bits<_Nb>& x, uint32_t y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb,32)> res = signedDivide(static_cast<intbv<_Nb> >(x),y);
    res.propagate_undefined(x);
    return res;
# else
    return signedDivide(static_cast<intbv<_Nb> >(x),y);
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> signedDivide(uint64_t x,const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb,64)> res = signedDivide(x,static_cast<intbv<_Nb> >(y));
    res.propagate_undefined(y);
    return res;
# else
    return signedDivide(x,static_cast<intbv<_Nb> >(y));
# endif
  }
  
  template<size_t _Nb1, size_t _Nb2>
  inline bits<MAX(_Nb1,_Nb2)> signedDivide(const bits<_Nb1>& x, const bits<_Nb2>& y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb1,_Nb2)> res = signedDivide(static_cast<intbv<_Nb1> >(x),
                                            static_cast<intbv<_Nb2> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return signedDivide(static_cast<intbv<_Nb1> >(x),
                        static_cast<intbv<_Nb2> >(y));
# endif
  }

  // Operator %

  template<size_t _Nb>
  inline bits<_Nb> operator%(const bits<_Nb>& x, const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = operator%(static_cast<intbv<_Nb> >(x),
                              static_cast<intbv<_Nb> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return operator%(static_cast<intbv<_Nb> >(x),
                     static_cast<intbv<_Nb> >(y));
# endif
  }

  template<size_t _Nb1, size_t _Nb2>
  inline bits<MAX(_Nb1,_Nb2)> operator%(const bits<_Nb1>& x, const bits<_Nb2>& y)
  {
# ifndef NO_PROPAGATE
    #   ifdef IS_64BIT
      // Hack for compiler aliasing bug.
      y.dummy();
#   endif

    
    bits<MAX(_Nb1,_Nb2)> res = operator%(static_cast<intbv<_Nb1> >(x),
                                         static_cast<intbv<_Nb2> >(y));
    res.propagate_undefined(x,y);
    return res;
# else
    return operator%(static_cast<intbv<_Nb1> >(x),
                     static_cast<intbv<_Nb2> >(y));
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator%(uint64_t x, const bits<_Nb>& y) {
    return operator%(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator%(const bits<_Nb>& x,uint64_t y) {
    return operator%(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator%(int64_t x, const bits<_Nb>& y) {
    return operator%(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator%(const bits<_Nb>& x,int64_t y) {
    return operator%(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator%(uint32_t x, const bits<_Nb>& y) {
    return operator%(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator%(const bits<_Nb>& x,uint32_t y) {
    return operator%(x,static_cast<bits<32> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator%(int32_t x, const bits<_Nb>& y) {
    return operator%(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator%(const bits<_Nb>& x,int32_t y) {
    return operator%(x,static_cast<bits<32> >(y));
  }

  // Operator &

  template<size_t _Nb>
  inline bits<_Nb> operator&(const bits<_Nb>& x, const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = operator&(static_cast<intbv<_Nb> >(x),
                              static_cast<intbv<_Nb> >(y));
    res.union_undefined(x,y);
    return res;
# else
    return operator&(static_cast<intbv<_Nb> >(x),
                     static_cast<intbv<_Nb> >(y));
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator&(uint64_t x, const bits<_Nb>& y) {
    return operator&(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator&(const bits<_Nb>& x,uint64_t y) {
    return operator&(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator&(int64_t x, const bits<_Nb>& y) {
    return operator&(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator&(const bits<_Nb>& x,int64_t y) {
    return operator&(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator&(uint32_t x, const bits<_Nb>& y) {
    return operator&(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator&(const bits<_Nb>& x,uint32_t y) {
    return operator&(x,static_cast<bits<32> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator&(int32_t x, const bits<_Nb>& y) {
    return operator&(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator&(const bits<_Nb>& x,int32_t y) {
    return operator&(x,static_cast<bits<32> >(y));
  }

  template<size_t _Nb1, size_t _Nb2>
  inline bits<MAX(_Nb1,_Nb2)> operator&(const bits<_Nb1>& x,const  bits<_Nb2>& y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb1,_Nb2)> res = operator&(static_cast<intbv<_Nb1> >(x),
                                         static_cast<intbv<_Nb2> >(y));
    res.union_undefined(x,y);
    return res;
# else
    return operator&(static_cast<intbv<_Nb1> >(x),
                     static_cast<intbv<_Nb2> >(y));
# endif
  }


  // Operator |

  template<size_t _Nb>
  inline bits<_Nb> operator|(const bits<_Nb>& x, const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = operator|(static_cast<intbv<_Nb> >(x),
                              static_cast<intbv<_Nb> >(y));
    res.union_undefined(x,y);
    return res;
# else
    return operator|(static_cast<intbv<_Nb> >(x),
                     static_cast<intbv<_Nb> >(y));
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator|(uint64_t x, const bits<_Nb>& y) {
    return operator|(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator|(const bits<_Nb>& x,uint64_t y) {
    return operator|(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator|(int64_t x, const bits<_Nb>& y) {
    return operator|(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator|(const bits<_Nb>& x,int64_t y) {
    return operator|(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator|(uint32_t x, const bits<_Nb>& y) {
    return operator|(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator|(const bits<_Nb>& x,uint32_t y) {
    return operator|(x,static_cast<bits<32> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator|(int32_t x, const bits<_Nb>& y) {
    return operator|(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator|(const bits<_Nb>& x,int32_t y) {
    return operator|(x,static_cast<bits<32> >(y));
  }

  template<size_t _Nb1, size_t _Nb2>
  inline bits<MAX(_Nb1,_Nb2)> operator|(const bits<_Nb1>& x, const bits<_Nb2>& y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb1,_Nb2)> res = operator|(static_cast<intbv<_Nb1> >(x),
                                         static_cast<intbv<_Nb2> >(y));
    res.union_undefined(x,y);
    return res;
# else
    return operator|(static_cast<intbv<_Nb1> >(x),
                     static_cast<intbv<_Nb2> >(y));
# endif
  }

  // Operator ^

  template<size_t _Nb>
  inline bits<_Nb> operator^(const bits<_Nb>& x, const bits<_Nb>& y)
  {
# ifndef NO_PROPAGATE
    bits<_Nb> res = operator^(static_cast<intbv<_Nb> >(x),
                              static_cast<intbv<_Nb> >(y));
    res.union_undefined(x,y);
    return res;
# else
    return operator^(static_cast<intbv<_Nb> >(x),
                     static_cast<intbv<_Nb> >(y));
# endif
  }

  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator^(uint64_t x, const bits<_Nb>& y) {
    return operator^(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator^(const bits<_Nb>& x,uint64_t y) {
    return operator^(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator^(int64_t x, const bits<_Nb>& y) {
    return operator^(static_cast<bits<64> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,64)> operator^(const bits<_Nb>& x,int64_t y) {
    return operator^(x,static_cast<bits<64> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator^(uint32_t x, const bits<_Nb>& y) {
    return operator^(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator^(const bits<_Nb>& x,uint32_t y) {
    return operator^(x,static_cast<bits<32> >(y));
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator^(int32_t x, const bits<_Nb>& y) {
    return operator^(static_cast<bits<32> >(x),y);
  }
  template<size_t _Nb>
  inline bits<MAX(_Nb,32)> operator^(const bits<_Nb>& x,int32_t y) {
    return operator^(x,static_cast<bits<32> >(y));
  }

  template<size_t _Nb1, size_t _Nb2>
  inline bits<MAX(_Nb1,_Nb2)> operator^(const bits<_Nb1>& x, const bits<_Nb2>& y)
  {
# ifndef NO_PROPAGATE
    bits<MAX(_Nb1,_Nb2)> res = operator^(static_cast<intbv<_Nb1> >(x),
                                         static_cast<intbv<_Nb2> >(y));
    res.union_undefined(x,y);
    return res;
# else
    return operator^(static_cast<intbv<_Nb1> >(x),
                     static_cast<intbv<_Nb2> >(y));
# endif
  }

  // Stream reading.
  template <size_t _Nb>
  std::istream& operator>>( std::istream& is, bits<_Nb> &x) 
  {
    operator>>(is,static_cast<intbv<_Nb> >(x));
    return is; 
  }

  // Stream writing.
  template <size_t _Nb>
  std::ostream& operator<<( std::ostream& os, const bits<_Nb> &x) 
  {
    operator<<(os,static_cast<intbv<_Nb> >(x));
    return os; 
  }

}
