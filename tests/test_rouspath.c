#include "rous.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  char out[ROUS_PATH_MAX];
  char tiny[4];
  char huge[ROUS_PATH_MAX + 64];

  assert(ru_find_in_path("/bin/sh", out, sizeof(out)) == 0);
  assert(strcmp(out, "/bin/sh") == 0);

  assert(ru_find_in_path("./local-bin", out, sizeof(out)) == 0);
  assert(strcmp(out, "./local-bin") == 0);

  assert(ru_find_in_path("sh", out, sizeof(out)) == 0);
  assert(access(out, X_OK) == 0);
  {
    const char *base = strrchr(out, '/');
    base = base ? base + 1 : out;
    assert(strcmp(base, "sh") == 0);
  }

  assert(ru_find_in_path("definitely-not-a-real-binary-xyz", out, sizeof(out)) != 0);

  assert(ru_find_in_path(0, out, sizeof(out)) != 0);
  assert(ru_find_in_path("", out, sizeof(out)) != 0);
  assert(ru_find_in_path("sh", 0, sizeof(out)) != 0);
  assert(ru_find_in_path("sh", out, 0) != 0);

  assert(ru_find_in_path("/bin/sh", tiny, sizeof(tiny)) != 0);

  memset(huge, 'a', sizeof(huge) - 1);
  huge[sizeof(huge) - 1] = 0;
  assert(ru_find_in_path(huge, out, sizeof(out)) != 0);

  printf("test_rouspath ok\n");
  return 0;
}

