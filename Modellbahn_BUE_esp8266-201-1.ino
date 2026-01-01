#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

// WLAN- und MQTT-Broker-Informationen
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "192.168.2.51";
const char* mqtt_client_id = "BUE";

// Objekte
WiFiClient espClient;
PubSubClient client(espClient);
Servo myservo;

// Servopin und Positionen
const int servoPin = 13; // IO13 ist oben rechts der 2. Pin serielle Schnittstelle ist oben
const int positionON = 180;
const int positionOFF = 0;

// Servo Bewegungssteuerung
int currentPos = positionOFF;
int targetPos = positionOFF;
unsigned long previousMillis = 0;
const long interval = 60; // Anpassen für die gewünschte Geschwindigkeit


void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  myservo.attach(servoPin);
  myservo.write(currentPos);
  delay(1000);

}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Verbinde mit ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi verbunden");
  Serial.println("IP Adresse: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  message.trim();
  
  if (strcmp(topic, "Bue") == 0) {
    if (message == "oeffnen") {
      targetPos = positionON;
      myservo.attach(servoPin);
    } else if (message == "schliessen") {
      targetPos = positionOFF;
      myservo.attach(servoPin);
    }
  }
}

void moveServo() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    if (currentPos < targetPos) {
      currentPos++;
      myservo.write(currentPos);
      if(currentPos==targetPos)
      {
        // Schranke offen
        myservo.detach();
        client.publish("BueStatus","offen");
      }
    } else if (currentPos > targetPos) {
      currentPos--;
      myservo.write(currentPos);
      if(currentPos==targetPos)
      {
        // Schranke geschlossen
        myservo.detach();
        client.publish("BueStatus","geschlossen");
      }
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Versuche MQTT-Verbindung...");
    if (client.connect(mqtt_client_id)) {
      Serial.println("verbunden");
      client.subscribe("Bue");
    } else {
      Serial.print("fehlgeschlagen, rc=");
      Serial.print(client.state());
      Serial.println(" versuche es in 5 Sekunden erneut");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  moveServo();
}
