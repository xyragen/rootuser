# rootuser

rootuser (rous) is a simple and lightweight utility for running commands as the root user written in C and Assembly with 0 dependencies.

## Build

Run make. Needs a C compiler and an assembler, nothing else.

    make

One binary lands in build, called rous. Debug builds use `make debug`.

## Install

Install setuid root so an allowed user can raise to root without a password.

    sudo make install

This puts rous in /usr/local/sbin owned by root with mode 4755. Point SBINDIR elsewhere to change the location.

## Security model

rous is setuid root, so it must decide who may use it before it touches any identity. The policy lives in ru_authorized and is checked against the real uid, independently of the command that is about to run, so it cannot be bypassed through the command path.

- uid 0 is always allowed.
- any other user must be a member of the admin group, ROUS_ADMIN_GROUP, which defaults to wheel.
- everyone else is refused.

Change the group at build time if wheel is not what you want:

    make CFLAGS="-DROUS_ADMIN_GROUP=\"admin\""

The identity change itself is done with raw syscalls in assembly. Every privilege changing syscall, setgroups, setgid, and setuid, is checked. On a negative result rous returns the errno and never reaches execve, so a rejected identity change can never run the command with the wrong privileges. Supplementary groups are loaded for the target account rather than cleared, and the command is resolved against a fixed trusted path instead of the caller's PATH.

Because of all this, do not deploy rous on a multi user system until you have reviewed the admin group and tested the policy in a disposable VM first.

## Usage

    rous pacman -S neovim
    rous systemctl restart sshd
    rous -u nobody id

Without -u the command runs as root. With -u it runs as the named user, which is handy for dropping privilege from a root session. The command is looked up on a fixed trusted path, the environment is rebuilt to name the target account in HOME, USER, and LOGNAME, the target's supplementary groups are loaded, and the identity is set with raw syscalls before the exec.

Options:

    -u, --user <name>  run as this user instead of root
    -h, --help         show this help
    -V, --version      show version