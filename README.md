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

## Usage

    rous pacman -S neovim
    rous systemctl restart sshd
    rous -u nobody id

Without -u the command runs as root. With -u it runs as the named user, which is handy for dropping privilege from a root session. The command is looked up in PATH, the environment is rebuilt so the child does not inherit the caller's HOME or USER, and the target identity is set with raw syscalls before the exec.

Options:

    -u, --user <name>  run as this user instead of root
    -h, --help         show this help
    -V, --version      show version
