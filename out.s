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
#	Handling Scope Node
#	Handling Equ Node
movl	7, %reg
movl	%reg, y
#	Handling Equ Node
movl	5, %reg
movl	%reg, x
#	Handling Equ Node
movq	$x, %reg
movq	%reg, px
#	Handling Equ Node
movdqu	z, %reg
pblendw	4, lit0, %reg
movdqu	%reg, z

.data
q:	.long
#	Handling For Node
#	Handling Equ Node
movl	0, %reg
movl	%reg, i
L0:
#	Handling Lt Node
movl	i, %reg
cmpl	45, %reg
setlt	%reg
movq	%reg, %r15
cmpq	$0, %r15
jz	L1
#	Handling Scope Node
#	Handling Equ Node
#	Handling Add Node
movl	x, %reg
addl	1, %reg
movl	%reg, %reg
movl	%reg, x
#	Handling Equ Node
#	Handling Add Node
movl	i, %reg
addl	1, %reg
movl	%reg, %reg
movl	%reg, i
jmp	L0
L1:
#	Handling Label Node
label:
#	Handling If Node
#	Handling GtEqu Node
movl	x, %reg
cmpl	0, %reg
setgt   %reg
movq	%reg, %r15
cmpq	$0, %r15
jz	L3
#	Handling If Node
#	Handling Lt Node
movl	y, %reg
cmpl	4, %reg
setlt	%reg
movq	%reg, %r15
cmpq	$0, %r15
jz	L4
#	Handling IfElse Node
#	Handling LogicalAnd Node
#	Handling GtEqu Node
movl	x, %reg
cmpl	0, %reg
setgt   %reg
movl	%reg, %reg
#	Handling GtEqu Node
movl	y, %reg
cmpl	0, %reg
setgt   %reg
andl	%reg, %reg
cmpl	$0, %reg
setne	%reg
movq	%reg, %r15
cmpq	$0, %r15
jz	L7
#	Handling RetExp Node
movb	3, %rax
movq	%rbp, %rsp
popq	%rbp
popq	%rbx
popq	%r12
popq	%r13
popq	%r14
popq	%r15
jmp	L8
L7:
#	Handling Goto Node
jmp	label
L8:
L4:
L3:
#	Handling Equ Node
movdqu	q, %reg
#	Handling Add Node
#	Handling Add Node
#	Handling Deref Node
movq	z, %r15
movdqu	(%r15), %reg
movdqa	%reg, %reg
#	Handling Deref Node
movq	lit1, %r15
movb	(%r15), %reg
paddl	%reg, %reg
movdqa	%reg, %reg
paddl	q, %reg
pblendw	4, %reg, %reg
movdqu	%reg, q
#	Handling Equ Node
#	Handling Add Node
movl	x, %reg
#	Handling VecIndex Node
#	Handling Deref Node
movq	z, %r15
movdqu	(%r15), %reg
pextrl 3, %reg, %reg
addl	%reg, %reg
movl	%reg, %reg
movl	%reg, x
#	Handling RetExp Node
#	Handling Add Node
movl	x, %reg
addl	y, %reg
movl	%reg, %rax
movq	%rbp, %rsp
popq	%rbp
popq	%rbx
popq	%r12
popq	%r13
popq	%r14
popq	%r15
