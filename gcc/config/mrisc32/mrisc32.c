/* Target Code for MRISC32
   Copyright (C) 2008-2019 Free Software Foundation, Inc.
   Contributed by Marcus Geelnard.

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

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "df.h"
#include "memmodel.h"
#include "tm_p.h"
#include "regs.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "diagnostic-core.h"
#include "output.h"
#include "stor-layout.h"
#include "varasm.h"
#include "calls.h"
#include "expr.h"
#include "builtins.h"
#include "print-tree.h"

#include "optabs.h"
#include "explow.h"
#include "cfgrtl.h"
#include "alias.h"

/* These 4 are needed to allow using satisfies_constraint_J.  */
#include "insn-config.h"
#include "recog.h"
#include "tm_p.h"
#include "tm-constrs.h"

/* This file should be included last.  */
#include "target-def.h"

/* This table is used by the REGNO_REG_CLASS() macro.  */

const enum reg_class mrisc32_regno_to_class[FIRST_PSEUDO_REGISTER] = {
  SPECIAL_REGS, GENERAL_REGS, GENERAL_REGS, GENERAL_REGS,  /*   z  s1  s2  s3 */
  GENERAL_REGS, GENERAL_REGS, GENERAL_REGS, GENERAL_REGS,  /*  s4  s5  s6  s7 */
  GENERAL_REGS, GENERAL_REGS, GENERAL_REGS, GENERAL_REGS,  /*  s8  s9 s10 s11 */
  GENERAL_REGS, GENERAL_REGS, GENERAL_REGS, GENERAL_REGS,  /* s12 s13 s14 s15 */
  GENERAL_REGS, GENERAL_REGS, GENERAL_REGS, GENERAL_REGS,  /* s16 s17 s18 s19 */
  GENERAL_REGS, GENERAL_REGS, GENERAL_REGS, GENERAL_REGS,  /* s20 s21 s22 s23 */
  GENERAL_REGS, GENERAL_REGS, GENERAL_REGS, GENERAL_REGS,  /* s24 s25  fp  tp */
  GENERAL_REGS, GENERAL_REGS, GENERAL_REGS, SPECIAL_REGS,  /*  sp  vl  lr  pc */
  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,   /*  vz  v1  v2  v3 */
  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,   /*  v4  v5  v6  v7 */
  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,   /*  v8  v9 v10 v11 */
  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,   /* v12 v13 v14 v15 */
  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,   /* v16 v17 v18 v19 */
  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,   /* v20 v21 v22 v23 */
  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,   /* v24 v25 v26 v27 */
  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,  VECTOR_REGS,   /* v28 V29 v30 v31 */
  SPECIAL_REGS, SPECIAL_REGS                               /* ?fp ?ap         */
};

#define LOSE_AND_RETURN(msgid, x)		\
  do						\
    {						\
      mrisc32_operand_lossage (msgid, x);	\
      return;					\
    } while (0)

#define BITSET_P(VALUE,BIT) (((VALUE) & (1L << (BIT))) != 0)

/* If non-zero, this is an offset to be added to SP to redefine the CFA
   when restoring the FP register from the stack.  Only valid when generating
   the epilogue.  */

static int epilogue_cfa_sp_offset;

/* Helper functions for checking constant ranges.  */

static bool
mrisc32_is_imm15 (int ival)
{
  return ival >= -16384 && ival <= 16383;
}


/* Worker function for TARGET_RETURN_IN_MEMORY.  */

static bool
mrisc32_return_in_memory (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  const HOST_WIDE_INT size = int_size_in_bytes (type);
  return (size == -1 || size > 2 * UNITS_PER_WORD);
}

/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its
   FUNCTION_DECL; otherwise, FUNC is 0.

   We always return values in register s1 for MRISC32.  */

static rtx
mrisc32_function_value (const_tree valtype,
		        const_tree fntype_or_decl ATTRIBUTE_UNUSED,
		        bool outgoing ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (TYPE_MODE (valtype), MRISC32_S1);
}

/* Define how to find the value returned by a library function.

   We always return values in register s1 for MRISC32.  */

static rtx
mrisc32_libcall_value (machine_mode mode,
                       const_rtx fun ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (mode, MRISC32_S1);
}

/* Handle TARGET_FUNCTION_VALUE_REGNO_P.

   We always return values in register s1 for MRISC32.  */

static bool
mrisc32_function_value_regno_p (const unsigned int regno)
{
  return (regno == MRISC32_S1);
}

/* Emit an error message when we're in an asm, and a fatal error for
   "normal" insns.  Formatted output isn't easily implemented, since we
   use output_operand_lossage to output the actual message and handle the
   categorization of the error.  */

static void
mrisc32_operand_lossage (const char *msgid, rtx op)
{
  debug_rtx (op);
  output_operand_lossage ("%s", msgid);
}

/* The PRINT_OPERAND_ADDRESS worker.  */

static void
mrisc32_print_operand_address (FILE *file, machine_mode, rtx x)
{
  if (REG_P (x))
    {
      /* Register indirect.  */
      fprintf (file, "%s, #0", reg_names[REGNO (x)]);
    }

  else if (GET_CODE (x) == PLUS
	   && REG_P (XEXP (x, 0))
	   && GET_CODE (XEXP (x, 1)) == CONST_INT)
    {
      /* Register + offset.  */
      fprintf (file, "%s, #%ld",
	       reg_names[REGNO (XEXP (x, 0))],
	       INTVAL (XEXP (x, 1)));
    }

  else if (GET_CODE (x) == PLUS
	   && REG_P (XEXP (x, 0))
	   && REG_P (XEXP (x, 1)))
    {
      /* Register + register.  */
      fprintf (file, "%s, %s",
	       reg_names[REGNO (XEXP (x, 0))],
	       reg_names[REGNO (XEXP (x, 1))]);
    }

  else if (GET_CODE (x) == PLUS
	   && GET_CODE (XEXP (x, 0)) == MULT
	   && REG_P (XEXP (XEXP (x, 0), 0))
	   && CONST_INT_P (XEXP (XEXP (x, 0), 1))
	   && REG_P (XEXP (x, 1)))
    {
      /* Register + scale * register.  */
      fprintf (file, "%s, %s*%d",
	       reg_names[REGNO (XEXP (x, 1))],
	       reg_names[REGNO (XEXP (XEXP (x, 0), 0))],
	       (int) INTVAL (XEXP (XEXP (x, 0), 1)));
    }

  else
    {
      /* This should be a plain symbol reference.  */
      output_addr_const (file, x);
    }
}

