#include "rous.h"
#include <grp.h>
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

  fprintf(out, "examples:\n");
  fprintf(out, "  %s pacman -S neovim\n", ROUS_NAME);
  fprintf(out, "  %s -u nobody id\n", ROUS_NAME);
}


int ru_authorized(uid_t ruid) {
  struct passwd *pw;
  struct group *gr;
  gid_t groups[ROUS_GRP_MAX];
  int ngroups = ROUS_GRP_MAX;
  int i;
  if (ruid == 0) return 1;
  pw = getpwuid(ruid);
  if (!pw) return 0;
  gr = getgrnam(ROUS_ADMIN_GROUP);
  if (!gr) return 0;
  if (getgrouplist(pw->pw_name, pw->pw_gid, groups, &ngroups) < 0) return 0;
  for (i = 0; i < ngroups; i++)
    if (groups[i] == gr->gr_gid) return 1;
  return 0;
}


int ru_find_in_path(const char *cmd, char *out, size_t outn) {
  const char *p;
  if (!cmd || !*cmd || !out || outn == 0) return -1;
  if (strchr(cmd, '/')) {
    if (strlen(cmd) >= outn) return -1;
    strcpy(out, cmd);
    return 0;
  }
  p = ROUS_TRUSTED_PATH;
  while (*p) {
    const char *colon = strchr(p, ':');
    size_t dlen = colon ? (size_t)(colon - p) : strlen(p);
    int wr;
    if (dlen == 0) {
      wr = snprintf(out, outn, "./%s", cmd);
    } else {
      wr = snprintf(out, outn, "%.*s/%s", (int)dlen, p, cmd);
    }
    if (wr > 0 && (size_t)wr < outn && access(out, X_OK) == 0)
      return 0;
    if (!colon) break;
    p = colon + 1;
  }
  return -1;
}


int ru_build_env(char **env, int max, const char *term, const char *home, const char *user) {
  static char homebuf[256];
  static char userbuf[256];
  static char logbuf[256];
  static char termbuf[256];
  int n = 0;
  if (!env || max < 6) return -1;
  env[n++] = "PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
  snprintf(homebuf, sizeof(homebuf), "HOME=%s", (home && *home) ? home : "/root");
  env[n++] = homebuf;
  snprintf(userbuf, sizeof(userbuf), "USER=%s", (user && *user) ? user : "root");
  env[n++] = userbuf;
  snprintf(logbuf, sizeof(logbuf), "LOGNAME=%s", (user && *user) ? user : "root");
  env[n++] = logbuf;
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


int ru_user_groups(const char *name, gid_t base_gid, gid_t *out, int max) {
  struct passwd *pw;
  int n = max;
  if (!name || !*name || !out || max <= 0) return -1;
  pw = getpwnam(name);
  if (!pw) return -1;
  if (getgrouplist(name, base_gid, out, &n) < 0) return -1;
  return n;
}
