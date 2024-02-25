# Project Hawk-Eye

## Summary

This repository contains all the necessary files to detect people and cats using ESP32S3 microcontrollers. Once detected, it transmits the information to a local mesh network and then notifies a main node by lighting up an LED.

## Setup

To use this, follow these steps:

1. Flash `animaldetection.ino` to one ESP32S3 microcontroller.
2. Flash `persondetection.ino` to another ESP32S3 microcontroller.
3. Flash `mainnode.ino` to any ESP8266 board.
   
You can adjust various parameters in the code such as the hostname, password, and port based on your convenience.
