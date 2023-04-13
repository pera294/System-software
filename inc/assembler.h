#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_

#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include<vector>
#include<bits/stdc++.h>
#include<list>
#include<iterator>


#include "section.h"
#include "symbol.h"
#include "parser.h"
#include "lexer.h"
#include "flinkdata.h"
#include "relocdata.h"

using namespace std;

class Assembler{

  private:

    Parser* parser;
    string input;
    string output;

    ofstream *outputFile;
    ifstream *inputFile;

    vector<Symbol*> symbol_table;
    vector<Section*> section_table;
    vector<Flinkdata*> flink_table;
    

    Section* currentSection;

    bool end = false;
    bool error=false;
    vector<string> errors;

   

    int localcounter;
    

  public:

  Assembler(ofstream* os ,ifstream* is);

  void closeOutput(){ outputFile->close();}

  bool isEnd(){return end;}

  void print_AllRelocTables();

  //Symbols
   
  void addSymbolValuetoSection(Symbol* symbol);
  void print_SymbolTable();
  string removeComma(string word);
  string removeLabel(string word);
  bool checkComma(string word);

  Symbol* searchSymbolTable(string name);
  Symbol* searchSymbolTableID(int id);
  

  //Sections
  void openSection(string name);
  Section* searchSectionTable(string name);
  Section* getSectionbyId (int id);

  void print_SectionTable();
  void printfInfoSections();


  void print_Flinktable();
  void assembleLine();


  bool isLabel(string word);
  bool isRegister(string word);
  bool isNumber(string word);

  bool isSymbol(string word);

  bool checkifResolved(string symbname);

  void resolveSymbols(string name,Section* section,int val);
  void resolveGlobalSymbols(string name,Section* section,int val);


  int strHexInt(string hexstr);
  string intHex(int num);
  string littleendian(string hexnum);

  list<Flinkdata*> getUnresolvedSymbolsfromFlink(string name);

  string  mergeTokens(vector<string> tokens,int start);

  string resolveOperandJump(string operand);
  string calculateSecondPartJump(string addr,string operand);

  string resolveOperandData(string operand);
  string calculateSecondPartData(string addr,string operand);
  
  void checkError(vector<string> tokens);
  void printErrors();
  bool isError(){return error;};

  void writeFile();

};



#endif
