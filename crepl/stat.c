//
// Created by kr2 on 6/5/21.
//

#include "stat.h"
#include "common.h"
#include <assert.h>
#include <ctype.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static char *trim(char *stat) {
  assert(stat != NULL);

  // rtrim
  size_t len = strlen(stat);
  while (isspace(stat[len]))
    --len;
  stat[len + 1] = '\0';

  // ltrim
  while (isspace(*stat))
    ++stat;
  return stat;
}

bool isfunction(char *stat) {
  // TODO: to complete other types
  // Just a general judgement
  assert(stat != NULL);

  if (strlen(stat) <= 3)
    return false;
  stat = trim(stat);
  return strncmp(stat, "int", 3) == 0;
}

int interpret(char *stat) {
  // interpret command and return the result

  static char wrapper[4096];
  memset(wrapper, 0, sizeof(wrapper));

  // create wrapper
  snprintf(wrapper, 4096, "int func_wrapper() { return %s; }", stat);

  char buffer[4096];
  memset(buffer, 0, sizeof(buffer));
  if (compile(wrapper, buffer) == false) {
    // TODO
    // shared object file is not created
    printf(ANSI_COLOR_RED "Compile Error\n" ANSI_COLOR_RESET);
    return -1;
  }

  // compile succeed
  char *error;
  void *handle;
  int (*func_wrapper)();
  int ret;

  handle = dlopen(buffer, RTLD_LAZY);
  if (!handle) {
    fprintf(stderr, "%s\n", dlerror());
    exit(EXIT_FAILURE);
  }

  dlerror();

  func_wrapper = (int (*)(int))dlsym(handle, "func_wrapper");
  ret          = func_wrapper();

  error = dlerror();
  if (error != NULL) {
    fprintf(stderr, "%s\n", error);
    exit(EXIT_FAILURE);
  }

  dlclose(handle);
  remove(buffer); // whether succeed

  return ret;
}

bool compile(char *stat, char *so_path) {
  /* DESCRIPTION
   *   compile function will compile *stat to a shared object
   *   will write so_path to the shared object path
   *   do not forget to remove so_path
   * RETURN
   *   true as succeed, false as failed
   */

  assert(so_path != NULL);
  static int cnt = 0;
  ++cnt;

  const int FNAME_N = 256;
  char source_code_fname[FNAME_N], shared_objects_fname[FNAME_N];

  snprintf(source_code_fname, FNAME_N, "/tmp/crepl-source-code-%d.c", cnt);
  snprintf(shared_objects_fname, FNAME_N, "/tmp/crepl-shared-object-%d.so",
           cnt);

  FILE *f_cs = fopen(source_code_fname, "w");
  if (f_cs == NULL) {
    fprintf(stderr, "Failed to open file %s.\n", source_code_fname);
    return false;
  }

  fprintf(f_cs, "%s", stat);
  fflush(f_cs); // flush output to avoid empty file

  // compile the source code
  int status;
  pid_t pid = fork();

  if (pid < 0) {
    goto failed;
  } else if (pid == 0) {
    char *cmd[] = {
      COMPILER,          "-shared",   "-fpic",  "-o", shared_objects_fname,
      source_code_fname, global_file, (char *)0
    };

    execvp(COMPILER, cmd);
  } else {
    waitpid(pid, &status, 0);
  }

  // compile failed
  if (status != 0)
    goto failed;

  // compile succeed
  strncpy(so_path, shared_objects_fname, strlen(shared_objects_fname) + 1);

succeed:
  fclose(f_cs);
  remove(source_code_fname);
  return true;

failed:
  fclose(f_cs);
  remove(source_code_fname);
  return false;
}
