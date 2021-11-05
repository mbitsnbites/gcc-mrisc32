;; Machine description for MRISC32
;; Copyright (C) 2019-2020 Free Software Foundation, Inc.
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
    (MRISC32_R1     1)
    (MRISC32_R2     2)
    (MRISC32_R3     3)
    (MRISC32_R4     4)
    (MRISC32_R5     5)
    (MRISC32_R6     6)
    (MRISC32_R7     7)
    (MRISC32_R8     8)
    (MRISC32_R9     9)
    (MRISC32_R10    10)
    (MRISC32_R11    11)
    (MRISC32_R12    12)
    (MRISC32_R13    13)
    (MRISC32_R14    14)
    (MRISC32_R15    15)
    (MRISC32_R16    16)
    (MRISC32_R17    17)
    (MRISC32_R18    18)
    (MRISC32_R19    19)
    (MRISC32_R20    20)
    (MRISC32_R21    21)
    (MRISC32_R22    22)
    (MRISC32_R23    23)
    (MRISC32_R24    24)
    (MRISC32_R25    25)
    (MRISC32_R26    26)
    (MRISC32_TP     27)
    (MRISC32_FP     28)
    (MRISC32_SP     29)
    (MRISC32_LR     30)
    (MRISC32_VL     31)
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

;; All natively supported integer modes.
(define_mode_iterator ANYI [SI HI QI])

;; All natively supported floating-point modes.
(define_mode_iterator ANYF [SF])

;; All natively supported integer and floating-point modes.
(define_mode_iterator ANYIF [SI HI QI SF])

;; Load & store operations for different modes.
(define_mode_attr modeload [(SI "ldw") (HI "lduh") (QI "ldub") (SF "ldw")])
(define_mode_attr modestore [(SI "stw") (HI "sth") (QI "stb") (SF "stw")])

;; Mnemonic suffix for different modes
(define_mode_attr modesuffix [(SI "") (HI ".h") (QI ".b") (SF "")])


;; -------------------------------------------------------------------------
;; Tuning attributes
;; -------------------------------------------------------------------------

;; Most instructions are four bytes long.
(define_attr "length" ""
  (const_int 4))

;; Microarchitectures we know how to tune for.
(define_attr "tune" "generic"
  (const_string "generic"))

;; Different instruction types.
(define_attr "type"
  "unknown,const,nop,
   arith,logical,shift,shuf,cmp,move,load,store,
   branch,jump,call,ret,
   imul,idiv,
   fadd,fmul,fdiv,ftoi,itof,fcmp,fminmax,
   saturating,
   alu_alu"
  (const_string "unknown"))


;; -------------------------------------------------------------------------
;; nop instruction
;; -------------------------------------------------------------------------

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop"
  [(set_attr "type" "nop")])


;; -------------------------------------------------------------------------
;; Arithmetic instructions
;; TODO(m): Add support for DImode.
;; -------------------------------------------------------------------------

;; SImode

(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   add\t%0,%1,%2
   add\t%0,%1,#%2"
  [(set_attr "type" "arith,arith")])

(define_insn "*add_shift_si4"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(plus:SI (ashift:SI (match_operand:SI 1 "register_operand" "r")
			    (match_operand:QI 2 "mrisc32_const_index_shift" "n"))
		 (match_operand:SI 3 "register_operand" "r")))]
  ""
{
  /* Convert the shift amount to a scale factor.  */
  operands[2] = GEN_INT (1 << INTVAL (operands[2]));
  return "ldea\t%0,%3,%1*%2";
}
  [(set_attr "type" "arith")])

(define_insn "subsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(minus:SI (match_operand:SI 1 "mrisc32_reg_or_i15hl_operand" "r,I")
		  (match_operand:SI 2 "register_operand" "r,r")))]
  ""
  "@
   sub\t%0,%1,%2
   sub\t%0,#%1,%2"
  [(set_attr "type" "arith,arith")])

(define_insn "negsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(neg:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "sub\t%0,z,%1"
  [(set_attr "type" "arith")])

(define_insn "mulsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(mult:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   mul\t%0,%1,%2
   mul\t%0,%1,#%2"
  [(set_attr "type" "imul,imul")])

(define_insn "maddsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(plus:SI (mult:SI (match_operand:SI 1 "register_operand" "r,r")
			  (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I"))
		 (match_operand:SI 3 "register_operand" "0,0")))]
  ""
  "@
   madd\t%0,%1,%2
   madd\t%0,%1,#%2"
  [(set_attr "type" "imul,imul")])

(define_insn "divsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(div:SI (match_operand:SI 1 "register_operand" "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  "TARGET_DIV"
  "@
   div\t%0,%1,%2
   div\t%0,%1,#%2"
  [(set_attr "type" "idiv,idiv")])

(define_insn "udivsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(udiv:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  "TARGET_DIV"
  "@
   divu\t%0,%1,%2
   divu\t%0,%1,#%2"
  [(set_attr "type" "idiv,idiv")])

(define_insn "modsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(mod:SI (match_operand:SI 1 "register_operand" "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  "TARGET_DIV"
  "@
   rem\t%0,%1,%2
   rem\t%0,%1,#%2"
  [(set_attr "type" "idiv,idiv")])

(define_insn "umodsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(umod:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  "TARGET_DIV"
  "@
   remu\t%0,%1,%2
   remu\t%0,%1,#%2"
  [(set_attr "type" "idiv,idiv")])


;; SFmode

