#ifndef ROUS_H
#define ROUS_H
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#ifdef __cplusplus
extern "C" {
#endif

#define ROUS_NAME "rous"
#define ROUS_VERSION "0.1"

#define ROUS_PATH_MAX 4096
#define ROUS_ENV_MAX 16
#define ROUS_GRP_MAX 256

#ifndef ROUS_ADMIN_GROUP
#define ROUS_ADMIN_GROUP "wheel"
#endif

#define ROUS_TRUSTED_PATH "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin"

/*
 * Target identity handed to the assembly core. Field order is fixed so
 * the raw syscall code can read it by offset; the static asserts below
 * lock the layout in at compile time.
 */
typedef struct {
  const gid_t *grouplist;
  uid_t uid;
  gid_t gid;
  int ngroups;
} ru_ids_t;

_Static_assert(offsetof(ru_ids_t, grouplist) == 0, "grouplist offset");
_Static_assert(offsetof(ru_ids_t, uid) == 8, "uid offset");
_Static_assert(offsetof(ru_ids_t, gid) == 12, "gid offset");
_Static_assert(offsetof(ru_ids_t, ngroups) == 16, "ngroups offset");

long ru_geteuid(void);
long ru_getuid(void);
long ru_run(const char *path, char *argv[], char *envp[], const ru_ids_t *ids);

int ru_authorized(uid_t ruid);
int ru_find_in_path(const char *cmd, char *out, size_t outn);
int ru_build_env(char **env, int max, const char *term, const char *home, const char *user);
int ru_lookup_user(const char *name, uid_t *uid, gid_t *gid, char *home, size_t homen);
int ru_user_groups(const char *name, gid_t base_gid, gid_t *out, int max);
void ru_usage(FILE *out);

#ifdef __cplusplus
}
#endif
#endif

