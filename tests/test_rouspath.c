#include "rous.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
  char out[ROUS_PATH_MAX];
  const char *path = "/one:/bin:/two";

  assert(ru_find_in_path("/bin/sh", 0, out, sizeof(out)) == 0);
  assert(strcmp(out, "/bin/sh") == 0);
  assert(ru_find_in_path("./local-bin", 0, out, sizeof(out)) == 0);
  assert(strcmp(out, "./local-bin") == 0);

  assert(ru_find_in_path("sh", path, out, sizeof(out)) == 0);
  assert(strcmp(out, "/bin/sh") == 0);

  assert(ru_find_in_path("definitely-not-a-real-binary-xyz", path, out, sizeof(out)) != 0);

  assert(ru_find_in_path(0, path, out, sizeof(out)) != 0);
  assert(ru_find_in_path("", path, out, sizeof(out)) != 0);
  assert(ru_find_in_path("sh", path, 0, sizeof(out)) != 0);
  assert(ru_find_in_path("sh", path, out, 0) != 0);

  printf("test_rouspath ok\n");
  return 0;
}
