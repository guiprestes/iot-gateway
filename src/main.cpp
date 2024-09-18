#include <Arduino.h>
#include <LoRa.h>

#define LORA_SS 18
#define LORA_RST 14
#define LORA_DI0 26

#define BAND 915E6

void readData() {
  int packetSize = LoRa.parsePacket();
  String data = "";

  if(packetSize) {
      Serial.println("Recebendo dados...");
      while(LoRa.available()) {
        //data += (char)LoRa.read();
        Serial.print((char)LoRa.read());
      }
      Serial.println("Dados recebidos!");
  }

//  Serial.println("Dados: ");
//  Serial.println(String(data));
}

void setup() {
  Serial.begin(115200);

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DI0);
  if(!LoRa.begin(BAND)) {
    Serial.println("Erro ao incializar o Gateway");
    while(1);
  }
  Serial.println("Gateway Pronto!");
}

void loop() {
  readData();
}