#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "LabMicros";
const char* password = "seluspeesc@";
const char* mqtt_server = "64.23.247.116";  // IP do seu broker MQTT
const char* mqtt_topic_campainha = "campainha";
const char* mqtt_topic_tranca = "tranca";
const char* mqtt_user = "paulo";
const char* mqtt_password = "paulo";

WiFiClient espClient;
PubSubClient client(espClient);

const int buttonPin = 2;  // Pino GPIO onde o botão está conectado
const int ledPin = 5;    // Pino onde o LED está conectado

int buttonState = 0;  // Variável para armazenar o estado do botão

void setup_wifi() {
  delay(10);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando se reconectar ao MQTT...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      Serial.println("Conectado ao MQTT com autenticação");
      client.subscribe(mqtt_topic_tranca);
    } else {
      Serial.print("Falha na conexão ao MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);

  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Mensagem recebida: ");
  Serial.println(message);

  if (String(topic) == mqtt_topic_tranca) {
    if (message == "Abrir") {
      digitalWrite(ledPin, HIGH);  // Acende o LED
      delay(5000);                 // Mantém o LED aceso por 5 segundos
      digitalWrite(ledPin, LOW);   // Apaga o LED
      Serial.println("LED aceso por 5 segundos");
    }
  }
}

void setup() {
  pinMode(buttonPin, INPUT);  // Configura o pino do botão como entrada
  pinMode(ledPin, OUTPUT);    // Configura o pino do LED como saída
  digitalWrite(ledPin, LOW);  // Inicialmente, mantém o LED apagado
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  reconnect();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int currentButtonState = digitalRead(buttonPin);  // Lê o estado atual do botão

  if (currentButtonState == HIGH && buttonState == LOW) {
    client.publish(mqtt_topic_campainha, "Button Pressed");
    Serial.println("Botão pressionado, mensagem enviada ao MQTT");
  }

  buttonState = currentButtonState;

  delay(100);  // Pequena pausa para evitar detecção múltipla de pressionamento
}
