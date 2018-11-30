/*   
 * SLBlind
 * ESP8266-based MQTT Blind Controller
 * Licensed under the MIT License, Copyright (c) 2018 Sumit Lonkar
 * 
 * User-configurable Parameters 
*/

// Wifi Parameters

#define WIFI_SSID "<Wifi-network>"
#define WIFI_PASSWORD "<Wifi- password>"

// Static IP Parameters

#define STATIC_IP false
#define IP 192,168,1,100
#define GATEWAY 192,168,1,1
#define SUBNET 255,255,255,0

// MQTT Parameters

#define MQTT_BROKER "<broker Ip>"
#define MQTT_CLIENTID "<unique_name>"
#define MQTT_USERNAME "mqtt_username"
#define MQTT_PASSWORD "mqtt_password"

// Blind Parameters

/*
  Below are some build parameters for my office blind.
  As you can see I have 'office' word in my each mqtt
  topic. This helps me from not having common mqtt topic
  across all different blinds.
*/

// Office Blind NodeMCU 1
#define BLIND_ALIAS "Office Blind"
#define MQTT_BLIND_AVAILABILITY_TOPIC "slblind/office/cover/availability"
#define MQTT_BLIND_TILT_STATE_TOPIC "slblind/office/cover/tilt-state"
#define MQTT_BLIND_COMMAND_TOPIC "slblind/office/cover/set"
#define MQTT_BLIND_STATE_TOPIC "slblind/office/cover/state"
#define MQTT_BLIND_TILIT_TOPIC "slblind/office/cover/tilt"
    
#define BLIND_PIN D1

// Mapping NodeMCU Ports to Arduino GPIO Pins
#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12 
#define D7 13
#define D8 15

