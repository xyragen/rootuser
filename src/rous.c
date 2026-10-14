#include "rous.h"
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void ru_usage(FILE *out) {
  fprintf(out, "usage: %s [-u user] <command> [args...]\n", ROUS_NAME);
  fprintf(out, "run a command as the root user\n\n");
  fprintf(out, "  -u, --user <name>  run as this user instead of root\n");
  fprintf(out, "  -h, --help         show this help\n");
  fprintf(out, "  -V, --version      show version\n\n");
  fprintf(out, "examples:\n");
  fprintf(out, "  %s pacman -S neovim\n", ROUS_NAME);
  fprintf(out, "  %s -u nobody id\n", ROUS_NAME);
}

int ru_find_in_path(const char *cmd, const char *path, char *out, size_t outn) {
  const char *p;
  if (!cmd || !*cmd || !out || outn == 0) return -1;
  if (strchr(cmd, '/')) {
    if (strlen(cmd) >= outn) return -1;
    strcpy(out, cmd);
    return 0;
  }
  if (!path || !*path)
    path = "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
  p = path;
  while (*p) {
    const char *colon = strchr(p, ':');
    size_t dlen = colon ? (size_t)(colon - p) : strlen(p);
    if (dlen == 0) {
      if ((size_t)snprintf(out, outn, "./%s", cmd) < outn && access(out, X_OK) == 0)
        return 0;
    } else {
      char dir[ROUS_PATH_MAX];
      if (dlen >= sizeof(dir)) dlen = sizeof(dir) - 1;
      memcpy(dir, p, dlen);
      dir[dlen] = 0;
      if ((size_t)snprintf(out, outn, "%s/%s", dir, cmd) < outn && access(out, X_OK) == 0)
        return 0;
    }
    if (!colon) break;
    p = colon + 1;
  }
  return -1;
}

int ru_build_env(char **env, int max, const char *term, const char *home) {
  static char homebuf[256];
  static char termbuf[256];
  int n = 0;
  if (!env || max < 6) return -1;
  env[n++] = "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
  snprintf(homebuf, sizeof(homebuf), "HOME=%s", (home && *home) ? home : "/root");
  env[n++] = homebuf;
  env[n++] = "USER=root";
  env[n++] = "LOGNAME=root";
  env[n++] = "SHELL=/bin/sh";
  if (term && *term && n < max - 1) {
    snprintf(termbuf, sizeof(termbuf), "TERM=%s", term);
    env[n++] = termbuf;
  }
  env[n] = 0;
  return n;
}

int ru_lookup_user(const char *name, uid_t *uid, gid_t *gid, char *home, size_t homen) {
  struct passwd *pw;
  if (!name || !*name) return -1;
  pw = getpwnam(name);
  if (!pw) return -1;
  if (uid) *uid = pw->pw_uid;
  if (gid) *gid = pw->pw_gid;
  if (home && homen) {
    strncpy(home, pw->pw_dir, homen - 1);
    home[homen - 1] = 0;
  }
  return 0;
}