/* Helper for PRINT_OPERAND:  Print the condition part of the opcode
   to FILE.  */

static void
mrisc32_print_branch_condition (FILE *file, enum rtx_code code)
{
  switch (code)
    {
    case EQ:
    case NE:
      fputs (code == EQ ? "z" : "nz", file);
      break;

    case GT:
    case GE:
    case LT:
    case LE:
    case GTU:
    case GEU:
    case LTU:
    case LEU:
      /* Conveniently, the MRISC32 names for these conditions are the same
	 as their RTL equivalents.  */
      fputs (GET_RTX_NAME (code), file);
      break;

    default:
      output_operand_lossage ("'%s' is an unsupported branch condition",
			      GET_RTX_NAME (code));
      break;
    }
}

/* The PRINT_OPERAND worker.  */

static void
mrisc32_print_operand (FILE *file, rtx op, int letter)
{
  enum rtx_code code;

  gcc_assert (op);
  code = GET_CODE (op);

  switch (letter)
    {
    case 0:
      /* Print an operand as without a modifier letter.  */
      switch (code)
	{
	case REG:
	  if (REGNO (op) > MRISC32_V31)
	    internal_error ("internal error: bad register: %d", REGNO (op));
	  fprintf (file, "%s", reg_names[REGNO (op)]);
	  return;

	case MEM:
	  output_address (GET_MODE (XEXP (op, 0)), XEXP (op, 0));
	  return;

	default:
	  /* No need to handle all strange variants, let output_addr_const
	     do it for us.  */
	  if (CONSTANT_P (op))
	    {
	      output_addr_const (file, op);
	      return;
	    }

	  LOSE_AND_RETURN ("unexpected operand", op);
	}
      break;

    case 'C':
      mrisc32_print_branch_condition (file, code);
      break;

    case 'N':
      mrisc32_print_branch_condition (file, reverse_condition (code));
      break;

    default:
      LOSE_AND_RETURN ("invalid operand modifier letter", op);
    }
}


static void
mrisc32_emit_load_immediate_int (rtx dst, int ival)
{
  char insn_str[64];
  sprintf (insn_str, "ldi\t%%0, #%d", ival);
  output_asm_insn (insn_str, &dst);
}

static void
mrisc32_emit_load_immediate_sym_plus_int (rtx dst, rtx sym_ref, int ival)
{
  char insn_str[100];
  char num_str[24];
  sprintf (num_str, "%s%d", ival > 0 ? "+" : "", ival);
  sprintf (insn_str, "ldi\t%%0, #%%1%s@pc", ival ? num_str : "");
  rtx operands[2] = { dst, sym_ref };
  output_asm_insn (insn_str, &operands[0]);
}

const char *
mrisc32_emit_load_immediate (rtx dst, rtx src)
{
  if (GET_CODE (src) == CONST_INT)
    {
      mrisc32_emit_load_immediate_int (dst, INTVAL (src));
    }
  else if (GET_CODE (src) == CONST_DOUBLE)
    {
      long l;
      REAL_VALUE_TO_TARGET_SINGLE (*CONST_DOUBLE_REAL_VALUE (src), l);
      mrisc32_emit_load_immediate_int (dst, (int) l);
    }
  else if (SYMBOL_REF_P (src))
    {
      mrisc32_emit_load_immediate_sym_plus_int (dst, src, 0);
    }
  else if (GET_CODE (src) == CONST)
    {
      /* Handle const (plus (symbol_ref) (const_int))  */
      if (GET_CODE (XEXP (src, 0)) == PLUS
	  && SYMBOL_REF_P (XEXP (XEXP (src, 0), 0))
	  && CONST_INT_P (XEXP (XEXP (src, 0), 1)))
	{
	  rtx sym = XEXP (XEXP (src, 0), 0);
	  const int ival = INTVAL (XEXP (XEXP (src, 0), 1));
	  mrisc32_emit_load_immediate_sym_plus_int (dst, sym, ival);
	}
      else
	{
	  abort ();
	}
    }
  else
    {
      abort ();
    }

  return "";
}

/* Information about a function's frame layout.  */

struct GTY(())  mrisc32_frame_info {
  /* The size of the frame in bytes.  */
  HOST_WIDE_INT total_size;

  /* Bit X is set if the function saves or restores scalar register X.  */
  unsigned int mask;

  /* Offsets of scalar register save areas from frame bottom */
  HOST_WIDE_INT sreg_sp_offset;

  /* Offset of virtual frame pointer from stack pointer/frame bottom */
  HOST_WIDE_INT frame_pointer_offset;

  /* Offset of hard frame pointer from stack pointer/frame bottom */
  HOST_WIDE_INT hard_frame_pointer_offset;

  /* The offset of arg_pointer_rtx from the bottom of the frame.  */
  HOST_WIDE_INT arg_pointer_offset;
};

/* Per-function machine data.  */

struct GTY(()) machine_function
{
  /* The current frame information, calculated by mrisc32_compute_frame.  */
  struct mrisc32_frame_info frame;

};

/* Zero initialization is OK for all current fields.  */

static struct machine_function *
mrisc32_init_machine_status (void)
{
  return ggc_cleared_alloc<machine_function> ();
}

/* The TARGET_OPTION_OVERRIDE worker.  */

static void
mrisc32_option_override (void)
{
  /* Set the per-function-data initializer.  */
  init_machine_status = mrisc32_init_machine_status;
}

/* Emit an instruction of the form (set TARGET (CODE OP0 OP1)).  */

void
mrisc32_emit_binary (enum rtx_code code, rtx target, rtx op0, rtx op1)
{
  emit_insn (gen_rtx_SET (target, gen_rtx_fmt_ee (code, GET_MODE (target),
						  op0, op1)));
}

