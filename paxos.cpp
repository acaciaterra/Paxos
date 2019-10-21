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
#define propose 4

typedef struct{
    int id; //cada nodo tem um id
	int idr; //id real do nodo
	std:: vector<int> papel; //cada processo pode assumir um ou mais papeis
}tnodo;

// int N, token;
std:: vector< std:: pair<int, int>> propostas;
std:: vector<tnodo> nodo;
std:: vector<std:: pair<int, int>> maiornumerorecebido;
// blablabla
std:: vector< std:: pair<int, int>> respostas;
int enviados = 0;

// o processo proposer recebe a resposta do acceptor contendo o numero da maior proposta
// recebida e tambem o valor (se for 0, é NULL, significa que ainda nao aceitou a proposta)
void send_prepareresponse(int acceptor, int proposer, int nproposta, int vproposta){
    printf("O processo %d recebeu do processo %d o prepare response contendo o numero %d e o valor %d\n",
    proposer, acceptor, nproposta, vproposta);
    // aqui o proposer comeca a execucao da fase 2 do algoritmo
    // o proposer deve verificar se recebeu a resposta de uma maioria de acceptors
    printf("\n\t>>>>>>>>>>>> enviados: %d recebidos: %d\n", respostas[nproposta - 1].first, respostas[0].second);
}

// essa funcao representa o processo acceptor na fase 1 do algoritmo
void send_preparerequest(int sender, int receiver, int numeroproposta){
	//sender = proposer e receiver = acceptor
    printf("O processo %d recebeu a proposta de numero %d do processo %d\n", receiver, numeroproposta, sender);
    // se a proposta recebida tem numero maior do que a maior ja recebida anteriormente,
    // entao responde ao prepare request com um prepare response contendo o numero n da maior
    // proposta recebida e a promessa de nao aceitar nenhuma proposta com numero menor
    if(numeroproposta > maiornumerorecebido[receiver].first){
        maiornumerorecebido[receiver].first = numeroproposta;
        // responsesrecebidas[sender]++;
        respostas[numeroproposta - 1].second++;
        send_prepareresponse(receiver, sender, maiornumerorecebido[receiver].first, maiornumerorecebido[receiver].second != 0);
    }
}

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
    maiornumerorecebido.resize(N);
    // responsesrecebidas.resize(N);
    propostas.push_back(std::make_pair(1, 5));

    // printf(">%d >%d >%d\n", maiornumerorecebido[0], maiornumerorecebido[1], maiornumerorecebido[2]);
    // printf(">>> %d\n", propostas[0].first);

    for (int i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        // printf(fa_name, "%d", i);
        nodo[i].id = facility(fa_name, 1);
        nodo[i].idr = i;
        nodo[i].papel.resize(1);
        nodo[i].papel[0] = 2;
    }

    print_init();

    for (int i = 0; i < N; i++)
        schedule(test, 30.0, i);

    schedule(fault, 140.0, 4);
    schedule(propose, 100.0, 2);

    while(time() < 150.0) {
   	 	cause(&event, &token); //causa o proximo evento
   	 	switch(event) {
            case test:
	 		if(status(nodo[token].id) != 0){
				// nodofalho++;  //contabiliza nodos FALHOS
				break; //se o nodo for falho, então quebra
			}else{
				// nodosemfalha++;  //contabiliza nodos SEM-FALHA
			}

            schedule(test, 30.0, token);
			break;

			case fault:
			r = request(nodo[token].id, token, 0);
	 		if(r != 0) {
	 			puts("Impossível falhar nodo");
	 			exit(1);
	 		}
				printf("EVENTO: O nodo %d falhou no tempo %5.1f\n", token, time());

			break;

	 		case repair:
	 		release(nodo[token].id, token);
	 		printf("EVENTO: O nodo %d recuperou no tempo %5.1f\n", token, time());

			schedule(test, 30.0, token);
			break;

            case propose:
            // aqui o nodo token (proposer coordenador) precisa saber quais processos sao acceptors,
            // para enviar o prepare request para uma maioria de acceptors
            // precisa tambem selecionar uma proposta N, para isso sera mantido um vetor de propostas

            respostas.push_back(std:: make_pair(0,0));
            //envia para os processos acceptors o numero da proposta (first no pair)
            for(int i = 0; i < N; i ++){
                if(nodo[i].papel[0] == 2 && status(nodo[i].id) == 0){
                    enviados++;
                    respostas[0].first++;
                    send_preparerequest(token, i, propostas[0].first);
                }
            }
            break;
	 	}
	}

	return 0;
}
