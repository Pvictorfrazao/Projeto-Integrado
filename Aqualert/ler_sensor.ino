#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "Maxbotix.h"

// Configuração dos sensores
Maxbotix rangeSensorAD1(34, Maxbotix::AN, Maxbotix::LV);
Maxbotix rangeSensorAD2(35, Maxbotix::AN, Maxbotix::LV);

unsigned long tempoAtivacaoSensor1 = 0;
const int distanciaSensoresCM = 200;
const int distanciaMin = 17;
const int distanciaMax = 40;

// Config WiFi
const char* ssid = "Nome do WiFi";
const char* password = "Senha do WiFi";

// Endereço da API Flask (Ngrok)
String serverName = "https://semifictionalized-milagro-unmonopolized.ngrok-free.dev/add_leitura";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Conectando ao WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado!");
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
      tempoAtivacaoSensor1 = 0;

      if (intervalo > 0) {
        float velocidadeCMporMS = (float)distanciaSensoresCM / intervalo;
        float velocidadeKMH = velocidadeCMporMS * 0.036;

        // Só continua se a velocidade for maior que zero
        if (velocidadeKMH > 0.0) {
          Serial.print("VEL: ");
          Serial.println(velocidadeKMH);

          if (WiFi.status() == WL_CONNECTED) {
            WiFiClientSecure client;
            client.setInsecure();  // ignora certificado SSL (para testes)
            HTTPClient http;

            http.begin(client, serverName);
            http.addHeader("Content-Type", "application/json");

            String json = "{\"velocidade\": " + String(velocidadeKMH, 2) + "}";
            int httpResponseCode = http.POST(json);

            if (httpResponseCode > 0) {
              Serial.print("Servidor respondeu: ");
              Serial.println(httpResponseCode);
              Serial.println(http.getString());  // mostra resposta da API
            } else {
              Serial.print("Erro HTTP: ");
              Serial.println(httpResponseCode);
            }

            http.end();
          }
        }
      }
    }
  }

  delay(100);
}
