/*
 * Comunicacao.cpp
 *
 *  Created on: 8 de out de 2015
 *      Author: Cremildo Lima
 */


#include "../includes/comunicacao.h"


#ifdef __linux__
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#endif

Comunicacao::Comunicacao(char* porta){
	hPorta = 0;
	this->porta = porta;
}

int Comunicacao::iniciar(){
	int resultado = EXIT_SUCCESS;
#if _WIN32 || _WIN64
	//MANDA A STRING DA PORTA, diz q faz leitura e escrita e só abre se já existir
		hPorta = CreateFile(porta, GENERIC_READ | GENERIC_WRITE,
				0, NULL, OPEN_EXISTING, 0, NULL);
		if (hPorta == INVALID_HANDLE_VALUE){
			resultado = GetLastError();//retorna o erro do windows
		}else{
			//mesma leitura do arduino no delay
			DCB dcb;
			//zerar todos os bytes da dcb, porta serial
			memset(&dcb, 0, sizeof(dcb));
			dcb.DCBlength = sizeof(dcb);
			dcb.BaudRate = CBR_9600;
			dcb.Parity = NOPARITY; //N�O VAI TRATAR DE PARIDADE
			dcb.StopBits = ONESTOPBIT;
			dcb.ByteSize = 8;  //cada byte vai ter tantos bits

			//at� aqui s� fez a leitura, agora precisa injetar na porta
			if (!SetCommState(hPorta, &dcb)){
				resultado = GetLastError();
			}




		}
#endif

#ifdef __linux__
	hPorta = open(porta, O_RDWR | O_NOCTTY | O_NDELAY);
	if(hPorta == -1){
		resultado = errno;
	}else{
		fcntl(hPorta, F_SETFL, 0);
	}
#endif

	Sleep(1000);//estava 2000

	return resultado;
}

int Comunicacao::ler(char* buffer, long unsigned int bytesParaLer){

	int resultado = EXIT_FAILURE;

	long unsigned int bytesLidos = 0;

#if _WIN32 || _WIN64
	ReadFile(hPorta, buffer, bytesParaLer, &bytesLidos, NULL);
#endif

#ifdef __linux__
	bytesLidos = read(hPorta, (void*)buffer, bytesParaLer);
#endif

	if(bytesLidos == bytesParaLer){
		resultado = EXIT_SUCCESS;
	}
	return resultado;
}

int Comunicacao::finalizar(){

#if _WIN32 || _WIN64
		CloseHandle(hPorta);
#endif

#ifdef __linux__
	close(hPorta);
#endif

	return EXIT_SUCCESS;

}
