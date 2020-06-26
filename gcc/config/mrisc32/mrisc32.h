/* Target Definitions for MRISC32.
   Copyright (C) 2015-2020 Free Software Foundation, Inc.
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

#ifndef GCC_MRISC32_H
#define GCC_MRISC32_H

#undef  STARTFILE_SPEC
#define STARTFILE_SPEC "%{!mno-crt0:crt0%O%s}"

#undef  LIB_SPEC
#define LIB_SPEC "--start-group			\
		   -lc					\
		   %{msim:-lmr32sim}			\
		   %{!msim:-lmr32default}		\
		   --end-group"

#define TARGET_DEFAULT (MASK_HARD_FLOAT	\
			| MASK_DIV		\
			| MASK_PACKED_OPS	\
			| MASK_SATURATING_OPS	\
			| MASK_VECTOR_OPS)

/* Layout of Source Language Data Types */

#define INT_TYPE_SIZE 32
#define SHORT_TYPE_SIZE 16
#define LONG_TYPE_SIZE 32
#define LONG_LONG_TYPE_SIZE 64

#define FLOAT_TYPE_SIZE 32
#define DOUBLE_TYPE_SIZE 64
#define LONG_DOUBLE_TYPE_SIZE 64

#define DEFAULT_SIGNED_CHAR 0

#undef  SIZE_TYPE
#define SIZE_TYPE "unsigned int"

#undef  PTRDIFF_TYPE
#define PTRDIFF_TYPE "int"

#undef  WCHAR_TYPE
#define WCHAR_TYPE "unsigned int"

#undef  WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE BITS_PER_WORD

#define REGISTER_NAMES {                                   \
  "z", "s1", "s2", "s3", "s4", "s5", "s6", "s7",           \
  "s8", "s9", "s10", "s11", "s12", "s13", "s14", "s15",    \
  "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23",  \
  "s24", "s25", "fp", "tp", "sp", "vl", "lr", "pc",        \
  "vz", "v1", "v2", "v3", "v4", "v5", "v6", "v7",          \
  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15",    \
  "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23",  \
  "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31",  \
  "?fp", "?ap"                                             \
}

#define MRISC32_Z      0
#define MRISC32_S1     1
#define MRISC32_S2     2
#define MRISC32_S3     3
#define MRISC32_S4     4
#define MRISC32_S5     5
#define MRISC32_S6     6
#define MRISC32_S7     7
#define MRISC32_S8     8
#define MRISC32_S9     9
#define MRISC32_S10    10
#define MRISC32_S11    11
#define MRISC32_S12    12
#define MRISC32_S13    13
#define MRISC32_S14    14
#define MRISC32_S15    15
#define MRISC32_S16    16
#define MRISC32_S17    17
#define MRISC32_S18    18
#define MRISC32_S19    19
#define MRISC32_S20    20
#define MRISC32_S21    21
#define MRISC32_S22    22
#define MRISC32_S23    23
#define MRISC32_S24    24
#define MRISC32_S25    25
#define MRISC32_FP     26
#define MRISC32_TP     27
#define MRISC32_SP     28
#define MRISC32_VL     29
#define MRISC32_LR     30
#define MRISC32_PC     31
#define MRISC32_VZ     32
#define MRISC32_V1     33
#define MRISC32_V2     34
#define MRISC32_V3     35
#define MRISC32_V4     36
#define MRISC32_V5     37
#define MRISC32_V6     38
#define MRISC32_V7     39
#define MRISC32_V8     40
#define MRISC32_V9     41
#define MRISC32_V10    42
#define MRISC32_V11    43
#define MRISC32_V12    44
#define MRISC32_V13    45
#define MRISC32_V14    46
#define MRISC32_V15    47
#define MRISC32_V16    48
#define MRISC32_V17    49
#define MRISC32_V18    50
#define MRISC32_V19    51
#define MRISC32_V20    52
#define MRISC32_V21    53
#define MRISC32_V22    54
#define MRISC32_V23    55
#define MRISC32_V24    56
#define MRISC32_V25    57
#define MRISC32_V26    58
#define MRISC32_V27    59
#define MRISC32_V28    60
#define MRISC32_V29    61
#define MRISC32_V30    62
#define MRISC32_V31    63
#define MRISC32_QFP    64
#define MRISC32_QAP    65
#define FIRST_PSEUDO_REGISTER (66)

