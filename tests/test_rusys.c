#include "rous.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
 * These exercise the assembly core and the authorization policy without
 * needing to be root. ru_run is only ever safe to call from a privileged
 * process, so here it must fail: when the real caller is not root the
 * setgroups/setgid/setuid calls are rejected and ru_run has to return a
 * negative errno instead of falling through to execve. We point it at a
 * path that does not exist so that even a bug that skipped the id
 * changes could not accidentally run anything.
 */
int main(void) {
  ru_ids_t ids;
  char *argv[2];
  char *envp[1];
  long rc;

  argv[0] = (char *)"/nonexistent-rous-should-never-exec";
  argv[1] = 0;
  envp[0] = 0;

  ids.grouplist = 0;
  ids.uid = 0;
  ids.gid = 0;
  ids.ngroups = 0;

  if (geteuid() != 0) {
    rc = ru_run("/nonexistent-rous-should-never-exec", argv, envp, &ids);
    assert(rc < 0);
  }

  printf("test_rusys ok\n");
  return 0;
}
