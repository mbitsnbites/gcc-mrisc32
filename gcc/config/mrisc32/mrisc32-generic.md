;; Generic pipeline description for MRISC32 targets
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

;; This is meant to describe a fairly generic, scalar pipeline, and is
;; modelled after the MRISC32-A1 pipeline.

(define_automaton "generic")
(define_cpu_unit "gen_issue" "generic")
(define_cpu_unit "gen_branch" "generic")
(define_cpu_unit "gen_alu" "generic")
(define_cpu_unit "gen_mem" "generic")
(define_cpu_unit "gen_mul" "generic")
(define_cpu_unit "gen_div,gen_div2,gen_div3" "generic")
(define_cpu_unit "gen_fpu" "generic")
(define_cpu_unit "gen_sau" "generic")
(define_cpu_unit "gen_ctrl" "generic")

(define_insn_reservation "generic_alu" 1
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "unknown,const,nop,arith,logical,shift,shuf,crc,cmp,move"))
  "gen_issue,gen_alu")

(define_insn_reservation "generic_load" 2
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "load"))
  "gen_issue,gen_alu,gen_mem")

(define_insn_reservation "generic_store" 1
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "store"))
  "gen_issue,gen_alu,gen_mem")

(define_insn_reservation "generic_branch" 1
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "branch,jump,call,ret"))
  "gen_issue,gen_branch")

(define_insn_reservation "generic_imul" 3
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "imul"))
  "gen_issue,gen_mul")

(define_insn_reservation "generic_idivsi" 3
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "idiv"))
  "gen_issue,gen_div,gen_div2*14")

(define_insn_reservation "generic_fdiv" 4
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "fdiv"))
  "gen_issue,gen_div,gen_div2*11,gen_div3")

(define_insn_reservation "generic_faddmul" 4
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "fadd,fmul"))
  "gen_issue,gen_fpu")

(define_insn_reservation "generic_fconv" 4
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "ftoi,itof"))
  "gen_issue,gen_fpu")

(define_insn_reservation "generic_fcmp" 1
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "fcmp,fminmax"))
  "gen_issue,gen_fpu")

(define_insn_reservation "generic_sau" 2
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "saturating"))
  "gen_issue,gen_sau")

(define_insn_reservation "generic_ctrl" 1
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "xchgsr,sync,cctrl"))
  "gen_issue,gen_ctrl")


;; These represent special instruction combinations, where one insn is
;; implemented as two or more assembler instructions.

(define_insn_reservation "generic_alu_alu" 2
  (and (eq_attr "tune" "generic")
       (eq_attr "type" "alu_alu"))
  "gen_issue,gen_alu*2")

