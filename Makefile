CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Werror -std=c11 -Iinclude -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE
ASFLAGS ?=
DBGFLAGS = -O0 -g -Wall -Wextra -std=c11 -Iinclude -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
SBINDIR = $(PREFIX)/sbin
OBJDIR = build/obj
LIB_OBJ = $(OBJDIR)/syscall.o $(OBJDIR)/rous.o
ROUS_OBJ = $(LIB_OBJ) $(OBJDIR)/main.o
all: build/rous
build $(OBJDIR):
	mkdir -p build $(OBJDIR)
$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJDIR)/syscall.o: src/syscall.S | $(OBJDIR)
	$(CC) $(CFLAGS) $(ASFLAGS) -c $< -o $@
build/rous: $(ROUS_OBJ) | build
	$(CC) $(CFLAGS) $(ROUS_OBJ) -o $@
debug: | build $(OBJDIR)
	$(CC) $(DBGFLAGS) -c src/syscall.S -o $(OBJDIR)/syscall.o
	$(CC) $(DBGFLAGS) -c src/rous.c -o $(OBJDIR)/rous.o
	$(CC) $(DBGFLAGS) -c src/main.c -o $(OBJDIR)/main.o
	$(CC) $(DBGFLAGS) $(ROUS_OBJ) -o build/rous
test: build/rous build/test_rouspath build/test_ruenv
	./build/test_rouspath
	./build/test_ruenv
	sh tests/run.sh
build/test_rouspath: tests/test_rouspath.c $(LIB_OBJ) | build
	$(CC) $(CFLAGS) tests/test_rouspath.c $(LIB_OBJ) -o $@
build/test_ruenv: tests/test_ruenv.c $(LIB_OBJ) | build
	$(CC) $(CFLAGS) tests/test_ruenv.c $(LIB_OBJ) -o $@
install: all
	install -d $(DESTDIR)$(SBINDIR)
	install -m 4755 build/rous $(DESTDIR)$(SBINDIR)/rous
clean:
	rm -rf build
.PHONY: all debug test install clean