/* Compute (CODE OP0 OP1) and store the result in a new register
   of mode MODE.  Return that new register.  */

static rtx
mrisc32_force_binary (machine_mode mode, enum rtx_code code, rtx op0, rtx op1)
{
  rtx reg;

  reg = gen_reg_rtx (mode);
  mrisc32_emit_binary (code, reg, op0, op1);
  return reg;
}

/* Return true if CMP1 is a suitable second operand for integer ordering
   test CODE.  See also the *s[cc] patterns in mrisc32.md.  */

static bool
mrisc32_int_order_operand_ok_p (enum rtx_code code, rtx cmp1)
{
  switch (code)
    {
    case EQ:
    case NE:
    case LT:
    case LTU:
    case LE:
    case LEU:
      return mrisc32_reg_or_imm15_operand (cmp1, VOIDmode);

    case GT:
    case GTU:
    case GE:
    case GEU:
      return mrisc32_reg_or_int_zero_operand (cmp1, VOIDmode);

    default:
      gcc_unreachable ();
    }
}

/* Compare CMP0 and CMP1 using ordering test CODE and store the result
   in TARGET.  CMP0 and TARGET are register_operands.  If INVERT_PTR
   is nonnull, it's OK to set TARGET to the inverse of the result and
   flip *INVERT_PTR instead.  */

static void
mrisc32_emit_int_order_test (enum rtx_code code, bool *invert_ptr,
			     rtx target, rtx cmp0, rtx cmp1)
{
  machine_mode mode;

  /* First see if there is a MRISC32 instruction that can do this
     operation.  If not, try doing the same for the inverse
     operation.  If that also fails, force CMP1 into a register and
     try again.  */
  mode = GET_MODE (cmp0);
  if (mrisc32_int_order_operand_ok_p (code, cmp1))
    mrisc32_emit_binary (code, target, cmp0, cmp1);
  else
    {
      enum rtx_code inv_code = reverse_condition (code);
      if (!mrisc32_int_order_operand_ok_p (inv_code, cmp1))
	{
	  cmp1 = force_reg (mode, cmp1);
	  mrisc32_emit_int_order_test (code, invert_ptr, target, cmp0, cmp1);
	}
      else if (invert_ptr == 0)
	{
	  rtx inv_target;

	  inv_target = mrisc32_force_binary (GET_MODE (target),
					     inv_code, cmp0, cmp1);
	  mrisc32_emit_binary (XOR, target, inv_target, const1_rtx);
	}
      else
	{
	  *invert_ptr = !*invert_ptr;
	  mrisc32_emit_binary (inv_code, target, cmp0, cmp1);
	}
    }
}


/* Convert a comparison into something that can be used in a branch. On
   entry, *OP0 and *OP1 are the values being compared and *CODE is the code
   used to compare them.

   Update *CODE, *OP0 and *OP1 so that they describe the final comparison.  */

static void
mrisc32_prepare_compare (enum rtx_code *code, rtx *op0, rtx *op1)
{
  rtx cmp_op0 = *op0;
  rtx cmp_op1 = *op1;

  if (GET_MODE_CLASS (GET_MODE (*op0)) == MODE_INT)
    {
      if ((*op1 == const0_rtx) ||
	  (*op1 == const_true_rtx && (*code == EQ || *code == NE)))
	{
	  /* Trivial: Any comparison between a register and zero, or
	     equality comparison between a register and -1.  */
	  if (MEM_P (*op0))
	    *op0 = force_reg (GET_MODE (*op0), *op0);
	}
      else
	{
	  /* The comparison needs a separate s[cc] instruction.  Store the
	     result of the s[cc] in *OP0 and compare it against set (-1).  */
	  bool invert = false;
	  *op0 = gen_reg_rtx (GET_MODE (cmp_op0));
	  mrisc32_emit_int_order_test (*code, &invert, *op0, cmp_op0, cmp_op1);
	  *code = (invert ? NE : EQ);
	  *op1 = const_true_rtx;
	}
    }
  else
    {
      /* Floating point comparisons are implemented using fs[cc] + bs.  */
      *op0 = gen_reg_rtx (SImode);
      mrisc32_emit_binary (*code, *op0, cmp_op0, cmp_op1);
      *code = EQ;
      *op1 = const_true_rtx;
    }
}


/* Compare OPERANDS[1] with OPERANDS[2] using comparison code
   CODE and jump to OPERANDS[3] if the condition holds.  */

void
mrisc32_expand_conditional_branch (rtx *operands)
{
  enum rtx_code code = GET_CODE (operands[0]);
  rtx op0 = operands[1];
  rtx op1 = operands[2];
  rtx condition;

  mrisc32_prepare_compare (&code, &op0, &op1);
  condition = gen_rtx_fmt_ee (code, VOIDmode, op0, op1);
  emit_jump_insn (gen_condjump (condition, operands[3]));
}

void
mrisc32_expand_scc (rtx operands[])
{
  rtx target = operands[0];
  enum rtx_code code = GET_CODE (operands[1]);
  rtx op0 = operands[2];
  rtx op1 = operands[3];

  gcc_assert (GET_MODE_CLASS (GET_MODE (op0)) == MODE_INT);

  mrisc32_emit_int_order_test (code, 0, target, op0, op1);
}

/* Expand the patterns "call", "sibcall", "call_value" and "sibcall_value".
   Expands a function call where argument RETVAL is an optional RTX providing
   return value storage, the argument FNADDR is and RTX describing the function
   to call, the argument CALLARG1 is the number or registers used as operands
   and the argument SIBCALL should be true if this is a nested function
   call.  */

