#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_INPUT_FILES 256
#define BUFFER_SIZE 256
#define BIN_COUNT 65536

int parse_arguments(int argc, char** argv,
	int* inputFilePathsCountPtr, char** inputFilePathsPtr)
{
	*inputFilePathsCountPtr = 0;

	for (int i = 1; i < argc; ++i)
	{
		char* arg = argv[i];
		int argLen = strlen(arg);

		if (argLen == 0) continue;

		if (arg[0] != '-')
		{
			inputFilePathsPtr[(*inputFilePathsCountPtr)++] = arg;
			continue;
		}

		if (strcmp(arg, "-") == 0)
		{
			fprintf(stderr, "Reading from stdin not yet supported.\n");
			return 0;
		}

		fprintf(stderr, "Unexpected option \"%s\".\n", arg);
		return 0;
	}

	if (*inputFilePathsCountPtr == 0)
	{
		fprintf(stderr, "Expected one or more input files.\n");
		return 0;
	}

	return 1;
}

int next_power_of_two(int value)
{
	int power = 1;
	while (power < value) power << 1;
	return power;
}

int main(int argc, char** argv)
{
	int inputFilePathsCount;
	char* inputFilePaths[MAX_INPUT_FILES];

	if (!parse_arguments(argc, argv, &inputFilePathsCount, inputFilePaths))
	{
		fprintf(stderr, "Error parsing arguments, exiting.\n");
		return 1;
	}

	uint32_t bins[BIN_COUNT];
	char buffer[BUFFER_SIZE];

	int firstBin = BIN_COUNT;
	int lastBin = 0;

	for (int i = 0; i < inputFilePathsCount; ++i)
	{
		char* filePath = inputFilePaths[i];

		FILE* file = fopen(filePath, "r");
		if (file == NULL)
		{
			fprintf(stderr, "Unable to read from file \"%s\", exiting.\n", filePath);
			return 1;
		}

		while (fgets(buffer, sizeof(buffer), file))
		{
			int value = strtol(buffer, (char**) NULL, 10);

			if (value < 0) continue;
			if (value >= BIN_COUNT) continue;

			if (value < firstBin) firstBin = value;
			if (value > lastBin) lastBin = value;
			
			++(bins[value]);
		}
	}

	for (int i = firstBin; i <= lastBin; ++i)
	{
		printf("%i:%i\n", i, bins[i]);
	}

	return 0;
}
