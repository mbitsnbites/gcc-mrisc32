;; Constraint definitions for MRISC32
;; Copyright (C) 2019-2020 Free Software Foundation, Inc.
;; Contributed by Marcus Geelnard

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
;; Constant constraints.
;; -------------------------------------------------------------------------

(define_constraint "O"
  "Integer zero."
  (and (match_code "const_int")
       (match_test "ival == 0")))

(define_constraint "M"
  "Integer -1."
  (and (match_code "const_int")
       (match_test "ival == -1")))

(define_constraint "I"
  "A 15-bit signed constant value (-16384..16383)"
  (and (match_code "const_int")
       (match_test "ival >= -16384 && ival <= 16383")))

(define_constraint "J"
  "A 21-bit signed constant value (-1048576..1048575)"
  (and (match_code "const_int")
       (match_test "ival >= -1048576 && ival <= 1048575")))

(define_constraint "Z"
  "Any zero constant (integer or floating-point)."
  (ior
    (and (match_code "const_int")
	 (match_test "ival == 0"))
    (and (match_code "const_double")
	 (match_test "real_equal (rval, &dconst0)"))))


;; -------------------------------------------------------------------------
;; Register constraints.
;; -------------------------------------------------------------------------

;; TODO(m): This should probably not be SCALAR_REGS...
(define_register_constraint "c" "SCALAR_REGS"
  "Registers which can hold a sibling call address")


;; -------------------------------------------------------------------------
;; Memory address constraints.
;; -------------------------------------------------------------------------

(define_constraint "A"
  "An absolute address."
  (and (match_code "mem")
       (ior (match_test "GET_CODE (XEXP (op, 0)) == SYMBOL_REF")
	    (match_test "GET_CODE (XEXP (op, 0)) == LABEL_REF")
	    (match_test "GET_CODE (XEXP (op, 0)) == CONST"))))

(define_constraint "B"
  "An offset address."
  (and (match_code "mem")
       (match_test "mrisc32_offset_address_p (op, true)")))

(define_constraint "C"
  "An indexed address."
  (and (match_code "mem")
       (match_test "mrisc32_indexed_address_p (op, true)")))

(define_constraint "W"
  "A register indirect memory operand."
  (and (match_code "mem")
       (match_test "REG_P (XEXP (op, 0))
		    && REG_OK_FOR_BASE_P (XEXP (op, 0))")))

