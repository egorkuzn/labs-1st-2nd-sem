#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

char DFS(char* used, char* input_data, short* output, short N, short cur, short* cur_out, FILE* fout) {
    if (used[cur] == -1) { //vertex is black
        return 0;
    }
    if (used[cur] == 1) { //cycles detecting
        return -1;
    }
    used[cur] = 1; //vertex's visit : grey
    for (short i = 0; i < N; ++i) {
        int number = cur * N + i;
        if ((input_data[number / 8] & (1 << (number % 8))) == 1 << (number % 8)) { //checking way from cur+1 to i+1 by using of mask
            if (DFS(used, input_data, output, N, i, cur_out, fout) == -1) {
                return -1;
            }
        }
    }
    used[cur] = -1; //make vertex black
    output[*cur_out] = cur; //add in stack
    ++(*cur_out);
    return 0;
}

char CheckVerticesEdges(short N, int M, FILE* fout) {
    if (N < 0 || N > 2000) {
        fprintf(fout, "bad number of vertices");
        return -1;
    }
    if (M < 0 || M > N * (N + 1) / 2) {
        fprintf(fout, "bad number of edges");
        return -1;
    }
    return 0;
}

char TopSort(char* input_data, short* output, short N, short* cur_out, FILE* fout) {// there is used Tarjan's algo in realisation
    char* used = (char*)calloc(N, sizeof(char));
    assert(used != NULL);
    for (short i = 0; i < N; ++i) {
        used[i] = 0; //firstly all white
    }
    for (short i = 0; i < N; ++i) {
        if (DFS(used, input_data, output, N, i, cur_out, fout) == -1) {
            fprintf(fout, "impossible to sort");
            free(used); //memory clear
            free(input_data);
            free(output);
            return -1;
        }
    }
    free(input_data);
    free(used);
    return 0;
}

void OutputTopSort(short* output, short* cur_out, FILE* fout) {
    for (short i = *cur_out - 1; i >= 0; --i) {
        fprintf(fout, "%d ", output[i] + 1);
    }
    free(output);
}
char WriteData(char* input_data, short N, int M, FILE* fin, FILE* fout) { //comprassed matrix of vertexes
    for (int i = 0; i < N * N / 8 + 1; ++i) {
        input_data[i] = 0;
    }
    for (int i = 0; i < M; ++i) {
        short from, to;
        if (fscanf(fin, "%hi%hi", &from, &to) == EOF) {
            fprintf(fout, "bad number of lines");
            return -1;
        }
        if (from < 1 || from > N || to < 1 || to > N) {
            fprintf(fout, "bad vertex");
            return -1;
        }
        int number = (from - 1) * N + (to - 1);
        if ((input_data[number / 8] & (1 << (number % 8))) == 0) {
            input_data[number / 8] += 1 << (number % 8);
        }
    }
    return 0;
}

int main() {
    FILE* fin = fopen("in.txt", "r");
    FILE* fout = fopen("out.txt", "w");
    short N;
    int M;
    if (fscanf(fin, "%hi", &N) == EOF) {
        fprintf(fout, "bad number of lines");
        fclose(fin);
        fclose(fout);
        return 0;
    }
    if (fscanf(fin, "%d", &M) == EOF) {
        fprintf(fout, "bad number of lines");
        fclose(fin);
        fclose(fout);
        return 0;
    }
    if (CheckVerticesEdges(N, M, fout) == -1) {
        fclose(fin);
        fclose(fout);
        return 0;
    }
    char* input_data = (char*)calloc((N * N) / 8 + 1, sizeof(char));
    assert(input_data != NULL);
    if (WriteData(input_data, N, M, fin, fout) == -1) {
        free(input_data);
        fclose(fin);
        fclose(fout);
        return 0;
    }
    fclose(fin);
    short* output = (short*)calloc(N, sizeof(short));
    assert(output != NULL);
    short cur_out = 0;
    if (TopSort(input_data, output, N, &cur_out, fout) == -1) {
        fclose(fout);
        return 0;
    }
    OutputTopSort(output, &cur_out, fout);
    fclose(fout);
    return 0;
}
