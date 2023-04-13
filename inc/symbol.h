#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include<vector>
#include<bits/stdc++.h>

#include "section.h"
#include "flinkdata.h"

using namespace std;

class Symbol {
private:
 static int cnt;

 string name;
 int id;
 int value;

 int section;
 string scope;

 bool resolved;

public:



Symbol(string name,int value,int section,string scope,bool resolved);
Symbol(int id,string name,int value,int section,string scope,bool resolved);

int getId() {return this->id;}
void setId(int id ){this->id = id;}

int getValue() {return value;}
void setValue(int value ){this->value = value;}

string getName(){return this->name;}
void setName(string name) { this->name=name;}

int getSection() {return section;}
void setSection(int section){this->section = section;}

string getScope() {return scope;}
void setScope(string scope ){this->scope= scope;}

bool isResovled() {return this->resolved;}
void setResolved(bool flag ){this->resolved = flag;}


string stringSymbol();



  
friend ostream &operator<<(ostream &os,const Symbol &s){
  os <<setw(4)<<dec<<left<< s.id << ",";
  os <<setw(10)<<left<< setfill(' ') << s.name << ",";
  os <<"0x"<< hex <<setw(8)<<right<< setfill('0')<<s.value<< ",";
  os <<setw(10)<<left<< setfill(' ') << s.section <<",";
  os <<setw(6)<<left<<setfill(' ')<< s.scope<< ",";
  os <<setw(1)<< s.resolved <<",";

    return os;
  }

};




#endif
