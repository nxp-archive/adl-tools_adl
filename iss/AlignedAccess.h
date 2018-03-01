//
// Non-inlined memory accesses for aligned accesses.  Used by the
// multi-threading DMI cache in order to make sure that the accesses actually
// occur, rather than being optimized out.
//

#ifndef _ALIGNED_ACCESS_H_
#define _ALIGNED_ACCESS_H_

#ifdef INLINE_ALIGNED_ACCESSES

# define OptionalInline inline 
# define OptionalBody(x) x

#else

# ifdef IMPL_ALIGNED_ACCESSES
#  undef  OptionalInline 
#  undef  OptionalBody
#  define OptionalInline 
#  define OptionalBody(x) x
# else
#  undef  OptionalInline 
#  undef  OptionalBody
#  define OptionalInline 
#  define OptionalBody(x)
# endif

#endif

OptionalInline uint64_t aligned_byte_read64(byte_t *p,unsigned offset)
  OptionalBody({
      uint64_t *ptr = (uint64_t*)&p[offset];
      return be64(*ptr);
    });

OptionalInline void aligned_byte_write64(byte_t *p,unsigned offset,uint64_t orig_value)
  OptionalBody({
      uint64_t *ptr = (uint64_t*)&p[offset];
      *ptr = be64(orig_value);
    });

OptionalInline uint32_t aligned_byte_read32(byte_t *p,unsigned offset)
  OptionalBody({
      uint32_t *ptr = (uint32_t*)&p[offset];
      return be32(*ptr);
    });

OptionalInline void aligned_byte_write32(byte_t *p,unsigned offset,uint32_t orig_value)
  OptionalBody({
      uint32_t *ptr = (uint32_t*)&p[offset];
      *ptr = be32(orig_value);
    });

OptionalInline uint16_t aligned_byte_read16(byte_t *p,unsigned offset)
  OptionalBody({
      uint16_t *ptr = (uint16_t*)&p[offset];
      return be16(*ptr);
    });

OptionalInline void aligned_byte_write16(byte_t *p,unsigned offset,uint16_t orig_value)
  OptionalBody({
      uint16_t *ptr = (uint16_t*)&p[offset];
      *ptr = be16(orig_value);
    });

OptionalInline uint8_t aligned_byte_read8(byte_t *p,unsigned offset)
  OptionalBody({
      uint8_t *ptr = (uint8_t*)&p[offset];
      return *ptr;
    });

OptionalInline void aligned_byte_write8(byte_t *p,unsigned offset,uint8_t orig_value)
  OptionalBody({
      uint8_t *ptr = (uint8_t*)&p[offset];
      *ptr = orig_value;
    });

// Data invariant TLM2 versions of the above.

OptionalInline uint64_t tlm2_aligned_byte_read64(byte_t *p,unsigned offset)
  OptionalBody({
      uint64_t *ptr = (uint64_t*)((unsigned long)p+((offset ^ DWMask))) - DWMask;
      return *ptr;
    });

OptionalInline void tlm2_aligned_byte_write64(byte_t *p,unsigned offset,uint64_t orig_value)
  OptionalBody({
      uint64_t *ptr = (uint64_t*)((unsigned long)p+((offset ^ DWMask))) - DWMask;
      *ptr = orig_value;
    });

OptionalInline uint32_t tlm2_aligned_byte_read32(byte_t *p,unsigned offset)
  OptionalBody({
      uint32_t *ptr = (uint32_t*) ((unsigned long)p+((offset ^ DWMask))) - WordMask;
      return *ptr;
    });

OptionalInline void tlm2_aligned_byte_write32(byte_t *p,unsigned offset,uint32_t orig_value)
  OptionalBody({
      uint32_t *ptr = (uint32_t*) ((unsigned long)p+((offset ^ DWMask))) - WordMask;
      *ptr = orig_value;
    });

OptionalInline uint16_t tlm2_aligned_byte_read16(byte_t *p,unsigned offset)
  OptionalBody({
      uint16_t *ptr = (uint16_t*) ((unsigned long)p+((offset ^ DWMask))) - HWMask;
      return *ptr;
    });

OptionalInline void tlm2_aligned_byte_write16(byte_t *p,unsigned offset,uint16_t orig_value)
  OptionalBody({
      uint16_t *ptr = (uint16_t*) ((unsigned long)p+((offset ^ DWMask))) - HWMask;
      *ptr = orig_value;
    });

OptionalInline uint8_t tlm2_aligned_byte_read8(byte_t *p,unsigned offset)
  OptionalBody({
      uint8_t *ptr = (uint8_t*)&p[offset ^ DWMask];
      return *ptr;
    });

OptionalInline void tlm2_aligned_byte_write8(byte_t *p,unsigned offset,uint8_t orig_value)
  OptionalBody({
      uint8_t *ptr = (uint8_t*)&p[offset ^ DWMask];
      *ptr = orig_value;
    });

#endif
