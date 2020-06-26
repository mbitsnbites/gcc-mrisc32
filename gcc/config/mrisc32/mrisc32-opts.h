/* Copyright (C) 2022 Free Software Foundation, Inc.
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

/* Definitions for option handling for MRISC32.  */

#ifndef GCC_MRISC32_OPTS_H
#define GCC_MRISC32_OPTS_H

/* The code model defines the address generation strategy.  */
enum mrisc32_cmodel {
  /* Static code and data fit within a 4MB region.  */
  MRISC32_CMODEL_SMALL,
  /* No assumptions about addresses of code and data.  */
  MRISC32_CMODEL_LARGE
};
//extern enum mrisc32_cmodel mrisc32_current_cmodel;

/* Code model macro.  */
#undef TARGET_CMODEL
#define TARGET_CMODEL mrisc32_current_cmodel

#endif
