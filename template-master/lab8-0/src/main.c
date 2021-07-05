#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

typedef struct edge {
    int from;
    int to;
    int weight;
} edge;

void swap(void* a, void* b, size_t size) {   
    for (size_t i = 0; i < size; i++) {
        char tmp;
        tmp = *((char*)b + i);
        *((char*)b + i) = *((char*)a + i);
        *((char*)a + i) = tmp;
    }
}

int FindSet(int* parent, int v) {
    if (v == parent[v]) {
        return v;
    }
    return parent[v] = FindSet(parent, parent[v]);
}

void UnionSets(int* parent, int* rank, int v1, int v2) {
    v1 = parent[v1];
    v2 = parent[v2];
    if (v1 == v2) {
        return;
    }
    if (rank[v1] < rank[v2]) {
        swap(&v1, &v2, sizeof(int));
    }
    parent[v2] = v1;
    if (rank[v1] == rank[v2]) {
        rank[v1]++;
    }
}

int CompareNodes(const void* a, const void* b) {  
    return ((*(edge*)a).weight - (*(edge*)b).weight);
}

void TotalFree(int* parent, int* rank, int* frame, edge* edges) {
    free(parent);
    free(rank);
    free(frame);
    free(edges);
}

int ReadData(int M, int N, edge* edges, FILE* fin, FILE* fout) {
    for (int i = 0; i < M; ++i) {
        if (fscanf(fin, "%d %d %d\n", &edges[i].from, &edges[i].to, &edges[i].weight) == EOF) {
            fprintf(fout, "bad number of lines");
            return -1;
        }
        if (edges[i].from < 1 || edges[i].from > N || edges[i].to < 1 || edges[i].to > N) {
            fprintf(fout, "bad vertex");
            return -1;
        }
        if (edges[i].weight < 0 || edges[i].weight > INT_MAX) {
            fprintf(fout, "bad length");
            return -1;
        }
        edges[i].from--;
        edges[i].to--;
    }
    return 0;
}

int main() {
    FILE* fin = fopen("in.txt", "r");
    FILE* fout = fopen("out.txt", "w");
    int N, M;
    if (fscanf(fin, "%d\n%d\n", &N, &M) == EOF) {
        fprintf(fout, "bad number of lines");
        fclose(fin);
        fclose(fout);
        return 0;
    }
    if (N < 0 || N > 5000) {
        fprintf(fout, "bad number of vertices");
        fclose(fin);
        fclose(fout);
        return 0;
    }
    if (M < 0 || M > N * (N - 1) / 2) {
        fprintf(fout, "bad number of edges");
        fclose(fin);
        fclose(fout);
        return 0;
    }
    if (N == 0 || (M == 0 && N != 1)) {
        fprintf(fout, "no spanning tree");
        fclose(fin);
        fclose(fout);
        return 0;
    }
    int* parent = (int*)calloc(N, sizeof(int));
    assert(parent != NULL);
    int* rank = (int*)calloc(N, sizeof(int));
    assert(rank != NULL);
    int* frame = (int*)calloc(M, sizeof(int));
    assert(frame != NULL);
    edge* edges = (edge*)calloc(M, sizeof(edge));
    assert(edges != NULL);
    if (ReadData(M,N,edges, fin, fout) == -1) {
        TotalFree(parent, rank, frame, edges);
        fclose(fin);
        fclose(fout);
        return 0;
    }
    if (N == 1) {
        TotalFree(parent, rank, frame, edges);
        fclose(fin);
        fclose(fout);
        return 0;
    }
    if (M < N - 1) {
        fprintf(fout, "no spanning tree");
        TotalFree(parent, rank, frame, edges);
        fclose(fin);
        fclose(fout);
        return 0;
    }
    qsort(edges, M, sizeof(edge), CompareNodes);
    for (int i = 0; i < N; ++i) {
        parent[i] = i;
        rank[i] = 0;
    }
    int frame_pos = 0;
    for (int i = 0; i < M; ++i) {
        if (FindSet(&parent[0], edges[i].from) == FindSet(&parent[0], edges[i].to)) {
            continue;
        }
        UnionSets(&parent[0], &rank[0], edges[i].from, edges[i].to);
        frame[frame_pos] = i;
        frame_pos++;
    }
    int root = parent[0];
    for (int i = 0; i < N; ++i) {
        if (root != FindSet(&parent[0], i)) {
            fprintf(fout, "no spanning tree");
            TotalFree(parent, rank, frame, edges);
            fclose(fin);
            fclose(fout);
            return 0;
        }
    }
    for (int i = 0; i < frame_pos; ++i) {
        fprintf(fout, "%d %d\n", edges[frame[i]].from + 1, edges[frame[i]].to + 1);
    }
    TotalFree(parent, rank, frame, edges);
    fclose(fin);
    fclose(fout);
    return 0;
}
