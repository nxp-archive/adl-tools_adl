//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _INTBV_IMPL
#define _INTBV_IMPL
namespace adl {

  // Construct from string.  Where radix is explicitly set, the input should
  // contain a prefix, e.g. 0x...
  template <size_t _Nb>
  inline intbv<_Nb>::intbv( const char* s) 
  {
    _Base::_readNumber (s,_Nb); 
    _sanitize();
  }

  template <size_t _Nb>
  inline intbv<_Nb>::intbv( const std::string& s)  
  {
    _Base::_readNumber (s,_Nb); 
    _sanitize();
  }

  template <size_t _Nb>
  inline intbv<_Nb>::intbv( const std::string& s, int format)   
  {
    _Base::_readNumber (s,_Nb,format); 
    _sanitize(); 
  }

  template <size_t _Nb>
  inline intbv<_Nb>::intbv( const std::istream& is)  
  {
    _Base::_readNumber (is,_Nb); 
    _sanitize();
  }

  // copy (diff size)
  template <size_t _Nb> template <size_t _Nb2>
  inline intbv<_Nb>::intbv (const intbv <_Nb2> &x) 
  { 
    if (_Nb >_Nb2) {   
      _Base::_construct_from_smaller_sized (x);
    } else if (_Nb < _Nb2) {
      _Base::_construct_from_larger_sized (x);   
    }
    _sanitize();
  }

#ifndef __NO_RNUMBER__
  template <size_t _Nb>
  inline intbv<_Nb>::intbv(const rnumber::RNumber &x)
  {
    _Base::_constructFromRNumber(x);
    _sanitize();
  }
#endif
    
  // operator= (diff size)
  template <size_t _Nb> template <size_t _Nb2>
  inline intbv<_Nb> &intbv<_Nb>::operator= (const intbv <_Nb2> &x)
  { 
    // These are constants so this branch will be folded out.
    if (_Nb >_Nb2) {   
      _Base::_construct_from_smaller_sized (x);
      _sanitize();
    } else {
      _Base::_construct_from_larger_sized (x);   
    }
    return *this;
  }

  template <size_t _Nb>
  inline intbv<_Nb> & intbv<_Nb>::set (size_t start, size_t stop,const intbv<_Nb>& x)
  { 
    _Base::_setField(_Nb,start % _Nb,stop % _Nb,x);
    return *this;
  }

  template <>
  inline intbv<0> & intbv<0>::set (size_t, size_t,const intbv<0>&)
  { 
    return *this;
  }


  template <size_t _Nb> template <size_t _Nb2>    
  inline intbv<_Nb> & intbv<_Nb>::set (size_t start, size_t stop, const intbv<_Nb2>& x)
  {    
    _Base::_setField(_Nb,start % _Nb,stop % _Nb,intbv<_Nb>(x));
    return *this;
  }

  template <> template <size_t _Nb2>    
  inline intbv<0> & intbv<0>::set (size_t start, size_t stop, const intbv<_Nb2>& x)
  {    
    return *this;
  }

  template <size_t _Nb> template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
  inline intbv<_Nb> & intbv<_Nb>::set (const intbv<_Nb1> &start, const intbv<_Nb2> &stop, const intbv<_Nb3> &x)
  {
    int a = start.uint32();
    int b = stop.uint32();
    set(a,b,x);
    return *this;
  }

  template <size_t _Nb>    
  inline intbv<_Nb> & intbv<_Nb>::mask (size_t start, size_t stop) 
  {
    // sets bits start:stop to 1
    // if stop < start, we wrap
    intbv<_Nb> x (0);
    x.set();
    if (stop >= start) {
      return set(start,stop,x);
    }
    else {
      set(start,_Nb-1,x);
      set(0,stop,x);   
    }
    return *this;
  }

  template <size_t _Nb> template <size_t _Nb1,size_t _Nb2>
  inline intbv<_Nb> & intbv<_Nb>::mask (const intbv<_Nb1>& start, const intbv<_Nb2>& stop)
  {
    return mask(start.uint32(),stop.uint32());
  }

  template <size_t _Nb>    
  inline intbv<_Nb> & intbv<_Nb>::mask_inv (size_t start, size_t stop) 
  {
    // sets bits start:stop to 0
    // if stop < start, we wrap
    intbv<_Nb> x (0);
    if (stop >= start) {
      return set(start,stop,x);
    }
    else {
      set(start,_Nb-1,x);
      set(0,stop,x);   
      return *this;
    }
  }
 
  template <size_t _Nb> template <size_t _Nb1,size_t _Nb2>
  inline intbv<_Nb> & intbv<_Nb>::mask_inv (const intbv<_Nb1>& start, const intbv<_Nb2>& stop)
  {
    return mask_inv(start.uint32(),stop.uint32());
  }

  // The simple_mask functions only work for start < stop.

  template <size_t _Nb>    
  inline intbv<_Nb> & intbv<_Nb>::simple_mask (size_t start, size_t stop) 
  {
    assert (start <= stop);
    intbv<_Nb> x (0);
    x.set();
    return set(start,stop,x);
  }

  template <size_t _Nb> template <size_t _Nb1,size_t _Nb2>
  inline intbv<_Nb> & intbv<_Nb>::simple_mask (const intbv<_Nb1>& start, const intbv<_Nb2>& stop)
  {
    return simple_mask(start.uint32(),stop.uint32());
  }

  template <size_t _Nb>    
  inline intbv<_Nb> & intbv<_Nb>::simple_mask_inv (size_t start, size_t stop) 
  {
    assert (start <= stop);
    intbv<_Nb> x (0);
    return set(start,stop,x);
  }
 
  template <size_t _Nb> template <size_t _Nb1,size_t _Nb2>
  inline intbv<_Nb> & intbv<_Nb>::simple_mask_inv (const intbv<_Nb1>& start, const intbv<_Nb2>& stop)
  {
    return simple_mask_inv(start.uint32(),stop.uint32());
  }
   
  // gets a field/slice (returns same size intbv w/ value right justified
  template <size_t _Nb>
  inline const intbv<_Nb> intbv<_Nb>::operator () (size_t start, size_t stop) const
  {
    return get(start,stop);
  }

  template <size_t _Nb>    
  inline const intbv<_Nb> intbv<_Nb>::get (size_t start, size_t stop) const
  {
    intbv<_Nb> res(*this);
    _Base::_getField(_Nb,start % _Nb,stop % _Nb,res);
    res._sanitize();
    return res; 
  }


  template <size_t _Nb> template <size_t _Nb1, size_t _Nb2>  
  inline const intbv<_Nb> intbv<_Nb>::operator () (const intbv<_Nb1>& start, const intbv<_Nb2>& stop) const
  {
    return get(start,stop);
  }


  template <size_t _Nb> template <size_t _Nb1, size_t _Nb2>
  inline const intbv<_Nb> intbv<_Nb>::get (const intbv<_Nb1>& start, const intbv<_Nb2>& stop) const
  {
    int a = start.uint32();
    int b = stop.uint32();
    return get(a,b);
  } 

  //  gets a field/slice (returns given size intbv w/ value right justified
  template <size_t _Nb> template <size_t _Nb2>
  inline const intbv<_Nb2> intbv<_Nb>::get(size_t start, size_t stop) const
  {
    // should this be overloaded, if so 2 more functions needed:
    //  1. getField (unsigned,unsigned) the work should be here, others just wrappers
    //  2. getField (intbv,intbv)
    intbv<_Nb> tmp(*this);
    _Base::_getField(_Nb,start % _Nb,stop % _Nb,tmp);
    return intbv<_Nb2>(tmp);
  }

