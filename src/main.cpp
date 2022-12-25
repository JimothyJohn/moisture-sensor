// Load relevant libraries
#include <WiFiClientSecure.h>
#include <MQTTClient.h>  // Enable MQTT
#include <ArduinoJson.h> // Handle JSON messages
#include <Wire.h>        // I2C Comms
#include "secrets.h"     // AWS IoT credentials

#define JSON_SIZE 512
#define MQTT_PORT 8883
#define ACPIN 32

#define COMMAND_RELAY_OFF 0x00
#define COMMAND_RELAY_ON 0x01

uint8_t qwiicRelayAddress = 0x18; // default
uint8_t relayActive = 0;          // default

// Dual-core tasks
TaskHandle_t MQTTHandler;
TaskHandle_t CurrentHandler;

// Network clients
WiFiClientSecure net = WiFiClientSecure();
MQTTClient client = MQTTClient(JSON_SIZE);

float moisture;

const char *topic = "Weaver/Home/moisture/Bedford/master";

void messageHandler(String &topic, String &payload)
{
  Serial.println("incoming: " + topic + " - " + payload);
}

void connect_wifi()
{
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
}

void connectAWS()
{
  connect_wifi();

  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.begin(MQTT_SERVER, MQTT_PORT, net);

  // Create a message handler
  client.onMessage(messageHandler);

  Serial.print("\rConnecting to AWS IOT");

  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }

  if (!client.connected())
  {
    Serial.println("\rAWS IoT Timeout!        ");
    return;
  }

  Serial.println("\rAWS IoT connected!          ");
}

// Auxiiliary Task
void MQTTProcess(void *pvParameters)
{
  /*
  Core 0 process
  Acquires signal sample and publishes
  measurements as JSON over MQTT
  */

  StaticJsonDocument<JSON_SIZE> mqttPayload;
  char buffer[JSON_SIZE];
  double readingSamples;

  // Initialize global JSONmqttPayload
  mqttPayload["device"] = "toilet";

  size_t n;

  for (;;)
  {
    // Make sure MQTT client is connected
    if (!client.connected())
    {
      connectAWS();
    }
    else
    {
      // Average water reading is ~1300
      // Average dry reading is ~3500
      // Normlize moisture to 0-1
      moisture = (analogRead(ACPIN) - 1200) / 2000;

      if (moisture < 0.5 && relayActive)
      {
        Serial.println("Killing lights!");
        Wire.beginTransmission(qwiicRelayAddress);
        Wire.write(COMMAND_RELAY_OFF);
        Wire.endTransmission();
        relayActive = 0;
      }
      else if (moisture > 0.5 && !relayActive)
      {
        Serial.println("Activating lights!");
        Wire.beginTransmission(qwiicRelayAddress);
        Wire.write(COMMAND_RELAY_ON);
        Wire.endTransmission();
        relayActive = 1;
        mqttPayload["moisture"] = relayActive;

        // Prepare JSON message
        n = serializeJson(mqttPayload, buffer);

        if (client.publish(topic, buffer, n))
        {
          Serial.print("\rSent ");
          Serial.print(moisture);
          Serial.print(" to topic \"");
          Serial.print(topic);
          Serial.print("\"");
        }
        else
        {
          Serial.println(buffer);
          Serial.print("Unable to send to ");
          Serial.println(topic);
        }
      }

      client.loop();
      // DELETING THIS DELAY WILL CRASH THE MCU
      delay(1000);
    }
  }
}

// TODO Convert to NFC / error handling process
void MeasureMoisture(void *pvParameters)
{
  for (;;)
  {
    delay(1000);
  }
}

// Setup sequence
void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ;

  Wire.begin();

  delay(2000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  xTaskCreatePinnedToCore(
      MQTTProcess,  /* Task function. */
      "MQTT",       /* name of task. */
      10000,        /* Stack size of task */
      NULL,         /* parameter of the task */
      1,            /* priority of the task */
      &MQTTHandler, /* Task handle to keep track of created task */
      0);           /* pin task to core 0 */

  xTaskCreatePinnedToCore(
      MeasureMoisture, /* Task function. */
      "Current",       /* name of task. */
      10000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      2,               /* priority of the task */
      &CurrentHandler, /* Task handle to keep track of created task */
      1);              /* pin task to core 1 */
}

// Main loop
void loop() {}