void
mrisc32_expand_call (rtx retval, rtx fnaddr, rtx callarg1, bool sibcall)
{
  rtx call, use = NULL;

#if 0
  /* Calls via the PLT require the PIC register.  */
  if (flag_pic
      && GET_CODE (XEXP (fnaddr, 0)) == SYMBOL_REF
      && !SYMBOL_REF_LOCAL_P (XEXP (fnaddr, 0)))
    {
      crtl->uses_pic_offset_table = 1;
      rtx hard_pic = gen_rtx_REG (Pmode, REAL_PIC_OFFSET_TABLE_REGNUM);
      emit_move_insn (hard_pic, pic_offset_table_rtx);
      use_reg (&use, hard_pic);
    }
#endif

  if (!mrisc32_call_insn_operand (XEXP (fnaddr, 0), Pmode))
    {
      fnaddr = copy_to_mode_reg (Pmode, XEXP (fnaddr, 0));
      fnaddr = gen_rtx_MEM (SImode, fnaddr);
    }

  call = gen_rtx_CALL (VOIDmode, fnaddr, callarg1);
  if (retval)
    call = gen_rtx_SET (retval, call);

  /* Normal calls clobber LR.  This is required in order to
     prevent e.g. a prologue store of LR being placed into
     the delay slot of the call, after it has been updated.  */
  if (!sibcall)
    {
      rtx clob = gen_rtx_CLOBBER (VOIDmode, gen_rtx_REG (Pmode, MRISC32_LR));
      call = gen_rtx_PARALLEL (VOIDmode, gen_rtvec (2, call, clob));
    }
  call = emit_call_insn (call);

  CALL_INSN_FUNCTION_USAGE (call) = use;
}

/* Returns true if REGNO must be saved for the current function.  */

static bool
mrisc32_callee_saved_regno_p (int regno)
{
  /* Check call-saved registers.  */
  /* TODO(m): This does not seem to cover VL, for instance.  */
  if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
    return true;

  /* Save LR when we're calling other functions.  */
  if (regno == MRISC32_LR && !crtl->is_leaf)
    return true;

  return false;
}

static unsigned int
mrisc32_stack_align (unsigned int loc)
{
  return (loc + ((STACK_BOUNDARY/8)-1)) & -(STACK_BOUNDARY/8);
}

/* MRISC32 stack frames grown downward.  High addresses are at the top.

	+-------------------------------+
	|                               |
	|  incoming stack arguments     |
	|                               |
	+-------------------------------+ <-- incoming stack pointer
	|                               |
	|  callee-allocated save area   |
	|  for arguments that are       |
	|  split between registers and  |
	|  the stack                    |
	|                               |
	+-------------------------------+ <-- arg_pointer_rtx
	|                               |
	|  callee-allocated save area   |
	|  for register varargs         |
	|                               |
	+-------------------------------+ <-- hard_frame_pointer_rtx;
	|                               |     stack_pointer_rtx + sreg_sp_offset
	|  Scalar registers save area   |       + UNITS_PER_WORD
	|                               |
	+-------------------------------+ <-- frame_pointer_rtx (virtual)
	|                               |
	|  local variables              |
	|                               |
      P +-------------------------------+
	|                               |
	|  outgoing stack arguments     |
	|                               |
	+-------------------------------+ <-- stack_pointer_rtx

   Dynamic stack allocations such as alloca insert data at point P.
   They decrease stack_pointer_rtx but leave frame_pointer_rtx and
   hard_frame_pointer_rtx unchanged.  */


/* Compute the size of the local area and the size to be adjusted by the
 * prologue and epilogue.  */

static void
mrisc32_compute_frame (void)
{
  struct mrisc32_frame_info *frame;
  HOST_WIDE_INT offset;
  unsigned int regno, num_x_saved = 0;

  frame = &cfun->machine->frame;

  memset (frame, 0, sizeof (*frame));

  /* Find out which scalar regs we need to save.  */
  num_x_saved = 0;
  for (regno = 0; regno <= 31; regno++)
    {
      if (mrisc32_callee_saved_regno_p (regno))
	frame->mask |= 1 << regno, num_x_saved++;
    }

  /* At the bottom of the frame are any outgoing stack arguments.  */
  offset = mrisc32_stack_align (crtl->outgoing_args_size);

  /* Next are local stack variables.  */
  /* TODO(m): For some functions (e.g. __negdi2) we get an offset here even
     though there are no local stack variables.  */
  offset += mrisc32_stack_align (get_frame_size ());

  /* The virtual frame pointer points above the local variables.  */
  frame->frame_pointer_offset = offset;

  /* Next are the callee-saved scalar regs.  */
  if (frame->mask)
    {
      unsigned x_save_size = mrisc32_stack_align (num_x_saved * UNITS_PER_WORD);
      offset += x_save_size;
    }
  frame->sreg_sp_offset = offset - UNITS_PER_WORD;

  /* The hard frame pointer points above the callee-saved GPRs.  */
  frame->hard_frame_pointer_offset = offset;

  /* Next is the callee-allocated area for pretend stack arguments.  */
  offset += mrisc32_stack_align (crtl->args.pretend_args_size);

  /* Arg pointer must be below pretend args, but must be above alignment
     padding.  */
  frame->arg_pointer_offset = offset - crtl->args.pretend_args_size;
  frame->total_size = offset;
}

static int
mrisc32_num_saved_regs ()
{
  struct mrisc32_frame_info *frame = &cfun->machine->frame;
  int num_saved = 0;
  for (int regno = 0; regno <= 31; regno++)
    if (BITSET_P (frame->mask, regno))
      ++num_saved;
  return num_saved;
}

/* Emit a move from SRC to DEST.  Assume that the move expanders can
   handle all moves if !can_create_pseudo_p ().  The distinction is
   important because, unlike emit_move_insn, the move expanders know
   how to force Pmode objects into the constant pool even when the
   constant pool address is not itself legitimate.  */

static rtx
mrisc32_emit_move (rtx dest, rtx src)
{
  return (can_create_pseudo_p ()
	  ? emit_move_insn (dest, src)
	  : emit_move_insn_1 (dest, src));
}

/* Make the last instruction frame-related and note that it performs
   the operation described by FRAME_PATTERN.  */

static void
mrisc32_set_frame_expr (rtx frame_pattern)
{
  rtx insn;

  insn = get_last_insn ();
  RTX_FRAME_RELATED_P (insn) = 1;
  REG_NOTES (insn) = alloc_EXPR_LIST (REG_FRAME_RELATED_EXPR,
				      frame_pattern,
				      REG_NOTES (insn));
}

