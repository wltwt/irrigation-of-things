#pragma once
//#include "netset.h"
#include <PubSubClient.h>


extern PubSubClient client;

void mqttCallback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setupMQTT();
void mqttTask(void *parameter);