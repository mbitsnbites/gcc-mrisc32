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
  UNSPEC_SYNC           ; sync
  UNSPEC_CCTRL          ; cctrl
])


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
	(unspec:SI [(match_operand:SI 1 "register_operand" "r,r")
		    (match_operand:SI 2 "mrisc32_reg_or_i15hl_operand" "r,I")]
		   UNSPEC_XCHGSR))
   (clobber (mem:BLK (const_int 0)))]
  ""
  "@
  xchgsr\t%0, %1, %2
  xchgsr\t%0, %1, #%2"
  [(set_attr "type" "xchgsr")])

(define_insn "mrisc32_getsrsi2"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	(unspec:SI [(match_operand:SI 1 "mrisc32_reg_or_i15hl_operand" "r,I")]
		   UNSPEC_GETSR))
   (clobber (mem:BLK (const_int 0)))]
  ""
  "@
  getsr\t%0, %1
  getsr\t%0, #%1"
  [(set_attr "type" "xchgsr")])

(define_insn "mrisc32_setsrsi2"
  [(unspec_volatile [(match_operand:SI 0 "register_operand" "r,r")
		     (match_operand:SI 1 "mrisc32_reg_or_i15hl_operand" "r,I")]
		    UNSPEC_SETSR)
   (clobber (mem:BLK (const_int 0)))]
  ""
  "@
  setsr\t%0, %1
  setsr\t%0, #%1"
  [(set_attr "type" "xchgsr")])

(define_insn "mrisc32_sync"
  [(unspec_volatile:BLK [(const_int 0)] UNSPEC_SYNC)
   (clobber (mem:BLK (const_int 0)))]
  ""
  "sync"
  [(set_attr "type" "sync")])

(define_insn "mrisc32_cctrlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (unspec:SI [(match_operand:SI 1 "register_operand" "0")
                    (match_operand:SI 2 "register_operand" "r")]
		   UNSPEC_CCTRL))
   (clobber (mem:BLK (const_int 0)))]
  ""
  "cctrl\t%0, %2"
  [(set_attr "type" "cctrl")])

