#include <WiFi.h>          // ESP32
#include <PubSubClient.h>
// --------------------------------------------------
// WLAN & MQTT KONFIGURATION
// --------------------------------------------------
const char* ssid        = "1";
const char* password    = "";

const char* mqtt_server = "192.168.2.51";   // IP oder Hostname des MQTT-Brokers
const int   mqtt_port   = 1883;

const char* mqtt_client_id = "ESP32_Client_01";
const char* mqtt_sub_topic = "haus/steuerung";
const char* mqtt_pub_topic = "haus/sensor1";

// --------------------------------------------------
// Objekte
// --------------------------------------------------
WiFiClient espClient;
PubSubClient client(espClient);

// --------------------------------------------------
// Callback: Wird aufgerufen, wenn eine MQTT-Nachricht ankommt
// --------------------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("MQTT Nachricht empfangen [");
  Serial.print(topic);
  Serial.print("]: ");

  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Beispiel: Reaktion auf empfangene Daten
  if (message == "LED_ON") {
    Serial.println("→ LED einschalten");
  }
  if (message == "LED_OFF") {
    Serial.println("→ LED ausschalten");
  }
}

// --------------------------------------------------
// WLAN verbinden
// --------------------------------------------------
void setup_wifi() {
  delay(100);
  Serial.println();
  Serial.print("Verbinde mit WLAN: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WLAN verbunden!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());
}

// --------------------------------------------------
// MQTT verbinden (mit Auto-Reconnect)
// --------------------------------------------------
void reconnect() {
  while (!client.connected()) {
    Serial.print("Verbinde mit MQTT... ");

    if (client.connect(mqtt_client_id)) {
      Serial.println("verbunden!");

      // Topic abonnieren
      client.subscribe(mqtt_sub_topic);
      Serial.print("Abonniert: ");
      Serial.println(mqtt_sub_topic);

    } else {
      Serial.print("Fehler, rc=");
      Serial.print(client.state());
      Serial.println(" → Neuer Versuch in 5 Sekunden");
      delay(5000);
    }
  }
}

// --------------------------------------------------
// SETUP
// --------------------------------------------------
void setup() {
  Serial.begin(115200);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// --------------------------------------------------
// LOOP
// --------------------------------------------------
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Beispiel: Alle 5 Sekunden Daten senden
  static unsigned long lastMsg = 0;
  if (millis() - lastMsg > 5000) {
    lastMsg = millis();

    String payload = String("Temperatur: ") + random(20, 30);
    client.publish(mqtt_pub_topic, payload.c_str());

    Serial.print("MQTT gesendet: ");
    Serial.println(payload);
  }
}
