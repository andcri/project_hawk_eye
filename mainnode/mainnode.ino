#include "painlessMesh.h"
#include <espnow.h>
#include <ESP8266WiFi.h>

#define   MESH_PREFIX     "andrea"
#define   MESH_PASSWORD   "andreafinal"
#define   MESH_PORT       5555
#define LED 5

Scheduler userScheduler;
painlessMesh  mesh;

void sendMessage() ;

void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u msg=%s\n", from, msg.c_str());
    // Check if msg contains "A new"
  if (msg.charAt(0) == 'A') {
    // Print something else on the screen
    digitalWrite(LED, HIGH);
  }
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("-->New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(16, INPUT);

  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
}

void loop() {
  // Read the button
  int value = digitalRead(16);
  // if the button is pressed we reset the led
  if (value == 1) {
    digitalWrite(LED, LOW);
  }
  mesh.update();
}
