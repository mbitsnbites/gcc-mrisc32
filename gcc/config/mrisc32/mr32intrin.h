/* MRISC32 intrinsics include file.

   Copyright (C) 2020 Free Software Foundation, Inc.
   Contributed by Marcus Geelnard <m@bitsnbites.eu>

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _GCC_MR32INTRIN_H
#define _GCC_MR32INTRIN_H_ 1

#ifndef __MRISC32__
#error "This include file is only compatible with MRISC32 target CPU:s."
#endif  /* __MRISC32__  */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO(m): This should be defined by the compiler. Right now we have to
   assume that all MRISC32 CPU:s support saturating/halving
   operations.  */
#define __MRISC32_SATURATING_OPS__ 1

#ifdef __MRISC32_PACKED_OPS__
/* We use specific typedefs for all supported packed data types, mostly
   for documentation purposes.  */
typedef uint32_t int8x4_t;
typedef uint32_t int16x2_t;
typedef uint32_t uint8x4_t;
typedef uint32_t uint16x2_t;
typedef uint32_t float8x4_t;
typedef uint32_t float16x2_t;
#endif  /* __MRISC32_PACKED_OPS__  */

/* TODO(m): We currently use inline assembly to implement the intrinsics
   functions. Ideally they should all be implemented as builtins.  */

