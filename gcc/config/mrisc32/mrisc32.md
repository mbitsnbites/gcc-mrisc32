;; Machine description for MRISC32
;; Copyright (C) 2009-2019 Free Software Foundation, Inc.
;; Contributed by Marcus Geelnard <m@bitsnbites.eu>

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 3, or (at your
;; option) any later version.

;; GCC is distributed in the hope that it will be useful, but WITHOUT
;; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
;; License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

;; -------------------------------------------------------------------------
;; MRISC32 constants
;; -------------------------------------------------------------------------

;; Register numbers
(define_constants
  [
    (MRISC32_Z      0)
    (MRISC32_S1     1)
    (MRISC32_S2     2)
    (MRISC32_S3     3)
    (MRISC32_S4     4)
    (MRISC32_S5     5)
    (MRISC32_S6     6)
    (MRISC32_S7     7)
    (MRISC32_S8     8)
    (MRISC32_S9     9)
    (MRISC32_S10    10)
    (MRISC32_S11    11)
    (MRISC32_S12    12)
    (MRISC32_S13    13)
    (MRISC32_S14    14)
    (MRISC32_S15    15)
    (MRISC32_S16    16)
    (MRISC32_S17    17)
    (MRISC32_S18    18)
    (MRISC32_S19    19)
    (MRISC32_S20    20)
    (MRISC32_S21    21)
    (MRISC32_S22    22)
    (MRISC32_S23    23)
    (MRISC32_S24    24)
    (MRISC32_S25    25)
    (MRISC32_FP     26)
    (MRISC32_TP     27)
    (MRISC32_SP     28)
    (MRISC32_VL     29)
    (MRISC32_LR     30)
    (MRISC32_PC     31)
    (MRISC32_VZ     32)
    (MRISC32_V1     33)
    (MRISC32_V2     34)
    (MRISC32_V3     35)
    (MRISC32_V4     36)
    (MRISC32_V5     37)
    (MRISC32_V6     38)
    (MRISC32_V7     39)
    (MRISC32_V8     40)
    (MRISC32_V9     41)
    (MRISC32_V10    42)
    (MRISC32_V11    43)
    (MRISC32_V12    44)
    (MRISC32_V13    45)
    (MRISC32_V14    46)
    (MRISC32_V15    47)
    (MRISC32_V16    48)
    (MRISC32_V17    49)
    (MRISC32_V18    50)
    (MRISC32_V19    51)
    (MRISC32_V20    52)
    (MRISC32_V21    53)
    (MRISC32_V22    54)
    (MRISC32_V23    55)
    (MRISC32_V24    56)
    (MRISC32_V25    57)
    (MRISC32_V26    58)
    (MRISC32_V27    59)
    (MRISC32_V28    60)
    (MRISC32_V29    61)
    (MRISC32_V30    62)
    (MRISC32_V31    63)
  ]
)

(define_c_enum "unspec" [
   UNSPEC_AVG
])


;; -------------------------------------------------------------------------
;; MRISC32 specific constraints and predicates
;; -------------------------------------------------------------------------

(include "constraints.md")
(include "predicates.md")


;; -------------------------------------------------------------------------
;; Mode iterators and attributes
;; -------------------------------------------------------------------------

;; Most instructions are four bytes long.
(define_attr "length" "" (const_int 4))

;; All natively supported integer modes
(define_mode_iterator ALLI [SI HI QI])

;; All natively supported floating-point modes
(define_mode_iterator ALLF [SF])

;; All natively supported integer and floating-point modes
(define_mode_iterator ALLIF [SI HI QI SF])

;; Mnemonic suffix for packed modes
(define_mode_attr posuffix [(SI "") (HI ".h") (QI ".b") (SF "")])

;; Mnemonic suffix for load/store operations (signed and unsigned)
(define_mode_attr ldsuffix [(SI "w") (HI "h") (QI "b") (SF "w")])
(define_mode_attr ldusuffix [(SI "w") (HI "uh") (QI "ub") (SF "w")])


;; -------------------------------------------------------------------------
;; nop instruction
;; -------------------------------------------------------------------------

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")


;; -------------------------------------------------------------------------
;; Arithmetic instructions
;; TODO(m): Add support for DImode.
;; -------------------------------------------------------------------------

;; SImode

