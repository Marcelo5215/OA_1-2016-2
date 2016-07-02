#include "index.h"

#define FIM_IND "final"

struct index_P{
	char key[9];
	long int byte_offset;
};

typedef struct index_P indexI;

struct tableindex_P {
	indexI *vet_ind;
	int tamanho;
	int num_ind_sec;
};

/*Protótipos internos ao .h*/
void ordenaIndicePrimario(tabelaInd_Prim *ind, int esquerda, int direita);
char *getRegistroPrimario(FILE *fp, long int byte_offset);
int findRegistroPrimario(char *nomeArq, tabelaInd_Prim *ind, char *chave_primaria);
int findRegistroPrimarioArq(FILE *fp, tabelaInd_Prim *ind, char *chave_primaria);
void incluirRegistroPrimario(char *nomeArq, tabelaInd_Prim *ind, char *registro);
int IsNumeric(char* String);

int primeiroElementoIndicePrimario(tabelaInd_Prim *ind) { return (ind == NULL) ? -1 : 0; }
//Há ind->tamanho elementos, de 0 até ind->tamanho -1, incluindo FIM_IND que eh o ind->tamanho - 1
int ultimoElementoIndicePrimario(tabelaInd_Prim *ind) { return (ind == NULL) ? -1 : ind->tamanho - 1; }

/* Cria um indice a partir de um arquivo do modo especificado:      *
 * MATRIC       NOME                      OP   CURSO  TURMA         *                    
 * 150016794    Marcelo de Araujo Lopes   00    EC      A           *
 * com uma chave primaria que e a concatenacao dos campos matricula *
 * e nome, e já ordenado.                                           */
tabelaInd_Prim *criaIndicePrimario(char *nomeArq){
	FILE *fp;
	fp = fopen(nomeArq, "r+");

	char stringAUX[64];      //servira apenas para ler as linhas do arquivo
	char chave[10], *aux;          //servira para adotarmos a chave primaria de cada registro
	long int byte_offset = 0;
	int tam_indice = 1;        //tamanho atual do indice
	
	tabelaInd_Prim *CP = (tabelaInd_Prim *) malloc(sizeof(tabelaInd_Prim));    //tabela de indices de Chaves Primarias
	CP->vet_ind = (indexI *) malloc(sizeof(indexI));
	CP->tamanho = 0;
	
	while(fscanf(fp,"%[^\n]\n", stringAUX) > 0){
		//adquiri e concatena os campos para fazer a chave primaria
		aux = strtok(stringAUX, " ");
		chave[0] = aux[0];
		chave[1] = aux[1] - 32;
		chave[2] = aux[2] - 32;
		chave[3] = '\0';

		do{
			aux = strtok(NULL, " ");
		}while(aux != NULL && IsNumeric(aux) == 0);
		strcat(chave, aux);
		chave[9] = '\0';

		//atribui os valores ao indice
		CP->vet_ind[tam_indice-1].byte_offset = byte_offset;
		strcpy(CP->vet_ind[tam_indice-1].key, chave);
		CP->tamanho++;
		tam_indice++; 

		//aloca mais um espaco para o proximo registro no indice 
		CP->vet_ind = (indexI *) realloc(CP->vet_ind, sizeof(indexI) * tam_indice);
		//calcula o byte_offset do prox
		byte_offset = byte_offset + TAM_REG;

	}
	//um espaco será sempre alocado a mais, agora usaremos ele para indicar o final...
	strcpy(CP->vet_ind[tam_indice-1].key, FIM_IND);
	CP->vet_ind[tam_indice-1].byte_offset = 0;
	CP->tamanho = tam_indice;

	//ordena os indices
	ordenaIndicePrimario(CP, primeiroElementoIndicePrimario(CP), ultimoElementoIndicePrimario(CP));

	fclose(fp);

	return CP;
}

