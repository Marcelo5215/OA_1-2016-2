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

	pBTree arvB = criaABIndicePrimario(IP, 6);

	inOrder(arvB);

	free(IP);
	limpaArvoreB(arvB);



	return 0;
}
