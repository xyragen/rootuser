#include "rous.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

/*
 * Authorization policy. uid 0 is always allowed. Any other uid is only
 * allowed when it maps to a real account that belongs to the admin
 * group. A uid with no passwd entry can never be authorized, and a
 * made up high uid must be refused.
 */
int main(void) {
  assert(ru_authorized(0) == 1);

  assert(ru_authorized((uid_t)999999) == 0);
  assert(ru_authorized((uid_t)-1) == 0);

  if (geteuid() == 0) {
    assert(ru_authorized(getuid()) == 1);
  } else {
    int a = ru_authorized(getuid());
    assert(a == 0 || a == 1);
  }

  printf("test_ruauth ok\n");
  return 0;
}
