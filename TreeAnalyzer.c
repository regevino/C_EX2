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

int isDigit(char c)
{
    return c >= 48 && c <= 57;
}

int parseFile(const char *filename)
{
    FILE *fp = fopen(filename, "r");

    //Check if file exists or if it is empty:
    if (fp == NULL)
    {
        fprintf(stderr, INVALID_INPUT_MSG);
        fclose(fp);
        return EXIT_FAILURE;
    }

    char *line = NULL;

    //Check if first line contains an integer number:
    fgets(line, MAX_LINE_LENGTH, fp);
    for (int i = 0; i < MAX_LINE_LENGTH && line[i] != '\0'; i++)
    {
        if (!isDigit(line[i]))
        {

        }
    }
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