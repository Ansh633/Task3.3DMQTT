#include <WiFi.h>           // Library for WiFi connection
#include <PubSubClient.h>    // Library for MQTT communication

// WiFi credentials
const char* ssid = "Ansh";
const char* password = "12345678";

// MQTT broker details
const char* mqttServer = "broker.emqx.io";  // Address of the MQTT broker
const int mqttPort = 1883;                  // MQTT broker port
const char* TopicWave = "SIT210/waveAnsh";  // MQTT topic for "wave" messages
const char* TopicPat = "SIT210/patsAnsh";   // MQTT topic for "pat" messages

// Create WiFi and MQTT client objects
WiFiClient espClient;
PubSubClient client(espClient);

// Pin declarations
const int trigPin = 9;        // Ultrasonic sensor trigger pin
const int echoPin = 10;       // Ultrasonic sensor echo pin
const int patLedPin = 7;      // LED pin for indicating "pat" detection
const int waveLedPin = 6;     // LED pin for indicating "wave" detection

void setup() {
  Serial.begin(115200);       // Initialize the serial monitor at 115200 baud rate
  pinMode(trigPin, OUTPUT);   // Set ultrasonic sensor trigger pin as output
  pinMode(echoPin, INPUT);    // Set ultrasonic sensor echo pin as input
  pinMode(patLedPin, OUTPUT); // Set LED for "pat" as output
  pinMode(waveLedPin, OUTPUT);// Set LED for "wave" as output

  connectToWiFi();            // Connect to WiFi network

  // Set up MQTT client server and callback function
  client.setServer(mqttServer, mqttPort);
  client.setCallback(printMessage);  // Function to handle received MQTT messages

  connectToMQTT();            // Connect to the MQTT broker and subscribe to topics
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();          // Reconnect to MQTT if the connection is lost
  }
  client.loop();              // Process incoming MQTT messages

  // Check if a "pat" or "wave" gesture is detected and publish the result
  if (detectPat()) {
    client.publish(TopicPat, "Pat detected");   // Publish "pat" detection message
  } else {
    client.publish(TopicWave, "Wave detected"); // Publish "wave" detection message
  }
  
  delay(1000); // Delay to control the publishing rate
}

// Function to connect to WiFi
void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);                // Start connecting to WiFi network
  while (WiFi.status() != WL_CONNECTED) {    // Wait until WiFi is connected
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");       // Print confirmation message once connected
}

// Function to connect to MQTT broker
void connectToMQTT() {
  while (!client.connected()) {              // Repeat until successfully connected
    Serial.println("Connecting to MQTT...");
    if (client.connect("NanoClient")) {      // Try to connect with a client ID
      Serial.println("Connected to MQTT");
      client.subscribe(TopicWave);           // Subscribe to "wave" topic
      client.subscribe(TopicPat);            // Subscribe to "pat" topic
    } else {
      delay(5000);                           // Wait and retry if connection fails
    }
  }
}

// Callback function to handle received MQTT messages
void printMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  
  // Convert payload into a readable string
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println("Message: " + message);

  // Check topic and turn on the appropriate LED
  if (strcmp(topic, TopicWave) == 0) {           // If the topic is "wave"
    Serial.println("Wave message received. Turning on wave LED...");
    digitalWrite(waveLedPin, HIGH);              // Turn on wave LED
    digitalWrite(patLedPin, LOW);                // Turn off pat LED to ensure only one LED is on
  } else if (strcmp(topic, TopicPat) == 0) {     // If the topic is "pat"
    Serial.println("Pat message received. Turning on pat LED...");
    digitalWrite(patLedPin, HIGH);               // Turn on pat LED
    digitalWrite(waveLedPin, LOW);               // Turn off wave LED
  }
}

// Function to detect "pat" gesture using the ultrasonic sensor
bool detectPat() {
  // Trigger a pulse for the ultrasonic sensor
  digitalWrite(trigPin, LOW);       // Ensure trigger pin is low for 2 microseconds
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);      // Send a 10-microsecond pulse to trigger
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the pulse returned to the echo pin
  long duration = pulseIn(echoPin, HIGH);       // Pulse duration in microseconds
  int distance = duration * 0.034 / 2;          // Calculate distance in cm
  Serial.println(distance);                     // Print the distance for debugging

  // Determine gesture based on distance
  if (distance > 20) {                          // If distance is greater than 20 cm
    return true;                                // Detected "pat" gesture
  }
  return false;                                 // Otherwise, it's a "wave" gesture
}
