#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"

/*
MARCELO DE ARAUJO LOPES JUNIOR - 150016794
RAFAEL DE LIMA CHEHAB - 150045123

PROGRAMA DE TESTE, CUIDADO DURANTE A UTILIZACAO.
- CRIE OS INDICES PRIMARIOS E SECUNDARIOS ANTES DE UTILIZAR OUTROS RECURSOS
- CUIDADO COM A OPCAO FEITA NO INDICE SECUNDARIO, SEMPRE UTILIZE A MESMA, CASO CONTRARIO O INDICE SECUNDARIO PODE NAO FUNCIONAR CORRETAMENTE
- CERTIFIQUE-SE DE DIGITAR OS CAMPOS CORRETOS QUANDO SOLICITADOS
*/

void menu(){
	system("clear");
	printf("*****************************************************\n");
	printf("*  1 - Cria Indice Primario.         				\n");
	printf("*  2 - Imprime Indice Primario.      				\n");
	printf("*  3 - Insere Registro.			       				\n");
	printf("*  4 - Retira Registro.       						\n");
	printf("*  5 - Atualiza Indice Primario.     				\n");
	printf("*  6 - Intercala Listas.             				\n");
	printf("*  7 - Cria Indice Secundario             			\n");
	printf("*  8 - Imprime Indice Secundario           			\n");
	printf("*  9 - Imprime Indice Primario em um arquivo 		\n");
	printf("*  99 - Sair          	                        	\n");
	printf("*****************************************************\n");
	printf("Opcao:   ");
}

char* leRegistro(){
	int i;
	static char registro[64];
	char matric[7], nome[42], turma[2], opcao[3], curso[3];
	printf("Digite a matricula (6 caracteres): \n");
	scanf("%[^\n]", matric);
	getchar();
	printf("Digite o nome (ate 40 caracteres): \n");
	scanf("%[^\n]", nome);
	getchar();
	printf("Digite a turma (1 caracter): \n");
	scanf("%[^\n]", turma);
	getchar();
	printf("Digite a opcao (2 caracteres): \n");
	scanf("%[^\n]", opcao);
	getchar();
	printf("Digite o curso (2 caracteres): \n");
	scanf("%[^\n]", curso);
	getchar();

 	strcat(registro, matric);
	strcat(registro, " ");
	strcat(registro, nome);
	for(i = strlen(nome); i < 41;i++){
		strcat(registro, " ");
	}
	strcat(registro, opcao);	
	for(i = 2; i < 4;i++){
		strcat(registro, " ");
	}
	strcat(registro, curso);	
	for(i = 2; i < 9;i++){
		strcat(registro, " ");
	}
	strcat(registro, turma);
	strcat(registro, "\0");

	return registro;
}


int main(int agrc, char** argv){

	tabelaInd_Prim* IP = NULL;
	int A, op;
	indexS* IS = NULL;
	char nomeArq[50], nomeArq2[50];

	do{
		menu();
		scanf("%d", &A);
		getchar();

		switch(A){
			case 1:
				printf("Digite o nome do arquivo:\n");
				scanf("%[^\n]s", nomeArq);
				getchar();
				IP = criaIndicePrimario(nomeArq);
				break;

			case 2:
				if(IP == NULL){
					printf("Crie um indice primario");
					getchar();
					break;
				}
				imprimeIndicePrimario(IP);
				getchar();
				break;

			case 3:	
				if(IS == NULL){
					printf("Crie um indice secundario");
					getchar();
					break;
				}
				if(IP == NULL){
					printf("Crie um indice primario");
					getchar();
					break;
				}			
				printf("Opcoes: Curso(0), Turma(1)\n");
				scanf("%d", &op);
				getchar();
				incluirRegistro(nomeArq, IP, IS, leRegistro(), op);
				getchar();
				break;

			case 4:	
				if(IS == NULL){
					printf("Crie um indice secundario");
					getchar();
					break;
				}
				if(IP == NULL){
					printf("Crie um indice primario");
					getchar();
					break;
				}
				printf("Opcoes: Curso(0), Turma(1)\n");
				scanf("%d", &op);
				getchar();
				retirarRegistro(nomeArq, IP, IS, leRegistro(), op);
				getchar();
				break;

			case 5:
				if(IS == NULL){
					printf("Crie um indice secundario");
					break;
				}
				if(IP == NULL){
					printf("Crie um indice primario");
					break;
				}
				printf("Primeiro registro solitado sera o a ser atualizado, e o segundo oque.\n\n");
				atualizarRegistro(nomeArq, IP, IS, leRegistro(), leRegistro(), 0);
				break;

			case 6:
				intercalaListasPrimario((char*)"lista1.txt", (char*)"lista2.txt");
				printf("Cheque listas12.txt\n");
				getchar();
				break;

			case 7:
				printf("Digite o nome do arquivo:\n");
				scanf("%[^\n]", nomeArq2);
				getchar();
				printf("Opcoes: Curso(0), Turma(1)\n");
				scanf("%d", &op);
				getchar();
				IS = criaIndiceSecundario(nomeArq2, op);
				break;

			case 8:
				if(IS == NULL){
					printf("Crie um indice secundario");
					getchar();
					break;
				}
				imprimeIndiceSecundario(IS);
				getchar();
				break;

			case 9:
				if(IP == NULL){
					printf("Crie um indice primario");
					getchar();
					break;
				}				
				imprimeIndicePrimarioArq(IP, "indiceLista.ind");
				getchar();
				break;

			case 99:
				break;

			default:
				printf("Opcao inexistente.\n");
				getchar();
				break;
		}


	}while(A != 99);


	if(IP != NULL)
		free(IP);
	if(IS != NULL)
		limpaIndiceSecundario(IS);

	return 0;
}
