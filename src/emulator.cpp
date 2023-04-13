#include "../inc/emulator.h"


int Emulator::MEMORY_SIZE = 1<<16;
int Emulator::MEMORY_MAPPED_REGISTERS = 0xFF00;

Emulator::Emulator(string inputFilename){

  ifstream inputFile(inputFilename);
  getline(inputFile,this->code); 
  inputFile.close();
  this->memory = vector<uint8_t>();

}

void Emulator::init(){

int memcnt = 0;
string curr;

//cout<<"code:"<<this->code;

// for(int i=0; i<MEMORY_SIZE;i++){
//   memory.push_back(0);
// }

  memory = this->string_to_vector(code);

 

 for(int j=0; j<8;j++){
   registers[j]=0;
 }



// for(int i=0; i<memory.size();i++){
//   cout <<hex<<setfill('0')<<(memory[i]);
// }

registers[6]= MEMORY_MAPPED_REGISTERS; //sp 
registers[7] = (short)(memory[1] << 8) + (short)memory[0];//pc 



//cout<<"init end"<<endl;

}

void Emulator::push_psw(){
  short sp = registers[6];
  short psw_dummy = psw;
   //cout<<"psw pushed: "<< psw_dummy<<endl;
  sp=sp-2;

  memory[sp]=(uint8_t)((psw_dummy)&0xff);
  memory[sp+1]=(uint8_t)((psw_dummy>>8)&0xff);
  
  
  registers[6]= registers[6]-2;
}
void Emulator::pop_psw(){
 
  short sp = registers[6];
  short temp;

  temp = memory[sp+1];
  temp = temp<<8;
  temp =temp | memory[sp];

  registers[6]= registers[6]+2;
  //cout<<"psw pop: "<< temp <<endl;
  psw=temp;

}

void Emulator::push_pc(){
  short sp = registers[6];
  short pc_dummy = registers[7];
  sp=sp-2;
   //cout<<"pc pushed: "<< pc_dummy<<endl;

  memory[sp]=(uint8_t)((pc_dummy)&0xff);
  memory[sp+1]=(uint8_t)((pc_dummy>>8)&0xff);
  
 
  registers[6]= registers[6]-2;
}
void Emulator::pop_pc(){

  short sp = registers[6];
  short temp;

  temp = memory[sp+1];
  temp = temp<<8;
  temp = temp | memory[sp];

  registers[6]= registers[6]+2;

  //cout<<"pc pop: "<< temp<<endl;
  registers[7]=temp;

}



