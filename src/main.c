#include "rous.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
  const char *user = 0;
  const char *home = "/root";
  char userhome[256];
  char path[ROUS_PATH_MAX];
  char *env[ROUS_ENV_MAX];
  const char *term;
  uid_t uid = 0;
  gid_t gid = 0;
  int ai = 1;
  long rc;

  while (ai < argc && argv[ai][0] == '-' && argv[ai][1] != 0) {
    if (strcmp(argv[ai], "-h") == 0 || strcmp(argv[ai], "--help") == 0) {
      ru_usage(stdout);
      return 0;
    } else if (strcmp(argv[ai], "-V") == 0 || strcmp(argv[ai], "--version") == 0) {
      printf("%s %s\n", ROUS_NAME, ROUS_VERSION);
      return 0;
    } else if (strcmp(argv[ai], "-u") == 0 || strcmp(argv[ai], "--user") == 0) {
      if (ai + 1 >= argc) {
        fprintf(stderr, "%s: -u needs a user name\n", ROUS_NAME);
        return 1;
      }
      user = argv[++ai];
      ai++;
    } else if (strcmp(argv[ai], "--") == 0) {
      ai++;
      break;
    } else {
      fprintf(stderr, "%s: unknown option %s\n", ROUS_NAME, argv[ai]);
      ru_usage(stderr);
      return 1;
    }
  }

  if (ai >= argc) {
    fprintf(stderr, "%s: no command given\n", ROUS_NAME);
    ru_usage(stderr);
    return 1;
  }

  if (ru_geteuid() != 0) {
    fprintf(stderr, "%s: not privileged, install with owner root and mode 4755\n", ROUS_NAME);
    return 1;
  }

  if (user) {
    if (ru_lookup_user(user, &uid, &gid, userhome, sizeof(userhome)) != 0) {
      fprintf(stderr, "%s: unknown user %s\n", ROUS_NAME, user);
      return 1;
    }
    home = userhome;
  }

  if (ru_find_in_path(argv[ai], getenv("PATH"), path, sizeof(path)) != 0) {
    fprintf(stderr, "%s: command not found: %s\n", ROUS_NAME, argv[ai]);
    return 127;
  }

  term = getenv("TERM");
  if (ru_build_env(env, ROUS_ENV_MAX, term, home) < 0) {
    fprintf(stderr, "%s: cannot build environment\n", ROUS_NAME);
    return 1;
  }

  argv[ai] = path;
  rc = ru_run(path, &argv[ai], env, uid, gid);
  if (rc != 0) {
    if (rc < 0) rc = -rc;
    fprintf(stderr, "%s: %s: %s\n", ROUS_NAME, path, strerror((int)rc));
    return rc == ENOENT ? 127 : 126;
  }
  return 0;
}
