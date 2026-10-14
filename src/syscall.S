	.text
	.p2align 4

	.globl ru_geteuid
	.type ru_geteuid, @function
ru_geteuid:
	mov $107, %eax
	syscall
	ret
	.size ru_geteuid, .-ru_geteuid

	.globl ru_getuid
	.type ru_getuid, @function
ru_getuid:
	mov $102, %eax
	syscall
	ret
	.size ru_getuid, .-ru_getuid

/*
 * long ru_run(const char *path, char *argv[], char *envp[], const ru_ids_t *ids)
 *
 *   rdi = path
 *   rsi = argv
 *   rdx = envp
 *   rcx = ids -> { grouplist@0, uid@8, gid@12, ngroups@16 }
 *
 * Every privilege changing syscall is checked. The Linux syscall ABI
 * returns -errno in rax on failure, so a negative rax means the call did
 * not take effect. On any failure we restore the callee saved registers
 * and return that negative errno without ever reaching execve, so a
 * dropped or rejected identity change can never fall through to running
 * the command with the wrong privileges.
 *
 * Order matters: groups are cleared and reset while we still hold
 * privilege, then gid, then uid last, so there is no window where we
 * have lost the power to change the remaining ids.
 */
	.globl ru_run
	.type ru_run, @function
ru_run:
	push %r12
	push %r13
	push %r14
	push %r15

	mov %rdi, %r12
	mov %rsi, %r13
	mov %rdx, %r14
	mov %rcx, %r15

	mov 16(%r15), %rdi
	mov 0(%r15), %rsi
	mov $116, %eax
	syscall
	test %rax, %rax
	js .Lru_fail

	mov 12(%r15), %rdi
	mov $106, %eax
	syscall
	test %rax, %rax
	js .Lru_fail

	mov 8(%r15), %rdi
	mov $105, %eax
	syscall
	test %rax, %rax
	js .Lru_fail

	mov %r12, %rdi
	mov %r13, %rsi
	mov %r14, %rdx
	mov $59, %eax
	syscall

.Lru_fail:
	pop %r15
	pop %r14
	pop %r13
	pop %r12
	ret
	.size ru_run, .-ru_run

	.section .note.GNU-stack,"",@progbits

