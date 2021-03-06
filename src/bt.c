#include "bt.h"

int insere(pBTree local, char* chave);
void limpaPagina(pBTree local);


//encontra o local no qual se deve inserir o elemento
int binary_search(char *key, char **chaves, int ordem) {
	int ini = 0, fim = ordem;
	
	while (ini < fim) {
		int meio = (ini + fim) / 2;
		
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

//cria uma arvore B vazia
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

	arvB->pai = arvB;
	
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
	for (i = 0; i < ordem; ++i){
		arvB->filhos[i] = NULL;
	}
	arvB->ordem = ordem;
	arvB->n_chaves = 0;

	arvB->pai = NULL;
	
	return arvB;
}

arvB_ret atualizapai(pBTree arvB, pBTree pai){
	if(arvB == NULL){
		return ARVB_ERR;
	}
	int i;
	
	//printf("atualiza %p\n", arvB);
	arvB->pai = pai;

	for (i = 0; i < arvB->ordem ; ++i){
		if(atualizapai(arvB->filhos[i], pai) == ARVB_ERR){
			break;
		}
	}

	return ARVB_OK;
}

arvB_ret limpaArvoreB(pBTree arvB){
	
	static int nivel = 0;
	
	if(arvB == NULL){
		nivel--;
		return ARVB_ERR;
	}
	int i;
	
	//printf("%s %d\n", arvB->chave[0], nivel);
	
	for (i = 0; i < arvB->ordem -1; ++i){
		free(arvB->chave[i]);
	}
	free(arvB->chave);

	for (i = 0; i <= arvB->n_chaves ; ++i){
		nivel++;
		if(limpaArvoreB(arvB->filhos[i]) == ARVB_ERR){
			break;
		}
	}
	if (arvB->filhos != NULL){
		free(arvB->filhos);
	}

	free(arvB);

	nivel--;
	return ARVB_OK;
}

pBTree buscaAB(pBTree raiz, int *seeks ,char* chave){
	if (raiz == NULL){
		return NULL;
	}
	int i = 0, index = 0;
	while( strcmp(raiz->chave[i], chave) < 0){
		i++;
		index++;
	}

	*seeks = *seeks + 1;
	if(strcmp(raiz->chave[i], chave) == 0){
		return raiz;
	}
	return(buscaAB(raiz->filhos[index], seeks, chave));	
}

//cria a arvoreB baseada em um índice Primario
pBTree criaABIndicePrimario(tabelaInd_Prim *ind, int ordem){
	pBTree arvB = criaArvoreB(ordem);
	int i;
	//int a;
	for (i = 0; i < ultimoElementoIndicePrimario(ind) ; ++i){
		arvB = insereAB_helper(arvB, getKey(ind, i));
	}
	return arvB;
}

//Cria novos filhos
pBTree create (char **temp, pBTree *filhos, int ini, int fim, int ordem) {
	pBTree filho = criaArvoreB(ordem);
	int i, j;
	
	for (i = ini, j = 0; i < fim; i++, j++) {
		strcpy(filho->chave[j], temp[i]);
		filho->filhos[j] = filhos[i];
	}
	filho->filhos[j] = filhos[fim];
	filho->n_chaves = j;
	
	return filho;
}

//realiza a insercao na arvore B
char *insereAB_v2(pBTree raiz, char* chave){
	
	static char **temp;
	static pBTree filho2;
	int ind = binary_search(chave, raiz->chave, raiz->n_chaves);
	int i;
	
	printf("%p %d\n", raiz, ind);

	if (ind < raiz->ordem && !strcmp(raiz->chave[ind], chave)) {
		printf("CHAVE JÁ EXISTENTE\n");
		return NULL;
	} else {
		if (raiz->filhos[ind] != NULL) {
			
			char *subiu = (char *) malloc(sizeof(char) * TAM_CHAVE);
			
			char *p = insereAB_v2(raiz->filhos[ind], chave);
			
			printf("SUBIU %s %d\n", p, ind);
			
			if (p == NULL)
				return NULL;
			
			strcpy(subiu, p);
			
			if (raiz->n_chaves < raiz->ordem - 1) {
				//somente adiciona
				
				int i;
				
				for (i = raiz->n_chaves; i > ind; i--) {
					strcpy(raiz->chave[i], raiz->chave[i - 1]);
					raiz->filhos[i + 1] = raiz->filhos[i];
				}
				strcpy(raiz->chave[ind], subiu);
				raiz->filhos[ind + 1] = filho2;
				
				for (i = 0; i < raiz->ordem; ++i){
					free(temp[i]);
				}
				free(temp);
				filho2 = NULL;
				
				raiz->n_chaves++;
				
				return NULL;
			} else {
				for (i = 0; i < raiz->ordem; ++i){
					free(temp[i]);
				}
				free(temp);
				temp = (char**) malloc(sizeof(char*) * raiz->ordem);
				for (i = 0; i < raiz->ordem; ++i){
					temp[i] = (char*)malloc(sizeof(char) * TAM_CHAVE);
				}
				for (i = raiz->ordem - 1; i > ind; i--) {
					strcpy(temp[i], raiz->chave[i - 1]);
				}
				strcpy(temp[ind], subiu);
				for (i = ind - 1; i >= 0; i--) {
					strcpy(temp[i], raiz->chave[i]);
				}
				
				pBTree *filhos1 = (pBTree*)malloc(sizeof(pBTree) * raiz->ordem + 1);
				for (i = 0; i < raiz->ordem + 1; ++i){
					filhos1[i] = NULL;
				}
				
				for (i = raiz->ordem; i > ind + 1; i--) {
					filhos1[i] = raiz->filhos[i - 1];
				}
				filhos1[ind + 1] = filho2;
				for (i = ind; i >= 0; i--) {
					filhos1[i] = raiz->filhos[i];
				}
			
				char *sobe = temp[raiz->ordem / 2];
				
				for (i = 0; i < raiz->ordem / 2; i++) {
					strcpy(raiz->chave[i], temp[i]);
					raiz->filhos[i] = filhos1[i];
				}
				raiz->filhos[i] = filhos1[i];
				raiz->n_chaves = raiz->ordem / 2;
				
				pBTree new_raiz = criaPagina(raiz->ordem);
				
				int j;
				for (i = raiz->ordem / 2 + 1, j = 0; i < raiz->ordem; i++, j++) {
					strcpy(new_raiz->chave[j], temp[i]);
					new_raiz->filhos[j] = filhos1[i];
				}
				new_raiz->filhos[j] = filhos1[i];
				new_raiz->n_chaves = j;
				
				
				if (raiz == raiz->pai) {
					//aumenta o tamanho
					pBTree new_pai = criaArvoreB(raiz->ordem);
					strcpy(new_pai->chave[0], sobe);
					new_pai->filhos[0] = raiz;
					new_pai->filhos[1] = new_raiz;
					
					atualizapai(new_pai, new_pai);
					
					filho2 = NULL;
					
					new_pai->n_chaves = 1;
					return NULL;
				} else {
					//cria outra coisa temporaria
					filho2 = new_raiz;
					
					return sobe;
				}
			}
			
		} else if (raiz->n_chaves < raiz->ordem - 1) {
		
			for (i = raiz->n_chaves; ind < i; i--) {
				strcpy(raiz->chave[i], raiz->chave[i - 1]);
			}
			strcpy(raiz->chave[ind], chave);
			raiz->n_chaves++;
			return NULL;
		} else {
			temp = (char**) malloc(sizeof(char*) * raiz->ordem);
			for (i = 0; i < raiz->ordem; ++i){
				temp[i] = (char*)malloc(sizeof(char) * TAM_CHAVE);
			}
			
			for (i = raiz->ordem - 1; ind < i; i--) {
				strcpy(temp[i], raiz->chave[i - 1]);
			}
			strcpy(temp[ind], chave);
			for (i = ind - 1; i >= 0; i--) {
				strcpy(temp[i], raiz->chave[i]);
			}
			/*for (i = 0; i < raiz->ordem; ++i){
				printf("%s\t", temp[i]);
			}
			printf("\n");*/
			
			pBTree *filhos = (pBTree*)malloc(sizeof(pBTree) * raiz->ordem + 1);
			for (i = 0; i < raiz->ordem + 1; ++i){
				filhos[i] = NULL;
			}
			
			char *sobe = temp[raiz->ordem / 2];
			
			for (i = 0; i < raiz->ordem / 2; i++) {
				strcpy(raiz->chave[i], temp[i]);
			}
			raiz->n_chaves = raiz->ordem / 2;
			
			/*for (i = 0; i < raiz->n_chaves; ++i){
				printf("%s\t", raiz->chave[i]);
			}
			printf("\n");*/
			
			filho2 = create(temp, filhos, raiz->ordem / 2 + 1, raiz->ordem, raiz->ordem);
			
			/*for (i = 0; i < filho2->n_chaves; ++i){
				printf("%s\t", filho2->chave[i]);
			}
			printf("\n");*/
			filho2->pai = raiz->pai;
			
			
			if (raiz == raiz->pai) {
				//printf("Cheguei\n");
				
				//aumenta o tamanho
				pBTree new_pai = criaArvoreB(raiz->ordem);
				strcpy(new_pai->chave[0], sobe);
				new_pai->filhos[0] = raiz;
				new_pai->filhos[1] = filho2;
				
				atualizapai(new_pai, new_pai);
				
				for (i = 0; i < raiz->ordem; ++i){
					free(temp[i]);
				}
				free(temp);
				filho2 = NULL;
				
				new_pai->n_chaves = 1;
				
				return NULL;
			} else {
				
				return sobe;
			}
			
			return sobe;
		}
	}
}

pBTree insereAB_helper(pBTree raiz, char *chave) {
	insereAB_v2(raiz, chave);
	
	printf("%p %p\n", raiz, raiz->pai);
	
	return raiz->pai;
}

void limpaPagina(pBTree local){
	if (local == NULL){
		return;
	}
	int i;
	for (i = 0; i < local->n_chaves ; ++i){
		free(local->chave[i]);
	}
	free(local->chave);
	free(local->filhos);
}

//insere de forma ordenada na pagina dada
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

//imprime de outro modo a arvore
void inOrder2(pBTree raiz){
	if(raiz == NULL){
		return;
	}

	int i = 0 ;
	pBTree aux = raiz;
	
	while (i < raiz->n_chaves) {
		printf("%s\t", raiz->chave[i]);
		i++;
	}
	printf("\n");
	
	i = 0;
	while(i <= raiz->n_chaves){
		inOrder2(raiz->filhos[i]);
		i++;
	}
}

void escreveABarq(FILE *fp, pBTree arvB){
	if (fp == NULL || arvB == NULL){
		return;
	}
	int i, j;
	
	char flag = 'r';
	if (arvB->filhos[0] == NULL) {
		flag = 'f';
	}
	
	fwrite(&flag, sizeof(char), 1, fp);
	fwrite(&arvB->ordem, sizeof(int), 1, fp);
	fwrite(&arvB->n_chaves, sizeof(int), 1, fp);

	for (i = 0; i < arvB->n_chaves; i++){
		fwrite(arvB->chave[i], sizeof(char), TAM_CHAVE, fp);
	}
	fflush(fp);
	if (flag == 'r') {
		for(i = 0 ; i <= arvB->n_chaves; i++){
			escreveABarq(fp, arvB->filhos[i]);
		}
	}

	return;
}

pBTree leituraABarq(FILE *fp, pBTree arvB){
	if (fp == NULL || feof(fp) != 0){
		return NULL;
	}
	int ordem;
	size_t read;
	char flag;
	char chave[TAM_CHAVE];
	int i, j;

	read = fread(&flag, sizeof(char), 1, fp);
	read = fread(&ordem, sizeof(int), 1, fp);
	
	if (arvB == NULL) {
		arvB = criaArvoreB(ordem);
	}
	
	read = fread(&arvB->n_chaves, sizeof(int), 1, fp);

	for (i = 0; i < arvB->n_chaves; ++i){
		read = fread(chave, sizeof(char), TAM_CHAVE, fp);
		strcpy(arvB->chave[i], chave);
	}

	if (flag == 'r') {
		for(i = 0 ; i <= arvB->n_chaves ; i++){
			pBTree ret = leituraABarq(fp, NULL);
			arvB->filhos[i] = ret;
		}
	}
	
	return arvB;
}
