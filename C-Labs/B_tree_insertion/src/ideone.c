#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

enum { T = 2 };

typedef struct BTree {
	int n;						// фактическое число ключей
	int key[2 * T - 1];			// ключи по возрастанию
	struct BTree *child[2 * T];	// потомки; либо все NULL, либо все не NULL 
} BTree;

int GetChildIdx(const BTree *bT, int k) {
	for (int c = 0; c < bT->n; ++c) // TODO: ускорить
		if (k <= bT->key[c]) return c;
	return bT->n;
}
bool Find(const BTree *bT, int k) {
	if (bT == NULL) return false;
	int childIdx = GetChildIdx(bT, k);
	if (bT->key[childIdx] == k)
		return true;
	return Find(bT->child[childIdx], k);
}

void MoveTail(BTree *src, int i, BTree *dst, int j) {
	int t = dst->n - j;
	memmove(dst->key + j, src->key + i, t * sizeof(int));
	memmove(dst->child + j, src->child + i, (t + 1) * sizeof(BTree*));
}

void AddKey(BTree *bT, int k) {
	int c = GetChildIdx(bT, k);
	bT->n += 1;
	MoveTail(bT, c, bT, c + 1);
	bT->key[c] = k;
}

BTree* Create() {
	return (BTree*)calloc(1, sizeof(BTree)); // заполним нулями
}

void Split(int c, BTree *p) {
	BTree *fc = p->child[c];
	BTree *nc = Create();
	nc->n = T - 1;
	MoveTail(fc, T, nc, 0);
	fc->n = T - 1;
	p->n += 1;
	MoveTail(p, c, p, c + 1);
	p->key[c] = fc->key[T - 1];
	p->child[c + 1] = nc;
}

bool IsLeaf(BTree *bT) {
	return bT->child[0] == NULL; // все NULL или все не NULL
}

bool IsFull(BTree *bT) {
	return bT->n == 2 * T - 1;
}

void InsertNonFull(BTree *bT, int k) {
	if (IsLeaf(bT))
		AddKey(bT, k);
	else {
		int childIdx = GetChildIdx(bT, k);
		if (IsFull(bT->child[childIdx]))
			Split(childIdx, bT);
		InsertNonFull(bT->child[GetChildIdx(bT, k)], k);
	}
}

void Insert(BTree **bT, int k) {
	BTree* nonFull = Create();
	nonFull->child[0] = *bT;
	InsertNonFull(nonFull, k);
	if (nonFull->n > 0)
		*bT = nonFull;
}

void Print(BTree *bT) {
	if (IsLeaf(bT)) {
		printf("[ ");
		for (int c = 0; c < bT->n; ++c)
			printf("%d ", bT->key[c]);
		printf("]");
		return;
	}
	printf("[ ");
	for (int c = 0; c < bT->n; ++c) {
		Print(bT->child[c]);
		printf(", %d, ", bT->key[c]);
	}
	Print(bT->child[bT->n]);
	printf("]");
}

int main(void) {
	BTree *bT = NULL;
	Insert(&bT, 1);
	Print(bT);
	printf("\n");
	Insert(&bT, 2);
	Print(bT);
	printf("\n");
	Insert(&bT, 3);
	Print(bT);
	printf("\n");
	Insert(&bT, 4);
	Print(bT);
	printf("\n");
	Insert(&bT, 5);
	Print(bT);
	printf("\n");
	Insert(&bT, 6);
	Print(bT);
	printf("\n");
	Insert(&bT, 7);
	Print(bT);
	printf("\n");
	Insert(&bT, 8);
	Print(bT);
	printf("\n");
	Insert(&bT, 9);
	Print(bT);
	printf("\n");
	return 0;
}
