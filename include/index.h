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
	#define TAM_REG 64	
	#define TAM_MATRIC 7
	#define TAM_NOME 41
	#define TAM_OP 4
	#define TAM_CURSO 9
	#define TAM_TURMA 1

	struct tableindex_P;
	typedef struct tableindex_P tabelaInd_Prim;
	typedef struct indexS_P indexS;

	int primeiroElementoIndicePrimario(tabelaInd_Prim *ind);
	int ultimoElementoIndicePrimario(tabelaInd_Prim *ind);

	tabelaInd_Prim* criaIndicePrimario(char *nomeArq);
	void imprimeIndicePrimario(tabelaInd_Prim *ind);
	void imprimeIndicePrimarioArq(tabelaInd_Prim *ind, char *nomeArq);

	void intercalaListasPrimario(char *lista1, char *lista2);
	
	void incluirRegistro(char *nomeArq, tabelaInd_Prim *IP, indexS *IS, char *registro, int OP);
	void retirarRegistro(char *nomeArq, tabelaInd_Prim *IP, indexS *IS, char *registro, int OP);
	void atualizarRegistro(char *nomeArq, tabelaInd_Prim *IP, indexS *IS, char *regi_antigo, char *regi_novo, int OP);

	indexS *criaIndiceSecundario(char *nomeArq, int OP);
	void imprimeIndiceSecundario(indexS *ind);
	void limpaIndiceSecundario(indexS *ind);

#endif
