#include "../inc/parser.h"
#include "../inc/lexer.h"


Parser::Parser(ifstream* is){
  
  // ifstream inputfile;
  // inputfile.open(file_name);

  std::string str;
  
  while(std::getline(*is,str)){
    if(str.size()>0) {
      str = removeComments(str); 
      str = removeCommas(str);
      str = removeSpaces(str);
      
       if(str.size()>0) {
        this->lines.push_back(str);
        //vector<string> temp = Lexer::getTokens(str);
        //Lexer::printTokens(str);

       }
       
       
          
    }
      
  }
 
 
 }

void Parser::printLines(){
   for(std::string & line : lines)
            std::cout<<line<<std::endl;
}

string Parser::removeComments(string str){
  string temp = str.substr(0,str.find('#'));
  return temp;

}

string Parser::removeSpaces(string str){

  string s;
  string word;
  std::istringstream ss(str);
   while (ss >> word) {
        if (!s.empty()) {
            s += ' ';
        }
        s += word;
    }
 
    return s;
 
  
}

string Parser::removeCommas(string str){
   string s = str;
    const char x = ',';
    const char y = ' ';
 
    std::replace(s.begin(), s.end(), x, y);
    //cout<<s<<endl;
    return s;

}

string Parser:: getNextLine(){
  string ret = lines.front();
  lines.erase(lines.begin());
  return ret;

}