//imprime o indice
void imprimeIndicePrimario(tabelaInd_Prim *ind){
	int i=0;

	while(strcmp(ind->vet_ind[i].key, FIM_IND) != 0){
		if(ind->vet_ind[i].key[0] == '*'){
			i++;
			continue;
		}
		printf("%s --- %10ld\n", ind->vet_ind[i].key, ind->vet_ind[i].byte_offset);
		i++;
	}
	printf("%s\n", ind->vet_ind[i].key);
}
//cria um arquivo com o indice primario 
void imprimeIndicePrimarioArq(tabelaInd_Prim *ind, char *nomeArq){
	FILE *fp = fopen(nomeArq, "w");

	for(int i = 0 ; i < ultimoElementoIndicePrimario(ind) ; i++){
		if(ind->vet_ind[i].key[0] == '*'){
			i++;
			continue;
		}
		fprintf(fp, "%s       %10ld\n", ind->vet_ind[i].key, ind->vet_ind[i]. byte_offset);
	}

	fclose(fp);
}

//ordena o indice primario com o mecanismmo do quicksort recursivo
void ordenaIndicePrimario(tabelaInd_Prim *ind, int esquerda, int direita){
	if(direita >  ind->tamanho){
		printf("Tamanho inadequado.\n");
		return;
	}
	int i = esquerda, j = direita;
	char pivo[9];
	strcpy(pivo, ind->vet_ind[esquerda].key);
	indexI temp;
	
	while(j >= i){
		while(strcmp(ind->vet_ind[i].key, pivo) < 0){
			i++;
		}
		while(strcmp(ind->vet_ind[j].key, pivo) > 0){
			j--;
		}
		if(j >= i){
			temp = ind->vet_ind[i];
			ind->vet_ind[i] = ind->vet_ind[j];
			ind->vet_ind[j] = temp;
			i++;
			j--;
		}
	};
	if(j > esquerda){
		ordenaIndicePrimario(ind, esquerda, j);
	}
	if(i < direita){
		ordenaIndicePrimario(ind, i, direita);
	}
} 

//retorna o registro em determinado byte_offset
char *getRegistroPrimario(FILE *fp, long int byte_offset){
	static char saida[64];
	fseek(fp, byte_offset, SEEK_SET);
	if (fscanf(fp,"%[^\n]\n", saida)<0){
		printf("ERRO no arquivo\n");
	}

	return saida;
}

//procura pelo registro com determinada chave, busca binaria
//retorna posição que ocupa no índice primário
int findRegistroPrimario(char *nomeArq, tabelaInd_Prim *ind, char *chave_primaria) {
	FILE *fp = fopen(nomeArq, "r");
	char string[64];
	int i, j;
	if (fp == NULL) {
		printf("Arquivo inexistente\n");
		return -1;
	}
	
	i = primeiroElementoIndicePrimario(ind);
	j = ultimoElementoIndicePrimario(ind);
	
	while (i < j) {
		int meio = (i + j) / 2;
		
		if (strcmp(ind->vet_ind[meio].key, chave_primaria) == 0) {
			fseek(fp, ind->vet_ind[meio].byte_offset, SEEK_SET);
			if (fscanf(fp, "%[^\n]\n", string) < 0){
				printf("ERRO no arquivo\n");
			}
			printf("%s\n", string);
			fclose(fp);
			return meio;
		} else if (strcmp(ind->vet_ind[meio].key, chave_primaria) < 0) {
			i = meio + 1;
		} else {
			j = meio - 1;
		}
	}
	
	if(i == j && strcmp(ind->vet_ind[j].key, chave_primaria) == 0) {
		fseek(fp, ind->vet_ind[i].byte_offset, SEEK_SET);
		if (fscanf(fp, "%[^\n]\n", string) < 0){
			printf("ERRO no arquivo\n");
		}
		printf("%s\n", string);
		fclose(fp);
		return i;
	}
	printf("Nao existe essa chave\n");
	fclose(fp);
	return -1;
}

//procura pelo registro com determinada chave, busca binaria
//retorna byte que ocupa no arquivo
int findRegistroPrimarioArq(FILE *fp, tabelaInd_Prim *ind, char *chave_primaria){
	int i, j;
	if (fp == NULL) {
		printf("Arquivo inexistente\n");
		return -1;
	}
	
	i = primeiroElementoIndicePrimario(ind);
	j = ultimoElementoIndicePrimario(ind);
	
	while (i < j) {
		int meio = (i + j) / 2;
		
		if (strcmp(ind->vet_ind[meio].key, chave_primaria) == 0) {
			fseek(fp, ind->vet_ind[meio].byte_offset, SEEK_SET);
			return ind->vet_ind[meio].byte_offset;
		} else if (strcmp(ind->vet_ind[meio].key, chave_primaria) < 0) {
			i = meio + 1;
		} else {
			j = meio - 1;
		}
	}
	
	if(i == j && strcmp(ind->vet_ind[j].key, chave_primaria) == 0) {
		fseek(fp, ind->vet_ind[i].byte_offset, SEEK_SET);
		return ind->vet_ind[i].byte_offset;
	}
	printf("Nao existe essa chave\n");
	return -1;
}

