package br.edu.ifba.embarcados.webcoisas.sensoriamento;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.RandomAccessFile;

public class LeitorSensoriamento implements Runnable {

	private static final String ARQUIVO_PIPE = "/home/junior/Desenvolvimento/embarcados/logs/sensoriamentop";

	private RandomAccessFile pipe = null;

	private boolean continuar = true;

	private static Integer sensores = 0;

	private static final int DESLOCAMENTO_RFID = 26;
	private static final int DESLOCAMENTO_MOVMT = 18;
	private static final int DESLOCAMENTO_VELOC = 10;

	public static int getRFID() {
		int id = getSensores();

		id = id >> DESLOCAMENTO_RFID;

		return id;
	}

	public static int getMovimento() {
		int mov = getSensores();

		mov = (mov & 67108863) >> DESLOCAMENTO_MOVMT;

		return mov;
	}

	public static int getVelocidade() {
		int vel = getSensores();

		vel = (vel & 262143) >> DESLOCAMENTO_VELOC;

		return vel;
	}

	public static int getAltitude() {
		int altitude = getSensores();

		altitude = (altitude & 1023);

		return altitude;
	}

	public LeitorSensoriamento() {
		try {
			pipe = new RandomAccessFile(ARQUIVO_PIPE, "r");
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void run() {

		continuar = true;
		while (continuar) {
			String s = "";
			try {
				if (((s = pipe.readLine()) != null) && !s.equals("")) {
					synchronized (sensores) {
						sensores = Integer.parseInt(s);
					}

					Thread.sleep(1000);
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	public void parar() {
		continuar = false;
	}

	public static Integer getSensores() {
		synchronized (sensores) {
			return sensores;
		}
	}
}