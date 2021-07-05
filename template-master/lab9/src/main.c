#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_VERTICES_SIZE 5000

typedef enum workingResult {//type to control situation 
    badWeight,
    badVertex,
    success,
    noPath,
    overflow,
    pathCorrect,
    badNumOfLines,
    badNumOfVertices,
    badNumOfEdges
} workingResult;

typedef struct adjacencyMatrix { // matrix of ways 
    size_t size;
    unsigned int** data;
} TMatrix;

typedef struct minDist { // the shortest ways array
    size_t size;
    long long int* data;
} TMinDist;

typedef struct intVector {
    size_t capacity;
    size_t size;
    int* vector;
} TVector;

int CheckNumberOfVertices(int numberOfVertices) {
    return numberOfVertices >= 0 && numberOfVertices <= MAX_VERTICES_SIZE;
}

int CheckNumberOfEdges(int numberOfEdges, int numberOfVertices) {
    return numberOfEdges >= 0 && numberOfEdges <= numberOfVertices * (numberOfVertices + 1) / 2;
}

TMatrix* initMatrix(int userSize) {
    TMatrix* matrix = malloc(sizeof(TMatrix));
    assert(matrix != NULL);
    matrix->size = userSize;
    matrix->data = malloc(userSize * sizeof(int*));
    assert(matrix->data != NULL);
    for (size_t i = 0; i < matrix->size; i++) {
        matrix->data[i] = malloc(sizeof(int) * userSize);
        assert(matrix->data[i] != NULL);
        memset(matrix->data[i], 0, userSize * sizeof(int));
    }
    return matrix;
}

int IsBetweenBoundaries(long long int checkingNumber, int leftBoundary, int rightBoundary) {
    return checkingNumber >= leftBoundary && checkingNumber <= rightBoundary;
}

void ResizeVector(TVector* vector, size_t newSize) {
    if (newSize <= vector->capacity) {
        vector->size = newSize;
    }
    else {
        assert("Increasing vector size is not supported for now");
    }
}

TMinDist* InitMinDistancesArray(int start, int userSize) { // creating array of the shortest ways
    TMinDist* minDist = malloc(sizeof(TMinDist));
    assert(minDist != NULL);
    minDist->data = malloc(sizeof(long long int) * userSize);
    assert(minDist->data != NULL);
    minDist->size = userSize;
    for (int i = 0; i < userSize; i++) {
        minDist->data[i] = LLONG_MAX;
    }
    minDist->data[start - 1] = 0;
    return minDist;
}

void FreeMatrix(TMatrix* matrix) {
    for (size_t i = 0; i < matrix->size; i++) {
        free(matrix->data[i]);
    }
    free(matrix->data);
    free(matrix);
}

void TotalFree(TMinDist* minDistances, TVector* path, TMatrix* matrix) {// clear used memory
    if (matrix != NULL) {
        FreeMatrix(matrix);
    }
    if (minDistances != NULL) {
        free(minDistances->data);
        free(minDistances);
    }
    if (path != NULL) {
        free(path->vector);
        free(path);
    }

}

int InputInitialValues(int* numberOfVertices, int* start, int* finish, int* numberOfEdges) { // string getter
    int haveNextInput = scanf("%d%d%d%d", numberOfVertices, start, finish, numberOfEdges);
    return haveNextInput || *start < 1 || *numberOfVertices < *start;
}

TMatrix* InputEdges(int numberOfEdges, int numberOfVertices, workingResult* inputController) {
    TMatrix* matrix = initMatrix(numberOfVertices);
    if (numberOfVertices == 0) {
        *inputController = badVertex;
    }
    for (long long int i = 0; i < numberOfEdges; i++) {
        int from, to;
        long long int weight;
        if (!scanf("%d%d%lld", &from, &to, &weight)) {
            *inputController = badNumOfLines;
            break;
        }
        if (!IsBetweenBoundaries(from, 1, numberOfVertices) || !IsBetweenBoundaries(to, 1, numberOfVertices)) {
            *inputController = badVertex;
            continue;
        }
        if (!IsBetweenBoundaries(weight, 0, INT_MAX)) {
            *inputController = badWeight;
            continue;
        }
        from--;
        to--;
        matrix->data[from][to] = (unsigned int)weight;
        matrix->data[to][from] = (unsigned int)weight;
    }
    return matrix;
}

TMinDist* FindAllShortestPath(int start, TMatrix* matrix) {
    TMinDist* minDistances = InitMinDistancesArray(start, matrix->size);
    bool* verticesToVisit = malloc(sizeof(bool) * minDistances->size);
    assert(verticesToVisit != NULL);
    memset(verticesToVisit, true, minDistances->size * sizeof(bool));
    int minimumIdx;
    do {
        minimumIdx = MAX_VERTICES_SIZE;
        long long int minValueOfPath = LLONG_MAX;
        for (size_t i = 0; i < minDistances->size; i++) {
            if ((verticesToVisit[i] == true) && (minDistances->data[i] < minValueOfPath)) {
                minValueOfPath = minDistances->data[i];
                minimumIdx = i;
            }
        }
        if (minimumIdx != MAX_VERTICES_SIZE) {
            for (size_t i = 0; i < minDistances->size; i++) {
                long long int weightOfCurrentEdge = matrix->data[minimumIdx][i];
                if (weightOfCurrentEdge > 0) {
                    long long int temp = minValueOfPath + weightOfCurrentEdge;
                    if (temp < minDistances->data[i]) {
                        minDistances->data[i] = temp;
                    }
                }
            }
            verticesToVisit[minimumIdx] = false;
        }
    } while (minimumIdx < MAX_VERTICES_SIZE);
    free(verticesToVisit);

    return minDistances;
}

