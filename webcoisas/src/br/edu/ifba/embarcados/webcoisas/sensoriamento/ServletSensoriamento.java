package br.edu.ifba.embarcados.webcoisas.sensoriamento;

import javax.servlet.ServletException;

import com.sun.jersey.spi.container.servlet.ServletContainer;
import com.sun.jersey.spi.container.servlet.WebConfig;

@SuppressWarnings("serial")
public class ServletSensoriamento extends ServletContainer {
	
	private LeitorSensoriamento leitor = null;
	private Thread tleitor = null;
	
	@Override
	protected void init(WebConfig webConfig) throws ServletException {
		System.out.println("Iniciando serviço web...");
	
		leitor = new LeitorSensoriamento();
		tleitor = new Thread(leitor);
		tleitor.start();
		super.init(webConfig);
	}

	@Override
	public void destroy() {
		System.out.println("Finalizando serviço web...");
		
		leitor.parar();
		try {
			tleitor.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		super.destroy();
	}
}