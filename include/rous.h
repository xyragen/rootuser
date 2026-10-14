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

long ru_geteuid(void);
long ru_run(const char *path, char *argv[], char *envp[], uid_t uid, gid_t gid);

int ru_find_in_path(const char *cmd, const char *path, char *out, size_t outn);
int ru_build_env(char **env, int max, const char *term, const char *home);
int ru_lookup_user(const char *name, uid_t *uid, gid_t *gid, char *home, size_t homen);
void ru_usage(FILE *out);

#ifdef __cplusplus
}
#endif
#endif

