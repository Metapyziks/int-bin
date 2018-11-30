#include <stdio.h>
#include <string.h>

#define MAX_INPUT_FILES 256

int main(int argc, char** argv)
{
	char* inputFilePaths[MAX_INPUT_FILES];
	int inputFilePathsCount = 0;

	for (int i = 1; i < argc; ++i)
	{
		char* arg = argv[i];
		int argLen = strlen(arg);

		if (argLen == 0) continue;

		if (arg[0] != '-')
		{
			inputFilePaths[inputFilePathsCount++] = arg;
			continue;
		}

		if (strcmp(arg, "-") == 0)
		{
			fprintf(stderr, "Reading from stdin not yet supported.\n");
			return 1;
		}

		fprintf(stderr, "Unexpected option \"%s\".\n", arg);
		return 1;
	}

	if (inputFilePathsCount == 0)
	{
		fprintf(stderr, "Expected one or more input files\n");
		return 1;
	}

	printf("Input count: %i\n", inputFilePathsCount);

	return 0;
}
