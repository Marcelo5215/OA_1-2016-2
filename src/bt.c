#include "bt.h"

int insere(pBTree local, char* chave);
void limpaPagina(pBTree local);


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

	arvB->pai = NULL;
	
	return arvB;
}

arvB_ret atualizapai(pBTree arvB, pBTree pai){
	if(arvB == NULL){
		return ARVB_ERR;
	}
	int i;
	
	arvB->pai = pai;

	for (i = 0; i < arvB->ordem ; ++i){
		if(atualizapai(arvB->filhos[i], pai) == ARVB_ERR){
			break;
		}
	}

	return ARVB_OK;
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

pBTree criaABIndicePrimario(tabelaInd_Prim *ind, int ordem){
	pBTree arvB = criaArvoreB(ordem);
	char *chave;
	int i;
	for (i = 0; i < ultimoElementoIndicePrimario(ind) ; ++i){
		chave = getKey(ind, i);
		printf("%s\n", chave);
		insereAB(&arvB, chave);
	}

	return arvB;
}

pBTree insereAB_sec(pBTree raiz, char* chave){
	int i, index, novo_n, j;
	char promotedKey[10];
	pBTree filho, nova_raiz, pont;

	//se existir lugar na pagina raiz
	if (raiz->n_chaves < raiz->ordem - 1){
		insere(raiz, chave);
		return raiz;
	}
	else{
		//busca o nó folha onde o filho deveria ser inserido
		filho = buscaDir(raiz, chave);
		//se existir lugar no nó do filho
		if (filho->n_chaves < filho->ordem - 1){
			insere(filho, chave);
			return raiz;
		}
		else{
			//split
			pBTree esq = raiz;
			pBTree dir = criaArvoreB(raiz->ordem);
			//copia metade das chaves do raiz para a esquerda(esq), eoutra metade para a direita(dir)
			//e a chave no meio será a chave promovida 
			novo_n = 0; 
			strcpy(promotedKey, raiz->chave[raiz->n_chaves/2]);
			for (i = 0; i < raiz->n_chaves/2; ++i){
			  	strcpy(esq->chave[i], raiz->chave[i]);
			  	novo_n++;
			}
			j = 0; 
			for (i = raiz->n_chaves/2 + 1; i < raiz->n_chaves; ++i, j++){
			  	strcpy(dir->chave[j], raiz->chave[i]);
			  	dir->n_chaves++;
			} 
			raiz->n_chaves = novo_n;
			//caso o pai exista e tenha vaga
			if (raiz->pai != NULL && raiz->pai->n_chaves < raiz->pai->ordem -1){
				index = insere(raiz->pai, raiz->chave[raiz->n_chaves/2]);
				if(index > 0){
					raiz->pai->filhos[index - 1] = esq;
					raiz->pai->filhos[index] = dir;
				}
				else if (index == 0){
					raiz->pai->filhos[0] = esq;
					raiz->pai->filhos[1] = dir;
				}
				else{
					raiz->pai->filhos[raiz->pai->ordem-1] = dir;
					raiz->pai->filhos[raiz->pai->ordem-2] = esq;
				}
				return raiz->pai;
			} //CASO NÃO TENHA VAGA
			else if (raiz->pai != NULL){
				nova_raiz = insereAB_sec(raiz->pai, promotedKey);
				pBTree AUX = nova_raiz;
				//busca pelo local onde o esquerdo deve ir
				index = 0;
				do{
					i=0;
					while(strcmp(AUX->chave[i], esq->chave[0]) < 0 && i < AUX->n_chaves){
						i++;
						index++;
					}
					AUX = AUX->filhos[index];
				}while(AUX->filhos[0] != NULL);
				i=0;
				while(strcmp(AUX->chave[i], esq->chave[0]) < 0 && i < AUX->n_chaves){
					i++;
					index++;
				}
				esq->pai = AUX;
				dir->pai = AUX;
				if(index > 0){
					AUX->filhos[index] = esq;
					AUX->filhos[index + 1] = dir;
				}
				else if (index == 0){
					AUX->filhos[0] = esq;
					AUX->filhos[1] = dir;
				}
				else{
					AUX->filhos[AUX->ordem-1] = dir;
					AUX->filhos[AUX->ordem-2] = esq;
				}

				return nova_raiz;
			}
			//monta a nova raiz
			nova_raiz = criaArvoreB(raiz->ordem);
			index = insere(nova_raiz, promotedKey);
			insere(dir, chave);
			esq->pai = nova_raiz;
			dir->pai = nova_raiz;
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


//modificar para tb adicionar ponteiros
pBTree create (char **temp, pBTree *filhos, int ini, int fim, int ordem) {
	pBTree filho = criaArvoreB(ordem);
	int i, j;
	
	for (i = ini, j = 0; i < fim; i++, j++) {
		strcpy(filho->chave[j], temp[i]);
		filho->filho[j] = filhos[i];
	}
	filhos->filhos[j] = filhos[fim];
	filho->n_chaves = j;
	
	return filho;
}

char *insereAB_v2(pBTree raiz, char* chave){
	
	static char **temp;
	static pBTree filhos1;
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
				raiz->filhos[raiz->n_chaves + 1] = raiz->filhos[raiz->n_chaves];
				int i;
				for (i = raiz->n_chaves; i > ind; i--) {
					strcpy(raiz->chave[i], raiz->chave[i - 1]);
					raiz->filhos[i] = raiz->filhos[i - 1];
				}
				strcpy(raiz->chave[ind], subiu);
				raiz->filhos[ind] = filho2;
				
				for (i = 0; i < raiz->ordem; ++i){
					free(temp[i]);
				}
				free(temp);
				filho2 = NULL;
				
				return NULL;
			} else {
				char **temp1 = (char**) malloc(sizeof(char*) * raiz->ordem);
				for (i = 0; i < raiz->ordem; ++i){
					temp1[i] = (char*)malloc(sizeof(char) * TAM_CHAVE);
				}
				for (i = raiz->ordem - 1; ind < i; i--) {
					strcpy(temp[i], raiz->chave[i - 1]);
				}
				strcpy(temp[ind], subiu);
				for (i = ind - 1; i >= 0; i--) {
					strcpy(temp[i], raiz->chave[i]);
				}
				
				
				filhos1 = (pBTree*)malloc(sizeof(pBTree) * raiz->ordem + 1);
				for (i = 0; i < raiz->ordem + 1; ++i){
					filhos1[i] = NULL;
				}
				
				for (i = raiz->ordem; i > ind + 1; i--) {
					filhos1[i] = raiz->filhos[i - 1];
				}
				filhos1[ind + 1] = filhos2;
				for (i = ind; i >= 0; i--) {
					filhos1[i] = raiz->filhos[i];
				}
				
				//INACABADO
				//TODO: atualizar raiz e criar nova raiz
				//subir um nível
			
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
					new_raiz->filhos[j + 1] = filhos1[i];
				}
				new_raiz->filhos0i] = filhos2;
				new_raiz->n_chaves = raiz->ordem / 2;
				
				if (raiz == raiz->pai) {
					pBTree new_pai = criaArvoreB(raiz->ordem);
					
					//aumenta o tamanho
				} else {
					//cria outra coisa temporaria
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
			for (i = 0; i < ordem; ++i){
				temp[i] = (char*)malloc(sizeof(char) * TAM_CHAVE);
			}
			
			for (i = raiz->ordem - 1; ind < i; i--) {
				strcpy(temp[i], raiz->chave[i - 1]);
			}
			strcpy(temp[ind], chave);
			for (i = ind - 1; i >= 0; i--) {
				strcpy(temp[i], raiz->chave[i]);
			}
			
			
			filhos = (pBTree*)malloc(sizeof(pBTree) * raiz->ordem + 1);
			for (i = 0; i < raiz->ordem + 1; ++i){
				filhos[i] = NULL;
			}
			
			char *sobe = temp[raiz->ordem / 2];
			
			for (i = 0; i < raiz->ordem / 2; i++) {
				strcpy(raiz->chave[i], temp[i]);
			}
			raiz->n_chaves = raiz->ordem / 2;
			
			filho2 = create(temp, filhos, raiz->ordem / 2 + 1, raiz->ordem, raiz->ordem);
			filho2->pai = raiz->pai;
			
			return sobe;
		}
	}
}

void insereAB_rec(pBTree raiz, char* chave){

}

void insereAB(pBTree *raiz, char* chave){
	if (raiz == NULL){
		return;
	}
	//variaveis
	int i = 0, index;

	if((*raiz)->n_chaves < (*raiz)->ordem-1 && (*raiz)->filhos[0] == NULL){
		printf("CASO - 0\n");
		insere((*raiz), chave);
	}
	else{
		//busca local de insercao
		pBTree AUX;
		AUX = (*raiz);
		while(AUX->filhos[index] != NULL){
			index = 0;
			while(strcmp(AUX->chave[i], chave) < 0 && i < AUX->n_chaves){
				i++;
				index++;
			}
			AUX =  AUX->filhos[index];
		}
		//se não houver nenhum pai (raiz da arvore), split
		if (AUX->pai == NULL){
			printf("CASO - 1\n");
			pBTree esquerda = criaArvoreB((*raiz)->ordem);
			pBTree direita = criaArvoreB((*raiz)->ordem);
			//pagina da esquerda
			for (i = 0; i < (*raiz)->n_chaves/2; ++i){
				strcpy(esquerda->chave[i], (*raiz)->chave[i]);
				esquerda->n_chaves++;
			}
			//pagina da direita
			for (i = (*raiz)->n_chaves/2 + 1 ; i < (*raiz)->n_chaves; ++i){
				strcpy(direita->chave[i], (*raiz)->chave[i]);
				direita->n_chaves++;
			}
			insere(direita, chave);

			pBTree nova_raiz = criaArvoreB((*raiz)->ordem);
			//insere a chave promovida na nova raiz
			insere(nova_raiz, (*raiz)->chave[(*raiz)->n_chaves/2]);
			esquerda->pai = nova_raiz;
			direita->pai = nova_raiz;
			nova_raiz->filhos[0] = esquerda;
			nova_raiz->filhos[1] = direita;
			limpaPagina((*raiz));
			(*raiz) = nova_raiz;

			return;
		}
		//se houver pai
		else{
			//se for possivel inserir no local
			if (AUX->n_chaves < AUX->ordem-1){
				printf("CASO - 2\n");
				insere(AUX, chave);
				return;
			}
			else {
				//necessário realizar o split novamente
				pBTree esquerda = criaArvoreB((*raiz)->ordem);
				pBTree direita = criaArvoreB((*raiz)->ordem);
				//pagina da esquerda
				for (i = 0; i < AUX->n_chaves/2; ++i){
					strcpy(esquerda->chave[i], AUX->chave[i]);
					esquerda->n_chaves++;
				}
				//pagina da direita
				for (i = AUX->n_chaves/2 + 1 ; i < AUX->n_chaves; ++i){
					strcpy(direita->chave[i], AUX->chave[i]);
					direita->n_chaves++;
				}
				insere(direita, chave);
				//se tiver espaço no pai
				if (AUX->pai->n_chaves < AUX->pai->ordem -1){
					printf("CASO - 3\n");
					index = insere(AUX->pai, AUX->chave[AUX->n_chaves/2]);
					limpaPagina(AUX->pai->filhos[index]);
					AUX->pai->filhos[index] = esquerda;
					AUX->pai->filhos[index+1] = direita;
					esquerda->pai = AUX->pai;
					direita->pai = AUX->pai;
					return;
				}
				//caso o pai não tenha espaco, procurase um pai que tenha
				else{
					printf("INACABADO\n");
				}
			}
		}
	}
	return;
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
	//free(local);
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

// pBTree buscaDir(pBTree raiz, char *chave){
// 	int i = 0;
// 	int index = 0;
// 	while(strcmp(raiz->chave[i], chave) < 0 && i < raiz->n_chaves){
// 		i++;
// 		index++;
// 	}
// 	if(i == raiz->n_chaves-1){
// 		index++;
// 	}
// 	else if(raiz->filhos[index] == NULL){
// 		return raiz;
// 	}
// 	return (buscaDir(raiz->filhos[index], chave));
// }

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
	fwrite(arvB, sizeof(BTree), 1, fp);

	for (i = 0; i < arvB->n_chaves; i++){
		fwrite(arvB->chave[i], sizeof(char), TAM_CHAVE, fp);
	}

	for(i = 0 ; i < arvB->ordem ; i++){
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
		fread(chave, sizeof(char), TAM_CHAVE, fp);
		strcpy(arvB->chave[i], chave);
	}

	for(i = 0 ; i <= arvB->ordem ; i++){
		leituraABarq(fp, arvB->filhos[i]);
	}
	
	return;
}
