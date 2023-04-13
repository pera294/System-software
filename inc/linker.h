#ifndef LINKER_H_
#define LINKER_H_

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
#include "linkdata.h"

using namespace std;

class Linker{

private:

ofstream* output_file;
vector<string> inputFileNames;

vector<Linkdata*> linklist;

string finaldata;

vector<string> sectionOrder;

vector<Symbol*> globalSymbol_table;

struct Info{
  string filename;
  string sectionName;
  int sectionId;
  int location;

  Info(string fn,string sn, int sid,int loc){
    filename=fn;
    sectionName=sn;
    sectionId = sid;
    location=loc;
  }
};

vector<Info*> infolist;


int globalcounter=0;

public:

Linker(ofstream* os,vector<string> inputFileNames);

string getfinaldata(){return this->finaldata;}
void setfinaldata(string finaldata){this->finaldata=finaldata;}


void load();

void printAll();
void printdata();
void printAllInfo();

void linkFiles();


void createSymbolTable();
void updateSymbolTable();
void printSymbolTable();

void resolveRelocations();


int calculateSymbolValue(Symbol* symbol);

void writeFile();

string findmyfile(string name);


int strHexInt(string hexstr);
string intHex(int num);
string littleendian(string hexnum);
void changeByte(int index,int len,string value);

int getSymbolValue(string name);


// int find_globalSection_symbol(string filename,int symbolid);
// int find_globalSection_section(string filename,int section_id);

string find_reloc_file(Relocdata* pnt);

int find_smallSection_location(string filename,int sectionid);

string find_symbol_name(string filename,int symbid);

int find_smallSection_where_is_symbol_defined(string name);
string find_filename_where_is_symbol_defined(string  name);

};


#endif