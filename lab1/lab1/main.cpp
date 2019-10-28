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
                    IMem[i] = bitset<8>(line.substr(0,8));
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
                    DMem[i] = bitset<8>(line.substr(0,8));
                    i++;
                }
            }
            else cout<<"Unable to open file";
                dmem.close();
        }
        
        bitset<32> readDataMem(bitset<32> Address)
        {
            string datamem;
            unsigned long address = Address.to_ulong();
            for(int i = 0;i<4;i++)
            {
                datamem += DMem[(int)address+i].to_string();
            }
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
//    newState.WB.nop = 0;
    State.WB.nop = 1;
//    newState.MEM.nop = 0;
    State.MEM.nop = 1;
//    newState.EX.nop = 0;
    State.EX.nop = 1;
//    newState.ID.nop = 0;
    State.ID.nop = 1;
//    newState.IF.nop = 0;
    State.IF.nop = 0;
    int cycle = 0;
    State.IF.PC = cycle*4;         //init pc
    
    bool isBranch = 0;
    bitset<32> address;
             
    while (1) {
        
        /* --------------------- WB stage --------------------- */
        if(State.WB.nop != 1){
            //nop == 1, stall
            if(State.WB.wrt_enable){
                myRF.writeRF(State.WB.Wrt_reg_addr,State.WB.Wrt_data);
            }
        }


        /* --------------------- MEM stage --------------------- */
        if(State.MEM.nop == 1){          //nop propagation;
            newState.WB.nop = 1;
        }else{
            newState.WB.nop = 0;
            newState.WB.Rs = bitset<5> (State.MEM.Rs.to_string());
            newState.WB.Rt = State.MEM.Rt;
            newState.WB.Wrt_reg_addr = State.MEM.Wrt_reg_addr;
            newState.WB.wrt_enable = State.MEM.wrt_enable;
            newState.WB.Wrt_data = State.MEM.ALUresult;
            //nop is unset, do these operations
            if(State.MEM.rd_mem){       //lw
                newState.WB.Wrt_data = myDataMem.readDataMem(State.MEM.ALUresult);
            }
            if(State.MEM.wrt_mem){      //sw
                //mem-mem forwarding
                if(State.WB.wrt_enable){
                    if(State.WB.Wrt_reg_addr == State.MEM.Rt){
                        State.MEM.Store_data = State.WB.Wrt_data;
                    }
                }
                myDataMem.writeDataMem(State.MEM.ALUresult,State.MEM.Store_data);
            }
        }

        /* --------------------- EX stage --------------------- */
        string sign1;
        if(State.EX.Imm.to_string()[0]=='1'){
            sign1 = "1111111111111111";
        }else{
            sign1 = "0000000000000000";
        }
        bitset<32> extendImm = bitset<32> (sign1+State.EX.Imm.to_string());
        bitset<32> operand1;
        bitset<32> operand2;
        if(State.EX.nop == 1){          //nop propagation;
            newState.MEM.nop = 1;
        }else{
            newState.MEM.nop = 0;       //nop is set, jump these operations
            //EX-EX forwarding
            if (State.MEM.wrt_enable) {
                if (State.MEM.Wrt_reg_addr == State.EX.Rs) {
                    State.EX.Read_data1 = State.MEM.ALUresult;
                }
            }
            //MEM-EX forwarding
            if (State.WB.wrt_enable) {
                if (State.WB.Wrt_reg_addr == State.EX.Rs) {
                    State.EX.Read_data1 = State.WB.Wrt_data;
                }
            }
            operand1 = State.EX.Read_data1;
            if(State.EX.is_I_type){
                operand2 = extendImm;
            }else{
                //EX-EX forwarding
                if (State.MEM.wrt_enable) {
                    if(State.MEM.Wrt_reg_addr == State.EX.Rt){
                        State.EX.Read_data2 = State.MEM.ALUresult;
                    }
                }
                //MEM-EX forwarding:
                if (State.WB.wrt_enable) {
                    if (State.WB.Wrt_reg_addr == State.EX.Rt) {
                        State.EX.Read_data2 = State.WB.Wrt_data;
                    }
                }
                operand2 = State.EX.Read_data2;
            }
            if(State.EX.alu_op == 1){
                newState.MEM.ALUresult = bitset<32>(operand2.to_ulong() +
                                                    operand1.to_ulong());
            }else{
                newState.MEM.ALUresult = bitset<32>(operand1.to_ulong() -
                                                    operand2.to_ulong());
            }
            newState.MEM.Store_data = operand2;
            newState.MEM.Rs = State.EX.Rs;
            newState.MEM.Rt = State.EX.Rt;
            newState.MEM.Wrt_reg_addr = State.EX.Wrt_reg_addr;
            newState.MEM.wrt_enable = State.EX.wrt_enable;
            newState.MEM.rd_mem = State.EX.rd_mem;
            newState.MEM.wrt_mem = State.EX.wrt_mem;
        }

        /* --------------------- ID stage --------------------- */
        if(isBranch){       //branch message from former stage, so pc = new pc
            isBranch = 0;  //reset current branch command to 0
            address = 0;    //reset
        }//if is branch, there must be a nop in current ID stage
        //thus the following if will true
        if(State.ID.nop == 1){          //nop propagation;
            newState.EX.nop = 1;
        }else{
            newState.EX.nop = 0;
            //Decoder
            newState.EX.Rs = bitset<5> (State.ID.Instr.to_string().substr(6,5));
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
                newState.EX.Wrt_reg_addr = bitset<5> (State.ID.Instr.to_string().substr(11,5));
                if(State.ID.Instr.to_string().substr(0,6) == "000100"){     //beq, not write back
                    newState.EX.wrt_enable = 0;
                    //rd wrt--TODO
                }else if(State.ID.Instr.to_string().substr(0,6) == "101011"){   //sw, not rigester, memeroy
                    newState.EX.wrt_enable = 0;
                    newState.EX.wrt_mem = 1;
                    newState.EX.rd_mem = 0;
                }else if(State.ID.Instr.to_string().substr(0,6) == "100011"){          //lw
                    newState.EX.wrt_enable = 1;
                    newState.EX.rd_mem = 1;
                    newState.EX.wrt_mem = 0;
                }
            }else{      //R type
                newState.EX.Wrt_reg_addr = bitset<5> (State.ID.Instr.to_string().substr(16,5));
                newState.EX.wrt_enable = 1;
                newState.EX.rd_mem = 0;
                newState.EX.wrt_mem = 0;
            }
            //bne:branch if not equal
            if(State.ID.Instr.to_string().substr(0,6) == "000100"){
                if(newState.EX.Read_data1 != newState.EX.Read_data2){
                    isBranch = 1;
                    string sign1;
                    if(newState.EX.Imm.to_string()[0]=='1'){
                        sign1 = "1111111111111111";
                    }else{
                        sign1 = "0000000000000000";
                    }
                    bitset<32> extendImm = bitset<32> (sign1+newState.EX.Imm.to_string());
                    //cout<<"ex:"<<extendImm.to_string().substr(2,32)<<endl;
                    address = bitset<32> (extendImm.to_string().substr(2,32) + "00");
                    //cout<<"ad:"<<address.to_ulong()<<endl;
                    newState.ID.nop = 1;
                }
                //once it is a branch command, no matter branch or not, next EX stage do not work
                newState.EX.nop = 1;
            }
        }
        
        /* --------------------- IF stage --------------------- */
        //cout<<"pc:"<<State.IF.PC.to_ulong()<<endl;
        newState.ID.Instr = myInsMem.readInstr(State.IF.PC);   //fetch instruction
        if(newState.ID.Instr.to_string() == "11111111111111111111111111111111"){
            State.IF.nop = 1;       //halt
        }
        if(State.IF.nop == 1){
            if(isBranch==0){        //halt, not branch, thus noramlly halt
                newState.IF.PC = State.IF.PC;
                newState.ID.nop = 1;
            }else{               //halt and branch, thus branch first ignore halt
                newState.IF.PC = State.IF.PC.to_ulong() + address.to_ulong();
            }
        }else{
            if(isBranch==0){  //not halt not branch, thus normally execute ID in next stage
                newState.ID.nop = 0;
                newState.IF.PC = State.IF.PC.to_ulong() + 4;
            }else{  //not halt and branch, thus normally upadte the pc for latter operation
                newState.IF.PC = State.IF.PC.to_ulong() + address.to_ulong();
            }
        }
        //stall
        bool stall = 0;
        if(State.EX.rd_mem){
            if(State.EX.Wrt_reg_addr == newState.EX.Rs||
               State.EX.Wrt_reg_addr == newState.EX.Rt){
                stall = 1;
                newState.EX.nop = 1;
                newState.IF = State.IF;
                newState.ID = State.ID;
                newState.EX.Wrt_reg_addr = newState.EX.Wrt_reg_addr.to_ulong() + 1;
            }
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

