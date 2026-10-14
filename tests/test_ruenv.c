#include "rous.h"
#include <assert.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  char *env[ROUS_ENV_MAX];
  uid_t uid;
  gid_t gid;
  char home[256];
  int n;
  int i;
  int found_home = 0;
  int found_path = 0;
  int found_term = 0;

  n = ru_build_env(env, ROUS_ENV_MAX, "xterm-256color", "/root");
  assert(n >= 5);
  assert(env[n] == 0);
  for (i = 0; i < n; i++) {
    assert(strchr(env[i], '=') != 0);
    if (strncmp(env[i], "PATH=", 5) == 0) found_path = 1;
    if (strncmp(env[i], "HOME=", 5) == 0) found_home = 1;
    if (strncmp(env[i], "TERM=", 5) == 0) found_term = 1;
  }
  assert(found_path);
  assert(found_home);
  assert(found_term);

  n = ru_build_env(env, ROUS_ENV_MAX, 0, 0);
  assert(n >= 5);
  assert(env[n] == 0);
  for (i = 0; i < n; i++)
    assert(strncmp(env[i], "TERM=", 5) != 0);

  assert(ru_lookup_user("root", &uid, &gid, home, sizeof(home)) == 0);
  assert(uid == 0);
  assert(gid == 0);

  assert(ru_lookup_user("no-such-user-rous-xyz", &uid, &gid, home, sizeof(home)) != 0);
  assert(ru_lookup_user(0, &uid, &gid, home, sizeof(home)) != 0);
  assert(ru_lookup_user("", &uid, &gid, home, sizeof(home)) != 0);

  printf("test_ruenv ok\n");
  return 0;
}
