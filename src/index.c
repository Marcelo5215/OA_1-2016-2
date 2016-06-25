#include "index.h"

#define FIM_IND "final"

struct index_P{
	char key[31];
	long int byte_offset;
};

typedef struct index_P indexI;

struct tableindex_P {
	indexI *vet_ind;
	// index_S *vet_ind_sec_asso;
	int tamanho;
	int num_ind_sec;
};

//estruturas para o indice secundario
typedef struct labels{
	char chave[31];
	int pont;
}labels;

struct indexS_P{
	labels *CS;
	labels *Lab;
	int tamanhoC, tamanhoL;
};

/*Protótipos internos ao .h*/
void ordenaIndicePrimario(tabelaInd_Prim *ind, int esquerda, int direita);
char *getRegistroPrimario(FILE *fp, long int byte_offset);
int findRegistroPrimario(char *nomeArq, tabelaInd_Prim *ind, char *chave_primaria);
int findRegistroPrimarioArq(FILE *fp, tabelaInd_Prim *ind, char *chave_primaria);
void incluirRegistroPrimario(char *nomeArq, tabelaInd_Prim *ind, char *registro);
void retirarRegistroPrimario(char *nomeArq, tabelaInd_Prim *ind, char *registro);
void atualizarRegistroPrimario(char *nomeArq, tabelaInd_Prim *ind, char *regi_antigo, char *regi_novo);
void incluirRegistroSecundario(char *nomeArq, indexS *ind, char *registro, int OP);
void retirarRegistroSecundario(char *nomeArq, indexS *ind, char *registro, int OP);
void atualizarRegistroSecundario(char *nomeArq, indexS *IS, char *regi_antigo, char *regi_novo, int OP);

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
	char chave[31];          //servira para adotarmos a chave primaria de cada registro
	long int byte_offset = 0;
	int tam_indice = 1;        //tamanho atual do indice
	int i, j;
	
	tabelaInd_Prim *CP = (tabelaInd_Prim *) malloc(sizeof(tabelaInd_Prim));    //tabela de indices de Chaves Primarias
	CP->vet_ind = (indexI *) malloc(sizeof(indexI));
	CP->tamanho = 0;
	
	while(fscanf(fp,"%[^\n]\n", stringAUX) > 0){
		// aux = strtok(stringAUX, " ");
		// chave[0] = aux[0];
		// chave[1] = aux[1];
		// chave[2] = aux[3];

		// aux = strtok(NULL, " ");
		// strcpy(matricula, aux);
		// strcat(chave, matricula);
		// for (i = 0; i < 31; ++i){
		// 	chave[i] = ' ';
		// }
		// chave[30] ='\0';
		//adquiri e concatena os campos para fazer a chave primaria
		i=0; j=0;
		while(i < 31){
			if(stringAUX[j] != ' '){
				chave[i] = stringAUX[j];
				i++; j++;
			}
			else if(stringAUX[j] == ' ' && stringAUX[j+1] == ' '){
				break;
			}
			else
				j++;
		}
		chave[30] ='\0';

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
	char pivo[31];
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
	fscanf(fp,"%[^\n]\n", saida);

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
			fscanf(fp, "%[^\n]\n", string);
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
		fscanf(fp, "%[^\n]\n", string);
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

// cria um arquivo com as duas listas intercaladas
void intercalaListasPrimario(char *lista1, char *lista2){
	//arquivos a serem manipulados
	FILE *fp1, *fp2, *saida;
	fp1 = fopen(lista1, "r");
	fp2 = fopen(lista2, "r");
	saida = fopen("lista12.txt", "w");
	//indices das listas
	tabelaInd_Prim *CP1 = criaIndicePrimario(lista1);
	tabelaInd_Prim *CP2 = criaIndicePrimario(lista2);
	
	//a partir dos indices une as listas
	int i = 0 , j = 0;
	while (strcmp(CP1->vet_ind[i].key, FIM_IND) != 0 && strcmp(CP2->vet_ind[j].key, FIM_IND) != 0) {
		if(strcmp(CP1->vet_ind[i].key, CP2->vet_ind[j].key) < 0){
			fprintf(saida, "%s\n", getRegistroPrimario(fp1, CP1->vet_ind[i].byte_offset));
			i++;
		}
		else if(strcmp(CP2->vet_ind[j].key, CP1->vet_ind[i].key) < 0){
			fprintf(saida, "%s\n", getRegistroPrimario(fp2, CP2->vet_ind[j].byte_offset));
			j++;
		}
		else{
			fprintf(saida, "%s\n", getRegistroPrimario(fp2, CP2->vet_ind[j].byte_offset));
			i++; j++;
		}
	}
	// se algum dos dois não  chegou ao fim...
	if(strcmp(CP1->vet_ind[i].key, FIM_IND) == 0){
		while(strcmp(CP2->vet_ind[j].key, FIM_IND) != 0){
			fprintf(saida, "%s\n", getRegistroPrimario(fp2, CP2->vet_ind[j].byte_offset));
			j++;
		}
	} 
	else if(strcmp(CP2->vet_ind[j].key, FIM_IND) == 0){
		while(strcmp(CP1->vet_ind[i].key, FIM_IND) != 0){
			fprintf(saida, "%s\n", getRegistroPrimario(fp1, CP1->vet_ind[i].byte_offset));
			i++;
		}
	}

	fclose(fp1);
	fclose(fp2);
	fclose(saida);
	free(CP1);
	free(CP2);
}

//inclui um registro ao arquivo e no indice primario
void incluirRegistroPrimario(char *nomeArq, tabelaInd_Prim *ind, char *registro) {	
	FILE *fp = fopen(nomeArq, "r+");
	if(fp == NULL){
		printf("O arquivo não existe\n");
		return;
	}
	int i = 0, j = 0;
	char chave_primaria[31];

	//monta a chave primaria do registro novo
	for (i = 0; i < 31; ++i){
		chave_primaria[i] = ' ';
	}
	chave_primaria[30] ='\0';
	i=0; j=0;
	while(i < 31){
		if(registro[j] != ' '){
			chave_primaria[i] = registro[j];
			i++; j++;
		}
		else if(registro[j] == ' ' && registro[j+1] == ' '){
			break;
		}
		else
			j++;
	}
	chave_primaria[30] ='\0';

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

//retira um registro do arquivo e do indice secundario
//no arquivo o registro ficara com um asterisco indicando que ele foi removido
void retirarRegistroPrimario(char *nomeArq, tabelaInd_Prim *ind, char *registro){
	FILE *fp = fopen(nomeArq, "r+");
	indexI temp;
	
	if(fp == NULL){
		printf("O arquivo não existe\n");
		return;
	}

	int i, j, flag = 0;
	char chave_primaria[31], string[64];

	//monta a chave primaria do registro a ser excluido
	for (i = 0; i < 31; ++i){
		chave_primaria[i] = ' ';
	}
	chave_primaria[30] ='\0';
	i=0; j=0;
	while(i < 31){
		if(registro[j] != ' '){
			chave_primaria[i] = registro[j];
			i++; j++;
		}
		else if(registro[j] == ' ' && registro[j+1] == ' '){
			break;
		}
		else
			j++;
	}
	chave_primaria[30] ='\0';

	i = primeiroElementoIndicePrimario(ind);
	j = ultimoElementoIndicePrimario(ind);
	flag = 0;
	while (i < j) {
		int meio = (i + j) / 2;
		
		if (strcmp(ind->vet_ind[meio].key, chave_primaria) == 0) {
			i = j = meio;
			flag = 1;
			break;
		} else if (strcmp(ind->vet_ind[meio].key, chave_primaria) < 0) {
			i = meio + 1;
		} else {
			j = meio - 1;
		}
	}
	if(i == j && strcmp(ind->vet_ind[j].key, chave_primaria) == 0) {
		flag = 1;
	}
	if(!flag){
		printf("Registro inexistente\n");
		return;
	}

	//recebe o byte no qual comeca o registro
	i = findRegistroPrimarioArq(fp, ind, chave_primaria);
	strcpy(string, registro);
	string[0] = '*';
	fseek(fp, i, SEEK_SET);
	fprintf(fp, "%s", string);
	
	//busca pelo registro no arquivo e o marca como retirado
	i = primeiroElementoIndicePrimario(ind);
	j = ultimoElementoIndicePrimario(ind);
	while (i < j) {
		int meio = (i + j) / 2;
		
		if (strcmp(ind->vet_ind[meio].key, chave_primaria) == 0) {
			i = j = meio;
			break;
		} else if (strcmp(ind->vet_ind[meio].key, chave_primaria) < 0) {
			i = meio + 1;
		} else {
			j = meio - 1;
		}
	}
	chave_primaria[0] = '*';
	//retira o registro dos indices
	//i contém o registro que se deseja retirar
	strcpy(ind->vet_ind[i].key, chave_primaria);
	temp = ind->vet_ind[i];
	for (; i > 0; i--) {
		ind->vet_ind[i] = ind->vet_ind[i - 1];
	}
	ind->vet_ind[i] = temp;
	
	fclose(fp);
	return;
}

//atualiza o registro no arquivo e e no indice 
void atualizarRegistroPrimario(char *nomeArq, tabelaInd_Prim *ind, char *regi_antigo, char *regi_novo) {
	char chave_antiga[31];
	char chave_nova[31];
	int i, j;
	
	for (i = 0; i < 31; ++i){
		chave_antiga[i] = ' ';
	}
	chave_antiga[30] ='\0';
	i=0; j=0;
	while(i < 31){
		if(regi_antigo[j] != ' '){
			chave_antiga[i] = regi_antigo[j];
			i++; j++;
		}
		else if(regi_antigo[j] == ' ' && regi_antigo[j+1] == ' '){
			break;
		}
		else
			j++;
	}
	chave_antiga[30] ='\0';
	
	for (i = 0; i < 31; ++i){
		chave_nova[i] = ' ';
	}
	chave_nova[30] ='\0';
	i=0; j=0;
	while(i < 31){
		if(regi_novo[j] != ' '){
			chave_nova[i] = regi_novo[j];
			i++; j++;
		}
		else if(regi_novo[j] == ' ' && regi_novo[j+1] == ' '){
			break;
		}
		else
			j++;
	}
	chave_nova[30] ='\0';
	
	if (strcmp(chave_antiga, chave_nova) == 0) {
		//soh atualiza o arquivo
		FILE *fp = fopen(nomeArq, "r+");
		int byte = findRegistroPrimarioArq(fp, ind, chave_antiga);
		
		fseek(fp, byte, SEEK_SET);
		fprintf(fp, "%s", regi_novo);
		fclose(fp);
	} else {
		//ao se retirar, insere-se um * no arquivo e no índice,
		//e move-se ao topo do índice o elemento deletado
		retirarRegistroPrimario(nomeArq, ind, regi_antigo);
		//ao se inserir, caso haja um elemento deletado
		//remove-se o primeiro que será regi_antigo
		incluirRegistroPrimario(nomeArq, ind, regi_novo);
	}
}

//cria indice secundario, por lista invertida,
indexS *criaIndiceSecundario(char *nomeArq, int OP){
	FILE *fp = fopen(nomeArq, "r");
	indexS *IS = (indexS *)malloc(sizeof(indexS));
	IS->CS = (labels *)malloc(sizeof(labels));
	IS->Lab = (labels *)malloc(sizeof(labels));

	char string[64];
	char chave[31], chaveSe[31];
	int i, j, Lab_count = 1, CS_count = 1;
	int flag = 0;
	while(fscanf(fp, "%[^\n]\n", string) > 0){
		if(string[0] == '*'){
			j++;
			continue;
		}
		for (i = 0; i < 31; ++i){
			chave[i] = ' ';
		}
		chave[30] ='\0';
		//adquiri e concatena os campos para fazer a chave primaria
		i=0; j=0;
		while(i < 31){
			if(string[j] != ' '){
				chave[i] = string[j];
				i++; j++;
			}
			else if(string[j] == ' ' && string[j+1] == ' '){
				break;
			}
			else
				j++;
		}
		chave[30] ='\0';

		//monta a chave secundaria
		switch(OP){
			case 0:
				chaveSe[0] = string[52];
				chaveSe[1] = string[53];
				chaveSe[2] = '\0';
				break;
			case 1:
				chaveSe[0] = string[61];
				chaveSe[1] = '\0';
			default:
				exit(0);
		}
		
		//atribui a nova chave secundaria e sua respectiva chave primaria
		strcpy(IS->Lab[Lab_count-1].chave, chave);
		flag = 0;
		//verifica se ja existe tal chave Secundaria
		for(i=0; i < CS_count - 1 && flag < 1; i++){
			if(strcmp(IS->CS[i].chave, chaveSe) == 0){
				flag = 1;
			}
		}
		//se não existir é adicionado
		if(!flag){
			strcpy(IS->CS[CS_count-1].chave, chaveSe);
			IS->CS[CS_count-1].pont = Lab_count-1;
			CS_count++;
			IS->Lab[Lab_count-1].pont = -1;
			Lab_count++;
		}
		else{//monta os ponteiros das chavesP
			int anterior = -1, atual;
			
			atual = IS->CS[i - 1].pont;
			while (atual != -1 && strcmp(IS->Lab[atual].chave, chave) < 0) {
				anterior = atual;
				atual = IS->Lab[atual].pont;
			}
			
			if (anterior == -1) {
				IS->Lab[Lab_count - 1].pont = IS->CS[i - 1].pont;
				IS->CS[i - 1].pont = Lab_count - 1;
			} else {
			
				IS->Lab[Lab_count-1].pont = atual;
				IS->Lab[anterior].pont = Lab_count - 1;
			}
			Lab_count++;
		}	

		//aloca mais espaço para o proximo
		IS->CS = (labels*)realloc(IS->CS, sizeof(labels)*CS_count);
		IS->Lab = (labels*)realloc(IS->Lab, sizeof(labels)*Lab_count);
	}
	strcpy(IS->CS[CS_count-1].chave, FIM_IND);
	strcpy(IS->Lab[Lab_count-1].chave, FIM_IND);

	IS->tamanhoC =  CS_count-1;
	IS->tamanhoL =  Lab_count-1;

	fclose(fp);
	return(IS);
}

//imprime o indice secundario na tela
void imprimeIndiceSecundario(indexS *ind){
	int i=0;
	while(i < ind->tamanhoC){
		printf("%s - %4d\n", ind->CS[i].chave, ind->CS[i].pont);
		i++;
	}
	printf("\n");
	i=0;
	while(i < ind->tamanhoL){
		printf("%s - %4d\n", ind->Lab[i].chave, ind->Lab[i].pont);
		i++;
	}
}

//incui registro secundario no indice secundario e caso nao exista adiciona no indice primario e no arquivo
void incluirRegistroSecundario(char *nomeArq, indexS *ind, char *registro, int OP){
	char chaveSe[31], chave[31];
	int i = 0, j, flag = 0;

	for (i = 0; i < 31; ++i){
		chave[i] = ' ';
	}
	chave[30] ='\0';
	//adquiri e concatena os campos para fazer a chave primaria
	i=0; j=0;
	while(i < 31){
		if(registro[j] != ' '){
			chave[i] = registro[j];
			i++; j++;
		}
		else if(registro[j] == ' ' && registro[j+1] == ' '){
			break;
		}
		else
			j++;
	}
	chave[30] ='\0';
	//aloca mais espaço para o proximo se ele não estiver já como um indice
	for(i = 0; i < ind->tamanhoL ; i++){
		if(strcmp(ind->Lab[i].chave, chave) == 0){
			return;
		}
	}
	ind->tamanhoL++;
	ind->Lab = (labels *)realloc(ind->Lab, sizeof(labels)*(ind->tamanhoL+1));

	//monta a chave secundaria
	switch(OP){
		case 0:
			chaveSe[0] = registro[52];
			chaveSe[1] = registro[53];
			chaveSe[2] = '\0';
			break;
		case 1:
			chaveSe[0] = registro[61];
			chaveSe[1] = '\0';
			break;
		default:
			printf("Opção inválida.\n");
			return ;
	}
	
	//atribui a nova chave secundaria e sua respectiva chave primaria
	strcpy(ind->Lab[ind->tamanhoL-1].chave, chave);
	flag = 0;
	//verifica se ja existe tal chave Secundaria
	for(i=0; i < ind->tamanhoC  && flag < 1; i++){
		if(strcmp(ind->CS[i].chave, chaveSe) == 0){
			flag = 1;
		}
	}
	if(!flag){
		ind->tamanhoC++;
		ind->CS = (labels*)realloc(ind->CS, sizeof(labels)*(ind->tamanhoC+1));
		strcpy(ind->CS[ind->tamanhoC-1].chave, chaveSe);
		ind->CS[ind->tamanhoC-1].pont = ind->tamanhoL-1;
		ind->Lab[ind->tamanhoL-1].pont = -1;
	}
	else{//monta os ponteiros das chavesP
		
		int anterior = -1, atual;
			
			atual = ind->CS[i - 1].pont;
			while (atual != -1 && strcmp(ind->Lab[atual].chave, chave) < 0) {
				anterior = atual;
				atual = ind->Lab[atual].pont;
			}
			
			if (anterior == -1) {
				ind->Lab[ind->tamanhoL - 1].pont = ind->CS[i - 1].pont;
				ind->CS[i - 1].pont = ind->tamanhoL - 1;
			} else {
			
				ind->Lab[ind->tamanhoL - 1].pont = atual;
				ind->Lab[anterior].pont = ind->tamanhoL - 1;
			}
		
		/*i= ind->CS[i-1].pont;
		while (ind->Lab[i].pont != -1 ) {
			i = ind->Lab[i].pont;
		}
		ind->Lab[i].pont = ind->tamanhoL - 1;
		ind->Lab[ind->tamanhoL-1].pont = -1;*/
	}	
	strcpy(ind->CS[ind->tamanhoC].chave, FIM_IND);
	strcpy(ind->Lab[ind->tamanhoL].chave, FIM_IND);

	return;
}

//retira o determinado registro secundario do arquivo e dos indices
void retirarRegistroSecundario(char *nomeArq, indexS *ind, char *registro, int OP){
	char chave[31], chaveSe[31];
	int i = 0, j, k=0;

	for (i = 0; i < 31; ++i){
		chave[i] = ' ';
	}
	chave[30] ='\0';
	//adquiri e concatena os campos para fazer a chave primaria
	i=0; j=0;
	while(i < 31){
		if(registro[j] != ' '){
			chave[i] = registro[j];
			i++; j++;
		}
		else if(registro[j] == ' ' && registro[j+1] == ' '){
			break;
		}
		else
			j++;
	}
	chave[30] ='\0';

	switch(OP){
		case 0:
			chaveSe[0] = registro[52];
			chaveSe[1] = registro[53];
			chaveSe[2] = '\0';
			break;
		case 1:
			chaveSe[0] = registro[61];
			chaveSe[1] = '\0';
			break;
		default:
			printf("Opção inválida.\n");
			return ;
	}

	//realiza a remocao no local apropriado (i)
	for(i = 0; i < ind->tamanhoL ; i++){
		if(strcmp(ind->Lab[i].chave, chave) == 0){

			//muda o ponteiro do elemento anterior e do proximo na sublista do registro retirado
			j=0;
			while(j < ind->tamanhoC){
				if(ind->CS[j].pont > i){
					ind->CS[j].pont--;
				}
				if(strcmp(ind->CS[j].chave, chaveSe) == 0){
					for(k = ind->CS[j].pont; ind->Lab[k].pont != -1; k = ind->Lab[k].pont){
						if(ind->Lab[k].pont == i){
							ind->Lab[k].pont = ind->Lab[ind->Lab[k].pont].pont;
						}
					}
				}
				j++;
			}

			//copia os valores próximos para o seu lugar , dos ponteiros tambem
			j = 0;
			while(strcmp(ind->Lab[i+j].chave, FIM_IND) != 0){
				/*ind->Lab[i+j].pont = ind->Lab[i+j+1].pont;
				strcpy(ind->Lab[i+j].chave, ind->Lab[i+j+1].chave);*/
				ind->Lab[i+j] = ind->Lab[i+j+1];
				j++;
			}
			//muda os ponteiros
			j = 0;
			while(j < ind->tamanhoL){
				if(ind->Lab[j].pont > i && ind->Lab[j].pont != -1){
					ind->Lab[j].pont--;
				}
				if(ind->Lab[j].pont == i) {
					ind->Lab[j].pont = ind->Lab[ind->Lab[j].pont].pont;
				}
				j++;
			}

			//realoca para um tamanho menor
			ind->Lab = (labels *) realloc(ind->Lab, sizeof(labels)*(ind->tamanhoL));
			ind->tamanhoL--;
			strcpy(ind->Lab[ind->tamanhoL].chave, FIM_IND);
			return;
		}
	}

	printf("Registro inexistente.\n");
	getchar();
}

void atualizarRegistroSecundario(char *nomeArq, indexS *IS, char *regi_antigo, char *regi_novo, int OP) {
	
	char chaveSe_antigo[3], chaveSe_novo[3];
	
	switch(OP){
		case 0:
			chaveSe_antigo[0] = regi_antigo[52];
			chaveSe_antigo[1] = regi_antigo[53];
			chaveSe_antigo[2] = '\0';
			break;
		case 1:
			chaveSe_antigo[0] = regi_antigo[61];
			chaveSe_antigo[1] = '\0';
			break;
		default:
			printf("Opção inválida.\n");
			return ;
	}
	switch(OP){
		case 0:
			chaveSe_novo[0] = regi_novo[52];
			chaveSe_novo[1] = regi_novo[53];
			chaveSe_novo[2] = '\0';
			break;
		case 1:
			chaveSe_novo[0] = regi_novo[61];
			chaveSe_novo[1] = '\0';
			break;
		default:
			printf("Opção inválida.\n");
			return ;
	}
	
	if (strcmp(chaveSe_antigo, chaveSe_novo) != 0) {
		retirarRegistroSecundario(nomeArq, IS, regi_antigo, OP);
		incluirRegistroSecundario(nomeArq, IS, regi_novo, OP);
	}
}

void limpaIndiceSecundario(indexS *ind){
	free(ind->CS);
	free(ind->Lab);
	free(ind);
}

void incluirRegistro(char *nomeArq, tabelaInd_Prim *IP, indexS *IS, char *registro, int OP) {
	if(strlen(registro) < 62){
		printf("Registro de tamanho inadequado.\n");
		return;
	}
	incluirRegistroPrimario(nomeArq, IP, registro);
	incluirRegistroSecundario(nomeArq, IS, registro, OP);
}
void retirarRegistro(char *nomeArq, tabelaInd_Prim *IP, indexS *IS, char *registro, int OP) {
	char chave[31];
	int i, j;
	if(strlen(registro) < 63){
		printf("Registro de tamanho inadequado.\n");
		return;
	}

	//monta a crave primaria, para a verificacao
	for (i = 0; i < 31; ++i){
		chave[i] = ' ';
	}
	chave[30] ='\0';
	//adquiri e concatena os campos para fazer a chave primaria
	i=0; j=0;
	while(i < 31){
		if(registro[j] != ' '){
			chave[i] = registro[j];
			i++; j++;
		}
		else if(registro[j] == ' ' && registro[j+1] == ' '){
			break;
		}
		else
			j++;
	}
	chave[30] ='\0';
	if(findRegistroPrimario(nomeArq, IP, chave) == -1){
		printf("Registro inexistente.\n");
		return;
	}
	retirarRegistroPrimario(nomeArq, IP, registro);
	retirarRegistroSecundario(nomeArq, IS, registro, OP);
}

void atualizarRegistro(char *nomeArq, tabelaInd_Prim *IP, indexS *IS, char *regi_antigo, char *regi_novo, int OP) {
	char chave[31];
	int i, j;
	//monta a crave primaria, para a verificacao de regi_antigo
	for (i = 0; i < 31; ++i){
		chave[i] = ' ';
	}
	chave[30] ='\0';
	//adquiri e concatena os campos para fazer a chave primaria
	i=0; j=0;
	while(i < 31){
		if(regi_antigo[j] != ' '){
			chave[i] = regi_antigo[j];
			i++; j++;
		}
		else if(regi_antigo[j] == ' ' && regi_antigo[j+1] == ' '){
			break;
		}
		else
			j++;
	}
	chave[30] ='\0';
	if(findRegistroPrimario(nomeArq, IP, chave) == -1){
		printf("Registro inexistente.\n");
		return;
	}
	atualizarRegistroPrimario(nomeArq, IP, regi_antigo, regi_novo);
	atualizarRegistroSecundario(nomeArq, IS, regi_antigo, regi_novo, OP);
}

#undef FIM_IND
