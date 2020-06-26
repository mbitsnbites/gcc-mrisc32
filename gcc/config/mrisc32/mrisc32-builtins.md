;; Machine description for MRISC32
;; Copyright (C) 2019-2023 Free Software Foundation, Inc.
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
;; MRISC32 specific instructions used for builtins
;; -------------------------------------------------------------------------

(define_c_enum "unspec" [
  UNSPEC_ADD_B          ; add.b
  UNSPEC_ADD_H          ; add.h
  UNSPEC_SUB_B          ; sub.b
  UNSPEC_SUB_H          ; sub.h
  UNSPEC_SEQ_B          ; seq.b
  UNSPEC_SEQ_H          ; seq.h
  UNSPEC_SNE_B          ; sne.b
  UNSPEC_SNE_H          ; sne.h
  UNSPEC_SLT_B          ; slt.b
  UNSPEC_SLT_H          ; slt.h
  UNSPEC_SLTU_B         ; sltu.b
  UNSPEC_SLTU_H         ; sltu.h
  UNSPEC_SLE_B          ; sle.b
  UNSPEC_SLE_H          ; sle.h
  UNSPEC_SLEU_B         ; sleu.b
  UNSPEC_SLEU_H         ; sleu.h
  UNSPEC_REV            ; rev
  UNSPEC_CRC32C_8       ; crc32c.8
  UNSPEC_CRC32C_16      ; crc32c.16
  UNSPEC_CRC32C_32      ; crc32c.32
  UNSPEC_CRC32_8        ; crc32.8
  UNSPEC_CRC32_16       ; crc32.16
  UNSPEC_CRC32_32       ; crc32.32
  UNSPEC_XCHGSR         ; xchgsr
  UNSPEC_GETSR          ; getsr
  UNSPEC_SETSR          ; setsr
  UNSPEC_WAIT           ; wait
  UNSPEC_SYNC           ; sync
  UNSPEC_CCTRL          ; cctrl
])


;; -------------------------------------------------------------------------
;; Packed integer arithmentic.
;; TODO(m): Should these be on vector form (e.g. V4QI instead of SI)?
;; -------------------------------------------------------------------------

