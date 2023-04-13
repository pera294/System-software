#include "../inc/assembler.h"

Assembler::Assembler(ofstream* os, ifstream* is){

  

  this->inputFile = is;
  this->outputFile = os;

  this->parser = new Parser(is);


  this->symbol_table = vector<Symbol*>();
  this->section_table = vector<Section*>();
  this->flink_table = vector<Flinkdata*>();
 

  Section * UNDsection = new Section("UND",0);
  section_table.push_back(UNDsection);
  this->currentSection = UNDsection;

  this-> localcounter = 0;
  this->errors = vector<string>();
  

 

}


string Assembler::calculateSecondPartData(string addr,string operand){

  string ret;

  if(addr == "neposredno"){
    ret+="000";

    operand = operand.substr(1,operand.length()-1);
    if(isNumber(operand)){
       int val = stoul(operand,nullptr,0);
       ret += intHex(val);
    }

     else if(isSymbol(operand)){

            Symbol* s = searchSymbolTable(operand);
            
            if(s!=nullptr){ 
              //ako je vec definisan 
              if(s->isResovled()){
                //currentSection->addData(littleendian(intHex(s->getValue())));
                ret += intHex(s->getValue());
              }
              else{
                //ako nije defisan
              Flinkdata *newflink = new Flinkdata(operand,currentSection->getSize()+3,
                                                          currentSection->getId(),2);
              flink_table.push_back(newflink);
              ret += "0000";
                 
              }

            }
            else{ //simbol postoji u tabeli

                Symbol *symbol = new Symbol(operand,0,currentSection->getId(),"LOCAL",0);
                symbol_table.push_back(symbol);

                Flinkdata *newflink = new Flinkdata(operand,currentSection->getSize()+3,
                                                            currentSection->getId(),2);
                flink_table.push_back(newflink);
                ret += "0000";
            }

              //reloc zapis
            int idsymb = searchSymbolTable(operand)->getId();
            bool res = searchSymbolTable(operand)->isResovled();

            int addend = 0;
            if(res){
                if(searchSymbolTableID(idsymb)->getScope()=="GLOBAL" 
                  || searchSymbolTableID(idsymb)->getScope()=="EXTERN" ){
                    addend=0;
                  }
                  else{ //LOCAL
                    addend=searchSymbolTableID(idsymb)->getValue();
                  }
            }


            string type;
            if(searchSymbolTableID( idsymb )->getScope() == "LOCAL"){
             type ="loc_rel";
            }
            else 
              type = "glob_rel";
            
            Relocdata* rd= new Relocdata(currentSection->getId(),currentSection->getSize()+3,type,idsymb,searchSymbolTableID(idsymb)->getSection(),addend,res,"big","abs");
            currentSection->reloc_table.push_back(rd);


          }

    

  } else if(addr=="pcrel"){
    ret+="703";
    operand = operand.substr(1,operand.length()-1);

    if(isSymbol(operand)){

            Symbol* s = searchSymbolTable(operand);
            
            if(s!=nullptr){ 
              //ako je vec definisan 
              if(s->isResovled()){
                //currentSection->addData(littleendian(intHex(s->getValue())));
                ret += intHex(s->getValue());
              }
              else{
                //ako nije defisan
              Flinkdata *newflink = new Flinkdata(operand,currentSection->getSize()+3,
                                                          currentSection->getId(),2);
              flink_table.push_back(newflink);
              ret += "0000";
                 
              }

            }
            else{ //simbol postoji u tabeli

                Symbol *symbol = new Symbol(operand,0,currentSection->getId(),"LOCAL",0);
                symbol_table.push_back(symbol);

                Flinkdata *newflink = new Flinkdata(operand,currentSection->getSize()+3,
                                                            currentSection->getId(),2);
                flink_table.push_back(newflink);
                ret += "0000";
            }

              //reloc zapis
            int idsymb = searchSymbolTable(operand)->getId();
            bool res = searchSymbolTable(operand)->isResovled();

            int addend = -2;
            string type;
            if(searchSymbolTableID( idsymb )->getScope() == "LOCAL"){
             type ="loc_rel";
            }
            else 
              type = "glob_rel";
            
            Relocdata* rd= new Relocdata(currentSection->getId(),currentSection->getSize()+3,type,idsymb,searchSymbolTableID(idsymb)->getSection(),addend,res,"big","rel");
            currentSection->reloc_table.push_back(rd);
            

          }

    

  } else if(addr=="memdir"){

    ret+="004";

    if(isNumber(operand)){
       int val = stoul(operand,nullptr,0);
       ret += intHex(val);
    }
   if(isSymbol(operand)){

            Symbol* s = searchSymbolTable(operand);
            
            if(s!=nullptr){ 
              //ako je vec definisan 
              if(s->isResovled()){
                //currentSection->addData(littleendian(intHex(s->getValue())));
                ret += intHex(s->getValue());
              }
              else{
                //ako nije defisan
              Flinkdata *newflink = new Flinkdata(operand,currentSection->getSize()+3,
                                                          currentSection->getId(),2);
              flink_table.push_back(newflink);
              ret += "0000";
                 
              }

            }
            else{ //simbol ne postoji u tabeli

                Symbol *symbol = new Symbol(operand,0,currentSection->getId(),"LOCAL",0);
                symbol_table.push_back(symbol);

                Flinkdata *newflink = new Flinkdata(operand,currentSection->getSize()+3,
                                                            currentSection->getId(),2);
                flink_table.push_back(newflink);
                ret += "0000";
            }

              //reloc zapis
            int idsymb = searchSymbolTable(operand)->getId();
            bool res = searchSymbolTable(operand)->isResovled();

            int addend = 0;
            if(res){
                if(searchSymbolTableID(idsymb)->getScope()=="GLOBAL" 
                  || searchSymbolTableID(idsymb)->getScope()=="EXTERN" ){
                    addend=0;
                  }
                  else{ //LOCAL
                    addend=searchSymbolTableID(idsymb)->getValue();
                  }
            }
            
            string type;
            if(searchSymbolTableID( idsymb )->getScope() == "LOCAL"){
             type ="loc_rel";
            }
            else 
              type = "glob_rel";
            
             Relocdata* rd= new Relocdata(currentSection->getId(),currentSection->getSize()+3,type,idsymb,searchSymbolTableID(idsymb)->getSection(),addend,res,"big","abs");
            currentSection->reloc_table.push_back(rd);

          }
        
  
  } else if(addr=="regdir"){
    ret += operand[1];
    ret+= "01";
    
  }
  else if(addr=="regindir"){
    operand = operand.substr(1,2);

    //cout<<"sad op =" <<operand<<endl;
   
    ret += operand[1];
    ret+= "02";

  } else if(addr=="regindpom"){
     
       operand = operand.substr(1,operand.length()-2);
       
      int pos = operand.find('+');
      if( pos == std::string::npos) return "error";

      string part1 = operand.substr(0,pos);
      string part2 = operand.substr(pos+1,operand.length()-pos-1);

        ret+=part1[1];
        ret+="03";

       if(isNumber(part2)){
        int val = stoul(part2,nullptr,0);
        ret += intHex(val);
        }
       else if(isSymbol(part2)){

            Symbol* s = searchSymbolTable(part2);
            
            if(s!=nullptr){ 
              //ako je vec definisan 
              if(s->isResovled()){
                //currentSection->addData(littleendian(intHex(s->getValue())));
                ret += intHex(s->getValue());
              }
              else{
                //ako nije defisan
              Flinkdata *newflink = new Flinkdata(part2,currentSection->getSize()+3,
                                                          currentSection->getId(),2);
              flink_table.push_back(newflink);
              ret += "0000";
                 
              }

            }
            else{ //simbol ne postoji u tabeli

                Symbol *symbol = new Symbol(part2,0,currentSection->getId(),"LOCAL",0);
                symbol_table.push_back(symbol);

                Flinkdata *newflink = new Flinkdata(part2,currentSection->getSize()+3,
                                                            currentSection->getId(),2);
                flink_table.push_back(newflink);
                ret += "0000";
            }

              //reloc zapis
            int idsymb = searchSymbolTable(part2)->getId();
            bool res = searchSymbolTable(part2)->isResovled();

            int addend = 0;
            if(res){
                if(searchSymbolTableID(idsymb)->getScope()=="GLOBAL" 
                  || searchSymbolTableID(idsymb)->getScope()=="EXTERN" ){
                    addend=0;
                  }
                  else{ //LOCAL
                    addend=searchSymbolTableID(idsymb)->getValue();
                  }
            }
            
            string type;
            if(searchSymbolTableID( idsymb )->getScope() == "LOCAL"){
             type ="loc_rel";
            }
            else 
              type = "glob_rel";
            
             Relocdata* rd= new Relocdata(currentSection->getId(),currentSection->getSize()+3,type,idsymb,searchSymbolTableID(idsymb)->getSection(),addend,res,"big","abs");
            currentSection->reloc_table.push_back(rd);


          }
      
     
  }


return ret;

}

 string Assembler::calculateSecondPartJump(string addr,string operand){

  string ret;

  if(addr == "neposredno"){
    ret+="f000";

    if(isNumber(operand)){
       int val = stoul(operand,nullptr,0);
       ret += intHex(val);

    }else if(isSymbol(operand)){

            Symbol* s = searchSymbolTable(operand);
            
            if(s!=nullptr){ 
              if(s->isResovled()){
                //currentSection->addData(littleendian(intHex(s->getValue())));
                ret += intHex(s->getValue());
              }
              else{
                //ako nije defisan
              Flinkdata *newflink = new Flinkdata(operand,currentSection->getSize()+3,
                                                          currentSection->getId(),2);
              flink_table.push_back(newflink);
              ret += "0000";
                 
              }

            }
            else{ //simbol postoji u tabeli

                Symbol *symbol = new Symbol(operand,0,currentSection->getId(),"LOCAL",0);
                symbol_table.push_back(symbol);

                Flinkdata *newflink = new Flinkdata(operand,currentSection->getSize()+3,
                                                            currentSection->getId(),2);
                flink_table.push_back(newflink);
                ret += "0000";
            }


            //reloc zapis
            int idsymb = searchSymbolTable(operand)->getId();
            bool res = searchSymbolTable(operand)->isResovled();

            int addend = 0;
            if(res){
                if(searchSymbolTableID(idsymb)->getScope()=="GLOBAL" 
                  || searchSymbolTableID(idsymb)->getScope()=="EXTERN" ){
                    addend=0;
                  }
                  else{ //LOCAL
                    addend=searchSymbolTableID(idsymb)->getValue();
                  }
            }
            
            string type;
            if(searchSymbolTableID( idsymb )->getScope() == "LOCAL"){
             type ="loc_rel";
            }
            else 
              type = "glob_rel";
            
             Relocdata* rd= new Relocdata(currentSection->getId(),currentSection->getSize()+3,type,idsymb,searchSymbolTableID(idsymb)->getSection(),addend,res,"big","abs");
            currentSection->reloc_table.push_back(rd);


          }

    

  }

  else if(addr=="pcrel"){
    ret+="f705";
    operand = operand.substr(1,operand.length()-1);

    if(isSymbol(operand)){

            Symbol* s = searchSymbolTable(operand);
            
            if(s!=nullptr){ 
              //ako je vec definisan 
              if(s->isResovled()){
                //currentSection->addData(littleendian(intHex(s->getValue())));
                ret += intHex(s->getValue());
              }
              else{
                //ako nije defisan
              Flinkdata *newflink = new Flinkdata(operand,currentSection->getSize()+3,
                                                          currentSection->getId(),2);
              flink_table.push_back(newflink);
              ret += "0000";
                 
              }

            }
            else{ //simbol postoji u tabeli

                Symbol *symbol = new Symbol(operand,0,currentSection->getId(),"LOCAL",0);
                symbol_table.push_back(symbol);

                Flinkdata *newflink = new Flinkdata(operand,currentSection->getSize()+3,
                                                            currentSection->getId(),2);
                flink_table.push_back(newflink);
                ret += "0000";
            }

             //reloc zapis
            int idsymb = searchSymbolTable(operand)->getId();
            bool res = searchSymbolTable(operand)->isResovled();

            int addend = -2;
            // if(res){
            //     if(searchSymbolTableID(idsymb)->getScope()=="GLOBAL" 
            //       || searchSymbolTableID(idsymb)->getScope()=="EXTERN" ){
            //         addend=0;
            //       }
            //       else{ //LOCAL
            //         addend=searchSymbolTableID(idsymb)->getValue();
            //       }
            // }
            
            string type;
            if(searchSymbolTableID( idsymb )->getScope() == "LOCAL"){
             type ="loc_rel";
            }
            else 
              type = "glob_rel";
            
             Relocdata* rd= new Relocdata(currentSection->getId(),currentSection->getSize()+3,type,idsymb,searchSymbolTableID(idsymb)->getSection(),addend,res,"big","rel");
            currentSection->reloc_table.push_back(rd);


          }

    

  }
  else if(addr=="memdir"){

    ret+="f004";

    operand = operand.substr(1,operand.length()-1);

    if(isNumber(operand)){
       int val = stoul(operand,nullptr,0);
       ret += intHex(val);
    }
   if(isSymbol(operand)){

            Symbol* s = searchSymbolTable(operand);
            
            if(s!=nullptr){ 
              //ako je vec definisan 
              if(s->isResovled()){
                //currentSection->addData(littleendian(intHex(s->getValue())));
                ret += intHex(s->getValue());
              }
              else{
                //ako nije defisan
              Flinkdata *newflink = new Flinkdata(operand,currentSection->getSize()+3,
                                                          currentSection->getId(),2);
              flink_table.push_back(newflink);
              ret += "0000";
                 
              }

            }
            else{ //simbol postoji u tabeli

                Symbol *symbol = new Symbol(operand,0,currentSection->getId(),"LOCAL",0);
                symbol_table.push_back(symbol);

                Flinkdata *newflink = new Flinkdata(operand,currentSection->getSize()+3,
                                                            currentSection->getId(),2);
                flink_table.push_back(newflink);
                ret += "0000";
            }

            //reloc zapis
            int idsymb = searchSymbolTable(operand)->getId();
            bool res = searchSymbolTable(operand)->isResovled();

            int addend = 0;
            if(res){
                if(searchSymbolTableID(idsymb)->getScope()=="GLOBAL" 
                  || searchSymbolTableID(idsymb)->getScope()=="EXTERN" ){
                    addend=0;
                  }
                  else{ //LOCAL
                    addend=searchSymbolTableID(idsymb)->getValue();
                  }
            }
            
            string type;
            if(searchSymbolTableID( idsymb )->getScope() == "LOCAL"){
             type ="loc_rel";
            }
            else 
              type = "glob_rel";
            
            Relocdata* rd= new Relocdata(currentSection->getId(),currentSection->getSize()+3,type,idsymb,searchSymbolTableID(idsymb)->getSection(),addend,res,"big","abs");
            currentSection->reloc_table.push_back(rd);


          }
        
  
  }
  else if(addr=="regdir"){
   
    operand = operand.substr(1,operand.length()-1);
    ret +="f";
    ret += operand[1];
    ret+= "01";

    
  }
  else if(addr=="regindir"){
    operand = operand.substr(2,2);

    ret +="f";
    ret += operand[1];
    ret+= "02";

  }
  else if(addr=="regindpom"){
      ret+= "f";

       operand = operand.substr(2,operand.length()-3);
       
      int pos = operand.find('+');
      if( pos == std::string::npos) return "error";

      string part1 = operand.substr(0,pos);
      string part2 = operand.substr(pos+1,operand.length()-pos-1);

        ret+=part1[1];
        ret+="03";

       if(isNumber(part2)){
        int val = stoul(part2,nullptr,0);
        ret += intHex(val);
        }
       else if(isSymbol(part2)){

            Symbol* s = searchSymbolTable(part2);
            
            if(s!=nullptr){ 
              //ako je vec definisan 
              if(s->isResovled()){
                //currentSection->addData(littleendian(intHex(s->getValue())));
                ret += intHex(s->getValue());
              }
              else{
                //ako nije defisan
              Flinkdata *newflink = new Flinkdata(part2,currentSection->getSize()+3,
                                                          currentSection->getId(),2);
              flink_table.push_back(newflink);
              ret += "0000";
                 
              }

            }
            else{ //simbol postoji u tabeli

                Symbol *symbol = new Symbol(part2,0,currentSection->getId(),"LOCAL",0);
                symbol_table.push_back(symbol);

                Flinkdata *newflink = new Flinkdata(part2,currentSection->getSize()+3,
                                                            currentSection->getId(),2);
                flink_table.push_back(newflink);
                ret += "0000";
            }


             //reloc zapis
            int idsymb = searchSymbolTable(part2)->getId();
            bool res = searchSymbolTable(part2)->isResovled();

            int addend = 0;
            if(res){
                if(searchSymbolTableID(idsymb)->getScope()=="GLOBAL" 
                  || searchSymbolTableID(idsymb)->getScope()=="EXTERN" ){
                    addend=0;
                  }
                  else{ //LOCAL
                    addend=searchSymbolTableID(idsymb)->getValue();
                  }
            }
            
           string type;
            if(searchSymbolTableID( idsymb )->getScope() == "LOCAL"){
             type ="loc_rel";
            }
            else 
              type = "glob_rel";
            
             Relocdata* rd= new Relocdata(currentSection->getId(),currentSection->getSize()+3,type,idsymb,searchSymbolTableID(idsymb)->getSection(),addend,res,"big","abs");
            currentSection->reloc_table.push_back(rd);



          }

          
  }
  

  return ret;

 }

