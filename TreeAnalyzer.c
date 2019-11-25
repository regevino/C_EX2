#include "queue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ARG_ERR_MSG "Usage: TreeAnalyzer <Graph File Path> <First Vertex> <Second Vertex>\n"
#define INVALID_INPUT_MSG "Invalid input\n"
#define MAX_LINE_LENGTH 1025
#define MAX_CHILDREN 512
#define SUCCESS 0
#define LEAF_FLAG (-1)
#define NOT_VISITED (-1)
#define FST_VERTEX_PREV (-1)
#define END_OF_NEIGHBORS (-1)
/**
 *
 */
struct Vertex
{
    int children[MAX_CHILDREN + 1];
    struct Vertex *father;
    int key;
    int distance;
    int prev;
};

/**
 *
 * @param vertexCounter
 * @param vertices
 * @param root
 * @return
 */
struct Vertex *getRoot(int vertexCounter, struct Vertex *const *vertices, struct Vertex **root)
{
    for (int k = 0; k < vertexCounter; k++)
    {
        if (vertices[k]->father == NULL)
        {
            *root = vertices[k];
        }
    }
    return *root;
}

/**
 *
 * @param vertices
 * @param key
 * @param vertexCounter
 */
void BFS(struct Vertex *const *vertices, int key, int vertexCounter)
{
	for (int i = 0; i < vertexCounter; i++)
	{
		vertices[i]->distance = NOT_VISITED;
	}
	vertices[key]->distance = 0;
	vertices[key]->prev = FST_VERTEX_PREV;
	Queue *queue = allocQueue();
	enqueue(queue, key);
	while (!queueIsEmpty(queue))
	{
		unsigned int u = dequeue(queue);
		for (int i = 0; i < MAX_CHILDREN + 1 && vertices[u]->children[i] != END_OF_NEIGHBORS; i++)
		{
			int w = vertices[u]->children[i];
			if (vertices[w]->distance == NOT_VISITED)
			{
				enqueue(queue, w);
				vertices[w]->prev = (int)u;
				vertices[w]->distance = vertices[u]->distance + 1;
			}
		}
		if (vertices[u]->father != NULL)
		{
			if (vertices[u]->father->distance == NOT_VISITED)
			{
				enqueue(queue, vertices[u]->father->key);
				vertices[u]->father->prev = (int)u;
				vertices[u]->father->distance = vertices[u]->distance + 1;
			}
		}
	}
	freeQueue(&queue);
}

/**
 *
 * @param vertexCounter
 * @param vertices
 * @param maxDistance
 * @param keyMaxDistance
 * @return
 */
int getDiameter(int vertexCounter, struct Vertex *const *vertices, int keyMaxDistance)
{
	int maxDistance = 0;
	BFS(vertices, keyMaxDistance, vertexCounter);
	for (int i = 0; i < vertexCounter; i++)
	{
		if (vertices[i]->children[0] == LEAF_FLAG)
		{
			if (vertices[i]->distance > maxDistance)
			{
				maxDistance = vertices[i]->distance;
			}
		}
	}

    return maxDistance;
}


/**
 *
 * @return
 */
int failureExit()
{
    fprintf(stderr, INVALID_INPUT_MSG);
    return EXIT_FAILURE;
}

/**
 *
 * @param filePointer
 * @param line
 * @return
 */
int checkVertexNum(FILE *filePointer, char *line)
{
    int vertexCounter = 0;
    while (fgets(line, MAX_LINE_LENGTH, filePointer) != NULL)
    {
        vertexCounter++;
    }
    return vertexCounter;
}

/**
 *
 * @param c
 * @return
 */
int isDigit(char c)
{
    return c >= '0' && c <= '9';
}

/**
 *
 * @param token
 * @return
 */
int checkChildren(char *token)
{
    for (int i = 0; i < MAX_CHILDREN && token[i] != '\0'; i++)
    {
        if (!isDigit(token[i]) && token[i] != '\r' && token[i] != '\n')
        {
			fprintf(stderr, INVALID_INPUT_MSG);
            return EXIT_FAILURE;
        }
    }
    return SUCCESS;
}

/**
 *
 * @param checkedTokens
 * @param token
 * @param length
 * @return
 */
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
			fprintf(stderr, INVALID_INPUT_MSG);
            return EXIT_FAILURE;
        }
    }
    return SUCCESS;
}

/**
 *
 * @param fstVertex
 * @param secVertex
 * @param vertexCounter
 * @param root
 */
