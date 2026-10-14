#include "rous.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


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
