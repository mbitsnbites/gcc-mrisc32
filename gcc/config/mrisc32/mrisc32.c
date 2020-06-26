/* Target Code for MRISC32
   Copyright (C) 2008-2020 Free Software Foundation, Inc.
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

bool
mrisc32_is_imm14 (const int ival)
{
  if (ival >= -8192 && ival <= 8191)
    return true;

  if ((ival & 0x7ffff) == 0 || (ival & 0x7ffff) == 0x7ffff)
    return true;

  return false;
}

/* Return TRUE if all values between -OFFSET and +OFFSET can be
   represented as an immediate value for all regular arithmetic
   instructions (including ADD and LDW/STW).  */

static bool
mrisc32_is_small_offset (const int offset)
{
  return (offset >= -8191 && offset <= 8191);
}

/* Worker function for TARGET_RETURN_IN_MEMORY.  */

static bool
mrisc32_return_in_memory (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  const HOST_WIDE_INT size = int_size_in_bytes (type);
  return (size == -1 || size > 2 * UNITS_PER_WORD);
}

/* Implement TARGET_FUNCTION_VALUE and TARGET_LIBCALL_VALUE.
   For normal calls, VALTYPE is the return type and MODE is VOIDmode.
   For libcalls, VALTYPE is null and MODE is the mode of the return value.  */

static rtx
mrisc32_function_value_1 (const_tree valtype, const_tree fn_decl_or_type,
			  machine_mode mode)
{
  if (valtype)
    {
      int unsigned_p;
      const_tree func;

      if (fn_decl_or_type && DECL_P (fn_decl_or_type))
	func = fn_decl_or_type;
      else
	func = NULL;

      mode = TYPE_MODE (valtype);
      unsigned_p = TYPE_UNSIGNED (valtype);

      /* Since TARGET_PROMOTE_FUNCTION_MODE unconditionally promotes
	 return values, promote the mode here too.  */
      mode = promote_function_mode (valtype, mode, &unsigned_p, func, 1);
    }

  /* We always return values in register s1.  */
  return gen_rtx_REG (mode, MRISC32_S1);
}

/* Define how to find the value returned by a function.  */

static rtx
mrisc32_function_value (const_tree valtype,
			const_tree fn_decl_or_type,
			bool outgoing ATTRIBUTE_UNUSED)
{
  return mrisc32_function_value_1 (valtype, fn_decl_or_type, VOIDmode);
}

/* Define how to find the value returned by a library function.  */

static rtx
mrisc32_libcall_value (machine_mode mode,
		       const_rtx fun ATTRIBUTE_UNUSED)
{
  return mrisc32_function_value_1 (NULL_TREE, NULL_TREE, mode);
}

/* Handle TARGET_FUNCTION_VALUE_REGNO_P.  */