enum reg_class
{
  NO_REGS,
  SCALAR_REGS,
  VECTOR_REGS,
  GENERAL_REGS,
  SPECIAL_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};

#define REG_CLASS_CONTENTS {                                  \
  { 0x00000000, 0x00000000, 0x00000000 }, /* NO_REGS */       \
  { 0xFFFFFFFF, 0x00000000, 0x00000003 }, /* SCALAR_REGS */   \
  { 0x00000000, 0xFFFFFFFF, 0x00000000 }, /* VECTOR_REGS */   \
  { 0x7FFFFFFE, 0x00000000, 0x00000003 }, /* GENERAL_REGS */  \
  { 0x80000001, 0x00000000, 0x00000000 }, /* SPECIAL_REGS */  \
  { 0xFFFFFFFF, 0xFFFFFFFF, 0x00000003 }  /* ALL_REGS */      \
}

#define N_REG_CLASSES LIM_REG_CLASSES

#define REG_CLASS_NAMES { \
  "NO_REGS",              \
  "SCALAR_REGS",          \
  "VECTOR_REGS",          \
  "GENERAL_REGS",         \
  "SPECIAL_REGS",         \
  "ALL_REGS"              \
}

/* We allocate s15 as a dedicated scratch register for operations
   that are split into two instructions, such as addpchi+stw.  */
/* TODO(m): v1-v31 are not fixed, but we mark them as such just to keep
   the register allocator away from them.  */

#define FIXED_REGISTERS {                                         \
  /*   z  s1  s2  s3  s4  s5  s6  s7 */   1, 0, 0, 0, 0, 0, 0, 0, \
  /*  s8  s9 s10 s11 s12 s13 s14 s15 */   0, 0, 0, 0, 0, 0, 0, 1, \
  /* s16 s17 s18 s19 s20 s21 s22 s23 */   0, 0, 0, 0, 0, 0, 0, 0, \
  /* s24 s25  fp  tp  sp  vl  lr  pc */   0, 0, 1, 1, 1, 1, 0, 1, \
  /*  vz  v1  v2  v3  v4  v5  v6  v7 */   1, 1, 1, 1, 1, 1, 1, 1, \
  /*  v8  v9 v10 v11 v12 v13 v14 v15 */   1, 1, 1, 1, 1, 1, 1, 1, \
  /* v16 v17 v18 v19 v20 v21 v22 v23 */   1, 1, 1, 1, 1, 1, 1, 1, \
  /* v24 v25 v26 v27 v28 v29 v30 v31 */   1, 1, 1, 1, 1, 1, 1, 1, \
  /* ?fp ?ap                         */   1, 1                    \
}

/* TODO(m): v1-v15 are also scratch registers.  */

#define CALL_REALLY_USED_REGISTERS {                              \
  /*   z  s1  s2  s3  s4  s5  s6  s7 */   0, 1, 1, 1, 1, 1, 1, 1, \
  /*  s8  s9 s10 s11 s12 s13 s14 s15 */   1, 1, 1, 1, 1, 1, 1, 0, \
  /* s16 s17 s18 s19 s20 s21 s22 s23 */   0, 0, 0, 0, 0, 0, 0, 0, \
  /* s24 s25  fp  tp  sp  vl  lr  pc */   0, 0, 0, 0, 0, 0, 0, 0, \
  /*  vz  v1  v2  v3  v4  v5  v6  v7 */   0, 0, 0, 0, 0, 0, 0, 0, \
  /*  v8  v9 v10 v11 v12 v13 v14 v15 */   0, 0, 0, 0, 0, 0, 0, 0, \
  /* v16 v17 v18 v19 v20 v21 v22 v23 */   0, 0, 0, 0, 0, 0, 0, 0, \
  /* v24 v25 v26 v27 v28 v29 v30 v31 */   0, 0, 0, 0, 0, 0, 0, 0, \
  /* ?fp ?ap                         */   0, 0                    \
}

