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
#define etapa1 5
#define etapa2 6
#define etapa3 7
#define etapa4 8

typedef struct{
    int id; //cada nodo tem um id
	int idr; //id real do nodo
	std:: vector<int> papel; //cada processo pode assumir um ou mais papeis
    int reg; // se o 'registrador' for 0, significa que esse processo ainda nao decidiu
}tnodo;

// int N, token;
std:: vector< std:: pair<int, int>> propostas;
std:: vector<tnodo> nodo;
std:: vector<std:: pair<int, int>> maiornumerorecebido;
// blablabla
std:: vector< std:: vector <std:: pair<int, int>>> respostas;
std:: vector< std::vector <int>> quemrespondeu;
std:: vector<int> numeros;
std:: vector<int> passoupela3;
int enviados = 0;
int npropostaatual = 0;

// o processo proposer recebe a resposta do acceptor contendo o numero da maior proposta
// recebida e tambem o valor (se for 0, é NULL, significa que ainda nao aceitou a proposta)
void send_prepareresponse(int acceptor, int proposer, int nproposta, int vproposta, int pos){
    printf("O processo %d recebeu do processo %d o prepare response (numero %d, valor %d)\n",
    proposer, acceptor, nproposta, vproposta);
    // aqui o proposer comeca a execucao da fase 2 do algoritmo
    // o proposer deve verificar se recebeu a resposta de uma maioria de acceptors
    printf("\n----------------------------------------\n");
    printf("Proposta numero: %d\n", nproposta);
    printf("prepare request enviados: %d\n", respostas[proposer][pos].first);
    printf("prepare response recebidos: %d\n", respostas[proposer][pos].second);
    printf("----------------------------------------\n\n");
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
    }
}

// parametros: quem envia a mensagem, quem recebe, o numero da proposta e o valor dela
void send_acceptrequest(int sender, int receiver, int num, int val){
    printf("O processo %d enviou um accept request para o processo %d (numero %d,valor %d)\n", sender, receiver, num, val);
}

void print_init(){
	printf ("===========================================================\n");
	printf ("         Execucao de uma instancia do Paxos\n");
	printf ("         Aluna: Acacia dos Campos da Terra\n");
	printf ("         Orientador: Elias P. Duarte Jr.\n");
	printf ("===========================================================\n\n");
}

