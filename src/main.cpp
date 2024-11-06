#include <Arduino.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Firebase_ESP_Client.h>
#include <PubSubClient.h>
#include <config.h>

#define LORA_SS 18
#define LORA_RST 14
#define LORA_DI0 26
#define BAND 915E6

const char* mqttServer = "demo.thingsboard.io";

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect();

void readData() {
  int packetSize = LoRa.parsePacket();
  String data = "";

  if(packetSize) {
    while (LoRa.available()) {
      data += (char) LoRa.read();
    }
    if(!client.connected()) {
      reconnect();
    }
    if(client.publish("v1/devices/me/telemetry", data.c_str())) {
      Serial.print("\nDados Enviados! -> " + data);
    }
  }
}

void sendDataDashboardTester() {
  srand(time(NULL));
  double mockTemperature = 25.00;
  double mockAccel = (rand() % (400 - 100+1) + 100);
  double mockAxisX = (rand() % (20 - 5+1) + 100);
  double mockAxisZ = (rand() % (350 - 100+1) + 100);
  double mockAxisY = (rand() % (1 - (-10)+1) + 100);

  String PacketTeste = "{\"nodeID\":" + String(1) +
          ",\"time\":" + String(millis()) +
          ",\"temperature\":" + String(mockTemperature) +
          ",\"accel\":" + String(mockAccel) +
          ",\"axisX\":" + String(mockAxisX) +
          ",\"axisY\":" + String(mockAxisY) +
          ",\"axisZ\":" + String(mockAxisZ) +
          "}";

  if(client.publish("v1/devices/me/telemetry", PacketTeste.c_str())) {
    Serial.print("Dados de teste Enviados");
  }

  delay(10000);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("\nBuscando broker...");
    if (client.connect(BROKER_CID, BROKER_USER, BROKER_PSWD)) {
      Serial.println(" OK!");
    } else {
      Serial.print("Falha ao reconectar: ");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  client.setServer(mqttServer, 1883);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("...");
    delay(300);
  }
  Serial.print("WiFi Conectado! ");

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
  if(!LoRa.begin(BAND)) {
    Serial.print("Erro ao incializar o Gateway");
    while(1);
  }

  while(!client.connected()) {
    Serial.print("\nBuscando MQTT... ");
    if(client.connect(BROKER_CID, BROKER_USER, BROKER_PSWD)) {
      Serial.print("Broker MQTT Conectado");
      client.subscribe("v1/devices/me/telemetry");
    } else {
      Serial.print("fail: ");
      Serial.println(client.state());
      reconnect();
    }
  }

  Serial.print("\nGateway Pronto!\n");
}

void loop() {
  readData();
}