static bool
mrisc32_function_value_regno_p (const unsigned int regno)
{
  /* We always return values in register s1.  */
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

static void
mrisc32_print_set_condition (FILE *file, enum rtx_code code)
{
  switch (code)
    {
    case EQ:
    case NE:
    case LT:
    case LE:
    case LTU:
    case LEU:
    case GT:
    case GE:
    case GTU:
    case GEU:
      /* Conveniently, the MRISC32 names for these conditions are the same
	 as their RTL equivalents.  */
      fputs (GET_RTX_NAME (code), file);
      break;

    default:
      output_operand_lossage ("'%s' is an unsupported set condition",
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

    case 'S':
      mrisc32_print_set_condition (file, code);
      break;

    case 'R':
      mrisc32_print_set_condition (file, reverse_condition (code));
      break;

    case 'Q':
      mrisc32_print_set_condition (file, swap_condition (code));
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

  /* Offsets of vector register save areas from frame bottom */
  HOST_WIDE_INT vreg_sp_offset;

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
      return mrisc32_reg_or_imm14_operand (cmp1, VOIDmode);

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
	  /* Do the reversed comparison, and then reverse the result
	     by producing result = (xor (result -1)).  */
	  rtx inv_target;
	  inv_target = mrisc32_force_binary (GET_MODE (target),
					      inv_code, cmp0, cmp1);
	  mrisc32_emit_binary (XOR, target, inv_target, constm1_rtx);
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
mrisc32_prepare_compare (enum rtx_code *code, rtx *op0, rtx *op1, machine_mode cmp_mode)
{
  rtx cmp_op0 = *op0;
  rtx cmp_op1 = *op1;

  if (cmp_mode == SImode)
    {
      if ((*op1 == const0_rtx) ||
	  (*op1 == constm1_rtx && (*code == EQ || *code == NE)))
	{
	  /* Trivial: Any comparison between a register and zero, or
	     equality comparison between a register and -1, can be
	     handled with a single conditional branch instruction.
	     We don't have to do any perparations.  */
	  return;
	}
      else
	{
	  /* The comparison needs a separate s[cc] instruction.  Store the
	     result of the s[cc] in *OP0 and compare it against true (-1).  */
	  bool invert = false;
	  *op0 = gen_reg_rtx (SImode);
	  mrisc32_emit_int_order_test (*code, &invert, *op0, cmp_op0, cmp_op1);
	  *code = (invert ? NE : EQ);
	  *op1 = constm1_rtx;
	}
    }
  else if (cmp_mode == SFmode)
    {
      /* Floating point comparisons are implemented using fs[cc] + bs.  */
      *op0 = gen_reg_rtx (SImode);
      mrisc32_emit_binary (*code, *op0, cmp_op0, cmp_op1);
      *code = EQ;
      *op1 = constm1_rtx;
    }
  else
    {
      gcc_unreachable ();
    }
}


/* Compare OPERANDS[1] with OPERANDS[2] using comparison code
   CODE and jump to OPERANDS[3] if the condition holds.  */

void
mrisc32_expand_conditional_branch (rtx *operands, machine_mode cmp_mode)
{
  enum rtx_code code = GET_CODE (operands[0]);
  rtx op0 = operands[1];
  rtx op1 = operands[2];
  rtx condition;
  rtx set_rtx;

  /* Prepare the comparison operation (emit extra code if
     necessary).  */
  mrisc32_prepare_compare (&code, &op0, &op1, cmp_mode);

  /* Emit the branch instruction.  */
  condition = gen_rtx_fmt_ee (code, VOIDmode, op0, op1);
  set_rtx = gen_rtx_SET (
      pc_rtx,
      gen_rtx_IF_THEN_ELSE (VOIDmode,
			     condition,
			     gen_rtx_LABEL_REF (VOIDmode, operands[3]),
			     pc_rtx));
  emit_jump_insn (set_rtx);
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

/* Returns true if REGNO must be saved for the current function.  */

static bool
mrisc32_callee_saved_regno_p (int regno)
{
  /* Check call-saved registers.  */
  /* TODO(m): This does not seem to cover VL, for instance.  */
  if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
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
	+-------------------------------+ <-- stack_pointer_rtx + vreg_sp_offset
	|                               |       + UNITS_PER_WORD
	|  Vector registers save area   |
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

/* For stack frames that can't be allocated with a single ADD immediate
   instruction, compute the best value to initially allocate. It must at
   a minimum allocate enough space to spill the callee-saved
   registers.  */

static HOST_WIDE_INT
mrisc32_first_stack_step (struct mrisc32_frame_info *frame)
{
  HOST_WIDE_INT size;

  /* Can allocate the full stack frame with a single instruction?  */
  size = frame->total_size;
  if (mrisc32_is_small_offset (size))
    return size;

  /* Next, try to allocate at least enough to store the callee-saved regs.  */
  size = mrisc32_stack_align (frame->total_size - frame->vreg_sp_offset);
  if (mrisc32_is_small_offset (size))
    return size;

  /* TODO(m): What to do, what to do?  */
  gcc_unreachable ();
}

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

  /* Next are the callee-saved vector regs.  */
  /* TODO(m): Implement support for vector registers.  */
  frame->vreg_sp_offset = offset;  /* This is wrong, but unused anyway.  */

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
mrisc32_num_saved_regs (struct mrisc32_frame_info *frame)
{
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

/* Add a constant offset of any distance to a frame related pointer
   (usually the stack pointer or the frame pointer). This may or may
   not clobber the MRISC32_PROLOGUE_TEMP register.  */

static void
mrisc32_emit_frame_addi (rtx dst_rtx, rtx src_rtx, const int offset)
{
  rtx insn;
  rtx frame_pattern;

  if (mrisc32_is_imm14 (offset))
    {
      insn = gen_add3_insn (dst_rtx, src_rtx, GEN_INT (offset));
      RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
    }
  else
    {
      mrisc32_emit_move (MRISC32_PROLOGUE_TEMP (Pmode), GEN_INT (offset));
      emit_insn (gen_add3_insn (dst_rtx,
				src_rtx,
				MRISC32_PROLOGUE_TEMP (Pmode)));

      /* Describe the effect of the previous instructions.  */
      frame_pattern = plus_constant (Pmode, src_rtx, offset);
      frame_pattern = gen_rtx_SET (dst_rtx, frame_pattern);
      mrisc32_set_frame_expr (frame_pattern);
    }
}

void
mrisc32_expand_prologue (void)
{
  struct mrisc32_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT size = frame->total_size;
  int num_saved_regs;
  rtx insn;

  if (flag_stack_usage_info)
    current_function_static_stack_size = size;

  /* Save the registers.  */
  num_saved_regs = mrisc32_num_saved_regs (frame);
  if (num_saved_regs > 0)
    {
      HOST_WIDE_INT step1 = mrisc32_first_stack_step (frame);

      insn = gen_add3_insn (stack_pointer_rtx,
			    stack_pointer_rtx,
			    GEN_INT (-step1));
      RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
      size -= step1;

      /* Save the scalar registers. */
      HOST_WIDE_INT offset = frame->sreg_sp_offset - size;
      for (int regno = 31; regno >= 0; regno--)
	if (BITSET_P (frame->mask, regno))
	  {
	    rtx mem =
	      gen_frame_mem (word_mode, plus_constant (Pmode,
						       stack_pointer_rtx,
						       offset));
	    mrisc32_save_reg (gen_rtx_REG (word_mode, regno), mem);
	    offset -= UNITS_PER_WORD;
	  }
    }

  /* Set up the frame pointer, if we're using one.  */
  if (frame_pointer_needed)
    {
      insn = gen_add3_insn (hard_frame_pointer_rtx, stack_pointer_rtx,
			    GEN_INT (frame->hard_frame_pointer_offset - size));
      RTX_FRAME_RELATED_P (emit_insn (insn)) = 1;
    }

  /* Allocate the rest of the frame.  */
  if (size > 0)
    {
      mrisc32_emit_frame_addi (stack_pointer_rtx, stack_pointer_rtx, -size);
    }
}

void
mrisc32_expand_epilogue (void)
{
  struct mrisc32_frame_info *frame = &cfun->machine->frame;
  HOST_WIDE_INT step1 = frame->total_size;
  HOST_WIDE_INT step2 = 0;
  rtx insn;
  int num_saved_regs;

  /* Reset the epilogue cfa info before starting to emit the epilogue.  */
  epilogue_cfa_sp_offset = 0;

  /* Move past any dynamic stack allocations.  */
  if (cfun->calls_alloca)
    {
      rtx adjust = GEN_INT (-frame->hard_frame_pointer_offset);
      if (!mrisc32_is_small_offset (INTVAL (adjust)))
	{
	  mrisc32_emit_move (MRISC32_PROLOGUE_TEMP (Pmode), adjust);
	  adjust = MRISC32_PROLOGUE_TEMP (Pmode);
	}

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

  /* If we need to restore registers, deallocate as much stack as
     possible in the second step without going out of range.  */
  num_saved_regs = mrisc32_num_saved_regs (frame);
  if (num_saved_regs > 0)
    {
      step2 = mrisc32_first_stack_step (frame);
      step1 -= step2;
    }

  /* Set TARGET to BASE + STEP1.  */
  if (step1 > 0)
    {
      /* Get an rtx for STEP1 that we can add to BASE.  */
      rtx adjust = GEN_INT (step1);
      if (!mrisc32_is_small_offset (INTVAL (adjust)))
	{
	  mrisc32_emit_move (MRISC32_PROLOGUE_TEMP (Pmode), adjust);
	  adjust = MRISC32_PROLOGUE_TEMP (Pmode);
	}

      insn = emit_insn (
	       gen_add3_insn (stack_pointer_rtx, stack_pointer_rtx,
			      adjust));

      rtx dwarf = NULL_RTX;
      rtx cfa_adjust_rtx = gen_rtx_PLUS (Pmode, stack_pointer_rtx,
					 GEN_INT (step2));

      dwarf = alloc_reg_note (REG_CFA_DEF_CFA, cfa_adjust_rtx, dwarf);
      RTX_FRAME_RELATED_P (insn) = 1;

      REG_NOTES (insn) = dwarf;
    }
  else if (frame_pointer_needed)
    {
      /* Tell mrisc32_restore_reg to emit dwarf to redefine CFA when restoring
	 old value of FP.  */
      epilogue_cfa_sp_offset = step2;
    }

  /* Restore the registers.  */
  num_saved_regs = mrisc32_num_saved_regs (frame);
  if (num_saved_regs > 0)
    {
      HOST_WIDE_INT offset = frame->sreg_sp_offset -
			     (frame->total_size - step2) -
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
  if (step2 > 0)
    {
      insn = emit_insn (gen_add3_insn (stack_pointer_rtx, stack_pointer_rtx,
				       GEN_INT (step2)));

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

/* Worker function for TARGET_FUNCTION_OK_FOR_SIBCALL.  */

static bool
mrisc32_function_ok_for_sibcall (tree decl ATTRIBUTE_UNUSED,
				 tree exp ATTRIBUTE_UNUSED)
{
  /* Right now we allow sibcall for all call types. This may change in
     the future.  */
  return true;
}

/* Worker function for TARGET_WARN_FUNC_RETURN.  */

static bool
mrisc32_warn_func_return (tree decl ATTRIBUTE_UNUSED)
{
  /* True if a function’s return statements should be checked for
     matching the function’s return type.
     Right now we don't suppress warnings.  */
  return true;
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

/* Return true for valid memory register indirect addresses.  */

bool
mrisc32_register_address_p (rtx x, bool strict_p)
{
  if (!MEM_P (x))
    return false;

  x = XEXP (x, 0);

  if (REG_P (x)
      && mrisc32_reg_ok_for_base_p (x, strict_p))
    {
      return true;
    }

  return false;
}

/* Return true for valid memory offset addresses.  */

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
      && mrisc32_is_imm14 (INTVAL (XEXP (x, 1))))
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

/* Worker function for TARGET_LEGITIMATE_ADDRESS_P.  */

static bool
mrisc32_legitimate_address_p (machine_mode mode ATTRIBUTE_UNUSED,
			      rtx x, bool strict_p)
{
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
      && mrisc32_is_imm14 (INTVAL (XEXP (x, 1))))
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

/* Used by the mrisc32_*_movsrc_operand predicates.  */

bool
mrisc32_valid_memsrc_operand (rtx op)
{
  if (MEM_P (op))
    {
      machine_mode mode = GET_MODE (op);
      rtx x = XEXP (op, 0);
      bool strict = reload_in_progress || reload_completed;
      return mrisc32_legitimate_address_p (mode, x, strict);
    }

  return false;
}

/* Make ADDR suitable for use as a call or sibcall target.  */

rtx
mrisc32_legitimize_call_address (rtx addr)
{
  if (!mrisc32_call_insn_operand (addr, VOIDmode))
    {
      return copy_addr_to_reg (addr);
    }
  return addr;
}

/* If (set DEST SRC) is not a valid move instruction, emit an equivalent
   sequence that is valid.  */

bool
mrisc32_legitimize_move (machine_mode mode, rtx dest, rtx src)
{
  /* Both src and dest are non-registers;  one special case is supported where
     the source is (const_int 0) and the store can source the zero register.  */
  if (!register_operand (dest, mode)
      && !mrisc32_reg_or_zero_operand (src, mode)
      && !(reload_in_progress || reload_completed))
    {
      mrisc32_emit_move (dest, force_reg (mode, src));
      return true;
    }

  return false;
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

/* Check if a specific "ashift" + "and" operation can be implemented using
   MKBF. The operation is (X << OFFSET) & MASK.
   OFFSET_OP and MASK_OP must be be const_int operands.  */

bool
mrisc32_is_mask_suitable_for_mkbf (rtx offset_op, rtx mask_op)
{
  unsigned offset = UINTVAL (offset_op);
  unsigned mask = UINTVAL (mask_op) >> offset;
  if (offset >= 32 || mask == 0)
    return false;
  return mask == (0xffffffffU >> __builtin_clz (mask));
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

/* Convert MASK and OFFSET to a MKBF control word.  */

rtx
mrisc32_mask_to_mkbf_ctrl (rtx offset_op, rtx mask_op)
{
  unsigned offset = UINTVAL (offset_op);
  unsigned mask = UINTVAL (mask_op) >> offset;
  unsigned width = 32 - __builtin_clz (mask);
  unsigned ctrl = ((width & 31) << 5) | (offset & 31);
  return gen_int_mode (ctrl, SImode);
}

/* Implement TARGET_PROMOTE_FUNCTION_MODE */

/* This function is equivalent to default_promote_function_mode_always_promote
   except that it returns a promoted mode even if type is NULL_TREE.  This is
   needed by libcalls which have no type (only a mode) such as fixed conversion
   routines that take a signed or unsigned char/short argument and convert it
   to a fixed type.  */

static machine_mode
mrisc32_promote_function_mode (const_tree type ATTRIBUTE_UNUSED,
			       machine_mode mode,
			       int *punsignedp ATTRIBUTE_UNUSED,
			       const_tree fntype ATTRIBUTE_UNUSED,
			       int for_return ATTRIBUTE_UNUSED)
{
  int unsignedp;

  if (type != NULL_TREE)
    return promote_mode (type, mode, punsignedp);

  unsignedp = *punsignedp;
  PROMOTE_MODE (mode, unsignedp, type);
  *punsignedp = unsignedp;
  return mode;
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

#undef  TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P	mrisc32_legitimate_address_p
#undef  TARGET_LEGITIMATE_CONSTANT_P
#define TARGET_LEGITIMATE_CONSTANT_P	mrisc32_legitimate_constant_p

#undef  TARGET_SETUP_INCOMING_VARARGS
#define TARGET_SETUP_INCOMING_VARARGS 	mrisc32_setup_incoming_varargs

#undef  TARGET_PROMOTE_FUNCTION_MODE
#define TARGET_PROMOTE_FUNCTION_MODE mrisc32_promote_function_mode

/* Define this to return an RTX representing the place where a
   function returns or receives a value of data type RET_TYPE, a tree
   node representing a data type.  */
#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE mrisc32_function_value
#undef TARGET_LIBCALL_VALUE
#define TARGET_LIBCALL_VALUE mrisc32_libcall_value
#undef TARGET_FUNCTION_VALUE_REGNO_P
#define TARGET_FUNCTION_VALUE_REGNO_P mrisc32_function_value_regno_p

#undef TARGET_FUNCTION_OK_FOR_SIBCALL
#define TARGET_FUNCTION_OK_FOR_SIBCALL mrisc32_function_ok_for_sibcall

#undef TARGET_WARN_FUNC_RETURN
#define TARGET_WARN_FUNC_RETURN mrisc32_warn_func_return

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
