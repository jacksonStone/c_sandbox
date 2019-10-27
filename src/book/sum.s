	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 14	sdk_version 10, 14
	.globl	_sum                    ## -- Begin function sum
	.p2align	4, 0x90
_sum:                                   ## @sum
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	addsd	%xmm1, %xmm0
	movsd	_accum(%rip), %xmm1     ## xmm1 = mem[0],zero
	addsd	%xmm0, %xmm1
	movsd	%xmm1, _accum(%rip)
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_accum                  ## @accum
.zerofill __DATA,__common,_accum,8,3

.subsections_via_symbols
