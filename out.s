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
#	Handling Equ Node
subq	$8, %rsp
subq	$8, %rsp
movq	-304(%rbp), %rsi
leaq	L4(%rip), %r14
movq	%r14, %rsi
movq	%rsi, -296(%rbp)
#	Handling Equ Node
subq	$16, %rsp
subq	$16, %rsp
movdqu	-328(%rbp), %xmm2
movdqu	-312(%rbp), %xmm15
movdqu	%xmm15, %xmm2
#	Handling Deref Node
movq	-296(%rbp), %r15
subq	$16, %rsp
movdqu	-344(%rbp), %xmm3
movdqu	(%r15), %xmm3
pblendw	$255, %xmm3, %xmm2
movdqu	%xmm2, -312(%rbp)
#	Handling For Node
#	Handling Equ Node
subq	$8, %rsp
subq	$8, %rsp
movq	-368(%rbp), %r8
movq	$0, %r8
movq	%r8, -360(%rbp)
L5:
#	Handling Lt Node
subq	$8, %rsp
movq	-376(%rbp), %r9
movq	-360(%rbp), %r9
xorq	%r15, %r15
cmpq	-8(%rbp), %r9
setl	%r15b
movq	%r15, %r9
xorq	%r15, %r15
movq	%r9, %r15
cmpq	$0, %r15
je	L6
#	Handling Equ Node
subq	$16, %rsp
subq	$16, %rsp
movdqu	-400(%rbp), %xmm6
movdqu	-312(%rbp), %xmm15
movdqu	%xmm15, %xmm6
#	Handling Add Node
subq	$16, %rsp
movdqu	-416(%rbp), %xmm7
movdqu	-312(%rbp), %xmm15
movdqu	%xmm15, %xmm7
#	Handling Deref Node
movq	-296(%rbp), %r15
subq	$16, %rsp
movdqu	-432(%rbp), %xmm8
movdqu	(%r15), %xmm8
addps	%xmm8, %xmm7
pblendw	$255, %xmm7, %xmm6
movdqu	%xmm6, -312(%rbp)
#	Handling Equ Node
subq	$8, %rsp
#	Handling Add Node
subq	$8, %rsp
movq	-456(%rbp), %rbx
movq	-360(%rbp), %rbx
addq	$1, %rbx
subq	$8, %rsp
movq	-464(%rbp), %rdi
movq	%rbx, %rdi
movq	%rdi, -360(%rbp)
jmp	L5
L6:
#	Handling RetExp Node
#	Handling Add Node
#	Handling Add Node
#	Handling Add Node
#	Handling VecIndex Node
subq	$4, %rsp
movdqu	-472(%rbp), %xmm1
movdqu	-312(%rbp), %xmm15
pshufd	$0, %xmm15, %xmm1
subq	$4, %rsp
movdqu	%xmm2, -328(%rbp)
movdqu	-476(%rbp), %xmm2
movss	%xmm1, %xmm2
#	Handling VecIndex Node
subq	$4, %rsp
movdqu	%xmm3, -344(%rbp)
movdqu	-480(%rbp), %xmm3
movdqu	-312(%rbp), %xmm15
pshufd	$1, %xmm15, %xmm3
addss	%xmm3, %xmm2
subq	$4, %rsp
movdqu	-484(%rbp), %xmm4
movss	%xmm2, %xmm4
#	Handling VecIndex Node
subq	$4, %rsp
movdqu	-488(%rbp), %xmm5
movdqu	-312(%rbp), %xmm15
pshufd	$2, %xmm15, %xmm5
addss	%xmm5, %xmm4
subq	$4, %rsp
movdqu	%xmm6, -400(%rbp)
movdqu	-492(%rbp), %xmm6
movss	%xmm4, %xmm6
#	Handling VecIndex Node
subq	$4, %rsp
movdqu	%xmm7, -416(%rbp)
movdqu	-496(%rbp), %xmm7
movdqu	-312(%rbp), %xmm15
pshufd	$3, %xmm15, %xmm7
addss	%xmm7, %xmm6
movss	%xmm6, %xmm0
movq	%rbp, %rsp
popq	%rbp
popq	%rbx
popq	%r12
popq	%r13
popq	%r14
popq	%r15
ret


.data
L2: .asciz "hello"
L3: .asciz "world"
L4: .long 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000