void printDetails(char *fstVertex, char *secVertex, int vertexCounter, const struct Vertex *root,
				  int minDistance, int maxDistance, int diameter, int *path, int distance)
{
	fprintf(stdout, "Root Vertex: %d\n", root->key);
	fprintf(stdout, "Vertices Count: %d\n", vertexCounter);
	fprintf(stdout, "Edges Count: %d\n", vertexCounter - 1);
    fprintf(stdout, "Length of Minimal Branch: %d\n", minDistance);
    fprintf(stdout, "Length of Maximal Branch: %d\n", maxDistance);
    fprintf(stdout, "Diameter Length: %d\n", diameter);
	fprintf(stdout, "Shortest Path Between %s and %s: ", fstVertex, secVertex);
    for (int i = 0; i < distance - 1; i++)
    {
        fprintf(stdout, "%d ", path[i]);
    }
    fprintf(stdout, "%d\n", path[distance - 1]);
}

void destroyVertices(int vertexCounter, struct Vertex **vertices)
{
    for (int j = 0; j < vertexCounter; j++)
    {
        free(vertices[j]);
        vertices[j] = NULL;
    }
}

/**
 *
 * @param filePointer
 * @return
 */
int parseFile(FILE *filePointer, char *fstVertex, char *secVertex)
{
    //Edge case: Check if file exists:
    if (filePointer == NULL)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        return EXIT_FAILURE;
    }

    //Edge case: Check if file is empty.
    //If not then get number of vertexes from file:
    char line[MAX_LINE_LENGTH];
    if (fgets(line, MAX_LINE_LENGTH, filePointer) == NULL)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        return EXIT_FAILURE;
    }

    //Edge case: Check if number of vertexes is a whole number:
    for (int i = 0; i < MAX_LINE_LENGTH && line[i] != '\r' && line[i] != '\n'; i++)
    {
        if (!isDigit(line[i]))
        {
			fprintf(stderr, INVALID_INPUT_MSG);
            return EXIT_FAILURE;
        }
    }

    int vertexNum = (int) strtol(line, NULL, 10);

    //Edge case: Check if vertexNum is 0:
    if (vertexNum == 0)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        return EXIT_FAILURE;
    }

    //Edge case: Check if vertexNum matches number of vertexes:
    int vertexCounter = checkVertexNum(filePointer, line);

    if (vertexCounter != vertexNum)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        return EXIT_FAILURE;
    }

    rewind(filePointer);
    fgets(line, MAX_LINE_LENGTH, filePointer);

    int key = 0, tokenCounter = 1, i = 0;
    char *checkedTokens[MAX_CHILDREN];
    char currentToken[MAX_CHILDREN];
    while (fgets(line, MAX_LINE_LENGTH, filePointer) != NULL)
    {
        char *token = strtok(line, " ");

        if (strncmp(token, "-", 1) == 0)
        {
            key++;
            continue;
        }

        while (token != NULL)
        {
            int tokenInteger = (int)strtol(token, NULL, 10);
            if (checkChildren(token) || isInCheckedTokens(checkedTokens, token, i)
            	|| tokenInteger == key)
            {
				fprintf(stderr, INVALID_INPUT_MSG);
                return EXIT_FAILURE;
            }

            strncpy(currentToken, token, (int) strlen(token));
			checkedTokens[i] = currentToken;
            token = strtok(NULL, " ");
            tokenCounter++;
            i++;
        }
        key++;
    }

    if (tokenCounter != vertexNum)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        return EXIT_FAILURE;
    }

    //Check if given vertices exist and if they are legal:

    for (int j = 0; j < vertexNum && fstVertex[j] != '\0'; j++)
    {
        if (!isDigit(fstVertex[j]))
        {
			fprintf(stderr, INVALID_INPUT_MSG);
            return EXIT_FAILURE;
        }
    }
    for (int j = 0; j < vertexNum && secVertex[j] != '\0'; j++)
    {
        if (!isDigit(secVertex[j]))
        {
			fprintf(stderr, INVALID_INPUT_MSG);
            return EXIT_FAILURE;
        }
    }
    int vertex1 = (int)strtol(fstVertex, NULL, 10);
    int vertex2 = (int)strtol(secVertex, NULL, 10);
    if (vertex1 < 0 || vertex1 >= vertexNum || vertex2 < 0 || vertex2 >= vertexNum)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        return EXIT_FAILURE;
    }
    return SUCCESS;
}

/**
 *
 * @param filePointer
 * @param fstVertex
 * @param secVertex
 * @return
 */
