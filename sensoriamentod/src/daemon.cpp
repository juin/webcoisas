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
using namespace std;

#define ARQUIVO_LOG "~/Desenvolvimento/embarcados/logs/sensoriamento.log"
#define ARQUIVO_PIPE "~/Desenvolvimento/embarcados/logs/sensoriamentop"

bool continuar = true;
int sensores = 0;

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
	sensores++;

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

		log << "Pipe sendo criando..." << endl;
		if (!pipeIniciado()) {
			log << "Falha de inicializacao do PIPE" << endl;

			exit(1);
		}

		continuar = true;
		while (continuar) {
			int sensores = lerSensores();

			log << "Enviando sensores: " << sensores << endl;

			enviarParaPipe(sensores);

			sleep(1);
		}

		unlink(ARQUIVO_PIPE);

		log << "Daemon de sensoriamento finalizado!" << endl;
	}

	return 0;
}
