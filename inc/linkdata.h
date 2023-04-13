#ifndef LINKDATA_H_
#define LINKDATA_H_


#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include<vector>
#include<bits/stdc++.h>

#include "relocdata.h"
#include "section.h"
#include "symbol.h"

using namespace std;


class Linkdata{

public:

string filename;

vector<Symbol*> symbol_table;
vector<Section*> section_table;

//za sve svoje sekcije
vector<Relocdata*> reloc_table;

string data;

Linkdata(string filename){
  this->filename=filename;
  this->symbol_table = vector<Symbol*> ();
  this->section_table =  vector<Section*> ();
  this->reloc_table =  vector<Relocdata*> ();
}


friend ostream &operator<<(ostream &os,const Linkdata &l){

  os<<l.filename<<endl;

  os<<"Section table:"<<endl;
  for(auto i = l.section_table.begin(); i!=l.section_table.end();i++){
    os<<(**i)<<endl;
  }



  os<<"Symbol table:"<<endl;
  for(auto i = l.symbol_table.begin(); i!=l.symbol_table.end();i++){
    os<<(**i)<<endl;
  }

  os<<"Reloc table:"<<endl;
  for(auto i = l.reloc_table.begin(); i!=l.reloc_table.end();i++){
    os<<(**i)<<endl;
  }


  os<<"Data"<<endl;

  for(auto i = l.section_table.begin(); i!=l.section_table.end();i++){
    os<<(**i).getId()<<": "<<(**i).getData()<<endl;
  }




  os<<"-----------------"<<endl;

  return os;

  


}

};


#endif