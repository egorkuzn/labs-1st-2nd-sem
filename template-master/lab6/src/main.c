#include <stdio.h>
#include <malloc.h>

typedef struct node //     
{
	int value;
	unsigned char height;
	struct node* left;
	struct node* right;
}node;
unsigned char height(node* p)
{
	return p ? p->height : 0;
}
short bfactor(node* p)
{
	return height(p->right) - height(p->left);
}
void fixheight(node* p)
{
	unsigned char hl = height(p->left);
	unsigned char hr = height(p->right);
	p->height = (hl > hr ? hl : hr) + 1;
}
node* rotateright(node* p) //    p
{
	node* q = p->left;
	p->left = q->right;
	q->right = p;
	fixheight(p);
	fixheight(q);
	return q;
}
node* rotateleft(node* q) //    q
{
	node* p = q->right;
	q->right = p->left;
	p->left = q;
	fixheight(q);
	fixheight(p);
	return p;
}
node* balance(node* p)//   
{
	fixheight(p);
	short bf = bfactor(p);
	if (bf == 2) {
		if (bfactor(p->right) < 0)
			p->right = rotateright(p->right);
		return rotateleft(p);
	}
	if (bf == -2) {
		if (bfactor(p->left) > 0)
			p->left = rotateleft(p->left);
		return rotateright(p);
	}
	return p;
}

node* push(node* p, node* new_p, int value) //  
{
	if (!p) {
		p = new_p;
		p->value = value;
		p->right = NULL;
		p->left = NULL;
		p->height = 1;
		return p;
	}
	if (value < p->value)
		p->left = push(p->left, new_p, value);
	else
		p->right = push(p->right, new_p, value);
	return balance(p);
}
int main() {
	int N, c;
	if (!scanf("%d", &N))
		return 0;
	if (N == 0) {
		printf("0");
		return 0;
	}
	node* tree = (node*)malloc(sizeof(node) * N);
	if (!tree)
	{
		free(tree);
		return 0;
	}
	if (!scanf("%d", &c)) {
		free(tree);
		return 0;
	}
	tree[0] = (node){ c,1,NULL,NULL };
	node* root = &tree[0];
	for (int i = 1; i < N; ++i) {
		if (!scanf("%d", &c)) {
			free(tree);
			return 0;
		}
		root = push(root, &tree[i], c);
	}
	printf("%u", height(root));
	free(tree);
	return 0;
}
