#ifndef EMULATOR_H_
#define EMULATOR_H_

#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include<vector>
#include<bits/stdc++.h>
#include<list>
#include<iterator>

using namespace std;


class Emulator{
private:

string code;

short registers[8];
short psw;
vector <uint8_t> memory;
bool end = false;


public:

Emulator(string inputFilename);
void init();
void emulation();
void printResults();


int getAdressJmp(uint8_t byte2,uint8_t byte3);
int getAdressData(uint8_t byte2,uint8_t byte3);

static int MEMORY_SIZE;
static int MEMORY_MAPPED_REGISTERS;

void push_psw();
void pop_psw();
void push_pc();
void pop_pc();

void set_Z(){psw = (psw | 0x0001);}
void set_O(){psw = (psw | 0x0002);}
void set_C(){psw = (psw | 0x0004);}
void set_N(){psw = (psw | 0x0008);}

void clear_Z(){psw = (psw & 0xfffe);}
void clear_O(){psw = (psw & 0xfffd);}
void clear_C(){psw = (psw & 0x000b);}
void clear_N(){psw = (psw & 0x0007);}

bool check_Z(){return (psw & 0x0001);}
bool check_O(){return (psw & 0x0002);}
bool check_C(){return (psw & 0x0004);}
bool check_N(){return (psw & 0x0008);}

std::vector<uint8_t> string_to_vector(const std::string& str) {
  std::vector<uint8_t> result;
  std::stringstream ss;
  for (std::size_t i = 0; i < str.length(); i += 2) {
    std::string pair = str.substr(i, 2);
    
    long byte = stoul(pair,nullptr,16);
  
   
    result.push_back((uint8_t)byte);
  }
  return result;
}




};





#endif
