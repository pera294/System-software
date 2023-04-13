#include "../inc/section.h"

int Section::cnt=0;

Section::Section( string name,int size){
  this->id=cnt++;
  this->name = name;
  this->size = size;
  this->reloc_table = vector<Relocdata*>();
}

Section::Section(int id,string name,int size){
  this->id=id;
  this->name = name;
  this->size = size;
  this->reloc_table = vector<Relocdata*>();

}

void Section::addData(string data){
  this->size = this->size + data.size()/2;
  this->data.append(data);
}


void Section::printSectionInfo(){

  cout<< endl;
  cout<< "Section info:" << this->name<<"  id="<<this->id;
  cout<<endl;
  cout<< "Section size is " << dec << this->size;
  cout<<endl;

  for(int i=0; i <data.length();i++){
    cout<<data[i];
    if(i%4==3) cout<<' ';
    if(i%16==15) cout<<endl;
  }

  
  if(!reloc_table.empty()){

    cout<<endl;
    cout<<"Relocation table:";
    cout<<endl;

    cout<<"Offset    Type  Symb_id  Addend  Resolved  ";
    cout<<endl;

    for( auto j = reloc_table.begin(); j != reloc_table.end(); j++ ){
        cout<< **j << endl;

  }
  
  cout<<endl;



  }
   
  
}  

/*
string Section::DataToString(){
 
  stringstream ss;

  for(auto i = data.begin(); i!=data.end(); i++){
    //cout<<setw(4)<<right<<setfill('0')<<static_cast<int> (*i);
    ss<<setw(4)<<setfill('0')<<hex<<static_cast<int> (*i);

     cout <<static_cast<int> (*i);
  }
  cout<<endl;
  return ss.str();
}*/

void Section::changeByte(int index,int len,string value){
    for(int i =0;i<len;i++){
      data[index+i] = value[i];
    }
}



string Section::stringSection(){
  string ret="";

  ret += to_string(id);
  ret += " ";
  ret += name;
  ret += " ";
  ret += to_string(size);
  return ret;
}