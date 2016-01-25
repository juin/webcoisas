#include <Wire.h>
#include "RCSwitch.h"
#include "ADXL345.h"
#include "Adafruit_BMP085.h"

#define MIDDLEWARE
//instâncias do emissor e do receptor
RCSwitch emissor  = RCSwitch();
RCSwitch receptor = RCSwitch();

//instância do acelerômetro
ADXL345 acel = ADXL345();
Adafruit_BMP085 bmp;

#define RFID_PIN_TRANSMISSOR 4
#define RFID_PIN_RECEPTOR 0
#define RFID_LIMITE_INFERIOR 10
#define RFID_LIMITE_SUPERIOR 20

#define DESLOCAMENTO_RFID  26
#define DESLOCAMENTO_MOVMT 18
#define DESLOCAMENTO_VELOC 10
//altitude

#define PINO_VELOCIDADE  0

//Constante que representa o pino onde o positivo
//do buzzer será ligado.
const int buzzer = 6;

#ifdef MIDDLEWARE
struct InfoRF {
  int id;
  int movimento;
  int velocidade;
  int altitude;
} infoRF;
#else
  long infoRF;
#endif

void setup() {
  Serial.begin(9600);

  if (!bmp.begin()) {
    Serial.println("Não foi possível encontrar um sensor BMP085 válido.Por favor, verifique.");
    //while (1) {}
  }

  //Definindo o pino buzzer como de saída.
  pinMode(buzzer, OUTPUT);

  //Configuração do emissor receptor RF
  emissor.enableTransmit(RFID_PIN_TRANSMISSOR);
  receptor.enableReceive(0);

  //Configuração do acelerômetro para detecção de movimento
  //Ligar o acelerômetro
  //quando detecta o movimento é o activity nos 3 eixos
  acel.powerOn();
  acel.setInactivityX(1);
  acel.setInactivityY(1);
  acel.setInactivityZ(1);

  acel.setActivityX(1);
  acel.setActivityY(1);
  acel.setActivityZ(1);

  //A detecção da inatividade ou atividade tem um limite
  //valor muito alto torna o sensor insensível threshold
  acel.setActivityThreshold(50);
  acel.setInactivityThreshold(50);
  //notificar uma situação de inatividade
  acel.setTimeInactivity(10);//estava 10

  //Acionamento da detecção da atividade e inatividade
  acel.setInterruptMapping(ADXL345_INT_ACTIVITY_BIT, ADXL345_INT1_PIN);
  acel.setInterruptMapping(ADXL345_INT_INACTIVITY_BIT, ADXL345_INT1_PIN);

  acel.setInterrupt(ADXL345_INT_ACTIVITY_BIT, 1);
  acel.setInterrupt(ADXL345_INT_INACTIVITY_BIT, 1);

}
//tirou os parâmetros pq já vai pegar direto dos sensores
long lerSensoresRF() {

  //no anologRead vai passar o pino do potenciômetro
  long velocidade = analogRead(PINO_VELOCIDADE);
  long altitude = bmp.readAltitude();
  long movimento = 0;


  velocidade = map(velocidade, 0, 1023, 0, 100);
  //altitude = map(altitude, 0, 1023, 0, 40);

  byte interrupAcel = acel.getInterruptSource();

  if (acel.triggered(interrupAcel, ADXL345_INT_ACTIVITY_BIT)) {
    movimento = 1;
  }

  long id = RFID_LIMITE_INFERIOR;
  long info = id << DESLOCAMENTO_RFID;
  info = info | (movimento << DESLOCAMENTO_MOVMT);
  info = info | (velocidade << DESLOCAMENTO_VELOC);
  info = info | altitude;

  return info;
}

int RFIDValido(long rf) {
  int valido = 0;

#ifdef MIDDLEWARE
  infoRF.id = rf >> DESLOCAMENTO_RFID;
  if ((infoRF.id >= RFID_LIMITE_INFERIOR)
  && (infoRF.id <= RFID_LIMITE_SUPERIOR)) {
    valido = 1;
  }
#else
  int id = rf >> DESLOCAMENTO_RFID;
  if ((id >= RFID_LIMITE_INFERIOR)
  && (id <= RFID_LIMITE_SUPERIOR)) {
    valido = 1;
  }
#endif
  return valido;
}

void enviarParaUSB() {
  //int tam = sizeof(dados);
#ifdef MIDDLEWARE
  char buff[sizeof(InfoRF)] = {0};

  //char buff[tam];

  memcpy(&buff, &infoRF, sizeof(InfoRF));

  Serial.write("I");
  Serial.write((uint8_t*)&buff, sizeof(InfoRF));
  Serial.write("T");
#else
  Serial.write("I");
  Serial.write((uint8_t*)&infoRF, sizeof(infoRF));
  Serial.write("T");
#endif
}
void emitir(long info) {
  emissor.send(info, 32);
}

long receber() {
  long info = -1;

  if (receptor.available()) {
    info = receptor.getReceivedValue();

    receptor.resetAvailable();
  }

  return info;
}
#ifdef MIDDLEWARE
int extrairMovimento(long info) {
  int movimento = (info & 67108863) >> DESLOCAMENTO_MOVMT;

  return (movimento == 1);
}

int extrairVelocidade(long info) {
  int velocidade = (info & 262143) >> DESLOCAMENTO_VELOC;

  return velocidade;
}

int extrairAltitude(long info) {
  int altitude = (info & 1023);

  return altitude;
}
#endif
void ativaSom(){
  tone(buzzer,1500);
  }

void desativaSom(){
  noTone(buzzer);
  }
  
void loop() {

  // EMISSAO DE DADOS
  long info = lerSensoresRF();
  emitir(info);


  delay(1000);

  // RECEPCAO DE DADOS
  info = receber();

  if (info != -1) {
    if (RFIDValido(info)) {
#ifdef MIDDLEWARE
      //ativaSom();
      infoRF.movimento = extrairMovimento(info);
      infoRF.velocidade = extrairVelocidade(info);
      infoRF.altitude = extrairAltitude(info);
#else
      infoRF = info;
#endif
      enviarParaUSB();

      /*int velocidade = extrairVelocidade(info);
      Serial.print("O animal estah se movimento na velocidade de ");
      Serial.print(velocidade);
      Serial.println(" Km/h");

      int altitude = extrairAltitude(info);
      Serial.print("A altitude do animal eh ");
      Serial.print(altitude);
      Serial.println(" metros");

      int movimento = extrairMovimento(info);
      Serial.print("O animal movimentou ");
      Serial.println(movimento);*/

    }
  }else{
    //desativaSom();
    }
}