  template <size_t _Nb> template <size_t _Nb1,size_t _Nb2,size_t _Nb3>
  inline const intbv<_Nb3> intbv<_Nb>::get (const intbv<_Nb1> &start,const intbv<_Nb2> &stop) const
  {
    return get(start.uint32(),stop.uint32());
  }

  // Helper sign-extend function- returns a number with a new size that is the
  // old number w/the msb sign extended.
  template <size_t _Nb> template <size_t _Nb2> 
  inline intbv<_Nb2> intbv<_Nb>::signExtend() const
  {
    intbv<_Nb2> x = *this;
    if (_Nb2 > _Nb) {
      if (get(0)) {
        x.simple_mask(0,(_Nb2-_Nb));
      }
    }
    return x;
  } 
 
  template <size_t _Nb>
  inline void intbv<_Nb>::signExtend(size_t bit) 
  {
    // it doesn't make sense to sign extend 1 bit intbvs
    if (_Nb == 1) {
      return;
    }
    if (get(bit)) {
      this->simple_mask(0,bit);
    }
    else this->simple_mask_inv(0,bit);
  }

  // comparing diff sized intbv's
  template<size_t _Nb> template<size_t _Nb2> 
  inline bool intbv<_Nb>::signedLE (const intbv<_Nb2>& rhs) const {
    CmpArgCheck(_Nb,_Nb2);
    intbv<MAX(_Nb,_Nb2)> tmp (0);
    if (_Nb > _Nb2) {
      tmp.set(_Nb-_Nb2,_Nb-1,rhs);
      tmp.signExtend(_Nb-_Nb2);
      return signedLE(tmp);
    }
    else {
      tmp.set(_Nb2-_Nb,_Nb2-1,*this);
      tmp.signExtend(_Nb2-_Nb);
      return tmp.signedLE(rhs);
    }
  }

  template<size_t _Nb> template<size_t _Nb2>
  inline bool intbv<_Nb>::signedLT (const intbv<_Nb2>& rhs) const {
    CmpArgCheck(_Nb,_Nb2);
    intbv<MAX(_Nb,_Nb2)> tmp (0);
    if (_Nb > _Nb2) {
      tmp.set(_Nb-_Nb2,_Nb-1,rhs);
      tmp.signExtend(_Nb-_Nb2);
      return signedLT(tmp);
    }
    else {
      tmp.set(_Nb2-_Nb,_Nb2-1,*this);
      tmp.signExtend(_Nb2-_Nb);
      return tmp.signedLT(rhs);
    }
  }

  template<size_t _Nb> template<size_t _Nb2>
  inline bool intbv<_Nb>::signedGT (const intbv<_Nb2>& rhs) const {
    CmpArgCheck(_Nb,_Nb2);
    intbv<MAX(_Nb,_Nb2)> tmp (0);
    if (_Nb > _Nb2) {
      tmp.set(_Nb-_Nb2,_Nb-1,rhs);
      tmp.signExtend(_Nb-_Nb2);
      return signedGT(tmp);
    }
    else {
      tmp.set(_Nb2-_Nb,_Nb2-1,*this);
      tmp.signExtend(_Nb2-_Nb);
      return tmp.signedGT(rhs);
    }
  }

  template<size_t _Nb> template<size_t _Nb2>
  inline bool intbv<_Nb>::signedGE (const intbv<_Nb2>& rhs) const {
    CmpArgCheck(_Nb,_Nb2);
    intbv<MAX(_Nb,_Nb2)> tmp (0);
    if (_Nb > _Nb2) {
      tmp.set(_Nb-_Nb2,_Nb-1,rhs);
      tmp.signExtend(_Nb-_Nb2);
      return signedGE(tmp);
    }
    else {
      tmp.set(_Nb2-_Nb,_Nb2-1,*this);
      tmp.signExtend(_Nb2-_Nb);
      return tmp.signedGE(rhs);
    }
  }

