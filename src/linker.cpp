#include "../inc/linker.h"


Linker::Linker(ofstream* os,vector<string> inputFileNames){

  this->inputFileNames=inputFileNames;
  this->output_file=os;
  this->linklist = vector<Linkdata*>();
  this->infolist = vector<Info*>();
  this->globalSymbol_table = vector<Symbol*>();

  this->sectionOrder = vector<string>();

  this->finaldata ="";

}

void Linker::load(){

  int cnt=0;

  for(auto i = inputFileNames.begin(); i!= inputFileNames.end();i++){

      ifstream inputFile(*i);

      Parser * p = new Parser(&inputFile);
      //cout<<p->getNextLine();
 
      Linkdata* linkdata = new Linkdata(*i);

      string line = p->getNextLine(); // line = symbol_table_start
      line= p->getNextLine();
      // cout<<line<<endl;

    while(line != "symbol_table_end"){

       vector<string> tokens = Lexer::getTokens(line);
        bool r = (tokens[5]=="1");
        Symbol *symbol = new Symbol(stoi(tokens[0]),tokens[1],stoi(tokens[2]),stoi(tokens[3]),tokens[4],r);

          if(symbol->getScope()=="LOCAL"){
            string prefix =(*i).substr(0,(*i).length()-2); 
            prefix += "_";
            //cout<<" prefix = "<<prefix;
            symbol->setName(prefix + symbol->getName());
          }

       linkdata->symbol_table.push_back(symbol);

       line=p->getNextLine();

     }
      
     
      line=p->getNextLine(); //line = symbol_table_end
      line=p->getNextLine(); //line = section_table_start

      while(line != "section_table_end"){

        vector<string> tokens = Lexer::getTokens(line);
        Section * section = new Section(stoi(tokens[0]),tokens[1],stoi(tokens[2]));

        linkdata->section_table.push_back(section);
        line=p->getNextLine();

      }

      
      line=p->getNextLine(); 
      line=p->getNextLine(); // line = reloc_table_start
      
      while(line != "reloc_table_end"){

       vector<string> tokens = Lexer::getTokens(line);
       bool r = (tokens[5]=="1");
       Relocdata * relocdata = new Relocdata(stoi(tokens[0]),stoi(tokens[1]),tokens[2],stoi(tokens[3]),stoi(tokens[4]),stoi(tokens[5]),r,tokens[6],tokens[7]);

       linkdata->reloc_table.push_back(relocdata);
       line=p->getNextLine();

      }

      line=p->getNextLine(); // line = reloc_table_end
      line=p->getNextLine(); // line = data_start

      while(line != "data_end"){

       vector<string> tokens = Lexer::getTokens(line);

       int sectionid=stoi(tokens[0]);
       string data;

       if(tokens.size()==2)
           data = tokens[1];
        else 
           data="";   

        for(auto i = linkdata->section_table.begin(); i!= linkdata->section_table.end();i++){
            if(sectionid == (*i)->getId()){
            (*i)->setData(data);
            }
        }
      
        line=p->getNextLine();
    } 

    // line = data_end
    linklist.push_back(linkdata);
    cnt++;
  }

 }

void Linker::printAll(){

  cout<<endl;
  cout<< "Link list";
  cout<<endl;

  for(auto i = linklist.begin(); i!=linklist.end();i++){
    cout<<(**i)<<endl<<"---------------------"<<endl;
  }

}

void Linker::printAllInfo(){

  cout<<endl;
  cout<< "Info list:";
  cout<<endl;
  for(auto i = infolist.begin(); i!=infolist.end();i++){
    cout<< (*i)->filename<<"  "<<(*i)->sectionName<<"  "<<(*i)->sectionId<<"  "<<(*i)->location<<endl;
  }

  cout<<endl;
}

void Linker::printdata(){

  cout<<"Final data:";
  int cnt=0;

  for(int i=0; i <finaldata.length();i++){
   
    if(i%4==0) cout<<' ';
    if(i%16==0){
      cout<<endl;
      cout<<hex<<right<<setw(4)<<setfill('0')<<cnt<<": ";
      cnt= cnt+8;
    } 

    cout<<finaldata[i];
  }
  cout<<endl;
}



