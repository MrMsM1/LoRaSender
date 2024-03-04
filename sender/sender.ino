#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// Defining the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

int counter = 0;
bool senderState = true; // Assume it's initially ON
String comment;
const char* ssid = "Hello";
const char* password = "12345678";
const char* serverAddress = "http://192.168.8.187:30001";

const char *mqtt_broker = "192.168.235.221";
const char *topic = "senderState";
const char *topic0 = "Connected";
//const char *mqtt_username = "emqx";
//const char *mqtt_password = "public";
const int mqtt_port = 1883;

//WiFiClient espClient;
//PubSubClient client(espClient);

void setup() {
  // Initializing Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  // Setting LoRa transceiver module
  LoRa.setPins(ss, rst, dio0);
  LoRa.setSpreadingFactor(7);
  LoRa.setCodingRate4(5);
  LoRa.setSignalBandwidth(62.5E3);

  // Setting the LoRa.begin(---E-) argument with our location's frequency 
  while (!LoRa.begin(433E6)) {
    Serial.println(".");
    delay(500);
  }

  // Setting sync word (0xF3) to match the receiver
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");

//  // Connect to Wi-Fi
//  WiFi.begin(ssid, password);
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(1000);
//    Serial.println("Connecting to WiFi...");
//  }
//  Serial.println("Connected to WiFi");

//  //connecting to a mqtt broker
//  client.setServer(mqtt_broker, mqtt_port);
//  client.setCallback(callback);
//  while (!client.connected()) {
//      String client_id = "esp32-client-";
//      client_id += String(WiFi.macAddress());
//      Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
//      if (client.connect(client_id.c_str())) {
//          Serial.println("Public EMQX MQTT broker connected");
//      } else {
//          Serial.print("failed with state ");
//          Serial.print(client.state());
//          delay(2000);
//      }
//  }
//  // Publish and subscribe
//  client.publish(topic0, "Hi, I'm ESP32 ^^");
//  client.subscribe(topic);
  
}

void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(topic);
    Serial.print("Message:");

    // Convert payload to string
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    Serial.println(message);

    // Check if the topic is "senderState"
    if (strcmp(topic, "senderState") == 0) {
        // Parse JSON
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, message);

        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }

        // Extract state and comment
        bool state = doc["senderState"];  
        comment = doc["comment"].as<String>();

        // Set senderState based on the received state
        senderState = state;

        Serial.println("Setting senderState to: " + String(senderState));
    }

    Serial.println("-----------------------");
}



void sendMessage(){
  Serial.print("Sender Sending packet: ");
  Serial.println(counter);
  Serial.println(comment);
  // Sending LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print("{\"Data\":\"S0 \",\"Comment\":\"" + String(comment) + "\"}");
  LoRa.endPacket(true);
  counter++;
  Serial.println("send completed");
}
int count = 0;
void loop() {
//  client.loop();
  if(senderState){
    
    
    if(count == 2){
      count = 0;
    }else{
      delay(1000);
      sendMessage();
    }
    count++;
  }
}
