#ifndef H_INDEX
#define H_INDEX
	/*
	MODULO RESPONSAVEL POR FORNECER AS FUNÇÕES DE UTILIZACAO DE INDICES PRIMARIOS E SECUNDARIOS

	OBS: OS INDICES SAO CRIADOS ORDENADOS E PERMANECEM ORDENADOS APÓS AS OPERACOES DE INSERIR E
	RETIRAR.
	*/

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#define TAM_REG 54	
	// #define TAM_MATRIC 7
	// #define TAM_NOME 41
	// #define TAM_OP 4
	// #define TAM_CURSO 9
	// #define TAM_TURMA 1

	struct tableindex_P;
	typedef struct tableindex_P tabelaInd_Prim;
	int primeiroElementoIndicePrimario(tabelaInd_Prim *ind);
	int ultimoElementoIndicePrimario(tabelaInd_Prim *ind);

	tabelaInd_Prim* criaIndicePrimario(char *nomeArq);
	void imprimeIndicePrimario(tabelaInd_Prim *ind);
	void imprimeIndicePrimarioArq(tabelaInd_Prim *ind, char *nomeArq);
	char* getKey(tabelaInd_Prim *ind, int index);

#endif