/* Return a frame-related rtx that stores REG at MEM.
   REG must be a single register.  */

static rtx
mrisc32_frame_set (rtx mem, rtx reg)
{
  rtx set = gen_rtx_SET (mem, reg);
  RTX_FRAME_RELATED_P (set) = 1;
  return set;
}

/* Save register REG to MEM.  Make the instruction frame-related.  */

static void
mrisc32_save_reg (rtx reg, rtx mem)
{
  mrisc32_emit_move (mem, reg);
  mrisc32_set_frame_expr (mrisc32_frame_set (mem, reg));
}

/* Restore register REG from MEM.  */

static void
mrisc32_restore_reg (rtx reg, rtx mem)
{
  rtx insn = mrisc32_emit_move (reg, mem);
  rtx dwarf = NULL_RTX;
  dwarf = alloc_reg_note (REG_CFA_RESTORE, reg, dwarf);

  if (epilogue_cfa_sp_offset && REGNO (reg) == HARD_FRAME_POINTER_REGNUM)
    {
      rtx cfa_adjust_rtx = gen_rtx_PLUS (Pmode, stack_pointer_rtx,
					 GEN_INT (epilogue_cfa_sp_offset));
      dwarf = alloc_reg_note (REG_CFA_DEF_CFA, cfa_adjust_rtx, dwarf);
    }

  REG_NOTES (insn) = dwarf;
  RTX_FRAME_RELATED_P (insn) = 1;
}

void
mrisc32_expand_prologue (void)
{
  struct mrisc32_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT size = frame->total_size;
  rtx insn;
  int num_saved_regs;

  mrisc32_compute_frame ();

  if (flag_stack_usage_info)
    current_function_static_stack_size = size;

  /* Adjust the stack pointer.  */
  if (size != 0)
    {
      insn = gen_add3_insn (stack_pointer_rtx,
			    stack_pointer_rtx,
			    GEN_INT (-size));
      RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
    }

  /* Save the registers.  */
  num_saved_regs = mrisc32_num_saved_regs ();
  if (num_saved_regs > 0)
    {
      /* Save the link register and s-registers. */
      HOST_WIDE_INT offset = frame->sreg_sp_offset -
			     (num_saved_regs - 1) * UNITS_PER_WORD;
      for (int regno = 0; regno <= 31; regno++)
	if (BITSET_P (frame->mask, regno))
	  {
	    rtx mem =
	      gen_frame_mem (word_mode, plus_constant (Pmode,
						       stack_pointer_rtx,
						       offset));
	    mrisc32_save_reg (gen_rtx_REG (word_mode, regno), mem);
	    offset += UNITS_PER_WORD;
	  }
    }

  /* Set up the frame pointer, if we're using one.  */
  if (frame_pointer_needed)
    {
      insn = gen_add3_insn (hard_frame_pointer_rtx, stack_pointer_rtx,
			    GEN_INT (frame->hard_frame_pointer_offset));
      RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
    }
}

void
mrisc32_expand_epilogue (void)
{
  struct mrisc32_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT size = frame->total_size;
  rtx insn;
  int num_saved_regs;

  mrisc32_compute_frame ();

  /* Reset the epilogue cfa info before starting to emit the epilogue.  */
  epilogue_cfa_sp_offset = 0;

  /* Move past any dynamic stack allocations.  */
  if (cfun->calls_alloca)
    {
      rtx adjust = GEN_INT (-frame->hard_frame_pointer_offset);
      insn = emit_insn (
	       gen_add3_insn (stack_pointer_rtx, hard_frame_pointer_rtx,
			      adjust));

      rtx dwarf = NULL_RTX;
      rtx cfa_adjust_value = gen_rtx_PLUS (
			       Pmode, hard_frame_pointer_rtx,
			       GEN_INT (-frame->hard_frame_pointer_offset));
      rtx cfa_adjust_rtx = gen_rtx_SET (stack_pointer_rtx, cfa_adjust_value);
      dwarf = alloc_reg_note (REG_CFA_ADJUST_CFA, cfa_adjust_rtx, dwarf);
      RTX_FRAME_RELATED_P (insn) = 1;

      REG_NOTES (insn) = dwarf;
    }

  if (frame_pointer_needed)
    {
      /* Tell mrisc32_restore_reg to emit dwarf to redefine CFA when restoring
	 old value of FP.  */
      epilogue_cfa_sp_offset = size;
    }

  /* Restore the registers.  */
  num_saved_regs = mrisc32_num_saved_regs ();
  if (num_saved_regs > 0)
    {
      HOST_WIDE_INT offset = frame->sreg_sp_offset -
			     (num_saved_regs - 1) * UNITS_PER_WORD;
      for (int regno = 0; regno <= 31; regno++)
	if (BITSET_P (frame->mask, regno))
	  {
	    rtx mem =
	      gen_frame_mem (word_mode, plus_constant (Pmode,
						       stack_pointer_rtx,
						       offset));
	    mrisc32_restore_reg (gen_rtx_REG (word_mode, regno), mem);
	    offset += UNITS_PER_WORD;
	  }
    }

  /* Deallocate the final bit of the frame.  */
  if (size > 0)
    {
      insn = emit_insn (gen_add3_insn (stack_pointer_rtx, stack_pointer_rtx,
				       GEN_INT (size)));

      rtx dwarf = NULL_RTX;
      rtx cfa_adjust_rtx = gen_rtx_PLUS (Pmode, stack_pointer_rtx,
					 const0_rtx);
      dwarf = alloc_reg_note (REG_CFA_DEF_CFA, cfa_adjust_rtx, dwarf);
      RTX_FRAME_RELATED_P (insn) = 1;

      REG_NOTES (insn) = dwarf;
    }
}

/* Implements the macro INITIAL_ELIMINATION_OFFSET, return the OFFSET.  */

