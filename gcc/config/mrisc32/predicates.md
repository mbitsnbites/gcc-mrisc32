;; Predicate definitions for MRISC32
;; Copyright (C) 2009-2020 Free Software Foundation, Inc.
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
;; Predicates
;; -------------------------------------------------------------------------

;; Constant operands.

(define_predicate "mrisc32_const_int_zero_operand"
  (and (match_code "const_int")
       (match_test "INTVAL (op) == 0")))

(define_predicate "mrisc32_const_int_minusone_operand"
  (and (match_code "const_int")
       (match_test "INTVAL (op) == -1")))

(define_predicate "mrisc32_const_8_or_16_operand"
  (and (match_code "const_int")
       (ior (match_test "INTVAL (op) == 8")
            (match_test "INTVAL (op) == 16"))))

(define_predicate "mrisc32_byte_bitpos_operand"
  (and (match_code "const_int")
       (ior (match_test "INTVAL (op) == 0")
	    (match_test "INTVAL (op) == 8")
            (match_test "INTVAL (op) == 16")
            (match_test "INTVAL (op) == 24"))))

;; A valid third operand for type A and C instructions.

(define_predicate "mrisc32_reg_or_imm14_operand"
  (ior (match_code "reg,subreg")
       (and (match_code "const_int")
	    (match_test "mrisc32_is_imm14 (INTVAL (op))"))))

;; A valid register operand, including the zero register.

(define_predicate "mrisc32_reg_or_zero_operand"
  (ior (match_code "reg,subreg")
       (and (match_code "const_int")
	    (match_test "INTVAL (op) == 0"))
       (and (match_code "const_double")
	    (match_test "real_equal (CONST_DOUBLE_REAL_VALUE (op), &dconst0)"))))

(define_predicate "mrisc32_reg_or_int_zero_operand"
  (ior (match_code "reg,subreg")
       (and (match_code "const_int")
	    (match_test "INTVAL (op) == 0"))))

(define_predicate "mrisc32_reg_or_dbl_zero_operand"
  (ior (match_code "reg,subreg")
       (and (match_code "const_double")
	    (match_test "real_equal (CONST_DOUBLE_REAL_VALUE (op), &dconst0)"))))

;; A valid source operand for an integer or floating-point move operation.

(define_predicate "mrisc32_movsrc_operand"
  (match_code "mem,const_int,const_double,reg,subreg,symbol_ref,label_ref,const")
{
  if (mrisc32_valid_memsrc_operand (op))
    return 1;

  return general_operand (op, mode);
})

;; A valid shift amount for scaled indexed addressing (e.g. LDEA).

(define_predicate "mrisc32_const_index_shift"
  (ior (match_code "reg,subreg")
       (and (match_code "const_int")
	    (match_test "IN_RANGE (INTVAL (op), 1, 3)"))))

;; A valid constant immediate byte mask.

(define_predicate "mrisc32_byte_mask_for_shuf"
  (match_code "const_int")
{
  return mrisc32_is_mask_suitable_for_shuf (op);
})

;; An (in)equality comparison operator.

(define_predicate "mrisc32_equality_comparison_operator"
  (match_code "eq,ne"))

;; A valid comparison operator for set (S[cc]).
(define_predicate "mrisc32_set_comparison_operator"
  (match_code "eq,ne,lt,le,ltu,leu"))

;; A valid reverse comparison operator for set (S[cc]).
(define_predicate "mrisc32_set_rev_comparison_operator"
  (match_code "gt,ge,gtu,geu"))

;; A valid operand for call instructions.

(define_predicate "mrisc32_call_insn_operand"
  (ior (match_code "symbol_ref")
       (match_operand 0 "register_operand")))

