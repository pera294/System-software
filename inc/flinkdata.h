#ifndef FLINKDATA_H_
#define FLINKDATA_H_


#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include<vector>
#include<bits/stdc++.h>

#include "section.h"

class Flinkdata {
private:
  string name;
  int location;
  int sectionNum;
  int numbytes;
  bool resolved;
public:
 

  Flinkdata(string name,int location,int sectionNum,int numbytes){
    this->name = name;
    this->location = location;
    this->sectionNum = sectionNum;
    this->numbytes = numbytes;
    this->resolved = false;
  }

  string getName() {return this->name;}
  void setName(string name) {this->name = name;}

  int getLocation() {return this->location;}
  void setLocation(int loc) {this->location = loc;}

  int getsectionNum() {return this->sectionNum;}
  void setsectionNum(int sn) {this->sectionNum = sn;}

  int getnumBytes() {return this->numbytes;}
  void setnumBytes(int nb) {this->numbytes = nb;}

  bool getResolved() {return this->resolved;}
  void setResolved(bool res) {this->resolved = res;}

  friend ostream &operator<<(ostream &os,const Flinkdata &f){
    os<< setw(16)<< f.name <<setw(8)<< f.location <<setw(8)<< f.sectionNum << setw(8)<<f.numbytes<<setw(2)<<f.resolved;
    return os;
  }


};
#endif