void Emulator:: emulation(){

  while(!end){

    uint8_t curr = memory[registers[7]];
      //cout<<"pc= "<< dec<< registers[7]<<endl;
     // cout<<"mem[pc]= "<< dec<< (short)curr<<endl;
    

    switch(curr){

      case 0x00: { //halt
        end=true;
        registers[7]+=1;
        break;
      }

      case 0x10: { //int
        //cout<<endl<<"int start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];

        //cout<<"byte2"<<(short)memory[registers[7]+1]<< endl;
        int regnum = (regsdesc & 0xf0)>>4;
        // cout<<"regnum = "<<regnum<<endl;
        // cout<<"reg[regnum]= "<<(short)registers[regnum]<<endl;

        uint8_t low = memory[ (registers[regnum]%8) *2];
        uint8_t high = memory[ (registers[regnum]%8)*2 +1];
        //  cout<<"low = "<<(short)low<<endl;
        //  cout<<"high = "<<(short)high<<endl;

        short adress = (high<<8) | low;


        registers[7]+=2;
        push_pc();
        push_psw();


        registers[7] = adress;
        //cout<<(short)registers[7];
       // cout<<endl<<"start end"<<endl;
       

        break;
      }

      case 0x20: { //iret
        //cout<<endl<<"iret start"<<endl;
      
        pop_psw();
        pop_pc();
        //cout<<endl<<"iret end"<<endl;

        break;
      }

      case 0x30: { //call
         //cout<<endl<<"call start"<<endl;
        
        uint8_t byte2 = memory[registers[7]+1];
        uint8_t byte3 = memory[registers[7]+2];
        uint8_t byte4;
        uint8_t byte5;

        int addrmode = getAdressJmp(byte2,byte3);
        short value;

      

        switch(addrmode){
          case 0:{//neposredno
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
            value = (byte4<<8) | byte5;
            registers[7]+=5;
            
            break;
          }
          case 1:{//regdir
            int regnum = byte2 & 0x0f;
            value = registers[regnum];
            registers[7]+=3;
            
            break;
          }
          case 2:{//regind
            int regnum = byte2 & 0x0f;
            value = memory [ registers[regnum] ];
            registers[7]+=3;
            break;
          }
          case 3:{//regindpom
            int regnum = byte2 & 0x0f;
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];

            short temp = (byte4<<8) | byte5;

            value = memory [ registers[regnum] + temp ];
            registers[7]+=5;
            break;
          }
          case 4:{//memdir
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
            int memloc =  (byte4<<8) | byte5;
            value = memory[memloc];
            registers[7]+=5;

            break;
          }
          case 5:{//pcrel
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];

            registers[7]+=5; 
            value = ((byte4<<8) | byte5) +registers[7]; 
          
  
            break;
          }


        }

        push_pc();
        registers[7]=value;
        //cout<<endl<<"call end"<<endl;
        //exit(1);

        break;
      }


      case 0x40: { //ret
        //cout<<endl<<"ret start"<<endl;
        pop_pc();
        //cout<<endl<<"ret end"<<endl;

        break;
      }

      case 0x50: { //jmp

        
        //cout<<endl<<"jmp start"<<endl;
        uint8_t byte2 = memory[registers[7]+1];
        uint8_t byte3 = memory[registers[7]+2];
        uint8_t byte4;
        uint8_t byte5;

        // cout<<"byte2 = "<<(short)byte2<<endl;
        // cout<<"byte3 = "<<(short)byte3<<endl;

       

        int addrmode = getAdressJmp(byte2,byte3);
        short value;

        //cout<<"addr mode = "<<addrmode<<endl;

        switch(addrmode){
          case 0:{//neposredno
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
            value = (byte4<<8) | byte5;
            registers[7]+=5;
            
            break;
          }
          case 1:{//regdir
            int regnum = byte2 & 0x0f;
            value = registers[regnum];
            registers[7]+=3;
            
            break;
          }
          case 2:{//regind
            int regnum = byte2 & 0x0f;
            value = memory [ registers[regnum] ];
            registers[7]+=3;
            break;
          }
          case 3:{//regindpom
            int regnum = byte2 & 0x0f;
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];

            short temp = (byte4<<8) | byte5;

            value = memory [ registers[regnum]+ temp ] ;
            registers[7]+=5;
            break;
          }
          case 4:{//memdir
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
            int memloc =  (byte4<<8) | byte5;
            value = memory[memloc];
            registers[7]+=5;

            break;
          }
          case 5:{//pcrel
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];

            registers[7]+=5; 
            value = ((byte4<<8) | byte5) + registers[7]; 
            
            
            break;
          }


        }

        //push_pc();
        registers[7]=value;
        //cout<<"pc=" <<registers[7]<<endl;
        //cout<<endl<<"jmp end"<<endl;

        break;
      }

      case 0x51: { //jeq
         //cout<<endl<<"jeq start"<<endl;
        uint8_t byte2 = memory[registers[7]+1];
        uint8_t byte3 = memory[registers[7]+2];
        uint8_t byte4;
        uint8_t byte5;

        int addrmode = getAdressJmp(byte2,byte3);
        short value;

        switch(addrmode){
          case 0:{//neposredno
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
            value = (byte4<<8) | byte5;
            registers[7]+=5;
            
            break;
          }
          case 1:{//regdir
            int regnum = byte2 & 0x0f;
            value = registers[regnum];
            registers[7]+=3;
            
            break;
          }
          case 2:{//regind
            int regnum = byte2 & 0x0f;
            value = memory [ registers[regnum] ];
            registers[7]+=3;
            break;
          }
          case 3:{//regindpom
            int regnum = byte2 & 0x0f;
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];

            short temp = (byte4<<8) | byte5;

            value = memory [ registers[regnum] + temp];
            registers[7]+=5;
            break;
          }
          case 4:{//memdir
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
            int memloc =  (byte4<<8) | byte5;
            value = memory[memloc];
            registers[7]+=5;

            break;
          }
          case 5:{//pcrel
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
             registers[7]+=5;
            value = ((byte4<<8) | byte5) +registers[7]; 
           
            
            break;
          }


        }

        if((psw & 0x0001) ==0){
          registers[7]=value;
        }
        //cout<<endl<<"jeq end"<<endl;
        break;
      }

      case 0x52: { //jne
         //cout<<endl<<"jne start"<<endl;
        uint8_t byte2 = memory[registers[7]+1];
        uint8_t byte3 = memory[registers[7]+2];
        uint8_t byte4;
        uint8_t byte5;

        int addrmode = getAdressJmp(byte2,byte3);
        short value;

        switch(addrmode){
          case 0:{//neposredno
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
            value = (byte4<<8) | byte5;
            registers[7]+=5;
            
            break;
          }
          case 1:{//regdir
            int regnum = byte2 & 0x0f;
            value = registers[regnum];
            registers[7]+=3;
            
            break;
          }
          case 2:{//regind
            int regnum = byte2 & 0x0f;
            value = memory [ registers[regnum] ];
            registers[7]+=3;
            break;
          }
          case 3:{//regindpom
            int regnum = byte2 & 0x0f;
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];

            short temp = (byte4<<8) | byte5;

            value = memory [ registers[regnum]+ temp ];
            registers[7]+=5;
            break;
          }
          case 4:{//memdir
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
            int memloc =  (byte4<<8) | byte5;
            value = memory[memloc];
            registers[7]+=5;

            break;
          }
          case 5:{//pcrel
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
            registers[7]+=5;
            value = ((byte4<<8) | byte5) +registers[7]; 
            
            
            break;
          }


        }

        if((psw & 0x0001) !=0){
          registers[7]=value;
        }
       
        break;
      }

      case 0x53: { //jgt
        //cout<<endl<<"jgt start"<<endl;
        uint8_t byte2 = memory[registers[7]+1];
        uint8_t byte3 = memory[registers[7]+2];
        uint8_t byte4;
        uint8_t byte5;

        int addrmode = getAdressJmp(byte2,byte3);
        short value;

        switch(addrmode){
          case 0:{//neposredno
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
            value = (byte4<<8) | byte5;
            registers[7]+=5;
            
            break;
          }
          case 1:{//regdir
            int regnum = byte2 & 0x0f;
            value = registers[regnum];
            registers[7]+=3;
            
            break;
          }
          case 2:{//regind
            int regnum = byte2 & 0x0f;
            value = memory [ registers[regnum] ];
            registers[7]+=3;
            break;
          }
          case 3:{//regindpom
            int regnum = byte2 & 0x0f;
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];

            short temp = (byte4<<8) | byte5;

            value = memory [ registers[regnum]+ temp ];
            registers[7]+=5;
            break;
          }
          case 4:{//memdir
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
            int memloc =  (byte4<<8) | byte5;
            value = memory[memloc];
            registers[7]+=5;

            break;
          }
          case 5:{//pcrel
            byte4 = memory[registers[7]+3];
            byte5 = memory[registers[7]+4];
            registers[7]+=5;
            value = ((byte4<<8) | byte5) +registers[7]; 
            
            
            break;
          }


        }

        if((psw & 0x0001) ==0 && (psw & 0x0002) ==0 && (psw & 0x0004) ==0 ){
          registers[7]=value;
        }
         //cout<<endl<<"jgt wns"<<endl;
        break;

      }

      case 0x60: { //xchg
        //cout<<endl<<"xchg start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];


        uint8_t regS = regsdesc & 0x0f;
        uint8_t regD = (regsdesc>>4) & 0x0f;
        

        uint8_t temp = registers[regD];
        registers[regD] = registers[regS];
        registers[regS]=temp;

        registers[7]+=2;
        //cout<<endl<<"xchg start"<<endl;
        break;
      }

      case 0x70: { //add
        //cout<<endl<<"add start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];

        uint8_t regS = regsdesc & 0x0f;
        uint8_t regD = (regsdesc>>4) & 0x0f;
        
        // cout<<"regD="<<(short)regD<<endl;
        // cout<<"regs="<<(short)regS<<endl;
        
        registers[regD] = registers[regD]+registers[regS];
        //cout<< "add = "<< (short)registers[regD]<<endl;
       
        registers[7]+=2;
         //cout<<endl<<"add end"<<endl;
        break;
        
      }

      case 0x71: { //sub
        //cout<<endl<<"sub start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];

        uint8_t regS = regsdesc & 0x0f;
        uint8_t regD = (regsdesc>>4) & 0x0f;
        
        // cout<<"regD="<<(short)regD<<endl;
        // cout<<"regs="<<(short)regS<<endl;
        
        registers[regD] = registers[regD]-registers[regS];
        //cout<< "sub = "<< (short)registers[regD]<<endl;
        
       
        registers[7]+=2;
         //cout<<endl<<"sub end"<<endl;

        break;
      }

      case 0x72: { //mul
        //cout<<endl<<"mul start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];

        uint8_t regS = regsdesc & 0x0f;
        uint8_t regD = (regsdesc>>4) & 0x0f;
        
        // cout<<"regD="<<(short)regD<<endl;
        // cout<<"regs="<<(short)regS<<endl;
        
        registers[regD] = registers[regD]*registers[regS];
        //cout<< "mul = "<< (short)registers[regD]<<endl;
       
        registers[7]+=2;
        //cout<<endl<<"mul end"<<endl;
        break;
      }

      case 0x73: { //div
        //cout<<endl<<"div start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];

        uint8_t regS = regsdesc & 0x0f;
        uint8_t regD = (regsdesc>>4) & 0x0f;
        
        // cout<<"regD="<<(short)regD<<endl;
        // cout<<"regs="<<(short)regS<<endl;
        
        registers[regD] = registers[regD]/registers[regS];
        //cout<< "div = "<< (short)registers[regD]<<endl;
       
        registers[7]+=2;
        //cout<<endl<<"div end"<<endl;
        break;
      }

      case 0x74: { //cmp
        //cout<<endl<<"cmp start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];

        uint8_t regD = (regsdesc>>4) & 0x0f;
        uint8_t regS = regsdesc & 0x0f;

        if(registers[regD] == registers[regS]) {
          set_Z();
          clear_O();
          clear_N();
          clear_C();

        } else if(registers[regD]< registers[regS]){
          clear_Z();

          
          if((registers[regD] - registers[regS])>0xffff){
            set_O();
          }else{
            clear_O();
          }

          set_N();
          set_C();


        }else if(registers[regD]>registers[regS]){
          clear_Z();
          clear_O();
          clear_N();
          clear_C();

        }

        
        //registers[regD] = registers[regD]-registers[regS];

        registers[7]+=2;
        // cout<<endl<<"cmp end"<<endl;
        break;
      }

      case 0x80: { //not
         //cout<<endl<<"not start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];

        uint8_t regD = (regsdesc>>4) & 0x0f;
        uint8_t regS = regsdesc & 0x0f;

        registers[regD] = ~registers[regD];

        registers[7]+=2;
        // cout<<endl<<"not end"<<endl;

        break;
      }

      case 0x81: { //and
        //cout<<endl<<"and start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];

        uint8_t regD = (regsdesc>>4) & 0x0f;
        uint8_t regS = regsdesc & 0x0f;

        
        registers[regD] = registers[regD] & registers[regS];
       
        registers[7]+=2;
        // cout<<endl<<"and end"<<endl;

        break;
      }

      case 0x82: { //or
       //cout<<endl<<"or start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];

        uint8_t regD = (regsdesc>>4) & 0x0f;
        uint8_t regS = regsdesc & 0x0f;

        
        registers[regD] = registers[regD] | registers[regS];
       
        registers[7]+=2;
         //cout<<endl<<"or end"<<endl;

        break;
      }

      case 0x83: { //xor
       //cout<<endl<<"xor start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];

        uint8_t regD = (regsdesc>>4) & 0x0f;
        uint8_t regS = regsdesc & 0x0f;

        
        registers[regD] = registers[regD] ^ registers[regS];
       
        registers[7]+=2;
        //cout<<endl<<"xor end"<<endl;
        break;
      }

      case 0x84: { //test
       //cout<<endl<<"test start"<<endl;

        uint8_t regsdesc = memory[registers[7]+1];

        uint8_t regD = (regsdesc>>4) & 0x0f;
        uint8_t regS = regsdesc & 0x0f;

        if((registers[regD] & registers[regS]) == 0){
          set_Z();
        } else {
          clear_Z();
        }

        if(0x8000 & ((registers[regD] & registers[regS])) !=0){
          set_N();
        }else{
          clear_N();
        }

        registers[regD] = registers[regD] & registers[regS];
       
        registers[7]+=2;

         //cout<<endl<<"test end"<<endl;
        break;
      }

      case 0x90: { //shl
       //cout<<endl<<"shl start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];

        uint8_t regD = (regsdesc>>4) & 0x0f;
        uint8_t regS = regsdesc & 0x0f;

        if((registers[regD] << registers[regS])== 0){
          set_Z();
        }else{
          clear_Z();
        }

         if(0x8000 & ((registers[regD] << registers[regS])) !=0){
          set_N();
        }else{
          clear_N();
        }

        if(0x8000 & ((registers[regD] << registers[regS]-1)) !=0){
          set_C();
        }else{
          clear_C();
        }

        
        registers[regD] = registers[regD] << registers[regS];
       
        registers[7]+=2;
        // cout<<endl<<"shl end"<<endl;

        break;
      }

      case 0x91: { //shr
         //cout<<endl<<"shr start"<<endl;
        uint8_t regsdesc = memory[registers[7]+1];

        uint8_t regD = (regsdesc>>4) & 0x0f;
        uint8_t regS = regsdesc & 0x0f;

        if((registers[regD] >> registers[regS])== 0){
          set_Z();
        }else{
          clear_Z();
        }

         if(0x8000 & ((registers[regD] >> registers[regS])) !=0){
          set_N();
        }else{
          clear_N();
        }

        if(0x8000 & ((registers[regD] >> registers[regS]-1)) !=0){
          set_C();
        }else{
          clear_C();
        }

        
        registers[regD] = registers[regD] >> registers[regS];
       
        registers[7]+=2;
         //cout<<endl<<"shr end"<<endl;
        break;
      }

      case 0xa0: { //pop i ldr

        uint8_t byte2 = memory[registers[7]+1];
        uint8_t byte3 = memory[registers[7]+2];
        uint8_t byte4;
        uint8_t byte5;

        if(byte3 == 0x42){ //pop
         //cout<<endl<<"pop start"<<endl;
          int regnum = (byte2>>4) & 0x0f;
          short value = (memory[registers[6]+1] << 8) | memory[registers[6]];

          registers[regnum]=value;

           //cout<<endl<<"value poped: "<<(short)value<<endl;

          registers[6]+=2;
          registers[7]+=3;
           //cout<<endl<<"pop end"<<endl;

        }else{ //ldr
          //cout<<endl<<"ldr start"<<endl;

          int addrmode = getAdressData(byte2,byte3);
          short value;

          

          switch(addrmode){

            case 0:{ //neposredno
              byte4 = memory[registers[7]+3];
              byte5 = memory[registers[7]+4];
              value = (byte4<<8) | byte5;
              
              //cout<<"ldr imm value = "<<value<<endl;

              int regnumD = (byte2>>4) & 0x0f;
              registers[regnumD] = value;


              registers[7]+=5;
              break;
            }
            case 1:{ //regdir

              int regnumD = (byte2>>4) & 0x0f;
              int regnumR = (byte2) & 0x0f;
              registers[regnumD] = registers[regnumR];
            
              registers[7]+=3;
              break;
            }

            case 2:{ //regind
              int regnumR = (byte2) & 0x0f;
              int regnumD = (byte2>>4) & 0x0f;
            

              registers[regnumD] = memory[registers[regnumR]];

              //cout<<"ldr regind = "<<registers[regnumD] <<endl;
              registers[7]+=3;
              break;
            }

            case 3:{ //regindpom
              byte4 = memory[registers[7]+3];
              byte5 = memory[registers[7]+4];

              int regnumR = (byte2) & 0x0f;
              int regnumD = (byte2>>4) & 0x0f;

              // cout<<"regnuR = "<<regnumR<<endl;
              // cout<<"regnuD = "<<regnumD<<endl;

              short memloc = (byte4<<8) | byte5;

              //cout<<"memloc"<<memloc<<endl;

              short low  =  memory[registers[regnumR] +memloc];
              short high =  memory[registers[regnumR] +memloc+1];

              registers[regnumD]= (high<<8) |low;
              //cout<<"ldr regindpom value:"<<(short)registers[regnumD]<<endl;

              registers[7]+=5;
              break;
            }

            case 4:{ //memdir
              byte4 = memory[registers[7]+3];
              byte5 = memory[registers[7]+4];
              int regnumD = (byte2>>4) & 0x0f;
              value = (byte4<<8) | byte5;

              uint8_t high = memory[value + 1];
              uint8_t low = memory[value];

              // cout << "high = "<<(short)high<<endl;
              // cout << "low = "<<(short)low<<endl;


              registers[regnumD] = (high << 8) | low;

              registers[7]+=5;
              break;
            }

            case 5:{//pcrel
              byte4 = memory[registers[7]+3];
              byte5 = memory[registers[7]+4];
              int regnumD = (byte2>>4) & 0x0f;
             
              value = (byte4<<8) + byte5 +registers[7]; //???????
              registers[regnumD] =value;
              registers[7]+=5;
              break;
            }

          }




           //cout<<endl<<"ldr end"<<endl;
        }


        

        break;
      }

      case 0xb0: { //push i str
        uint8_t byte2 = memory[registers[7]+1];
        uint8_t byte3 = memory[registers[7]+2];
        uint8_t byte4;
        uint8_t byte5;

      if(byte3 == 0x12){ //push
          //cout<<endl<<"push start"<<endl;
          
          int regnum = (byte2>>4) & 0x0f;
          short value = registers[regnum];
          

          registers[6]= registers[6]-2;
          short sp = registers[6];

          uint8_t low = (value)&0xff;
          uint8_t high = (value>>8)&0xff;

           //cout<<endl<<"value pushed: "<<(short)high << (short)low<<endl;
          
          memory[sp+1]=high;
          memory[sp]=low;
          
          registers[7]+=3;
          // cout<<endl<<"push end"<<endl;

        }else{ //str
          //cout<<endl<<"str start"<<endl;
          

          int addrmode = getAdressData(byte2,byte3);
          //cout<< "addrmode = "<<addrmode<<endl;
           
          short value;

          switch(addrmode){

            case 0:{ //neposredno
              cout<<"Error";
              exit(1);
              
              break;
            }
            case 1:{ //regdir
              int regnumR = (byte2) & 0x0f;
              int regnumD = (byte2>>4) & 0x0f;
              
              memory[registers[regnumD] ] = registers[regnumR];
              registers[7]+=3;

              break;
            }

            case 2:{ //regind
              //cout<<"regind"<<endl;
              int regnumR = (byte2) & 0x0f;
              int regnumD = (byte2>>4) & 0x0f;

              // cout<<"regnumR = "<<regnumR<<endl;
              // cout<<"regnumD = "<<regnumD<<endl;

              // cout<<"registers[regnumD] = "<<registers[regnumD]<<endl;
              // cout<<"registers[regnumR] = "<<registers[regnumR]<<endl;

              unsigned short value = registers[regnumD];
              
              
              uint8_t low = (value)&0xff;
              uint8_t high = (value>>8)&0xff;

              memory[registers[regnumR] ] = low;
              memory[registers[regnumR]+1 ] = high;


              registers[7]+=3;

              // cout<< (short)memory[registers[regnumR]]<<endl;
              // cout<< (short)memory[registers[regnumR]+1]<<endl;
              //cout<< "str end"<<endl;
              
              break;
            }

            case 3:{ //regindpom
              int regnumR = (byte2) & 0x0f;
              int regnumD = (byte2>>4) & 0x0f;

              byte4 = memory[registers[7]+3];
              byte5 = memory[registers[7]+4];
              int pom = (byte4<<8) | byte5;

              unsigned short value = registers[regnumD]+pom;
              uint8_t low = (value)&0xff;
              uint8_t high = (value>>8)&0xff;

              memory[registers[regnumR] ] = low;
              memory[registers[regnumR]+1 ] = high;


              registers[7]+=3;
              break;
            }

            case 4:{ //memdir
              byte4 = memory[registers[7]+3];
              byte5 = memory[registers[7]+4];
              int regnumD = (byte2>>4) & 0x0f;
              value = (byte4<<8) | byte5;

              //cout << "str value = "<<(short)value<<endl;

              uint8_t low =registers[regnumD] & 0xff;
              uint8_t high = (registers[regnumD]>>8)& 0xff;

              // cout<<"high="<<(short)high<<endl;
              // cout<<"low="<<(short)low<<endl;


              memory[value] = low;
              memory[value +1] = high;

              registers[7]+=5;
              
              break;
            }

            case 5:{//pcrel
              byte4 = memory[registers[7]+3];
              byte5 = memory[registers[7]+4];
              int regnumD = (byte2>>4) & 0x0f;
              registers[7]+=5;
              value = ((byte4<<8) | byte5) + registers[7];
              
              break;
            }

          }

          // cout<<endl<<"str end"<<endl;

        }

       
      
       

        break;
      }

      

     
      

    }







  }
}