  //    Arithmetic operations on intbvs.
  // -
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator-=(int64_t rhs)
  {
    ArithArgCheck(_Nb,64);
    return operator-=(static_cast<intbv<MAX(_Nb,64)> >(rhs));
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator-=(uint64_t rhs)
  {
    ArithArgCheck(_Nb,64);
    return operator-=(static_cast<intbv<MAX(_Nb,64)> >(rhs));
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator-=(int32_t rhs)
  {
    ArithArgCheck(_Nb,32);
    return operator-=(static_cast<intbv<MAX(_Nb,32)> >(rhs));
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator-=(uint32_t rhs)
  {
    ArithArgCheck(_Nb,32);
    return operator-=(static_cast<intbv<MAX(_Nb,32)> >(rhs));
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator-=(const intbv<_Nb>& rhs)
  {
    // subtract with a same-sized intbv
    _Base::_subtract(rhs);
    _sanitize();
    return *this;
  }

  // +
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator+=(uint64_t rhs) {    
    ArithArgCheck(_Nb,64);
    return operator+=(static_cast<intbv<MAX(_Nb,64)> >(rhs));
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator+=(int64_t rhs)
  {
    ArithArgCheck(_Nb,64);
    return operator+=(static_cast<intbv<MAX(_Nb,64)> >(rhs));
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator+=(uint32_t rhs)
  {
    ArithArgCheck(_Nb,32);
    return operator+=(static_cast<intbv<MAX(_Nb,32)> >(rhs));
  }
  
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator+=(int32_t rhs)
  {
    ArithArgCheck(_Nb,32);
    return operator+=(static_cast<intbv<MAX(_Nb,32)> >(rhs));
  }
  
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator+=(const intbv<_Nb>& rhs)
  {
    // add with a same-sized intbv
    _Base::_add(rhs);
    _sanitize();
    return *this;
  }

  // *
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator*=(int64_t rhs)
  {
    ArithArgCheck(_Nb,64);
    return operator*=(static_cast<intbv<MAX(_Nb,64)> >(rhs));
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator*=(uint64_t rhs)
  {
    ArithArgCheck(_Nb,64);
    return operator*=(static_cast<intbv<MAX(_Nb,64)> >(rhs));
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator*=(int32_t rhs)
  {
    ArithArgCheck(_Nb,32);
    return operator*=(static_cast<intbv<MAX(_Nb,32)> >(rhs));
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator*=(uint32_t rhs)
  {
    ArithArgCheck(_Nb,32);
    return operator*=(static_cast<intbv<MAX(_Nb,32)> >(rhs));
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator*=(const intbv<_Nb>& rhs)
  {
    // multiply by a same-sized intbv
    _Base::_multiply(rhs);
    _sanitize();
    return *this;
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::signedMultiplyEqual(uint64_t rhs)
  {
    // multiply by an unsigned integer
    _Base::_multiplySigned(rhs);
    _sanitize();
    return *this;
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::signedMultiplyEqual(const intbv<_Nb>& rhs)
  {
    // multiply by a same-sized intbv
    _Base::_multiplySigned(rhs);
    _sanitize();
    return *this;
  }

  // Call base class method, assume that sizes are word aligned and sign is extended.
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::divideSignedAligned(const intbv<_Nb>& rhs)
  {
    // divide by a same-sized intbv
    _Base::_divideSigned(rhs);
    _sanitize();
    return *this;
  } 
  
  template <size_t _Nb>
  inline intbv<MAX(_Nb,64)>& intbv<_Nb>::signedDivideEqual(uint64_t rhs)
  {
    ArithArgCheck(_Nb,64);
    // divide by an unsigned integer
    intbv<_WORD_ALIGNED(MAX(_Nb,64))> tmp(*this);
    intbv<_WORD_ALIGNED(MAX(_Nb,64))> tmp_rhs((int64_t)rhs);
    if (_WORD_ALIGNED(MAX(_Nb,64)) > _Nb) {
      tmp.signExtend(_WORD_ALIGNED(MAX(_Nb,64)) - _Nb);
    }
    tmp.divideSignedAligned(tmp_rhs);
    *this = tmp;
     _sanitize();
    return *this;
  }

  template <size_t _Nb>
  inline intbv<MAX(_Nb,32)>& intbv<_Nb>::signedDivideEqual(uint32_t rhs)
  {
    ArithArgCheck(_Nb,32);
    // divide by an unsigned integer
    intbv<_WORD_ALIGNED(MAX(_Nb,32))> tmp(*this);
    intbv<_WORD_ALIGNED(MAX(_Nb,32))> tmp_rhs((int32_t)rhs);
    if (_WORD_ALIGNED(MAX(_Nb,32)) > _Nb) {
      tmp.signExtend(_WORD_ALIGNED(MAX(_Nb,32)) - _Nb);
    }
    tmp.divideSignedAligned(tmp_rhs);
    *this = tmp;
    _sanitize();
    return *this;
  } 
  
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::signedDivideEqual(const intbv<_Nb>& rhs)
  {
    // divide by a same-sized intbv
    intbv<_WORD_ALIGNED(_Nb)> tmp(*this);
    intbv<_WORD_ALIGNED(_Nb)> tmp_rhs(rhs);
    if (_WORD_ALIGNED(_Nb) > _Nb) {
      tmp.signExtend(_WORD_ALIGNED(_Nb) - _Nb);
      tmp_rhs.signExtend(_WORD_ALIGNED(_Nb) - _Nb);
    }    
    tmp.divideSignedAligned(tmp_rhs);
    *this = tmp;
    _sanitize();
    return *this;
  }
  
  template <size_t _Nb> template < size_t _Nb2>
  inline intbv<_Nb>& intbv<_Nb>::signedDivideEqual(const intbv<_Nb2>& rhs)
  {
    // divide by a same-sized intbv
    intbv<_WORD_ALIGNED(_Nb2)> tmp(*this);
    intbv<_WORD_ALIGNED(_Nb2)> tmp_rhs(rhs);
    if (_WORD_ALIGNED(_Nb2) > _Nb) {
      tmp.signExtend(_WORD_ALIGNED(_Nb2) - _Nb);
    }
    if (_WORD_ALIGNED(_Nb2) > _Nb2) {
      tmp_rhs.signExtend(_WORD_ALIGNED(_Nb2) - _Nb2);
    }    
    tmp.divideSignedAligned(tmp_rhs);
    *this = tmp;
    _sanitize();
    return *this;
  }
  
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator/=(const intbv<_Nb>& rhs)
  {
    // divide by a same-sized intbv
    _Base::_divide(rhs);
    _sanitize();
    return *this;
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator/=(int64_t rhs)
  {
    ArithArgCheck(_Nb,64);
    return operator/=(static_cast<intbv<MAX(_Nb,64)> >(rhs));
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator/=(uint64_t rhs)
  {
    ArithArgCheck(_Nb,64);
    return operator/=(static_cast<intbv<MAX(_Nb,64)> >(rhs));
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator/=(int32_t rhs)
  {
    ArithArgCheck(_Nb,32);
    return operator/=(static_cast<intbv<MAX(_Nb,32)> >(rhs));
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator/=(uint32_t rhs)
  {
    ArithArgCheck(_Nb,32);
    return operator/=(static_cast<intbv<MAX(_Nb,32)> >(rhs));
  }
 
  template <size_t _Nb> template<size_t _Nb2>
  inline intbv<_Nb>& intbv<_Nb>::operator/=(const intbv<_Nb2>& rhs)
  {
    ArithArgCheck(_Nb,_Nb2);
    // This and mod the only op= operators overloaded for diff-sized, the
    // others just use the diff size copy ctor and it works, but for
    // these, we must grow before t]he operation and truncate only
    // on the assigment, else we get incorrect results.

    // operates on different sized intbv's
    intbv <MAX(_Nb,_Nb2) > tmp_y (rhs);
    intbv <MAX(_Nb,_Nb2) > tmp_x (*this);
    tmp_x /= tmp_y;
    *this = tmp_x;
    _sanitize();
    return *this;
  }

  template <size_t _Nb> template<size_t _Nb2>
  inline intbv<_Nb>& intbv<_Nb>::operator%=(const intbv<_Nb2>& rhs)
  {
    ArithArgCheck(_Nb,_Nb2);
    // This and mod the only op= operators overloaded for diff-sized, the
    // others just use the diff size copy ctor and it works, but for
    // these, we must grow before the operation and truncate only
    // on the assigment, else we get incorrect results.

    // operates on different sized intbv's
    intbv <MAX(_Nb,_Nb2) > tmp_y (rhs);
    intbv <MAX(_Nb,_Nb2) > tmp_x (*this);
    tmp_x %= tmp_y;
    *this = tmp_x;
    _sanitize();
    return *this;
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator%=(const intbv<_Nb>& rhs)
  {
    // mod by a same-sized intbv
    // optimize me: let divide return the remainder as well
    //  as the quotient
    *this = *this - ( *this / rhs ) * rhs;
    _sanitize();
    return *this;
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator%=(int64_t rhs)
  {
    ArithArgCheck(_Nb,64);
    return operator%=(static_cast<intbv<MAX(_Nb,64)> >(rhs));
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator%=(uint64_t rhs)
  {
    ArithArgCheck(_Nb,64);
    return operator%=(static_cast<intbv<MAX(_Nb,64)> >(rhs));
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator%=(int32_t rhs)
  {
    ArithArgCheck(_Nb,32);
    return operator%=(static_cast<intbv<MAX(_Nb,32)> >(rhs));
  }
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator%=(uint32_t rhs)
  {
    ArithArgCheck(_Nb,32);
    return operator%=(static_cast<intbv<MAX(_Nb,32)> >(rhs));
  }

  // bitwise (logical) operations on intbv's
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator&=(const intbv<_Nb>& rhs)
  {
    _Base::_and(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator|=(const intbv<_Nb>& rhs)
  {
    _Base::_or(rhs);
    return *this;
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator^=(const intbv<_Nb>& rhs)
  {
    _Base::_xor(rhs);
    return *this;
  }

  //    Shift operations on intbvs.
  //  param:  position  The number of places to shift.      
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator<<=(size_t position)
  {
    if (EXPECT(position < _Nb, true))
      {
        _Base::_left_shift(position);
        _sanitize();
      }
    else
      _Base::_reset();
    return *this;
  }

  template <size_t _Nb> template <size_t _Nb2>
  inline intbv<_Nb>& intbv<_Nb>::operator<<=(const intbv<_Nb2>& position)
  {
    return *this <<= position.uint32();
  }

  template <size_t _Nb> template <size_t _Nb2>
  inline intbv<_Nb>& intbv<_Nb>::operator>>=(const intbv<_Nb2>& position)
  {
    return *this >>= position.uint32();
  }

  template <size_t _Nb> template <size_t _Nb2>
  inline intbv<_Nb>& intbv<_Nb>::signedRightShiftEqual(const intbv<_Nb2>& position)
  {
    return *this >>= position.uint32();
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::operator>>=(size_t position)
  {
    if (EXPECT(position < _Nb, true))
      {
        _Base::_right_shift(position);
        _sanitize();
      }
    else
      _Base::_reset();
    return *this;
  }


  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::signedRightShiftEqual(size_t position)
  {
    // This is a signed shift, so if we shift less than the size of the bit
    // vector, we sign extend (the sign bit is now at bit n, where n == the
    // right shift amount.  However, if we shift off the end, we still want to
    // propagate the sign bit, so we set all bits to 1 if the sign bit was
    // originally a 1, otherwise we leave it as 0.
    bool sbit = _Unchecked_test(0);
    *this >>= position;
    if (position < _Nb) {
      signExtend(position);
    } else if (sbit) {
      invert();
    }
    return *this;
  }

  template <size_t _Nb> template <size_t _Nb2>
  intbv<_Nb> intbv<_Nb>::left_rotate(const intbv<_Nb2>& position) const
  {
    return this->left_rotate(position.uint32());
  }

  template <size_t _Nb>
  inline intbv<_Nb> intbv<_Nb>::left_rotate (size_t position) const
  {
    // restrict position to be between 0:_Nb
    position %= _Nb;
    intbv<_Nb> res(*this);
    if (position && (position != _Nb))
      _Base::_left_rotate(position,_Nb,res);
    res._sanitize();
    return res;
  }

  template <size_t _Nb>
  inline intbv<_Nb> intbv<_Nb>::right_rotate(size_t position) const
  {   
    // restrict position to be between 0:_Nb
    position %= _Nb;
    intbv<_Nb> res(*this);
    if (position && (position != _Nb))
      _Base::_right_rotate(position,_Nb,res);
    res._sanitize();
    return res; 
  }

  template <size_t _Nb> template <size_t _Nb2>
  inline intbv<_Nb> intbv<_Nb>::right_rotate (const intbv<_Nb2>& position) const
  {
    return this->right_rotate(position.uint32());
  }

  template<size_t _Nb>
  inline intbv<_Nb> intbv<_Nb>::swap_bytes() const
  {
    // First, swap the bytes.
    intbv<_Nb>  res;
    _Base::_swap_bytes(res);
    // Now adjust for sub-word length issues if our size is not an even multiple
    // of words.
    if ( _Nb % _INTBV_BITS_PER_WORD ) {
      int adjust = (_INTBV_BITS_PER_WORD * _INTBV_WORDS(_Nb)) -  _Nb;
      res._right_shift(adjust);
    }

    return res;
  }

  template<size_t _Nb>
  inline intbv<_Nb> intbv<_Nb>::swap_words() const
  {
    // First, swap the bytes.
    intbv<_Nb>  res;
    int adjust = 0;
    if ( _Nb % _INTBV_BITS_PER_WORD ) {
      adjust = (_INTBV_BITS_PER_WORD * _INTBV_WORDS(_Nb)) -  _Nb;
    }
    _Base::_swap_words(res,adjust);
    // Now adjust for sub-word length issues if our size is not an even multiple
    // of words.
    if (adjust) {
      res._right_shift(adjust);
    }
    return res;
  }
 
  //  Versions of single-bit set, reset, invert, and getBit.
  //  They do no range checking. 
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::_Unchecked_set(size_t pos)
  {
    _Base::_getword(_Nb,pos) |= _Base::_maskwordbit(_Nb,pos);
    return *this;
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::_Unchecked_set(size_t pos, int val)
  {
    int mask = _Base::_maskwordbit(_Nb,pos);
    uint32_t w = _Base::_getword(_Nb,pos);
    if (val)
      w |= mask;
    else
      w &= ~mask;
    _Base::_setword(_Nb,pos,w);
    return *this;
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::_Unchecked_reset(size_t pos)
  {
    _Base::_getword(_Nb,pos) &= ~_Base::_maskwordbit(_Nb,pos);
    return *this;
  }

  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::_Unchecked_invert(size_t pos)
  {
    _Base::_getword(_Nb,pos) ^= _Base::_maskwordbit(_Nb,pos);
    return *this;
  }

  template <size_t _Nb>
  inline bool intbv<_Nb>::_Unchecked_test(size_t pos) const
  {
    unsigned w = _Base::_getword(_Nb,pos);
    unsigned m = _Base::_maskwordbit(_Nb,pos);
    return (w & m) != static_cast<unsigned>(0);
  }


  // Set, reset, and invert.  
  //Sets every bit to true.
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::set()
  {
    _Base::_set();
    _sanitize();
    return *this;
  }

  // Sets a given bit to a particular value.
  // param:  position  The index of the bit.
  // param:  val  Either true or false, defaults to true.    
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::set(size_t position, bool val)
  {
    assert(position < _Nb);
    return _Unchecked_set(position, val);
  }

  template <size_t _Nb> template <size_t _Nb1>
  inline intbv<_Nb>& intbv<_Nb>::set(const intbv<_Nb1>& position, bool val)
  {
    return _Unchecked_set(position.uint32(),val);
  }

  template <size_t _Nb> template <size_t _Nb1>
  inline intbv<_Nb>& intbv<_Nb>::set(size_t position,const intbv<_Nb1>& val)
  {
    return _Unchecked_set(position,val.uint32());
  }

  template <size_t _Nb> template <size_t _Nb1, size_t _Nb2>
  inline intbv<_Nb>& intbv<_Nb>::set(const intbv<_Nb1>& position,const intbv<_Nb2>& val)
  {
    return _Unchecked_set(position.uint32(),val[_Nb2-1].uint32());
  }

  // Sets every bit to false.
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::reset()
  {
    _Base::_reset();
    return *this;
  }

  template <size_t _Nb>
  std::string  intbv<_Nb>::str (int format) const
  { return _Base::_str(_Nb, format); }

  template <size_t _Nb>
  std::ostream&  intbv<_Nb>::printToOS( std::ostream& os) const
  {
    return _Base::_printToOS (os, _Nb); 
  }

  // Sets a given bit to false.
  // param:  position  The index of the bit.    
  //
  // Same as writing @c set(pos,false)
  //
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::reset(size_t position)
  {
    assert(position < _Nb);
    return _Unchecked_reset(position);
  }

  // Toggles every bit to its opposite value.
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::invert()
  {
    _Base::_invert();
    _sanitize();
    return *this;
  }

  // Toggles every bit to its opposite value.
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::negate()
  {
    _Base::_negate();
    _sanitize(); 
    return *this;  
  }


  // Toggles a given bit to its opposite value.
  // param:  position  The index of the bit.    
  template <size_t _Nb>
  inline intbv<_Nb>& intbv<_Nb>::invert(size_t position)
  {
    assert(position < _Nb);
    return _Unchecked_invert(position);
  }
    
  // Tests the value of a bit.
  // param:  position  The index of a bit.    
  // returns:  The value at pos.
  template <size_t _Nb>
  inline bool intbv<_Nb>::get(size_t position) const
  {
    assert(position < _Nb);
    return _Unchecked_test(position);
  }

  // Returns the carry out of each bit from an add operation.
  template <size_t Nb1>
  inline intbv<Nb1> Carry (const intbv<Nb1>& a,unsigned b, const intbv<1>& cin) 
  {
    intbv<Nb1> tmp_b (b);
    return Carry (a,tmp_b,cin);
  }


  // Returns the carry out of each bit from an add operation.
  template <size_t Nb1, size_t Nb2>
  inline intbv<MAX(Nb1,Nb2)> Carry (const intbv<Nb1>& a,const intbv<Nb2>& b, const intbv<1>& cin) 
  {
    ArithArgCheck(Nb1,Nb2);

    intbv<MAX(Nb1,Nb2)> car;
    intbv<MAX(Nb1,Nb2)> sum;
    
    sum = a + b + cin;
    car = (a&b) | ((a^b)&(~sum));
    return (car);
  }

  // global concatenation
  // 2 intbv's
  template <size_t _Nb1, size_t _Nb2>
  inline intbv< _Nb1 + _Nb2> concat (const intbv<_Nb1>& x,const  intbv<_Nb2>& y)
  {
    intbv<_Nb1+_Nb2> res (y);
    // use set field to copy in 
    res.set(0,_Nb1-1,x);
    return res;
  }

   // 1 intbv & 1 integer
  template <size_t _Nb1>
  inline intbv< _Nb1 + 32> concat (const intbv<_Nb1>& x,uint32_t y)
  {
    intbv<_Nb1+32> res (y);
    // use set field to copy in
    res.set(0,_Nb1-1,x);
    return res;
  }

  // 3 intbv's
  template <size_t _Nb1, size_t _Nb2, size_t _Nb3>
  inline intbv<_Nb1+_Nb2+_Nb3> concat (const intbv<_Nb1>& x, const intbv<_Nb2>& y, const intbv<_Nb3>& z)
  {

    intbv<_Nb1+_Nb2+_Nb3> res (z);
    res.set(_Nb1,_Nb1+_Nb2-1,y);
    res.set(0,_Nb1-1,x);
    return res;
  }
  // 4 intbv's
  template <size_t _Nb1, size_t _Nb2, size_t _Nb3,size_t _Nb4>
  inline intbv<_Nb1+_Nb2+_Nb3+_Nb4> concat (const intbv<_Nb1>& w, const intbv<_Nb2>& x,const intbv<_Nb3>& y,const intbv<_Nb4>& z)
  {

    intbv<_Nb1+_Nb2+_Nb3+_Nb4> res (z);
    res.set(_Nb1+_Nb2,       _Nb1 + _Nb2 + _Nb3 -1,     y);
    res.set(_Nb1,            _Nb1 + _Nb2 -1,  x);
    res.set(0,               _Nb1 -1,         w);

    return res;
  }

  // 5 intbv's
  template <size_t _Nb1, size_t _Nb2, size_t _Nb3,size_t _Nb4,size_t _Nb5>
  inline intbv<_Nb1+_Nb2+_Nb3+_Nb4+_Nb5> concat (const intbv<_Nb1>& v, const intbv<_Nb2>& w,const intbv<_Nb3>& x,const
                                                 intbv<_Nb4>& y,const intbv<_Nb5>& z)
  {

    intbv<_Nb1+_Nb2+_Nb3+_Nb4+_Nb5> res (z);
    res.set(_Nb1+_Nb2+_Nb3,       _Nb1 + _Nb2 + _Nb3 +_Nb4-1,     y);
    res.set(_Nb1+_Nb2,            _Nb1 + _Nb2 + _Nb3 -1,     x);
    res.set(_Nb1,                 _Nb1 + _Nb2 -1,  w);
    res.set(0,                    _Nb1 -1,         v);
    return res;
  }

  // 6 intbv's
  template <size_t _Nb1, size_t _Nb2, size_t _Nb3,size_t _Nb4,size_t _Nb5,size_t _Nb6>
  inline intbv<_Nb1+_Nb2+_Nb3+_Nb4+_Nb5+_Nb6> concat (const intbv<_Nb1>& u, const intbv<_Nb2>& v, const intbv<_Nb3>& w,
                                                      const intbv<_Nb4>& x,const intbv<_Nb5>& y, const intbv<_Nb6>& z)
  {

    intbv<_Nb1+_Nb2+_Nb3+_Nb4+_Nb5+_Nb6> res (z);
    res.set(_Nb1+_Nb2+_Nb3+_Nb4,       _Nb1 + _Nb2 + _Nb3 +_Nb4 +_Nb5-1,     y);
    res.set(_Nb1+_Nb2+_Nb3,       _Nb1 + _Nb2 + _Nb3 +_Nb4-1,     x);
    res.set(_Nb1+_Nb2,            _Nb1 + _Nb2 + _Nb3 -1,     w);
    res.set(_Nb1,                 _Nb1 + _Nb2 -1,  v);
    res.set(0,                    _Nb1 -1,         u);
    return res;
  }

  // Global arithmetic operations on intbvs.
  // param:  x  A intbv.
  // param:  y  A intbv of the same size as x.]
  // returns:  A new intbv.

  
  
  /////////////////////////
  //
  // Subtraction
  //  
  
  // Same size
  template<size_t _Nb>
  inline intbv<_Nb> operator-(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    // operates on same sized intbv's (requires one fewer temporary)
    intbv<_Nb> result(x);
    result -= y;
    return result;
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline intbv< MAX(_Nb1,_Nb2)> operator-(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    ArithArgCheck(_Nb1,_Nb2);

    intbv <MAX(_Nb1,_Nb2) > tmp_x (x);
    intbv <MAX(_Nb1,_Nb2) > tmp_y (y);
    tmp_x -= tmp_y;
    return tmp_x;
  }

  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator-(uint64_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(64,_Nb);
    return operator-(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator-(const intbv<_Nb>& x, uint64_t y)
  {
    ArithArgCheck(_Nb,64);
    return operator-(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  // int64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator-(int64_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(64,_Nb);
    return operator-(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator-(const intbv<_Nb>& x, int64_t y)
  {
    ArithArgCheck(_Nb,64);
    return operator-(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  }  
  // uint32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator-(uint32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return operator-(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator-(const intbv<_Nb>& x, uint32_t y)
  {
    ArithArgCheck(_Nb,32);
    return operator-(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }    
  // int32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator-(int32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return operator-(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator-(const intbv<_Nb>& x, int32_t y)
  {
    ArithArgCheck(_Nb,32);
    return operator-(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }
 

  //////////////////////
  //
  // Addition
  //
  
  // Same size
  template<size_t _Nb>
  inline intbv<_Nb> operator+(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    intbv<_Nb> result(x);
    result += y;
    return result;
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline intbv<MAX(_Nb1,_Nb2)> operator+(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    ArithArgCheck(_Nb1,_Nb2);
    intbv <MAX(_Nb1,_Nb2) > tmp_x (x);
    intbv <MAX(_Nb1,_Nb2) > tmp_y (y);
    tmp_x += tmp_y;
    return tmp_x;
  } 
  
  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator+(uint64_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(64,_Nb);
    return operator+(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator+(const intbv<_Nb>& x, uint64_t y)
  {
    ArithArgCheck(_Nb,64);
    return operator+(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  // int64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator+(int64_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(64,_Nb);
    return operator+(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator+(const intbv<_Nb>& x, int64_t y)
  {
    ArithArgCheck(_Nb,64);
    return operator+(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  }  
  // uint32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator+(uint32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return operator+(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator+(const intbv<_Nb>& x, uint32_t y)
  {
    ArithArgCheck(_Nb,32);
    return operator+(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }    
  // int32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator+(int32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return operator+(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator+(const intbv<_Nb>& x, int32_t y)
  {
    ArithArgCheck(_Nb,32);
    return operator+(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }
  
    
  ////////////////////////////////
  // 
  // Multiplication
  //
  
  
  // Same size
  template<size_t _Nb>
  inline intbv<_Nb> operator*(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    // operates on same sized intbv's (requires one fewer temporary)
    // both operands are treated as unsigned.
    intbv<_Nb> result(x);
    result *= y;
    return result;
  }

  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline intbv<MAX(_Nb1,_Nb2)> operator*(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    ArithArgCheck(_Nb1,_Nb2);
    // operates on different sized intbv's
    // both operands are treated as unsigned.
    intbv <MAX(_Nb1,_Nb2)> tmp_x (x);
    intbv <MAX(_Nb1,_Nb2)> tmp_y (y);
    tmp_x *= tmp_y;
    return tmp_x;
  }

  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator*(uint64_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(64,_Nb);
    return operator*(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator*(const intbv<_Nb>& x, uint64_t y)
  {
    ArithArgCheck(_Nb,64);
    return operator*(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator*(int64_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(64,_Nb);
    return operator*(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator*(const intbv<_Nb>& x, int64_t y)
  {
    ArithArgCheck(_Nb,64);
    return operator*(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  }  
  // uint32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator*(uint32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return operator*(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator*(const intbv<_Nb>& x, uint32_t y)
  {
    ArithArgCheck(_Nb,32);
    return operator*(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }    
  // int32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator*(int32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return operator*(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator*(const intbv<_Nb>& x, int32_t y)
  {
    ArithArgCheck(_Nb,32);
    return operator*(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }


  /////////////////////////
  //
  // Signed multiplication
  //


  // Same size
  template<size_t _Nb>
  inline intbv<_Nb> signedMultiply(const intbv<_Nb> &x,const intbv<_Nb> &y)
  {
    // both operands are treated as signed
    intbv<_WORD_ALIGNED(_Nb)> result = x.template signExtend<_WORD_ALIGNED(_Nb)>();
    intbv<_WORD_ALIGNED(_Nb)> tmp_y  = y.template signExtend<_WORD_ALIGNED(_Nb)>();
    result.signedMultiplyEqual(tmp_y);
    return result;
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline intbv<MAX(_Nb1,_Nb2)> signedMultiply(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    ArithArgCheck(_Nb1,_Nb2);
    // operates on different sized intbv's
    // both operands are treated as signed.
    intbv <_WORD_ALIGNED(MAX(_Nb1,_Nb2))> tmp_x = x.template signExtend<_WORD_ALIGNED(MAX(_Nb1,_Nb2))>();
    intbv <_WORD_ALIGNED(MAX(_Nb1,_Nb2))> tmp_y = y.template signExtend<_WORD_ALIGNED(MAX(_Nb1,_Nb2))>();  
    
    tmp_x.signedMultiplyEqual(tmp_y);

    return tmp_x;
  }
  
  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> signedMultiply(const intbv<_Nb> &x, uint64_t y)
  {
    ArithArgCheck(_Nb,64);
    return signedMultiply(x,intbv<64>(y));
  }

  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> signedMultiply(uint64_t x,const intbv<_Nb> &y)
  {
    ArithArgCheck(64,_Nb);
    return signedMultiply(intbv<64>(x),y);
  }
  
  // uint32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> signedMultiply(const intbv<_Nb>& x, uint32_t y)
  {
    ArithArgCheck(_Nb,32);
    return signedMultiply(x,intbv<32>(y));
  }

  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> signedMultiply(uint32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return signedMultiply(intbv<32>(x),y);
  }

  /////////////////////////////////
  //
  // Signed division
  //
    
  // Same size
  template<size_t _Nb>
  inline intbv<_Nb> signedDivide(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    // operates on same sized intbv's (requires one fewer temporary)
    // both operands are treated as signed    
    intbv<_WORD_ALIGNED(_Nb)> tmp_x = x.template signExtend<_WORD_ALIGNED(_Nb)>();
    intbv<_WORD_ALIGNED(_Nb)> tmp_y = y.template signExtend<_WORD_ALIGNED(_Nb)>(); 
    tmp_x.divideSignedAligned(tmp_y);
    return tmp_x;
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline intbv<MAX(_Nb1,_Nb2)> signedDivide(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    ArithArgCheck(_Nb1,_Nb2);
    intbv<_WORD_ALIGNED(MAX(_Nb1,_Nb2))> tmp_x = x.template signExtend<_WORD_ALIGNED(MAX(_Nb1,_Nb2))>();
    intbv<_WORD_ALIGNED(MAX(_Nb1,_Nb2))> tmp_y = y.template signExtend<_WORD_ALIGNED(MAX(_Nb1,_Nb2))>(); 
    tmp_x.divideSignedAligned(tmp_y);
    return tmp_x;
  }
  
  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> signedDivide(const intbv<_Nb>& x, uint64_t y)
  {
    ArithArgCheck(_Nb,64);
    return signedDivide(x,intbv<64>(y));
  }

  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> signedDivide(uint64_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(64,_Nb);
    return signedDivide(intbv<64>(x),y);
  }

  // uint32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> signedDivide(const intbv<_Nb>& x, uint32_t y)
  {
    ArithArgCheck(_Nb,32);
    return signedDivide(x,intbv<32>(y));
  }

  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> signedDivide(uint32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return signedDivide(intbv<32>(x),y);
  }  
  
  // int32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> signedDivide(const intbv<_Nb>& x, int32_t y)
  {
    ArithArgCheck(_Nb,32);
    return signedDivide(x.template signExtend<_WORD_ALIGNED(MAX(_Nb,32))>(),intbv<32>(y));
  }

  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> signedDivide(int32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return signedDivide(intbv<32>(x),y.template signExtend<_WORD_ALIGNED(MAX(_Nb,32))>());
  }  

  //////////////////////////
  //
  // Division
  //
  
  // Same size
  template<size_t _Nb>
  inline intbv<_Nb> operator/(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    // operates on same sized intbv's (requires one fewer temporary)
    intbv<_Nb> result(x);
    result /= y;
    return result;
  }

  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline intbv<MAX(_Nb1,_Nb2)> operator/(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    ArithArgCheck(_Nb1,_Nb2);
    // operates on different sized intbv's
    intbv <MAX(_Nb1,_Nb2) > tmp_x (x);
    intbv <MAX(_Nb1,_Nb2) > tmp_y (y);
    tmp_x /= tmp_y;
    return tmp_x;
  }
  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator/(uint64_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(64,_Nb);
    return operator/(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator/(const intbv<_Nb>& x, uint64_t y)
  {
    ArithArgCheck(_Nb,64);
    return operator/(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator/(int64_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(64,_Nb);
    return operator/(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator/(const intbv<_Nb>& x, int64_t y)
  {
    ArithArgCheck(_Nb,64);
    return operator/(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  }  
  // uint32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator/(uint32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return operator/(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator/(const intbv<_Nb>& x, uint32_t y)
  {
    ArithArgCheck(_Nb,32);
    return operator/(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }    
  // int32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator/(int32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return operator/(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator/(const intbv<_Nb>& x, int32_t y)
  {
    ArithArgCheck(_Nb,32);
    return operator/(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }

  ////////////////////////
  //
  // Modulus
  //
  
  // Same size
  template<size_t _Nb>
  inline intbv<_Nb> operator%(const intbv<_Nb> &x, const intbv<_Nb>& y)
  {
    // operates on same sized intbv's (requires one fewer temporary)
    intbv <_Nb> tmp_x (x);
    tmp_x %= y;
    return tmp_x; 
  }

  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline intbv<MAX(_Nb1,_Nb2)> operator%(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    ArithArgCheck(_Nb1,_Nb2);
    // operates on different sized intbv's
    intbv <MAX(_Nb1,_Nb2) > tmp_x (x);
    intbv <MAX(_Nb1,_Nb2) > tmp_y (y);
    tmp_x %= tmp_y;
    return tmp_x;
  }

  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator%(uint64_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(64,_Nb);
    return operator%(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator%(const intbv<_Nb>& x, uint64_t y)
  {
    ArithArgCheck(_Nb,64);
    return operator%(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator%(int64_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(64,_Nb);
    return operator%(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator%(const intbv<_Nb>& x, int64_t y)
  {
    ArithArgCheck(_Nb,64);
    return operator%(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  }  
  // uint32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator%(uint32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return operator%(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator%(const intbv<_Nb>& x, uint32_t y)
  {
    ArithArgCheck(_Nb,32);
    return operator%(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }    
  // int32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator%(int32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return operator%(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator%(const intbv<_Nb>& x, int32_t y)
  {
    ArithArgCheck(_Nb,32);
    return operator%(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }

  /////////////////////////////////
  //
  // Signed modulus
  //
    
  // Same size
  template<size_t _Nb>
  inline intbv<_Nb> signedMod(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    intbv<_Nb> tmp_x(x);
    intbv<_Nb> tmp_y(y);
    bool neg = false;

    if (tmp_x.get(0)) {
      tmp_x.negate();
      neg = true;
    }

    if (tmp_y.get(0)) {
      tmp_y.negate();
    }

    tmp_x = tmp_x % tmp_y;
    if (neg) {
      tmp_x.negate();
    }

    return tmp_x;
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline intbv<MAX(_Nb1,_Nb2)> signedMod(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    ArithArgCheck(_Nb1,_Nb2);
    intbv<_WORD_ALIGNED(MAX(_Nb1,_Nb2))> tmp_x = x.template signExtend<_WORD_ALIGNED(MAX(_Nb1,_Nb2))>();
    intbv<_WORD_ALIGNED(MAX(_Nb1,_Nb2))> tmp_y = y.template signExtend<_WORD_ALIGNED(MAX(_Nb1,_Nb2))>(); 

    return signedMod(tmp_x,tmp_y);
  }
  
  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> signedMod(const intbv<_Nb>& x, uint64_t y)
  {
    ArithArgCheck(_Nb,64);
    return signedMod(x,intbv<64>(y));
  }

  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> signedMod(uint64_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(64,_Nb);
    return signedMod(intbv<64>(x),y);
  }

  // uint32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> signedMod(const intbv<_Nb>& x, uint32_t y)
  {
    ArithArgCheck(_Nb,32);
    return signedMod(x,intbv<32>(y));
  }

  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> signedMod(uint32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return signedMod(intbv<32>(x),y);
  }  
  
  // int32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> signedMod(const intbv<_Nb>& x, int32_t y)
  {
    ArithArgCheck(_Nb,32);
    return signedMod(x.template signExtend<_WORD_ALIGNED(MAX(_Nb,32))>(),intbv<32>(y));
  }

  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> signedMod(int32_t x,const intbv<_Nb>& y)
  {
    ArithArgCheck(32,_Nb);
    return signedMod(intbv<32>(x),y.template signExtend<_WORD_ALIGNED(MAX(_Nb,32))>());
  }  

  
  ///////////////////////////
  //
  // Comparisons
  //
  
  template <size_t _Nb> template <size_t _Nb2>
  inline bool intbv<_Nb>::operator==(const intbv<_Nb2>& rhs) const
  { 
    CmpArgCheck(_Nb,_Nb2);
    intbv <MAX(_Nb,_Nb2)> tmp_x (*this);
    intbv <MAX(_Nb,_Nb2)> tmp_y (rhs);
    return tmp_x == tmp_y;
  }


  template <size_t _Nb> template <size_t _Nb2>
  inline bool intbv<_Nb>::operator!=(const intbv<_Nb2>& rhs) const
  { 
    CmpArgCheck(_Nb,_Nb2);
    intbv <MAX(_Nb,_Nb2)> tmp_x (*this);
    intbv <MAX(_Nb,_Nb2)> tmp_y (rhs);
    return tmp_x != tmp_y;
  }

  template <size_t _Nb> template <size_t _Nb2>
  inline bool intbv<_Nb>::operator<=(const intbv<_Nb2>& rhs) const
  { 
    CmpArgCheck(_Nb,_Nb2);
    intbv <MAX(_Nb,_Nb2)> tmp_x (*this);
    intbv <MAX(_Nb,_Nb2)> tmp_y (rhs);
    return tmp_x <= tmp_y;
  }

  template <size_t _Nb> template <size_t _Nb2>
  inline bool intbv<_Nb>::operator>=(const intbv<_Nb2>& rhs) const
  { 
    CmpArgCheck(_Nb,_Nb2);
    intbv <MAX(_Nb,_Nb2)> tmp_x (*this);
    intbv <MAX(_Nb,_Nb2)> tmp_y (rhs);
    return tmp_x >= tmp_y;
  }

  template <size_t _Nb> template <size_t _Nb2>
  inline bool intbv<_Nb>::operator<(const intbv<_Nb2>& rhs) const
  { 
    CmpArgCheck(_Nb,_Nb2);
    bool rc = false;
    intbv <MAX(_Nb,_Nb2)> tmp_x (*this);
    intbv <MAX(_Nb,_Nb2)> tmp_y (rhs);
    rc =  tmp_x < tmp_y;
    return rc;
  }

  template <size_t _Nb> template <size_t _Nb2>
  inline bool intbv<_Nb>::operator>(const intbv<_Nb2>& rhs) const
  { 
    CmpArgCheck(_Nb,_Nb2);
    intbv <MAX(_Nb,_Nb2)> tmp_x (*this);
    intbv <MAX(_Nb,_Nb2)> tmp_y (rhs);
    return tmp_x > tmp_y;
  }



  // Global bitwise operations on intbvs.
  // param:  x  A intbv.
  // param:  y  A intbv of the same size as x.    
  // returns:  A new intbv.
  
  
  //////////////////
  // 
  // And
  // 
  
  // Same size
  template<size_t _Nb>
  inline intbv<_Nb> operator&(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    // operates on same sized intbv's (requires one fewer temporary)
    intbv<_Nb> result(x);
    result &= y;
    return result;
  }

  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline intbv<MAX(_Nb1,_Nb2)> operator&(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    ArithArgCheck(_Nb1,_Nb2);
    // operates on different sized intbv's
    intbv <MAX(_Nb1,_Nb2) > tmp_x (x);
    intbv <MAX(_Nb1,_Nb2) > tmp_y (y);
    tmp_x &= tmp_y;
    return tmp_x;
  }  
  
  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator&(const intbv<_Nb>& x, uint64_t y) {
    ArithArgCheck(_Nb,64);
    return operator&(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator&(uint64_t x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator&(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  // int64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator&(const intbv<_Nb>& x, int64_t y) {
    ArithArgCheck(_Nb,64);
    return operator&(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator&(int64_t x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator&(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  // uint32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator&(const intbv<_Nb>& x, uint32_t y) {
    ArithArgCheck(_Nb,32);
    return operator&(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator&(uint32_t x, const intbv<_Nb>& y) {
    ArithArgCheck(32,_Nb);
    return operator&(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  // int32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator&(const intbv<_Nb>& x, int32_t y) {
    ArithArgCheck(_Nb,32);
    return operator&(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator&(int32_t x, const intbv<_Nb>& y) {
    ArithArgCheck(32,_Nb);
    return operator&(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }

  
  //////////////////
  //
  //  OR
  //

  // Same size
  template<size_t _Nb>
  inline intbv<_Nb> operator|(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    // operates on same sized intbv's (requires one fewer temporary)
    intbv<_Nb> result(x);
    result |= y;
    return result;
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline intbv<MAX(_Nb1,_Nb2)> operator|(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    ArithArgCheck(_Nb1,_Nb2);
    // operates on different sized intbv's
    intbv <MAX(_Nb1,_Nb2) > tmp_x (x);
    intbv <MAX(_Nb1,_Nb2) > tmp_y (y);
    tmp_x |= tmp_y;
    return tmp_x;
  }
  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator|(const intbv<_Nb>& x, uint64_t y) {
    ArithArgCheck(_Nb,64);
    return operator|(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator|(uint64_t x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator|(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  // int64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator|(const intbv<_Nb>& x, int64_t y) {
    ArithArgCheck(_Nb,64);
    return operator|(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator|(int64_t x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator|(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  // uint32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator|(const intbv<_Nb>& x, uint32_t y) {
    ArithArgCheck(_Nb,32);
    return operator|(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator|(uint32_t x, const intbv<_Nb>& y) {
    ArithArgCheck(32,_Nb);
    return operator|(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  // int32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator|(const intbv<_Nb>& x, int32_t y) {
    ArithArgCheck(_Nb,32);
    return operator|(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator|(int32_t x, const intbv<_Nb>& y) {
    ArithArgCheck(32,_Nb);
    return operator|(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  
  ////////////////
  //
  //  XOR
  //

  // Same size
  template <size_t _Nb>
  inline intbv<_Nb> operator^(const intbv<_Nb>& x, const intbv<_Nb>& y)
  {
    // operates on same sized intbv's (requires one fewer temporary)
    intbv<_Nb> result(x);
    result ^= y;
    return result;
  }
  
  // Different size
  template<size_t _Nb1, size_t _Nb2>
  inline intbv<MAX(_Nb1,_Nb2)> operator^(const intbv<_Nb1>& x, const intbv<_Nb2>& y)
  {
    ArithArgCheck(_Nb1,_Nb2);
    // operates on different sized intbv's
    intbv <MAX(_Nb1,_Nb2) > tmp_x (x);
    intbv <MAX(_Nb1,_Nb2) > tmp_y (y);
    tmp_x ^= tmp_y;
    return tmp_x;
  }
  // uint64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator^(const intbv<_Nb>& x, uint64_t y) {
    ArithArgCheck(_Nb,64);
    return operator^(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator^(uint64_t x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator^(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  // int64_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator^(const intbv<_Nb>& x, int64_t y) {
    ArithArgCheck(_Nb,64);
    return operator^(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,64)> operator^(int64_t x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator^(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  }
  // uint32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator^(const intbv<_Nb>& x, uint32_t y) {
    ArithArgCheck(_Nb,32);
    return operator^(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator^(uint32_t x, const intbv<_Nb>& y) {
    ArithArgCheck(32,_Nb);
    return operator^(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
  // int32_t
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator^(const intbv<_Nb>& x, int32_t y) {
    ArithArgCheck(_Nb,32);
    return operator^(x,static_cast<intbv<MAX(_Nb,32)> >(y));
  }
  template<size_t _Nb>
  inline intbv<MAX(_Nb,32)> operator^(int32_t x, const intbv<_Nb>& y) {
    ArithArgCheck(32,_Nb);
    return operator^(static_cast<intbv<MAX(_Nb,32)> >(x),y);
  }
 
  /////////////
  //
  //  Not
  //
  template <size_t _Nb>
  inline bool operator!(intbv<_Nb> x)
  {
    // logical not
    return (x == 0);
  }


  // Stream reading.
  template <size_t _Nb>
  std::istream& operator>>( std::istream& is, intbv<_Nb> &x) {
    x.readNumber(is); return is; }
  // Stream writing.
  template <size_t _Nb>
  std::ostream& operator<<( std::ostream& os, const intbv<_Nb> &x) {
    x.writeNumber(os); return os; }

#ifdef IS_64BIT

  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator+(const intbv<_Nb>& x,unsigned long long int y) {
    ArithArgCheck(_Nb,64);
    return operator+(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator+(unsigned long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator+(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 

  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator+(const intbv<_Nb>& x,long long int y) {
    ArithArgCheck(_Nb,64);
    return operator+(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator+(long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator+(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 


  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator-(const intbv<_Nb>& x,unsigned long long int y) {
    ArithArgCheck(_Nb,64);
    return operator-(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator-(unsigned long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator-(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 

  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator-(const intbv<_Nb>& x,long long int y) {
    ArithArgCheck(_Nb,64);
    return operator-(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator-(long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator-(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 


  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator%(const intbv<_Nb>& x,unsigned long long int y) {
    ArithArgCheck(_Nb,64);
    return operator%(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator%(unsigned long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator%(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 

  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator%(const intbv<_Nb>& x,long long int y) {
    ArithArgCheck(_Nb,64);
    return operator%(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator%(long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator%(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 


  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator*(const intbv<_Nb>& x,unsigned long long int y) {
    ArithArgCheck(_Nb,64);
    return operator*(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator*(unsigned long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator*(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 

  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator*(const intbv<_Nb>& x,long long int y) {
    ArithArgCheck(_Nb,64);
    return operator*(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator*(long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator*(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 


  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator/(const intbv<_Nb>& x,unsigned long long int y) {
    ArithArgCheck(_Nb,64);
    return operator/(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator/(unsigned long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator/(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 

  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator/(const intbv<_Nb>& x,long long int y) {
    ArithArgCheck(_Nb,64);
    return operator/(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator/(long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator/(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 

  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator&(const intbv<_Nb>& x,unsigned long long int y) {
    ArithArgCheck(_Nb,64);
    return operator&(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator&(unsigned long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator&(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 

  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator&(const intbv<_Nb>& x,long long int y) {
    ArithArgCheck(_Nb,64);
    return operator&(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator&(long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator&(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 

  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator|(const intbv<_Nb>& x,unsigned long long int y) {
    ArithArgCheck(_Nb,64);
    return operator|(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator|(unsigned long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator|(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 

  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator|(const intbv<_Nb>& x,long long int y) {
    ArithArgCheck(_Nb,64);
    return operator|(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator|(long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator|(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator^(const intbv<_Nb>& x,unsigned long long int y) {
    ArithArgCheck(_Nb,64);
    return operator^(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator^(unsigned long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator^(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 

  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator^(const intbv<_Nb>& x,long long int y) {
    ArithArgCheck(_Nb,64);
    return operator^(x,static_cast<intbv<MAX(_Nb,64)> >(y));
  } 
  template <size_t _Nb>
  inline  intbv<MAX(_Nb,64)> operator^(long long int x, const intbv<_Nb>& y) {
    ArithArgCheck(64,_Nb);
    return operator^(static_cast<intbv<MAX(_Nb,64)> >(x),y);
  } 


#endif

  //#undef _INTBV_WORDS
  //#undef _INTBV_BITS_PER_WORD

}

#endif /* _INTBV */
