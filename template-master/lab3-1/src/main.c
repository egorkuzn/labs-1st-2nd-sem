#include <stdio.h>
#include <malloc.h>

void swap(int* a, int* b) {
    int c;
    c = *a;
    *a = *b;
    *b = c;
}

void sift(int mass[], int length, int i) {
    while ((2 * i <= length && mass[i] < mass[2 * i]) || (2 * i + 1 <= length && mass[i] < mass[2 * i + 1])) {
        if (2 * i + 1 > length) {
            swap(&mass[i], &mass[2 * i]);
            i *= 2;
        }
        else {
            if (mass[2 * i] > mass[2 * i + 1]) {
                swap(&mass[i], &mass[2 * i]);
                i *= 2;
            }
            else {
                swap(&mass[i], &mass[2 * i + 1]);
                i *= 2;
                ++i;
            }
        }
    }
}

int main() {
    int n;
    if (!scanf("%d", &n))
        return 0;
    int* array = (int*)malloc((n + 1) * sizeof(int));
    array[0] = 0;
    for (long k = 1; k < n + 1; ++k)
        if (!scanf("%d", &array[k])) {
            free(array);
            return 0;
        }
    for (int i = n / 2; i > 0; --i)
        sift(array, n, i);
    for (int k = n; k >= 1; --k) {
        sift(array, k, 1);
        swap(&array[1], &array[k]);
    }
    for (int k = 1; k <= n; ++k)
        printf("%d ", array[k]);
    free(array);
    return 0;
}
