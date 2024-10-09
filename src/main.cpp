#include <Arduino.h>
#include <LoRa.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <HTTPClient.h>
#include <config.h>

#define LORA_SS 18
#define LORA_RST 14
#define LORA_DI0 26

#define BAND 915E6

FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

void readData() {
  int packetSize = LoRa.parsePacket();
  String data = "";
  String documentPath = "/sensor-data/accel";
  FirebaseJson content;
  FirebaseJson contentDatabase;

  if(packetSize) {
    while (LoRa.available()) {
      data += (char) LoRa.read();
    }

    Serial.print(data);
    content.set("data", data);
    if (Firebase.RTDB.setJSON(&firebaseData, "/sensor/data", &content)) {
      Serial.print("\nDados Enviados\n");
    } else {
      Serial.println("Erro ao enviar dados: " + firebaseData.errorReason());
    }
  }
}

void setup() {
  Serial.begin(115200);

  firebaseConfig.database_url = FIREBASE_HOST;
  firebaseConfig.api_key = FIREBASE_AUTH;

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("...");
    delay(300);
  }
  Serial.println("WiFi Conectado!");

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
  if(!LoRa.begin(BAND)) {
    Serial.println("Erro ao incializar o Gateway");
    while(1);
  }

  if (Firebase.signUp(&firebaseConfig, &firebaseAuth, "", "")) {
    Serial.println("Acesso a base de dados concedido com sucesso!");
  }
  else {
    Serial.printf("%s\n", firebaseConfig.signer.signupError.message.c_str());
  }
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  Serial.println("Gateway Pronto!");
}

void loop() {
  readData();
}