//
// created by kr2 on 6/5/21.
// frontend of <crepl>
//

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "stat.h"
#include "common.h"

char global_file[] = "/tmp/crepl-global-file.c";

static void usage(int argc, char *argv[]) {
  printf("Usage: %s [OPTION...]\n", argv[0]);
  printf("\t-e, --execute     execute C statement\n");
  printf("\t-i, --interpret   enter crepl\n");
}

static void stop(int status) {
  remove(global_file);
  exit(status);
}

static inline void parse_args(int argc, char *argv[]) {
  const struct option table[] = {
    { "execute", required_argument, NULL, 'e' },
    { "interpret", no_argument, NULL, 'i' },
    { 0, 0, NULL, 0 },
  };

  int o;
  while ((o = getopt_long(argc, argv, "-e:i", table, NULL)) != -1) {
    switch (o) {
    case 'e': printf("%d\n", interpret(optarg)); stop(EXIT_SUCCESS);
    case 'i': return;
    default: usage(argc, argv); stop(EXIT_SUCCESS);
    }
  }
}

static inline bool add_func(char *stat) {
  // add to global_file
  FILE *f = fopen(global_file, "a");
  if (f == NULL) {
    return false;
  }

  fprintf(f, "%s", stat);
  fflush(f);
  fclose(f);
  return true;
}

int main(int argc, char *argv[]) {
  remove(global_file);
  parse_args(argc, argv);

  // create global file
  FILE *f = fopen(global_file, "a");
  fclose(f);

  // static char line[4096];
  while (true) {
    char *line;
    size_t len;

    if ((line = readline("crepl> ")) == NULL)
      break;
    add_history(line);

    if (isfunction(line)) {
      // compile(line, path);
      bool ret = add_func(line);
      if (ret) {
        printf("Add: %s\n", line);
      }
    } else {
      int ret = interpret(line);
      // TODO
      if (ret != -1) {
        printf("(%s) == %d\n", line, ret);
      }
    }

    free(line);
  }

  stop(EXIT_SUCCESS);
}
