package br.edu.ifba.embarcados.webcoisas;

import javax.ws.rs.GET;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.MediaType;

import br.edu.ifba.embarcados.webcoisas.sensoriamento.LeitorSensoriamento;

@Path("servico")
public class Servico {
	
	@GET
	@Path("/id/")
	@Produces(MediaType.TEXT_PLAIN)
	public String getId() {
		return LeitorSensoriamento.getRFID() + "";
	}

	@GET
	@Path("/sensores")
	@Produces(MediaType.TEXT_PLAIN)
	public String getSensores() {
		String resultado = "";
		resultado = resultado + "\nLeitura de todos os sensores:";
		resultado = resultado + "\nVelocidade: "+LeitorSensoriamento.getVelocidade();
		resultado = resultado + "\nAltitude: "+LeitorSensoriamento.getAltitude();
		resultado = resultado + "\nMovimento: "+LeitorSensoriamento.getMovimento();
		
		return resultado;
	}

	@GET
	@Path("/sensores/movimento")
	@Produces(MediaType.TEXT_PLAIN)
	public String getMovimento() {
		return LeitorSensoriamento.getMovimento() + "";
	}

	@GET
	@Path("/sensores/velocidade")
	@Produces(MediaType.TEXT_PLAIN)
	public String getVelocidade() {
		return LeitorSensoriamento.getVelocidade() + "";
	}

	@GET
	@Path("/sensores/altitude")
	@Produces(MediaType.TEXT_PLAIN)
	public String getAltitude() {
		return LeitorSensoriamento.getAltitude() + "";
	}
	
}