static inline uint32_t _mr32_cpuid(uint32_t a, uint32_t b) { uint32_t r; __asm__ ("cpuid\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }

#ifdef __MRISC32_PACKED_OPS__
static inline int8x4_t _mr32_add_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("add.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_add_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("add.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int8x4_t _mr32_sub_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("sub.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_sub_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("sub.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#endif  /* __MRISC32_PACKED_OPS__  */

#ifdef __MRISC32_PACKED_OPS__
static inline uint8x4_t _mr32_seq_b(int8x4_t a, int8x4_t b) { uint8x4_t r; __asm__ ("seq.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_seq_h(int16x2_t a, int16x2_t b) { uint16x2_t r; __asm__ ("seq.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_sne_b(int8x4_t a, int8x4_t b) { uint8x4_t r; __asm__ ("sne.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_sne_h(int16x2_t a, int16x2_t b) { uint16x2_t r; __asm__ ("sne.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_slt_b(int8x4_t a, int8x4_t b) { uint8x4_t r; __asm__ ("slt.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_slt_h(int16x2_t a, int16x2_t b) { uint16x2_t r; __asm__ ("slt.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_sltu_b(uint8x4_t a, uint8x4_t b) { uint8x4_t r; __asm__ ("sltu.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_sltu_h(uint16x2_t a, uint16x2_t b) { uint16x2_t r; __asm__ ("sltu.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_sle_b(int8x4_t a, int8x4_t b) { uint8x4_t r; __asm__ ("sle.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_sle_h(int16x2_t a, int16x2_t b) { uint16x2_t r; __asm__ ("sle.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_sleu_b(uint8x4_t a, uint8x4_t b) { uint8x4_t r; __asm__ ("sleu.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_sleu_h(uint16x2_t a, uint16x2_t b) { uint16x2_t r; __asm__ ("sleu.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#endif  /* __MRISC32_PACKED_OPS__  */

/* TODO(m): Keep or drop the word-sized min/max intrinsics? They can be
   generated with regular C code. If we keep them, we should have
   immediate variants too.  */
static inline int32_t _mr32_min(int32_t a, int32_t b) { int32_t r; __asm__ ("min\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint32_t _mr32_minu(uint32_t a, uint32_t b) { uint32_t r; __asm__ ("minu\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int32_t _mr32_max(int32_t a, int32_t b) { int32_t r; __asm__ ("max\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint32_t _mr32_maxu(uint32_t a, uint32_t b) { uint32_t r; __asm__ ("maxu\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#ifdef __MRISC32_PACKED_OPS__
static inline int8x4_t _mr32_min_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("min.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_min_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("min.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_minu_b(uint8x4_t a, uint8x4_t b) { uint8x4_t r; __asm__ ("minu.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_minu_h(uint16x2_t a, uint16x2_t b) { uint16x2_t r; __asm__ ("minu.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int8x4_t _mr32_max_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("max.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_max_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("max.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_maxu_b(uint8x4_t a, uint8x4_t b) { uint8x4_t r; __asm__ ("maxu.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_maxu_h(uint16x2_t a, uint16x2_t b) { uint16x2_t r; __asm__ ("maxu.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#endif  /* __MRISC32_PACKED_OPS__  */

#ifdef __MRISC32_PACKED_OPS__
static inline int8x4_t _mr32_asr_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("asr.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_asr_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("asr.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int8x4_t _mr32_lsl_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("lsl.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_lsl_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("lsl.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_lsr_b(uint8x4_t a, int8x4_t b) { uint8x4_t r; __asm__ ("lsr.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_lsr_h(uint16x2_t a, int16x2_t b) { uint16x2_t r; __asm__ ("lsr.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#endif  /* __MRISC32_PACKED_OPS__  */

/* Create a control word for use with the SHUF instruction.
    sign_mode - 0 = zero-fill, 1 = sign-fill
    selN      - A 3-bit selector for byte N of the result word
                Bit 2:    Copy/fill mode (0 = copy, 1 = fill)
                Bits 0-1: Source byte index
                          (0 = least signficant byte,
                           3 = most significant byte)  */
#define _MR32_SHUFCTL(sign_mode, sel3, sel2, sel1, sel0) \
  (((((uint32_t)(sign_mode)) & 1u) << 12) | \
   ((((uint32_t)(sel3)) & 7u) << 9) | ((((uint32_t)(sel2)) & 7u) << 6) | \
   ((((uint32_t)(sel1)) & 7u) << 3) | (((uint32_t)(sel0)) & 7u))

/* Note: The second argument (the control word) must be a numeric
   constant. _MR32_SHUFCTL() can be used for creating a valid control
   word.  */
static inline uint32_t _mr32_shuf(uint32_t a, const uint32_t b) { uint32_t r; __asm__ ("shuf\t%0, %1, #%2" : "=r"(r) : "r"(a), "i"(b)); return r; }

static inline int32_t _mr32_mulq(int32_t a, int32_t b) { int32_t r; __asm__ ("mulq\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#ifdef __MRISC32_PACKED_OPS__
static inline int8x4_t _mr32_mulq_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("mulq.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_mulq_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("mulq.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int8x4_t _mr32_mul_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("mul.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_mul_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("mul.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int8x4_t _mr32_mulhi_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("mulhi.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_mulhi_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("mulhi.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_mulhiu_b(uint8x4_t a, uint8x4_t b) { uint8x4_t r; __asm__ ("mulhiu.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_mulhiu_h(uint16x2_t a, uint16x2_t b) { uint16x2_t r; __asm__ ("mulhiu.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#endif  /* __MRISC32_PACKED_OPS__  */

#ifdef __MRISC32_DIV__
#ifdef __MRISC32_PACKED_OPS__
static inline int8x4_t _mr32_div_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("div.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_div_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("div.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_divu_b(uint8x4_t a, uint8x4_t b) { uint8x4_t r; __asm__ ("divu.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_divu_h(uint16x2_t a, uint16x2_t b) { uint16x2_t r; __asm__ ("divu.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int8x4_t _mr32_rem_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("rem.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_rem_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("rem.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_remu_b(uint8x4_t a, uint8x4_t b) { uint8x4_t r; __asm__ ("remu.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_remu_h(uint16x2_t a, uint16x2_t b) { uint16x2_t r; __asm__ ("remu.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#endif  /* __MRISC32_PACKED_OPS__  */
#endif  /* __MRISC32_DIV__  */

#ifdef __MRISC32_SATURATING_OPS__
static inline int32_t _mr32_adds(int32_t a, int32_t b) { int32_t r; __asm__ ("adds\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint32_t _mr32_addsu(uint32_t a, uint32_t b) { uint32_t r; __asm__ ("addsu\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int32_t _mr32_addh(int32_t a, int32_t b) { int32_t r; __asm__ ("addh\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint32_t _mr32_addhu(uint32_t a, uint32_t b) { uint32_t r; __asm__ ("addhu\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int32_t _mr32_subs(int32_t a, int32_t b) { int32_t r; __asm__ ("subs\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint32_t _mr32_subsu(uint32_t a, uint32_t b) { uint32_t r; __asm__ ("subsu\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int32_t _mr32_subh(int32_t a, int32_t b) { int32_t r; __asm__ ("subh\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint32_t _mr32_subhu(uint32_t a, uint32_t b) { uint32_t r; __asm__ ("subhu\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#ifdef __MRISC32_PACKED_OPS__
static inline int8x4_t _mr32_adds_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("adds.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_adds_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("adds.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_addsu_b(uint8x4_t a, uint8x4_t b) { uint8x4_t r; __asm__ ("addsu.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_addsu_h(uint16x2_t a, uint16x2_t b) { uint16x2_t r; __asm__ ("addsu.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int8x4_t _mr32_addh_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("addh.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_addh_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("addh.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_addhu_b(uint8x4_t a, uint8x4_t b) { uint8x4_t r; __asm__ ("addhu.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_addhu_h(uint16x2_t a, uint16x2_t b) { uint16x2_t r; __asm__ ("addhu.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int8x4_t _mr32_subs_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("subs.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_subs_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("subs.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_subsu_b(uint8x4_t a, uint8x4_t b) { uint8x4_t r; __asm__ ("subsu.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_subsu_h(uint16x2_t a, uint16x2_t b) { uint16x2_t r; __asm__ ("subsu.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int8x4_t _mr32_subh_b(int8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("subh.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_subh_h(int16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("subh.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_subhu_b(uint8x4_t a, uint8x4_t b) { uint8x4_t r; __asm__ ("subhu.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_subhu_h(uint16x2_t a, uint16x2_t b) { uint16x2_t r; __asm__ ("subhu.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#endif  /* __MRISC32_PACKED_OPS__  */
#endif  /* __MRISC32_SATURATING_OPS__  */

static inline uint32_t _mr32_clz(uint32_t a) { uint32_t r; __asm__ ("clz\t%0, %1" : "=r"(r) : "r"(a)); return r; }
static inline uint32_t _mr32_rev(uint32_t a) { uint32_t r; __asm__ ("rev\t%0, %1" : "=r"(r) : "r"(a)); return r; }
#ifdef __MRISC32_PACKED_OPS__
static inline uint8x4_t _mr32_clz_b(uint8x4_t a) { uint8x4_t r; __asm__ ("clz.b\t%0, %1" : "=r"(r) : "r"(a)); return r; }
static inline uint16x2_t _mr32_clz_h(uint16x2_t a) { uint16x2_t r; __asm__ ("clz.h\t%0, %1" : "=r"(r) : "r"(a)); return r; }
static inline uint8x4_t _mr32_rev_b(uint8x4_t a) { uint8x4_t r; __asm__ ("rev.b\t%0, %1" : "=r"(r) : "r"(a)); return r; }
static inline uint16x2_t _mr32_rev_h(uint16x2_t a) { uint16x2_t r; __asm__ ("rev.h\t%0, %1" : "=r"(r) : "r"(a)); return r; }
#endif  /* __MRISC32_PACKED_OPS__  */

#ifdef __MRISC32_HARD_FLOAT__
static inline float _mr32_fmin(float a, float b) { float r; __asm__ ("fmin\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float _mr32_fmax(float a, float b) { float r; __asm__ ("fmax\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#ifdef __MRISC32_PACKED_OPS__
static inline float8x4_t _mr32_fmin_b(float8x4_t a, float8x4_t b) { float8x4_t r; __asm__ ("fmin.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float16x2_t _mr32_fmin_h(float16x2_t a, float16x2_t b) { float16x2_t r; __asm__ ("fmin.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float8x4_t _mr32_fmax_b(float8x4_t a, float8x4_t b) { float8x4_t r; __asm__ ("fmax.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float16x2_t _mr32_fmax_h(float16x2_t a, float16x2_t b) { float16x2_t r; __asm__ ("fmax.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#endif  /* __MRISC32_PACKED_OPS__  */
#endif  /* __MRISC32_HARD_FLOAT__  */

#ifdef __MRISC32_HARD_FLOAT__
#ifdef __MRISC32_PACKED_OPS__
static inline uint8x4_t _mr32_fseq_b(float8x4_t a, float8x4_t b) { uint8x4_t r; __asm__ ("fseq.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_fseq_h(float16x2_t a, float16x2_t b) { uint16x2_t r; __asm__ ("fseq.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_fsne_b(float8x4_t a, float8x4_t b) { uint8x4_t r; __asm__ ("fsne.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_fsne_h(float16x2_t a, float16x2_t b) { uint16x2_t r; __asm__ ("fsne.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_fslt_b(float8x4_t a, float8x4_t b) { uint8x4_t r; __asm__ ("fslt.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_fslt_h(float16x2_t a, float16x2_t b) { uint16x2_t r; __asm__ ("fslt.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_fsle_b(float8x4_t a, float8x4_t b) { uint8x4_t r; __asm__ ("fsle.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_fsle_h(float16x2_t a, float16x2_t b) { uint16x2_t r; __asm__ ("fsle.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#endif  /* __MRISC32_PACKED_OPS__  */
#endif  /* __MRISC32_HARD_FLOAT__  */

#ifdef __MRISC32_HARD_FLOAT__
static inline float _mr32_itof(int32_t a, int32_t b) { float r; __asm__ ("itof\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float _mr32_utof(uint32_t a, int32_t b) { float r; __asm__ ("utof\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int32_t _mr32_ftoi(float a, int32_t b) { int32_t r; __asm__ ("ftoi\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint32_t _mr32_ftou(float a, int32_t b) { uint32_t r; __asm__ ("ftou\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int32_t _mr32_ftoir(float a, int32_t b) { int32_t r; __asm__ ("ftoir\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint32_t _mr32_ftour(float a, int32_t b) { uint32_t r; __asm__ ("ftour\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#ifdef __MRISC32_PACKED_OPS__
static inline float8x4_t _mr32_itof_b(int8x4_t a, int8x4_t b) { float8x4_t r; __asm__ ("itof.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float16x2_t _mr32_itof_h(int16x2_t a, int16x2_t b) { float16x2_t r; __asm__ ("itof.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float8x4_t _mr32_utof_b(uint8x4_t a, int8x4_t b) { float8x4_t r; __asm__ ("utof.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float16x2_t _mr32_utof_h(uint16x2_t a, int16x2_t b) { float16x2_t r; __asm__ ("utof.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int8x4_t _mr32_ftoi_b(float8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("ftoi.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_ftoi_h(float16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("ftoi.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_ftou_b(float8x4_t a, int8x4_t b) { uint8x4_t r; __asm__ ("ftou.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_ftou_h(float16x2_t a, int16x2_t b) { uint16x2_t r; __asm__ ("ftou.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int8x4_t _mr32_ftoir_b(float8x4_t a, int8x4_t b) { int8x4_t r; __asm__ ("ftoir.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline int16x2_t _mr32_ftoir_h(float16x2_t a, int16x2_t b) { int16x2_t r; __asm__ ("ftoir.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint8x4_t _mr32_ftour_b(float8x4_t a, int8x4_t b) { uint8x4_t r; __asm__ ("ftour.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline uint16x2_t _mr32_ftour_h(float16x2_t a, int16x2_t b) { uint16x2_t r; __asm__ ("ftour.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#endif  /* __MRISC32_PACKED_OPS__  */
#endif  /* __MRISC32_HARD_FLOAT__  */

#ifdef __MRISC32_HARD_FLOAT__
#ifdef __MRISC32_PACKED_OPS__
static inline float8x4_t _mr32_fadd_b(float8x4_t a, float8x4_t b) { float8x4_t r; __asm__ ("fadd.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float16x2_t _mr32_fadd_h(float16x2_t a, float16x2_t b) { float16x2_t r; __asm__ ("fadd.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float8x4_t _mr32_fsub_b(float8x4_t a, float8x4_t b) { float8x4_t r; __asm__ ("fsub.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float16x2_t _mr32_fsub_h(float16x2_t a, float16x2_t b) { float16x2_t r; __asm__ ("fsub.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float8x4_t _mr32_fmul_b(float8x4_t a, float8x4_t b) { float8x4_t r; __asm__ ("fmul.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float16x2_t _mr32_fmul_h(float16x2_t a, float16x2_t b) { float16x2_t r; __asm__ ("fmul.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float8x4_t _mr32_fdiv_b(float8x4_t a, float8x4_t b) { float8x4_t r; __asm__ ("fdiv.b\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
static inline float16x2_t _mr32_fdiv_h(float16x2_t a, float16x2_t b) { float16x2_t r; __asm__ ("fdiv.h\t%0, %1, %2" : "=r"(r) : "r"(a), "r"(b)); return r; }
#endif  /* __MRISC32_PACKED_OPS__  */
#endif  /* __MRISC32_HARD_FLOAT__  */

#ifdef __cplusplus
}
#endif

#endif  /* _GCC_MR32INTRIN_H  */

