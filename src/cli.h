#ifndef SL_CLI_H
#define SL_CLI_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>

void SL_usage_print(void);
int SL_cli_handle(int argc, char** argv);

#endif // SL_CLI_H