//inclui um registro ao arquivo e no indice primario
void incluirRegistroPrimario(char *nomeArq, tabelaInd_Prim *ind, char *registro) {	
	FILE *fp = fopen(nomeArq, "r+");
	if(fp == NULL){
		printf("O arquivo não existe\n");
		return;
	}
	int i = 0, j = 0;
	char chave_primaria[10], *aux;

	//monta a chave primaria do registro novo
	aux = strtok(registro, " ");
	chave_primaria[0] = aux[0];
	chave_primaria[1] = aux[1] - 32;
	chave_primaria[2] = aux[2] - 32;
	chave_primaria[3] = '\0';

	do{
		aux = strtok(NULL, " ");
		printf("%s\n", aux);
	}while(aux != NULL && IsNumeric(aux) == 0);
	strcat(chave_primaria, aux);
	chave_primaria[9] = '\0';

	//verifica se ele já existe
	i = primeiroElementoIndicePrimario(ind);
	j = ultimoElementoIndicePrimario(ind);
	while (i < j) {
		int meio = (i + j) / 2;
		
		if (strcmp(ind->vet_ind[meio].key, chave_primaria) == 0) {
			printf("Registro já existente\n");
			fclose(fp);
			return;
		} else if (strcmp(ind->vet_ind[meio].key, chave_primaria) < 0) {
			i = meio + 1;
		} else {
			j = meio - 1;
		}
	}
	
	if(i == j && strcmp(ind->vet_ind[j].key, chave_primaria) == 0) {
		printf("Registro já existente\n");
		fclose(fp);
		return;
	}
	
	//procura o local no qual se deve adiconar
	if (i == j) {
		if (strcmp(ind->vet_ind[i].key, FIM_IND) == 0) {
			//j já está correto
		} else if (strcmp(ind->vet_ind[j].key, chave_primaria) < 0) {
			j++;
		} else {
			//j já está correto
		}
	} else {
		//i > j
		j = i;
	}
	
	if (ind->vet_ind[0].key[0] == '*') { //se há algo a se retirar

		int byte = ind->vet_ind[0].byte_offset;
		
		//passa todos abaixo do local a ser inserido para o começo
		for (i = primeiroElementoIndicePrimario(ind); i < j - 1; i++){
			ind->vet_ind[i] = ind->vet_ind[i + 1];
		}
		
		strcpy(ind->vet_ind[j - 1].key, chave_primaria);
		ind->vet_ind[j - 1].byte_offset = byte;
	
		fseek(fp, byte, SEEK_SET);
		fprintf(fp, "%s", registro);
	} else {
		
		ind->tamanho++;
		ind->vet_ind = (indexI *) realloc(ind->vet_ind, sizeof(indexI) * ind->tamanho);

		//passa todos do local a ser inserido para a frente
		for (i = ultimoElementoIndicePrimario(ind); i > j; i--){
			ind->vet_ind[i] = ind->vet_ind[i-1];
		}
		
		strcpy(ind->vet_ind[j].key, chave_primaria);
		//atualiza o byte_offset do elemento inserido, como foi inserido ao final do arquivo só é necessário colocar o dele
		//Há ind->tamanho registros incluindo o que está sendo inserido e o final,
		//logo há ind->tamanho registros antes dele, cada um com tamanho TAM_REG
		ind->vet_ind[j].byte_offset = (ind->tamanho - 2) * TAM_REG;
		
		fseek(fp, 0, SEEK_END);
		fprintf(fp, "%s\r\n", registro);
	}
	fclose(fp);
}

int IsNumeric(char* String){
	int FirstDigit = 48;
   	int Result = 1;
   	for(int i=0; i<strlen(String); i++){
   	   if((String[i] < FirstDigit) || (String[i] > FirstDigit + 9)) Result = 0;
   	}
   	return Result;
}

char* getKey(tabelaInd_Prim *ind, int index){
	return (ind->vet_ind[index].key);
}

#undef FIM_IND
