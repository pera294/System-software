#include <iostream>
#include <string.h>
#include <fstream>

#include "../inc/emulator.h"

using namespace std;


int main(int argc, char *argv[]) {

  string inputFilename = argv[1];

  Emulator *emulator = new Emulator(inputFilename);
  
  emulator->init();

  emulator->emulation();

  emulator->printResults();
  
  return 0;
}