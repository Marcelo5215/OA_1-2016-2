#include "bt.h"

pBTree criaArvoreB(int ordem){
	int  i;
	pBTree arvB = (pBTree)malloc(sizeof(BTree));

	arvB->chave = (char**)malloc(sizeof(char*) * ordem-1);
	for (i = 0; i < ordem; ++i){
		arvB->chave[i] = (char*)malloc(sizeof(char) * TAM_CHAVE);
	}
	arvB->filhos = (pBTree*)malloc(sizeof(pBTree) * ordem);
	arvB->ordem = ordem;
	arvB->n_chaves = 0;

	return arvB;
}

arvB_ret limpaArvoreB(pBTree arvB){
	if(arvB == NULL){
		return ARVB_ERR;
	}
	int i;
	for (i = 0; i < arvB->ordem -1; ++i){
		free(arvB->chave[i]);
	}
	free(arvB->chave);

	for (i = 0; i < arvB->ordem ; ++i){
		if(limpaArvoreB(arvB->filhos[i]) == ARVB_ERR){
			break;
		}
	}
	free(arvB->filhos);

	free(arvB);

	return ARVB_OK;
}

pBTree criaFilho(char* chave, int ordem){
	if(strlen(chave) > 8){
		return NULL;
	}
	int i;
	pBTree filho = (pBTree)malloc(sizeof(BTree));

	filho->chave = (char**)malloc(sizeof(char*) * ordem-1);
	for (i = 0; i < ordem; ++i){
		filho->chave[i] = (char*)malloc(sizeof(char) * TAM_CHAVE);
	}

	filho->filhos = (pBTree*)malloc(sizeof(pBTree) * ordem);
	filho->ordem = ordem;
	filho->n_chaves = 0;

	return filho;
}

pBTree buscaAB(pBTree raiz, int *ind ,char* chave){
	if (raiz == NULL){
		return NULL;
	}
	int i;
	for (i = 0; i < raiz->n_chaves; ++i){
		if(chave == raiz->chave[i]){
			*ind = i;
			return raiz;
		}
	}

	for (i = 0; i < raiz->ordem ; ++i){
		return(buscaAB(raiz->filhos[i], ind, chave));	
	}

	return NULL;
}

// arvB_ret insereAB(pBTree raiz, char* chave){

// }