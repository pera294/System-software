#ifndef SECTION_H_
#define SECTION_H_

#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include<vector>
#include<bits/stdc++.h>

#include "relocdata.h"

using namespace std;

class Section {
private:
 static int cnt;

 int id;
 string name;
 int size;
 
 string data;


public:
 vector<Relocdata*> reloc_table;

Section(string name,int size);

Section(int id,string name,int size);

int getId() {return this->id;}
void setId(int id ){this->id = id;}

string getName(){return this->name;}
void setName(string name) { this->name=name;}

int getSize() {return size;}
void setSize(int size ){this->size = size;}

friend ostream &operator<<(ostream &os,const Section &s){
  os<<setw(4)<<left<< s.id << ",";
  os<<setw(10)<<left<< setfill(' ')<< s.name << ",";
  os<<setw(8)<<dec<<left<< setfill(' ')<< s.size;
  return os;
}

string getData() {return this->data;}
void setData(string data){this->data = data;}
void addData(string data);


void printSectionInfo();

//string DataToString();


void changeByte(int index,int len,string value);

string stringSection();
string stringReloctable();

};


#endif
