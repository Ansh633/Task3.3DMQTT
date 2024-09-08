/*
Name- Ansh Sharma
Student Id- 2310994763
In Task 3.3D of SIT210, the goal is to create a publish-subscribe 
communication system using Arduino Nano 33 IoT, an ultrasonic sensor, an LED, and the MQTT protocol
In the following code represent Publisher which will publish the details of the ultrasonic sensor to
the broker which will then be reccieved by subsriber
*/

// here i have includedd the libraries which i have used for this task 
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Ansh";
const char* password = "1234567890";

const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* TopicWave = "SIT210/wavesJasnoor";
const char* TopicPat = "SIT210/patsJasnoor";

WiFiClient espClient;
PubSubClient client(espClient);

// Here i Have declare the pins for the ultrasonic sensor one for trig and other for echo
const int trigPin = 9;
const int echoPin = 10;

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);

  // Here i have initilized the pins for the trig and echo
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  connectToWiFi();
  client.setServer(mqttServer, mqttPort);
  connectToMQTT();
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();
  }
  client.loop();

  // Here if pat is detected then we publish at topic of pat other wise at wave
  if (detectPat()) {
    client.publish(TopicPat);
    delay(1000);
  }
  else{
    client.publish(TopicWave);
    delay(1000);
  }
}

void connectToWiFi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
}

void connectToMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("SenderClient")) {
      Serial.println("Connected to MQTT");
    } else {
      delay(5000);
    }
  }
}


// Here is the function of detection of pat , the datatype is bool to give us true or false also
bool detectPat() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long durO = pulseIn(echoPin, HIGH);
  int disO= durO * 0.034 / 2;

  if (disO> 20) {
    return true;
  }
  if (disO> 0 && disO< 20) {
    return false;
  }
}
