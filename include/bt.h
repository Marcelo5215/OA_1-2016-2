#ifndef BT_H
#define BT_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	#define TAM_CHAVE 9

	typedef struct BTree* pBTree;

	typedef struct BTree{
		char **chave;
		pBTree *filhos;
		int n_chaves, ordem; 
	}BTree;

	typedef enum arvB_ret{
		ARVB_OK,
		ARVB_ERR,
	}arvB_ret;

	pBTree criaArvoreB(int ordem);
	arvB_ret limpaArvoreB(pBTree arvB);
	pBTree criaFilho(char* chave, int ordem);
	pBTree buscaAB(pBTree raiz, int *ind ,char* chave);

#endif