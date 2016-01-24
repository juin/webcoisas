#include <Wire.h>
#include "ADXL345.h"
#include "Adafruit_BMP085.h"


ADXL345 acel = ADXL345();
Adafruit_BMP085 bmp;

struct Dado{
  float altitude;
  char queda;  
  };

Dado dados; 
void setup() {
  Serial.begin(9600);
  acel.powerOn();
  if (!bmp.begin()) {
    Serial.println("Não foi possível encontrar um sensor BMP085 válido.Por favor, verifique.");
    while (1) {}
  }
  //detecção de queda
  //registra interrupção actions - 1 == on; 0 == off  
  acel.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);


  //Limite da queda e duração
   acel.setFreeFallThreshold(7);// //(5 - 9)
   acel.setFreeFallDuration(5); // //(20 - 70) 
}

void enviarDados(){
  
  int tam = sizeof(dados);
  char buff[tam];

  memcpy(&buff, &dados, tam);

  Serial.write("A");
  Serial.write((uint8_t*)&buff, tam);
  Serial.write("T");
}

void checkSetup()
{
  Serial.print("Free Fall Threshold = ");
  Serial.println(acel.getFreeFallThreshold());
  Serial.print("Free Fall Duration = ");
  Serial.println(acel.getFreeFallDuration());
}

void loop() {
  
  dados.altitude = bmp.readAltitude();
  dados.queda = 'N';
  
  //Leitura interrompe fonte e procurar ações desencadeadas
  byte interrupts = acel.getInterruptSource();
  if(acel.triggered(interrupts, ADXL345_FREE_FALL)){
    dados.queda = 'Q';
    Serial.println("queda");
  }

  enviarDados();
  delay(100);
}
