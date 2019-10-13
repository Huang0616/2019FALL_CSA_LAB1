//
//  main.cpp
//  mips
//
//  Created by 黄逸飞 on 9/20/19.
//  Copyright © 2019 黄逸飞. All rights reserved.
//
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
    
    void Read(bitset<5> RdReg1, bitset<5> RdReg2)
    {
        // implement the funciton by you.
        unsigned long Re1 = RdReg1.to_ulong();
        unsigned long Re2 = RdReg2.to_ulong();
        ReadData1 = Registers[(int)Re1];
        ReadData2 = Registers[(int)Re2];
    }
    
    void Write(bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {
        // implement the funciton by you.
        unsigned long WrR = WrtReg.to_ulong();
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
        data1 = oprand1.to_ulong();
        data2 = oprand2.to_ulong();
        if(ALUOP.to_ulong()==1){         //add
            result = data1 + data2;
            ALUresult = bitset<32>(result);
        }else if(ALUOP.to_ulong()==3)     //sub
        {
            result = data1 - data2;
            ALUresult =  bitset<32> (result);
        }else if(ALUOP.to_ulong()==4){   //and
            ALUresult = oprand1&oprand2;
        }else if(ALUOP.to_ulong()==5){   //or
            ALUresult = oprand1|oprand2;
        }else if(ALUOP.to_ulong()==7){   //not or
            ALUresult = ~(oprand1|oprand2);
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
                IMem[i] = bitset<8> (line.substr(0,8));
                i++;
            }
            
        }
        else cout<<"Unable to open file";
        imem.close();
        
    }
    
    bitset<32> ReadMemory (bitset<32> ReadAddress)
    {
        // implement by you. (Read the byte at the ReadAddress and the following three byte).
        unsigned long address = ReadAddress.to_ulong();
        int i = 0;
        string add = "";
        for(;i<4;i++)
        {
            add += IMem[(int)address+i].to_string();
        }
        Instruction = bitset<32> (add);
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
                DMem[i] = bitset<8> (line.substr(0,8));
                i++;
            }
        }
        else cout<<"Unable to open file";
        dmem.close();
        
    }
    
    bitset<32> MemoryRead (bitset<32> Address, bitset<1> readmem)
    {
        if(readmem==1)
        {
            unsigned long address = Address.to_ulong();
            string memory = "";
            for(int i = 0;i<4;i++)
            {
                memory += DMem[(int)address+i].to_string();
            }
            readdata = bitset<32> (memory);
        }
        return readdata;
    }
    
    void MemoryWrite (bitset<32> Address, bitset<32> WriteData, bitset<1> writemem)
    {
        
        // implement by you.
        if(writemem==1)
        {
            unsigned long address = Address.to_ulong();
            string memory = WriteData.to_string();
            for(int i = 0;i<4;i++)
            {
                DMem[(int)address+i] = bitset<8> (memory.substr(8*i,8));
            }
        }
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
    
    unsigned long address = 0;              //initialize pc
    bitset<32> inst = bitset<32> (address);
    while (true)
    {
        // Fetch
        bitset<32> instruction = myInsMem.ReadMemory(inst);
        inst = bitset<32> (inst.to_ulong()+4);                  //renew pc
        // If current insturciton is "11111111111111111111111111111111", then break;
        if(instruction == bitset<32> ("11111111111111111111111111111111"))
        {
            break;
        }
        // decode(Read RF)
        string instruct = instruction.to_string();
        bitset<6> opcode = bitset<6> (instruct.substr(0,6));        //the last 3 of top six bits
        bitset<3> op = bitset<3> (instruct.substr(3,3));
        bitset<5> Rs = bitset<5> (instruct.substr(6,5));
        bitset<5> Rt = bitset<5> (instruct.substr(11,5));
        bitset<5> Rd = bitset<5> (instruct.substr(16,5));
        bitset<16> imm = bitset<16> (instruct.substr(16));
        bitset<3> func = bitset<3> (instruct.substr(29));           //R type the last 3 bits means the operation
        bitset<1> Wrtable = bitset<1> ("1");
        bitset<26> address = bitset<26> (instruct.substr(6,26));
        // Execute
        if(opcode.to_ulong()==2){                               //J j
            bitset<32> jumpAddress = bitset<32> (inst.to_string().substr(0,4)+address.to_string()+"00");
            inst = jumpAddress.to_ulong();
        }
        myRF.Read(Rs, Rt);
        string sign1;
        if(imm.to_string()[0]=='1'){
            sign1 = "1111111111111111";
        }else{
            sign1 = "0000000000000000";
        }
        bitset<32> extendImm = bitset<32> (sign1+imm.to_string());     //signextend
        if(opcode.to_ulong() == 0)                                      //R type
        {
            myALU.ALUOperation(func, myRF.ReadData1, myRF.ReadData2);
            myRF.Write(Rd, myALU.ALUresult, Wrtable);
        }else if(opcode.to_ulong()<16&&opcode.to_ulong()>4){      //I type and not lw sw
            myALU.ALUOperation(op, myRF.ReadData1, extendImm);
            myRF.Write(Rt, myALU.ALUresult, Wrtable);
        }
        // Read/Write Mem
        if(opcode==bitset<6>("100011")){                                        //I lw
            myALU.ALUOperation(bitset<3>("001"), myRF.ReadData1, extendImm);
            myRF.Write(Rt, myDataMem.MemoryRead(myALU.ALUresult,1), Wrtable);
        }
        // Write back to RF
        if(opcode==bitset<6>("101011")){                                        //I sw
            myALU.ALUOperation(bitset<3>("001"), myRF.ReadData1, extendImm);
            myDataMem.MemoryWrite(myALU.ALUresult, myRF.ReadData2, 1);
        }
        if(opcode==bitset<6>("000100")&&myRF.ReadData1==myRF.ReadData2){        //I beq
            bitset<32> branchAddress = bitset<32> (extendImm.to_string().substr(0,30)+"00");
            inst = bitset<32> (inst.to_ulong()+branchAddress.to_ulong());
        }
        myRF.OutputRF(); // dump RF;
    }
    myDataMem.OutputDataMem(); // dump data mem
    
    return 0;
    
}