(define_insn "addsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(plus:SF (match_operand:SF 1 "register_operand" "r")
		 (match_operand:SF 2 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fadd\t%0,%1,%2"
  [(set_attr "type" "fadd")])

(define_insn "subsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(minus:SF (match_operand:SF 1 "register_operand" "r")
		  (match_operand:SF 2 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fsub\t%0,%1,%2"
  [(set_attr "type" "fadd")])

(define_insn "mulsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(mult:SF (match_operand:SF 1 "register_operand" "r")
		 (match_operand:SF 2 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fmul\t%0,%1,%2"
  [(set_attr "type" "fmul")])

(define_insn "divsf3"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(div:SF (match_operand:SF 1 "register_operand" "r")
		(match_operand:SF 2 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fdiv\t%0,%1,%2"
  [(set_attr "type" "fdiv")])

(define_insn "negsf2"
  [(set (match_operand:SF 0 "register_operand" "=r")
	(neg:SF (match_operand:SF 1 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fsub\t%0,z,%1"
  [(set_attr "type" "fadd")])


;; -------------------------------------------------------------------------
;; Multiplications with high-part.
;; TODO(m): Implement all supported combinations.
;; -------------------------------------------------------------------------

(define_expand "mulsidi3"
  [(set (match_operand:DI 0 "register_operand"           "=&r")
	(mult:DI (sign_extend:DI
		   (match_operand:SI 1 "register_operand" "r"))
		 (sign_extend:DI
		   (match_operand:SI 2 "register_operand" "r"))))]
  ""
{
  rtx temp = gen_reg_rtx (SImode);
  emit_insn (gen_mulsi3 (temp, operands[1], operands[2]));
  emit_insn (gen_smulsi3_highpart (mrisc32_subreg_di_high (operands[0]),
				   operands[1], operands[2]));
  emit_insn (gen_movsi (mrisc32_subreg_di_low (operands[0]), temp));
  DONE;
})

;; TODO(m): This is a HACK! It works around mulsi3 mrisc32_reg_or_i15hl_operand
;; that does not match subreg, which is required by DI multiplication stuff.
(define_insn "*mulsi3_subreg"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(mult:SI (match_operand:SI 1 "register_operand" "r")
		 (match_operand:SI 2 "register_operand" "r")))]
  ""
  "mul\t%0,%1,%2"
  [(set_attr "type" "imul")])

(define_insn "smulsi3_highpart"
  [(set (match_operand:SI 0 "register_operand"               "=r")
	(truncate:SI
	  (lshiftrt:DI
	    (mult:DI (sign_extend:DI
		       (match_operand:SI 1 "register_operand" "r"))
		     (sign_extend:DI
		       (match_operand:SI 2 "register_operand" "r")))
	    (const_int 32))))]
  ""
  "mulhi\t%0,%1,%2"
  [(set_attr "type" "imul")])

(define_insn "umulsi3_highpart"
  [(set (match_operand:SI 0 "register_operand"               "=r")
	(truncate:SI
	  (lshiftrt:DI
	    (mult:DI (zero_extend:DI
		       (match_operand:SI 1 "register_operand" "r"))
		     (zero_extend:DI
		       (match_operand:SI 2 "register_operand" "r")))
	    (const_int 32))))]
  ""
  "mulhiu\t%0,%1,%2"
  [(set_attr "type" "imul")])


;; -------------------------------------------------------------------------
;; Logical instructions
;; -------------------------------------------------------------------------

(define_insn "and<mode>3"
  [(set (match_operand:ANYI 0 "register_operand" "=r,r")
	(and:ANYI (match_operand:ANYI 1 "register_operand" "r,r")
		  (match_operand:ANYI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   and\t%0,%1,%2
   and\t%0,%1,#%2"
  [(set_attr "type" "logical,logical")])

(define_insn "ior<mode>3"
  [(set (match_operand:ANYI 0 "register_operand" "=r,r")
	(ior:ANYI (match_operand:ANYI 1 "register_operand" "r,r")
		  (match_operand:ANYI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   or\t%0,%1,%2
   or\t%0,%1,#%2"
  [(set_attr "type" "logical,logical")])

(define_insn "xor<mode>3"
  [(set (match_operand:ANYI 0 "register_operand" "=r,r")
	(xor:ANYI (match_operand:ANYI 1 "register_operand" "r,r")
		  (match_operand:ANYI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   xor\t%0,%1,%2
   xor\t%0,%1,#%2"
  [(set_attr "type" "logical,logical")])

;; Optimizations for bitwise operations with negated inputs.

(define_insn "*and_np_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (not:SI (match_operand:SI 1 "register_operand" "r"))
		(match_operand:SI 2 "register_operand" "r")))]
  ""
  "and.np\t%0,%1,%2"
  [(set_attr "type" "logical")])

(define_insn "*and_nn_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (not:SI (match_operand:SI 1 "register_operand" "r"))
		(not:SI (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "and.nn\t%0,%1,%2"
  [(set_attr "type" "logical")])

(define_insn "*ior_np_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ior:SI (not:SI
		  (match_operand:SI 1 "register_operand" "r"))
		(match_operand:SI 2 "register_operand" "r")))]
  ""
  "or.np\t%0,%1,%2"
  [(set_attr "type" "logical")])

(define_insn "*ior_nn_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ior:SI (not:SI (match_operand:SI 1 "register_operand" "r"))
		(not:SI (match_operand:SI 2 "register_operand" "r"))))]
  ""
  "or.nn\t%0,%1,%2"
  [(set_attr "type" "logical")])

;; ~(a ^ b) == a ^ ~b
(define_insn "*xor_np_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(not:SI (xor:SI (match_operand:SI 1 "register_operand" "r")
			(match_operand:SI 2 "register_operand" "r"))))]
  ""
  "xor.np\t%0,%1,%2"
  [(set_attr "type" "logical")])

;; Optimization for op1 & op2 when op2 is a constant operand that is a byte
;; mask (e.g. 0x00ff00ff).

(define_insn "*shuf_byte_mask_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (match_operand:SI 1 "register_operand" "r")
		(match_operand:SI 2 "mrisc32_byte_mask_for_shuf" "")))]
  ""
{
  operands[2] = mrisc32_mask_to_shuf_ctrl (operands[2]);
  return "shuf\t%0,%1,#%2";
}
  [(set_attr "type" "shuf")])

;; Optimization for op1 & op2 when op2 is a constant operand that is a
;; mask consisting of contigous one-bits starting at the LSB (e.g.
;; 0x0001ffff).

(define_insn "*ebfu_mask_lowbits_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (match_operand:SI 1 "register_operand" "r")
		(match_operand 2 "const_int_operand" "i")))]
  "UINTVAL (operands[2]) != 0
   && UINTVAL (operands[2]) ==
      (0xffffffffU >> __builtin_clz (UINTVAL (operands[2])))"
{
  /* Convert the mask to a width.  */
  unsigned width = 32 - __builtin_clz (UINTVAL (operands[2]));
  operands[2] = GEN_INT (width);
  return "ebfu\t%0,%1,#<0:%2>";
}
  [(set_attr "type" "shift")])


;; -------------------------------------------------------------------------
;; Shift instructions
;; -------------------------------------------------------------------------

;; SImode

(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(ashift:SI (match_operand:SI 1 "register_operand" "r,r")
		   (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
{
  switch (which_alternative)
    {
      case 0:
	return "lsl\\t%0,%1,%2";
      case 1:
	operands[2] = gen_int_mode (UINTVAL (operands[2]) & 31, SImode);
	return "lsl\\t%0,%1,#%2";
      default:
	gcc_unreachable ();
    }
}
  [(set_attr "type" "shift,shift")])

(define_insn "ashrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(ashiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
		     (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
{
  switch (which_alternative)
    {
      case 0:
	return "asr\\t%0,%1,%2";
      case 1:
	operands[2] = gen_int_mode (UINTVAL (operands[2]) & 31, SImode);
	return "asr\\t%0,%1,#%2";
      default:
	gcc_unreachable ();
    }
}
  [(set_attr "type" "shift,shift")])

(define_insn "lshrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(lshiftrt:SI (match_operand:SI 1 "register_operand" "r,r")
		     (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
{
  switch (which_alternative)
    {
      case 0:
	return "lsr\\t%0,%1,%2";
      case 1:
	operands[2] = gen_int_mode (UINTVAL (operands[2]) & 31, SImode);
	return "lsr\\t%0,%1,#%2";
      default:
	gcc_unreachable ();
    }
}
  [(set_attr "type" "shift,shift")])

;; We implement bit rotation with an EBFU + IBF pair.

(define_insn_and_split "rotl<mode>3"
  [(set (match_operand:ANYI 0 "register_operand" "=&r")
	(rotate:ANYI (match_operand:ANYI 1 "register_operand" "r")
		     (match_operand:ANYI 2 "const_int_operand" "n")))]
  ""
  "#"
  "&& (UINTVAL (operands[2]) < GET_MODE_BITSIZE (<MODE>mode))"
  [(const_int 0)]
{
  HOST_WIDE_INT offset = INTVAL (operands[2]);
  HOST_WIDE_INT width = GET_MODE_BITSIZE (<MODE>mode) - offset;
  emit_insn (gen_extzv<mode> (operands[0], operands[1], GEN_INT (offset), GEN_INT (width)));
  emit_insn (gen_insv<mode> (operands[0], GEN_INT (width), GEN_INT (offset), operands[1]));
  DONE;
})


;; DImode
;;
;; Split constant shifts into combinations of EBF(U) + IBF + shift.
;; Note that shift lengths 32-63 are alread handled efficiently, so we do not
;; have to handle those cases.
;;
;; TODO(m): Implement register-based shift lengths.

(define_insn_and_split "ashldi3"
  [(set (match_operand:DI 0 "register_operand" "=&r")
	(ashift:DI (match_operand:DI 1 "register_operand" "r")
		   (match_operand:DI 2 "const_int_operand" "n")))]
  ""
  "#"
  "&& (UINTVAL (operands[2]) <= 31)"
  [(const_int 0)]
{
  HOST_WIDE_INT offset = INTVAL (operands[2]);
  HOST_WIDE_INT width = GET_MODE_BITSIZE (SImode) - offset;

  rtx dsth = mrisc32_subreg_di_high (operands[0]);
  rtx dstl = mrisc32_subreg_di_low (operands[0]);
  rtx srch = mrisc32_subreg_di_high (operands[1]);
  rtx srcl = mrisc32_subreg_di_low (operands[1]);

  // EBFU + IBF + LSL
  emit_insn (gen_extzvsi (dsth, srcl, GEN_INT (offset), GEN_INT (width)));
  emit_insn (gen_insvsi (dsth, GEN_INT (width), GEN_INT (offset), srch));
  emit_insn (gen_ashlsi3 (dstl, srcl, GEN_INT (offset)));
  DONE;
})

(define_insn_and_split "ashrdi3"
  [(set (match_operand:DI 0 "register_operand" "=&r")
	(ashiftrt:DI (match_operand:DI 1 "register_operand" "r")
		     (match_operand:DI 2 "const_int_operand" "n")))]
  ""
  "#"
  "&& (UINTVAL (operands[2]) <= 31)"
  [(const_int 0)]
{
  HOST_WIDE_INT offset = INTVAL (operands[2]);
  HOST_WIDE_INT width = GET_MODE_BITSIZE (SImode) - offset;

  rtx dsth = mrisc32_subreg_di_high (operands[0]);
  rtx dstl = mrisc32_subreg_di_low (operands[0]);
  rtx srch = mrisc32_subreg_di_high (operands[1]);
  rtx srcl = mrisc32_subreg_di_low (operands[1]);

  // EBFU + IBF + ASR
  emit_insn (gen_extzvsi (dstl, srcl, GEN_INT (width), GEN_INT (offset)));
  emit_insn (gen_insvsi (dstl, GEN_INT (offset), GEN_INT (width), srch));
  emit_insn (gen_ashrsi3 (dsth, srch, GEN_INT (offset)));
  DONE;
})

(define_insn_and_split "lshrdi3"
  [(set (match_operand:DI 0 "register_operand" "=&r")
	(lshiftrt:DI (match_operand:DI 1 "register_operand" "r")
		     (match_operand:DI 2 "const_int_operand" "n")))]
  ""
  "#"
  "&& (UINTVAL (operands[2]) <= 31)"
  [(const_int 0)]
{
  HOST_WIDE_INT offset = INTVAL (operands[2]);
  HOST_WIDE_INT width = GET_MODE_BITSIZE (SImode) - offset;

  rtx dsth = mrisc32_subreg_di_high (operands[0]);
  rtx dstl = mrisc32_subreg_di_low (operands[0]);
  rtx srch = mrisc32_subreg_di_high (operands[1]);
  rtx srcl = mrisc32_subreg_di_low (operands[1]);

  // EBFU + IBF + LSR
  emit_insn (gen_extzvsi (dstl, srcl, GEN_INT (width), GEN_INT (offset)));
  emit_insn (gen_insvsi (dstl, GEN_INT (offset), GEN_INT (width), srch));
  emit_insn (gen_lshrsi3 (dsth, srch, GEN_INT (offset)));
  DONE;
})


;; -------------------------------------------------------------------------
;; Bitfield instructions
;; -------------------------------------------------------------------------

(define_expand "extvsi"
  [(set (match_operand:SI 0 "register_operand")
	(sign_extract:SI (match_operand:SI 1 "register_operand")
			 (match_operand 2 "const_int_operand")
			 (match_operand 3 "const_int_operand")))]
  ""
{
  if (!(UINTVAL (operands[2]) <= 32 && UINTVAL (operands[3]) <= 31))
    FAIL;
})

(define_insn "*extvsi_internal"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(sign_extract:SI (match_operand:SI 1 "register_operand" "r")
			 (match_operand 2 "const_int_operand" "n")
			 (match_operand 3 "const_int_operand" "n")))]
  "UINTVAL (operands[2]) <= 32 && UINTVAL (operands[3]) <= 31"
{
  operands[2] = gen_int_mode (UINTVAL (operands[2]) & 31, SImode);
  return "ebf\t%0,%1,#<%3:%2>";
}
  [(set_attr "type" "shift")])

(define_expand "extzv<mode>"
  [(set (match_operand:ANYI 0 "register_operand")
	(zero_extract:ANYI (match_operand:ANYI 1 "register_operand")
			   (match_operand 2 "const_int_operand")
			   (match_operand 3 "const_int_operand")))]
  ""
{
  if (!(UINTVAL (operands[2]) <= GET_MODE_BITSIZE (<MODE>mode)
	&& UINTVAL (operands[3]) < GET_MODE_BITSIZE (<MODE>mode)))
    FAIL;
})

(define_insn "*extzv<mode>_internal"
  [(set (match_operand:ANYI 0 "register_operand" "=r")
	(zero_extract:ANYI (match_operand:ANYI 1 "register_operand" "r")
			   (match_operand 2 "const_int_operand" "n")
			   (match_operand 3 "const_int_operand" "n")))]
  "UINTVAL (operands[2]) <= GET_MODE_BITSIZE (<MODE>mode)
   && UINTVAL (operands[3]) < GET_MODE_BITSIZE (<MODE>mode)"
{
  operands[2] = gen_int_mode (UINTVAL (operands[2]) & 31, SImode);
  return "ebfu\t%0,%1,#<%3:%2>";
}
  [(set_attr "type" "shift")])

(define_expand "insv<mode>"
  [(set (zero_extract:ANYI (match_operand:ANYI 0 "register_operand")
			   (match_operand 1 "const_int_operand")
			   (match_operand 2 "const_int_operand"))
	(match_operand:ANYI 3 "general_operand"))]
  ""
{
  unsigned HOST_WIDE_INT width = UINTVAL (operands[1]);
  unsigned HOST_WIDE_INT offset = UINTVAL (operands[2]);
  if (width == 0 || (offset + width) > GET_MODE_BITSIZE (<MODE>mode))
    FAIL;

  operands[3] = force_reg (<MODE>mode, operands[3]);
})

(define_insn "*insv<mode>_internal"
  [(set (zero_extract:ANYI (match_operand:ANYI 0 "register_operand" "+r")
			   (match_operand 1 "const_int_operand" "n")
			   (match_operand 2 "const_int_operand" "n"))
	(match_operand:ANYI 3 "register_operand" "r"))]
  "!(UINTVAL (operands[1]) == 0
     || (UINTVAL (operands[2]) + UINTVAL (operands[1])
	 > GET_MODE_BITSIZE (<MODE>mode)))"
{
  operands[2] = gen_int_mode (UINTVAL (operands[2]) & 31, <MODE>mode);
  return "ibf\t%0,%3,#<%2:%1>";
}
  [(set_attr "type" "shift")])

;; Match (x & ~mask) | ((y << offset) & mask), where mask is on the form
;; 0b00...01...11 << offset
(define_insn "*ibfsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ior:SI (and:SI (match_operand:SI 1 "register_operand" "0")
			(match_operand 2 "const_int_operand" "n"))
		(and:SI (ashift:SI (match_operand:SI 3 "register_operand" "r")
				   (match_operand 4 "const_int_operand" "n"))
			(match_operand 5 "const_int_operand" "n"))))]
  "mrisc32_is_mask_offset_suitable_for_ibf (operands[5], operands[2], operands[4])"
{
  unsigned HOST_WIDE_INT offset = UINTVAL (operands[4]);
  unsigned HOST_WIDE_INT mask = UINTVAL (operands[5]) >> offset;
  unsigned HOST_WIDE_INT width = GET_MODE_BITSIZE (SImode) - __builtin_clz (mask);
  operands[5] = GEN_INT (width);
  return "ibf\t%0,%3,#<%4:%5>";
}
  [(set_attr "type" "shift")])

