/*   
 * SLBlind
 * ESP8266-based MQTT Blind Controller
 * Licensed under the MIT License, Copyright (c) 2018 Sumit Lonkar
 * 
 * User-configurable Parameters 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include "config.h"


const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const boolean static_ip = STATIC_IP;
IPAddress ip(IP);
IPAddress gateway(GATEWAY);
IPAddress subnet(SUBNET);

const char* mqtt_broker = MQTT_BROKER;
const char* mqtt_clientId = MQTT_CLIENTID;
const char* mqtt_username = MQTT_USERNAME;
const char* mqtt_password = MQTT_PASSWORD;

const char* blind_alias = BLIND_ALIAS;
const char* mqtt_blind_availability_topic = MQTT_BLIND_AVAILABILITY_TOPIC;
const char* mqtt_blind_tilt_state_topic = MQTT_BLIND_TILT_STATE_TOPIC;
const char* mqtt_blind_command_topic = MQTT_BLIND_COMMAND_TOPIC;
const char* mqtt_blind_state_topic = MQTT_BLIND_STATE_TOPIC;
const char* mqtt_blind_tilt_topic = MQTT_BLIND_TILIT_TOPIC;

const int blindPin = BLIND_PIN;

const char* birthMessage = "online";
const char* lwtMessage = "offline";

const char *closedState = "closed";
const char *openState = "open";


WiFiClient espClient;
PubSubClient client(espClient);

Servo blindServo;

/*
  When you will be mounting servo in the blind bay,
  you will have to determine mounting position based on
  availiblity of power plug near by. 

  If you are mounting it on right hand side then set below
  variable to true else false.

  Implementation Details -

  I am setting blind to start in close position when iner 
  edge of blind is facing down. I like my blindes closed 
  that way. Feel to modify start and end position as per
  your preference. Just make sure you test these position
  while coupling servo with blind shaft.

  PS - Sorry for such long comment. Will remove it once 
  README.md is finished. 
*/

bool isRightMounted = false;
int startPosition = (isRightMounted == true) ? 180 : 0;
int openPosition = 90;
int endPosition = (isRightMounted == true) ? 0 : 180;

int currentPosition = 0;

// Wifi setup function

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  if (static_ip) {
    WiFi.config(ip, gateway, subnet);
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print(" WiFi connected - IP address: ");
  Serial.println(WiFi.localIP());
}

// MQTT message received Callback
// calls triggerBlindAction(), passing topic and payload as parameters

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  
  Serial.println();

  String topicToProcess = topic;
  payload[length] = '\0';
  String payloadToProcess = (char*)payload;
  triggerBlindAction(topicToProcess, payloadToProcess);
}

void triggerBlindAction(String topicToProcess, String payloadToProcess) {
  if(topicToProcess == mqtt_blind_command_topic) {
    if (payloadToProcess.equals("OPEN")) {
        moveServoToPositing(openPosition);
        publish_state(openState);
      } else if (payloadToProcess.equals("CLOSE")) {
        moveServoToPositing(endPosition);
        publish_state(closedState);
      }  
   } else if (topicToProcess == mqtt_blind_tilt_topic) {
    int tiltPosition = payloadToProcess.toInt();
    moveServoToPositing(tiltPosition);
    }
}

void moveServoToPositing(int servoPosition) {
      blindServo.attach(blindPin);
      delay(500);
      blindServo.write(servoPosition); 
      delay(500);
      blindServo.detach();
  }

void publish_birth_message() {
  // Publish the birthMessage
  Serial.print("Publishing birth message \"");
  Serial.print(birthMessage);
  Serial.print("\" to ");
  Serial.print(mqtt_blind_availability_topic);
  Serial.println("...");
  client.publish(mqtt_blind_availability_topic, birthMessage, true);
}

void publish_state(const char *state) {
  // Publish the state
  Serial.print("Publishing state message \"");
  Serial.print(state);
  Serial.print("\" to ");
  Serial.print(mqtt_blind_state_topic);
  Serial.println("...");
  client.publish(mqtt_blind_state_topic, state, true);
}

void publish_tilt_state(const char *tilt_state) {
  // Publish the tilt state
  Serial.print("Publishing tilt state message \"");
  Serial.print(tilt_state);
  Serial.print("\" to ");
  Serial.print(mqtt_blind_tilt_state_topic);
  Serial.println("...");
  client.publish(mqtt_blind_tilt_state_topic, tilt_state, true);
}


void setup() {
    // Setup serial output, connect to wifi, connect to MQTT broker, set MQTT message callback
  Serial.begin(115200);

  Serial.println("Starting SLBlind...");

  setup_wifi();
  client.setServer(mqtt_broker, 1883);
  client.setCallback(callback);
}

// Function that runs in loop() to connect/reconnect to the MQTT broker, and publish the current blind status on connect

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_clientId, mqtt_username, mqtt_password, mqtt_blind_availability_topic, 0, true, lwtMessage)) {
      Serial.println("Connected!");

      // Publish the birth message on connect/reconnect
      publish_birth_message();

      // Subscribe to command topic
      Serial.print("Subscribing to ");
      Serial.print(mqtt_blind_command_topic);
      Serial.println("...");
      client.subscribe(mqtt_blind_command_topic);

      // Subscribe to  tilt command topic
      Serial.print("Subscribing to ");
      Serial.print(mqtt_blind_tilt_topic);
      Serial.println("...");
      client.subscribe(mqtt_blind_tilt_topic);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  // Connect/reconnect to the MQTT broker and listen for messages
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
