#include "Maxbotix.h"

// Defina os pinos analógicos do ESP32 usados
// (34 e 35 são bons porque são somente entrada analógica)
#define SENSOR1_PIN 34
#define SENSOR2_PIN 35

// Configuração dos sensores Maxbotix usando saídas analógicas (AN)
Maxbotix rangeSensorAD1(SENSOR1_PIN, Maxbotix::AN, Maxbotix::LV);
Maxbotix rangeSensorAD2(SENSOR2_PIN, Maxbotix::AN, Maxbotix::LV);

unsigned long tempoAtivacaoSensor1 = 0; // Tempo de ativação do primeiro sensor
const int distanciaSensoresCM = 45; // Distância entre os dois sensores em cm
const int distanciaMin = 100; // Limite inferior de distância (1 metro)
const int distanciaMax = 200; // Limite superior de distância (2 metros)

void setup() {
  Serial.begin(115200); // No ESP32 é comum usar 115200
}

void loop() {
  int range1 = rangeSensorAD1.getRange();
  int range2 = rangeSensorAD2.getRange();

  if (range1 >= distanciaMin && range1 <= distanciaMax) {
    if (tempoAtivacaoSensor1 == 0) {
      tempoAtivacaoSensor1 = millis();
    }
  }

  if (range2 >= distanciaMin && range2 <= distanciaMax) {
    if (tempoAtivacaoSensor1 != 0) {
      unsigned long tempoAtivacaoSensor2 = millis();
      unsigned long intervalo = tempoAtivacaoSensor2 - tempoAtivacaoSensor1;
      tempoAtivacaoSensor1 = 0; // Reseta o tempo de ativação do primeiro sensor

      // Calcula a velocidade em cm/ms e converte para km/h
      if (intervalo > 0) {
        float velocidadeCMporMS = (float)distanciaSensoresCM / intervalo;
        float velocidadeKMH = velocidadeCMporMS * 0.036;

        Serial.print("VEL:");
        Serial.println(velocidadeKMH); // Mudou isso para facilitar para o Python
      }
    }
  }

  delay(100); // Aguarda 100 ms antes de repetir o loop
}
