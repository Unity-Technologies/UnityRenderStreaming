/*===---- arm_fp16.h - ARM FP16 intrinsics ---------------------------------===
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *===-----------------------------------------------------------------------===
 */

#ifndef __ARM_FP16_H
#define __ARM_FP16_H

#include <stdint.h>

typedef __fp16 float16_t;
#define __ai static __inline__ __attribute__((__always_inline__, __nodebug__))

#if defined(__ARM_FEATURE_FP16_SCALAR_ARITHMETIC) && defined(__aarch64__)
#ifdef __LITTLE_ENDIAN__
#define vabdh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vabdh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vabdh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vabdh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vabsh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vabsh_f16(__s0); \
  __ret; \
})
#else
#define vabsh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vabsh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vaddh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vaddh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vaddh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vaddh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcageh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcageh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vcageh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcageh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcagth_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcagth_f16(__s0, __s1); \
  __ret; \
})
#else
#define vcagth_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcagth_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcaleh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcaleh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vcaleh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcaleh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcalth_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcalth_f16(__s0, __s1); \
  __ret; \
})
#else
#define vcalth_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcalth_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vceqh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vceqh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vceqh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vceqh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vceqzh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vceqzh_f16(__s0); \
  __ret; \
})
#else
#define vceqzh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vceqzh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcgeh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcgeh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vcgeh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcgeh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcgezh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcgezh_f16(__s0); \
  __ret; \
})
#else
#define vcgezh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcgezh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcgth_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcgth_f16(__s0, __s1); \
  __ret; \
})
#else
#define vcgth_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcgth_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcgtzh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcgtzh_f16(__s0); \
  __ret; \
})
#else
#define vcgtzh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcgtzh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcleh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcleh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vcleh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcleh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vclezh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vclezh_f16(__s0); \
  __ret; \
})
#else
#define vclezh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vclezh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vclth_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vclth_f16(__s0, __s1); \
  __ret; \
})
#else
#define vclth_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vclth_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcltzh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcltzh_f16(__s0); \
  __ret; \
})
#else
#define vcltzh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcltzh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_n_s16_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  int16_t __ret; \
  __ret = (int16_t) __builtin_neon_vcvth_n_s16_f16(__s0, __p1); \
  __ret; \
})
#else
#define vcvth_n_s16_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  int16_t __ret; \
  __ret = (int16_t) __builtin_neon_vcvth_n_s16_f16(__s0, __p1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_n_s32_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  int32_t __ret; \
  __ret = (int32_t) __builtin_neon_vcvth_n_s32_f16(__s0, __p1); \
  __ret; \
})
#else
#define vcvth_n_s32_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  int32_t __ret; \
  __ret = (int32_t) __builtin_neon_vcvth_n_s32_f16(__s0, __p1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_n_s64_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  int64_t __ret; \
  __ret = (int64_t) __builtin_neon_vcvth_n_s64_f16(__s0, __p1); \
  __ret; \
})
#else
#define vcvth_n_s64_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  int64_t __ret; \
  __ret = (int64_t) __builtin_neon_vcvth_n_s64_f16(__s0, __p1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_n_u16_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcvth_n_u16_f16(__s0, __p1); \
  __ret; \
})
#else
#define vcvth_n_u16_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcvth_n_u16_f16(__s0, __p1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_n_u32_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint32_t __ret; \
  __ret = (uint32_t) __builtin_neon_vcvth_n_u32_f16(__s0, __p1); \
  __ret; \
})
#else
#define vcvth_n_u32_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint32_t __ret; \
  __ret = (uint32_t) __builtin_neon_vcvth_n_u32_f16(__s0, __p1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_n_u64_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint64_t __ret; \
  __ret = (uint64_t) __builtin_neon_vcvth_n_u64_f16(__s0, __p1); \
  __ret; \
})
#else
#define vcvth_n_u64_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint64_t __ret; \
  __ret = (uint64_t) __builtin_neon_vcvth_n_u64_f16(__s0, __p1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_s16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int16_t __ret; \
  __ret = (int16_t) __builtin_neon_vcvth_s16_f16(__s0); \
  __ret; \
})
#else
#define vcvth_s16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int16_t __ret; \
  __ret = (int16_t) __builtin_neon_vcvth_s16_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_s32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int32_t __ret; \
  __ret = (int32_t) __builtin_neon_vcvth_s32_f16(__s0); \
  __ret; \
})
#else
#define vcvth_s32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int32_t __ret; \
  __ret = (int32_t) __builtin_neon_vcvth_s32_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_s64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int64_t __ret; \
  __ret = (int64_t) __builtin_neon_vcvth_s64_f16(__s0); \
  __ret; \
})
#else
#define vcvth_s64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int64_t __ret; \
  __ret = (int64_t) __builtin_neon_vcvth_s64_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_u16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcvth_u16_f16(__s0); \
  __ret; \
})
#else
#define vcvth_u16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcvth_u16_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_u32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint32_t __ret; \
  __ret = (uint32_t) __builtin_neon_vcvth_u32_f16(__s0); \
  __ret; \
})
#else
#define vcvth_u32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint32_t __ret; \
  __ret = (uint32_t) __builtin_neon_vcvth_u32_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_u64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint64_t __ret; \
  __ret = (uint64_t) __builtin_neon_vcvth_u64_f16(__s0); \
  __ret; \
})
#else
#define vcvth_u64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint64_t __ret; \
  __ret = (uint64_t) __builtin_neon_vcvth_u64_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtah_s16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int16_t __ret; \
  __ret = (int16_t) __builtin_neon_vcvtah_s16_f16(__s0); \
  __ret; \
})
#else
#define vcvtah_s16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int16_t __ret; \
  __ret = (int16_t) __builtin_neon_vcvtah_s16_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtah_s32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int32_t __ret; \
  __ret = (int32_t) __builtin_neon_vcvtah_s32_f16(__s0); \
  __ret; \
})
#else
#define vcvtah_s32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int32_t __ret; \
  __ret = (int32_t) __builtin_neon_vcvtah_s32_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtah_s64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int64_t __ret; \
  __ret = (int64_t) __builtin_neon_vcvtah_s64_f16(__s0); \
  __ret; \
})
#else
#define vcvtah_s64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int64_t __ret; \
  __ret = (int64_t) __builtin_neon_vcvtah_s64_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtah_u16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcvtah_u16_f16(__s0); \
  __ret; \
})
#else
#define vcvtah_u16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcvtah_u16_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtah_u32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint32_t __ret; \
  __ret = (uint32_t) __builtin_neon_vcvtah_u32_f16(__s0); \
  __ret; \
})
#else
#define vcvtah_u32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint32_t __ret; \
  __ret = (uint32_t) __builtin_neon_vcvtah_u32_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtah_u64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint64_t __ret; \
  __ret = (uint64_t) __builtin_neon_vcvtah_u64_f16(__s0); \
  __ret; \
})
#else
#define vcvtah_u64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint64_t __ret; \
  __ret = (uint64_t) __builtin_neon_vcvtah_u64_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