int
mrisc32_initial_elimination_offset (int from, int to)
{
  HOST_WIDE_INT src, dest;

  mrisc32_compute_frame ();

  if (to == HARD_FRAME_POINTER_REGNUM)
    dest = cfun->machine->frame.hard_frame_pointer_offset;
  else if (to == STACK_POINTER_REGNUM)
    dest = 0; /* The stack pointer is the base of all offsets, hence 0.  */
  else
    gcc_unreachable ();

  if (from == FRAME_POINTER_REGNUM)
    src = cfun->machine->frame.frame_pointer_offset;
  else if (from == ARG_POINTER_REGNUM)
    src = cfun->machine->frame.arg_pointer_offset;
  else
    gcc_unreachable ();

  return src - dest;
}

/* Return the next register to be used to hold a function argument or
   NULL_RTX if there's no more space.  */

static rtx
mrisc32_function_arg (cumulative_args_t cum_v, const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  if (*cum <= MRISC32_S8)
    return gen_rtx_REG (arg.mode, *cum);
  else
    return NULL_RTX;
}

// TODO(m): Just use arg.promoted_size_in_bytes () instead?
#define MRISC32_FUNCTION_ARG_SIZE(MODE, TYPE)	\
  ((MODE) != BLKmode ? GET_MODE_SIZE (MODE)	\
   : (unsigned) int_size_in_bytes (TYPE))

static void
mrisc32_function_arg_advance (cumulative_args_t cum_v,
			      const function_arg_info &arg)
{
  /* TODO(m): Check if this is correct.  */

  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  *cum = (*cum <= MRISC32_S8
	  ? *cum + ((MRISC32_FUNCTION_ARG_SIZE (arg.mode, arg.type) + 3) / 4)
	  : *cum);
}

/* Worker function for TARGET_SETUP_INCOMING_VARARGS.  */

static void
mrisc32_setup_incoming_varargs (cumulative_args_t cum_v,
			        const function_arg_info &arg,
			        int *pretend_size,
			        int no_rtl)
{
  CUMULATIVE_ARGS local_cum;
  int num_saved;
  int regno;

  /* The caller has advanced CUM up to, but not beyond, the last named
     argument.  Advance a local copy of CUM past the last "real" named
     argument, to find out how many registers are left over.  */
  local_cum = *get_cumulative_args (cum_v);
  mrisc32_function_arg_advance (pack_cumulative_args (&local_cum), arg);

  /* Find out how many registers we need to save.  */
  num_saved = 1 + MRISC32_S8 - local_cum;

  *pretend_size = num_saved < 0 ? 0 : GET_MODE_SIZE (SImode) * num_saved;

  if (no_rtl)
    return;

  for (regno = local_cum; regno <= MRISC32_S8; regno++)
    {
      rtx reg = gen_rtx_REG (SImode, regno);
      rtx slot = gen_rtx_PLUS (Pmode,
			       gen_rtx_REG (SImode, ARG_POINTER_REGNUM),
			       GEN_INT (UNITS_PER_WORD * (regno - local_cum)));
      emit_move_insn (gen_rtx_MEM (SImode, slot), reg);
    }
}

/* Return non-zero if the function argument described by ARG is to be
   passed by reference.  */

static bool
mrisc32_pass_by_reference (cumulative_args_t, const function_arg_info &arg)
{
  if (arg.aggregate_type_p ())
    return true;
  unsigned HOST_WIDE_INT size = arg.type_size_in_bytes ();
  return size > 4 * (MRISC32_S8 - MRISC32_S1 + 1);
}

/* Some function arguments will only partially fit in the registers
   that hold arguments.  Given a new arg, return the number of bytes
   that fit in argument passing registers.  */

static int
mrisc32_arg_partial_bytes (cumulative_args_t cum_v, const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
  int bytes_left, size;

  if (*cum > MRISC32_S8)
    return 0;

  if (mrisc32_pass_by_reference (cum_v, arg))
    size = 4;
  else if (arg.type)
    {
      if (AGGREGATE_TYPE_P (arg.type))
	return 0;
      size = int_size_in_bytes (arg.type);
    }
  else
    size = GET_MODE_SIZE (arg.mode);

  bytes_left = 4 * ((MRISC32_S8 - MRISC32_S1 + 1) - (*cum - MRISC32_S1));

  if (size > bytes_left)
    return bytes_left;
  else
    return 0;
}

/* Worker function for TARGET_STATIC_CHAIN.  */

static rtx
mrisc32_static_chain (const_tree ARG_UNUSED (fndecl_or_type), bool incoming_p)
{
  rtx addr, mem;

  if (incoming_p)
    addr = plus_constant (Pmode, arg_pointer_rtx, 2 * UNITS_PER_WORD);
  else
    addr = plus_constant (Pmode, stack_pointer_rtx, -UNITS_PER_WORD);

  mem = gen_rtx_MEM (Pmode, addr);
  MEM_NOTRAP_P (mem) = 1;

  return mem;
}

/* Worker function for TARGET_ASM_TRAMPOLINE_TEMPLATE.  */

static void
mrisc32_asm_trampoline_template (FILE *f ATTRIBUTE_UNUSED)
{
  /* TODO(m): Implement me! */
  abort();
}

/* Worker function for TARGET_TRAMPOLINE_INIT.  */

static void
mrisc32_trampoline_init (rtx m_tramp ATTRIBUTE_UNUSED,
			 tree fndecl ATTRIBUTE_UNUSED,
			 rtx chain_value ATTRIBUTE_UNUSED)
{
  /* TODO(m): Implement me! */
  abort();
}


/* Helper functions for `mrisc32_legitimate_address_p' and friends.  */

static bool
mrisc32_reg_ok_for_base_p (const_rtx reg, bool strict_p)
{
  if (strict_p)
    return REG_OK_FOR_BASE_STRICT_P (reg);
  else
    return REG_OK_FOR_BASE_NONSTRICT_P (reg);
}

static bool
mrisc32_reg_ok_for_index_p (const_rtx reg, bool strict_p)
{
  if (strict_p)
    return REG_OK_FOR_INDEX_STRICT_P (reg);
  else
    return REG_OK_FOR_INDEX_NONSTRICT_P (reg);
}

/* Return true for a valid memory offset addresses.  */