;; Match (x << offset) & (mask << offset), where mask is on the form
;; 0b00...01...11.
(define_insn "*mkbfsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (ashift:SI (match_operand:SI 1 "register_operand" "r")
			   (match_operand 2 "const_int_operand" "n"))
		(match_operand 3 "const_int_operand" "n")))]
  "mrisc32_is_mask_suitable_for_mkbf (operands[2], operands[3])"
{
  mrisc32_mask_to_mkbf_width_offset (operands[2], operands[3], &operands[2], &operands[3]);
  return "mkbf\t%0,%1,#<%3:%2>";
}
  [(set_attr "type" "shift")])


;; -------------------------------------------------------------------------
;; Special cases for masking, shifting and shuffling
;; -------------------------------------------------------------------------

(define_insn "bswaphi2"
  [(set (match_operand:HI 0 "register_operand" "=r")
	(bswap:HI (match_operand:HI 1 "register_operand" "r")))]
  ""
  "shuf\t%0,%1,#2305"
  [(set_attr "type" "shuf")])

(define_insn "bswapsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(bswap:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "shuf\t%0,%1,#83"
  [(set_attr "type" "shuf")])

(define_insn "*rotate_bytes_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(rotate:SI (match_operand:SI 1 "register_operand" "r")
		   (match_operand:SI 2 "const_int_operand" "i")))]
  "INTVAL (operands[2]) == 8
   || INTVAL (operands[2]) == 16
   || INTVAL (operands[2]) == 24"
{
  if (INTVAL (operands[2]) == 8)        // (x << 8) | (x >> 24)
    return "shuf\t%0,%1,#1091";
  else if (INTVAL (operands[2]) == 16)  // (x << 16) | (x >> 16)
    return "shuf\t%0,%1,#538";
  else if (INTVAL (operands[2]) == 24)  // (x << 24) | (x >> 8)
    return "shuf\t%0,%1,#209";
  gcc_unreachable ();
}
  [(set_attr "type" "shuf")])