string Assembler::mergeTokens(vector<string> tokens,int start){

  string ret;
  

  auto i = tokens.begin();
  advance(i,start);
  
  for( i ;i!=tokens.end();i++){
    ret = ret+ removeComma(*i);
  }

  return ret;


}

string Assembler::resolveOperandData(string operand){

  if(operand[0]=='$'){
    string rest = operand.substr(1,operand.length()-1);
    if(isSymbol(rest)) return "neposredno";
    if(isNumber(rest)) return "neposredno";
  }

  if(operand[0]=='%'){
    string rest = operand.substr(1,operand.length()-1);
    if(isSymbol(rest)) return "pcrel";
  }

   if(isNumber(operand)) return "memdir";
   if(isSymbol(operand)) return "memdir";
   if(isRegister(operand)) return "regdir";

   if( operand[0]=='[' &&  operand[operand.length()-1]==']'  ) {
             
      if(isRegister(operand.substr(1,operand.length()-2)) )  return "regindir";

      int pos = operand.find('+');
      if( pos == std::string::npos) return "error";

      string part1 = operand.substr(1,pos-1);
      string part2 = operand.substr(pos+1,operand.length()-pos-2);

      //cout<< part1<<" --- "<<part2<<cout;

      if(isRegister(part1) && isNumber(part2)) return "regindpom";
      if(isRegister(part1) && isSymbol(part2)) return "regindpom";

   }

   return "error";
  

}

