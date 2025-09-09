#include "Maxbotix.h"

// Configuração dos sensores Maxbotix usando saídas analógicas (AD)
Maxbotix rangeSensorAD1(A0, Maxbotix::AN, Maxbotix::LV);
Maxbotix rangeSensorAD2(A1, Maxbotix::AN, Maxbotix::LV);

unsigned long tempoAtivacaoSensor1 = 0; // Tempo de ativação do primeiro sensor
const int distanciaSensoresCM = 45; // Distância entre os dois sensores em cm
const int distanciaMin = 100; // Limite inferior de distância (1 metro)
const int distanciaMax = 200; // Limite superior de distância (2 metros)

void setup() {
  Serial.begin(9600); // Inicializa a comunicação serial a 9600 bps
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
        Serial.println(velocidadeKMH); //#Mudou isso, para facilitar para o python e também evitar divisão por 0
        }


    }
  }

  delay(100); // Aguarda 100 milissegundos antes de repetir o loop
}