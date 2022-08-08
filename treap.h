#ifndef TREAP_H
#define TREAP_H

#include <stdio.h>
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned long long ull;

typedef struct TreapNode TreapNode;
typedef struct TreapNodeKey TreapNodeKey;

struct TreapNodeKey{
	ull key;
	ull priority;
};

struct TreapNode{
	TreapNode* left;
	TreapNode* right;
	ull childCount;
	ull data;
	TreapNodeKey key;
};

TreapNode* treapInitNode(ull key, ull priority, ull data);
TreapNode* treapInsert(TreapNode* treap, ull key, ull priority, ull data);
TreapNode* treapRemove(TreapNode* treap, ull key);
TreapNode* treapMerge(TreapNode* treapL, TreapNode* treapR);
TreapNode* treapFind(TreapNode* treap, ull key);
void treapSplit(TreapNode* treap, ull key, TreapNode** treapL, TreapNode** treapR);
void treapCalculateChilds(TreapNode* node);
void treapFreeNode(TreapNode* node);
ull treapGetNodeCount(TreapNode* node);
ull treapGetCodeword(TreapNode *treap, ull key);
TreapNode* treapFindByCodeword(TreapNode *treap, ull codeWord);

#endif