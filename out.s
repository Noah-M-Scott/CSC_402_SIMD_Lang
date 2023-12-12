#	Handling Scope Node
#	Handling Equ Node

.data
buzz: .long 4

.text
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
movdqa	%xmm0, %xmm10
#	Handling Scope Node
#	Handling Equ Node
movl	$7, %reg
movl	%reg, y(%rip)
#	Handling Equ Node
movl	$5, %reg
movl	%reg, x(%rip)
#	Handling Equ Node
movq	$x, %reg
movq	%reg, px(%rip)
#	Handling Equ Node
movq	$L0, %reg
movq	%reg, string(%rip)
#	Handling Equ Node
movdqu	z(%rip), %reg
pblendw	4, $L1, %reg
movdqu	%reg, z(%rip)

.data
q:	.long
#	Handling For Node
#	Handling Equ Node
movl	$0, %reg
movl	%reg, i(%rip)
L2:
#	Handling Lt Node
movl	i(%rip), %reg
cmpl	$45, %reg
setlt	%reg
movq	%reg, %r15
cmpq	$0, %r15
jz	L3
#	Handling Scope Node
#	Handling Equ Node
#	Handling Add Node
movl	x(%rip), %reg
addl	$1, %reg
movl	%reg, %reg
movl	%reg, x(%rip)
#	Handling Equ Node
#	Handling Add Node
movl	i(%rip), %reg
addl	$1, %reg
movl	%reg, %reg
movl	%reg, i(%rip)
jmp	L2
L3:
#	Handling Label Node
label:
#	Handling If Node
#	Handling GtEqu Node
movl	x(%rip), %reg
cmpl	$0, %reg
setgt   %reg
movq	%reg, %r15
cmpq	$0, %r15
jz	L5
#	Handling If Node
#	Handling Lt Node
movl	y(%rip), %reg
cmpl	$4, %reg
setlt	%reg
movq	%reg, %r15
cmpq	$0, %r15
jz	L6
#	Handling IfElse Node
#	Handling LogicalAnd Node
#	Handling GtEqu Node
movl	x(%rip), %reg
cmpl	$0, %reg
setgt   %reg
movl	%reg, %reg
#	Handling GtEqu Node
movl	y(%rip), %reg
cmpl	$0, %reg
setgt   %reg
andl	%reg, %reg
cmpl	$0, %reg
setne	%reg
movq	%reg, %r15
cmpq	$0, %r15
jz	L9
#	Handling RetExp Node
movb	$3, %rax
movq	%rbp, %rsp
popq	%rbp
popq	%rbx
popq	%r12
popq	%r13
popq	%r14
popq	%r15
jmp	L10
L9:
#	Handling Goto Node
jmp	label
L10:
L6:
L5:
#	Handling Equ Node
movdqu	q(%rip), %reg
#	Handling Add Node
#	Handling Add Node
#	Handling Deref Node
movq	z(%rip), %r15
movdqu	(%r15), %reg
movdqa	%reg, %reg
#	Handling Deref Node
movq	$L11, %r15
movb	(%r15), %reg
paddl	%reg, %reg
movdqa	%reg, %reg
paddl	q(%rip), %reg
pblendw	4, %reg, %reg
movdqu	%reg, q(%rip)
#	Handling Equ Node
#	Handling Add Node
movl	x(%rip), %reg
#	Handling VecIndex Node
#	Handling Deref Node
movq	z(%rip), %r15
movdqu	(%r15), %reg
pextrl 3, %reg, %reg
addl	%reg, %reg
movl	%reg, %reg
movl	%reg, x(%rip)
#	Handling RetExp Node
#	Handling Add Node
movl	x(%rip), %reg
addl	y(%rip), %reg
movl	%reg, %rax
movq	%rbp, %rsp
popq	%rbp
popq	%rbx
popq	%r12
popq	%r13
popq	%r14
popq	%r15


.data
L0: .asciz "funny"
L1: .(null) 4, 3, 2, 1
L11: .(null) 1, 1, 1, 1
