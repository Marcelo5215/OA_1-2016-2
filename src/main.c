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

// 	tabelaInd_Prim* IP = NULL;
// 	char nomeArq[50], nomeArq2[50];

// 	IP = criaIndicePrimario("lista.txt");
// 	imprimeIndicePrimario(IP);

// 	pBTree arvB = criaArvoreB(3);
// 	//insereAB(&arvB, getKey(IP, 0));
// 	//insereAB(&arvB, getKey(IP, 1));
// 	//insereAB(&arvB, getKey(IP, 2));
// 	//insereAB(&arvB, getKey(IP, 3));
// 	//insereAB(&arvB, getKey(IP, 4));
// 	//insereAB(&arvB, getKey(IP, 5));
	
// 	arvB = insereAB_helper(arvB, getKey(IP, 0));
// 	arvB = insereAB_helper(arvB, getKey(IP, 1));
// 	arvB = insereAB_helper(arvB, getKey(IP, 2));
// 	arvB = insereAB_helper(arvB, getKey(IP, 3));
// 	arvB = insereAB_helper(arvB, getKey(IP, 4));
// 	arvB = insereAB_helper(arvB, getKey(IP, 5));
// 	// arvB = insereAB(arvB, getKey(IP, 6));
// 	inOrder(arvB);

// 	//escrita
// 	FILE *w = fopen("saida.bt", "wb");
// 	escreveABarq(w, arvB);
// 	fclose(w);

// 	printf("LEITURA:  \n");
// // 	pBTree arv = criaArvoreB(3);
// 	FILE *e = fopen("saida.bt", "rb");
// 	pBTree arv = leituraABarq(e, arv);
// 	inOrder(arv);
// 	fclose(e);

// 	int seeks;

// 	if(buscaAB(arv, &seeks, getKey(IP, 0)) != NULL){
// 		printf("seeks = %d\n", seeks);
// 		seeks = 0;
// 	}

// 	if(buscaAB(arv, &seeks, getKey(IP, 1)) != NULL){
// 		printf("seeks = %d\n", seeks);
// 		seeks = 0;
// 	}
// 	if(buscaAB(arv, &seeks, getKey(IP, 3)) != NULL){
// 		printf("seeks = %d\n", seeks);
// 		seeks = 0;
// 	}


// 	free(IP);
// 	limpaArvoreB(arvB);
// 	//free(arv);

	//rotinas
	tabelaInd_Prim* IP = NULL;
	pBTree arvoreB = NULL;
	FILE *saida;

	IP = NULL;
	int op, ordem, seeks;
	char nome_arq[50], chave[10];
	while(op != 99){
		menu();
		scanf("%d", &op);
		getchar();

		switch (op){
			case 1:
				printf("\nDigite o nome do arquivo: ");
				scanf("%[^\n]", nome_arq);
				IP = criaIndicePrimario(nome_arq);
				break;
			case 2:
				printf("Digite a ordem da arvore: ");
				scanf("%d", &ordem);
				if (ordem < 2){
					printf("Digite um ordem maior\n");
					break;
				}
				arvoreB = criaArvoreB(ordem);
				saida = fopen("saida.bt", "wb");
				break;
			case 3:
				if (arvoreB == NULL){
					printf("Crie uma arvore antes.\n");
					break;
				}
				printf("Digite a chave no formato 3 primeiras Letras e 5 números\n");
				scanf("%[^\n]", chave);
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
					scanf("%d", &ordem);
					criaABIndicePrimario(IP, ordem);
				}
				break;
			case 7:
				printf("Digite a chave no formato 3 primeiras Letras e 5 números\n");
				scanf("%[^\n]", chave);
				if(buscaAB(arvoreB, &seeks, chave) != NULL){
					printf("seeks = %d\n", seeks);
					seeks = 0;
				}
				else{
					printf("CHAVE NÃO EXISTE\n");
				}
				break;
			case 99:
				break;
			default:
				system("clear");
				printf("OPCAO INEXISTENTE.\n");
				break;
		}

		printf("PRECIONE ENTER\n");
		getchar();
	}
	if (arvoreB != NULL){	
		limpaArvoreB(arvoreB);;
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
	system("clear");

	printf("1. Abrir indice Primario\n");
	printf("2. Criar Arvore B vazia\n");
	printf("3. Adicionar chave\n");
	printf("4. Vizualizar em Ordem (Arvore B)\n");
	printf("5. Vizualizar indice Primario\n");
	printf("6. CRIAR arvore B a partir de indice\n");
	printf("7. Realizar busca da arvore B\n");
	printf("99. SAIR\n");

	return;
}
