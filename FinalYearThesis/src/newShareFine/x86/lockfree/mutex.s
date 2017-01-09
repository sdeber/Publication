	.file	"mutex.c"
	.comm	ctemp,16,16
	.local	tempMutex
	.comm	tempMutex,16,16
	.local	tempThreads
	.comm	tempThreads,16,16
	.text
.globl GetMutex
	.type	GetMutex, @function
GetMutex:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$72, %rsp
	movq	%rdi, -72(%rbp)
	movq	local_self@GOTTPOFF(%rip), %rax
	movq	%fs:(%rax), %rax
	movl	8(%rax), %eax
	movl	%eax, -20(%rbp)
	movq	-72(%rbp), %rax
	addq	$8, %rax
	movl	$1, %esi
	movq	%rax, %rdi
	.cfi_offset 3, -24
	call	FetchAndAdd
	movq	-72(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	Swap
	testq	%rax, %rax
	je	.L2
	movq	-72(%rbp), %rax
	addq	$8, %rax
	movq	$-1, %rsi
	movq	%rax, %rdi
	call	FetchAndAdd
	jmp	.L7
.L2:
	movl	-20(%rbp), %eax
	movslq	%eax,%rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	sched+160(%rax), %rax
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rax
	movq	%rax, %rbx
#APP
# 21 "mutex.c" 1
	movl %esp,(%rbx)
	movl %ebp,4(%rbx)
	movl $0f,8(%rbx)
	movl $0,%eax
	0:
# 0 "" 2
#NO_APP
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	testl	%eax, %eax
	jne	.L7
	movl	-20(%rbp), %ecx
	movl	-20(%rbp), %eax
	movslq	%eax,%rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	sched+160(%rax), %rdx
	movslq	%ecx,%rax
	movq	%rdx, tempThreads(,%rax,8)
	movl	-20(%rbp), %eax
	cltq
	movq	-72(%rbp), %rdx
	movq	%rdx, tempMutex(,%rax,8)
	movl	-20(%rbp), %eax
	movslq	%eax,%rdx
	movq	%rdx, %rax
	addq	%rax, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	addq	$sched+296, %rax
	movq	%rax, %rbx
#APP
# 25 "mutex.c" 1
	movl (%rbx),%esp;movl 4(%rbx),%ebp
# 0 "" 2
#NO_APP
	movq	local_self@GOTTPOFF(%rip), %rax
	movq	%fs:(%rax), %rax
	movl	8(%rax), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	cltq
	movq	tempMutex(,%rax,8), %rax
	movq	16(%rax), %rdx
	movl	-20(%rbp), %eax
	cltq
	movq	tempThreads(,%rax,8), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	lockfree_fifo_enqueue
	movl	-20(%rbp), %edx
	movq	sched+80(%rip), %rax
#APP
# 32 "mutex.c" 1
	1:  lock; btsl $0,(%rax);jnc 2f;jmp 1b; 2:  
# 0 "" 2
#NO_APP
	movq	sched(%rip), %rax
	movq	%rax, -48(%rbp)
	movq	-48(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -56(%rbp)
	cmpq	$0, -56(%rbp)
	jne	.L4
	movq	sched+80(%rip), %rax
#APP
# 32 "mutex.c" 1
	lock;btrl $0,(%rax)
# 0 "" 2
#NO_APP
	movq	$0, -64(%rbp)
	jmp	.L5
.L4:
	movq	-56(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -64(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, sched(%rip)
	movq	sched+80(%rip), %rax
#APP
# 32 "mutex.c" 1
	lock;btrl $0,(%rax)
# 0 "" 2
#NO_APP
	movq	-64(%rbp), %rax
	movq	-48(%rbp), %rcx
	movq	%rcx, 40(%rax)
.L5:
	movq	-64(%rbp), %rbx
	movslq	%edx,%rcx
	movq	%rcx, %rax
	salq	$2, %rax
	addq	%rcx, %rax
	salq	$3, %rax
	movq	%rbx, sched+160(%rax)
	movslq	%edx,%rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	sched+160(%rax), %rax
	testq	%rax, %rax
	je	.L6
	movl	-20(%rbp), %eax
	movslq	%eax,%rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	salq	$3, %rax
	movq	sched+160(%rax), %rax
	movq	%rax, %rbx
#APP
# 33 "mutex.c" 1
	movl $1,%eax; movl (%rbx),%esp
	;movl 4(%rbx),%ebp
	;jmp *8(%rbx)
# 0 "" 2
#NO_APP
	jmp	.L7
.L6:
#APP
# 36 "mutex.c" 1
	1:  lock; btsl $0,sched+352(%rip);jnc 2f;jmp 1b; 2:  
# 0 "" 2
#NO_APP
	movl	sched+344(%rip), %eax
	addl	$1, %eax
	movl	%eax, sched+344(%rip)
#APP
# 38 "mutex.c" 1
	lock;btrl $0,sched+352(%rip)
# 0 "" 2
#NO_APP
	movl	-20(%rbp), %eax
	movl	%eax, %edi
	call	kernelthread_wait
.L7:
	addq	$72, %rsp
	popq	%rbx
	leave
	ret
	.cfi_endproc
.LFE0:
	.size	GetMutex, .-GetMutex
.globl ReleaseMutex
	.type	ReleaseMutex, @function
ReleaseMutex:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	8(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	$0, -16(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.L9
	movq	-8(%rbp), %rdx
	movq	-24(%rbp), %rax
	movl	$0, %r8d
	movl	$1, %ecx
	movl	$0, %esi
	movq	%rax, %rdi
	call	CAS2
	testb	%al, %al
	jne	.L12
.L9:
	movq	-24(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, %rdi
	call	lockfree_fifo_dequeue
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	je	.L9
	movq	-24(%rbp), %rax
	addq	$8, %rax
	movq	$-1, %rsi
	movq	%rax, %rdi
	call	FetchAndAdd
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	scheduler_in
	jmp	.L11
.L12:
	nop
.L11:
	leave
	ret
	.cfi_endproc
.LFE1:
	.size	ReleaseMutex, .-ReleaseMutex
.globl cmutex_init
	.type	cmutex_init, @function
cmutex_init:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	movq	%rsp, %rbp
	.cfi_offset 6, -16
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	$1, (%rax)
	movq	-8(%rbp), %rax
	movq	$0, 8(%rax)
	movl	$32, %edi
	call	malloc
	movq	%rax, %rdx
	movq	-8(%rbp), %rax
	movq	%rdx, 16(%rax)
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	testq	%rax, %rax
	jne	.L14
	movl	$-1, %eax
	jmp	.L15
.L14:
	movq	-8(%rbp), %rax
	movq	16(%rax), %rax
	movq	%rax, %rdi
	call	lockfree_fifo_init
	movl	$0, %eax
.L15:
	leave
	ret
	.cfi_endproc
.LFE2:
	.size	cmutex_init, .-cmutex_init
	.ident	"GCC: (Ubuntu 4.4.1-4ubuntu8) 4.4.1"
	.section	.note.GNU-stack,"",@progbits
