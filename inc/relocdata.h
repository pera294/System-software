#ifndef RELOCDATA_H_
#define RELOCDATA_H_


#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include<vector>
#include<bits/stdc++.h>


using namespace std;

class Relocdata {
private:

 int mysection;
  int offset;

  string type;
  

  int symbol_id;
  int symbol_section_id;

  int addend;
  bool resolved;

  string endian;
  string addr;

 
  
public:
 

  Relocdata(int mysection,int offset,string type,int symbol_id,int symbol_section_id,int addend,bool resolved,string endian,string addr){

    this->mysection = mysection;
    this->offset = offset;

    this->type = type;
    
    

    this->symbol_id = symbol_id;
    this->symbol_section_id = symbol_section_id;

    this->addend = addend;
    this->resolved = resolved;

    this->endian=endian;
    this->addr=addr;
  }

    string getendian() {return this->endian;}
  

    string getaddr() {return this->addr;}
  
 


  int getsymbolsection() {return this->symbol_section_id;}
  void setsymbolsection(int id) {this->symbol_section_id = id;}


    int getmysection() {return this->mysection;}
  void setmysection(int id) {this->mysection = id;}



   string getType() {return this->type;}
  void setType(string type) {this->type = type;}

  //string appendType(string type){this->type += type;}

  int getOffset() {return this->offset;}
  void setOffset(int offset) {this->offset = offset;}

  int getSymbolId() {return this->symbol_id;}
  void setSymbolId(int id) {this->symbol_id = id;}

  int getaddend() {return this->addend;}
  void setaddend(int addend) {this->addend = addend;}

  bool getResolved() {return this->resolved;}
  void setResolved(bool res) {this->resolved = res;}

  friend ostream &operator<<(ostream &os,const Relocdata &rd){
    os<< setw(3)<<rd.mysection<< setw(10)<<dec<< rd.offset <<setw(10)<< rd.type <<setw(8)<< rd.symbol_id<<setw(8)<< rd.symbol_section_id <<dec<<  setw(8)<<rd.addend << setw(8)<<rd.resolved <<setw(8)<<rd.endian <<setw(8)<<rd.addr ;
    return os;
  }

  string stringRelocdata(){
    string ret="";

    ret += to_string(mysection);
    ret += " ";
    ret += to_string(offset);
    ret += " ";
    ret += type;
    ret += " ";
    ret += to_string(symbol_id );
    ret += " ";
    ret += to_string(symbol_section_id);
    ret += " ";
    ret += to_string(addend);
    ret += " ";
    ret += endian;
    ret += " ";
    ret += addr;
    return ret;
  }


};
#endif