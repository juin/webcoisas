/*
 * daemon.cpp
 *
 * Leitor de sensores que executa em background
 * no LINUX. Pode ser iniciado e parada a partir
 * da linha de comando.
 *
 *  Created on:24 de jan de 2016
 *      Author: Cremildo e Manuela
 */

#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
#include <string.h>
#include <signal.h>
#include "../includes/comunicacao.h"

using namespace std;

#define ARQUIVO_LOG "~/Desenvolvimento/embarcados/logs/sensoriamento.log"
#define ARQUIVO_PIPE "~/Desenvolvimento/embarcados/logs/sensoriamentop"
#define PORTA "/dev/ttyACM0"

bool continuar = true;
int sensores = 0;

Comunicacao com = NULL;

int daemonize() {
	int resultado = EXIT_SUCCESS;

	pid_t daemon_id = 0;
	pid_t sid = 0;

	daemon_id = fork();
	if (daemon_id < 0) {
		cout << "Falha de criacao do daemon!" << endl;

		resultado = EXIT_FAILURE;
	} else {
		if (daemon_id > 0) {
			cout << "Daemon criado! Saindo do processo principal" << endl;

			exit(0);
		}

		umask(0);

		sid = setsid();
		if (sid < 0) {
			exit(1);
		}

		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}

	cout << "Resultado = " << endl;
	return resultado;
}

bool pipeIniciado() {
	int resultado = mkfifo(ARQUIVO_PIPE, 666);

	return (resultado != -1);
}

int lerSensores() {
	char ai = 0, at = 0;
	int sensores = -1;

	//realizar a leitura do caracter "A" (Inicial)
	int resultado;

	resultado = com.ler((char*) &ai, sizeof(ai));



		if ((resultado == EXIT_SUCCESS) && (ai == 'I')) {
			//se a leitura de 'A' correr bem ler a altitude
			resultado = com.ler((char*) &sensores, sizeof(sensores));
			if (resultado == EXIT_SUCCESS) {
				int is = 0;
				//se a leitura da altitude correr bem
				resultado = com.ler((char*) &at, sizeof(at));
				if (resultado == EXIT_SUCCESS && (at == 'T')) {
					/*ut << "id = " << info.id << endl;
					cout << "velocidade = " << info.velocidade << endl;
					cout << "altitude = " << info.altitude << endl;
					cout << "movimento = " << info.movimento << endl;*/
					sensores = is;
				}
			}
		}

	return sensores;
}

int enviarParaPipe(int sensores) {
	int bytes = 0;

	int apipe = open(ARQUIVO_PIPE, O_RDWR);
	if (apipe != -1) {
		stringstream ss;
		ss << sensores;
		string strSensores = ss.str();

		bytes = write(apipe, strSensores.c_str(), strSensores.length());
	}
	close(apipe);

	return bytes;
}

void tratarSinal(int sinal) {
	switch (sinal) {
	case SIGHUP:
		continuar = false;
		break;
	case SIGTERM:
		continuar = false;
		break;
	case SIGQUIT:
		continuar = false;
		break;
	default:
		break;
	}
}

void iniciarTratamentoSinais() {
	signal(SIGHUP, tratarSinal);
	signal(SIGTERM, tratarSinal);
	signal(SIGINT, tratarSinal);
	signal(SIGQUIT, tratarSinal);
}

int main(int argc, char* argv[]) {

	if (daemonize() == EXIT_SUCCESS) {
		iniciarTratamentoSinais();

		ofstream log(ARQUIVO_LOG, ios_base::out | ios_base::app);
		log << "Arquivo de log iniciado!" << endl;

		//Inicia comunicação com o arduino
			com = Comunicacao(PORTA);
			if (com.iniciar() !=0){
				log << "Falha de inicializacao de comunicação dos sensores" << endl;
				exit(1);
			}

		log << "Pipe sendo criando..." << endl;
		if (!pipeIniciado()) {
			log << "Falha de inicializacao do PIPE" << endl;

			exit(2);
		}



		continuar = true;
		while (continuar) {
			int sensores = lerSensores();

			log << "Enviando sensores: " << sensores << endl;

			enviarParaPipe(sensores);

			sleep(1);
		}

		unlink(ARQUIVO_PIPE);
		//finaliza a comunicação com o arduino
		com.finalizar();


		log << "Daemon de sensoriamento finalizado!" << endl;
	}

	return 0;
}
