#ifndef LEXER_H_
#define LEXER_H_

#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include<vector>

using namespace std;

class Lexer {
  private:


  public:

  static vector<string> getTokens (string line); 
  static void printTokens(string line);
 

};

#endif