(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   add\\t%0, %1, %2
   add\\t%0, %1, #%2")

(define_insn "*add_shift_si4"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (ashift:SI (match_operand:SI 1 "register_operand" "r")
			    (match_operand:QI 2 "mrisc32_const_index_shift" "n"))
		 (match_operand:SI 3 "register_operand" "r")))]
  ""
{
  /* Convert the shift amount to a scale factor.  */
  operands[2] = GEN_INT (1 << INTVAL (operands[2]));
  return "ldea\\t%0, %3, %1*%2";
})

(define_insn "subsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(minus:SI (match_operand:SI 1 "mrisc32_reg_or_imm15_operand" "r,I")
		  (match_operand:SI 2 "register_operand" "r,r")))]
  ""
  "@
   sub\\t%0, %1, %2
   sub\\t%0, #%1, %2")

(define_insn "mulsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mult:SI (match_operand:SI 1 "register_operand" "r")
		 (match_operand:SI 2 "register_operand" "r")))]
  ""
  "mul\\t%0, %1, %2")

;; TODO(m): Add support for high word multiplication (mulhi and mulhiu).

(define_insn "divsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(div:SI (match_operand:SI 1 "register_operand" "r")
		(match_operand:SI 2 "register_operand" "r")))]
  "TARGET_DIV"
  "div\\t%0, %1, %2")

(define_insn "udivsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(udiv:SI (match_operand:SI 1 "register_operand" "r")
		 (match_operand:SI 2 "register_operand" "r")))]
  "TARGET_DIV"
  "divu\\t%0, %1, %2")

(define_insn "modsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mod:SI (match_operand:SI 1 "register_operand" "r")
		(match_operand:SI 2 "register_operand" "r")))]
  "TARGET_DIV"
  "rem\\t%0, %1, %2")

(define_insn "umodsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(umod:SI (match_operand:SI 1 "register_operand" "r")
		 (match_operand:SI 2 "register_operand" "r")))]
  "TARGET_DIV"
  "remu\\t%0, %1, %2")


;; HImode

