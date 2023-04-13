#include "../inc/lexer.h"

vector<string> Lexer::getTokens(string line){
  vector<string> ret;
  stringstream ss(line);
  string token;
  while(ss>>token){
    ret.push_back(token);
  }
  return ret;
}

void Lexer::printTokens(string line){
  stringstream ss(line);
  string token;
  while(ss>>token){
    cout<<token<<"  -   ";
  }
  cout<<endl;
}