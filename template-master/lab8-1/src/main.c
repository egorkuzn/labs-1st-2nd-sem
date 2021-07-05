#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#define MAX_VERTEX_NUM 5000

typedef enum checkData { // type of statuses
    SUCCESS,
    NO_INPUT,
    BAD_VERTEX,
    BAD_LENGTH,
    NO_SPANNING_TREE,
    BAD_NUM_OF_LINES,
    BAD_NUM_OF_VERTICES,
    BAD_NUM_OF_EDGES
} workingResult;

typedef struct adjacencyMatrix { 
    size_t size; // matrixes' size
    unsigned int** data; // main matrix
} TMatrix;

typedef struct distanceToAdjacentVertex { // vertex and it's the shortest way
    unsigned int vertexNum;
    unsigned int minEdgeWeight;
} TDistToVertex;

TMatrix* InitMatrix(int userSize) { // initialization of matrix
    TMatrix* matrix = (TMatrix*)calloc(1, sizeof(TMatrix));
    assert(matrix != NULL);
    matrix->size = userSize;
    matrix->data = (unsigned int**)calloc(userSize, sizeof(unsigned int*));
    assert(matrix->data != NULL);
    for (size_t i = 0; i < matrix->size; i++) {
        matrix->data[i] = (unsigned int*)calloc(userSize, sizeof(unsigned int));
        assert(matrix->data[i] != NULL);
        for (size_t j = 0; j < matrix->size; j++) {
            matrix->data[i][j] = UINT_MAX;
        }
    }
    return matrix;
}

void FreeMatrix(TMatrix* matrix) {  // clearing matrix's memory
    for (size_t i = 0; i < matrix->size; i++) {
        free(matrix->data[i]);
    }
    free(matrix->data);
    free(matrix);
}


void PrintIOException() { // made for gitlab's tester
    printf("There is no input. Check your file to data");
}

int CheckNumberOfEdges(int numberOfEdges, int numberOfVertices) {
    return (numberOfEdges <= numberOfVertices * (numberOfVertices + 1) / 2);
}

int InputInitialValues(int* numberOfVertices, int* numberOfEdges) {
    int haveNextInput = 0;
    if (!scanf("%d", numberOfVertices)) {
        PrintIOException();
    }
    haveNextInput = scanf("%d", numberOfEdges);
    return haveNextInput == 1;
}

int IsBetweenBoundaries(long long int checkingValue, int leftBoundary, int rightBoundary) {
    return (checkingValue >= leftBoundary && checkingValue <= rightBoundary);
}

TMatrix* InputEdges(int numberOfVertices, int numberOfEdges, workingResult* controlValueOfInputVertexes) {
    TMatrix* matrix = InitMatrix(numberOfVertices);
    workingResult checkForCorrectData = SUCCESS;
    for (int idx = 0; idx < numberOfEdges; ++idx) {
        int begin, end;
        int haveNextInput = 0;
        if (!scanf("%d%d", &begin, &end)) {
            PrintIOException();
        }
        long long int currentWeight;
        haveNextInput = scanf("%lld", &currentWeight); // if count, it must to be '1'        
        if (haveNextInput != 1) {
            *controlValueOfInputVertexes = NO_INPUT;
            return matrix;
        }
        if (!IsBetweenBoundaries(currentWeight, 0, INT_MAX)) {
            checkForCorrectData = BAD_LENGTH;
            continue;
        }
        if (!(IsBetweenBoundaries(begin, 1, numberOfVertices) && IsBetweenBoundaries(end, 1, numberOfVertices))) {
            checkForCorrectData = BAD_VERTEX;
            continue;
        }
        begin--; // index is a number of vertex minuse '1'
        end--;
        matrix->data[begin][end] = (unsigned int)currentWeight;
        matrix->data[end][begin] = (unsigned int)currentWeight;
    }
    *controlValueOfInputVertexes = checkForCorrectData;
    return matrix;
}

TDistToVertex* InitMinDistArray(unsigned int numOfVertices) {
    TDistToVertex* minDist = (TDistToVertex*)calloc(numOfVertices, sizeof(TDistToVertex));
    assert(minDist != NULL);
    for (unsigned int i = 0; i < numOfVertices; i++) {
        minDist[i].vertexNum = UINT_MAX;
        minDist[i].minEdgeWeight = UINT_MAX;
    }
    return minDist;
}

unsigned int GetNextVertexWithMinEdgeWeight(size_t matrixSize, bool* isVisited, TDistToVertex* minDists, bool* isThereNextEdge) { // searching of the minimum unvisited vertex
    unsigned int nextVertexWithMinEdgeWeight = 0;
    for (size_t columnIdx = 0; columnIdx < matrixSize; columnIdx++) {
        if (minDists[columnIdx].minEdgeWeight < minDists[nextVertexWithMinEdgeWeight].minEdgeWeight && !isVisited[columnIdx]) {
            *isThereNextEdge = true;
            nextVertexWithMinEdgeWeight = columnIdx;
        }
    }
    return nextVertexWithMinEdgeWeight;
}

