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
 * Represents a vertex in the tree
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
 * Gets the root of the tree
 * @param vertexCounter the number of vertices in the tree
 * @param vertices the list of vertices of the tree
 * @param root a pointer that will hold the address of the tree's root
 * @return a pointer to the root vertex of the tree
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
 * An implementation of the BFS algorithm
 * used to find distances of all vertices from a given vertex
 * @param vertices the list of vertices of the tree
 * @param key the algorithm calculates the distances of all vertices from the vertex with this key
 * @param vertexCounter the number of vertices in the tree
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
 * @param vertexCounter the number of vertices in the tree
 * @param vertices the list of vertices of the tree
 * @param keyMaxDistance the key of the vertex which has the maximal distance from the tree's root
 * @return the diameter of the tree
 */
int getDiameter(int vertexCounter, struct Vertex *const *vertices, int keyMaxDistance)
{
	int maxDistance = 0;
	BFS(vertices, keyMaxDistance, vertexCounter);
	for (int i = 0; i < vertexCounter; i++)
	{
		if (vertices[i]->distance > maxDistance)
		{
			maxDistance = vertices[i]->distance;
		}
	}

    return maxDistance;
}

/**
 * Calculates the number of vertices that appear in the file
 * @param filePointer a pointer to the file that is given to the program
 * @param line a string that holds the current line in the file
 * @return the number of vertices that appear in the file
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
 * Checks if a char represents a digit
 * @param c the char to be checked
 * @return true if c is a char
 */
int isDigit(char c)
{
    return c >= '0' && c <= '9';
}

/**
 * Checks if a token is a whole positive number
 * @param token the token to be checked
 * @return true if token is a whole positive number
 */
int isPosInt(char *token)
{
    for (int i = 0; i < MAX_CHILDREN && token[i] != '\0'; i++)
    {
        if (!isDigit(token[i]) && token[i] != '\r' && token[i] != '\n')
        {
            return EXIT_FAILURE;
        }
    }
    return SUCCESS;
}

/**
 * Checks if a token already appeared before in the file
 * @param checkedTokens the list of tokens that have appeared already in the file
 * @param token the token to be checked
 * @param tokenCounter the number of tokens that have appeared already in the file
 * @return true if token appeared already in the file
 */
int isInCheckedTokens(char checkedTokens[MAX_CHILDREN][MAX_CHILDREN], char *token, int tokenCounter)
{
    for (int i = 0; i < tokenCounter; i++)
    {
    	//Token might appear at end of line:
        if (token[strlen(token) - 1] == '\n')
        {
            token[strlen(token) - 1] = 0;
        }
        //For Windows:
        if (token[strlen(token) - 1] == '\r')
        {
            token[strlen(token) - 1] = 0;
        }

        if (strcmp(checkedTokens[i], token) == 0)
        {
            return EXIT_FAILURE;
        }
    }
    return SUCCESS;
}

/**
 * Prints the details that the program should output
 * @param fstVertex the first vertex the program received
 * @param secVertex the second vertex the program received
 * @param vertexCounter the number of vertices in the tree
 * @param root the root of the tree
 * @param minDistance the minimal distance between the root and another vertex in the tree
 * @param maxDistance the maximal distance between the root and another vertex in the tree
 * @param diameter the length of the longest path in the tree
 * @param path an array that holds the keys of vertices on the path
 * 		  between the two vertices the program received
 * @param distance the distance between the two vertices the program received
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

	//Edge case: only one vertex in the tree:
	if (distance == 0)
	{
		fprintf(stdout, "%d\n", path[distance]);
	} else
	  {
		for (int i = 0; i < distance - 1; i++)
		{
			fprintf(stdout, "%d ", path[i]);
		}
		fprintf(stdout, "%d\n", path[distance - 1]);
	  }
}

/**
 * Frees the memory the program allocated for the pointers to the vertices
 * @param vertexCounter the number of vertices in the tree
 * @param vertices the list of vertices of the tree
 */
void destroyVertices(int vertexCounter, struct Vertex **vertices)
{
    for (int j = 0; j < vertexCounter; j++)
    {
        free(vertices[j]);
        vertices[j] = NULL;
    }
}

