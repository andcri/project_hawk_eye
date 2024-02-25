#include <tflm_esp32.h>
#include <eloquent_tinyml.h>
#include <eloquent_tinyml/zoo/person_detection.h>
#include <eloquent_esp32cam.h>
#include "painlessMesh.h"

#define   MESH_PREFIX     "andrea"
#define   MESH_PASSWORD   "andreafinal"
#define   MESH_PORT       5555

Scheduler userScheduler;
painlessMesh  mesh;

void sendMessage();

void personDetected();

void personDetected() {
  String msg = "A person was detected here!! \n";
  mesh.sendBroadcast( msg );
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

using eloq::camera;
using eloq::tinyml::zoo::personDetection;

void setup() {
  Serial.begin(115200);

  mesh.setDebugMsgTypes( ERROR | STARTUP );

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  delay(3000);

  Serial.println("PERSON DETECTION");

  // camera settings
  camera.pinout.freenove_s3();
  camera.brownout.disable();
  camera.resolution.yolo();
  camera.pixformat.gray();

  // init camera
  while (!camera.begin().isOk())
      Serial.println(camera.exception.toString());

  // init tf model
  while (!personDetection.begin().isOk())
      Serial.println(personDetection.exception.toString());

  Serial.println("Camera OK");
}

void loop() {
  Serial.println("Loop");

  // capture picture
  if (!camera.capture().isOk()) {
      Serial.println(camera.exception.toString());
      return;
  }

  // run person detection
  if (!personDetection.run(camera).isOk()) {
      Serial.println(personDetection.exception.toString());
      return;
  }

  // a person has been detected!
  if (personDetection) {
      Serial.print("Person detected, sending update trough mesh \n");
      personDetected();
  }
  mesh.update();
}