(define_insn "addhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
	(plus:HI (match_operand:HI 1 "register_operand" "r")
		 (match_operand:HI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS"
  "add.h\\t%0, %1, %2")

(define_insn "subhi3"
  [(set (match_operand:HI 0 "register_operand"                     "=r,r")
	(minus:HI (match_operand:HI 1 "mrisc32_reg_or_zero_operand" "r,Z")
		  (match_operand:HI 2 "register_operand"            "r,r")))]
  "TARGET_PACKED_OPS"
  "@
   sub.h\\t%0, %1, %2
   sub.h\\t%0, z, %2")

(define_insn "mulhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
	(mult:HI (match_operand:HI 1 "register_operand" "r")
		 (match_operand:HI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS"
  "mul.h\\t%0, %1, %2")

(define_insn "divhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
	(div:HI (match_operand:HI 1 "register_operand" "r")
		(match_operand:HI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS && TARGET_DIV"
  "div.h\\t%0, %1, %2")

(define_insn "udivhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
	(udiv:HI (match_operand:HI 1 "register_operand" "r")
		 (match_operand:HI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS && TARGET_DIV"
  "divu.h\\t%0, %1, %2")

(define_insn "modhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
	(mod:HI (match_operand:HI 1 "register_operand" "r")
		(match_operand:HI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS && TARGET_DIV"
  "rem.h\\t%0, %1, %2")

(define_insn "umodhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
	(umod:HI (match_operand:HI 1 "register_operand" "r")
		 (match_operand:HI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS && TARGET_DIV"
  "remu.h\\t%0, %1, %2")


;; QImode

(define_insn "addqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
	(plus:QI (match_operand:QI 1 "register_operand" "r")
		 (match_operand:QI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS"
  "add.b\\t%0, %1, %2")

(define_insn "subqi3"
  [(set (match_operand:QI 0 "register_operand"                     "=r,r")
	(minus:QI (match_operand:QI 1 "mrisc32_reg_or_zero_operand" "r,Z")
		  (match_operand:QI 2 "register_operand"            "r,r")))]
  "TARGET_PACKED_OPS"
  "@
   sub.b\\t%0, %1, %2
   sub.b\\t%0, z, %2")

(define_insn "mulqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
	(mult:QI (match_operand:QI 1 "register_operand" "r")
		 (match_operand:QI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS"
  "mul.b\\t%0, %1, %2")

(define_insn "divqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
	(div:QI (match_operand:QI 1 "register_operand" "r")
		(match_operand:QI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS && TARGET_DIV"
  "div.b\\t%0, %1, %2")

(define_insn "udivqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
	(udiv:QI (match_operand:QI 1 "register_operand" "r")
		 (match_operand:QI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS && TARGET_DIV"
  "divu.b\\t%0, %1, %2")

(define_insn "modqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
	(mod:QI (match_operand:QI 1 "register_operand" "r")
		(match_operand:QI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS && TARGET_DIV"
  "rem.b\\t%0, %1, %2")

(define_insn "umodqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
	(umod:QI (match_operand:QI 1 "register_operand" "r")
		 (match_operand:QI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS && TARGET_DIV"
  "remu.b\\t%0, %1, %2")


;; SFmode

(define_insn "addsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(plus:SF (match_operand:SF 1 "register_operand" "r")
		 (match_operand:SF 2 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fadd\\t%0, %1, %2")

(define_insn "subsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(minus:SF (match_operand:SF 1 "register_operand" "r")
		  (match_operand:SF 2 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fsub\\t%0, %1, %2")

(define_insn "mulsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(mult:SF (match_operand:SF 1 "register_operand" "r")
		 (match_operand:SF 2 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fmul\\t%0, %1, %2")

(define_insn "divsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(div:SF (match_operand:SF 1 "register_operand" "r")
		(match_operand:SF 2 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fdiv\\t%0, %1, %2")

(define_insn "negsf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(neg:SF (match_operand:SF 1 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fsub\t%0, z, %1")


;; -------------------------------------------------------------------------
;; Fixed point multiplication.
;; TODO(m): Are these correct? How to test?
;; -------------------------------------------------------------------------

(define_insn "smulhssi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(truncate:SI
	  (ashiftrt:DI
	    (mult:DI
	      (sign_extend:DI (match_operand:SI 1 "register_operand" "r"))
	      (sign_extend:DI (match_operand:SI 2 "register_operand" "r")))
	    (const_int 31))))]
  ""
  "mulq\\t%0, %1, %2")

(define_insn "smulhshi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
	(truncate:HI
	  (ashiftrt:SI
	    (mult:SI
	      (sign_extend:SI (match_operand:HI 1 "register_operand" "r"))
	      (sign_extend:SI (match_operand:HI 2 "register_operand" "r")))
	    (const_int 15))))]
  "TARGET_PACKED_OPS"
  "mulq.h\\t%0, %1, %2")

(define_insn "smulhsqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
	(truncate:QI
	  (ashiftrt:HI
	    (mult:HI
	      (sign_extend:HI (match_operand:QI 1 "register_operand" "r"))
	      (sign_extend:HI (match_operand:QI 2 "register_operand" "r")))
	    (const_int 7))))]
  "TARGET_PACKED_OPS"
  "mulq.b\\t%0, %1, %2")


;; -------------------------------------------------------------------------
;; Logical instructions
;; -------------------------------------------------------------------------

(define_insn "andsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(and:SI (match_operand:SI 1 "register_operand" "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   and\\t%0, %1, %2
   and\\t%0, %1, #%2")

(define_insn "iorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(ior:SI (match_operand:SI 1 "register_operand" "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   or\\t%0, %1, %2
   or\\t%0, %1, #%2")

(define_insn "xorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(xor:SI (match_operand:SI 1 "register_operand" "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   xor\\t%0, %1, %2
   xor\\t%0, %1, #%2")

;; Optimization for ~(op1 | op2).

(define_insn "*norsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(and:SI (not:SI (match_operand:SI 1 "register_operand" "r,r"))
		(not:SI (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I"))))]
  ""
  "@
   nor\\t%0, %1, %2
   nor\\t%0, %1, #%2")

;; Optimization for op1 & ~op2.

(define_insn "*bicsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(and:SI (not:SI
		  (match_operand:SI 1 "mrisc32_reg_or_imm15_operand" "r,I"))
		(match_operand:SI 2 "register_operand" "r,r")))]
  ""
  "@
   bic\\t%0, %2, %1
   bic\\t%0, %2, #%1")

;; Optimization for op1 & op2 when op2 is a constant operand that is a byte
;; mask (e.g. 0x00ff00ff).

(define_insn "*shuf_byte_mask_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (match_operand:SI 1 "register_operand" "r")
		(match_operand:SI 2 "mrisc32_byte_mask_for_shuf" "")))]
  ""
{
  operands[2] = mrisc32_mask_to_shuf_ctrl (operands[2]);
  return "shuf\\t%0, %1, #%2";
})


;; -------------------------------------------------------------------------
;; Shift instructions
;; TODO(m): Add support for DImode.
;; -------------------------------------------------------------------------

;; SImode

(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(ashift:SI (match_operand:SI 1 "register_operand" "r,r")
		   (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   lsl\\t%0, %1, %2
   lsl\\t%0, %1, #%2")

(define_insn "ashrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(ashiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
		     (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   asr\\t%0, %1, %2
   asr\\t%0, %1, #%2")

(define_insn "lshrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(lshiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
		     (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   lsr\\t%0, %1, %2
   lsr\\t%0, %1, #%2")


;; HImode

(define_insn "ashlhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
	(ashift:HI (match_operand:HI 1 "register_operand" "r")
		   (match_operand:HI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS"
  "lsl.h\\t%0, %1, %2")

(define_insn "ashrhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
	(ashiftrt:HI (match_operand:HI 1 "register_operand" "r")
		     (match_operand:HI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS"
  "asr.h\\t%0, %1, %2")

(define_insn "lshrhi3"
  [(set (match_operand:HI 0 "register_operand" "=r")
	(lshiftrt:HI (match_operand:HI 1 "register_operand" "r")
		     (match_operand:HI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS"
  "lsr.h\\t%0, %1, %2")


;; QImode

(define_insn "ashlqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
	(ashift:QI (match_operand:QI 1 "register_operand" "r")
		   (match_operand:QI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS"
  "lsl.b\\t%0, %1, %2")

(define_insn "ashrqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
	(ashiftrt:QI (match_operand:QI 1 "register_operand" "r")
		     (match_operand:QI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS"
  "asr.b\\t%0, %1, %2")

(define_insn "lshrqi3"
  [(set (match_operand:QI 0 "register_operand" "=r")
	(lshiftrt:QI (match_operand:QI 1 "register_operand" "r")
		     (match_operand:QI 2 "register_operand" "r")))]
  "TARGET_PACKED_OPS"
  "lsr.b\\t%0, %1, %2")


;; -------------------------------------------------------------------------
;; Special instructions
;; TODO(m): Add support for DImode, HImode and QImode.
;; -------------------------------------------------------------------------

;; SImode

(define_insn "clzsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(clz:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "clz\\t%0, %1")

(define_insn "ctzsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ctz:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "rev\\t%0, %1\;clz\\t%0, %0"
  [(set_attr "length" "8")])

(define_insn "sminsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(smin:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   min\\t%0, %1, %2
   min\\t%0, %1, #%2")

(define_insn "uminsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(umin:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   minu\\t%0, %1, %2
   minu\\t%0, %1, #%2")

(define_insn "smaxsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(smax:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   max\\t%0, %1, %2
   max\\t%0, %1, #%2")

(define_insn "umaxsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(umax:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   maxu\\t%0, %1, %2
   maxu\\t%0, %1, #%2")

(define_insn "abssi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(abs:SI (match_operand:SI 1 "register_operand" "r")))
   (clobber (match_scratch:SI 2 "=r"))]
  ""
  "sub\\t%2, z, %1\;max\\t%0, %1, %2"
  [(set_attr "length" "8")])

(define_insn "bswaphi2"
  [(set (match_operand:HI 0 "register_operand" "=r")
	(bswap:HI (match_operand:HI 1 "register_operand" "r")))]
  ""
  "shuf\\t%0, %1, #2305")

(define_insn "bswapsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(bswap:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "shuf\\t%0, %1, #83")


;; SFmode

(define_insn "sminsf3"
  [(set (match_operand:SF 0 "register_operand"                    "=r,r")
	(smin:SF (match_operand:SF 1 "register_operand"            "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fmin\\t%0, %1, %2
   fmin\\t%0, %1, z")

(define_insn "smaxsf3"
  [(set (match_operand:SF 0 "register_operand"                    "=r,r")
	(smax:SF (match_operand:SF 1 "register_operand"            "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fmax\\t%0, %1, %2
   fmax\\t%0, %1, z")


;; -------------------------------------------------------------------------
;; Halving/saturating arithmetic instructions
;; -------------------------------------------------------------------------

;; TODO(m): Is this correct? How to test?
(define_insn "avgsi3_floor"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(unspec:SI [(match_operand:SI 1 "register_operand" "r")
		    (match_operand:SI 2 "register_operand" "r")]
	 UNSPEC_AVG))]
  ""
  "addh\\t%0, %1, %2")


;; -------------------------------------------------------------------------
;; Floating point conversion instructions
;; -------------------------------------------------------------------------

;; SF -> SI

(define_insn "fix_truncsfsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(fix:SI (match_operand:SF 1 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "ftoi\\t%0, %1, z")

(define_insn "fixuns_truncsfsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(unsigned_fix:SI (match_operand:SF 1 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "ftou\\t%0, %1, z")


;; SI -> SF

(define_insn "floatsisf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(float:SF (match_operand:SI 1 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "itof\\t%0, %1, z")

(define_insn "floatunssisf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(unsigned_float:SF (match_operand:SI 1 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "utof\\t%0, %1, z")


;; -------------------------------------------------------------------------
;; Integer conversion instructions
;; -------------------------------------------------------------------------

;; HImode

(define_insn "zero_extendhisi2"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r,r")
	(zero_extend:SI (match_operand:HI 1 "nonimmediate_operand" "r,A,BCW")))]
  ""
  "@
   shuf\\t%0, %1, #0x0b48
   addpchi\\t%0, #%1@pchi\;lduh\\t%0, %0, #%1+4@pclo
   lduh\\t%0, %1"
  [(set_attr "length" "4,8,4")])

(define_insn "extendhisi2"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r,r")
	(sign_extend:SI (match_operand:HI 1 "nonimmediate_operand" "r,A,BCW")))]
  ""
  "@
   shuf\\t%0, %1, #0x1b48
   addpchi\\t%0, #%1@pchi\;ldh\\t%0, %0, #%1+4@pclo
   ldh\\t%0, %1"
  [(set_attr "length" "4,8,4")])


;; QImode

(define_insn "zero_extendqisi2"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r,r")
	(zero_extend:SI (match_operand:QI 1 "nonimmediate_operand" "r,A,BCW")))]
  ""
  "@
   shuf\\t%0, %1, #0x0920
   addpchi\\t%0, #%1@pchi\;ldub\\t%0, %0, #%1+4@pclo
   ldub\\t%0, %1"
  [(set_attr "length" "4,8,4")])

(define_insn "extendqisi2"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r,r")
	(sign_extend:SI (match_operand:QI 1 "nonimmediate_operand" "r,A,BCW")))]
  ""
  "@
   shuf\\t%0, %1, #0x1920
   addpchi\\t%0, #%1@pchi\;ldb\\t%0, %0, #%1+4@pclo
   ldb\\t%0, %1"
  [(set_attr "length" "4,8,4")])



;; -------------------------------------------------------------------------
;; Move instructions
;; -------------------------------------------------------------------------

(define_expand "mov<mode>"
  [(parallel
    [(set (match_operand:ALLIF 0 "general_operand" "")
	  (match_operand:ALLIF 1 "general_operand" ""))
     (clobber (match_scratch:SI 2))])]
  ""
{
  /* If this is a store, force the value into a register.  */
  if (! (reload_in_progress || reload_completed))
  {
    if (MEM_P (operands[0]))
      {
	if (!mrisc32_const_zero_operand (operands[1], <MODE>mode))
	  operands[1] = force_reg (<MODE>mode, operands[1]);
	if (MEM_P (XEXP (operands[0], 0)))
	  operands[0] = gen_rtx_MEM (<MODE>mode, force_reg (<MODE>mode, XEXP (operands[0], 0)));
      }
    else
      {
	if (MEM_P (operands[1]) && MEM_P (XEXP (operands[1], 0)))
	  operands[1] = gen_rtx_MEM (<MODE>mode, force_reg (<MODE>mode, XEXP (operands[1], 0)));
      }
  }
})

;; Store to memory

(define_insn "*mov<mode>_store"
  [(set (match_operand:ALLIF 0 "memory_operand"             "=A,A,BCW,BCW")
	(match_operand:ALLIF 1 "mrisc32_reg_or_zero_operand" "r,Z,r,  Z"))
   (clobber (match_scratch:SI 2 "=&r,&r,X,X"))]
  ""
  "@
   addpchi\\t%2, #%0@pchi\;st<ldsuffix>\\t%1, %2, #%0+4@pclo
   addpchi\\t%2, #%0@pchi\;st<ldsuffix>\\tz, %2, #%0+4@pclo
   st<ldsuffix>\\t%1, %0
   st<ldsuffix>\\tz, %0"
  [(set_attr "length" "8,8,4,4")])

;; Load to register: SImode, HImode, QImode

(define_insn "*mov<mode>_load"
  [(set (match_operand:ALLI 0 "register_operand"          "=r,r,r,r")
	(match_operand:ALLI 1 "mrisc32_int_movsrc_operand" "r,i,A,BCW"))
   (clobber (match_scratch:SI 2 "=X,X,X,X"))]
  ""
  "@
   mov\\t%0, %1
   *return mrisc32_emit_load_immediate (operands[0], operands[1]);
   addpchi\\t%0, #%1@pchi\;ld<ldusuffix>\\t%0, %0, #%1+4@pclo
   ld<ldusuffix>\\t%0, %1"
  ;; TODO(m): Dynamically calculate the length for LDI.
  [(set_attr "length" "4,8,8,4")])

;; Load to register: SFmode

(define_insn "*mov<mode>_load"
  [(set (match_operand:ALLF 0 "register_operand"            "=r,r,r,r")
	(match_operand:ALLF 1 "mrisc32_float_movsrc_operand" "r,F,A,BCW"))
   (clobber (match_scratch:SI 2 "=X,X,X,X"))]
  ""
  "@
   mov\\t%0, %1
   *return mrisc32_emit_load_immediate (operands[0], operands[1]);
   addpchi\\t%0, #%1@pchi\;ld<ldusuffix>\\t%0, %0, #%1+4@pclo
   ld<ldusuffix>\\t%0, %1"
  ;; TODO(m): Dynamically calculate the length for LDI.
  [(set_attr "length" "4,8,8,4")])


;; -------------------------------------------------------------------------
;; Conditional branches
;; -------------------------------------------------------------------------

(define_expand "cbranchsi4"
  [(set (pc)
	(if_then_else (match_operator 0 "comparison_operator"
			[(match_operand:SI 1 "register_operand")
			 (match_operand:SI 2 "nonmemory_operand")])
		      (label_ref (match_operand 3 ""))
		      (pc)))]
  ""
{
  mrisc32_expand_conditional_branch (operands);
  DONE;
})

(define_expand "cbranchsf4"
  [(set (pc)
	(if_then_else (match_operator 0 "comparison_operator"
			[(match_operand:SF 1 "register_operand")
			 (match_operand:SF 2 "register_operand")])
		      (label_ref (match_operand 3 ""))
		      (pc)))]
  "TARGET_HARD_FLOAT"
{
  mrisc32_expand_conditional_branch (operands);
  DONE;
})

(define_expand "condjump"
  [(set (pc)
	(if_then_else (match_operand 0)
		      (label_ref (match_operand 1))
		      (pc)))])

;; We can compare against zero and branch in a single instruction.

(define_insn "*cbranchsi4_zero_any_comparison"
  [(set (pc)
	(if_then_else (match_operator 0 "comparison_operator"
			[(match_operand:SI 1 "register_operand" "r")
			 (match_operand:SI 2 "mrisc32_const_int_zero_operand" "O")])
		      (label_ref (match_operand 3 ""))
		      (pc)))]
  ""
  "b%C0\t%1, %l3")

;; We can compare eq/ne against -1 and branch in a single instruction.

(define_insn "*cbranchsi4_minusone_equality"
  [(set (pc)
	(if_then_else (match_operator 0 "mrisc32_equality_comparison_operator"
			[(match_operand:SI 1 "register_operand" "r")
			 (match_operand:SI 2 "mrisc32_const_int_minusone_operand" "M")])
		      (label_ref (match_operand 3 ""))
		      (pc)))]
  ""
{
  if (GET_CODE (operands[0]) == EQ)
    return "bs\t%1, %l3";
  else
    return "bns\t%1, %l3";
})


;; -------------------------------------------------------------------------
;; Set a register based on comparison
;; -------------------------------------------------------------------------

(define_expand "cstoresi4"
  [(set (match_operand:SI 0 "register_operand")
	(match_operator:SI 1 "comparison_operator"
	 [(match_operand:SI 2 "register_operand")
	  (match_operand:SI 3 "nonmemory_operand")]))]
  ""
{
  mrisc32_expand_scc (operands);
  DONE;
})


;; SImode

(define_insn "*seq"
  [(set (match_operand:SI 0 "register_operand"                   "=r,r")
	(eq:SI (match_operand:SI 1 "register_operand"             "r,r")
	       (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   seq\\t%0, %1, %2
   seq\\t%0, %1, #%2")

(define_insn "*sne"
  [(set (match_operand:SI 0 "register_operand"                   "=r,r")
	(ne:SI (match_operand:SI 1 "register_operand"             "r,r")
	       (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   sne\\t%0, %1, %2
   sne\\t%0, %1, #%2")

(define_insn "*slt"
  [(set (match_operand:SI 0 "register_operand"                   "=r,r")
	(lt:SI (match_operand:SI 1 "register_operand"             "r,r")
	       (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   slt\\t%0, %1, %2
   slt\\t%0, %1, #%2")

(define_insn "*sltu"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r")
	(ltu:SI (match_operand:SI 1 "register_operand"             "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   sltu\\t%0, %1, %2
   sltu\\t%0, %1, #%2")

(define_insn "*sle"
  [(set (match_operand:SI 0 "register_operand"                   "=r,r")
	(le:SI (match_operand:SI 1 "register_operand"             "r,r")
	       (match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   sle\\t%0, %1, %2
   sle\\t%0, %1, #%2")

(define_insn "*sleu"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r")
	(leu:SI (match_operand:SI 1 "register_operand"             "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_imm15_operand" "r,I")))]
  ""
  "@
   sleu\\t%0, %1, %2
   sleu\\t%0, %1, #%2")

;; The greater than (or equal) conditions are implemented by reversing
;; the operand order. Thus we do not support an immediate value as the
;; second operand, but we do support constant zero via the z register.

(define_insn "*sgt"
  [(set (match_operand:SI 0 "register_operand"                  "=r,r")
	(gt:SI (match_operand:SI 1 "register_operand"            "r,r")
	       (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  ""
  "@
   slt\\t%0, %2, %1
   slt\\t%0, z, %1")

(define_insn "*sgtu"
  [(set (match_operand:SI 0 "register_operand"                   "=r,r")
	(gtu:SI (match_operand:SI 1 "register_operand"            "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  ""
  "@
   sltu\\t%0, %2, %1
   sltu\\t%0, z, %1")

(define_insn "*sge"
  [(set (match_operand:SI 0 "register_operand"                  "=r,r")
	(ge:SI (match_operand:SI 1 "register_operand"            "r,r")
	       (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  ""
  "@
   sle\\t%0, %2, %1
   sle\\t%0, z, %1")

(define_insn "*sgeu"
  [(set (match_operand:SI 0 "register_operand"                   "=r,r")
	(geu:SI (match_operand:SI 1 "register_operand"            "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  ""
  "@
   sleu\\t%0, %2, %1
   sleu\\t%0, z, %1")


;; SFmode

(define_insn "*fseq"
  [(set (match_operand:SI 0 "register_operand"                  "=r,r")
	(eq:SI (match_operand:SF 1 "register_operand"            "r,r")
	       (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fseq\\t%0, %1, %2
   fseq\\t%0, %1, z")

(define_insn "*fsne"
  [(set (match_operand:SI 0 "register_operand"                  "=r,r")
	(ne:SI (match_operand:SF 1 "register_operand"            "r,r")
	       (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fsne\\t%0, %1, %2
   fsne\\t%0, %1, z")

(define_insn "*fslt"
  [(set (match_operand:SI 0 "register_operand"                  "=r,r")
	(lt:SI (match_operand:SF 1 "register_operand"            "r,r")
	       (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fslt\\t%0, %1, %2
   fslt\\t%0, %1, z")

(define_insn "*fsle"
  [(set (match_operand:SI 0 "register_operand"                  "=r,r")
	(le:SI (match_operand:SF 1 "register_operand"            "r,r")
	       (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fsle\\t%0, %1, %2
   fsle\\t%0, %1, z")

(define_insn "*fsgt"
  [(set (match_operand:SI 0 "register_operand"                  "=r,r")
	(gt:SI (match_operand:SF 1 "register_operand"            "r,r")
	       (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fslt\\t%0, %2, %1
   fslt\\t%0, z, %1")

(define_insn "*fsge"
  [(set (match_operand:SI 0 "register_operand"                  "=r,r")
	(ge:SI (match_operand:SF 1 "register_operand"            "r,r")
	       (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fsle\\t%0, %2, %1
   fsle\\t%0, z, %1")

;; TODO(m): Add proper support for unordered comparisons!
;; See riscv.c: riscv_emit_float_compare.

(define_insn "*fsuneq"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r")
	(uneq:SI (match_operand:SF 1 "register_operand"            "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fseq\\t%0, %1, %2
   fseq\\t%0, %1, z")

(define_insn "*fsltgt"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r")
	(ltgt:SI (match_operand:SF 1 "register_operand"            "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fsne\\t%0, %1, %2
   fsne\\t%0, %1, z")

(define_insn "*fsunlt"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r")
	(unlt:SI (match_operand:SF 1 "register_operand"            "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fslt\\t%0, %1, %2
   fslt\\t%0, %1, z")

(define_insn "*fsunle"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r")
	(unle:SI (match_operand:SF 1 "register_operand"            "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fsle\\t%0, %1, %2
   fsle\\t%0, %1, z")

(define_insn "*fsungt"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r")
	(ungt:SI (match_operand:SF 1 "register_operand"            "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fslt\\t%0, %2, %1
   fslt\\t%0, z, %1")

(define_insn "*fsunge"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r")
	(unge:SI (match_operand:SF 1 "register_operand"            "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fsle\\t%0, %2, %1
   fsle\\t%0, z, %1")


;; Ordered/unordered checks.
;; TODO(m): The MRISC32 ISA should have an instruction for checking if operands
;; are ordered. See https://github.com/mrisc32/mrisc32/issues/97

(define_insn "*fsord"
  [(set (match_operand:SI 0 "register_operand"            "=r")
	(ordered:SI (match_operand:SF 1 "register_operand" "r")
		    (match_operand:SF 2 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fsnan\\t%0, %1, %2\;nor\\t%0, %0, %0"
  [(set_attr "length" "8")])

(define_insn "*fsunord"
  [(set (match_operand:SI 0 "register_operand"              "=r")
	(unordered:SI (match_operand:SF 1 "register_operand" "r")
		      (match_operand:SF 2 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fsnan\\t%0, %1, %2")


;; -------------------------------------------------------------------------
;; Call and Jump instructions
;; -------------------------------------------------------------------------

(define_expand "call"
  [(parallel
    [(call (match_operand 0) (match_operand 1))
     (clobber (reg:SI MRISC32_LR))])]
  ""
{
  mrisc32_expand_call (NULL, operands[0], operands[1], false);
  DONE;
})

(define_expand "sibcall"
  [(call (match_operand 0) (match_operand 1))]
  ""
{
  mrisc32_expand_call (NULL, operands[0], operands[1], true);
  DONE;
})

(define_expand "call_value"
  [(parallel
    [(set (match_operand 0)
	  (call (match_operand 1) (match_operand 2)))
     (clobber (reg:SI MRISC32_LR))])]
  ""
{
  mrisc32_expand_call (operands[0], operands[1], operands[2], false);
  DONE;
})

(define_expand "sibcall_value"
  [(set (match_operand 0)
	(call (match_operand 1) (match_operand 2)))]
  ""
{
  mrisc32_expand_call (operands[0], operands[1], operands[2], true);
  DONE;
})

(define_insn "*call"
  [(call (mem:QI (match_operand:SI 0 "mrisc32_call_insn_operand" "r,s"))
	 (match_operand 1))
   (clobber (reg:SI MRISC32_LR))]
  "!SIBLING_CALL_P (insn)"
  "@
   jl\\t%0
   addpchi\\tlr, #%0@pchi\;jl\\tlr, #%0+4@pclo"
   [(set_attr "length" "4,8")])

(define_insn "*sibcall"
  [(call (mem:QI (match_operand:SI 0 "mrisc32_call_insn_operand" "c,s"))
	 (match_operand 1))]
  "SIBLING_CALL_P (insn)"
  "@
   j\\t%0
   b\\t#%0")

(define_insn "*call_value"
  [(set (match_operand 0)
	(call (mem:QI (match_operand:SI 1 "mrisc32_call_insn_operand" "r,s"))
	      (match_operand 2)))
   (clobber (reg:SI MRISC32_LR))]
  "!SIBLING_CALL_P (insn)"
  "@
   jl\\t%1
   addpchi\\tlr, #%1@pchi\;jl\\tlr, #%1+4@pclo"
   [(set_attr "length" "4,8")])

(define_insn "*sibcall_value"
  [(set (match_operand 0)
	(call (mem:QI (match_operand:SI 1 "mrisc32_call_insn_operand" "c,s"))
	      (match_operand 2)))]
  "SIBLING_CALL_P (insn)"
  "@
   j\\t%1
   b\\t#%1")

(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "nonimmediate_operand" "r"))]
  ""
  "j\\t%0")

(define_insn "jump"
  [(set (pc)
	(label_ref (match_operand 0 "" "")))]
  ""
  "b\\t#%l0")

(define_insn "simple_return"
  [(simple_return)]
  ""
  "ret")


;; -------------------------------------------------------------------------
;; Prologue & Epilogue
;; -------------------------------------------------------------------------

(define_expand "prologue"
  [(clobber (const_int 0))]
  ""
{
  mrisc32_expand_prologue ();
  DONE;
})

(define_expand "epilogue"
  [(return)]
  ""
{
  mrisc32_expand_epilogue ();
  emit_jump_insn (gen_simple_return ());
  DONE;
})