/**
 * Checks if the input to the program is valid
 * @param filePointer a pointer to the file the program received
 * @param fstVertex a pointer to the first vertex the program received
 * @param secVertex a pointer to the second vertex the program received
 * @return 0 if the input is valid, 1 otherwise
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
    //If not then get number of vertices from file:
    char line[MAX_LINE_LENGTH];
    if (fgets(line, MAX_LINE_LENGTH, filePointer) == NULL)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        return EXIT_FAILURE;
    }

    //Edge case: Check if number of vertices is a whole number:
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

    //Edge case: Check if vertexNum matches number of vertices:
    int vertexCounter = checkVertexNum(filePointer, line);

    if (vertexCounter != vertexNum)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        return EXIT_FAILURE;
    }

    //Check vertices in file:
    rewind(filePointer);
    fgets(line, MAX_LINE_LENGTH, filePointer);

    int key = 0, tokenCounter = 1;
    char checkedTokens[MAX_CHILDREN][MAX_CHILDREN] = {'\0'};
    while (fgets(line, MAX_LINE_LENGTH, filePointer) != NULL)
    {
        char *token = strtok(line, " ");

        //If the line represents a leaf:
        if (strncmp(token, "-", 1) == 0)
        {
            key++;
            continue;
        }

        while (token != NULL)
        {
            int tokenInteger = (int)strtol(token, NULL, 10);

            //Check if token is a valid number, appeared already or if it is equal to current key:
            if (isPosInt(token) || isInCheckedTokens(checkedTokens, token, tokenCounter)
				|| tokenInteger == key)
            {
				fprintf(stderr, INVALID_INPUT_MSG);
                return EXIT_FAILURE;
            }

            //Store the token in the next available cell in the list of checked tokens:
            strncpy(checkedTokens[tokenCounter], token, (int) strlen(token));

			tokenCounter++;
			token = strtok(NULL, " ");
        }
        key++;
    }

    if (tokenCounter != vertexNum)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        return EXIT_FAILURE;
    }

    //Check if given vertices exist and if they are legal:

    //Check if the given vertices are valid numbers:
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

    //Check if given vertices appear in the tree:
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
 * Builds the tree according to specifications given in the file
 * and output the relevant output messages
 * @param filePointer a pointer to the file the program received
 * @param fstVertex a pointer to the first vertex the program received
 * @param secVertex a pointer to the second vertex the program received
 * @return 0 if the processing succeeded, 1 otherwise
 */
int processTree(FILE *filePointer, char *fstVertex, char *secVertex)
{
    char line[MAX_LINE_LENGTH];

    //Initialise array of pointers to vertices:
    fgets(line, MAX_LINE_LENGTH, filePointer);
    int vertexCounter = (int) strtol(line, NULL, 10);
    struct Vertex **vertices = (struct Vertex **) malloc(vertexCounter * sizeof(struct Vertex *));

    //Check if allocation succeeded:
    if (vertices == NULL)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        free(vertices);
        return EXIT_FAILURE;
    }

    for (int k = 0; k < vertexCounter; k++)
    {
		//Initialise a pointer to current vertex:
        vertices[k] = (struct Vertex *) malloc(sizeof(struct Vertex));

		//Check if allocation succeeded:
        if (vertices[k] == NULL)
        {
			fprintf(stderr, INVALID_INPUT_MSG);

			//Free memory of vertices that were already allocated and of the list of vertices:
            for (int i = 0; i < k; i++)
            {
                free(vertices[i]);
                vertices[i] = NULL;
            }
            free(vertices);
            vertices = NULL;
            return EXIT_FAILURE;
        }

        vertices[k]->father = NULL;
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
        printDetails(fstVertex, secVertex, vertexCounter,
        		root, 0, 0, 0, path, 0);

        //Free memory of vertex:
        destroyVertices(vertexCounter, vertices);
        free(vertices);
        vertices = NULL;
        return SUCCESS;
    }

    //Find maximal branch:
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

    //Find minimal branch:
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

	//Calculate distances between the second given vertex to other vertices:
	BFS(vertices, vertex2, vertexCounter);

    int *path = (int *) malloc((vertices[vertex1]->distance + 1) * sizeof(int));

	//Check if allocation succeeded:
    if (path == NULL)
    {
		fprintf(stderr, INVALID_INPUT_MSG);
        free(path);

		//Free memory of vertices that were already allocated and of the list of vertices:
        for (int i = 0; i < vertexCounter; i++)
        {
            free(vertices[i]);
            vertices[i] = NULL;
        }
        free(vertices);
        vertices = NULL;
        return EXIT_FAILURE;
    }

	//Find path between the two given vertices and its length:
    path[0] = vertex1;
    int prev = vertices[vertex1]->prev;
    for (int l = 1; l < vertices[vertex1]->distance + 1; l++)
    {
        path[l] = prev;
        prev = vertices[prev]->prev;
    }

    printDetails(fstVertex, secVertex, vertexCounter, root, minDistance,
	     		 maxDistance, diameter, path, vertices[vertex1]->distance + 1);

    //Free memory:
    free(path);
    destroyVertices(vertexCounter, vertices);
    free(vertices);
    vertices = NULL;
    return SUCCESS;
}

/**
 * Runs the program TreeAnalyzer
 * @param argc the number of arguments the program received
 * @param argv the arguments the program received
 * @return 0 if program ended successfully, 1 otherwise
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