void Linker:: linkFiles() {

  list<string> remaining = list<string>();
  vector<string> mapped = vector<string>();

  for(auto i =linklist.begin(); i!= linklist.end();i++){
    for(auto j = (*i)->section_table.begin(); j!=(*i)->section_table.end(); j++){

      if(find(remaining.begin(),remaining.end(),(*j)->getName()) ==remaining.end()  ){
        remaining.push_back((*j)->getName());
        sectionOrder.push_back((*j)->getName());
      }
    }
  }


  // for(auto i =remaining.begin(); i!= remaining.end();i++){
  //   cout<<(*i)<<" ";
  // }

  //cout<<endl;

  string currentSection = remaining.front();
  remaining.pop_front();

  while(1){

    for(auto i =linklist.begin(); i!= linklist.end();i++){

      for(auto j = (*i)->section_table.begin(); j!=(*i)->section_table.end(); j++){

        if( (*j)->getName()== currentSection){

          Info* newinfo = new Info((*i)->filename,(*j)->getName(),(*j)->getId(),globalcounter);
          infolist.push_back(newinfo);

          finaldata+=(*j)->getData();
          globalcounter+= (*j)->getSize()*2;

        }

      }

    }

    if(remaining.empty()) break;
    
    currentSection = remaining.front();
    remaining.pop_front();

  }
}

void Linker :: createSymbolTable(){

  for(auto i =linklist.begin(); i!= linklist.end();i++){

    for(auto j = (*i)->symbol_table.begin(); j!=(*i)->symbol_table.end(); j++){

      if((*j)->getScope()!= "EXTERN"){
        globalSymbol_table.push_back((*j));

      }

    }

  }

  //Provera gresaka za global
  try{

    for(auto i = globalSymbol_table.begin(); i!= globalSymbol_table.end();i++){

     for(auto j = globalSymbol_table.begin(); j!= globalSymbol_table.end();j++){

        if(i == j) continue;

        if( (*i)->getScope()=="GLOBAL" && (*j)->getScope()=="GLOBAL" &&
            (*i)->getName() == (*j)->getName()  ){
              string msg = "Symbol "+ (*i)->getName()+ "already defined ";
              throw msg;
            }

        if( (*i)->getScope()=="EXTERN" && (*j)->getScope()=="GLOBAL" &&
            (*i)->getName() == (*j)->getName()  ){
              (*i)->setResolved(true);
            }    

     }

    }

    //Provera gresaka za external
    for(auto i = globalSymbol_table.begin(); i!= globalSymbol_table.end();i++){
        if((*i)->getScope()=="EXTERN" && (*i)->isResovled()==false){
            string msg = "Symbol "+ (*i)->getName()+ "not defined ";
            throw msg;
        }
    }



  }

  catch(const char* e){
            cout << e;
            exit(1);
  }
  

}


void Linker :: printSymbolTable(){

  cout<<endl;
  cout<< "Global Symbol Table";
  cout<<endl;

  for(auto i =globalSymbol_table.begin(); i!= globalSymbol_table.end();i++){
 //   if((*i)->getScope()=="GLOBAL")
      cout<<(**i)<<endl;
  }

   cout<<endl;

}



string Linker::findmyfile(string name){

  for(auto i =linklist.begin(); i!= linklist.end();i++){

     for(auto j = (*i)->symbol_table.begin(); j!=(*i)->symbol_table.end(); j++){

      if( (*j)->getName() == name && (*j)->getScope()!="EXTERN"){
        return (*i)->filename;
      }

     }
  

  }
  //greska
  return nullptr;
}

int Linker::calculateSymbolValue(Symbol* symbol){

  int oldvalue = symbol->getValue();
  int newvalue = 0;

  int start;

  string myfile = findmyfile(symbol->getName());

  for(auto i =infolist.begin(); i!= infolist.end();i++){

    if(myfile == (*i)->filename && symbol->getSection() ==(*i)->sectionId){
      start= (*i)->location/2;
    }
  }

  newvalue = start+oldvalue;


  return newvalue;
}



