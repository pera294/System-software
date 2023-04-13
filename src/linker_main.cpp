#include <iostream>
#include <string.h>
#include <fstream>

#include "../inc/parser.h"
#include "../inc/lexer.h"
#include "../inc/symbol.h"
#include "../inc/section.h"

#include "../inc/linker.h"



int main(int argc, char *argv[]) {

  //argv[1]= -hex
  //argv[2]= -o

  
  if(argc < 4){
    cout<<"Error";
    exit(1);
  }

  vector<string> inputFileNames = vector<string>();
  string outputFileName= argv[3];
  for(int i =4; i<argc;i++){
    inputFileNames.push_back(argv[i]);

  }

  // inputFileNames.push_back("ivt.o");
  // inputFileNames.push_back("math.o");
  // inputFileNames.push_back("main.o");
  // inputFileNames.push_back("isr_reset.o");
  // inputFileNames.push_back("isr_terminal.o");
  // inputFileNames.push_back("isr_timer.o");
  // inputFileNames.push_back("isr_user0.o");
 // inputFileNames.push_back("main.o");


 ofstream outputFile(outputFileName);

  Linker *linker = new Linker(&outputFile,inputFileNames);

  linker->load();

  //linker->printAll();
  linker->linkFiles();
  //linker->printAllInfo();

  //linker->printdata();
  //linker->printAllInfo();

  linker->createSymbolTable();
  //linker->printSymbolTable();
  //cout<<" -----------------------------"<<endl;
  linker->updateSymbolTable();
  //linker->printSymbolTable();

  linker->resolveRelocations();

  //linker->printdata();

  linker->writeFile();


return 0;
}