__ai float16_t vcvth_f16_u32(uint32_t __p0) {
  float16_t __ret;
  __ret = (float16_t) __builtin_neon_vcvth_f16_u32(__p0);
  return __ret;
}
#else
__ai float16_t vcvth_f16_u32(uint32_t __p0) {
  float16_t __ret;
  __ret = (float16_t) __builtin_neon_vcvth_f16_u32(__p0);
  return __ret;
}
#endif

#ifdef __LITTLE_ENDIAN__
__ai float16_t vcvth_f16_u64(uint64_t __p0) {
  float16_t __ret;
  __ret = (float16_t) __builtin_neon_vcvth_f16_u64(__p0);
  return __ret;
}
#else
__ai float16_t vcvth_f16_u64(uint64_t __p0) {
  float16_t __ret;
  __ret = (float16_t) __builtin_neon_vcvth_f16_u64(__p0);
  return __ret;
}
#endif

#ifdef __LITTLE_ENDIAN__
__ai float16_t vcvth_f16_u16(uint16_t __p0) {
  float16_t __ret;
  __ret = (float16_t) __builtin_neon_vcvth_f16_u16(__p0);
  return __ret;
}
#else
__ai float16_t vcvth_f16_u16(uint16_t __p0) {
  float16_t __ret;
  __ret = (float16_t) __builtin_neon_vcvth_f16_u16(__p0);
  return __ret;
}
#endif