void Linker::updateSymbolTable(){

   int temp;

   for(auto i = globalSymbol_table.begin(); i!= globalSymbol_table.end();i++){
      temp = calculateSymbolValue((*i));
      (*i)->setValue(temp);

   }

}


int Linker::strHexInt(string hexint){

  //cout<<"val="<<hexint;
  
  return stoul(hexint,nullptr,0);
}

string Linker::intHex(int num){

      stringstream ss;
      ss<<hex<<setw(4)<<setfill('0')<<num;
      return ss.str();

}

 string Linker::littleendian(string hexnum){

  string ret="";
  ret = ret+ hexnum[2]+ hexnum[3]+ hexnum[0] +hexnum[1];

  return ret;
 }



void Linker::resolveRelocations(){

   string relocfile;
   int reloc_section_start;
   
   string symbolname;
   string symbolfile;
   int symbol_section_id;
   int symbol_section_start;

   int value;

    for(auto i =linklist.begin(); i!= linklist.end();i++){

      for(auto j = (*i)->reloc_table.begin(); j!=(*i)->reloc_table.end(); j++){


        relocfile=this->find_reloc_file((*j));
        //cout<<"relocfile:" << relocfile<<endl;

        reloc_section_start = this->find_smallSection_location(relocfile,(*j)->getmysection());

        //cout<<"reloc_section_start:" << reloc_section_start<<endl;

        symbolname=this->find_symbol_name(relocfile,(*j)->getSymbolId());
        //cout<<"symbolname:"<<symbolname<<endl;

        symbol_section_id= this->find_smallSection_where_is_symbol_defined(symbolname);
        //cout<<"symbol_section_id: "<<symbol_section_id<<endl;

        symbolfile=this->find_filename_where_is_symbol_defined(symbolname);

        //cout<<"symbol file: " <<symbolfile<<endl;

        symbol_section_start=this->find_smallSection_location(symbolfile,symbol_section_id);

         if( (*j)->getType()=="glob_rel"){

            if((*j)->getaddr()=="rel"){ //globalno pc relativno

          // value= -reloc_section_start+symbol_section_start+getSymbolValue(symbolname)-(*j)->getaddend()-1;
            value = symbol_section_start-reloc_section_start+getSymbolValue(symbolname)+(*j)->getaddend()+3;


          // cout<<"symbol name:" <<symbolname<<endl;
          // cout<<"symbol value:" <<dec<<getSymbolValue(symbolname)<<endl;
          // cout<<"addend:" <<dec<<(*j)->getaddend()<<endl;
          // cout<<"offset:" <<dec<<(*j)->getOffset()*2<<endl;
          // cout<<"reloc_section_start:" <<dec<<reloc_section_start<<endl; 
          // cout<<"symb_section_start:" <<dec<<symbol_section_start<<endl; 
          // cout<<dec<<"staro: "<<value<<endl;
          // cout<<dec<<"novo: "<<(getSymbolValue(symbolname) & (0xffff));

            value = (value & 0xFFFF);
            stringstream ss;
            string v;
            ss<<hex<<setw(4)<<setfill('0')<<value;
            v= ss.str();
            if((*j)->getendian()=="little"){
                v = littleendian(v);
            }
        
            changeByte(reloc_section_start+(*j)->getOffset()*2,4,v);
            
           }

          else{ //globalno apsolutno

            value = getSymbolValue(symbolname);
            stringstream ss;
            string v;
            ss<<hex<<setw(4)<<setfill('0')<<value;
            v= ss.str();

            if((*j)->getendian()=="little"){
                  v = littleendian(v);
              }
            
            changeByte(reloc_section_start+(*j)->getOffset()*2,4,v);

          }

        } else { //lokalno

            if((*j)->getaddr()=="rel"){

              // cout<<"symbol name:" <<symbolname<<endl;
              // cout<<"symbol value:" <<dec<<getSymbolValue(symbolname)<<endl;
              // cout<<"addend:" <<dec<<(*j)->getaddend()<<endl;
              // cout<<"offset:" <<dec<<(*j)->getOffset()*2<<endl;
              // cout<<"reloc_section_start:" <<dec<<reloc_section_start<<endl; 
              // cout<<"symb_section_start:" <<dec<<symbol_section_start<<endl; 

              value = symbol_section_start-reloc_section_start+getSymbolValue(symbolname)+(*j)->getaddend()+3;
              // value = getSymbolValue(symbolname);
              value = (value & 0xFFFF);

              //cout<<dec<<value<<"  "<<hex<<value<<endl;

              string v;

              stringstream ss;
              ss<<hex<<setw(4)<<setfill('0')<<value;
              v= ss.str();
               
              if((*j)->getendian()=="little"){
                v = littleendian(v);
              }

              changeByte(reloc_section_start+(*j)->getOffset()*2,4,v);


            }
            else{ //lokalno apsolutno

              // cout<<"symbol name:" <<symbolname<<endl;
              // cout<<"symbol value:" <<dec<<getSymbolValue(symbolname)<<endl;
              // cout<<"addend:" <<dec<<(*j)->getaddend()<<endl;
              // cout<<"offset:" <<dec<<(*j)->getOffset()*2<<endl;
              // cout<<"reloc_section_start:" <<dec<<reloc_section_start<<endl; 
              // cout<<"symb_section_start:" <<dec<<symbol_section_start<<endl; 
              value = getSymbolValue(symbolname);
              //cout<<dec<<value<<"  "<<hex<<value<<endl;

              string v;

              stringstream ss;
               ss<<hex<<setw(4)<<setfill('0')<<value;
              v= ss.str();
              
              if((*j)->getendian()== v){
                v = littleendian(v);
              }

              changeByte(reloc_section_start+(*j)->getOffset()*2,4,v);

            }

         }

      }
    }

}



