/*
Name- Ansh Sharma
Student Id- 2310994763
In Task 3.3D of SIT210, the goal is to create a publish-subscribe 
communication system using Arduino Nano 33 IoT, an ultrasonic sensor, an LED, and the MQTT protocol
In the following code represent Suscriber which will recieve message from the publisher about the distance from the
ultrasonic sensor and will bink the led if the distance comes below a certain value 
*/

// here i have includedd the libraries which i have used for this task 

#include <WiFi.h>
#include <PubSubClient.h>

// Here i have declare my Wifi credintials throught which i will be able to connect my Nano
const char* ssid = "Ansh";
const char* password = "1234567890";

// Now the details of the broker which comtain the server, port,topic 
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttTopicWave = "SIT210/wavesAnshSharma";
const char* mqttTopicPat = "SIT210/patsAnshSharma";

// here the declaration is done
WiFiClient espClient;
PubSubClient client(espClient);

// Now when the distancecomes below athreshold value, Led have to blink for our purpose
// so here i have declare the pin for that Led
const int ledPin = 6;

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);

  // Here the initilization of the pin is done for led
  pinMode(ledPin, OUTPUT);

  connectToWiFi();

  // Here the setup of MQTT is done
  client.setServer(mqttServer, mqttPort);
  client.setprintmessageSer(printmessageSer);

  StartMQTTser();
}

void loop() {
  // if not connected then calling StartMQTTser() to connect 
  if (!client.connected()) {
    StartMQTTser();
  }
  client.loop();
}

// It is a function which will be connecting to wifi 
void connectToWiFi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);     // If i has not been connected , it will go on unit conneccted
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi"); // if connected then it will show on serial moniter that it have connected
}

//  This function is making the connecction to MQTT , if client is connected it will show connected otherwise it will wait
void StartMQTTser() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ReceiverClient")) {
      Serial.println("Connected to MQTT");

      // Subscribe to the topics that was declared in starting
      client.subscribe(mqttTopicWave);
      client.subscribe(mqttTopicPat);
    } else {
      delay(5000);
    }
  }
}

// A function is defined which will help to print the message recieved from the publisher on the serial moniter
void printmessageSer(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, mqttTopicWave) == 0) {
    Serial.println("Wave message received. Flashing LED 3 times...");
    flashLED(3);
  } else if (strcmp(topic, mqttTopicPat) == 0) {
    Serial.println("Pat message received. Flashing LED 5 times...");
    flashLED(5);  // Different flash pattern for "pat"
  }
}

// A fucntion which will preform the sequenctial on and off of led
void flashLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
}
