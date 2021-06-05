//
// created by kr2 on 6/5/21.
// frontend of <crepl>
//

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "stat.h"

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

  // usage(argc, argv);
  // stop(0);
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
  parse_args(argc, argv);

  // create global file
  FILE *f = fopen(global_file, "a");
  fclose(f);

  static char line[4096];
  while (true) {
    printf("crepl> ");
    fflush(stdout);
    if (!fgets(line, sizeof(line), stdin)) {
      break;
    }

    size_t len    = strlen(line);
    line[len - 1] = '\0'; // remove \n

    if (isfunction(line)) {
      // compile(line, path);
      add_func(line);
      printf("Add: %s\n", line);
    } else {
      printf("(%s) == %d\n", line, interpret(line));
    }
  }

  stop(EXIT_SUCCESS);
}
