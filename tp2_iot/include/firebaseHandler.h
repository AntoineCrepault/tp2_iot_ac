#pragma once

#include <Firebase_ESP_Client.h>

extern FirebaseAuth auth;
extern FirebaseConfig config;
extern FirebaseData fbdo;

void Wifisetup();
void postLog(String message);