;; Match: (a << 16) | (b & 0x0000ffff)
(define_insn "*pack2hi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ior:SI (ashift:SI (match_operand:SI 1 "register_operand" "r")
			   (const_int 16))
		(zero_extend:SI (match_operand:HI 2 "register_operand" "r"))))]
  "TARGET_PACKED_OPS"
  "pack\t%0,%1,%2"
  [(set_attr "type" "logical")])

;; Match: (a & 0xffff0000) | (b >> 16)
(define_insn "*packhi2hi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ior:SI (and:SI (match_operand:SI 1 "register_operand" "r")
			(const_int -65536))
		(lshiftrt:SI (match_operand:SI 2 "register_operand" "r")
			(const_int 16))))]
  "TARGET_PACKED_OPS"
  "packhi\t%0,%1,%2"
  [(set_attr "type" "logical")])

;; Match: ((a & 0x00ff00ff) << 8) | (b & 0x00ff00ff)
(define_insn "*pack4qi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ior:SI (and:SI (match_operand:SI 2 "register_operand" "r")
			(const_int 16711935))
		(and:SI (ashift:SI (match_operand:SI 1 "register_operand" "r")
				   (const_int 8))
			(const_int -16711936))))]
  "TARGET_PACKED_OPS"
  "pack.h\t%0,%1,%2"
  [(set_attr "type" "logical")])

;; Match: ((a & 0x0f0f0f0f) << 4) | (b & 0x0f0f0f0f)
(define_insn "*pack8nibbles3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ior:SI (and:SI (match_operand:SI 2 "register_operand" "r")
			(const_int 252645135))
		(and:SI (ashift:SI (match_operand:SI 1 "register_operand" "r")
				   (const_int 4))
			(const_int -252645136))))]
  "TARGET_PACKED_OPS"
  "pack.b\t%0,%1,%2"
  [(set_attr "type" "logical")])

;; Match: a & 0x0f0f0f0f
(define_insn "*and_0f0f0f0f_si"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (match_operand:SI 1 "register_operand" "r")
		(const_int 252645135)))]
  "TARGET_PACKED_OPS"
  "pack.b\t%0,z,%1"
  [(set_attr "type" "logical")])

;; Match: (a << 4) & 0xf0f0f0f0
(define_insn "*ashift4_and_f0f0f0f0_si"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(and:SI (ashift:SI (match_operand:SI 1 "register_operand" "r")
			   (const_int 4))
		(const_int -252645136)))]
  "TARGET_PACKED_OPS"
  "pack.b\t%0,%1,z"
  [(set_attr "type" "logical")])

;; Match: a < 0xffffu ? a : 0xffffu
(define_insn "*clamp_u16_si2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(umin:SI (match_operand:SI 1 "register_operand" "r")
		 (const_int 65535)))]
  "TARGET_PACKED_OPS && TARGET_SATURATING_OPS"
  "packsu\t%0,z,%1"
   [(set_attr "type" "logical")])

;; Match: (a < 0xffffu ? a : 0xffffu) << 16
(define_insn "*clamp_u16_shift16_si2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ashift:SI (umin:SI (match_operand:SI 1 "register_operand" "r")
			    (const_int 65535))
		   (const_int 16)))]
  "TARGET_PACKED_OPS && TARGET_SATURATING_OPS"
  "packsu\t%0,%1,z"
   [(set_attr "type" "logical")])

;; Match: a < -32768 ? -32768 : (a > 32767 ? 32767 : a)
(define_insn "*clamp_s16_si2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(smax:SI (smin:SI (match_operand:SI 1 "register_operand" "r")
			  (const_int 32767))
		 (const_int -32768)))]
  "TARGET_PACKED_OPS && TARGET_SATURATING_OPS"
  "packs\t%0,z,%1\;shuf\t%0,%0,#6984"
   [(set_attr "type" "alu_alu")
    (set_attr "length" "8")])


;; -------------------------------------------------------------------------
;; Special instructions
;; TODO(m): Add support for DImode, HImode and QImode.
;; -------------------------------------------------------------------------

;; SImode

(define_insn "clzsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(clz:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "clz\t%0,%1"
   [(set_attr "type" "logical")])

(define_insn "ctzsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ctz:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "rev\t%0,%1\;clz\t%0,%0"
  [(set_attr "length" "8")
   (set_attr "type" "alu_alu")])

(define_insn "popcountsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(popcount:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "popcnt\t%0,%1"
   [(set_attr "type" "logical")])

(define_insn "sminsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(smin:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   min\t%0,%1,%2
   min\t%0,%1,#%2"
   [(set_attr "type" "arith")])

(define_insn "uminsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(umin:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   minu\t%0,%1,%2
   minu\t%0,%1,#%2"
   [(set_attr "type" "arith")])

