/* Prototypes for mrisc32.c functions used in the md file & elsewhere.
   Copyright (C) 2019-2020 Free Software Foundation, Inc.
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

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

extern bool mrisc32_is_imm14 (const int ival);
extern const char *mrisc32_emit_load_immediate (rtx, rtx);
extern void mrisc32_expand_conditional_branch (rtx *, machine_mode);
extern void mrisc32_expand_scc (rtx *);
extern void mrisc32_expand_prologue (void);
extern void mrisc32_expand_epilogue (void);
extern int mrisc32_initial_elimination_offset (int, int);
extern bool mrisc32_register_address_p (rtx, bool);
extern bool mrisc32_offset_address_p (rtx, bool);
extern bool mrisc32_indexed_address_p (rtx, bool);
extern bool mrisc32_valid_memsrc_operand (rtx);
extern bool mrisc32_is_mask_suitable_for_shuf (rtx);
extern bool mrisc32_is_mask_suitable_for_mkbf (rtx, rtx);
extern rtx mrisc32_legitimize_call_address (rtx);
extern bool mrisc32_legitimize_move (machine_mode, rtx, rtx);
extern rtx mrisc32_mask_to_shuf_ctrl (rtx);
extern rtx mrisc32_mask_to_mkbf_ctrl (rtx, rtx);
extern rtx mrisc32_subreg_di_low (rtx);
extern rtx mrisc32_subreg_di_high (rtx);

