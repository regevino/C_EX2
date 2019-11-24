#include "queue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ARG_ERR_MSG "Usage: TreeAnalyzer <Graph File Path> <First Vertex> <Second Vertex>\n"
#define INVALID_INPUT_MSG "Invalid input\n"
#define MAX_LINE_LENGTH 1025
#define MAX_CHILDREN 512
#define SUCCESS 0
#define NO_PARENT_FLAG (-1)
#define LEAF_FLAG (-2)
struct Vertex
{
    int neighbors[MAX_CHILDREN + 1];
    int key;
};

void printDetails(int fstVertex, int secVertex, int vertexCounter, const struct Vertex *root);

struct Vertex *getRoot(int vertexCounter, struct Vertex *const *vertices, struct Vertex *root)
{
    for (int k = 0; k < vertexCounter; k++)
    {
        if (vertices[k]->neighbors[0] == NO_PARENT_FLAG)
        {
            root = vertices[k];
        }
    }
    return root;
}

int failureExit()
{
    fprintf(stderr, INVALID_INPUT_MSG);
    return EXIT_FAILURE;
}

int checkVertexNum(FILE *filePointer, char *line)
{
    int vertexCounter = 0;
    while (fgets(line, MAX_LINE_LENGTH, filePointer) != NULL)
    {
        vertexCounter++;
    }
    return vertexCounter;
}

int isDigit(char c)
{
    return c >= '0' && c <= '9';
}

int checkChildren(char *token)
{
    for (int i = 0; i < MAX_CHILDREN && token[i] != '\0'; i++)
    {
        if (!isDigit(token[i]) && token[i] != '\r' && token[i] != '\n')
        {
            fprintf(stderr, "ILLEGAL CHILD");//TODO change to failureExit
            return EXIT_FAILURE;
        }
    }
    return SUCCESS;
}

int isInCheckedTokens(char *checkedTokens[], char *token, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (token[strlen(token) - 1] == '\n')
        {
            token[strlen(token) - 1] = 0;
        }
        if (token[strlen(token) - 1] == '\r')
        {
            token[strlen(token) - 1] = 0;
        }

        if (strncmp(checkedTokens[i], token, MAX_CHILDREN) == 0)
        {
            fprintf(stderr, "CHILD APPEARS TWICE");//TODO change to failureExit
            return EXIT_FAILURE;
        }
    }
    return SUCCESS;
}

void destroyVertices(int vertexCounter, struct Vertex **vertices)
{
    for (int j = 0; j < vertexCounter; j++)
    {
        free(vertices[j]);
        vertices[j] = NULL;
    }
    free(vertices);
    vertices = NULL;
}

int parseFile(FILE *filePointer)
{
    //Edge case: Check if file exists:
    if (filePointer == NULL)
    {
        fprintf(stderr, "FILE DOESN'T EXIST OR IS EMPTY");//TODO change to failureExit
        return EXIT_FAILURE;
    }

    //Edge case: Check if file is empty.
    //If not then get number of vertexes from file:
    char line[MAX_LINE_LENGTH];
    if (fgets(line, MAX_LINE_LENGTH, filePointer) == NULL)
    {
        fprintf(stderr, "FILE IS EMPTY");//TODO change to failureExit
        return EXIT_FAILURE;
    }

    //Edge case: Check if number of vertexes is a whole number:
    for (int i = 0; i < MAX_LINE_LENGTH && line[i] != '\r' && line[i] != '\n'; i++)
    {
        if (!isDigit(line[i]))
        {
            fprintf(stderr, "NUMBER OF VERTEXES IS NOT AN INTEGER");//TODO change to failureExit
            return EXIT_FAILURE;
        }
    }

    int vertexNum = (int) strtol(line, NULL, 10);

    //Edge case: Check if vertexNum is 0:
    if (vertexNum == 0)
    {
        fprintf(stderr, "NUMBER OF VERTEXES IS ZERO");//TODO change to failureExit
        return EXIT_FAILURE;
    }

    //Edge case: Check if vertexNum matches number of vertexes:
    int vertexCounter = checkVertexNum(filePointer, line);

    if (vertexCounter != vertexNum)
    {
        fprintf(stderr,
                "NUMBER OF VERTEXES DOESN'T MATCH SPECIFIED NUMBER");//TODO change to failureExit
        return EXIT_FAILURE;
    }

    rewind(filePointer);
    fgets(line, MAX_LINE_LENGTH, filePointer);

    int key = 0, tokenCounter = 1;
    char *checkedTokens[MAX_CHILDREN] = {NULL};
    while (fgets(line, MAX_LINE_LENGTH, filePointer) != NULL)
    {
        char *token = strtok(line, " ");

        if (strncmp(token, "-", 1) == 0)
        {
            key++;
            fprintf(stderr, "%s", line);
            continue;
        }

        int i = 0;
        while (token != NULL)
        {
            int tokenInteger = (int)strtol(token, NULL, 10);
            if (checkChildren(token) || isInCheckedTokens(checkedTokens, token, i) || tokenInteger == key)//TODO check if token == key
            {
                fprintf(stderr, "TOKEN IS KEY");//TODO change to failureExit
                return EXIT_FAILURE;
            }
            checkedTokens[i] = token;
            token = strtok(NULL, " ");
            tokenCounter++;
            i++;
        }
        key++;
        fprintf(stderr, "%s\n", line);
    }

    if (tokenCounter != vertexNum)
    {
        fprintf(stderr,
                "NUMBER OF VERTEXES DOESN'T MATCH SPECIFIED NUMBER");//TODO change to failureExit
        return EXIT_FAILURE;
    }
    return SUCCESS;
}

