
.globl	main
.p2align 4
main:
pushq	%r15
pushq	%r14
pushq	%r13
pushq	%r12
pushq	%rbx
pushq	%rbp
movq	%rsp, %rbp
movl 0imm, %%reg
movl %%reg, y
movl 0imm, %%reg
movl %%reg, x
movdqu z, %%reg
pblendw 0imm, %%reg, 4
movdqu %%reg, z
q: .long
movl 0imm, %%reg
movl %%reg, i
L0:
movl i, %%reg
cmpl 0imm, %%reg
setlt   %%reg
movq	%%reg, %r15
cmpq	$0, %r15
jz	L1
movl i, %%reg
addl 0imm, %%reg
movl %%reg, %%reg
movl %%reg, i
jmp	L0
L1:
movl x, %%reg
cmpl 0imm, %%reg
setgt   %%reg
movq   %%reg, %r15
cmpq   $0, %r15
jz    L3
movl y, %%reg
cmpl 0imm, %%reg
setlt   %%reg
movq   %%reg, %r15
cmpq   $0, %r15
jz    L4
movl x, %%reg
cmpl 0imm, %%reg
setgt   %%reg
movl %%reg, %%reg
movl y, %%reg
cmpl 0imm, %%reg
setgt   %%reg
andl %%reg, %%reg
cmpl $0, %%reg
setne   %%reg
movq   %%%reg, %r15
cmpq   $0, %r15
jz    L7
movb	0imm, %%rax
movq	%rbp, %rsp
popq	%rbp
popq	%rbx
popq	%r12
popq	%r13
popq	%r14
popq	%r15
jmp   8
L7:
movb	0imm, %%rax
movq	%rbp, %rsp
popq	%rbp
popq	%rbx
popq	%r12
popq	%r13
popq	%r14
popq	%r15
L8:
L4:
L3:
movdqu q, %%reg
movq	z, %r15
movdqu	(%r15), %%reg
movdqa %%reg, %%reg
movq	0imm, %r15
movb	(%r15), %%reg
paddl %%reg, %%reg
movdqa %%reg, %%reg
paddl q, %%reg
pblendw %%reg, %%reg, 4
movdqu %%reg, q
movl x, %%reg
movq	z, %r15
movdqu	(%r15), %%reg
pextrl %%reg, %%reg, 3
addl %%reg, %%reg
movl %%reg, %%reg
movl %%reg, x
movl x, %%reg
addl y, %%reg
movl	%%reg, %%rax
movq	%rbp, %rsp
popq	%rbp
popq	%rbx
popq	%r12
popq	%r13
popq	%r14
popq	%r15
