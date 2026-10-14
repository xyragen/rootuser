# Changelog

## 0.1 - 2026-10-13

Initial release.

rous runs a command as the root user. The command line, trusted path search, environment rebuild, and user lookup live in C in main.c and rous.c. The identity change and exec live in assembly in syscall.S and talk to the kernel through raw syscalls, so the core has no libc dependency.

Every privilege changing syscall, setgroups, setgid, and setuid, is checked. On a negative result rous returns the errno and never reaches execve, so a rejected identity change can never run the command with the wrong privileges. An authorization policy in ru_authorized gates use on the real uid: root is always allowed, other users must be in the admin group, ROUS_ADMIN_GROUP, which defaults to wheel. The command is resolved on a fixed trusted path instead of the caller's PATH, and overlong path components are rejected rather than truncated.

A -u flag runs the command as another user, loading that account's supplementary groups and naming it in HOME, USER, and LOGNAME. Help and version flags are supported. rous refuses to run when it is not installed setuid root.