int Emulator::getAdressJmp(uint8_t byte2,uint8_t byte3){
  switch(byte3) {
    //cout<<"get addr jmp = "<< byte3;

    case 0x00: { return 0;} //neposredno
    case 0x01: { return 1;} //regdir
    case 0x02: { return 2;} //regind
    case 0x03: { return 3;} //regindpom
    case 0x04: { return 4;} //memdir
    case 0x05: { return 5;} //pcrel

  }
  return -1;
}

int Emulator::getAdressData(uint8_t byte2,uint8_t byte3){
  switch(byte3) {

    case 0x00: { return 0;}  //neposredno
    case 0x01: { return 1;} //regdir
    case 0x02: { return 2;} //regind

    case 0x03: { 
      if((byte2 & 0x0f)==7)
         return 5; //pcrel
      else return 3 ;   //regindpom
      
     
    }
    case 0x04: { return 4;} //memdir

  }
  
  return -1;
}


void Emulator::printResults(){
  cout<<"------------------------------------------------"<<endl;
  cout<<"Emulated processor executed halt instruction"<<endl;
  cout<<"Emulated processor state: psw=0b"<<bitset<16>(psw)<<endl;
  cout<<"r0=0x"<<setw(4)<<setfill('0')<<hex<<(registers[0])<<"    r1=0x"<<setw(4)<<setfill('0')<<hex<<(registers[1])<<"    ";
  cout<<"r2=0x"<<setw(4)<<setfill('0')<<hex<<(registers[2])<<"    r3=0x"<<setw(4)<<setfill('0')<<hex<<(registers[3])<<endl;
  cout<<"r4=0x"<<setw(4)<<setfill('0')<<hex<<(registers[4])<<"    r5=0x"<<setw(4)<<setfill('0')<<hex<<(registers[5])<<"    ";
  cout<<"r6=0x"<<setw(4)<<setfill('0')<<hex<<(registers[6])<<"    r7=0x"<<setw(4)<<setfill('0')<<hex<<(registers[7])<<endl;

}