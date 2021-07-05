#include <stdio.h>
#include <stdlib.h>
short translate_cin(char symbol, short board) {
	if ((symbol >= '0') && (symbol <= '9') && (symbol - '0' < board))
		return symbol - '0';

	else {
		if ((symbol >= 'A') && (symbol <= 'F') && (symbol - 'A' + 10 < board))
			return symbol - 'A' + 10;

		else {
			if ((symbol >= 'a') && (symbol <= 'f') && (symbol - 'a' + 10 < board))
				return symbol - 'a' + 10;

			else {
				printf("bad input");
				exit(EXIT_SUCCESS);
			}
		}
	}
}
char fr_int_to_char(short count) {
	if ((count >= 0) && (count <= 9)) {
		return(count + '0');
	}
	else {
		return(count - 10 + 'A');
	}
}
void check_b1_b2(long long bb1, long long bb2) {
	if ((bb1 > 16) || (bb2 > 16) || (bb1 < 2) || (bb2 < 2)) {
		printf("bad input");
		exit(EXIT_SUCCESS);
	}
}
int main() {
	long long b1, b2;
	if (!scanf("%lli%lli", &b1, &b2)) { return 0; }
	char cin[14];
	if (!scanf("%13s", cin)) { return 0; }
	check_b1_b2(b1, b2);
	if (cin[0] == '.') {
		printf("bad input");
		return(0);
	}
	short h1 = b1 % 100, h2 = b2 % 100;
	for (short k = 1; k < 13; ++k) {
		if (cin[k] == '.') {
			b1 = translate_cin(cin[k + 1], h1);
		}
	}
	long long deccount = 0;
	short num, i = 0;
	while (((cin[i] != '.') && (cin[i] != '\0')) && (i < 13)) {
		num = translate_cin(cin[i], h1);
		deccount *= h1;
		deccount += num;
		++i;
	}
	if (h2 == 10) {
		printf("%lli", deccount);
	}
	else {
		char decchar[50];
		short j = -1;
		while (((deccount == 0) && (j == -1)) || (deccount > 0)) {
			++j;
			char sight;
			sight = fr_int_to_char(deccount % h2);
			decchar[j] = sight;
			deccount /= h2;
		}
		for (short k = j; k >= 0; --k) {
			printf("%c", decchar[k]);
		}
	}
	if (cin[i] == '.') {
		double double_part = 0, stage = 1;
		++i;
		while (cin[i] != '\0') {
			stage *= h1;
			num = translate_cin(cin[i], h1);
			double_part += num / stage;
			++i;
		}
		if (double_part > 0) {
			printf(".");
		}
		else {
			return(0);
		}
		char d_h2[13];
		for (short k = 0; k < 13; d_h2[k] = '\0', ++k);
		short inc = 0;
		while ((double_part != 0) && (inc < 12)) {
			double_part *= h2;
			d_h2[inc] = fr_int_to_char((int)double_part);
			double_part = double_part - (int)double_part;
			++inc;
		}
		printf("%s", d_h2);
	}
	return(0);
}

