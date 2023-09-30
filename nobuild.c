#define NOB_IMPLEMENTATION
#include "src/nobuild.h"

#define OUT_BIN "slug"
#define SOURCE_FILES "src/main.c", "src/bytecode.c", "src/lex.c", "src/parser.c", "src/gen.c", "src/tool.c"
#define CFLAGS "-Wall", "-Wextra", "-O0", "-std=c11", "-Wformat=0"

static void build(void)
{
	Nob_Cmd cmd = {0};
	nob_cmd_append(&cmd, "cc", CFLAGS, "-o", OUT_BIN, SOURCE_FILES);
	nob_cmd_run_sync(cmd);
}

static void clean(void)
{
	Nob_Cmd cmd = {0};
	nob_cmd_append(&cmd, "rm", "-f", OUT_BIN);
	nob_cmd_run_sync(cmd);
}

static void re(void)
{
	clean();
	build();
}

int main(int argc, char **argv)
{
	NOB_GO_REBUILD_URSELF(argc, argv);
	
	const char *program = nob_shift_args(&argc, &argv);
	const char *subcommand = NULL;

	if (argc <= 0) {
		subcommand = "build";
	} 
	else {
		subcommand = nob_shift_args(&argc, &argv);
	}

	if (strcmp(subcommand, "build") == 0) {
		build();
	}
	else if (strcmp(subcommand, "clean") == 0) {
		clean();
	}
	else if (strcmp(subcommand, "re") == 0) {
		re();
	}

	return 0;
}