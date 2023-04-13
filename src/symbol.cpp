#include "../inc/symbol.h"

int Symbol::cnt=0;

Symbol::Symbol(string name,int value,int section,string scope,bool resolved){
  this->id = cnt++;
  this->name = name;
  this->value = value;
  this-> section = section;
  this->scope = scope;
  this->resolved = resolved;
}


Symbol::Symbol(int id,string name,int value,int section,string scope,bool resolved){
  this->id = id;
  this->name = name;
  this->value = value;
  this-> section = section;
  this->scope = scope;
  this->resolved = resolved;
}


string Symbol::stringSymbol(){
  string ret="";

  ret += to_string(id);
  ret += " ";
  ret += name;
  ret += " ";
  ret += to_string(value);
  ret += " ";
  ret += to_string(section);
  ret += " ";
  ret += scope;
  ret += " ";
  ret += to_string(resolved);

  return ret;
}