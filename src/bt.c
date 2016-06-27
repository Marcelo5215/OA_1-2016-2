#include "bt.h"

int insere(pBTree local, char* chave);

pBTree criaArvoreB(int ordem){
	int  i;
	pBTree arvB = (pBTree)malloc(sizeof(BTree));

	arvB->chave = (char**)malloc(sizeof(char*) * ordem-1);
	for (i = 0; i < ordem; ++i){
		arvB->chave[i] = (char*)malloc(sizeof(char) * TAM_CHAVE);
	}
	arvB->filhos = (pBTree*)malloc(sizeof(pBTree) * ordem);
	for (i = 0; i < ordem; ++i){
		arvB->filhos[i] = NULL;
	}
	arvB->ordem = ordem;
	arvB->n_chaves = 0;

	return arvB;
}

pBTree criaPagina(int ordem){
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
	int i = 0, index = 0;
	while( strcmp(raiz->chave[i], chave) < 0){
		i++;
		index++;
	}
	if(strcmp(raiz->chave[i], chave) == 0){
		return raiz;
	}
	return(buscaAB(raiz->filhos[i], ind, chave));	
}

pBTree criaABIndicePrimario(tabelaInd_Prim *ind, int ordem){
	pBTree arvB = criaArvoreB(ordem);
	char *chave;
	int i;
	for (i = 0; i < ultimoElementoIndicePrimario(ind) ; ++i){
		chave = getKey(ind, i);
		printf("%s\n", chave);
		arvB = insereAB(arvB, chave);
	}

	return arvB;
}

pBTree insereAB(pBTree raiz, char* chave){
	int i, index, novo_n, j;
	char promotedKey[10];
	pBTree filho, nova_raiz, raizFilho = NULL;
	filho = buscaAB(raiz, &index, chave);
	//verifica se a chave já existe
	if(filho != NULL){
		printf("CHAVE JÁ EXISTENTE\n");
		return raiz;
	}
	else{
		//se existir lugar na pagina raiz
		if (raiz->n_chaves < raiz->ordem - 1){
			insere(raiz, chave);
			return raiz;
		}
		else{
			//busca o nó folha onde o filho deveria ser inserido
			filho = buscaDir(raiz, chave, raizFilho);
			// if(filho == raiz){
			// 	printf("BANANA\n");
			// }
			//se existir lugar no nó do filho
			if (filho->n_chaves < filho->ordem - 1){
				insere(filho, chave);
				return raiz;
			}
			else{
				//split
				pBTree esq = filho;
				pBTree dir = criaArvoreB(raiz->ordem);

				//copia metade das chaves do filho para a esquerda(esq), eoutra metade para a direita(dir)
				//e a chave no meio será a chave promovida 
				novo_n = 0; 
				strcpy(promotedKey, filho->chave[filho->n_chaves/2]);

				for (i = 0; i < filho->n_chaves/2; ++i){
				  	strcpy(esq->chave[i], filho->chave[i]);
				  	novo_n++;
				}
				j = 0; 
				for (i = filho->n_chaves/2 + 1; i < filho->n_chaves; ++i, j++){
				  	strcpy(dir->chave[j], filho->chave[i]);
				  	dir->n_chaves++;
				} 
				filho->n_chaves = novo_n;
				if (raizFilho != NULL){
					index = insere(raizFilho, filho->chave[filho->n_chaves/2]);
					if(index > 0){
						raizFilho->filhos[index - 1] = esq;
						raizFilho->filhos[index] = dir;
					}
					else if (index == 0){
						raizFilho->filhos[0] = esq;
						raizFilho->filhos[1] = dir;
					}
					else{
						raizFilho->filhos[raizFilho->ordem-1] = dir;
						raizFilho->filhos[raizFilho->ordem-2] = esq;
					}
					return raizFilho;
				} 
				//monta a nova raiz
				nova_raiz = criaArvoreB(raiz->ordem);
				index = insere(nova_raiz, promotedKey);
				insere(dir, chave);
				if(index > 0){
					nova_raiz->filhos[index - 1] = esq;
					nova_raiz->filhos[index] = dir;
				}
				else if (index == 0){
					nova_raiz->filhos[0] = esq;
					nova_raiz->filhos[1] = dir;
				}
				else{
					nova_raiz->filhos[nova_raiz->ordem-1] = dir;
					nova_raiz->filhos[nova_raiz->ordem-2] = esq;
				}
				return nova_raiz;
			}
		}
	}
}

int insere(pBTree local, char* chave){
	int index = 0;
	int i = 0, j;
	//caso não exista nenhuma chave
	if(local->n_chaves == 0){
		strcpy(local->chave[0], chave);
		local->n_chaves++;
	}
	else{
		//se existir, procura seu local para inserir ordenado
		while(strcmp(local->chave[i], chave) < 0 && i < local->n_chaves){
			i++;
		}
		//se o seu local estiver no meio das chaves já colocadas ...
		if(i < local->n_chaves){
			j = local->n_chaves;
			//desloca todos as chaves para abrir espaço para a nova chave
			while(j >= i){
				strcpy(local->chave[j+1], local->chave[j]);
				j--;
			}
			strcpy(local->chave[i], chave);
			local->n_chaves++;
		}
		else{
			strcpy(local->chave[local->n_chaves], chave);
			local->n_chaves++;
		}
	}

	return i;
}

pBTree buscaDir(pBTree raiz, char *chave, pBTree raizFilho){
	int i = 0;
	int index = 0;
	while(strcmp(raiz->chave[i], chave) < 0 && i < raiz->n_chaves){
		i++;
		index++;
	}
	if(i == raiz->n_chaves-1){
		index++;
	}
	else if(raiz->filhos[index] == NULL){
		return raiz;
	}
	raizFilho = raiz;
	return (buscaDir(raiz->filhos[index], chave, raizFilho));
}

void inOrder(pBTree raiz){
	if(raiz == NULL){
		return;
	}

	int i = 0 ;
	pBTree aux = raiz;
	while(i < raiz->n_chaves){
		inOrder(raiz->filhos[i]);
		printf("%s\n", raiz->chave[i]);
		i++;
	}
	inOrder(raiz->filhos[i]);
}