void PrintPathsToAllVertices(TMinDist* minDistances) {
    for (size_t i = 0; i < minDistances->size; i++) {
        if (minDistances->data[i] == LLONG_MAX) {
            printf("oo ");
        }
        else if (minDistances->data[i] > INT_MAX) {
            printf("INT_MAX+ ");
        }
        else {
            printf("%lld ", minDistances->data[i]);
        }
    }
    printf("\n");
}

TVector* CreateVector(int userSize) {
    TVector* initPath = malloc(sizeof(TVector));
    assert(initPath != NULL);
    initPath->vector = malloc(sizeof(int) * userSize);
    assert(initPath->vector != NULL);
    initPath->size = userSize;
    initPath->capacity = userSize;
    return initPath;
}

TVector* GetShortestPath(int start, int finish, TMinDist* minDistances, TMatrix* matrix, workingResult* controlValueOfShortestPath) {
    TVector* path = CreateVector(matrix->size);
    int endOfPath = finish - 1;
    int pathIndex = 1;
    path->vector[0] = endOfPath + 1;
    long long int weight = minDistances->data[endOfPath];
    bool isThereNextPath = 0;
    bool isMoreThanOneBigPath = 0;
    while (endOfPath != start - 1) {
        for (size_t i = 0; i < path->size; i++) {
            if (matrix->data[endOfPath][i] != 0) {
                isThereNextPath = 1;
                long long int workingElement = matrix->data[endOfPath][i];
                long long int currentValueOfWeight = weight - workingElement;
                if (currentValueOfWeight == minDistances->data[i]) {
                    if (currentValueOfWeight == INT_MAX) {
                        for (size_t j = 0; j < path->size; j++) {
                            if (minDistances->data[endOfPath] - minDistances->data[j] == currentValueOfWeight &&
                                j != i) {
                                isMoreThanOneBigPath = 1;
                            }
                        }
                    }
                    weight = currentValueOfWeight;
                    endOfPath = i;
                    path->vector[pathIndex] = (int)i + 1;
                    pathIndex += 1;
                    break;
                }
            }
            if (i + 1 == path->size && !isThereNextPath) {
                *controlValueOfShortestPath = noPath;
                return path;
            }
        }
    }
    if (isMoreThanOneBigPath) {
        *controlValueOfShortestPath = overflow;
    }
    else {
        *controlValueOfShortestPath = pathCorrect;
    }
    ResizeVector(path, pathIndex);
    return path;
}

void PrintVector(TVector* path) {
    for (size_t i = 0; i < path->size; i++) {
        printf("%d ", path->vector[i]);
    }
}

bool IsOverflow(workingResult controlValueOfShortestPath, TMinDist* minDistances, int finish) {
    return controlValueOfShortestPath == overflow && minDistances->data[finish - 1] > INT_MAX;
}

void PrintMessageByStatus(workingResult status) {
    switch (status) {
    case badNumOfLines:
        printf("bad number of lines");
        break;
    case badNumOfEdges:
        printf("bad number of edges");
        break;
    case badVertex:
        printf("bad vertex");
        break;
    case noPath:
        printf("no path");
        break;
    case overflow:
        printf("overflow");
        break;
    case badNumOfVertices:
        printf("bad number of vertices");
        break;
    case badWeight:
        printf("bad length");
        break;
    default:
        break;
    }
}


int main() {
    int numberOfVertices, numberOfEdges;
    int start, finish;
    workingResult status = success;
    if (!InputInitialValues(&numberOfVertices, &start, &finish, &numberOfEdges)) {
        status = badNumOfLines;
    }
    else if (!CheckNumberOfVertices(numberOfVertices)) {
        status = badNumOfVertices;
    }
    else if (!CheckNumberOfEdges(numberOfEdges, numberOfVertices)) {
        status = badNumOfEdges;
    }
    TMatrix* matrix = NULL;
    TMinDist* minDistances = NULL;
    TVector* path = NULL;
    if (status == success) {
        matrix = InputEdges(numberOfEdges, numberOfVertices, &status);
    }
    if (status == success) {
        minDistances = FindAllShortestPath(start, matrix);
        PrintPathsToAllVertices(minDistances);
    }
    if (status == success) {
        path = GetShortestPath(start, finish, minDistances, matrix, &status);
    }
    if (IsOverflow(status, minDistances, finish)) {
        status = overflow;
    }
    if (status == pathCorrect) {
        PrintVector(path);
    }
    else {
        PrintMessageByStatus(status);
    }
    TotalFree(minDistances, path, matrix);
    return 0;
}