void Linker::changeByte(int index,int len,string value){
    for(int i =0;i<len;i++){
      finaldata[index+i] = value[i];
    }
}


int Linker::getSymbolValue(string name){


  for(auto i = globalSymbol_table.begin(); i!= globalSymbol_table.end();i++){

    if((*i)->getName()==name && (*i)->getScope()!="EXTERN")
      return (*i)->getValue();

  }

  //greska
  return 0;

}


int Linker::find_smallSection_location(string filename,int sectionid){

  for(auto i = infolist.begin();i!=infolist.end();i++){

    if((*i)->filename== filename && (*i)->sectionId==sectionid){
      return (*i)->location;
    }

  }
  //error
  cout <<filename << " "<<sectionid<<endl;

  cout<<"error small section location"<<endl;
  return 0;

}



int Linker::find_smallSection_where_is_symbol_defined(string name){
  //ne moze da bude 0 jer je onda exterm

  for(auto i = linklist.begin();i!=linklist.end();i++){

    for(auto j = (*i)->symbol_table.begin();j!=(*i)->symbol_table.end();j++){

      if((*j)->isResovled()&& (*j)->getName()==name)
        return (*j)->getSection();

    }

  }
  
  cout<<"error small section where is symbol defined"<<endl;
  return 0;
}


string Linker::find_filename_where_is_symbol_defined(string name){

  for(auto i = linklist.begin();i!=linklist.end();i++){

    for(auto j = (*i)->symbol_table.begin();j!=(*i)->symbol_table.end();j++){

      if((*j)->isResovled()&&  (*j)->getName()==name)
        return (*i)->filename;

    }

  }
    
  cout<<"error find filename where symbol is defined" << name <<endl;
  return "";


}

string Linker::find_symbol_name(string filename,int symbid){
   for(auto i = linklist.begin();i!=linklist.end();i++){
      if((*i)->filename == filename){
        for(auto j = (*i)->symbol_table.begin();j!=(*i)->symbol_table.end();j++){

          if((*j)->getId()==symbid)
            return (*j)->getName();

        }

      }
    }

    cout<<"error find symbol name"<<endl;
    return "";

}


string Linker::find_reloc_file(Relocdata* pnt){

  for(auto i =linklist.begin(); i!= linklist.end();i++){

     for(auto j = (*i)->reloc_table.begin(); j!=(*i)->reloc_table.end(); j++){

        if((*j)==pnt)
          return (*i)->filename;

     }
  
  }

  //greska
  cout<<"error find reloc file"<<endl;
  return "error";
}

void Linker::writeFile(){
   *output_file<<finaldata;
}