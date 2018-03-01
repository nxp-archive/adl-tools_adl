//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// Simple type-conversion template functions for easy conversion between builtin
// integer types and bit-vector types.
//

namespace adl {

  // Conversion functions for modifying a number thing into a uint32.
  template <size_t Nb>
  inline uint32_t to_uint32(const ns_bits<Nb> &x) {
    return x.uint32();
  }

  template <size_t Nb>
  inline uint32_t to_uint32(const ns_sbits<Nb> &x) {
    return x.uint32();
  }  

  inline uint32_t to_uint32(uint32_t x) {
    return x;
  }

  // Conversion functions for modifying a number thing into a uint64.
  template <size_t Nb>
  inline uint64_t to_uint64(const ns_bits<Nb> &x) {
    return x.uint64();
  }

  template <size_t Nb>
  inline uint64_t to_uint64(const ns_sbits<Nb> &x) {
    return x.uint64();
  }

  inline uint64_t to_uint64(uint64_t x) {
    return x;
  }

  // Conversions for converting a number thing into a bits object.
  template <template<size_t> class number_type,size_t Nb>
  inline number_type<Nb> to_bits(const number_type<Nb> &x) {
    return x;
  }

  template <template<size_t> class number_type>  
  inline number_type<32> to_bits(uint32_t x) {
    return number_type<32>(x);
  }

  template <template<size_t> class number_type>  
  inline number_type<64> to_bits(uint64_t x) {
    return number_type<64>(x);
  }

  template <template<size_t> class number_type>  
  inline number_type<sizeof(int)*8> to_bits(int x) {
    return number_type<sizeof(int)*8>(x);
  }

}