int main(int argc, char const *argv[]) {
    static int N, token, event, i, r, pos;
    static char fa_name[5]; //facility representa o objeto simulado

    char evento[7];
    int processo;
    float tempo;

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
    passoupela3.resize(50);
    // responsesrecebidas.resize(N);
    //aqui foram inseridas 50 propostas de valores aleatorios
    //os numeros de propostas sao definidos de acordo com o processo
    for(int i = 0; i < 50; i++){
        propostas.push_back(std::make_pair(i, rand()%200));
    }
    quemrespondeu.resize(N);
    numeros.resize(N);
    respostas.resize(N);

    std::fill(numeros.begin(), numeros.end(), -1);

    // printf(">%d >%d >%d\n", maiornumerorecebido[0], maiornumerorecebido[1], maiornumerorecebido[2]);
    // printf(">>> %d\n", propostas[0].first);

    for (int i = 0; i < N; i++) {
        memset(fa_name, '\0', 5);
        // printf(fa_name, "%d", i);
        nodo[i].id = facility(fa_name, 1);
        nodo[i].idr = i;
        nodo[i].papel.resize(1);
        nodo[i].papel[0] = 2;
        nodo[i].reg = 0;
    }

    // nodo[2].papel[0] = 1;
    print_init();
    printf("Numero de nodos no sistema: %d\n", N);

    for (int i = 0; i < N; i++)
        schedule(test, 30.0, i);

    tp = fopen(argv[1], "r");
    fscanf(tp, "%d\n", &N);
    while(!feof(tp)){
    	fscanf(tp, "%s %f %d\n", evento, &tempo, &processo);
    	// printf("%s %f %d\n", evento, tempo, processo);
    	schedule((strcmp("propose", evento) == 0 ? propose : test), tempo, processo);
    	//escalona os eventos. Faz a verificação de string pois o schedule não aceita string como parâmetro
    }
    fclose(tp);

    // schedule(fault, 70.0, 4);
    // schedule(propose, 100.0, 2);

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

            //armazena quantas vezes esse processo ja fez propose
            //para calcular qual o numero do propose (nunca é o mesmo)
            numeros[token]++;

            printf("[EVENTO] O processo %d esta fazendo o propose! (tempo %5.1f)\n\n", token, time());
            respostas[token].push_back(std:: make_pair(0,0));
            pos = respostas[token].size()-1;
            schedule(etapa1, 5.0, token);
            break;

            case etapa1:
            //apaga todas as respostas anteriores, para caso receba uma nova proposta
            quemrespondeu[token].erase(quemrespondeu[token].begin(), quemrespondeu[token].end());
            //envia para os processos acceptors o numero da proposta (first no pair)
            for(int i = 0; i < N; i ++){
                if(nodo[i].papel[0] == 2 && status(nodo[i].id) == 0){
                    enviados++;
                    respostas[token][pos].first++;
                    send_preparerequest(token, i, propostas[token + N * numeros[token]].first);
                }
            }
            schedule(etapa2, 5.0, token);
            break;

            case etapa2:
            for(int i = 0; i < N; i ++){
                if(nodo[i].papel[0] == 2 && status(nodo[i].id) == 0){
                    if(propostas[token + N * numeros[token]].first >= maiornumerorecebido[i].first){
                        maiornumerorecebido[i].first = propostas[token + N * numeros[token]].first;
                        // printf(">>>>>>>>>>>>>>>>>>> %d e %d\n", propostas[token + N * numeros[token]].first, propostas[token + N * numeros[token]].second);
                        // responsesrecebidas[sender]++;
                        respostas[token][pos].second++;
                        // aqui: quem respondeu a mensagem do token foi o processo i
                        quemrespondeu[token].push_back(i);
                         npropostaatual = maiornumerorecebido[i].first;
                        send_prepareresponse(i, token, maiornumerorecebido[i].first, maiornumerorecebido[i].second, pos);
                        // send_prepareresponse(receiver, sender, maiornumerorecebido[receiver].first, maiornumerorecebido[receiver].second);
                    }
                }
            }

            schedule(etapa3, 15.0, token);
            break;

            case etapa3:
            // verifica se recebeu a maioria das respostas para aquela proposta
            // no caso, a proposta 1, porque esse trabalho esta ficando simples demais
            // printf("maiornumerorecebido: %d >>>>>> token + N * numeros[token]: %d\n", maiornumerorecebido[token].first, (token + N * numeros[token]));
            if(respostas[token][pos].second >= (respostas[token][pos].first / 2) && maiornumerorecebido[token].first == token + N * numeros[token]){
                passoupela3[token + N * numeros[token]] = 1;
                printf(">>> Recebeu resposta da maioria!!\n");
                // agora o proposer envia o accept request para os processos dos quais recebeu a resposta
                for(int i = 0; i < quemrespondeu[token].size(); i++){
                    // parametros: quem envia a mensagem, quem recebe, o numero da proposta e o valor dela
                    if(status(nodo[quemrespondeu[token][i]].id) == 0 && nodo[quemrespondeu[token][i]].reg == 0){
                        send_acceptrequest(token, quemrespondeu[token][i], propostas[token + N * numeros[token]].first, propostas[token + N * numeros[token]].second);
                        break;
                    }
                }
            }

            schedule(etapa4, 5.0, token);
            break;

            case etapa4:
            if(respostas[token][pos].second >= (respostas[token][pos].first / 2) && maiornumerorecebido[token].first == token + N * numeros[token] && passoupela3[token + N * numeros[token]] == 1){
                for(int i = 0; i < quemrespondeu[token].size(); i++){
                    if(status(nodo[quemrespondeu[token][i]].id) == 0 && nodo[quemrespondeu[token][i]].reg == 0){
                        nodo[quemrespondeu[token][i]].reg = propostas[token + N * numeros[token]].second;
                        printf("processo %d decidiu pelo valor %d\n", quemrespondeu[token][i], propostas[token + N * numeros[token]].second);
                        printf("O processo %d atualizou seu proprio registrador com o valor %d\n", nodo[quemrespondeu[token][i]].idr, propostas[token + N * numeros[token]].second);
                        for(int i = 0; i < N; i++){
                            maiornumerorecebido[i].second = propostas[token + N * numeros[token]].second;
                            if(status(nodo[i].id) == 0 && nodo[i].reg == 0){
                                nodo[i].reg = propostas[token + N * numeros[token]].second;
                                printf("O processo %d aprendeu o valor %d e atualizou seu registrador\n", nodo[i].idr, propostas[token + N * numeros[token]].second);
                            }
                        }
                    }
                }
            }

            break;
	 	}
	}

	return 0;
}
