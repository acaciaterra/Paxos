/*
=============================================================================
Codigo fonte: implementacao do Paxos
Aluna: Acacia dos Campos da Terra
Orientador: Prof. Elias P. Duarte Jr.
=============================================================================
*/

/*--------bibliotecas--------*/
#include <stdio.h>
#include <stdlib.h>
#include "smpl.h"
#include <set>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>


/*--------eventos--------*/
#define test 1
#define fault 2
#define repair 3

typedef struct{
    int id; //cada nodo tem um id
	int idr; //id real!!!
	std:: vector<int> papel; //cada processo pode assumir um ou mais papeis
}tnodo;

// int N, token;
std:: vector<tnodo> nodo;

void print_init(){
	printf ("===========================================================\n");
	printf ("         Execucao de uma instancia do Paxos\n");
	printf ("         Aluna: Acacia dos Campos da Terra\n");
	printf ("         Orientador: Elias P. Duarte Jr.\n");
	printf ("===========================================================\n\n");
}

int main(int argc, char const *argv[]) {
    static int N, token, event, i, r;
    static char fa_name[5]; //facility representa o objeto simulado

    if(argc != 2){
    		puts("Uso correto: ./paxos <arquivo>");
    		exit(1);
    }

    //faz a leitura do numero de nodos
	FILE *tp = fopen(argv[1], "r");
	if (tp != NULL)
		fscanf(tp, "%d\n", &N);
    else
		printf("Erro ao ler arquivo\n");
	fclose(tp);

    smpl(0, "programa paxos");
	reset();
	stream(1);

    nodo.resize(N);

    for (int i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        // printf(fa_name, "%d", i);
        nodo[i].id = facility(fa_name, 1);
        nodo[i].idr = i;
        nodo[i].papel.resize(1);
    }

    print_init();

    for (int i = 0; i < N; i++)
        schedule(test, 30.0, i);

    while(time() < 250.0) {
   	 	cause(&event, &token); //causa o proximo evento
   	 	switch(event) {
            case test:
	 		if(status(nodo[token].id) != 0){
				// nodofalho++;  //contabiliza nodos FALHOS
				break; //se o nodo for falho, então quebra
			}else{
				// nodosemfalha++;  //contabiliza nodos SEM-FALHA
			}
			break;

			case fault:
			r = request(nodo[token].id, token, 0);
	 		if(r != 0) {
	 			puts("Impossível falhar nodo");
	 			exit(1);
	 		}
				printf("EVENTO: O nodo %d falhou no tempo %5.1f\n", token, time());

//-----------------------a cada evento, recalcula o vetor tests----------------------------------
			break;

	 		case repair:
	 		release(nodo[token].id, token);
	 		printf("EVENTO: O nodo %d recuperou no tempo %5.1f\n", token, time());


			schedule(test, 30.0, token);
			break;
	 	}
	}

	return 0;
}
