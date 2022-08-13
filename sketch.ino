#include <WiFi.h>
#include <PubSubClient.h>
const char* mqtt_server = "70.34.255.114";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;
int GPION0=2;
char lectura;
int serial=1;


//Distance

// defines pins numbers
const int trigPin = 12;
const int echoPin = 14;
// defines variables
long duration;
int distance;


void callback(char* topic, byte* payload, unsigned int length) {
  if(serial==1){
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  }
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(GPION0, HIGH);
  } else {
    digitalWrite(GPION0, LOW);
  }

}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if(serial==1){
    Serial.print("Attempting MQTT connection...");
    }
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      if(serial==1){
      Serial.println("connected");
      }
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
      client.subscribe("messageTopic");
    } else {
      if(serial==1){
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      }
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  Serial.begin(9600);
  Serial.print("Connecting to WiFi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println(" Connected!");
  
  pinMode(GPION0, OUTPUT);     // Initialize the GPION0 pin as an output
  digitalWrite(GPION0, LOW);
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}
void meassureDistance(){
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
  
  char cstr[16];
  itoa(distance, cstr, 10);

  client.publish("distanceTopic", itoa(distance, cstr, 10));
}

void loop() {
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    ++value;
    snprintf (msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    if(serial==1){
    Serial.print("Publish message: ");
    Serial.println(msg);
    }
    meassureDistance();

  }

  
  if(serial==1){
  if (Serial.available()){  
      lectura=Serial.read();
      if((lectura)=='A'){
        digitalWrite(GPION0, HIGH);
        Serial.println(lectura);
        }
      if((lectura)=='B'){
        digitalWrite(GPION0, LOW);
        Serial.println(lectura);
        }  
    }
 }

}