string Assembler::resolveOperandJump(string operand){

  if(isNumber(operand)) return "neposredno";
  if(isSymbol(operand)) return "neposredno";

  if(operand[0]=='%'){
    string rest = operand.substr(1,operand.length()-1);
    if(isSymbol(rest)) return "pcrel";
  }

  if(operand[0]=='*'){
    string rest = operand.substr(1,operand.length()-1);


    if(isNumber(rest)) return "memdir";
    if(isSymbol(rest)) return "memdir";

    if(isRegister(rest)) return "regdir";


    if( rest[0]=='[' &&  rest[rest.length()-1]==']'  ) {
             
      if(isRegister(rest.substr(1,rest.length()-2)) )  return "regindir";


      int pos = rest.find('+');
      if( pos == std::string::npos) return "error";

      string part1 = rest.substr(1,pos-1);
      string part2 = rest.substr(pos+1,rest.length()-pos-2);

      //cout<< part1<<" --- "<<part2<<cout;

      if(isRegister(part1) && isNumber(part2)) return "regindpom";
      if(isRegister(part1) && isSymbol(part2)) return "regindpom";

     }

  }

  return "error";
}

int Assembler::strHexInt(string hexint){

  cout<<"val="<<hexint;
  
  return stoul(hexint,nullptr,0);
}

