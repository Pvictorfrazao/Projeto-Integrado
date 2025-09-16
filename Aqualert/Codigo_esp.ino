#include <WiFi.h>
#include <HTTPClient.h>
#include "Maxbotix.h"

// ===== CONFIG WIFI =====
const char* ssid = "NOME_DA_SUA_REDE";
const char* password = "SENHA_DA_SUA_REDE";

// ===== CONFIG API =====
// Troque <IP_DO_SERVIDOR> pelo IP da máquina que roda a API Flask
// Se estiver rodando no mesmo PC da rede, veja o IP com "ipconfig" (Windows)
String serverName = "http://<IP_DO_SERVIDOR>:5000/add_leitura";

// ===== CONFIG SENSORES =====
#define SENSOR1_PIN 34
#define SENSOR2_PIN 35

Maxbotix rangeSensorAD1(SENSOR1_PIN, Maxbotix::AN, Maxbotix::LV);
Maxbotix rangeSensorAD2(SENSOR2_PIN, Maxbotix::AN, Maxbotix::LV);

unsigned long tempoAtivacaoSensor1 = 0;
const int distanciaSensoresCM = 45; 
const int distanciaMin = 100; 
const int distanciaMax = 200;

void setup() {
  Serial.begin(115200);

  // Conexão WiFi
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());
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

        Serial.print("VEL: ");
        Serial.println(velocidadeKMH);

        // ===== ENVIO PARA A API =====
        if (WiFi.status() == WL_CONNECTED) {
          HTTPClient http;
          http.begin(serverName);
          http.addHeader("Content-Type", "application/json");

          // Cria JSON para enviar
          String json = "{\"velocidade\": " + String(velocidadeKMH, 2) + "}";
          
          int httpResponseCode = http.POST(json);

          if (httpResponseCode > 0) {
            Serial.print("Resposta da API: ");
            Serial.println(httpResponseCode);
            String resposta = http.getString();
            Serial.println("Corpo: " + resposta);
          } else {
            Serial.print("Erro ao enviar: ");
            Serial.println(httpResponseCode);
          }
          http.end();
        } else {
          Serial.println("WiFi desconectado!");
        }
      }
    }
  }

  delay(100);
}
