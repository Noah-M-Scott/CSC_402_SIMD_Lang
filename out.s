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
subq	$4, %rsp
subq	$4, %rsp
movl	-148(%rbp), %esi
movq	%esi, %r15
cmpq	$0, %r15
jz	L0
#	Handling RetExp Node
movq	$L2, %rax
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
movq	$L3, %rax
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
.globl	bar
.p2align 4
bar:
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
subq	$16, %rsp
subq	$16, %rsp
movdqu	-312(%rbp), %xmm1
movdqu	-296(%rip), %xmm1
pblendw	3, $L4, %xmm1
movdqu	%xmm1, -296(%rip)
subq	$16, %rsp
#	Handling Equ Node
subq	$16, %rsp
subq	$16, %rsp
movdqu	-360(%rbp), %xmm2
movdqu	-328(%rip), %xmm2
#	Handling Deref Node
movq	-296(%rip), %r15
subq	$16, %rsp
movdqu	-376(%rbp), %xmm3
movdqu	(%r15), %xmm3
pblendw	63, %xmm3, %xmm2
movdqu	%xmm2, -328(%rip)
#	Handling For Node
#	Handling Equ Node
subq	$4, %rsp
subq	$4, %rsp
movl	-396(%rbp), %r8d
movl	$0, %r8d
movl	%r8d, -392(%rip)
L5:
#	Handling Lt Node
subq	$4, %rsp
movl	-400(%rbp), %r9d
movl	-392(%rip), %r9d
cmpl	0(%rip), %r9d
setlt	%reg
movq	%r9d, %r15
cmpq	$0, %r15
jz	L6
#	Handling Equ Node
subq	$16, %rsp
subq	$16, %rsp
movdqu	-420(%rbp), %xmm6
movdqu	-328(%rip), %xmm6
#	Handling Add Node
subq	$16, %rsp
movdqu	-436(%rbp), %xmm7
movdqu	-328(%rip), %xmm7
#	Handling Deref Node
movq	-296(%rip), %r15
subq	$16, %rsp
movdqu	-452(%rbp), %xmm8
movdqu	(%r15), %xmm8
paddl	%xmm8, %xmm7
pblendw	63, %xmm7, %xmm6
movdqu	%xmm6, -328(%rip)
#	Handling Equ Node
subq	$4, %rsp
#	Handling Add Node
subq	$4, %rsp
movl	-472(%rbp), %ebx
movl	-392(%rip), %ebx
addl	$1, %ebx
subq	$4, %rsp
movl	-476(%rbp), %edi
movl	%ebx, %edi
movl	%edi, -392(%rip)
jmp	L5
L6:
#	Handling RetExp Node
#	Handling Add Node
#	Handling Add Node
#	Handling VecIndex Node
#	Handling Deref Node
movq	-296(%rip), %r15
subq	$16, %rsp
movdqu	%xmm1, -312(%rbp)
movdqu	-480(%rbp), %xmm1
movdqu	(%r15), %xmm1
subq	$4, %rsp
movl	-496(%rbp), %edx
pextrl 0, %xmm1, %edx
subq	$4, %rsp
movl	-500(%rbp), %ecx
movl	%edx, %ecx
#	Handling VecIndex Node
#	Handling Deref Node
movq	-296(%rip), %r15
movdqu	(%r15), %xmm1
subq	$4, %rsp
movl	%r8d, -396(%rbp)
movl	-504(%rbp), %r8d
pextrl 1, %xmm1, %r8d
addl	%r8d, %ecx
subq	$4, %rsp
movl	%r9d, -400(%rbp)
movl	-508(%rbp), %r9d
movl	%ecx, %r9d
#	Handling VecIndex Node
#	Handling Deref Node
movq	-296(%rip), %r15
movdqu	(%r15), %xmm1
subq	$4, %rsp
movl	-512(%rbp), %eax
pextrl 2, %xmm1, %eax
addl	%eax, %r9d
movl	%r9d, %eax
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
L4: .(null) 1, 1, 1