string Assembler::intHex(int num){

      stringstream ss;
      ss<<hex<<setw(4)<<setfill('0')<<num;
      return ss.str();

}

 string Assembler::littleendian(string hexnum){
  string ret="";
  ret = ret+ hexnum[2]+ hexnum[3]+ hexnum[0] +hexnum[1];
  return ret;
 }

//Symbols
void Assembler::resolveGlobalSymbols(string name,Section* section,int val){
    searchSymbolTable(name)->setValue(val);
    searchSymbolTable(name)->setSection(section->getId());
    searchSymbolTable(name)->setResolved(true);
}

void Assembler::resolveSymbols(string name,Section* section,int val){
  //resi tabelu simbola 
   searchSymbolTable(name)->setValue(val);
   searchSymbolTable(name)->setSection(section->getId());
   searchSymbolTable(name)->setResolved(true);


  for(auto fi = flink_table.begin(); fi!=flink_table.end(); fi++){
    if(name == (*fi)->getName()){
      searchSymbolTable(name)->setSection(currentSection->getId());
      searchSymbolTable(name)->setValue(val);
      searchSymbolTable(name)->setResolved(true);

      stringstream ss;
      ss<<hex<<setw(4)<<setfill('0')<<val;

      section->changeByte( ((*fi)->getLocation()*2),4,ss.str());


      (*fi)->setResolved(true);
    }
  }

  //resi sekcije pisi podatke
 


}

void Assembler::addSymbolValuetoSection(Symbol* symbol){
   
       stringstream ss;
       ss<<hex<<setw(4)<<setfill('0')<<symbol->getValue();

        currentSection->addData(ss.str());

      
}

bool Assembler::isSymbol(string word){
  if(!isNumber(word) && !isLabel(word) && !isRegister(word)){
      if(isalpha(word[0]) ){
         
          return word.find_first_of("`-=[];',.~()+{}:|<>?*&^%$#@!") == string::npos;
      }
      else return false;
      
  }
  else return false;
}

bool Assembler::isNumber(string word){

      string temp=word;
      if(temp[0]=='0'){
        if(temp[1]=='x' || temp[1]=='X'){
          //temp.erase(0,1);
          return temp.find_first_not_of("0123456789abcdefABCDEF",2) == string::npos;
          }
        }
      

   return word.find_first_not_of("0123456789") == string::npos;
}
bool Assembler::isLabel(string word){
   if( isalpha(word[0])  && word[word.length()-1]== ':') return true;
    else return false;
}
bool Assembler::isRegister(string word){
  if( word == "r0"|| word == "r1"|| word == "r2"|| word == "r3"||
      word == "r4"|| word == "r5"|| word == "r6"|| word == "r7" )
    return true;
        
  else
    return false;
}

bool Assembler ::checkComma(string word){
  if(word[word.length()-1]== ',') return true;
    else return false;
}
string Assembler::removeComma(string word){
  if (word[word.length()-1]== ',') return word.substr(0,word.length()-1);
  else return word;
}

