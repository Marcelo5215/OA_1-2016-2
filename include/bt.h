#ifndef BT_H
#define BT_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "index.h"

	#define TAM_CHAVE 10

	typedef struct BTree* pBTree;

	typedef struct BTree{
		char **chave;
		pBTree *filhos;
		pBTree pai;
		int n_chaves, ordem; 
	}BTree;

	typedef enum arvB_ret{
		ARVB_OK,
		ARVB_ERR,
	}arvB_ret;

	pBTree criaArvoreB(int ordem);
	pBTree criaPagina(int ordem);
	arvB_ret limpaArvoreB(pBTree arvB);
	pBTree criaFilho(char* chave, int ordem);
	pBTree criaABIndicePrimario(tabelaInd_Prim *ind, int ordem);
	pBTree buscaAB(pBTree raiz, int *seeks ,char* chave);
	void insereAB(pBTree *raiz, char* chave);
	pBTree insereAB_helper(pBTree raiz, char *chave);
	pBTree buscaDir(pBTree raiz, char *chave);
	void inOrder(pBTree raiz);
	void inOrder2(pBTree raiz);
	int insere(pBTree local, char* chave);

	void escreveABarq(FILE *fp, pBTree arvB);
	pBTree leituraABarq(FILE *fp, pBTree arvB);

#endif
