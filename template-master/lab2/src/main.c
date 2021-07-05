#include <stdio.h>
#include <stdlib.h>
void swap(int* a, int* b)
{
	int c = *a;
	*a = *b;
	*b = c;
}
short arr_building(int array[]) {
	char str[50];
	short l = 0;
	for (int k = 0; k < 12; ++k)
		str[k] = 10;
	if(!scanf("%12[^\n]s", str))
		return 0;
	if (str[11] != 10)
		return 0;
	while (str[l] != '\0')
	{
		++l;
		if (str[l - 1] > '9' || str[l - 1] < '0')
			return 0;
		if (l > 10) {
			return 0;
		}
		else
			array[l - 1] = str[l - 1] - '0';
		for (int k = 0; k < l - 1; k++)
			if (array[l - 1] == array[k])
				return 0;
	}
	return l;
}
void next_permut(int array[], int length) {
	short pick = 0;
	for (int k = 0; k < length - 1; ++k)
		if (array[k] < array[k + 1]) {
			pick = 1;
			break;
		}
	if (pick == 0)
		exit(EXIT_SUCCESS);
	int i = length - 1;
	while (i > 0 && array[i - 1] >= array[i])
		i--;
	int j = length - 1;
	while (array[j] <= array[i - 1])
		j--;
	swap(&array[i - 1], &array[j]);
	j = length - 1;
	while (i < j) {
		swap(&array[i], &array[j]);
		i++;
		j--;
	}

}
int main() {
	int N, length = 0, arr[10];
	length = arr_building(arr);
	if (!scanf("%d", &N))
		return 0;
	if (length == 0) {
		printf("bad input");
		return 0;
	}
	for (int k = 0; k < N; ++k) {
		next_permut(arr, length);
		for (int i = 0; i < length; ++i)
			printf("%d", arr[i]);
		printf("\n");
	}
	return 0;
}