/* Registers used as temporaries in prologue/epilogue code. */

#define MRISC32_PROLOGUE_TEMP_REGNUM (MRISC32_S15)
#define MRISC32_PROLOGUE_TEMP(MODE) gen_rtx_REG (MODE, MRISC32_PROLOGUE_TEMP_REGNUM)

/* We can't copy to or from our CC register. */
#define AVOID_CCMODE_COPIES 1

/* A C expression whose value is a register class containing hard
   register REGNO.  */
extern const enum reg_class mrisc32_regno_to_class[];
#define REGNO_REG_CLASS(REGNO) mrisc32_regno_to_class[(REGNO)]

/* The Overall Framework of an Assembler File */

#undef  ASM_SPEC
#define ASM_COMMENT_START ";"
#define ASM_APP_ON ""
#define ASM_APP_OFF ""

#define FILE_ASM_OP     "\t.file\n"

/* Switch to the text or data segment.  */
#define TEXT_SECTION_ASM_OP  "\t.text"
#define DATA_SECTION_ASM_OP  "\t.data"

/* Assembler Commands for Alignment */

#define ASM_OUTPUT_ALIGN(STREAM,POWER) \
	fprintf (STREAM, "\t.p2align\t%d\n", POWER);

/* Output and Generation of Labels */

#define GLOBAL_ASM_OP "\t.global\t"

/* This is how to output an element of a case-vector that is absolute.  */

#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)  \
    fprintf (FILE, "\t.long\t.L%d\n", VALUE)				\

/* Passing Arguments in Registers */

/* A C type for declaring a variable that is used as the first
   argument of `FUNCTION_ARG' and other related values.  */
#define CUMULATIVE_ARGS unsigned int

/* If defined, the maximum amount of space required for outgoing arguments
   will be computed and placed into the variable
   `current_function_outgoing_args_size'.  No space will be pushed
   onto the stack for each call; instead, the function prologue should
   increase the stack frame size by this amount.  */
#define ACCUMULATE_OUTGOING_ARGS 1

/* A C statement (sans semicolon) for initializing the variable CUM
   for the state at the beginning of the argument list.
   For MRISC32, the first arg is passed in register 1 (aka S1).  */
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,FNDECL,N_NAMED_ARGS) \
  (CUM = MRISC32_S1)

/* How Scalar Function Values Are Returned */

/* STACK AND CALLING */

/* Define this macro if pushing a word onto the stack moves the stack
   pointer to a smaller address.  */
#define STACK_GROWS_DOWNWARD 1

/* Offset from the argument pointer register to the first argument's
   address.  On some machines it may depend on the data type of the
   function.  */
#define FIRST_PARM_OFFSET(F) 0

/* Define this macro to nonzero value if the addresses of local variable slots
   are at negative offsets from the frame pointer.  */
#define FRAME_GROWS_DOWNWARD 1

/* EXIT_IGNORE_STACK should be nonzero if, when returning from a function,
   the stack pointer does not matter.  The value is tested only in
   functions that have frame pointers.
   No definition is equivalent to always zero.  */

#define EXIT_IGNORE_STACK 0

/* Define this macro as a C expression that is nonzero for registers that are
   used by the epilogue or the return pattern.  The stack and frame
   pointer registers are already assumed to be used as needed.  */
#define EPILOGUE_USES(R) (R == MRISC32_LR)

/* A C expression whose value is RTL representing the location of the
   incoming return address at the beginning of any function, before
   the prologue.  */
#define INCOMING_RETURN_ADDR_RTX gen_rtx_REG (Pmode, MRISC32_LR)