void UpdateMinDestinationsToUnvisitedVertices(TMatrix* matrix, unsigned int newMSTVertex, bool* isVisited, TDistToVertex* minDists) {
    for (size_t columnIdx = 0; columnIdx < matrix->size; columnIdx++) {
        if (matrix->data[newMSTVertex][columnIdx] < minDists[columnIdx].minEdgeWeight && !isVisited[columnIdx]) {
            minDists[columnIdx].vertexNum = newMSTVertex;
            minDists[columnIdx].minEdgeWeight = matrix->data[newMSTVertex][columnIdx];
        }
    }
}

TDistToVertex* FindMinimumSpanningTree( unsigned int numberOfVertices, workingResult* status, TMatrix* matrix) { // common function
    bool* isVisited = (bool*)calloc(numberOfVertices, sizeof(bool));
    assert(isVisited != NULL);
    memset(isVisited, false, sizeof(bool) * numberOfVertices);
    TDistToVertex* minDists = InitMinDistArray(numberOfVertices);
    unsigned int newVertexInMSTree = 0;
    isVisited[0] = true; // strating from one of the vetices
    UpdateMinDestinationsToUnvisitedVertices(matrix, newVertexInMSTree, isVisited, minDists);
    bool isThereNextEdge = false;
    unsigned int nextVertexWithMinEdgeWeight = GetNextVertexWithMinEdgeWeight(matrix->size, isVisited, minDists, &isThereNextEdge);
    for (size_t strIdx = 0; strIdx < matrix->size - 1; strIdx++) {
        if (!isThereNextEdge) {
            *status = NO_SPANNING_TREE;
            break;
        }
        isVisited[nextVertexWithMinEdgeWeight] = true;
        newVertexInMSTree = nextVertexWithMinEdgeWeight;
        UpdateMinDestinationsToUnvisitedVertices(matrix, newVertexInMSTree, isVisited, minDists);
        isThereNextEdge = false;
        nextVertexWithMinEdgeWeight = GetNextVertexWithMinEdgeWeight(matrix->size, isVisited, minDists, &isThereNextEdge);
    }
    free(isVisited);
    return minDists;
}

void PrintMessageByStatus( workingResult status) { // all bad situations
    switch (status) {
    case NO_INPUT:
        printf("bad number of lines");
        break;
    case BAD_VERTEX:
        printf("bad vertex");
        break;
    case BAD_LENGTH:
        printf("bad length");
        break;
    case BAD_NUM_OF_LINES:
        printf("bad number of lines");
        break;
    case BAD_NUM_OF_VERTICES:
        printf("bad number of vertices");
        break;
    case BAD_NUM_OF_EDGES:
        printf("bad number of edges");
        break;
    case NO_SPANNING_TREE:
        printf("no spanning tree");
        break;
    default:
        break;
    }
}

void PrintMinimumSpanningTree(int numberOfVertices, TDistToVertex* minDists) {
    for (int i = 0; i < numberOfVertices; i++) {
        if (minDists[i].minEdgeWeight != UINT_MAX) {
            printf("%d %u\n", i + 1, minDists[i].vertexNum + 1);
        }
    }
}

int main(void) {
    int numberOfVertices;
    int numberOfEdges;
    workingResult status = SUCCESS;
    if (!InputInitialValues(&numberOfVertices, &numberOfEdges)) {
        status = BAD_NUM_OF_LINES;
    }
    else if (!IsBetweenBoundaries(numberOfVertices, 0, MAX_VERTEX_NUM)) {
        status = BAD_NUM_OF_VERTICES;
    }
    else if (!CheckNumberOfEdges(numberOfEdges, numberOfVertices)) {
        status = BAD_NUM_OF_EDGES;
    }
    TMatrix* matrix = NULL;
    TDistToVertex* minDists = NULL;
    if (status == SUCCESS) {
        matrix = InputEdges(numberOfVertices, numberOfEdges, &status);
    }
    if (status == SUCCESS) {
        if (numberOfVertices == 0 || (numberOfEdges == 0 && numberOfVertices != 1)) {
            status = NO_SPANNING_TREE;
        }
    }
    if (status == SUCCESS) {
        minDists = FindMinimumSpanningTree(numberOfVertices, &status, matrix);
    }
    if (status == SUCCESS) {
        PrintMinimumSpanningTree(numberOfVertices, minDists);
    }
    else {
        PrintMessageByStatus(status);
    }
    if (matrix != NULL) {
        FreeMatrix(matrix);
    }
    if (minDists != NULL) {
        free(minDists);
    }
    return 0;
}