bool
mrisc32_offset_address_p (rtx x, bool strict_p)
{
  if (!MEM_P (x))
    return false;

  x = XEXP (x, 0);

  if (GET_CODE (x) == PLUS
      && REG_P (XEXP (x, 0))
      && mrisc32_reg_ok_for_base_p (XEXP (x, 0), strict_p)
      && CONST_INT_P (XEXP (x, 1))
      && mrisc32_is_imm15 (INTVAL (XEXP (x, 1))))
    {
      return true;
    }

  return false;
}

/* Return true for memory indexed addresses.  */

bool
mrisc32_indexed_address_p (rtx x, bool strict_p)
{
  if (!MEM_P (x))
    return false;

  x = XEXP (x, 0);

  /* Register + register.  */
  if (GET_CODE (x) == PLUS
      && REG_P (XEXP (x, 0))
      && mrisc32_reg_ok_for_base_p (XEXP (x, 0), strict_p)
      && REG_P (XEXP (x, 1))
      && mrisc32_reg_ok_for_index_p (XEXP (x, 1), strict_p))
    {
      return true;
    }

  /* Register + scale * register.  */
  if (GET_CODE (x) == PLUS
      && GET_CODE (XEXP (x, 0)) == MULT
      && REG_P (XEXP (XEXP (x, 0), 0))
      && mrisc32_reg_ok_for_index_p (XEXP (XEXP (x, 0), 0), strict_p)
      && CONST_INT_P (XEXP (XEXP (x, 0), 1))
      && (INTVAL (XEXP (XEXP (x, 0), 1)) == 1
	  || INTVAL (XEXP (XEXP (x, 0), 1)) == 2
	  || INTVAL (XEXP (XEXP (x, 0), 1)) == 4
	  || INTVAL (XEXP (XEXP (x, 0), 1)) == 8)
      && REG_P (XEXP (x, 1))
      && mrisc32_reg_ok_for_base_p (XEXP (x, 1), strict_p))
    {
      return true;
    }

  return false;
}

/* Used by the mrisc32_*_movsrc_operand predicates.  */

bool
mrisc32_valid_memsrc_operand (rtx op)
{
  if (MEM_P (op))
    {
      rtx x = XEXP (op, 0);

      /* Label  */
      if (GET_CODE (x) == LABEL_REF)
	return true;

      /* reg (register indirect)  */
      if (REG_P (x))
	return true;

      /* reg, #imm15  */
      if (GET_CODE (x) == PLUS
	  && REG_P (XEXP (x, 0))
	  && CONST_INT_P (XEXP (x, 1))
	  && mrisc32_is_imm15 (INTVAL (XEXP (x, 1))))
	return true;

      /* reg, reg  */
      if (GET_CODE (x) == PLUS
	  && REG_P (XEXP (x, 0))
	  && REG_P (XEXP (x, 1)))
	return true;

      /* reg, scale * reg  */
      if (GET_CODE (x) == PLUS
	  && GET_CODE (XEXP (x, 0)) == MULT
	  && REG_P (XEXP (XEXP (x, 0), 0))
	  && CONST_INT_P (XEXP (XEXP (x, 0), 1))
	  && (INTVAL (XEXP (XEXP (x, 0), 1)) == 1
	      || INTVAL (XEXP (XEXP (x, 0), 1)) == 2
	      || INTVAL (XEXP (XEXP (x, 0), 1)) == 4
	      || INTVAL (XEXP (XEXP (x, 0), 1)) == 8)
	  && REG_P (XEXP (x, 1)))
	return true;
    }

  return false;
}

/* Worker function for TARGET_LEGITIMATE_ADDRESS_P.  */

static bool
mrisc32_legitimate_address_p (machine_mode mode ATTRIBUTE_UNUSED,
			      rtx x, bool strict_p,
			      addr_space_t as)
{
  gcc_assert (ADDR_SPACE_GENERIC_P (as));

  /* Register indirect.  */
  if (REG_P (x) && mrisc32_reg_ok_for_base_p (x, strict_p))
    {
      return true;
    }

  /* Register + offset.  */
  if (GET_CODE (x) == PLUS
      && REG_P (XEXP (x, 0))
      && mrisc32_reg_ok_for_base_p (XEXP (x, 0), strict_p)
      && CONST_INT_P (XEXP (x, 1))
      && IN_RANGE (INTVAL (XEXP (x, 1)), -16384, 16383))
    {
      return true;
    }

  /* Register + register.  */
  if (GET_CODE (x) == PLUS
      && REG_P (XEXP (x, 0))
      && mrisc32_reg_ok_for_base_p (XEXP (x, 0), strict_p)
      && REG_P (XEXP (x, 1))
      && mrisc32_reg_ok_for_index_p (XEXP (x, 1), strict_p))
    {
      return true;
    }

  /* Register + scale * register.  */
  if (GET_CODE (x) == PLUS
      && GET_CODE (XEXP (x, 0)) == MULT
      && REG_P (XEXP (XEXP (x, 0), 0))
      && mrisc32_reg_ok_for_index_p (XEXP (XEXP (x, 0), 0), strict_p)
      && CONST_INT_P (XEXP (XEXP (x, 0), 1))
      && (INTVAL (XEXP (XEXP (x, 0), 1)) == 1
	  || INTVAL (XEXP (XEXP (x, 0), 1)) == 2
	  || INTVAL (XEXP (XEXP (x, 0), 1)) == 4
	  || INTVAL (XEXP (XEXP (x, 0), 1)) == 8)
      && REG_P (XEXP (x, 1))
      && mrisc32_reg_ok_for_base_p (XEXP (x, 1), strict_p))
    {
      return true;
    }

  /* Symbol ref.  */
  if (GET_CODE (x) == SYMBOL_REF
      || GET_CODE (x) == LABEL_REF
      || GET_CODE (x) == CONST)
    {
      return true;
    }

  return false;
}

/* Worker function for TARGET_LEGITIMATE_CONSTANT_P. The main purpose of this
   function is to force certain constants to be loaded with a memory load
   instruction (via a local label) instead of endocing a complex combination
   of immediate load instructions.  */