/* Describe how we implement __builtin_eh_return.  */
#define EH_RETURN_DATA_REGNO(N) ((N) < 4 ? (N+2) : INVALID_REGNUM)

/* Store the return handler into the call frame.  */
#define EH_RETURN_HANDLER_RTX                                              \
  gen_frame_mem (Pmode,                                                    \
                 plus_constant (Pmode, frame_pointer_rtx, UNITS_PER_WORD))

/* Storage Layout */

#define BITS_BIG_ENDIAN 0
#define BYTES_BIG_ENDIAN 0
#define WORDS_BIG_ENDIAN 0

/* Alignment required for a function entry point, in bits.  */
#define FUNCTION_BOUNDARY 32

/* Define this macro as a C expression which is nonzero if accessing
   less than a word of memory (i.e. a `char' or a `short') is no
   faster than accessing a word of memory.  */
#define SLOW_BYTE_ACCESS 1

/* Number of storage units in a word; normally the size of a
   general-purpose register, a power of two from 1 or 8.  */
#define UNITS_PER_WORD 4

/* Define this macro to the minimum alignment enforced by hardware
   for the stack pointer on this machine.  The definition is a C
   expression for the desired alignment (measured in bits).  */
#define STACK_BOUNDARY 32

/* Normal alignment required for function parameters on the stack, in
   bits.  All stack parameters receive at least this much alignment
   regardless of data type.  */
#define PARM_BOUNDARY 32

/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY 32

/* No data type wants to be aligned rounder than this.  */
#define BIGGEST_ALIGNMENT 32

/* The best alignment to use in cases where we have a choice.  */
#define FASTEST_ALIGNMENT 32

/* Every structures size must be a multiple of 8 bits.  */
#define STRUCTURE_SIZE_BOUNDARY 8

/* Look at the fundamental type that is used for a bit-field and use 
   that to impose alignment on the enclosing structure.
   struct s {int a:8}; should have same alignment as "int", not "char".  */
#define PCC_BITFIELD_TYPE_MATTERS	1

/* Largest integer machine mode for structures.  If undefined, the default
   is GET_MODE_SIZE(DImode).  */
#define MAX_FIXED_MODE_SIZE 32

/* The MRISC32 s[cc] instructions produce -1 (rather than the default +1).  */
#define STORE_FLAG_VALUE (-1)

/* Make arrays of chars word-aligned for the same reasons.  */
#define DATA_ALIGNMENT(TYPE, ALIGN)		\
  (TREE_CODE (TYPE) == ARRAY_TYPE		\
   && TYPE_MODE (TREE_TYPE (TYPE)) == QImode	\
   && (ALIGN) < FASTEST_ALIGNMENT ? FASTEST_ALIGNMENT : (ALIGN))
     
/* Set this nonzero if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 1

/* Generating Code for Profiling */
#define FUNCTION_PROFILER(FILE,LABELNO) (abort (), 0)

/* Trampolines for Nested Functions.  */
#define TRAMPOLINE_SIZE (2 + 6 + 4 + 2 + 6)

/* Alignment required for trampolines, in bits.  */
#define TRAMPOLINE_ALIGNMENT 32

/* An alias for the machine mode for pointers.  */
#define Pmode         SImode

/* An alias for the machine mode used for memory references to
   functions being called, in `call' RTL expressions.  */
#define FUNCTION_MODE SImode

/* The register number of the stack pointer register, which must also
   be a fixed register according to `FIXED_REGISTERS'.  */
#define STACK_POINTER_REGNUM MRISC32_SP

/* The register number of the frame pointer register, which is used to
   access automatic variables in the stack frame.  */
#define FRAME_POINTER_REGNUM MRISC32_QFP

/* The register number of the arg pointer register, which is used to
   access the function's argument list.  */
#define ARG_POINTER_REGNUM MRISC32_QAP

#define HARD_FRAME_POINTER_REGNUM MRISC32_FP

