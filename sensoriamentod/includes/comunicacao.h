/*
 * comunicacao.h
 *
 * Prover uma classe cujos metódos possibilitem
 * a comunicação com um dispositivo controlador
 * que realiza a leitura de sensores de 10 eixos.
 *
 *  Created on: 8 de out de 2015
 *      Author: Cremildo Lima
 */

#ifndef COMUNICACAO_H_
#define COMUNICACAO_H_

#include <stdlib.h>
#if _WIN32 || _WIN64
#include <windows.h>
#endif

#ifdef __linux__
#include <unistd.h>
#define Sleep(x) usleep(x * 1000);
#endif

class Comunicacao {
private:
	char* porta;

#ifdef __linux__
	int hPorta;
#endif
#if _WIN32 || _WIN64
	HANDLE hPorta;
#endif

public:
	Comunicacao(char* porta);

	//iniciar comunicação com a porta serial
	int iniciar();

	//realiza a leitura de um buffer a partir da porta serial
	int ler(char* buffer, long unsigned int bytesParaLer);

	//finaliza o uso da porta serial
	int finalizar();

};



#endif /* COMUNICACAO_H_ */
