#	Handling Scope Node

.text
.globl	hello
.p2align 4
hello:
pushq	%r15
pushq	%r14
pushq	%r13
pushq	%r12
pushq	%rbx
pushq	%rbp
movq	%rsp, %rbp
movdqa	%xmm0, %xmm10
#	Placing Parameters on stack
pushq	%rdi
pushq	%rsi
pushq	%rdx
pushq	%rcx
pushq	%r8
pushq	%r9
subq	$16, %rsp
movdqu	%xmm10, (%rsp)
subq	$16, %rsp
movdqu	%xmm1, (%rsp)
subq	$16, %rsp
movdqu	%xmm2, (%rsp)
subq	$16, %rsp
movdqu	%xmm3, (%rsp)
subq	$16, %rsp
movdqu	%xmm4, (%rsp)
subq	$16, %rsp
movdqu	%xmm5, (%rsp)
#	Done
#	Handling Scope Node
#	Handling IfElse Node
subq	$8, %rsp
xorq	%r15, %r15
movq	-8(%rbp), %r15
cmpq	$0, %r15
je	L0
#	Handling RetExp Node
leaq	L2(%rip), %r14
movq	%r14, %rax
movq	%rbp, %rsp
popq	%rbp
popq	%rbx
popq	%r12
popq	%r13
popq	%r14
popq	%r15
ret
jmp	L1
L0:
#	Handling RetExp Node
leaq	L3(%rip), %r14
movq	%r14, %rax
movq	%rbp, %rsp
popq	%rbp
popq	%rbx
popq	%r12
popq	%r13
popq	%r14
popq	%r15
ret
L1:

.text
.globl	foo
.p2align 4
foo:
pushq	%r15
pushq	%r14
pushq	%r13
pushq	%r12
pushq	%rbx
pushq	%rbp
movq	%rsp, %rbp
movdqa	%xmm0, %xmm10
#	Placing Parameters on stack
pushq	%rdi
pushq	%rsi
pushq	%rdx
pushq	%rcx
pushq	%r8
pushq	%r9
subq	$16, %rsp
movdqu	%xmm10, (%rsp)
subq	$16, %rsp
movdqu	%xmm1, (%rsp)
subq	$16, %rsp
movdqu	%xmm2, (%rsp)
subq	$16, %rsp
movdqu	%xmm3, (%rsp)
subq	$16, %rsp
movdqu	%xmm4, (%rsp)
subq	$16, %rsp
movdqu	%xmm5, (%rsp)
#	Done
#	Handling Scope Node
subq	$8, %rsp
subq	$16, %rsp
#	Handling Equ Node
subq	$8, %rsp
subq	$8, %rsp
movq	-328(%rbp), %rsi
movq	L4(%rip), %rsi
movq	%rsi, -296(%rip)
#	Handling For Node
#	Handling Equ Node
subq	$4, %rsp
subq	$4, %rsp
movl	-340(%rbp), %edx
movl	$0, %edx
movl	%edx, -336(%rip)
L5:
#	Handling Lt Node
subq	$4, %rsp
movl	-344(%rbp), %ecx
movl	-336(%rip), %ecx
cmpl	-8(%rbp), %ecx
setlt	%reg
xorq	%r15, %r15
movl	%ecx, %r15d
cmpq	$0, %r15
je	L6
#	Handling Equ Node
subq	$16, %rsp
subq	$16, %rsp
movdqu	-364(%rbp), %xmm4
movdqu	-304(%rip), %xmm4
#	Handling Add Node
#	Handling Deref Node
movq	-296(%rip), %r15
subq	$16, %rsp
movdqu	-380(%rbp), %xmm5
movdqu	(%r15), %xmm5
subq	$16, %rsp
movdqu	-396(%rbp), %xmm6
movdqa	%xmm5, %xmm6
#	Handling Deref Node
movq	-296(%rip), %r15
movdqu	(%r15), %xmm5
paddq	%xmm5, %xmm6
pblendw	$255, %xmm6, %xmm4
movdqu	%xmm4, -304(%rip)
#	Handling Equ Node
subq	$4, %rsp
#	Handling Add Node
subq	$4, %rsp
movl	-416(%rbp), %r10d
movl	-336(%rip), %r10d
addl	$1, %r10d
subq	$4, %rsp
movl	-420(%rbp), %r11d
movl	%r10d, %r11d
movl	%r11d, -336(%rip)
jmp	L5
L6:


.data
L2: .asciz "hello"
L3: .asciz "world"
L4: .quad 1, 1
