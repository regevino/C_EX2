#include "queue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ARG_ERR_MSG "Usage: TreeAnalyzer <Graph File Path> <First Vertex> <Second Vertex>\n"
#define INVALID_INPUT_MSG "Invalid input\n"

#define MAX_LINE_LENGTH 1024
struct Vertex
{
    unsigned int key;
    struct Vertex *children;
};

int parseFile(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        fprintf(stderr, INVALID_INPUT_MSG);
        fclose(fp);
        return EXIT_FAILURE;
    }

    char *line = NULL;
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        
    }

    fclose(fp);
    return 0;
}


int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, ARG_ERR_MSG);
        return EXIT_FAILURE;
    }
//    return parseFile(argv[1]);
}