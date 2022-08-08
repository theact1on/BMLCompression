#include "treap.h"

TreapNode* treapInitNode(ull key, ull priority, ull data){
	TreapNode* newNode = (TreapNode*)malloc(sizeof(TreapNode));
	newNode->left = NULL;
	newNode->right = NULL;
	newNode->data = data;
	newNode->key.key = key;
	newNode->key.priority = priority;
	newNode->childCount = 0;
	
	return newNode;	
}

ull treapGetCodeword(TreapNode *treap, ull key){
	if(treap == NULL){
		return 0;
	}
	
	if(treap->key.key > key){
		return treapGetNodeCount(treap->right) + 1 + treapGetCodeword(treap->left, key);
	}else if(treap->key.key < key){
		return treapGetCodeword(treap->right, key);
	}else{
		return treapGetNodeCount(treap->right);
	}
}

TreapNode* treapFindByCodeword(TreapNode *treap, ull codeWord){
	if(treap == NULL)
		return NULL;
	
	ull rNodes = treapGetNodeCount(treap->right);
	if(rNodes > codeWord){
		return treapFindByCodeword(treap->right, codeWord);
	}else if(rNodes < codeWord){
		return treapFindByCodeword(treap->left, codeWord - rNodes - 1);
	}else{
		return treap;
	}
}

ull treapGetNodeCount(TreapNode* node){
	if(node == NULL)
		return 0;
	
	return node->childCount + 1;
}

void treapFreeNode(TreapNode* node){
	if(node == NULL) return;
	treapFreeNode(node->left);
	treapFreeNode(node->right);
	free(node);
}

TreapNode* treapInsert(TreapNode* treap, ull key, ull priority, ull data){
	TreapNode* treapL;
	TreapNode* treapR;
	treapSplit(treap, key, &treapL, &treapR);
	
	TreapNode* newNode = treapInitNode(key, priority, data);
	
	return treapMerge(treapMerge(treapL, newNode), treapR);
}

TreapNode* treapRemove(TreapNode* treap, ull key){
	TreapNode *treapL1,*treapR1,*treapL2,*treapR2;
	
	treapSplit(treap, key, &treapL1, &treapR1);
	treapSplit(treapR1, key+1, &treapL2, &treapR2);
	
	return treapMerge(treapL1, treapR2);
}

TreapNode* treapFind(TreapNode* treap, ull key){
	if(treap == NULL) return NULL;
	if (treap->key.key == key) return treap;
	if(treap->key.key > key){
		return treapFind(treap->left, key);
	}else{
		return treapFind(treap->right, key);
	}
}

TreapNode* treapMerge(TreapNode* treapL, TreapNode* treapR){
	if(treapL == NULL)return treapR;
	if(treapR == NULL)return treapL;
	
	if(treapL->key.priority > treapR->key.priority){
		treapL->right = treapMerge(treapL->right, treapR);
		treapCalculateChilds(treapL);
		return treapL;	
	}else{
		treapR->left = treapMerge(treapL, treapR->left);
		treapCalculateChilds(treapR);
		return treapR;
	}
	
}

void treapCalculateChilds(TreapNode* node){
	if(node == NULL) return;
	
	node->childCount = 0;
	if(node->right != NULL)
		node->childCount += node->right->childCount + 1;
	if(node->left != NULL)
		node->childCount += node->left->childCount + 1;
}

void treapSplit(TreapNode* treap, ull key, TreapNode** treapL, TreapNode** treapR){
	if(treap == NULL){
		*treapL = *treapR = NULL;
		return;
	}
	
	if(treap->key.key < key){
		(*treapL) = treap;
		treapSplit(treap->right, key, &((*treapL)->right), &(*treapR));
		treapCalculateChilds((*treapL)->right);
	}else{
		(*treapR) = treap;
		treapSplit(treap->left, key, &(*treapL), &((*treapR)->left));
		treapCalculateChilds((*treapR)->left);
	}
}

