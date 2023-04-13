#include <iostream>
#include <string.h>
#include <fstream>

#include "../inc/parser.h"
#include "../inc/lexer.h"
#include "../inc/symbol.h"
#include "../inc/section.h"

#include "../inc/assembler.h"



int main(int argc, char *argv[]) {

//  string outputFileName = "isr_reset.o"; 
//  string inputFileName = "../tests/isr_reset.s"; 

    string outputFileName;
    string inputFileName;

   
     outputFileName = argv[2];
     inputFileName = "../tests/";

     inputFileName = inputFileName + argv[3];
   
  
  
   ofstream outputFile(outputFileName);
   ifstream inputFile(inputFileName);


  //Assembler* assembler = new Assembler("../tests/math.s" , "outputstring");
  Assembler* assembler = new Assembler(&outputFile, &inputFile);

  while(!assembler->isEnd()){
    assembler->assembleLine();
  }

  if(!assembler->isError()){

   // assembler->print_SectionTable();
   // assembler->print_SymbolTable();
   // assembler->printfInfoSections();
   // assembler->print_Flinktable();

    //ubaci  u fajl
   assembler->writeFile();

  }else{
    assembler->printErrors();
      inputFile.close();
      outputFile.close();
      exit(1);

  }

 // assembler->closeOutput();
 inputFile.close();
 outputFile.close();

return 0;
}