#define ELIMINABLE_REGS					\
{{ ARG_POINTER_REGNUM,   STACK_POINTER_REGNUM},		\
 { ARG_POINTER_REGNUM,   HARD_FRAME_POINTER_REGNUM},	\
 { FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM},		\
 { FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM}}

/* This macro returns the initial difference between the specified pair
   of registers.  */
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			\
  do {									\
    (OFFSET) = mrisc32_initial_elimination_offset ((FROM), (TO));		\
  } while (0)

/* A C expression that is nonzero if REGNO is the number of a hard
   register in which function arguments are sometimes passed.
   TODO(m): (r >= MRISC32_V1 && r <= MRISC32_V8)  */
#define FUNCTION_ARG_REGNO_P(r) (r >= MRISC32_S1 && r <= MRISC32_S8)

/* A macro whose definition is the name of the class to which a valid
   base register must belong.  A base register is one used in an
   address which is the register value plus a displacement.  */
#define BASE_REG_CLASS SCALAR_REGS

/* A macro whose definition is the name of the class to which a
   valid index register must belong.  An index register is one used
   in an address where its value is either multiplied by a scale
   factor or added to another register (as well as added to a
   displacement).  */
#define INDEX_REG_CLASS SCALAR_REGS

/* Any hard scalar register is OK as an address base register.  */
#define HARD_REGNO_OK_FOR_BASE_P(NUM) ((NUM) <= MRISC32_PC)

/* Any hard scalar register is OK as an address index register.
   TODO(m): We should support vector registers too (gather/scatter).  */
#define HARD_REGNO_OK_FOR_INDEX_P(NUM) ((NUM) <= MRISC32_PC)

/* A C expression which is nonzero if register number NUM is suitable
   for use as a base register in operand addresses.  */
#define REGNO_OK_FOR_BASE_P(NUM)		 		\
  (HARD_REGNO_OK_FOR_BASE_P (NUM)				\
   || HARD_REGNO_OK_FOR_BASE_P ((unsigned) reg_renumber[NUM]))	\
   || (NUM) == FRAME_POINTER_REGNUM				\
   || reg_renumber[NUM] == FRAME_POINTER_REGNUM			\
   || (NUM) == ARG_POINTER_REGNUM				\
   || reg_renumber[NUM] == ARG_POINTER_REGNUM

/* A C expression which is nonzero if register number NUM is suitable
   for use as an index register in operand addresses.  */
#define REGNO_OK_FOR_INDEX_P(NUM)		 		\
  (HARD_REGNO_OK_FOR_INDEX_P (NUM) 		 		\
   || HARD_REGNO_OK_FOR_INDEX_P ((unsigned) reg_renumber[NUM]))

/* Helpers for REG_OK_FOR_BASE_P.  */
#define REG_OK_FOR_BASE_STRICT_P(REG)		\
  HARD_REGNO_OK_FOR_BASE_P (REGNO (REG))	\
   || (REGNO (REG) == FRAME_POINTER_REGNUM)	\
   || (REGNO (REG) == ARG_POINTER_REGNUM)

#define REG_OK_FOR_BASE_NONSTRICT_P(REG)	\
  (REG_OK_FOR_BASE_STRICT_P (REG)		\
   || (REGNO (REG) >= FIRST_PSEUDO_REGISTER))

/* A C expression that is nonzero if x (assumed to be a reg RTX) is valid for
   use as a base register.  */
#ifdef REG_OK_STRICT
#define REG_OK_FOR_BASE_P(REG) REG_OK_FOR_BASE_STRICT_P (REG)
#else
#define REG_OK_FOR_BASE_P(REG) REG_OK_FOR_BASE_NONSTRICT_P (REG)
#endif

/* Helpers for REG_OK_FOR_INDEX_P.  */
#define REG_OK_FOR_INDEX_STRICT_P(REG)		\
  HARD_REGNO_OK_FOR_INDEX_P (REGNO (REG))

#define REG_OK_FOR_INDEX_NONSTRICT_P(REG)	\
  (REG_OK_FOR_INDEX_STRICT_P (REG)		\
   || (REGNO (REG) >= FIRST_PSEUDO_REGISTER))