string Assembler::removeLabel(string word){
  if (word[word.length()-1]== ':') return word.substr(0,word.length()-1);
  else return word;
}

Symbol* Assembler::searchSymbolTable(string name){


  for(int i=0; i <symbol_table.size(); i ++){
    if(symbol_table.at(i)->getName() == name){
      Symbol* temp = symbol_table[i];
      return temp;
    }
  }
  return NULL;

}

Symbol* Assembler::searchSymbolTableID(int id){

   for(int i=0; i <symbol_table.size(); i ++){
    if(symbol_table.at(i)->getId() == id){
      Symbol* temp = symbol_table[i];
      return temp;
    }
  }
  return NULL;

}


 void Assembler::print_SymbolTable(){

  cout<<endl;
  cout<<"-----------------Symbol table-------------------";
  cout<<endl;
  cout<<"Id   Name        Value     Section    Scope  R   ";
  cout<<endl;


  for( auto i = symbol_table.begin(); i != symbol_table.end(); i++ ){
    cout<< **i << endl;

  }

 }


 bool Assembler:: checkifResolved(string symbname){
    Symbol* mysymbol = searchSymbolTable(symbname);
    if(mysymbol){ //simbol postoji
      return mysymbol->isResovled();

    }
    return false;
 }


//Sections    

 Section* Assembler::getSectionbyId (int id){
  for(auto i =section_table.begin();i != section_table.end(); i++){
    if((*i)->getId()==id) return (*i);
  }
  return nullptr;
 }


void Assembler:: printfInfoSections(){
  for(auto i =section_table.begin();i != section_table.end(); i++){
    (*i)->printSectionInfo();
  }
}

Section* Assembler::searchSectionTable(string name){
  for(int i=0; i <section_table.size(); i ++){
    if(section_table.at(i)->getName() == name){
      Section* temp = section_table[i];
      return temp;
    }
  }
  return NULL;
}