#ifdef __LITTLE_ENDIAN__
__ai float16_t vcvth_f16_s32(int32_t __p0) {
  float16_t __ret;
  __ret = (float16_t) __builtin_neon_vcvth_f16_s32(__p0);
  return __ret;
}
#else
__ai float16_t vcvth_f16_s32(int32_t __p0) {
  float16_t __ret;
  __ret = (float16_t) __builtin_neon_vcvth_f16_s32(__p0);
  return __ret;
}
#endif

#ifdef __LITTLE_ENDIAN__
__ai float16_t vcvth_f16_s64(int64_t __p0) {
  float16_t __ret;
  __ret = (float16_t) __builtin_neon_vcvth_f16_s64(__p0);
  return __ret;
}
#else
__ai float16_t vcvth_f16_s64(int64_t __p0) {
  float16_t __ret;
  __ret = (float16_t) __builtin_neon_vcvth_f16_s64(__p0);
  return __ret;
}
#endif

#ifdef __LITTLE_ENDIAN__
__ai float16_t vcvth_f16_s16(int16_t __p0) {
  float16_t __ret;
  __ret = (float16_t) __builtin_neon_vcvth_f16_s16(__p0);
  return __ret;
}
#else
__ai float16_t vcvth_f16_s16(int16_t __p0) {
  float16_t __ret;
  __ret = (float16_t) __builtin_neon_vcvth_f16_s16(__p0);
  return __ret;
}
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_n_f16_u32(__p0, __p1) __extension__ ({ \
  uint32_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vcvth_n_f16_u32(__s0, __p1); \
  __ret; \
})
#else
#define vcvth_n_f16_u32(__p0, __p1) __extension__ ({ \
  uint32_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vcvth_n_f16_u32(__s0, __p1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_n_f16_u64(__p0, __p1) __extension__ ({ \
  uint64_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vcvth_n_f16_u64(__s0, __p1); \
  __ret; \
})
#else
#define vcvth_n_f16_u64(__p0, __p1) __extension__ ({ \
  uint64_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vcvth_n_f16_u64(__s0, __p1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_n_f16_u16(__p0, __p1) __extension__ ({ \
  uint16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vcvth_n_f16_u16(__s0, __p1); \
  __ret; \
})
#else
#define vcvth_n_f16_u16(__p0, __p1) __extension__ ({ \
  uint16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vcvth_n_f16_u16(__s0, __p1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_n_f16_s32(__p0, __p1) __extension__ ({ \
  int32_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vcvth_n_f16_s32(__s0, __p1); \
  __ret; \
})
#else
#define vcvth_n_f16_s32(__p0, __p1) __extension__ ({ \
  int32_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vcvth_n_f16_s32(__s0, __p1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_n_f16_s64(__p0, __p1) __extension__ ({ \
  int64_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vcvth_n_f16_s64(__s0, __p1); \
  __ret; \
})
#else
#define vcvth_n_f16_s64(__p0, __p1) __extension__ ({ \
  int64_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vcvth_n_f16_s64(__s0, __p1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvth_n_f16_s16(__p0, __p1) __extension__ ({ \
  int16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vcvth_n_f16_s16(__s0, __p1); \
  __ret; \
})
#else
#define vcvth_n_f16_s16(__p0, __p1) __extension__ ({ \
  int16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vcvth_n_f16_s16(__s0, __p1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtmh_s16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int16_t __ret; \
  __ret = (int16_t) __builtin_neon_vcvtmh_s16_f16(__s0); \
  __ret; \
})
#else
#define vcvtmh_s16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int16_t __ret; \
  __ret = (int16_t) __builtin_neon_vcvtmh_s16_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtmh_s32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int32_t __ret; \
  __ret = (int32_t) __builtin_neon_vcvtmh_s32_f16(__s0); \
  __ret; \
})
#else
#define vcvtmh_s32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int32_t __ret; \
  __ret = (int32_t) __builtin_neon_vcvtmh_s32_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtmh_s64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int64_t __ret; \
  __ret = (int64_t) __builtin_neon_vcvtmh_s64_f16(__s0); \
  __ret; \
})
#else
#define vcvtmh_s64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int64_t __ret; \
  __ret = (int64_t) __builtin_neon_vcvtmh_s64_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtmh_u16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcvtmh_u16_f16(__s0); \
  __ret; \
})
#else
#define vcvtmh_u16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcvtmh_u16_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtmh_u32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint32_t __ret; \
  __ret = (uint32_t) __builtin_neon_vcvtmh_u32_f16(__s0); \
  __ret; \
})
#else
#define vcvtmh_u32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint32_t __ret; \
  __ret = (uint32_t) __builtin_neon_vcvtmh_u32_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtmh_u64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint64_t __ret; \
  __ret = (uint64_t) __builtin_neon_vcvtmh_u64_f16(__s0); \
  __ret; \
})
#else
#define vcvtmh_u64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint64_t __ret; \
  __ret = (uint64_t) __builtin_neon_vcvtmh_u64_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtnh_s16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int16_t __ret; \
  __ret = (int16_t) __builtin_neon_vcvtnh_s16_f16(__s0); \
  __ret; \
})
#else
#define vcvtnh_s16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int16_t __ret; \
  __ret = (int16_t) __builtin_neon_vcvtnh_s16_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtnh_s32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int32_t __ret; \
  __ret = (int32_t) __builtin_neon_vcvtnh_s32_f16(__s0); \
  __ret; \
})
#else
#define vcvtnh_s32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int32_t __ret; \
  __ret = (int32_t) __builtin_neon_vcvtnh_s32_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtnh_s64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int64_t __ret; \
  __ret = (int64_t) __builtin_neon_vcvtnh_s64_f16(__s0); \
  __ret; \
})
#else
#define vcvtnh_s64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int64_t __ret; \
  __ret = (int64_t) __builtin_neon_vcvtnh_s64_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtnh_u16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcvtnh_u16_f16(__s0); \
  __ret; \
})
#else
#define vcvtnh_u16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcvtnh_u16_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtnh_u32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint32_t __ret; \
  __ret = (uint32_t) __builtin_neon_vcvtnh_u32_f16(__s0); \
  __ret; \
})
#else
#define vcvtnh_u32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint32_t __ret; \
  __ret = (uint32_t) __builtin_neon_vcvtnh_u32_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtnh_u64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint64_t __ret; \
  __ret = (uint64_t) __builtin_neon_vcvtnh_u64_f16(__s0); \
  __ret; \
})
#else
#define vcvtnh_u64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint64_t __ret; \
  __ret = (uint64_t) __builtin_neon_vcvtnh_u64_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtph_s16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int16_t __ret; \
  __ret = (int16_t) __builtin_neon_vcvtph_s16_f16(__s0); \
  __ret; \
})
#else
#define vcvtph_s16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int16_t __ret; \
  __ret = (int16_t) __builtin_neon_vcvtph_s16_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtph_s32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int32_t __ret; \
  __ret = (int32_t) __builtin_neon_vcvtph_s32_f16(__s0); \
  __ret; \
})
#else
#define vcvtph_s32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int32_t __ret; \
  __ret = (int32_t) __builtin_neon_vcvtph_s32_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtph_s64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int64_t __ret; \
  __ret = (int64_t) __builtin_neon_vcvtph_s64_f16(__s0); \
  __ret; \
})
#else
#define vcvtph_s64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  int64_t __ret; \
  __ret = (int64_t) __builtin_neon_vcvtph_s64_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtph_u16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcvtph_u16_f16(__s0); \
  __ret; \
})
#else
#define vcvtph_u16_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint16_t __ret; \
  __ret = (uint16_t) __builtin_neon_vcvtph_u16_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtph_u32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint32_t __ret; \
  __ret = (uint32_t) __builtin_neon_vcvtph_u32_f16(__s0); \
  __ret; \
})
#else
#define vcvtph_u32_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint32_t __ret; \
  __ret = (uint32_t) __builtin_neon_vcvtph_u32_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vcvtph_u64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint64_t __ret; \
  __ret = (uint64_t) __builtin_neon_vcvtph_u64_f16(__s0); \
  __ret; \
})
#else
#define vcvtph_u64_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  uint64_t __ret; \
  __ret = (uint64_t) __builtin_neon_vcvtph_u64_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vdivh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vdivh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vdivh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vdivh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vfmah_f16(__p0, __p1, __p2) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __s2 = __p2; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vfmah_f16(__s0, __s1, __s2); \
  __ret; \
})
#else
#define vfmah_f16(__p0, __p1, __p2) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __s2 = __p2; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vfmah_f16(__s0, __s1, __s2); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vfmsh_f16(__p0, __p1, __p2) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __s2 = __p2; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vfmsh_f16(__s0, __s1, __s2); \
  __ret; \
})
#else
#define vfmsh_f16(__p0, __p1, __p2) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __s2 = __p2; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vfmsh_f16(__s0, __s1, __s2); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vmaxh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vmaxh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vmaxh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vmaxh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vmaxnmh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vmaxnmh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vmaxnmh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vmaxnmh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vminh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vminh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vminh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vminh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vminnmh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vminnmh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vminnmh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vminnmh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vmulh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vmulh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vmulh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vmulh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vmulxh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vmulxh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vmulxh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vmulxh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vnegh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vnegh_f16(__s0); \
  __ret; \
})
#else
#define vnegh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vnegh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vrecpeh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrecpeh_f16(__s0); \
  __ret; \
})
#else
#define vrecpeh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrecpeh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vrecpsh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrecpsh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vrecpsh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrecpsh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vrecpxh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrecpxh_f16(__s0); \
  __ret; \
})
#else
#define vrecpxh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrecpxh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vrndh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndh_f16(__s0); \
  __ret; \
})
#else
#define vrndh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vrndah_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndah_f16(__s0); \
  __ret; \
})
#else
#define vrndah_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndah_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vrndih_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndih_f16(__s0); \
  __ret; \
})
#else
#define vrndih_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndih_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vrndmh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndmh_f16(__s0); \
  __ret; \
})
#else
#define vrndmh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndmh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vrndnh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndnh_f16(__s0); \
  __ret; \
})
#else
#define vrndnh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndnh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vrndph_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndph_f16(__s0); \
  __ret; \
})
#else
#define vrndph_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndph_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vrndxh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndxh_f16(__s0); \
  __ret; \
})
#else
#define vrndxh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrndxh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vrsqrteh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrsqrteh_f16(__s0); \
  __ret; \
})
#else
#define vrsqrteh_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrsqrteh_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vrsqrtsh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrsqrtsh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vrsqrtsh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vrsqrtsh_f16(__s0, __s1); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vsqrth_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vsqrth_f16(__s0); \
  __ret; \
})
#else
#define vsqrth_f16(__p0) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vsqrth_f16(__s0); \
  __ret; \
})
#endif

#ifdef __LITTLE_ENDIAN__
#define vsubh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vsubh_f16(__s0, __s1); \
  __ret; \
})
#else
#define vsubh_f16(__p0, __p1) __extension__ ({ \
  float16_t __s0 = __p0; \
  float16_t __s1 = __p1; \
  float16_t __ret; \
  __ret = (float16_t) __builtin_neon_vsubh_f16(__s0, __s1); \
  __ret; \
})
#endif

#endif

#undef __ai

#endif /* __ARM_FP16_H */