(define_insn "mrisc32_add_b_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(unspec:SI [(match_operand:SI 1 "register_operand" "r")
		    (match_operand:SI 2 "register_operand" "r")]
		   UNSPEC_ADD_B))]
  "TARGET_PACKED_OPS"
  "add.b\t%0, %1, %2"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_add_h_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(unspec:SI [(match_operand:SI 1 "register_operand" "r")
		    (match_operand:SI 2 "register_operand" "r")]
		   UNSPEC_ADD_H))]
  "TARGET_PACKED_OPS"
  "add.h\t%0, %1, %2"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_sub_b_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(unspec:SI [(match_operand:SI 1 "register_operand" "r")
		    (match_operand:SI 2 "register_operand" "r")]
		   UNSPEC_SUB_B))]
  "TARGET_PACKED_OPS"
  "sub.b\t%0, %1, %2"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_sub_h_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(unspec:SI [(match_operand:SI 1 "register_operand" "r")
		    (match_operand:SI 2 "register_operand" "r")]
		   UNSPEC_SUB_H))]
  "TARGET_PACKED_OPS"
  "sub.h\t%0, %1, %2"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_seq_b_si3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_zero_operand" "r,Z,r")
		    (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,r,Z")]
		   UNSPEC_SEQ_B))]
  "TARGET_PACKED_OPS"
  "@
   seq.b\t%0, %1, %2
   seq.b\t%0, z, %2
   seq.b\t%0, %1, z"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_seq_h_si3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_zero_operand" "r,Z,r")
		    (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,r,Z")]
		   UNSPEC_SEQ_H))]
  "TARGET_PACKED_OPS"
  "@
   seq.h\t%0, %1, %2
   seq.h\t%0, z, %2
   seq.h\t%0, %1, z"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_sne_b_si3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_zero_operand" "r,Z,r")
		    (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,r,Z")]
		   UNSPEC_SNE_B))]
  "TARGET_PACKED_OPS"
  "@
   sne.b\t%0, %1, %2
   sne.b\t%0, z, %2
   sne.b\t%0, %1, z"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_sne_h_si3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_zero_operand" "r,Z,r")
		    (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,r,Z")]
		   UNSPEC_SNE_H))]
  "TARGET_PACKED_OPS"
  "@
   sne.h\t%0, %1, %2
   sne.h\t%0, z, %2
   sne.h\t%0, %1, z"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_slt_b_si3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_zero_operand" "r,Z,r")
		    (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,r,Z")]
		   UNSPEC_SLT_B))]
  "TARGET_PACKED_OPS"
  "@
   slt.b\t%0, %1, %2
   slt.b\t%0, z, %2
   slt.b\t%0, %1, z"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_slt_h_si3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_zero_operand" "r,Z,r")
		    (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,r,Z")]
		   UNSPEC_SLT_H))]
  "TARGET_PACKED_OPS"
  "@
   slt.h\t%0, %1, %2
   slt.h\t%0, z, %2
   slt.h\t%0, %1, z"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_sltu_b_si3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_zero_operand" "r,Z,r")
		    (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,r,Z")]
		   UNSPEC_SLTU_B))]
  "TARGET_PACKED_OPS"
  "@
   sltu.b\t%0, %1, %2
   sltu.b\t%0, z, %2
   sltu.b\t%0, %1, z"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_sltu_h_si3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_zero_operand" "r,Z,r")
		    (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,r,Z")]
		   UNSPEC_SLTU_H))]
  "TARGET_PACKED_OPS"
  "@
   sltu.h\t%0, %1, %2
   sltu.h\t%0, z, %2
   sltu.h\t%0, %1, z"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_sle_b_si3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_zero_operand" "r,Z,r")
		    (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,r,Z")]
		   UNSPEC_SLE_B))]
  "TARGET_PACKED_OPS"
  "@
   sle.b\t%0, %1, %2
   sle.b\t%0, z, %2
   sle.b\t%0, %1, z"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_sle_h_si3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_zero_operand" "r,Z,r")
		    (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,r,Z")]
		   UNSPEC_SLE_H))]
  "TARGET_PACKED_OPS"
  "@
   sle.h\t%0, %1, %2
   sle.h\t%0, z, %2
   sle.h\t%0, %1, z"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_sleu_b_si3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_zero_operand" "r,Z,r")
		    (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,r,Z")]
		   UNSPEC_SLEU_B))]
  "TARGET_PACKED_OPS"
  "@
   sleu.b\t%0, %1, %2
   sleu.b\t%0, z, %2
   sleu.b\t%0, %1, z"
  [(set_attr "type" "arith")])

(define_insn "mrisc32_sleu_h_si3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_zero_operand" "r,Z,r")
		    (match_operand:SI 2 "mrisc32_reg_or_zero_operand" "r,r,Z")]
		   UNSPEC_SLEU_H))]
  "TARGET_PACKED_OPS"
  "@
   sleu.h\t%0, %1, %2
   sleu.h\t%0, z, %2
   sleu.h\t%0, %1, z"
  [(set_attr "type" "arith")])


;; -------------------------------------------------------------------------
;; Saturating arithmentic.
;; -------------------------------------------------------------------------

(define_insn "ssaddsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ss_plus:SI (match_operand:SI 1 "register_operand" "r")
		    (match_operand:SI 2 "register_operand" "r")))]
  "TARGET_SATURATING_OPS"
  "adds\t%0, %1, %2"
  [(set_attr "type" "saturating")])

(define_insn "usaddsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(us_plus:SI (match_operand:SI 1 "register_operand" "r")
		    (match_operand:SI 2 "register_operand" "r")))]
  "TARGET_SATURATING_OPS"
  "addsu\t%0, %1, %2"
  [(set_attr "type" "saturating")])

(define_insn "sssubsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(ss_minus:SI (match_operand:SI 1 "register_operand" "r")
		    (match_operand:SI 2 "register_operand" "r")))]
  "TARGET_SATURATING_OPS"
  "subs\t%0, %1, %2"
  [(set_attr "type" "saturating")])

