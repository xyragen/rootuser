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
  gid_t groups[ROUS_GRP_MAX];
  int n;
  int i;
  int found_home = 0;
  int found_path = 0;
  int found_term = 0;
  int found_user = 0;
  int user_is_nobody = 0;

  n = ru_build_env(env, ROUS_ENV_MAX, "xterm-256color", "/root", "root");
  assert(n >= 5);
  assert(env[n] == 0);
  for (i = 0; i < n; i++) {
    assert(strchr(env[i], '=') != 0);
    if (strncmp(env[i], "PATH=", 5) == 0) found_path = 1;
    if (strncmp(env[i], "HOME=", 5) == 0) found_home = 1;
    if (strncmp(env[i], "TERM=", 5) == 0) found_term = 1;
    if (strncmp(env[i], "USER=", 5) == 0) found_user = 1;
  }
  assert(found_path);
  assert(found_home);
  assert(found_term);
  assert(found_user);

  n = ru_build_env(env, ROUS_ENV_MAX, 0, 0, 0);
  assert(n >= 5);
  assert(env[n] == 0);
  for (i = 0; i < n; i++)
    assert(strncmp(env[i], "TERM=", 5) != 0);

  assert(ru_build_env(0, ROUS_ENV_MAX, 0, 0, 0) < 0);
  assert(ru_build_env(env, 3, 0, 0, 0) < 0);

  n = ru_build_env(env, ROUS_ENV_MAX, "dumb", "/nonexistent", "nobody");
  assert(n >= 5);
  for (i = 0; i < n; i++) {
    if (strncmp(env[i], "USER=", 5) == 0) {
      assert(strcmp(env[i], "USER=nobody") == 0);
      user_is_nobody = 1;
    }
    if (strncmp(env[i], "LOGNAME=", 8) == 0)
      assert(strcmp(env[i], "LOGNAME=nobody") == 0);
    if (strncmp(env[i], "HOME=", 5) == 0)
      assert(strcmp(env[i], "HOME=/nonexistent") == 0);
  }
  assert(user_is_nobody);

  assert(ru_lookup_user("root", &uid, &gid, home, sizeof(home)) == 0);
  assert(uid == 0);
  assert(gid == 0);

  assert(ru_lookup_user("no-such-user-rous-xyz", &uid, &gid, home, sizeof(home)) != 0);
  assert(ru_lookup_user(0, &uid, &gid, home, sizeof(home)) != 0);
  assert(ru_lookup_user("", &uid, &gid, home, sizeof(home)) != 0);

  n = ru_user_groups("root", 0, groups, ROUS_GRP_MAX);
  assert(n >= 1);
  assert(groups[0] == 0);
  assert(ru_user_groups("no-such-user-rous-xyz", 0, groups, ROUS_GRP_MAX) < 0);
  assert(ru_user_groups(0, 0, groups, ROUS_GRP_MAX) < 0);
  assert(ru_user_groups("root", 0, 0, ROUS_GRP_MAX) < 0);
  assert(ru_user_groups("root", 0, groups, 0) < 0);

  printf("test_ruenv ok\n");
  return 0;
}

