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


.data
L2: .asciz "hello"
L3: .asciz "world"
