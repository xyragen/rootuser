# Changelog

## 0.1 - 2026-10-13

Initial release.

rous runs a command as the root user. The command line, PATH search, environment rebuild, and user lookup live in C in main.c. The identity change and exec live in assembly in syscall.S and talk to the kernel through raw syscalls, so the core has no libc dependency. It clears the supplementary groups, sets the real and effective gid and uid, then execs the resolved program. A -u flag runs the command as another user instead of root, and help and version flags are supported. rous refuses to run when it is not installed setuid root rather than silently doing nothing useful.