void Assembler::openSection(string name){

  Section* res = searchSectionTable(name);

  if( res == NULL){
    Section *sect= new Section(name,0);
    section_table.push_back(sect);
    currentSection = sect;
  }
  else{
    // nastavi sekciju
    currentSection = res;
    

  }

}

 void Assembler::print_SectionTable(){

  cout<<endl;
  cout<<"-----------------Section table-------------------";
  cout<<endl;
  cout<<"Id    Name      Size  ";
  cout<<endl;

  for( auto i = section_table.begin(); i != section_table.end(); i++ ){
    cout<< **i << endl;
  }

  
 }

 void Assembler::print_Flinktable(){

  cout<<endl;
  cout<<"-----------------Flink table-------------------";
  cout<<endl;
  cout<<"Name      Location    idSec     num(B)  rf  resolved";
  cout<<endl;

   for( auto i = flink_table.begin(); i != flink_table.end(); i++ ){
    cout<< **i << endl;
  }

 }


 list<Flinkdata*> Assembler::getUnresolvedSymbolsfromFlink(string name){
    list<Flinkdata*> ret = list<Flinkdata*>();
    for(auto i = flink_table.begin();i!=flink_table.end();i++){
      if( (*i)->getResolved()==false && (*i)->getName()==name) 
        ret.push_back((*i));
    }
    return ret;
 }



 void Assembler::assembleLine(){
    string line = this->parser->getNextLine();
    vector<string> tokens = Lexer::getTokens(line);

    checkError(tokens);
    //Lexer::printTokens(line);

    //for( int i = 0; i<tokens.size(); i++){ // citaj tokene u jednoj liniji

      string curr = tokens[0];

      if(curr== ".global" || curr ==".extern"){ //moraju da budu na pocetku 

      try{
          if(currentSection!= section_table[0] && curr == ".extern"){
            throw("Syntax error near .extern");
          }

          for(int j = 1; j<tokens.size(); j ++){
            string name = tokens[j];
            name = removeComma(name);
            //provera da li je vec u tabeli simbola ako jeste greska

            if(searchSymbolTable(name)!= NULL){
               throw("Symbol already declared as global or extern");
            }

            Symbol* symbol;

            if(curr == ".global" ){
              symbol = new Symbol(name,0,currentSection->getId(),"GLOBAL",false);
              //Flinkdata* flink = new Flinkdata(symbol->getName(),currentSection->getSize()/2,
                //                               currentSection->getId(),2,0);

             // addSymbolValuetoSection(symbol);
              symbol_table.push_back(symbol);

              //flink_table.push_back(flink);

            }
              
            else {
              symbol = new Symbol(name,0,currentSection->getId(),"EXTERN",false);
             // addSymbolValuetoSection(symbol);
              symbol_table.push_back(symbol);
            }
              

           
           

           

          }


      }

      catch(const char* e){
        cout << e;
        exit(1);
        }

      }

      else if (curr == ".section"){
        
        try
        {
          if(tokens.size()!=2)
              throw("syntax error near .section");
          

        //zavrsi prethodnu sekciju

        //otvori novu
        
        string section_name = tokens[1];
        openSection(section_name);

        localcounter=currentSection->getSize()/2;

        
        }
        catch(const char* e){
          cout << e;
          exit(1);
        }

      }

      else if(curr == ".word") {

        for(int i = 1; i<tokens.size(); i++){

          if(isNumber(tokens[i])){
            int num =stoul(tokens[i],nullptr,0);


              currentSection->addData(littleendian(intHex(num)));
             
          } 

          else{ //simbol 


          if(isSymbol(tokens[i])){

            Symbol* s = searchSymbolTable(tokens[i]);
            
            if(s!=nullptr){ 
              //ako je vec definisan 
              if(s->isResovled()){
                currentSection->addData(littleendian(intHex(s->getValue())));
              }
              else{
                //ako nije defisan
              Flinkdata *newflink = new Flinkdata(tokens[i],currentSection->getSize(),
                                                          currentSection->getId(),2);
              flink_table.push_back(newflink);
              currentSection->addData("0000");
                 
              }

            }
            else{ //simbol postoji u tabeli

                Symbol *symbol = new Symbol(tokens[i],0,currentSection->getId(),"LOCAL",0);
                symbol_table.push_back(symbol);

                Flinkdata *newflink = new Flinkdata(tokens[i],currentSection->getSize(),
                                                            currentSection->getId(),2);
                flink_table.push_back(newflink);
                currentSection->addData("0000");
            }

            //reloc zapis
            int idsymb = searchSymbolTable(tokens[i])->getId();
            bool res = searchSymbolTable(tokens[i])->isResovled();

            int addend = 0;
            if(res){
                if(searchSymbolTableID(idsymb)->getScope()=="GLOBAL" 
                  || searchSymbolTableID(idsymb)->getScope()=="EXTERN" ){
                    addend=0;
                  }
                  else{ //LOCAL
                    addend=searchSymbolTableID(idsymb)->getValue();
                  }
            }
            
           string type;
            if(searchSymbolTableID( idsymb )->getScope() == "LOCAL"){
             type ="loc_rel";
            }
            else 
              type = "glob_rel";
            
             Relocdata* rd= new Relocdata(currentSection->getId(),currentSection->getSize()-2,type,idsymb,searchSymbolTableID(idsymb)->getSection(),addend,res,"little","abs");
            currentSection->reloc_table.push_back(rd);


          }


          }


        }
        localcounter+= 2;
        return;
      }

      else if(curr == ".skip"){
        string s  = tokens[1];
        int cnt = stoul(s,nullptr,0);

        for(int i=0; i<cnt ; i++){

          currentSection->addData("00");
          localcounter+= 1;
        }

       
        
        return;
      }

      else if (curr == ".end"){
        this->end = true;
      }
      




      else if( isLabel(curr)){ //LABELA

        try{
           
           

            //Symbol* s = searchSymbolTable(removeLabel(curr)); // samo prvi

            list<Flinkdata*> flist = getUnresolvedSymbolsfromFlink(removeLabel(curr));

            if(!flist.empty()){

                for(auto i =flist.begin();i!=flist.end();i++){
                  resolveSymbols(removeLabel(curr),getSectionbyId((*i)->getsectionNum()),localcounter);
                }

            }else{

                if(searchSymbolTable(removeLabel(curr))!=nullptr){
                    resolveGlobalSymbols(removeLabel(curr),currentSection,localcounter);
                }
                else{

                  Symbol* label = new Symbol(removeLabel(curr),localcounter,currentSection->getId(),"LOCAL",true);
                  //currentSection->addData(intHex(localcounter));
                  //addSymbolValuetoSection(label);
            
                  symbol_table.push_back(label);

                }

              

            }

              int id = searchSymbolTable(removeLabel(curr))->getId();
              int val = searchSymbolTable(removeLabel(curr))->getValue();
              //  updatuj reloc tabelu za svaku sekciju
            
              for(auto i =section_table.begin();i!=section_table.end();i++){

                for(auto j = (*i)->reloc_table.begin();j != (*i)->reloc_table.end(); j++) {
                     
                     if((*j)->getSymbolId()==id &&(*j)->getResolved()==false ){

                        if(searchSymbolTableID((*j)->getSymbolId())->getScope()=="LOCAL")
                            (*j)->setaddend(val);
                        else   
                            (*j)->setaddend(0);

                        (*j)->setsymbolsection ( searchSymbolTableID( (*j)->getSymbolId() )->getSection()   );

                        if(searchSymbolTableID( (*j)->getSymbolId() )->getScope() == "LOCAL"){
                          (*j)->setType("loc_rel");
                        }
                        else 
                          (*j)->setType("glob_rel");
                        
                        (*j)->setResolved(true);
                     }
                }
               
                }
            
           
            

           
            
        }
        catch(const char* e){
          cout << e;
          exit(1);
        }
       
      }














      //instrukcije
      else if( curr == "halt" ){ //duzina (1B)
       currentSection->addData("00");
       localcounter+=1;
      }
      else if( curr == "iret" ){ //duzina (1B)
        currentSection->addData("20");
        localcounter+=1;
      }
      else if( curr == "ret" ){ //duzina (1B)
        currentSection->addData("40");
        localcounter+=1;
      }
      else if( curr == "int" ){ //duzina (2B)

        try{
          if(!isRegister(tokens[1])){
              errors.push_back("Invalid register "+tokens[1]);
          };

        }
        catch(const char* e){
          cout << e;
          exit(1);
        }

        string value = "10";
        string reg = tokens[1];
        value = value+reg[1]+"f";
        currentSection->addData(value);
        localcounter+=2;
      }
      else if( curr == "push" ){ //duzina (3B)

       try{
          if(!isRegister(tokens[1])){
             errors.push_back("Invalid register "+tokens[1]);
          };

        }
        catch(const char* e){
          cout << e;
          exit(1);
        }

        string value = "b0";

        string reg = tokens[1];
        value = value+reg[1]+"612";
        currentSection->addData(value);
        localcounter+=3;
      }
      else if( curr == "pop" ){ //duzina (3B)

       try{
          if(!isRegister(tokens[1])){
              errors.push_back("Invalid register "+tokens[1]);
          };

        }
        catch(const char* e){
          cout << e;
          exit(1);
        }
        string value = "a0";

        string reg = tokens[1];
        value = value+reg[1]+"642";
        currentSection->addData(value);
        localcounter+=3;

      }
      else if( curr == "not" ){ //duzina (2B)
        string value = "80";

         try{
          if(!isRegister(tokens[1])){
              errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          string reg = tokens[1];
          value = value+reg[1] +"0";
          currentSection->addData(value);
          localcounter+=2;



      }
      else if( curr == "xchg" ){ //duzina (2B)

        string value = "60";

         try{
          if(!isRegister(removeComma(tokens[1]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          if(!isRegister(removeComma(tokens[2]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          value = value+ removeComma(tokens[1])[1] +removeComma(tokens[2])[1];
          currentSection->addData(value);
          localcounter+=2;

      }
      else if( curr == "add" ){ //duzina (2B)
        string value = "70";

         try{
          if(!isRegister(removeComma(tokens[1]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          if(!isRegister(removeComma(tokens[2]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          value = value+ removeComma(tokens[1])[1] +removeComma(tokens[2])[1];
          currentSection->addData(value);
          localcounter+=2;

      }
      else if( curr == "sub" ){ //duzina (2B)
        string value = "71";

         try{
          if(!isRegister(removeComma(tokens[1]))){
             errors.push_back("Invalid register "+tokens[1]);
          };

          if(!isRegister(removeComma(tokens[2]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          value = value+ removeComma(tokens[1])[1] +removeComma(tokens[2])[1];
          currentSection->addData(value);
          localcounter+=2;

      }
      else if( curr == "mul" ){ //duzina (2B)
        string value = "72";

         try{
          if(!isRegister(removeComma(tokens[1]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          if(!isRegister(removeComma(tokens[2]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          value = value+ removeComma(tokens[1])[1] +removeComma(tokens[2])[1];
          currentSection->addData(value);
          localcounter+=2;


      }
      else if( curr == "div" ){ //duzina (2B)
        string value = "73";

         try{
          if(!isRegister(removeComma(tokens[1]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          if(!isRegister(removeComma(tokens[2]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          value = value+ removeComma(tokens[1])[1] +removeComma(tokens[2])[1];
          currentSection->addData(value);
          localcounter+=2;

      }
      else if( curr == "cmp" ){ //duzina (2B)
        string value = "74";

         try{
          if(!isRegister(removeComma(tokens[1]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          if(!isRegister(removeComma(tokens[2]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          value = value+ removeComma(tokens[1])[1] +removeComma(tokens[2])[1];
          currentSection->addData(value);
          localcounter+=2;

      }
      else if( curr == "and" ){ //duzina (2B)
        string value = "81";

         try{
          if(!isRegister(removeComma(tokens[1]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          if(!isRegister(removeComma(tokens[2]))){
             errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          value = value+ removeComma(tokens[1])[1] +removeComma(tokens[2])[1];
          currentSection->addData(value);
          localcounter+=2;

      }
      else if( curr == "or" ){ //duzina (2B)
        string value = "82";

         try{
          if(!isRegister(removeComma(tokens[1]))){
             errors.push_back("Invalid register "+tokens[1]);
          };

          if(!isRegister(removeComma(tokens[2]))){
             errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          value = value+ removeComma(tokens[1])[1] +removeComma(tokens[2])[1];
          currentSection->addData(value);
          localcounter+=2;

      }
      else if( curr == "xor" ){ //duzina (2B)
        string value = "83";

         try{
          if(!isRegister(removeComma(tokens[1]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          if(!isRegister(removeComma(tokens[2]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          value = value+ removeComma(tokens[1])[1] +removeComma(tokens[2])[1];
          currentSection->addData(value);
          localcounter+=2;

      }
      else if( curr == "test" ){ //duzina (2B)
        string value = "84";

         try{
          if(!isRegister(removeComma(tokens[1]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          if(!isRegister(removeComma(tokens[2]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          value = value+ removeComma(tokens[1])[1] +removeComma(tokens[2])[1];
          currentSection->addData(value);
          localcounter+=2;

      }
      else if( curr == "shl" ){ //duzina (2B)
        string value = "90";

         try{
          if(!isRegister(removeComma(tokens[1]))){
             errors.push_back("Invalid register "+tokens[1]);
          };

          if(!isRegister(removeComma(tokens[2]))){
              errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          value = value+ removeComma(tokens[1])[1] +removeComma(tokens[2])[1];
          currentSection->addData(value);
          localcounter+=2;


      }
      else if( curr == "shr" ){ //duzina (2B)
        string value = "91";

         try{
          if(!isRegister(removeComma(tokens[1]))){
             errors.push_back("Invalid register "+tokens[1]);
          };

          if(!isRegister(removeComma(tokens[2]))){
            errors.push_back("Invalid register "+tokens[1]);
          };

          }
          catch(const char* e){
            cout << e;
            exit(1);
          }

          value = value+ removeComma(tokens[1])[1] +removeComma(tokens[2])[1];
          currentSection->addData(value);
          localcounter+=2;

      }

      else if( curr == "call" ){  //duzina (3-5 B)

          string operand = mergeTokens(tokens,1);

          string addr = resolveOperandJump(operand);

          string firstpart = "30";

          string secondpart = calculateSecondPartJump(addr, operand);

          //cout<< firstpart<<secondpart<<endl;

          string result;
          result+=firstpart;
          result+=secondpart;

          currentSection->addData(result);

          if(addr == "regdir" || addr== "regindir")
            localcounter+=3;
          else
            localcounter+=5;

      }
      else if( curr == "jmp" ){  //duzina (3-5 B)

      string operand = mergeTokens(tokens,1);

          string addr = resolveOperandJump(operand);

          string firstpart = "50";

          string secondpart = calculateSecondPartJump(addr, operand);

         // cout<< firstpart<<secondpart<<endl;

          string result;
          result+=firstpart;
          result+=secondpart;

          currentSection->addData(result);

          if(addr == "regdir" || addr== "regindir")
            localcounter+=3;
          else
            localcounter+=5;


      }
      else if( curr == "jeq" ){  //duzina (3-5 B)

          string operand = mergeTokens(tokens,1);

          string addr = resolveOperandJump(operand);

          string firstpart = "51";

          string secondpart = calculateSecondPartJump(addr, operand);

         // cout<< firstpart<<secondpart<<endl;

          string result;
          result+=firstpart;
          result+=secondpart;

          currentSection->addData(result);

          if(addr == "regdir" || addr== "regindir")
            localcounter+=3;
          else
            localcounter+=5;


      }
      else if( curr == "jne" ){  //duzina (3-5 B)

          string operand = mergeTokens(tokens,1);

          string addr = resolveOperandJump(operand);

          string firstpart = "52";

          string secondpart = calculateSecondPartJump(addr, operand);

         // cout<< firstpart<<secondpart<<endl;

          string result;
          result+=firstpart;
          result+=secondpart;

          currentSection->addData(result);

          if(addr == "regdir" || addr== "regindir")
            localcounter+=3;
          else
            localcounter+=5;


      }
      else if( curr == "jgt" ){  //duzina (3-5 B)

          string operand = mergeTokens(tokens,1);

          string addr = resolveOperandJump(operand);

          string firstpart = "53";

          string secondpart = calculateSecondPartJump(addr, operand);

          //cout<< firstpart<<secondpart<<endl;

          string result;
          result+=firstpart;
          result+=secondpart;

          currentSection->addData(result);

          if(addr == "regdir" || addr== "regindir")
            localcounter+=3;
          else
            localcounter+=5;


      }
      else if( curr == "ldr" ){  //duzina (3-5 B)

          if(!isRegister(tokens[1])){
            errors.push_back("Invalid register "+tokens[1]);
          }

          string operand = mergeTokens(tokens,2);

          string addr = resolveOperandData(operand);

          string firstpart = "a0";
          string reg = removeComma(tokens[1]);
          char secondpart = reg[1];

          string thirdpart = calculateSecondPartData(addr, operand);

          //cout<<"res ldr= "<< firstpart<<secondpart<<thirdpart<<endl;

          string result;
          result+=firstpart;
          result+=secondpart;
          result+=thirdpart;

          currentSection->addData(result);

          if(addr == "regdir" || addr== "regindir")
            localcounter+=3;
          else
            localcounter+=5;

      }
      else if( curr == "str" ){  //duzina (3-5 B)

          if(!isRegister(tokens[1])){
            errors.push_back("Invalid register "+tokens[1]);
          }

          string operand = mergeTokens(tokens,2);

          string addr = resolveOperandData(operand);

          string firstpart = "b0";
          string reg = removeComma(tokens[1]);
          char secondpart = reg[1];

          string thirdpart = calculateSecondPartData(addr, operand);

         // cout<<"res str= "<< firstpart<<secondpart<<thirdpart<<endl;

          string result;
          result+=firstpart;
          result+=secondpart;
          result+=thirdpart;

          currentSection->addData(result);

          if(addr == "regdir" || addr== "regindir")
            localcounter+=3;
          else
            localcounter+=5;
      } 
     
      
      if(!errors.empty()) error=true;

    //}
  }


  void Assembler::checkError(vector<string> tokens){

    int labelcnt=0;
    int dircnt=0;
    int instcnt=0;

    string label;

    string temp;

    for(auto i = 0; i< tokens.size(); i++){

      temp=tokens[i];
      if(
        tokens[i] == "halt"||tokens[i] == "iret"||tokens[i] == "ret"||
        tokens[i] == "int" ||tokens[i] == "push"||tokens[i] == "pop"||
        tokens[i] == "not" ||tokens[i] == "xchg"||tokens[i] == "add"||
        tokens[i] == "sub" ||tokens[i] == "mul" ||tokens[i] == "div"||
        tokens[i] == "cmp" ||tokens[i] == "and" ||tokens[i] == "or" ||
        tokens[i] == "xor" ||tokens[i] == "test"||tokens[i] == "shl"||
        tokens[i] == "shr" ||tokens[i] == "call"||tokens[i] == "jmp"||
        tokens[i] == "jeq" ||tokens[i] == "jne" ||tokens[i] == "jgt"||
        tokens[i] == "ldr" ||tokens[i] == "str"
        ){
          instcnt++;
          continue;
        }
        if(temp[0]=='.'){
          dircnt++;
          continue;
        }

        if(temp[temp.length()-1]== ':'){
          labelcnt++;
          label=temp;
        }
      
    }


    try{

      if(dircnt+instcnt > 1){
       throw("Fatal error: Only 1 directive or Instruction per line");
    }

    if(labelcnt> 1){
       throw("Fatal error: Only 1 Label per line");
    }

    if(labelcnt == 1 && label != tokens[0]){
        throw("Fatal error: Label must be at the start of the line");
    }

   

  
    }
    catch(const char* e){
      cout << e;
      exit(1);
      }

  }

  void Assembler::printErrors(){

    cout<<"---";

    for(auto i=errors.begin();i!=errors.end();i++){
      cout<<*i<<endl;
    }

    }



    void Assembler::writeFile(){

      *outputFile << "symbol_table_start"<<endl;

      for(auto i =symbol_table.begin(); i!=symbol_table.end();i++){

        *outputFile<<(*i)->stringSymbol()<<endl;

      }

      *outputFile << "symbol_table_end"<<endl;

      *outputFile << "section_table_start"<<endl;

       for(auto i =section_table.begin(); i!=section_table.end();i++){

        *outputFile<<(*i)->stringSection()<<endl;

      }



      *outputFile << "section_table_end"<<endl;

      *outputFile << "reloc_table_start"<<endl;

      for(auto i =section_table.begin(); i!=section_table.end();i++){

        for(auto j =(*i)->reloc_table.begin(); j!=(*i)->reloc_table.end();j++){

          *outputFile<<(*j)->stringRelocdata()<<endl;

        }

      }


      *outputFile << "reloc_table_end"<<endl;

      *outputFile << "data_start"<<endl;
      for(auto i =section_table.begin(); i!=section_table.end();i++){

        *outputFile<<(*i)->getId()<<" "<<(*i)->getData()<<endl;

      }

      *outputFile << "data_end"<<endl;


      



    }