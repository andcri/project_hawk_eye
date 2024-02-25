#define CATCH_CONFIG_MAIN

#include "catch2/catch.hpp"

#include <Arduino.h>

#include "painlessmesh/logger.hpp"
using namespace painlessmesh::logger;

LoggClass Logg;

SCENARIO("We can log things") {
  Logg.setLoggLevel(ERROR | DEBUG | COMMUNICATION);
  Logg(ERROR, "We should see the next %u lines\n", 3);
  Logg(DEBUG, "We should see the next %u lines\n", 2);
  Logg(COMMUNICATION, "We should see the next %u lines\n", 1);
  Logg(ERROR, "But not the next one\n");
  Logg(S_TIME, "This should not be showing\n");
}
