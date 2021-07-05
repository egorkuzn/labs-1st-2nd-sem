#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
int SyntaxERROR(char* input) {
	int i = 0, bracket = 0;
	while (input[i] != '\0')
	{
		if (input[i] <= '9' && input[i] >= '0') {
			++i;
			continue;
		}
		if (input[i] == '(' && input[i + 1] != ')') {
			++i;
			++bracket;
			continue;
		}
		if (input[i] == ')')
			if (bracket > 0) {
				--bracket;
				++i;
				continue;
			}
		if (i > 0 && (input[i] == '+' || input[i] == '-' || input[i] == '*' || input[i] == '/')) {
			if (input[i] == '/' && input[i + 1] == '0')
			{
				printf("division by zero");
				exit(EXIT_SUCCESS);
			}
			if ((input[i - 1] <= '9' && input[i - 1] >= '0') || input[i - 1] == ')')
				if ((input[i + 1] <= '9' && input[i + 1] >= '0') || input[i + 1] == '(')
				{
					++i;
					continue;
				}

		}
		return 1;
	}
	if (bracket != 0)
		return 1;
	return 0;
}
struct elem {
	int value;
	struct elem* next;
};
void StackForInt(int x, struct elem** head) {
	struct elem* nw = malloc(sizeof(struct elem));
	nw->next = *head;  // *head - адрес, **head == NULL
	nw->value = x;
	*head = nw;
}
void HeadDel(struct elem** head) {
	if (*head) {
		struct elem* cur = *head;
		*head = (*head)->next;
		free(cur);
	}
}
void FreeList(struct elem* head) {
	if (head != NULL) {
		struct elem* t;
		t = head->next;
		free(head);
		FreeList(t);
	}
}
int PolTrans(char* input, char* output) {
	int i = 0, j = 0;
	struct elem* head = NULL;
	while (input[i] != '\0') {
		if (input[i] >= '0' && input[i] <= '9') {
			while (input[i] >= '0' && input[i] <= '9') {
				output[j] = input[i];
				++i;
				++j;
			}
			output[j] = ' ';
			++j;
		}
		switch (input[i]) {
		case '+':
		case '-':
			if ((head != NULL) && (head->value == '*' || head->value == '/' || head->value == '-' || head->value == '+')) {
				while (head != NULL && head->value != '(') {
					output[j] = head->value;
					output[j + 1] = ' ';
					j += 2;
					HeadDel(&head);
				}
			}
			StackForInt(input[i], &head);
			++i;
			break;
		case '*':
		case '/':
			if ((head != NULL) && (head->value == '*' || head->value == '/')) {
				output[j] = head->value;
				output[j + 1] = ' ';
				j += 2;
				HeadDel(&head);
			}
			StackForInt(input[i], &head);
			++i;
			break;
		case '(':
			StackForInt(input[i], &head);
			++i;
			break;
		case ')':
			if ((head != NULL) && (head->value == '*' || head->value == '/' || head->value == '-' || head->value == '+')) {
				while (head->value != '(') {
					output[j] = head->value;
					output[j + 1] = ' ';
					j += 2;
					HeadDel(&head);
				}
			}
			++i;
			HeadDel(&head);
			break;
		}
	}
	while (head != NULL) {
		output[j] = head->value;
		output[j + 1] = ' ';
		HeadDel(&head);
		j += 2;
	}
	j -= 2;
	FreeList(head);
	return j + 1;
}
int Calculator(char* input) {
	char output[2002];
	int i = 0, n = PolTrans(input, output);
	struct elem* head = NULL;
	while (i < n)
	{
		if ('0' <= output[i] && output[i] <= '9') {
			int count = 0;
			while ('0' <= output[i] && output[i] <= '9') {
				count *= 10;
				count += output[i] - '0';
				++i;
			}
			++i;
			StackForInt(count, &head);
			continue;
		}
		int one = head->value, two = (head->next)->value;
		HeadDel(&head);
		switch (output[i]) {
		case '+':
			two += one;
			break;
		case '-':
			two -= one;
			break;
		case '/':
			if (one == 0) {
				printf("division by zero");
				FreeList(head);
				exit(EXIT_SUCCESS);
			}
			else
				two /= one;
			break;
		case '*':
			two *= one;
			break;
		}
		i += 2;
		head->value = two;
	}
	int res = head->value;
	FreeList(head);
	return res;
}

int main() {
	char input[1002];
	if (!scanf("%1001[^\n]s", input)) {
		printf("syntax error");
		return 0;
	}
	if (SyntaxERROR(input)) {
		printf("syntax error");
		return 0;
	}
	int result = Calculator(input);
	printf("%d", result);
	return 0;
}
