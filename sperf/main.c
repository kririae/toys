#include <assert.h>
#include <float.h>
#include <unistd.h>
#include <regex.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>

#define BUF_SIZE          1024
#define SAMPLING_INTERVAL 1000  // Unit: ms
#define MOD               100023

#define VALID_ITEM(x) (hashTable[x].name != NULL)

struct SyscInfo {
  float time;
  char *name;
};

// Implementation of a simple hashTable
struct HashTableItem {
  float val;
  char *name;
} hashTable[MOD];

static int compareHashTableItem(const struct HashTableItem *a,
                                const struct HashTableItem *b) {
  float a_val = (a->name == NULL) ? FLT_MAX : a->val;
  float b_val = (b->name == NULL) ? FLT_MAX : b->val;

  return (a_val < b_val);
}

static void getSyscInfo(struct SyscInfo *out, const char *line) {
  /*
   * Map from line to SyscInfo:
   *   `close(2) = 0 <0.000016>` to
   *   SyscInfo {close, 0.000016 };
   *
   * Note: will allocate memory for out->name
   */
  assert(line != NULL);
  assert(out != NULL);

  const int L_BUF_SIZE = 128;
  int status;
  regmatch_t pmatch[L_BUF_SIZE];
  regex_t reg;
  const char *pattern = "^(\\w+)\\(.* <([0-9]+.[0-9]+)>";

  regcomp(&reg, pattern, REG_EXTENDED);
  status = regexec(&reg, line, L_BUF_SIZE - 1, pmatch, 0);
  regfree(&reg);
  if (status == REG_NOMATCH) {
    return;  // no match
  }

  ssize_t name_length = pmatch[1].rm_eo - pmatch[1].rm_so;
  assert(name_length > 0);
  out->name = (char *)malloc(name_length + 1);
  memcpy(out->name, line, name_length);
  out->name[name_length] = '\0';

  assert(pmatch[2].rm_so != -1);
  sscanf(line + pmatch[2].rm_so, "%f", &(out->time));

  return;
}

static int hashStr(const char *name) {
  assert(name != NULL);
  size_t len = strlen(name);
  int ans    = 1;

  for (int i = 0; i < len; ++i) {
    ans = (ans * name[i] + 1) % MOD;
  }

  return ans;
}

static void insertHash(const struct SyscInfo *info) {
  assert(info != NULL);

  int hash = hashStr(info->name);
  if (hashTable[hash].name == NULL) {
    hashTable[hash].name = info->name;
  } else {
    if (strcmp(hashTable[hash].name, info->name) != 0) {
      printf("[sperf] ERROR: <hash_conflict> \n  %s %d\n  %s %d\n",
             hashTable[hash].name,
             hashStr(hashTable[hash].name),
             info->name,
             hashStr(info->name));
    }
    assert(strcmp(hashTable[hash].name, info->name) == 0);  // identical
  }

  hashTable[hash].val += info->time;  // Collect syscall time
}

static void printHashTable() {
  float total_time = 0;
  ssize_t max_len  = 0;
  for (int i = 0; i < MOD; ++i) {
    if (VALID_ITEM(i)) {
      total_time += hashTable[i].val;

      ssize_t len = strlen(hashTable[i].name);
      if (len > max_len) {
        max_len = len;  // get max length for printing
      }
    }
  }

  struct HashTableItem tmp_hash_table[MOD];
  memset(tmp_hash_table, 0, sizeof(tmp_hash_table));
  memcpy(tmp_hash_table, hashTable, sizeof(hashTable));
  qsort(
      tmp_hash_table, MOD, sizeof(struct HashTableItem), compareHashTableItem);

  printf("--- sperf info ---\n");
  // Format HashTable
  for (int i = 0; i < MOD; ++i) {
    if (tmp_hash_table[i].name != NULL) {
      printf("[sperf] %-*s | %f (%f%%)\n",
             max_len,
             tmp_hash_table[i].name,
             tmp_hash_table[i].val,
             tmp_hash_table[i].val / total_time * 100.0f);
    }
  }
}

int main(int argc, char *argv[]) {
  /*
   * help: sperf COMMAND [ARG]...
   */

  if (argc == 1) {
    printf("Usage:\n  sperf COMMAND [ARG] ...");
    return 0;
  }

  printf("[sperf] Executing: ");
  int i;
  for (i = 1; i < argc; ++i) {
    printf("%s \n", argv[i]);
  }
  printf("[sperf] Collecting data...\n");

  // Fill with strace -r [COMMAND + ARG] + NULL = argc + 2
  char **exe_argv = (char **)malloc(sizeof(char *) * (argc + 2));
  exe_argv[0]     = "strace";
  exe_argv[1]     = "-T";  // syscall time
  for (i = 1; i < argc; ++i) {
    exe_argv[i + 1] = argv[i];
  }
  exe_argv[argc + 1] = NULL;

  int fildes[2];
  if (pipe(fildes) != 0) {  // Error processing
    printf("[sperf] Failed to create pipe;");
    fflush(stdout);
    return -1;
  }

  char buf[BUF_SIZE];
  pid_t pid;

  switch (pid = fork()) {
    case -1:
      printf("[sperf] Failed to execute.");
      fflush(stdout);
      return -1;

    case 0:                            // Child process
      close(fildes[0]);                // Read-end is not used
      dup2(fildes[1], STDERR_FILENO);  // Replace stdout with write-end
      //      dup2(fopen("/dev/null", "w"), STDOUT_FILENO);
      execvp("strace", exe_argv);
      assert(0);  // Should not be executed

    default: {  // Parent process
      close(fildes[1]);
      printf("[sperf] The fildes: %d %d\n", fildes[0], fildes[1]);

      struct timeval start_time, now;
      gettimeofday(&start_time, NULL);  // Collect start_time

      while (1) {  // loop of sampling
        int status;
        waitpid(pid, &status, WNOHANG);
        FILE *in = fdopen(fildes[0], "r");

        if (fgets(buf, BUF_SIZE, in) != NULL) {
          struct SyscInfo info;
          getSyscInfo(&info, buf);
          insertHash(&info);
        }

        gettimeofday(&now, NULL);
        uint64_t elapsed_time = ((now.tv_sec - start_time.tv_sec) * 1000000 +
                                 (now.tv_usec - start_time.tv_usec)) /
                                1000;  // Unit: us -> ms

        if (elapsed_time > SAMPLING_INTERVAL) {
          printHashTable();

          gettimeofday(&start_time, NULL);

          if (WIFEXITED(status)) {
            break;
          }
        }
      }

      // Reached end
      printf("[sperf] Reached end");
      break;
    }
  }

  free(exe_argv);
  return 0;
}
