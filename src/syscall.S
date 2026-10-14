	.text
	.p2align 4

	.globl ru_geteuid
	.type ru_geteuid, @function
ru_geteuid:
	mov $107, %eax
	syscall
	ret
	.size ru_geteuid, .-ru_geteuid

	.globl ru_run
	.type ru_run, @function
ru_run:
	push %rdi
	push %rsi
	push %rdx
	push %rcx
	push %r8

	xor %edi, %edi
	xor %esi, %esi
	mov $116, %eax
	syscall

	mov (%rsp), %rdi
	mov $106, %eax
	syscall

	mov 8(%rsp), %rdi
	mov $105, %eax
	syscall

	pop %r8
	pop %rcx
	pop %rdx
	pop %rsi
	pop %rdi
	mov $59, %eax
	syscall

	neg %rax
	ret
	.size ru_run, .-ru_run

	.section .note.GNU-stack,"",@progbits

