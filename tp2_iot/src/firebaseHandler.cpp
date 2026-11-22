#include <Arduino.h>
#include <WiFi.h> 
#include <time.h>
#include <Firebase_ESP_Client.h>  
#include "addons/TokenHelper.h" 
#include "addons/RTDBHelper.h" 
#include "secrets.h"
#include "firebaseHandler.h"

const String SAFE_NAME = "coffre_424242";

FirebaseAuth auth; 
FirebaseConfig config; 
FirebaseData fbdo; 

void Wifisetup() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  config.api_key = API_KEY; 
  config.token_status_callback = tokenStatusCallback; 
  auth.user.email = ""; 
  auth.user.password = ""; 
  if (Firebase.signUp(&config, &auth, "", "")) { 
    Serial.println("Connexion Firebase OK"); 
  } else { 
    Serial.println("Erreur Firebase"); 
    config.signer.signupError.message.c_str();
  } 
  Firebase.begin(&config, &auth); 
  Firebase.reconnectWiFi(true);  
  
  configTime(0,0, "pool.ntp.org", "time.nist.gov");
  struct tm timeInfo;
  while(!getLocalTime(&timeInfo))
  {
    delay(1500);
  }  
}

void postLog(String message)
{
  if (!Firebase.ready()) {
    return;
  }

  //utiliser l'ai pour le timestamp
  time_t now = time(nullptr);
  char iso[25];
  struct tm utcTime;
  gmtime_r(&now, &utcTime);
  strftime(iso, sizeof(iso), "%Y-%m-%dT%H:%M:%SZ", &utcTime);

  FirebaseJson content;
  content.set("fields/message/stringValue", message);
  content.set("fields/timestamp/timestampValue", iso);
  content.set("fields/safeName/stringValue", SAFE_NAME);
  

  if (Firebase.Firestore.createDocument(&fbdo, PROJECT_ID, "", "Log", content.raw()))
  {
    Serial.println("Message envoyé");
  }
  else
  {
    Serial.println(fbdo.errorReason());
  }
}