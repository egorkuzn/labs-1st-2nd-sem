#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int HashFunc(unsigned char* c, int* N, int* size) {
	int inc = 0;
	for (int i = 0; i < *N; ++i) {
		inc += c[i] % 3 * *size;
		*size *= 3;
	}
	*size /= 3;
	return inc;
}

int ReadTemplate(FILE* input, unsigned char* template, int* length) {
	for (int i = 0; i < 17; ++i) {
		if (!(template[i] = fgetc(input)))
			return 0;
		if (template[i] == '\n') {
			template[i] = '\0';
			break;
		}
		++* length;

	}
	return 1;
}
int ReadPart(FILE* input, unsigned char* part, int* length) {
	if (!fread(part, 1, *length, input)) {
		return 0;
	}
	if (feof(input))
		return 0;
	return 1;
}
int NextHash(FILE* input, int* text_hash, unsigned char* part, int* length, int* size) {
	*text_hash = *text_hash / 3;
	for (int i = 1; i < *length; ++i)
		part[i - 1] = part[i];
	if (!(part[*length - 1] = fgetc(input)))
		return 0;
	*text_hash += part[*length - 1] % 3 * *size;
	if (feof(input))
		return 0;
	return 1;
}
void IsIt(unsigned char* template, unsigned char* part, int* length, int* count) {
	for (int i = 0; i < *length; ++i)
		if (template[i] == part[i])
			printf("%d ", *count + i + 1);
		else {
			printf("%d ", *count + i + 1);
			return;
		}
}
void RobinCarpSearch(FILE* input, unsigned char* template, int* length) {
	int text_hash, template_hash, count = 0, size = 1, s = 1;
	unsigned char part[17];
	template_hash = HashFunc(template, length, &size);
	printf("\n%d ", template_hash);
	if (!ReadPart(input, part, length))
		return;
	text_hash = HashFunc(part, length, &s);
	while (1) {
		if (text_hash == template_hash)
			IsIt(template, part, length, &count);
		if (!NextHash(input, &text_hash, part, length, &size)) {
			return;
		}
		++count;
	}
}

int main() {
	FILE* input;
	input = fopen("in.txt", "rt");
	if (input == NULL)
		return 0;
	unsigned char template[17];
	int length = 0;
	if (!ReadTemplate(input, template, &length)) {
		fclose(input);
		return 0;
	}
	RobinCarpSearch(input, template, &length);
	fclose(input);
	return 0;
}