(define_insn "ussubsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(us_minus:SI (match_operand:SI 1 "register_operand" "r")
		    (match_operand:SI 2 "register_operand" "r")))]
  "TARGET_SATURATING_OPS"
  "subsu\t%0, %1, %2"
  [(set_attr "type" "saturating")])


;; -------------------------------------------------------------------------
;; Binary manipulation.
;; -------------------------------------------------------------------------

(define_insn "mrisc32_revsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(unspec:SI [(match_operand:SI 1 "register_operand" "r")]
		   UNSPEC_REV))]
  ""
  "rev\t%0, %1"
  [(set_attr "type" "logical")])

(define_insn "mrisc32_crc32c_8_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "0")
                    (match_operand:SI 2 "register_operand" "r")]
		   UNSPEC_CRC32C_8))]
  ""
  "crc32c.8\t%0, %2"
  [(set_attr "type" "crc")])

(define_insn "mrisc32_crc32c_16_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "0")
                    (match_operand:SI 2 "register_operand" "r")]
		   UNSPEC_CRC32C_16))]
  ""
  "crc32c.16\t%0, %2"
  [(set_attr "type" "crc")])

(define_insn "mrisc32_crc32c_32_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "0")
                    (match_operand:SI 2 "register_operand" "r")]
		   UNSPEC_CRC32C_32))]
  ""
  "crc32c.32\t%0, %2"
  [(set_attr "type" "crc")])

(define_insn "mrisc32_crc32_8_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "0")
                    (match_operand:SI 2 "register_operand" "r")]
		   UNSPEC_CRC32_8))]
  ""
  "crc32.8\t%0, %2"
  [(set_attr "type" "crc")])

(define_insn "mrisc32_crc32_16_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "0")
                    (match_operand:SI 2 "register_operand" "r")]
		   UNSPEC_CRC32_16))]
  ""
  "crc32.16\t%0, %2"
  [(set_attr "type" "crc")])

(define_insn "mrisc32_crc32_32_si3"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "0")
                    (match_operand:SI 2 "register_operand" "r")]
		   UNSPEC_CRC32_32))]
  ""
  "crc32.32\t%0, %2"
  [(set_attr "type" "crc")])


;; -------------------------------------------------------------------------
;; Control instructions.
;; -------------------------------------------------------------------------

(define_insn "mrisc32_xchgsrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(unspec_volatile:SI [(match_operand:SI 1 "register_operand" "r,r")
			     (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")]
			    UNSPEC_XCHGSR))]
  ""
  "@
  xchgsr\t%0, %1, %2
  xchgsr\t%0, %1, #%2"
  [(set_attr "type" "xchgsr")])

(define_insn "mrisc32_getsrsi2"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(unspec_volatile:SI [(match_operand:SI 1 "mrisc32_reg_or_i15hl_operand" "r,I")]
			    UNSPEC_GETSR))]
  ""
  "@
  getsr\t%0, %1
  getsr\t%0, #%1"
  [(set_attr "type" "xchgsr")])

(define_insn "mrisc32_setsrsi2"
  [(unspec_volatile [(match_operand:SI 0 "register_operand" "r,r")
		     (match_operand:SI 1 "mrisc32_reg_or_i15hl_operand" "r,I")]
		    UNSPEC_SETSR)]
  ""
  "@
  setsr\t%0, %1
  setsr\t%0, #%1"
  [(set_attr "type" "xchgsr")])

(define_insn "mrisc32_wait"
  [(unspec_volatile:BLK [(const_int 0)] UNSPEC_WAIT)]
  ""
  "wait"
  [(set_attr "type" "wait")])

(define_insn "mrisc32_sync"
  [(unspec_volatile:BLK [(const_int 0)] UNSPEC_SYNC)
   (clobber (mem:BLK (const_int 0)))]
  ""
  "sync"
  [(set_attr "type" "sync")])

(define_insn "mrisc32_cctrlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(unspec_volatile:SI [(match_operand:SI 1 "register_operand" "0")
			     (match_operand:SI 2 "register_operand" "r")]
			    UNSPEC_CCTRL))
   (clobber (mem:BLK (const_int 0)))]
  ""
  "cctrl\t%0, %2"
  [(set_attr "type" "cctrl")])

