#include <stdio.h>
#include <malloc.h>
void swap(int* a, int* b) {
	int c;
	c = *a;
	*a = *b;
	*b = c;
}

int order(int l, int r, int arr[]) {
	int x = arr[(l + r) / 2];
	int L = l;
	int R = r;
	while (L <= R) {
		while (arr[L] < x)
			++L;
		while (arr[R] > x)
			--R;
		if (L >= R) {
			break;
		}
		swap(&arr[L++], &arr[R--]);
	}
	return R;
}

void qsort(int mas[], int l, int r) {
	if (l - r >= 0)
		return;
	int c = order(l, r, mas);
	qsort(mas, l, c);
	qsort(mas, c + 1, r);
}

int main() {
	long n;
	if (!scanf("%ld", &n))
		return 0;
	if (n <= 0)
		return 0;
	int* massiv = (int*)malloc(n * sizeof(int));
	for (long i = 0; i < n; ++i) {
		if (!scanf("%d", &massiv[i])) {
			free(massiv);
			return 0;
		}

	}
	qsort(massiv, 0, n - 1);
	for (long i = 0; i < n; ++i)
		printf("%d ", massiv[i]);
	free(massiv);
	return 0;
}