int processTree(FILE *filePointer, char *fstVertex, char *secVertex)
{
    char line[MAX_LINE_LENGTH];

    //Initialise array of pointers to vertices:
    fgets(line, MAX_LINE_LENGTH, filePointer);
    int vertexCounter = (int) strtol(line, NULL, 10);
    struct Vertex **vertices = (struct Vertex **) malloc(vertexCounter * sizeof(struct Vertex *));
    if (vertices == NULL)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        free(vertices);
        return EXIT_FAILURE;
    }

    for (int k = 0; k < vertexCounter; k++)
    {
        vertices[k] = (struct Vertex *) malloc(sizeof(struct Vertex));
        if (vertices[k] == NULL)
        {
			fprintf(stderr, INVALID_INPUT_MSG);
            for (int i = 0; i < k; i++)
            {
                free(vertices[i]);
                vertices[i] = NULL;
            }
            free(vertices);
            vertices = NULL;
            return EXIT_FAILURE;
        }
    }

    //Add vertices to array:
    for (int key = 0; key < vertexCounter; key++)
    {
        fgets(line, MAX_LINE_LENGTH, filePointer);

        vertices[key]->key = key;

        char *token = strtok(line, " ");

        //If vertex is a leaf:
        if (strncmp(token, "-", 1) == 0)
        {
            vertices[key]->children[0] = LEAF_FLAG;
            continue;
        }

        //Write neighbors of current vertex:
        int i = 0;
        while (token != NULL)
        {
            int tokenInteger = (int)strtol(token, NULL, 10);
            vertices[key]->children[i] = tokenInteger;

            //Write current key as father of current child:
            vertices[tokenInteger]->father = vertices[key];

            token = strtok(NULL, " ");
            i++;
        }
		vertices[key]->children[i] = END_OF_NEIGHBORS;
	}

    //Find root of the tree:
    struct Vertex *root = NULL;
    root = getRoot(vertexCounter, vertices, &root);

    //Edge case: A tree of one vertex:
    if (vertexCounter == 1)
    {
        int path[] = {root->key};
        printDetails(fstVertex, secVertex, vertexCounter, root, 0, 0, 0, path, 0);

        //Free memory of vertices:
//        destroyVertices(vertexCounter, vertices);
		for (int j = 0; j < vertexCounter; j++)
		{
			free(vertices[j]);
			vertices[j] = NULL;
		}
        free(vertices);
        vertices = NULL;
        return SUCCESS;
    }

    //Find maximal and minimal branches:
    BFS(vertices, root->key, vertexCounter);
    int maxDistance = 1, keyMaxDistance = 0;
    for (int j = 0; j < vertexCounter; j++)
    {
        if (vertices[j]->children[0] == LEAF_FLAG)
        {
            if (vertices[j]->distance > maxDistance)
            {
                maxDistance = vertices[j]->distance;
                keyMaxDistance = j;
            }
        }
    }

	int minDistance = maxDistance;
	for (int m = 0; m < vertexCounter; m++)
	{
		if (vertices[m]->children[0] == LEAF_FLAG)
		{
			if (vertices[m]->distance < minDistance)
            {
                minDistance = vertices[m]->distance;
            }
		}
	}

    int diameter = getDiameter(vertexCounter, vertices, keyMaxDistance);

    int vertex1 = (int) strtol(fstVertex, NULL, 10);
    int vertex2 = (int) strtol(secVertex, NULL, 10);
    BFS(vertices, vertex2, vertexCounter);
    int *path = (int *) malloc((vertices[vertex1]->distance + 1) * sizeof(int));
    if (path == NULL)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        free(path);
        for (int i = 0; i < vertexCounter; i++)
        {
            free(vertices[i]);
            vertices[i] = NULL;
        }
        free(vertices);
        vertices = NULL;
        return EXIT_FAILURE;
    }
    path[0] = vertex1;
    int prev = vertices[vertex1]->prev;
    for (int l = 1; l < vertices[vertex1]->distance + 1; l++)
    {
        path[l] = prev;
        prev = vertices[prev]->prev;
    }

    printDetails(fstVertex, secVertex, vertexCounter, root, minDistance,
	     		 maxDistance, diameter, path, vertices[vertex1]->distance + 1);
    //Free memory of vertices:
    free(path);
	for (int j = 0; j < vertexCounter; j++)
	{
		free(vertices[j]);
		vertices[j] = NULL;
	}
//    destroyVertices(vertexCounter, vertices);
    free(vertices);
    vertices = NULL;
    return SUCCESS;
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, ARG_ERR_MSG);
        return EXIT_FAILURE;
    }

    FILE *filePointer = fopen(argv[1], "r");
    if (parseFile(filePointer, argv[2], argv[3]))
    {
        fclose(filePointer);
        return EXIT_FAILURE;
    }

    rewind(filePointer);
    if (processTree(filePointer, argv[2], argv[3]))
    {
        fclose(filePointer);
        return EXIT_FAILURE;
    }
    fclose(filePointer);
    return SUCCESS;
}