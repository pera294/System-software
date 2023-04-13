#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include<vector>
#include<bits/stdc++.h>

using namespace std;


class Parser{
  private:

    ifstream my_file;
    std::vector<std::string> lines;
    

  public:

  Parser(ifstream* is);

  std::vector<std::string> getLines(){ return lines;}

  void printLines();

  string removeComments(string str);
  string removeSpaces(string str);
  string removeCommas(string str);

  string getNextLine();

};



#endif