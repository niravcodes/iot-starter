#include <Arduino.h>

#define SHUTDOWN_PIN 0 // GPIO0 on ESP8266

#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <PubSubClient.h>

#include "soundmanager.h"

// topic for receiving transaction information
const char *TOPIC_TRANSACTION = "yarsa/teamx/transaction";

// Initialize WiFi and MQTT client
WiFiClient wifiClient;  // TCP client for WiFi
PubSubClient mqttClient(wifiClient);  // MQTT client for handling MQTT messages working on top of TCP client

// Function to connect to the WiFi network
void connectToWiFi()
{
  WiFi.begin("wifi_detox_2.4", "amrit@8520");
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to the WiFi network");
}

// Function to connect to the MQTT broker
void connectMqtt()
{
  // Wait until the MQTT client is connected
  // wifi should be connected in order to connect to the MQTT broker
  // if the WiFi is not connected, connecting to MQTT broker is not possible
  while (WiFi.status() == WL_CONNECTED && !mqttClient.connected())
  {
    // Generate a unique client ID using the MAC address of the ESP8266
    String client_id = "gu-client-" + String(WiFi.macAddress());

    Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
    if (mqttClient.connect(client_id.c_str()))
    {
      Serial.println("Connected to MQTT broker");

      mqttClient.subscribe(TOPIC_TRANSACTION, 1);
    }
    else
    {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Callback function for MQTT messages
// This function is called when a message is received on a subscribed topic
void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  if (strcmp(topic, TOPIC_TRANSACTION) == 0)
  {
    Serial.println("Transaction message received");

    // convert payload to a string
    char amount[50];
    strncpy(amount, (char *)payload, length);
    amount[length] = '\0'; // null-terminate

    Serial.printf("Transaction amount: %s\n", amount);

    // play the transaction amount sound
    SoundManager.playTransaction(String(amount));
  }
}

void setup()
{
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Set the shutdown pin for the amplifier
  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, LOW); // enable the amplifier (LOW means enabled, HIGH means disabled)

  // Initialize LittleFS (File System for storing sound files)
  LittleFS.begin();

  // Play the power-on sound
  SoundManager.play(SOUND_POWER_ON);

  // Connect to WiFi
  connectToWiFi();

  // Setup MQTT client
  // broker.emqx.io is a public MQTT broker
  mqttClient.setServer("broker.emqx.io", 1883); // set MQTT Broker address and port
  
  // Set the callback function for incoming MQTT messages
  // This function will be called when a message is received on a subscribed topic
  mqttClient.setCallback(mqttCallback);

  // Connect to the MQTT broker
  connectMqtt();
}

void loop()
{
  // Check if the WiFi is still connected
  // If the WiFi is disconnected, try to reconnect
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi disconnected, attempting to reconnect...");
    connectToWiFi();
  }
  else 
  {
    // if mqttClient is disconnected, try to reconnect
    if (!mqttClient.connected())
    {
      Serial.println("MQTT client disconnected, attempting to reconnect...");
      connectMqtt();
    }
    mqttClient.loop();  // Process incoming MQTT messages
  }
}