(define_insn "smaxsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(smax:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   max\t%0,%1,%2
   max\t%0,%1,#%2"
   [(set_attr "type" "arith")])

(define_insn "umaxsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(umax:SI (match_operand:SI 1 "register_operand" "r,r")
		 (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   maxu\t%0,%1,%2
   maxu\t%0,%1,#%2"
   [(set_attr "type" "arith")])

(define_insn "abssi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(abs:SI (match_operand:SI 1 "register_operand" "r")))
   (clobber (match_scratch:SI 2 "=&r"))]
  ""
  "sub\t%2,z,%1\;max\t%0,%1,%2"
  [(set_attr "length" "8")
   (set_attr "type" "alu_alu")])


;; SFmode

(define_insn "sminsf3"
  [(set (match_operand:SF 0 "register_operand"                        "=r,r")
	(smin:SF (match_operand:SF 1 "register_operand"                "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fmin\t%0,%1,%2
   fmin\t%0,%1,z")

(define_insn "smaxsf3"
  [(set (match_operand:SF 0 "register_operand"                        "=r,r")
	(smax:SF (match_operand:SF 1 "register_operand"                "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fmax\t%0,%1,%2
   fmax\t%0,%1,z")


;; -------------------------------------------------------------------------
;; Integer conversion instructions
;; -------------------------------------------------------------------------

;; HI -> SI

(define_insn "zero_extendhisi2"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r,r")
	(zero_extend:SI (match_operand:HI 1 "nonimmediate_operand" "r,A,R")))]
  ""
  "@
   shuf\t%0,%1,#2888
   addpchi\t%0,#%1@pchi\;lduh\t%0,%0,#%1+4@pclo
   lduh\t%0,%1"
  [(set_attr "length" "4,8,4")
   (set_attr "type" "shuf,load,load")])

(define_insn "extendhisi2"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r,r")
	(sign_extend:SI (match_operand:HI 1 "nonimmediate_operand" "r,A,R")))]
  ""
  "@
   shuf\t%0,%1,#6984
   addpchi\t%0,#%1@pchi\;ldh\t%0,%0,#%1+4@pclo
   ldh\t%0,%1"
  [(set_attr "length" "4,8,4")
   (set_attr "type" "shuf,load,load")])


;; QI -> SI

(define_insn "zero_extendqisi2"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r,r")
	(zero_extend:SI (match_operand:QI 1 "nonimmediate_operand" "r,A,R")))]
  ""
  "@
   shuf\t%0,%1,#2336
   addpchi\t%0,#%1@pchi\;ldub\t%0,%0,#%1+4@pclo
   ldub\t%0,%1"
  [(set_attr "length" "4,8,4")
   (set_attr "type" "shuf,load,load")])

(define_insn "extendqisi2"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r,r")
	(sign_extend:SI (match_operand:QI 1 "nonimmediate_operand" "r,A,R")))]
  ""
  "@
   shuf\t%0,%1,#6432
   addpchi\t%0,#%1@pchi\;ldb\t%0,%0,#%1+4@pclo
   ldb\t%0,%1"
  [(set_attr "length" "4,8,4")
   (set_attr "type" "shuf,load,load")])


;; -------------------------------------------------------------------------
;; Floating-point conversion instructions
;; -------------------------------------------------------------------------

;; SF -> SI

(define_insn "fix_truncsfsi2"
  [(set (match_operand:SI 0 "register_operand"        "=r")
	(fix:SI (match_operand:SF 1 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "ftoi\t%0,%1,z"
  [(set_attr "type" "ftoi")])

(define_insn "fixuns_truncsfsi2"
  [(set (match_operand:SI 0 "register_operand"                 "=r")
	(unsigned_fix:SI (match_operand:SF 1 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "ftou\t%0,%1,z"
  [(set_attr "type" "ftoi")])


;; SI -> SF

(define_insn "floatsisf2"
  [(set (match_operand:SF 0 "register_operand"          "=r")
	(float:SF (match_operand:SI 1 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "itof\t%0,%1,z"
  [(set_attr "type" "itof")])

(define_insn "floatunssisf2"
  [(set (match_operand:SF 0 "register_operand"                   "=r")
	(unsigned_float:SF (match_operand:SI 1 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "utof\t%0,%1,z"
  [(set_attr "type" "itof")])


;; -------------------------------------------------------------------------
;; Move & load/store instructions
;; -------------------------------------------------------------------------

(define_expand "mov<mode>"
  [(set (match_operand:ANYIF 0 "" "")
	(match_operand:ANYIF 1 "" ""))]
  ""
{
  if (mrisc32_legitimize_move (<MODE>mode, operands [0], operands [1]))
    DONE;
})

(define_insn "*mov<mode>"
  [(set (match_operand:ANYIF 0 "nonimmediate_operand"  "=r,r, r,r,A,A,R,R")
	(match_operand:ANYIF 1 "mrisc32_movsrc_operand" "r,iF,A,R,r,Z,r,Z"))]
  "register_operand (operands[0], <MODE>mode)
   || mrisc32_reg_or_zero_operand (operands[1], <MODE>mode)"
  "@
   mov\t%0,%1
   *return mrisc32_emit_load_immediate (operands[0], operands[1]);
   addpchi\t%0,#%1@pchi\;<modeload>\t%0,%0,#%1+4@pclo
   <modeload>\t%0,%1
   addpchi\tr15,#%0@pchi\;<modestore>\t%1,r15,#%0+4@pclo
   addpchi\tr15,#%0@pchi\;<modestore>\tz,r15,#%0+4@pclo
   <modestore>\t%1,%0
   <modestore>\tz,%0"
  [(set_attr "length" "4,8,8,4,8,8,4,4")
   (set_attr "type" "logical,logical,load,load,store,store,store,store")])


;; -------------------------------------------------------------------------
;; Conditional move
;; -------------------------------------------------------------------------

;; IMPORTANT! We need to use at least one define_insn() with a correct
;; if_then_else pattern to "unlock" the HAVE_conditional_move flag. See the
;; detection code in genconfig.c.

;; Here are some conditional move patterns that we support. We could
;; definitely do better (for instance, we don't need to treat the
;; destination register as an early-clobber unless there is a register
;; overlap between the destination and one of the sources), but we seem to
;; catch the most important cases and produce decent code.

;; SImode

(define_insn "*movsicc"
  [(set (match_operand:SI 0 "register_operand" "=&r,&r,&r,&r")
	(if_then_else:SI
	  (match_operator 5 "mrisc32_set_comparison_operator"
	    [(match_operand:SI 1 "register_operand"             "r,r,r,r")
	     (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I,r,I")])
	  (match_operand:SI 3 "register_operand"                "r,r,r,r")
	  (match_operand:SI 4 "mrisc32_reg_or_i15hl_operand"    "r,r,I,I")))]
  ""
  "@
   s%S5\t%0,%1,%2\;sel\t%0,%3,%4
   s%S5\t%0,%1,#%2\;sel\t%0,%3,%4
   s%S5\t%0,%1,%2\;sel\t%0,%3,#%4
   s%S5\t%0,%1,#%2\;sel\t%0,%3,#%4"
  [(set_attr "type" "alu_alu")
   (set_attr "length" "8")])

(define_insn "*movsicc_rev"
  [(set (match_operand:SI 0 "register_operand" "=&r,&r,&r,&r,&r")
	(if_then_else:SI
	  (match_operator 5 "mrisc32_set_rev_comparison_operator"
	    [(match_operand:SI 1 "register_operand"             "r,r,r,r,r")
	     (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I,r,I,O")])
	  (match_operand:SI 3 "register_operand"                "r,r,r,r,r")
	  (match_operand:SI 4 "mrisc32_reg_or_i15hl_operand"    "r,r,I,O,I")))]
  ""
  "@
   s%R5\t%0,%1,%2\;sel.132\t%0,%3,%4
   s%R5\t%0,%1,#%2\;sel.132\t%0,%3,%4
   s%Q5\t%0,%2,%1\;sel\t%0,%3,#%4
   s%R5\t%0,%1,#%2\;sel.132\t%0,%3,z
   s%Q5\t%0,z,%1\;sel\t%0,%3,#%4"
  [(set_attr "type" "alu_alu")
   (set_attr "length" "8")])

;; SFmode

(define_insn "*movsfcc"
  [(set (match_operand:SF 0 "register_operand" "=&r,&r,&r,&r")
	(if_then_else:SF
	  (match_operator 5 "mrisc32_set_comparison_operator"
	    [(match_operand:SF 1 "register_operand"             "r,r,r,r")
	     (match_operand:SF 2 "mrisc32_reg_or_zero_operand"  "r,Z,r,Z")])
	  (match_operand:SF 3 "register_operand"                "r,r,r,r")
	  (match_operand:SF 4 "mrisc32_reg_or_zero_operand"     "r,r,Z,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fs%S5\t%0,%1,%2\;sel\t%0,%3,%4
   fs%S5\t%0,%1,z\;sel\t%0,%3,%4
   fs%S5\t%0,%1,%2\;sel\t%0,%3,z
   fs%S5\t%0,%1,z\;sel\t%0,%3,z"
  [(set_attr "type" "alu_alu")
   (set_attr "length" "8")])

(define_insn "*movsfcc_rev"
  [(set (match_operand:SF 0 "register_operand" "=&r,&r,&r,&r")
	(if_then_else:SF
	  (match_operator 5 "mrisc32_set_rev_comparison_operator"
	    [(match_operand:SF 1 "register_operand"             "r,r,r,r")
	     (match_operand:SF 2 "mrisc32_reg_or_zero_operand"  "r,Z,r,Z")])
	  (match_operand:SF 3 "register_operand"                "r,r,r,r")
	  (match_operand:SF 4 "mrisc32_reg_or_zero_operand"     "r,r,Z,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fs%R5\t%0,%1,%2\;sel.132\t%0,%3,%4
   fs%R5\t%0,%1,z\;sel.132\t%0,%3,%4
   fs%R5\t%0,%1,%2\;sel.132\t%0,%3,z
   fs%R5\t%0,%1,z\;sel.132\t%0,%3,z"
  [(set_attr "type" "alu_alu")
   (set_attr "length" "8")])

;; SImode + SFmode
;; Note: This is required to enable movsfcc.

(define_insn "*movsfcc_sisf"
  [(set (match_operand:SF 0 "register_operand" "=&r,&r,&r,&r")
	(if_then_else:SF
	  (match_operator 5 "mrisc32_set_comparison_operator"
	    [(match_operand:SI 1 "register_operand"             "r,r,r,r")
	     (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I,r,I")])
	  (match_operand:SF 3 "register_operand"                "r,r,r,r")
	  (match_operand:SF 4 "mrisc32_reg_or_zero_operand"     "r,r,Z,Z")))]
  ""
  "@
   s%S5\t%0,%1,%2\;sel\t%0,%3,%4
   s%S5\t%0,%1,#%2\;sel\t%0,%3,%4
   s%S5\t%0,%1,%2\;sel\t%0,%3,z
   s%S5\t%0,%1,#%2\;sel\t%0,%3,z"
  [(set_attr "type" "alu_alu")
   (set_attr "length" "8")])

(define_insn "*movsfcc_sisf_rev"
  [(set (match_operand:SF 0 "register_operand" "=&r,&r,&r,&r")
	(if_then_else:SF
	  (match_operator 5 "mrisc32_set_rev_comparison_operator"
	    [(match_operand:SI 1 "register_operand"             "r,r,r,r")
	     (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I,r,I")])
	  (match_operand:SF 3 "register_operand"                "r,r,r,r")
	  (match_operand:SF 4 "mrisc32_reg_or_zero_operand"     "r,r,Z,Z")))]
  ""
  "@
   s%R5\t%0,%1,%2\;sel.132\t%0,%3,%4
   s%R5\t%0,%1,#%2\;sel.132\t%0,%3,%4
   s%R5\t%0,%1,%2\;sel.132\t%0,%3,z
   s%R5\t%0,%1,#%2\;sel.132\t%0,%3,z"
  [(set_attr "type" "alu_alu")
   (set_attr "length" "8")])

;; SFmode + SImode

(define_insn "*movsfcc_sfsi"
  [(set (match_operand:SI 0 "register_operand" "=&r,&r,&r,&r")
	(if_then_else:SI
	  (match_operator 5 "mrisc32_set_comparison_operator"
	    [(match_operand:SF 1 "register_operand"             "r,r,r,r")
	     (match_operand:SF 2 "mrisc32_reg_or_zero_operand"  "r,Z,r,Z")])
	  (match_operand:SI 3 "register_operand"                "r,r,r,r")
	  (match_operand:SI 4 "mrisc32_reg_or_i15hl_operand"    "r,r,I,I")))]
  "TARGET_HARD_FLOAT"
  "@
   fs%S5\t%0,%1,%2\;sel\t%0,%3,%4
   fs%S5\t%0,%1,z\;sel\t%0,%3,%4
   fs%S5\t%0,%1,%2\;sel\t%0,%3,#%4
   fs%S5\t%0,%1,z\;sel\t%0,%3,#%4"
  [(set_attr "type" "alu_alu")
   (set_attr "length" "8")])

(define_insn "*movsfcc_sfsi_rev"
  [(set (match_operand:SI 0 "register_operand" "=&r,&r,&r,&r")
	(if_then_else:SI
	  (match_operator 5 "mrisc32_set_rev_comparison_operator"
	    [(match_operand:SF 1 "register_operand"             "r,r,r,r")
	     (match_operand:SF 2 "mrisc32_reg_or_zero_operand"  "r,Z,r,Z")])
	  (match_operand:SI 3 "register_operand"                "r,r,r,r")
	  (match_operand:SI 4 "mrisc32_reg_or_i15hl_operand"    "r,r,I,I")))]
  "TARGET_HARD_FLOAT"
  "@
   fs%R5\t%0,%1,%2\;sel.132\t%0,%3,%4
   fs%R5\t%0,%1,z\;sel.132\t%0,%3,%4
   fs%Q5\t%0,%2,%1\;sel\t%0,%3,#%4
   fs%Q5\t%0,z,%1\;sel\t%0,%3,#%4"
  [(set_attr "type" "alu_alu")
   (set_attr "length" "8")])

;; The SEL instruction, expressed as a standard combination of xor+and+xor
;; that is emitted for the code (a & mask) | (b & ~mask).
;;
;; Equivalent to (this is what GCC would generate without this insn):
;;   xor    result, a, b
;;   and    result, result, mask
;;   xor    result, result, b
;;
;; MRISC32 instructions:
;;   sel      mask, a, b  (overwrite mask)
;;   sel.132  mask, b, a  (overwrite mask)
;;   sel.213  a, mask, b  (overwrite a)
;;   sel.231  b, mask, a  (overwrite b)
;;
;; Operands:
;;   0: result
;;   1: a
;;   2: b
;;   3: mask

(define_insn "mrisc32_sel"
  [(set (match_operand:SI 0 "register_operand"                        "+r,r,r,r")
	(xor:SI
	  (and:SI
	    (xor:SI (match_operand:SI 1 "register_operand"             "r,r,0,r")
		    (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I,r,0"))
	    (match_operand:SI 3 "register_operand"                     "0,0,r,r"))
	  (match_dup 2)))]
  ""
  "@
   sel\t%3,%1,%2
   sel\t%3,%1,#%2
   sel.213\t%1,%3,%2
   sel.231\t%2,%3,%1"
  [(set_attr "type" "logical")])



;; -------------------------------------------------------------------------
;; Conditional branches
;; -------------------------------------------------------------------------

(define_expand "cbranchsi4"
  [(use (match_operator 0 "ordered_comparison_operator"
         [(match_operand:SI 1 "register_operand")
          (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand")]))
   (use (match_operand 3 ""))]
  ""
{
  mrisc32_expand_conditional_branch (operands, SImode);
  DONE;
})

(define_expand "cbranchsf4"
  [(use (match_operator 0 "ordered_comparison_operator"
         [(match_operand:SF 1 "register_operand")
          (match_operand:SF 2 "register_operand")]))
   (use (match_operand 3 ""))]
  "TARGET_HARD_FLOAT"
{
  mrisc32_expand_conditional_branch (operands, SFmode);
  DONE;
})

;; We can compare against zero and branch in a single instruction.

(define_insn "*cbranchsi4_zero_any_comparison"
  [(set (pc)
	(if_then_else (match_operator 0 "comparison_operator"
			[(match_operand:SI 1 "register_operand" "r")
			 (match_operand:SI 2 "mrisc32_const_int_zero_operand" "O")])
		      (label_ref (match_operand 3 ""))
		      (pc)))]
  ""
  "b%C0\t%1,%l3"
  [(set_attr "type" "branch")])


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
    return "bs\t%1,%l3";
  else
    return "bns\t%1,%l3";
}
  [(set_attr "type" "branch")])


;; -------------------------------------------------------------------------
;; Set a register based on comparison
;; -------------------------------------------------------------------------

;; SImode

(define_insn "*seq"
  [(set (match_operand:SI 0 "register_operand"                   "=r,r")
	(eq:SI (match_operand:SI 1 "register_operand"             "r,r")
	       (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   seq\t%0,%1,%2
   seq\t%0,%1,#%2"
  [(set_attr "type" "cmp,cmp")])

(define_insn "*sne"
  [(set (match_operand:SI 0 "register_operand"                   "=r,r")
	(ne:SI (match_operand:SI 1 "register_operand"             "r,r")
	       (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   sne\t%0,%1,%2
   sne\t%0,%1,#%2"
  [(set_attr "type" "cmp,cmp")])

(define_insn "*slt"
  [(set (match_operand:SI 0 "register_operand"                   "=r,r")
	(lt:SI (match_operand:SI 1 "register_operand"             "r,r")
	       (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   slt\t%0,%1,%2
   slt\t%0,%1,#%2"
  [(set_attr "type" "cmp,cmp")])

(define_insn "*sltu"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r")
	(ltu:SI (match_operand:SI 1 "register_operand"             "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   sltu\t%0,%1,%2
   sltu\t%0,%1,#%2"
  [(set_attr "type" "cmp,cmp")])

(define_insn "*sle"
  [(set (match_operand:SI 0 "register_operand"                   "=r,r")
	(le:SI (match_operand:SI 1 "register_operand"             "r,r")
	       (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   sle\t%0,%1,%2
   sle\t%0,%1,#%2"
  [(set_attr "type" "cmp,cmp")])

(define_insn "*sleu"
  [(set (match_operand:SI 0 "register_operand"                    "=r,r")
	(leu:SI (match_operand:SI 1 "register_operand"             "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")))]
  ""
  "@
   sleu\t%0,%1,%2
   sleu\t%0,%1,#%2"
  [(set_attr "type" "cmp,cmp")])

;; The greater than (or equal) conditions are implemented by reversing
;; the operand order. Thus we do not support an immediate value as the
;; second operand, but we do support constant zero via the z register.

(define_insn "*sgt"
  [(set (match_operand:SI 0 "register_operand"                      "=r,r")
	(gt:SI (match_operand:SI 1 "register_operand"                "r,r")
	       (match_operand:SI 2 "mrisc32_reg_or_int_zero_operand" "r,O")))]
  ""
  "@
   slt\t%0,%2,%1
   slt\t%0,z,%1"
  [(set_attr "type" "cmp,cmp")])

(define_insn "*sgtu"
  [(set (match_operand:SI 0 "register_operand"                       "=r,r")
	(gtu:SI (match_operand:SI 1 "register_operand"                "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_int_zero_operand" "r,O")))]
  ""
  "@
   sltu\t%0,%2,%1
   sltu\t%0,z,%1"
  [(set_attr "type" "cmp,cmp")])

(define_insn "*sge"
  [(set (match_operand:SI 0 "register_operand"                      "=r,r")
	(ge:SI (match_operand:SI 1 "register_operand"                "r,r")
	       (match_operand:SI 2 "mrisc32_reg_or_int_zero_operand" "r,O")))]
  ""
  "@
   sle\t%0,%2,%1
   sle\t%0,z,%1"
  [(set_attr "type" "cmp,cmp")])

(define_insn "*sgeu"
  [(set (match_operand:SI 0 "register_operand"                       "=r,r")
	(geu:SI (match_operand:SI 1 "register_operand"                "r,r")
		(match_operand:SI 2 "mrisc32_reg_or_int_zero_operand" "r,O")))]
  ""
  "@
   sleu\t%0,%2,%1
   sleu\t%0,z,%1"
  [(set_attr "type" "cmp,cmp")])


;; SFmode

(define_insn "*fseq"
  [(set (match_operand:SI 0 "register_operand"                      "=r,r")
	(eq:SI (match_operand:SF 1 "register_operand"                "r,r")
	       (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fseq\t%0,%1,%2
   fseq\t%0,%1,z"
  [(set_attr "type" "fcmp,fcmp")])

(define_insn "*fsne"
  [(set (match_operand:SI 0 "register_operand"                      "=r,r")
	(ne:SI (match_operand:SF 1 "register_operand"                "r,r")
	       (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fsne\t%0,%1,%2
   fsne\t%0,%1,z"
  [(set_attr "type" "fcmp,fcmp")])

(define_insn "*fslt"
  [(set (match_operand:SI 0 "register_operand"                      "=r,r")
	(lt:SI (match_operand:SF 1 "register_operand"                "r,r")
	       (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fslt\t%0,%1,%2
   fslt\t%0,%1,z"
  [(set_attr "type" "fcmp,fcmp")])

(define_insn "*fsle"
  [(set (match_operand:SI 0 "register_operand"                      "=r,r")
	(le:SI (match_operand:SF 1 "register_operand"                "r,r")
	       (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fsle\t%0,%1,%2
   fsle\t%0,%1,z"
  [(set_attr "type" "fcmp,fcmp")])

(define_insn "*fsgt"
  [(set (match_operand:SI 0 "register_operand"                      "=r,r")
	(gt:SI (match_operand:SF 1 "register_operand"                "r,r")
	       (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fslt\t%0,%2,%1
   fslt\t%0,z,%1"
  [(set_attr "type" "fcmp,fcmp")])

(define_insn "*fsge"
  [(set (match_operand:SI 0 "register_operand"                      "=r,r")
	(ge:SI (match_operand:SF 1 "register_operand"                "r,r")
	       (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fsle\t%0,%2,%1
   fsle\t%0,z,%1"
  [(set_attr "type" "fcmp,fcmp")])

;; TODO(m): Add proper support for unordered comparisons!
;; See riscv.c: riscv_emit_float_compare.

(define_insn "*fsuneq"
  [(set (match_operand:SI 0 "register_operand"                        "=r,r")
	(uneq:SI (match_operand:SF 1 "register_operand"                "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fseq\t%0,%1,%2
   fseq\t%0,%1,z"
  [(set_attr "type" "fcmp,fcmp")])

(define_insn "*fsltgt"
  [(set (match_operand:SI 0 "register_operand"                        "=r,r")
	(ltgt:SI (match_operand:SF 1 "register_operand"                "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fsne\t%0,%1,%2
   fsne\t%0,%1,z"
  [(set_attr "type" "fcmp,fcmp")])

(define_insn "*fsunlt"
  [(set (match_operand:SI 0 "register_operand"                        "=r,r")
	(unlt:SI (match_operand:SF 1 "register_operand"                "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fslt\t%0,%1,%2
   fslt\t%0,%1,z"
  [(set_attr "type" "fcmp,fcmp")])

(define_insn "*fsunle"
  [(set (match_operand:SI 0 "register_operand"                        "=r,r")
	(unle:SI (match_operand:SF 1 "register_operand"                "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fsle\t%0,%1,%2
   fsle\t%0,%1,z"
  [(set_attr "type" "fcmp,fcmp")])

(define_insn "*fsungt"
  [(set (match_operand:SI 0 "register_operand"                        "=r,r")
	(ungt:SI (match_operand:SF 1 "register_operand"                "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fslt\t%0,%2,%1
   fslt\t%0,z,%1"
  [(set_attr "type" "fcmp,fcmp")])

(define_insn "*fsunge"
  [(set (match_operand:SI 0 "register_operand"                        "=r,r")
	(unge:SI (match_operand:SF 1 "register_operand"                "r,r")
		 (match_operand:SF 2 "mrisc32_reg_or_dbl_zero_operand" "r,Z")))]
  "TARGET_HARD_FLOAT"
  "@
   fsle\t%0,%2,%1
   fsle\t%0,z,%1"
  [(set_attr "type" "fcmp,fcmp")])


;; Ordered/unordered checks.

(define_insn "*fsord"
  [(set (match_operand:SI 0 "register_operand"            "=r")
	(ordered:SI (match_operand:SF 1 "register_operand" "r")
		    (match_operand:SF 2 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fsord\t%0,%1,%2"
  [(set_attr "type" "fcmp")])

(define_insn "*fsunord"
  [(set (match_operand:SI 0 "register_operand"              "=r")
	(unordered:SI (match_operand:SF 1 "register_operand" "r")
		      (match_operand:SF 2 "register_operand" "r")))]
  "TARGET_HARD_FLOAT"
  "fsunord\t%0,%1,%2"
  [(set_attr "type" "fcmp")])


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


;; -------------------------------------------------------------------------
;; Call and Jump instructions
;; -------------------------------------------------------------------------

(define_expand "sibcall"
  [(parallel [(call (match_operand 0 "")
		    (match_operand 1 ""))
	      (use (match_operand 2 ""))	;; next_arg_reg
	      (use (match_operand 3 ""))])]	;; struct_value_size_rtx
  ""
{
  rtx target = mrisc32_legitimize_call_address (XEXP (operands[0], 0));
  emit_call_insn (gen_sibcall_internal (target, operands[1]));
  DONE;
})

(define_insn "sibcall_internal"
  [(call (mem:SI (match_operand 0 "mrisc32_call_insn_operand" "c,s"))
	 (match_operand 1 "" ""))]
  "SIBLING_CALL_P (insn)"
  "@
   j\t%0
   tail\t#%0@pc"
  [(set_attr "length" "4,8")
   (set_attr "type" "jump,jump")])

(define_expand "sibcall_value"
  [(parallel [(set (match_operand 0 "")
		   (call (match_operand 1 "")
			 (match_operand 2 "")))
	      (use (match_operand 3 ""))])]		;; next_arg_reg
  ""
{
  rtx target = mrisc32_legitimize_call_address (XEXP (operands[1], 0));
  emit_call_insn (gen_sibcall_value_internal (operands[0], target, operands[2]));
  DONE;
})

(define_insn "sibcall_value_internal"
  [(set (match_operand 0 "" "")
	(call (mem:SI (match_operand 1 "mrisc32_call_insn_operand" "c,s"))
	      (match_operand 2 "" "")))]
  "SIBLING_CALL_P (insn)"
  "@
   j\t%1
   tail\t#%1@pc"
  [(set_attr "length" "4,8")
   (set_attr "type" "jump,jump")])

(define_expand "call"
  [(parallel [(call (match_operand 0 "")
		    (match_operand 1 ""))
	      (use (match_operand 2 ""))	;; next_arg_reg
	      (use (match_operand 3 ""))])]	;; struct_value_size_rtx
  ""
{
  rtx target = mrisc32_legitimize_call_address (XEXP (operands[0], 0));
  emit_call_insn (gen_call_internal (target, operands[1]));
  DONE;
})

(define_insn "call_internal"
  [(call (mem:SI (match_operand 0 "mrisc32_call_insn_operand" "c,s"))
	 (match_operand 1 "" ""))
   (clobber (reg:SI MRISC32_LR))]
  ""
  "@
   jl\t%0
   call\t#%0@pc"
  [(set_attr "length" "4,8")
   (set_attr "type" "call,call")])

(define_expand "call_value"
  [(parallel [(set (match_operand 0 "")
		   (call (match_operand 1 "")
			 (match_operand 2 "")))
	      (use (match_operand 3 ""))])]		;; next_arg_reg
  ""
{
  rtx target = mrisc32_legitimize_call_address (XEXP (operands[1], 0));
  emit_call_insn (gen_call_value_internal (operands[0], target, operands[2]));
  DONE;
})

(define_insn "call_value_internal"
  [(set (match_operand 0 "" "")
	(call (mem:SI (match_operand 1 "mrisc32_call_insn_operand" "c,s"))
	      (match_operand 2 "" "")))
   (clobber (reg:SI MRISC32_LR))]
  ""
  "@
   jl\t%1
   call\t#%1@pc"
  [(set_attr "length" "4,8")
   (set_attr "type" "call,call")])

(define_insn "jump"
  [(set (pc)
	(label_ref (match_operand 0 "" "")))]
  ""
  ;; TODO(m): Check that the offset fits in 21 bits.
  "b\t#%l0"
  [(set_attr "type" "branch")])

(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "register_operand" "r"))]
  ""
  "j\t%0"
  [(set_attr "type" "jump")])

(define_expand "simple_return"
  [(parallel [(simple_return) (use (match_dup 0))])]
  ""
{
  operands[0] = gen_rtx_REG (Pmode, MRISC32_LR);
})

(define_insn "*simple_return"
  [(simple_return)
   (use (match_operand:SI 0 "register_operand" "r"))]
  ""
  "ret"
  [(set_attr "type" "ret")])


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

(define_expand "sibcall_epilogue"
  [(const_int 2)]
  ""
{
  mrisc32_expand_epilogue ();
  DONE;
})

;; -------------------------------------------------------------------------
;; Tuning
;; -------------------------------------------------------------------------

(include "mrisc32-generic.md")

