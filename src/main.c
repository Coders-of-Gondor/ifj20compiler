/* main.c
 * Ondřej Míchal <xmicha80>
 * 24/09/2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error.h"

const char *program_name = "ifj20compiler";

typedef struct args {
	char *input_file;
} args_t;

void print_usage_hint() {
  printf("See '%s --help' for usage.\n", program_name);
}

void print_help() {
  printf("Usage: %s [options] file \n", program_name);
  printf("\n");
  printf("Options:\n");
  printf("\t-h, --help\t\tshow help screen\n");

  exit(SUCCESS);
}

void handle_args(int argc, char *argv[], args_t *args) {
  for (int i = 1; i < argc; i++) {
    // TODO: Add --debug option to enable debugging macros
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
      print_help();
    else if (strncmp(argv[i], "-", 1) == 0) {
      // Handle unrecognized options
      fprintf(stderr, "%s: unrecognized option %s\n", program_name, argv[i]);
      print_usage_hint();
      exit(ERROR_INTERNAL);
    }
    else {
      if (args->input_file == NULL)
        args->input_file = argv[i];
      else {
        fprintf(stderr, "%s: only one input file is accepted\n", program_name);
        print_usage_hint();
        exit(ERROR_INTERNAL);
      }
    }
  }

  if (args->input_file == NULL) {
    fprintf(stderr, "%s: no input file was specified\n", program_name);
    print_usage_hint();
    exit(ERROR_INTERNAL);
  }
}

int main(int argc, char *argv[]) {
  args_t args = { NULL, };
  handle_args(argc, argv, &args);

  FILE *f = fopen(args.input_file, "r");
  if (f == NULL) {
    fprintf(stderr, "%s: there was an error while opening file \"%s\"\n", program_name, args.input_file);
    print_usage_hint();
    return ERROR_INTERNAL;
  }

  return SUCCESS;
}
