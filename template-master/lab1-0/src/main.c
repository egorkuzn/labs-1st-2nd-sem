#include<stdio.h>
#include<stdlib.h>

int ReadTemplate(FILE* input, unsigned char* tempalte, int* length) {
	for (short i = 0; i < 17; ++i) {
		if (!(tempalte[i] = fgetc(input)))
			return 1;
		if (tempalte[i] == '\n') {
			tempalte[i] = '\0';
			break;
		}
		++* length;

	}
	return 0;
}

void FillSample(int length, unsigned char* template, int* part) {
	int count = length - 1;
	for (int i = (length - 2); i >= 0; i--) {
		int  letter = template[i];
		if (part[letter] == length) {
			part[letter] -= count;
		}
		count--;
	}
}

int ScanStr(FILE* input, unsigned char* str, int size, int length) {
	int j = 0;
	for (int i = size; i < length; i++) {
		str[j] = str[i];
		j++;
	}
	size = length - j;
	if (!fread(str + j, 1, size, input)) {
		return 0;
	}
	if (feof(input))
		return 0;
	return 1;
}

int BoyerMoore(FILE* input, int length, unsigned char* template, int* part, int* count) {
	unsigned char str[17];
	if (ScanStr(input, str, length, length) == 0)
		return 0;
	while (1) {
		for (int i = length - 1; i >= 0; i--) {
			printf("%d ", i + *count + 1);
			if (template[i] != str[i] || i == 0) {
				break;
			}
		}
		int shift = part[str[length - 1]];
		*count += shift;
		if (ScanStr(input, str, shift, length) == 0)
			return 0;
	}
}

int main() {
	unsigned char tempalte[17];
	FILE* input;
	input = fopen("in.txt", "rt");
	if (input == NULL) 		
		return 0;
	int length = 0;
	if (ReadTemplate(input, tempalte, &length)) {
		fclose(input);
		return 0;
	}
	int part[256];
	for (short i = 0; i < 256; i++) {
		part[i] = length;
	}
	FillSample(length, tempalte, part);
	int count = 0;
	BoyerMoore(input, length, tempalte, part, &count);
	fclose(input);
	return 0;
}
