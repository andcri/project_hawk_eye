#include <Andrea1234-project-1_inferencing.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/edgeimpulse/fomo.h>
#include "painlessMesh.h"

#define   MESH_PREFIX     "andrea"
#define   MESH_PASSWORD   "andreafinal"
#define   MESH_PORT       5555

Scheduler userScheduler;
painlessMesh  mesh;

void sendMessage();

void animalDetected();

void animalDetected() {
  String msg = "A animal was detected here!! \n";
  mesh.sendBroadcast( msg );
}

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
using eloq::ei::fomo;

void setup() {

    mesh.setDebugMsgTypes( ERROR | STARTUP );

    mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
    mesh.onReceive(&receivedCallback);
    mesh.onNewConnection(&newConnectionCallback);
    mesh.onChangedConnections(&changedConnectionCallback);
    mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

    delay(3000);
    Serial.begin(115200);
    Serial.println("ANIMAL DETECTION");

    // camera settings
    camera.pinout.freenove_s3();
    camera.brownout.disable();
    camera.resolution.yolo();
    camera.pixformat.rgb565();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera OK");
}


void loop() {
    // capture picture
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // run FOMO
    if (!fomo.run().isOk()) {
      Serial.println(fomo.exception.toString());
      return;
    }

    // how many animals were found?
    Serial.println("Found %d animal(s) in %dms",
      fomo.count(),
      fomo.benchmark.millis()
    );

    // if we identify an animal trigger the message
    if (fomo.foundAnyObject()) {
      Serial.print("Animal detected, sending update trough mesh \n");
      animalDetected();
    }

    mesh.update();
}