/* A C expression that is nonzero if x (assumed to be a reg RTX) is valid for
   use as an index register.  */
#ifdef REG_OK_STRICT
#define REG_OK_FOR_INDEX_P(REG) REG_OK_FOR_INDEX_STRICT_P (REG)
#else
#define REG_OK_FOR_INDEX_P(REG) REG_OK_FOR_INDEX_NONSTRICT_P (REG)
#endif

/* The maximum number of bytes that a single instruction can move
   quickly between memory and registers or between two memory
   locations.  */
#define MOVE_MAX UNITS_PER_WORD

/* A C expression that is nonzero if on this machine the number of
   bits actually used for the count of a shift operation is equal to
   the number of bits needed to represent the size of the object being
   shifted.
   The MRISC32 EBF, EBFU and MKBF instructions only perform plain
   shift operations when bits 5-9 are zero, so we can not enable
   SHIFT_COUNT_TRUNCATED.  */
#define SHIFT_COUNT_TRUNCATED 0

/* All load operations zero extend.  */
#define LOAD_EXTEND_OP(MEM) ZERO_EXTEND

/* Define this macro if it is advisable to hold scalars in registers
   in a wider mode than that declared by the program.  In such cases,
   the value is constrained to be within the bounds of the declared
   type, but kept valid in the wider mode.  The signedness of the
   extension may differ from that of the type.  */
#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE)		\
  if (GET_MODE_CLASS (MODE) == MODE_INT		\
      && GET_MODE_SIZE (MODE) < UNITS_PER_WORD)	\
    {							\
	MODE = word_mode;				\
    }

/* A C expression for the cost of a branch instruction.  */
#define BRANCH_COST(SPEED,PREDICATABLE) ((PREDICATABLE) ? 1 : 3)

/* A number, the maximum number of registers that can appear in a
   valid memory address.  */
#define MAX_REGS_PER_ADDRESS 2

/* The clz instruction is defined for zero.  */
#define CLZ_DEFINED_VALUE_AT_ZERO(MODE, VALUE)	\
  ((VALUE) = GET_MODE_UNIT_BITSIZE (MODE), 2)
#define CTZ_DEFINED_VALUE_AT_ZERO(MODE, VALUE)	\
  ((VALUE) = GET_MODE_UNIT_BITSIZE (MODE), 2)

/* An alias for a machine mode name.  This is the machine mode that
   elements of a jump-table should have.  */
#define CASE_VECTOR_MODE SImode

/* Define built-in preprocessor macros and assertions for the target
   CPU. The body of this macro is evaluated at run-time.  */
#define TARGET_CPU_CPP_BUILTINS()			\
  {							\
    builtin_define ("__MRISC32__");			\
    builtin_define ("__LITTLE_ENDIAN__");		\
    if (TARGET_HARD_FLOAT)				\
      builtin_define ("__MRISC32_HARD_FLOAT__");	\
    if (TARGET_DIV)					\
      builtin_define ("__MRISC32_DIV__");		\
    if (TARGET_PACKED_OPS)				\
      builtin_define ("__MRISC32_PACKED_OPS__");	\
    if (TARGET_SATURATING_OPS)				\
      builtin_define ("__MRISC32_SATURATING_OPS__");	\
    if (TARGET_VECTOR_OPS)				\
      builtin_define ("__MRISC32_VECTOR_OPS__");	\
  }

/* Define TARGET_SUPPORTS_WIDE_INT to be nonzero to indicate that large
   integers are stored in CONST_WIDE_INT rtl objects.  */
#define TARGET_SUPPORTS_WIDE_INT 1

/* Define these boolean macros to indicate whether or not your
   architecture has (un)conditional branches that can span all of
   memory.  */
#define HAS_LONG_COND_BRANCH false
#define HAS_LONG_UNCOND_BRANCH true

#endif /* GCC_MRISC32_H */
