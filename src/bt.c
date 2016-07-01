#include "bt.h"

int insere(pBTree local, char* chave);


//encontra o local no qual se deve inserir o elemento
int binary_search(char *key, char **chaves, int ordem) {
	int ini = 0, fim = ordem;
	
	while (ini < fim) {
		int meio = (ini + fim / 2);
		
		if (strcmp(chaves[meio], key) == 0) {
			return meio;
		} else if (strcmp(chaves[meio], key) > 0) {
			fim = meio;
		} else {
			ini = meio + 1;
		}
	}
	
	return ini;
}


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
	if (arvB->filhos != NULL){
		free(arvB->filhos);
	}

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


//modificar para tb adicionar ponteiros
pBTree create (char **temp, int ini, int fim, int ordem) {
	pBTree filho = criaArvoreB(ordem);
	int i, j;
	
	for (i = ini, j = 0; i <= fim; i++, j++) {
		strcpy(filho->chave[j], temp[i]);
	}
	filho->n_chaves = j;
	
	return filho;
}

char *insereAB_v2(pBTree raiz, char* chave){
	
	static char **temp;
	static pBTree filho2;
	int ind = binary_search(chave, raiz->chave, raiz->ordem);
	int i;

	if (ind < raiz->ordem && !strcmp(raiz->chave[ind], chave)) {
		printf("CHAVE JÁ EXISTENTE\n");
		return NULL;
	} else {
		if (raiz->filhos[ind] != NULL) {
			char *subiu = insereAB_v2(raiz->filhos[ind], chave);
			
			if (subiu == NULL)
				return NULL;
			
			if (raiz->n_chaves < raiz->ordem - 1) {
				//somente adiciona
			} else {
				//cria outra coisa temporaria
			}
			
		} else if (raiz->n_chaves < raiz->ordem - 1) {
		
			for (i = raiz->n_chaves; ind < i; i--) {
				strcpy(raiz->chave[i], raiz->chave[i - 1]);
			}
			return NULL;
		} else {
			temp = (char**) malloc(sizeof(char*) * raiz->ordem);
			
			for (i = raiz->ordem; ind < i; i--) {
				strcpy(temp[i], raiz->chave[i - 1]);
			}
			for (i = ind; i >= 0; i--) {
				strcpy(temp[i], raiz->chave[i]);
			}
			
			char *sobe = temp[raiz->ordem / 2];
			
			raiz->n_chaves = raiz->ordem / 2;
			
			filho2 = create(temp, raiz->ordem / 2 + 1, raiz->ordem, raiz->ordem);
			
			return sobe;
		}
	}
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

void escreveABarq(FILE *fp, pBTree arvB){
	if (fp == NULL || arvB == NULL){
		return;
	}
	int i, j;
	// printf("DENTRO:\n\n");
	// inOrder(arvB);
	fwrite(arvB, sizeof(BTree), 1, fp);

	for (i = 0; i < arvB->n_chaves; ++i){
		for (j = 0; j < TAM_CHAVE; ++j){
			fwrite(arvB->chave[i], sizeof(char), TAM_CHAVE, fp);
		}
	}

	for(i = 0 ; i <= arvB->ordem ; i++){
		escreveABarq(fp, arvB->filhos[i]);
	}

	return;
}

void leituraABarq(FILE *fp, pBTree arvB){
	if (fp == NULL || feof(fp) != 0){
		return;
	}
	else if(arvB == NULL){
		arvB = (pBTree)malloc(sizeof(BTree));
	}
	char chave[TAM_CHAVE];
	int i, j;

	fread(arvB, sizeof(BTree), 1, fp);

	for (i = 0; i < arvB->n_chaves; ++i){
		for (j = 0; j < TAM_CHAVE; ++j){
			fread(chave, sizeof(char), TAM_CHAVE, fp);
		}
		strcpy(arvB->chave[i], chave);
		printf("chave escrita:%s\n", arvB->chave[i]);
	}

	for(i = 0 ; i <= arvB->ordem ; i++){
		leituraABarq(fp, arvB->filhos[i]);
	}
	
	return;
}