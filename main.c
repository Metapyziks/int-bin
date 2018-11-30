#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define MAX_INPUT_FILES 256
#define BUFFER_SIZE 2048
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

	memset(bins, 0, sizeof(uint32_t) * BIN_COUNT);

	for (int i = 0; i < inputFilePathsCount; ++i)
	{
		char* filePath = inputFilePaths[i];

		struct stat st;
		if (stat(filePath, &st))
		{
			fprintf(stderr, "Unable to find file size of \"%s\", exiting.\n", filePath);
			return 1;
		}

		size_t fileSize = st.st_size;

		int file = open(filePath, O_RDONLY, 0);
		if (file == -1)
		{
			fprintf(stderr, "Unable to read from file \"%s\", exiting.\n", filePath);
			return 1;
		}

		void* map = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, file, 0);

		if (map == MAP_FAILED)
		{
			close(file);
			fprintf(stderr, "Error when mapping file, exiting.\n");
			return 1;
		}

		char* start = (char*) map;
		char* end = NULL;
		int32_t value;

		while ((value = strtol(start, &end, 10)) != 0)
		{
			start = end + 1;
			if (value < BIN_COUNT) ++(bins[value]);
		}

		if (munmap(map, fileSize))
		{
			close(file);
			fprintf(stderr, "Error when unmapping file, exiting.\n");
			return 1;
		}

		close(file);
	}

	for (int i = 0; i < BIN_COUNT; ++i)
	{
		if (bins[i] > 0)
		{
			printf("%i:%i\n", i, bins[i]);
		}
	}

	return 0;
}
