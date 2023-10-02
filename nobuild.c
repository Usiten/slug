#define NOB_IMPLEMENTATION
#include "src/nobuild.h"

#ifdef _WIN32
#  define OUT_BIN "slug.exe"
#else
#  define OUT_BIN "slug"
#endif

#define SOURCE_FILES "src/main.c", "src/bytecode.c", "src/lex.c", "src/parser.c", "src/vm.c", "src/gen.c", "src/tool.c", "src/cli.c", "src/map.c"
#define CFLAGS "-Wall", "-Wextra", "-O0", "-std=c11", "-Wformat=0"

static void build(void)
{
	Nob_Cmd cmd = {0};
	nob_cmd_append(&cmd, "gcc", CFLAGS, "-o", OUT_BIN, SOURCE_FILES);
	nob_cmd_run_sync(cmd);
}

static void clean(void)
{
	Nob_Cmd cmd = {0};
#ifdef _WIN32
	nob_cmd_append(&cmd, "del", "/Q", OUT_BIN);
#else
	nob_cmd_append(&cmd, "rm", "-f", OUT_BIN);
#endif
	nob_cmd_run_sync(cmd);
}

static void re(void)
{
	clean();
	build();
}

static void test(void)
{
	Nob_Cmd cmd = {0};
	#ifdef _WIN32
		nob_cmd_append(&cmd, "python", "run_tests.py");
	#else
		nob_cmd_append(&cmd, "python3", "run_tests.py");
	#endif
	nob_cmd_run_sync(cmd);
}

int main(int argc, char **argv)
{
	#ifndef _WIN32
		NOB_GO_REBUILD_URSELF(argc, argv);
	#endif
	
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
	else if (strcmp(subcommand, "test") == 0) {
		test();
	}

	return 0;
}