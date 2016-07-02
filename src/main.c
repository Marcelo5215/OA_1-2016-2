#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"
#include "bt.h"

/*
MARCELO DE ARAUJO LOPES JUNIOR - 150016794
RAFAEL DE LIMA CHEHAB - 150045123
*/

void menu();

int main(int agrc, char** argv){
	//rotinas
	tabelaInd_Prim* IP = NULL;
	pBTree arvoreB = NULL;
	FILE *saida = NULL;

	IP = NULL;
	int op, ordem, seeks;
	char nome_arq[50], chave[10];
	while(op != 99){
		menu();
		if(scanf("%d", &op) < 0){
			return -1;
		}
		getchar();

		switch (op){
			case 1:
				printf("\nDigite o nome do arquivo: ");
				if(scanf("%[^\n]", nome_arq) <0){
					return -1;
				}
				IP = criaIndicePrimario(nome_arq);
				break;
			case 2:
				printf("Digite a ordem da arvore: ");
				if(scanf("%d", &ordem) < 0){
					return -1;
				}
				if (ordem < 2){
					printf("Digite um ordem maior\n");
					break;
				}
				arvoreB = criaArvoreB(ordem);
				saida = fopen("indicelista.bt", "wb");
				break;
			case 3:
				if (arvoreB == NULL){
					printf("Crie uma arvore antes.\n");
					break;
				}
				printf("Digite a chave no formato 3 primeiras Letras e 5 números\n");
				if(scanf("%[^\n]", chave)<0){
					return -1;
				}
				arvoreB = insereAB_helper(arvoreB, chave);
				escreveABarq(saida, arvoreB);
				break;
			case 4:
				if(arvoreB == NULL){
					printf("Crie uma arvore antes\n");
					break;
				}
				printf("\n");
				inOrder(arvoreB);
				break;
			case 5:
				if(IP == NULL){
					printf("Crie um indice primeiro\n");
				}
				else{
					imprimeIndicePrimario(IP);
				}
				break;
			case 6:
				if(IP == NULL || arvoreB!= NULL){
					printf("Crie um indice Primario ou já existe um arvore B\n");
				}
				else{
					printf("Digite a ordem da arvore: ");
					if(scanf("%d", &ordem) < 0){
						return -1;
					}
					arvoreB = criaABIndicePrimario(IP, ordem);
					saida = fopen("indicelista.bt", "wb");
					escreveABarq(saida, arvoreB);
				}
				break;
			case 7:
				printf("Digite a chave no formato 3 primeiras Letras e 5 números\n");
				if(scanf("%[^\n]", chave) < 0){
					return -1;
				}
				getchar();
				if(buscaAB(arvoreB, &seeks, chave) != NULL){
					printf("seeks = %d\n", seeks);
					seeks = 0;
				}
				else{
					printf("CHAVE NÃO EXISTE\n");
				}
				break;
			case 8:
				if(arvoreB == NULL){
					printf("Crie uma arvore antes\n");
					break;
				}
				printf("\n");
				inOrder2(arvoreB);
				break;
			case 99:
				break;
			default:
				if( system("clear")){
					return -1;
				}
				printf("OPCAO INEXISTENTE.\n");
				break;
		}

		printf("PRESSIONE ENTER\n");
		getchar();
	}

	if (arvoreB == NULL){
		limpaArvoreB(arvoreB);
	}
	if (IP != NULL){
		free(IP);
	}
	if (saida != NULL){
		fclose(saida);
	}

	return 0;
}

void menu(){
	if( system("clear")){
		return;
	}

	printf("1. Abrir indice Primario\n");
	printf("2. Criar Arvore B vazia\n");
	printf("3. Adicionar chave (Arvore B)\n");
	printf("4. Vizualizar em Ordem (Arvore B)\n");
	printf("5. Vizualizar indice Primario\n");
	printf("6. CRIAR arvore B a partir de indice\n");
	printf("7. Realizar busca da arvore B\n");
	printf("8. Vizualizar arvore com as paginas\n");
	printf("99. SAIR\n");

	return;
}
