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

int failureExit(FILE *filePointer)
{
    fprintf(stderr, INVALID_INPUT_MSG);
    fclose(filePointer);
    return EXIT_FAILURE;
}

int isDigit(char c)
{
    return c >= '0' && c <= '9';
}

int parseFile(const char *filename)
{
    FILE *filePointer = fopen(filename, "r");

    //Check if file exists or if it is empty:
    if (filePointer == NULL)
    {
        fprintf(stderr, "FILE DOESN'T EXIST OR IS EMPTY");//TODO change to failureExit
        fclose(filePointer);
        return EXIT_FAILURE;
    }

    //Get number of vertexes from file:
    char line[MAX_LINE_LENGTH];
    fgets(line, MAX_LINE_LENGTH, filePointer);

    //Check if number of vertexes is a whole number:
    for (int i = 0; i < MAX_LINE_LENGTH && line[i] != '\0'; i++)
    {
        if (!isDigit(line[i]))
        {
            fprintf(stderr, "NUMBER OF VERTEXES IS NOT AN INTEGER");//TODO change to failureExit
            fclose(filePointer);
            return EXIT_FAILURE;
        }
    }

    long int vertexNum = strtol(line, NULL, 10);

    //Edge case: Check if vertexNum is 0:
    if (vertexNum == 0)
    {
        fprintf(stderr, "NUMBER OF VERTEXES IS ZERO");//TODO change to failureExit
        fclose(filePointer);
        return EXIT_FAILURE;
    }

    int vertexCounter = 0;
    while (fgets(line, MAX_LINE_LENGTH, filePointer) != NULL)
    {

        vertexCounter++;
    }

    if (vertexCounter != vertexNum)
    {

    }

    fclose(filePointer);
    return 0;
}


int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, ARG_ERR_MSG);
        return EXIT_FAILURE;
    }
    return parseFile(argv[1]);
}