#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem;
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu
    bool        wrt_enable;
    bool        nop;
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem;
    bool        wrt_enable;
    bool        nop;
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
    public:
        bitset<32> Reg_data;
         RF()
        {
            Registers.resize(32);
            Registers[0] = bitset<32> (0);
        }
    
        bitset<32> readRF(bitset<5> Reg_addr)
        {
            Reg_data = Registers[Reg_addr.to_ulong()];
            return Reg_data;
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data)
        {
            Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
        }
         
        void outputRF()
        {
            ofstream rfout;
            rfout.open("RFresult.txt",std::ios_base::app);
            if (rfout.is_open())
            {
                rfout<<"State of RF:\t"<<endl;
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

class INSMem
{
    public:
        bitset<32> Instruction;
        INSMem()
        {
            IMem.resize(MemSize);
            ifstream imem;
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
                  
        bitset<32> readInstr(bitset<32> ReadAddress)
        {
            string insmem;
            insmem.append(IMem[ReadAddress.to_ulong()].to_string());
            insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
            insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
            insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
            Instruction = bitset<32>(insmem);        //read instruction memory
            return Instruction;
        }
      
    private:
        vector<bitset<8> > IMem;
};
      
class DataMem
{
    public:
        bitset<32> ReadData;
        DataMem()
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
        
        bitset<32> readDataMem(bitset<32> Address)
        {
            string datamem;
            datamem.append(DMem[Address.to_ulong()].to_string());
            datamem.append(DMem[Address.to_ulong()+1].to_string());
            datamem.append(DMem[Address.to_ulong()+2].to_string());
            datamem.append(DMem[Address.to_ulong()+3].to_string());
            ReadData = bitset<32>(datamem);        //read data memory
            return ReadData;
        }
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData)
        {
            DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
            DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
            DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
            DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));
        }
                     
        void outputDataMem()
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

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl;
        
        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl;
        
        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl;
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;
        
        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl;
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl;
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl;
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl;
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl;
    }
    else cout<<"Unable to open file";
    printstate.close();
}
 

int main()
{
    
    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    
    struct stateStruct newState,State;
    int cycle = 0;
    State.IF.PC = cycle*4;         //init pc
             
    while (1) {

        /* --------------------- WB stage --------------------- */
        if(State.WB.wrt_enable){
            myRF.writeRF(State.WB.Wrt_reg_addr,State.WB.Wrt_data);
        }


        /* --------------------- MEM stage --------------------- */
        if(State.MEM.nop == 1){          //nop propagation; ignore the hazard
            newState.WB.nop = 1;
        }
        if(State.MEM.rd_mem){       //lw
            newState.WB.Wrt_data = myDataMem.readDataMem(State.MEM.ALUresult);
        }
        if(State.MEM.wrt_mem){      //sw
            myDataMem.writeDataMem(State.MEM.ALUresult,State.MEM.Store_data);
        }
        newState.WB.Rs = State.MEM.Rs;
        newState.WB.Rt = State.MEM.Rt;
        newState.WB.Wrt_reg_addr = State.EX.Wrt_reg_addr;
        newState.WB.wrt_enable = State.EX.wrt_enable;

        /* --------------------- EX stage --------------------- */
        if(State.EX.nop == 1){          //nop propagation; ignore the hazard
            newState.MEM.nop = 1;
        }
        string sign1;
        if(State.EX.Imm.to_string()[0]=='1'){
            sign1 = "1111111111111111";
        }else{
            sign1 = "0000000000000000";
        }
        bitset<32> extendImm = bitset<32> (sign1+State.EX.Imm.to_string());
        bitset<32> operand2;
        if(State.EX.is_I_type){
            operand2 = extendImm;
        }else{
            operand2 = State.EX.Read_data2;
        }
        if(State.EX.alu_op == 1){
            newState.MEM.ALUresult = bitset<32>(operand2.to_ulong() + State.EX.Read_data1.to_ulong());
        }else{
            newState.MEM.ALUresult = bitset<32>(State.EX.Read_data1.to_ulong() - operand2.to_ulong());
        }
        newState.MEM.Store_data = State.EX.Read_data2;
        newState.MEM.Rs = State.EX.Rs;
        newState.MEM.Rt = State.EX.Rt;
        newState.MEM.Wrt_reg_addr = State.EX.Wrt_reg_addr;
        newState.MEM.wrt_enable = State.EX.wrt_enable;
        newState.MEM.rd_mem = State.EX.rd_mem;
        newState.MEM.wrt_mem = State.EX.wrt_mem;

        /* --------------------- ID stage --------------------- */
        if(State.ID.nop == 1){          //nop propagation; ignore the hazard
            newState.EX.nop = 1;
        }
        //Decoder
        newState.EX.Rs = bitset<5> (State.ID.Instr.to_string().substr(7,5));
        newState.EX.Rt = bitset<5> (State.ID.Instr.to_string().substr(11,5));
        newState.EX.Imm = bitset<16> (State.ID.Instr.to_string().substr(16,16));
        newState.EX.Read_data1 = myRF.readRF(newState.EX.Rs);
        newState.EX.Read_data2 = myRF.readRF(newState.EX.Rt);
        if(State.ID.Instr.to_string().substr(0,6) == "000000"){         //opcode == 0
            newState.EX.is_I_type = 0;
        }else{
            newState.EX.is_I_type = 1;
        }
        if(newState.EX.is_I_type){
            newState.EX.alu_op = 1;            //1 for add
        }else{
            if(State.ID.Instr.to_string().substr(29,3) == "001"){
                newState.EX.alu_op = 1;        //R addu
            }else{
                newState.EX.alu_op = 0;        //R subu
            }
        }
        if(newState.EX.is_I_type){
            newState.EX.Wrt_reg_addr = newState.EX.Rt;
            if(State.ID.Instr.to_string().substr(0,6) == "000100"){     //beq, not write back
                newState.EX.wrt_enable = 0;
            }else if(State.ID.Instr.to_string().substr(0,6) == "101011"){   //sw, not rigester, memeroy
                newState.EX.wrt_enable = 0;
                newState.EX.wrt_mem = 1;
            }else{          //lw
                newState.EX.wrt_enable = 1;
                newState.EX.rd_mem = 1;
            }
        }else{      //R type
            newState.EX.Wrt_reg_addr = bitset<5> (State.ID.Instr.to_string().substr(16,5));
            newState.EX.wrt_enable = 1;
        }
        
        /* --------------------- IF stage --------------------- */
        newState.ID.Instr = myInsMem.readInstr(State.IF.PC);   //fetch instruction
        if(newState.ID.Instr==0){
            State.IF.nop = 1;       //halt
        }
        if(State.IF.nop == 1){
            newState.IF.PC = State.IF.PC;
            newState.ID.nop = 1;
        }else{
            newState.IF.PC = State.IF.PC.to_ulong() + 4;
        }

             
        if (State.IF.nop && State.ID.nop && State.EX.nop && State.MEM.nop && State.WB.nop)
            break;
        
        printState(newState, cycle++); //print states after executing cycle 0, cycle 1, cycle 2 ...
       
        State = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */
                    
    }
    
    myRF.outputRF(); // dump RF;
    myDataMem.outputDataMem(); // dump data mem
    
    return 0;
}