bool
mrisc32_legitimate_constant_p (machine_mode mode, rtx x)
{
  /* Anything larger than the native word size is better handled by a
     memory load operation. */
  if (GET_MODE_SIZE (mode) > 4)
    return false;

  switch (GET_CODE (x))
    {
    case CONST:
    case CONST_INT:
    case CONST_DOUBLE:
    case LABEL_REF:
      return true;

    case SYMBOL_REF:
      /* TLS symbols are never valid.  */
      return SYMBOL_REF_TLS_MODEL (x) == 0;

    default:
      return false;
    }
}

/* Check if an "and" operation can be implemented using SHUF. Note that we do
   not want to use SHUF when we can use AND with a 15-bit immediate value
   instead, since the latter produces code that is easier to read.  */

bool
mrisc32_is_mask_suitable_for_shuf (rtx op)
{
  unsigned mask = (unsigned) INTVAL (op);
  return mask == 0x0000ff00u ||
	 mask == 0x0000ffffu ||
	 mask == 0x00ff0000u ||
	 mask == 0x00ff00ffu ||
	 mask == 0x00ffff00u ||
	 mask == 0x00ffffffu ||
	 mask == 0xff000000u ||
	 mask == 0xff0000ffu ||
	 mask == 0xff00ff00u ||
	 mask == 0xff00ffffu ||
	 mask == 0xffff0000u ||
	 mask == 0xffff00ffu;
}


/* Convert a byte mask (e.g. 0xff00ff00) to a SHUF control word.  */

rtx
mrisc32_mask_to_shuf_ctrl (rtx op)
{
  unsigned mask0 = (unsigned) INTVAL (op);
  unsigned mask1 = mask0 >> 8;
  unsigned mask2 = mask0 >> 16;
  unsigned mask3 = mask0 >> 24;

  unsigned clear0 = 4 & ~mask0;
  unsigned clear1 = 4 & ~mask1;
  unsigned clear2 = 4 & ~mask2;
  unsigned clear3 = 4 & ~mask3;

  unsigned ctrl = (clear0 | 0) |
		  ((clear1 | 1) << 3) |
		  ((clear2 | 2) << 6) |
		  ((clear3 | 3) << 9);

  return GEN_INT (ctrl);
}


/* Convert a byte- or half-word extract operation to a SHUF control word.  */

rtx
mrisc32_extv_to_shuf_ctrl (rtx width, rtx bitpos, bool sign_extend)
{
  /* Sanity check the arguments.  */
  if ((INTVAL (width) != 8 && INTVAL (width) != 16) ||
      (INTVAL (bitpos) != 0 && INTVAL (bitpos) != 8 &&
       INTVAL (bitpos) != 16 && INTVAL (bitpos) != 24))
    abort ();

  const int num_bytes = INTVAL (width) / 8;
  const int byte_no = INTVAL (bitpos) / 8;

  int ctrl = sign_extend ? 0b1000000000000 : 0;
  switch (num_bytes)
    {
      case 1:
	ctrl |= 0b0100100100000 | byte_no * 0b001001001001;
	break;
      case 2:
	ctrl |= 0b0100100000000 | (byte_no + (byte_no + 1) * 0b001001001000);
	break;
    }

  return GEN_INT (ctrl);
}

rtx
mrisc32_subreg_di_low (rtx op)
{
  return simplify_gen_subreg (SImode, op, DImode, 0);
}

rtx
mrisc32_subreg_di_high (rtx op)
{
  return simplify_gen_subreg (SImode, op, DImode, UNITS_PER_WORD);
}


/* Initialize the GCC target structure.  */

#undef  TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES	hook_bool_const_tree_true

#undef  TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY		mrisc32_return_in_memory
#undef  TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK	must_pass_in_stack_var_size
#undef  TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE        mrisc32_pass_by_reference
#undef  TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES        mrisc32_arg_partial_bytes
#undef  TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG		mrisc32_function_arg
#undef  TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE	mrisc32_function_arg_advance

#undef TARGET_LRA_P
#define TARGET_LRA_P hook_bool_void_false

#undef  TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P
#define TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P	mrisc32_legitimate_address_p
#undef  TARGET_LEGITIMATE_CONSTANT_P
#define TARGET_LEGITIMATE_CONSTANT_P  mrisc32_legitimate_constant_p

#undef  TARGET_SETUP_INCOMING_VARARGS
#define TARGET_SETUP_INCOMING_VARARGS 	mrisc32_setup_incoming_varargs

/* Define this to return an RTX representing the place where a
   function returns or receives a value of data type RET_TYPE, a tree
   node representing a data type.  */
#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE mrisc32_function_value
#undef TARGET_LIBCALL_VALUE
#define TARGET_LIBCALL_VALUE mrisc32_libcall_value
#undef TARGET_FUNCTION_VALUE_REGNO_P
#define TARGET_FUNCTION_VALUE_REGNO_P mrisc32_function_value_regno_p

#undef TARGET_FLOAT_EXCEPTIONS_ROUNDING_SUPPORTED_P
#define TARGET_FLOAT_EXCEPTIONS_ROUNDING_SUPPORTED_P hook_bool_void_false

#undef TARGET_STATIC_CHAIN
#define TARGET_STATIC_CHAIN mrisc32_static_chain
#undef TARGET_ASM_TRAMPOLINE_TEMPLATE
#define TARGET_ASM_TRAMPOLINE_TEMPLATE mrisc32_asm_trampoline_template
#undef TARGET_TRAMPOLINE_INIT
#define TARGET_TRAMPOLINE_INIT mrisc32_trampoline_init

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE mrisc32_option_override

#undef  TARGET_PRINT_OPERAND
#define TARGET_PRINT_OPERAND mrisc32_print_operand
#undef  TARGET_PRINT_OPERAND_ADDRESS
#define TARGET_PRINT_OPERAND_ADDRESS mrisc32_print_operand_address

#undef  TARGET_CONSTANT_ALIGNMENT
#define TARGET_CONSTANT_ALIGNMENT constant_alignment_word_strings

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-mrisc32.h"
