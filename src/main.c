#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"
#include "bt.h"

/*
MARCELO DE ARAUJO LOPES JUNIOR - 150016794
RAFAEL DE LIMA CHEHAB - 150045123

PROGRAMA DE TESTE, CUIDADO DURANTE A UTILIZACAO.
- CRIE OS INDICES PRIMARIOS E SECUNDARIOS ANTES DE UTILIZAR OUTROS RECURSOS
- CUIDADO COM A OPCAO FEITA NO INDICE SECUNDARIO, SEMPRE UTILIZE A MESMA, CASO CONTRARIO O INDICE SECUNDARIO PODE NAO FUNCIONAR CORRETAMENTE
- CERTIFIQUE-SE DE DIGITAR OS CAMPOS CORRETOS QUANDO SOLICITADOS
*/

int main(int agrc, char** argv){

	tabelaInd_Prim* IP = NULL;
	char nomeArq[50], nomeArq2[50];

	IP = criaIndicePrimario("lista.txt");
	imprimeIndicePrimario(IP);

	pBTree arvB = criaArvoreB(3);
	//insereAB(&arvB, getKey(IP, 0));
	//insereAB(&arvB, getKey(IP, 1));
	//insereAB(&arvB, getKey(IP, 2));
	//insereAB(&arvB, getKey(IP, 3));
	//insereAB(&arvB, getKey(IP, 4));
	//insereAB(&arvB, getKey(IP, 5));
	
	printf("0\n");
	arvB = insereAB_helper(arvB, getKey(IP, 0));
	printf("1\n");
	arvB = insereAB_helper(arvB, getKey(IP, 1));
	printf("2\n");
	arvB = insereAB_helper(arvB, getKey(IP, 2));
	printf("3\n");
	arvB = insereAB_helper(arvB, getKey(IP, 3));
	printf("4\n");
	arvB = insereAB_helper(arvB, getKey(IP, 4));
	printf("5\n");
	arvB = insereAB_helper(arvB, getKey(IP, 5));
	printf("6\n");
	// arvB = insereAB(arvB, getKey(IP, 6));
	inOrder(arvB);

	//escrita
	FILE *w = fopen("saida.bt", "wb");
	escreveABarq(w, arvB);
	fclose(w);

	printf("LEITURA:  \n");
	pBTree arv = criaArvoreB(3);
	FILE *e = fopen("saida.bt", "rb");
	leituraABarq(e, arv);
	inOrder(arv);
	fclose(e);

	int seeks;

	if(buscaAB(arv, &seeks, getKey(IP, 0)) != NULL){
		printf("seeks = %d\n", seeks);
		seeks = 0;
	}

	if(buscaAB(arv, &seeks, getKey(IP, 1)) != NULL){
		printf("seeks = %d\n", seeks);
		seeks = 0;
	}
	if(buscaAB(arv, &seeks, getKey(IP, 3)) != NULL){
		printf("seeks = %d\n", seeks);
		seeks = 0;
	}


	free(IP);
	limpaArvoreB(arvB);
	//free(arv);



	return 0;
}