int processTree(FILE *filePointer, int fstVertex, int secVertex)
{
    char line[MAX_LINE_LENGTH];

    //Initialise array of pointers to vertices:
    fgets(line, MAX_LINE_LENGTH, filePointer);
    int vertexCounter = (int) strtol(line, NULL, 10);
    struct Vertex **vertices = (struct Vertex **) malloc(vertexCounter * sizeof(struct Vertex *));
    if (vertices == NULL)
    {
        fprintf(stderr, "MALLOC FAILED");
        free(vertices);
        vertices = NULL;
        return EXIT_FAILURE;
    }

    //Add vertices to array:
    for (int key = 0; key < vertexCounter; key++)
    {
        fgets(line, MAX_LINE_LENGTH, filePointer);

//        vertices[key] = (struct Vertex *) malloc(sizeof(struct Vertex));
//        if (vertices[key] == NULL)
//        {
//            fprintf(stderr, "MALLOC FAILED");
//            for (int i = 0; i < key; i++)
//            {
//                free(vertices[i]);
//                vertices[i] = NULL;
//            }
//            free(vertices);
//            vertices = NULL;
//            return EXIT_FAILURE;
//        }

        vertices[key]->key = key;
        vertices[key]->neighbors[0] = NO_PARENT_FLAG;

        char *token = strtok(line, " ");

        //If vertex is a leaf:
        if (strncmp(token, "-", 1) == 0)
        {
            vertices[key]->neighbors[1] = LEAF_FLAG;
            continue;
        }

        //Write neighbors of current vertex:
        int i = 1;
        while (token != NULL)
        {
            int tokenInteger = (int)strtol(token, NULL, 10);
            vertices[key]->neighbors[i] = tokenInteger; //TODO check if initialised

            //Write current key as father of current child:
            vertices[tokenInteger]->neighbors[0] = key;

            token = strtok(NULL, " ");
            i++;
        }
    }

    //Find root of the tree:
    struct Vertex *root = NULL;
    root = getRoot(vertexCounter, vertices, root);


    printDetails(fstVertex, secVertex, vertexCounter, root);
    //Free memory of vertices:
    free(vertices);
    vertices = NULL;
//    destroyVertices(vertexCounter, vertices);//TODO check if need to send pointer to pointer etc.
    return SUCCESS;
}

void printDetails(int fstVertex, int secVertex, int vertexCounter, const struct Vertex *root)
{
    fprintf(stdout, "Root Vertex: %d\n", root->key);
    fprintf(stdout, "Vertices Count: %d\n", vertexCounter);
    fprintf(stdout, "Edges Count: %d\n", vertexCounter - 1);
    fprintf(stdout, "Length of Minimal Branch: %d\n");
    fprintf(stdout, "Length of Maximal Branch: %d\n");
    fprintf(stdout, "Diameter Length: %d\n");
    fprintf(stdout, "Shortest Path Between %d and %d: ", fstVertex, secVertex);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, ARG_ERR_MSG);
        return EXIT_FAILURE;
    }

    FILE *filePointer = fopen(argv[1], "r");
    if (parseFile(filePointer))
    {
        fclose(filePointer);
        return EXIT_FAILURE;
    }

    rewind(filePointer);
//    if (processTree(filePointer))
//    {
//        fclose(filePointer);
//        return EXIT_FAILURE;
//    }
    fclose(filePointer);
    return SUCCESS;
}