#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.


class RF
{
    public:
        bitset<32> ReadData1, ReadData2; //ReadData1&2 are 32 bits, set 0 for all bits
     	RF()
    	{ 
          Registers.resize(32);  
          Registers[0] = bitset<32> (0);  //set reg[0] to 00000...
        }
	
        void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
        {   
            // implement the funciton by you.
            unsigned long Re1 = RdReg1.to_ulong;
            unsigned long Re2 = RdReg2.to_ulong;
            unsigned long WrR = WrtReg.to_ulong;
            ReadData1 = Registers[(int)Re1];
            ReadData2 = Registers[(int)Re2];
            if(WrtEnable==1)
            {
                Registers[(int)WrR] = WrtData;
            }
         }
		 
	void OutputRF() // write RF results to file
             {
               ofstream rfout;      //OutFile
                  rfout.open("RFresult.txt",std::ios_base::app);
                  if (rfout.is_open())
                  {
                    rfout<<"A state of RF:"<<endl;
                  for (int j = 0; j<32; j++)
                      {        
                        rfout << Registers[j]<<endl;
                      }
                     
                  }
                  else cout<<"Unable to open file";
                  rfout.close();
               
               }     
	private:
            vector<bitset<32> >Registers;
	
};

class ALU
{
      public:
             bitset<32> ALUresult;
             bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
             {   
                 // implement the ALU operations by you.
                 unsigned long data1,data2,result;
                 bitset<32> results;
                 data1 = oprand1.to_ulong;
                 data2 = oprand2.to_ulong;
                 if(ALUOP.to_ulong==1){
                     result = data1 + data2;
                     ALUresult =  bitset<32> results(result);
                 }else if(ALUOP.to_ulong==3)
                 {
                     result = data1 - data2;
                     ALUresult =  bitset<32> results(result);
                 }
                 return ALUresult;
               }            
};

class INSMem
{
      public:
          bitset<32> Instruction;
          INSMem() // read instruction memory
          {       IMem.resize(MemSize); 
                  ifstream imem;    //read the file into memory
                  string line;
                  int i=0;
                  imem.open("imem.txt");
                  if (imem.is_open())
                  {
                  while (getline(imem,line))
                     {      
                        IMem[i] = bitset<8>(line);
                        i++;
                     }
                     
                  }
                  else cout<<"Unable to open file";
                  imem.close();
                     
                  }
                  
          bitset<32> ReadMemory (bitset<32> ReadAddress) 
              {    
               // implement by you. (Read the byte at the ReadAddress and the following three byte).
                  unsigned long address = ReadAddress.to_ulong;
                  int i = 0;
                  String add = "";
                  for(;i<4;i++)
                  {
                      add += IMem[(int)address+i].to_string;
                  }
                  Instruction = bitset<32>(add);
                  return Instruction;
              }     
      
      private:
           vector<bitset<8> > IMem;
      
};
      
class DataMem    
{
      public:
          bitset<32> readdata;  
          DataMem() // read data memory
          {
             DMem.resize(MemSize); 
             ifstream dmem;
                  string line;
                  int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {      
                        DMem[i] = bitset<8>(line);
                        i++;
                       }
                  }
                  else cout<<"Unable to open file";
                  dmem.close();
          
          }  
          bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
          {    
               
               // implement by you.
              //???????????????????????????????????????????
              if(readmem==1)
              {
                  unsigned long address = Address.to_ulong;
                  String memory = "";
                  for(int 1 = 0;i<4;i++)
                  {
                      memory += DMem[(int)address+i].to_string;
                  }
                  bitset<32> results;
                  readdata = bitset<32> results(memory);
              }else{
                  readdata = Address;
              }
              if(writemem==1)
              {
                  unsigned long address = WriteData.to_ulong;
                  
              }
               return readdata;     
          }   
                     
          void OutputDataMem()  // write dmem results to file
          {
               ofstream dmemout;
                  dmemout.open("dmemresult.txt");
                  if (dmemout.is_open())
                  {
                  for (int j = 0; j< 1000; j++)
                       {     
                        dmemout << DMem[j]<<endl;
                       }
                     
                  }
                  else cout<<"Unable to open file";
                  dmemout.close();
               
               }             
      
      private:
           vector<bitset<8> > DMem;
      
};  


   
int main()
{
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;

    while (1)
	{
        // Fetch
        
		// If current insturciton is "11111111111111111111111111111111", then break;
        
		// decode(Read RF)
		
		// Execute
		
		// Read/Write Mem
		
		// Write back to RF
		
        myRF.OutputRF(); // dump RF;    
    }
        myDataMem.OutputDataMem(); // dump data mem
      
        return 0;
        
}
