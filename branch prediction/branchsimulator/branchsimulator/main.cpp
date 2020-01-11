#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include<math.h>
#include <list>
using namespace std;

class READFILE
{
public:
    int k;
    int m;
    vector<bitset<32> > Instruction;
    vector<int> Branch;
    int totalline;
    READFILE(){
        //line of the trace
        ifstream infile("trace.txt");
        char str[1024];
        int n=0;
        while(!infile.eof())
        {
        infile.getline(str, sizeof(str));
        n++;
        }
        totalline = n;
        
        Instruction.resize(totalline);
        Branch.resize(totalline);
        //read config
        ifstream config;
        string line;
        config.open("config.txt");
        if (config.is_open())
        {
            getline(config,line);
            m =  std::stoi(line);
            getline(config,line);
            k=  std::stoi(line);
        }
        else cout<<"Unable to open file";
        config.close();
        //read trace
        ifstream trace;
        string inst;
        int i = 0;
        trace.open("trace.txt");
        if (trace.is_open())
        {
            while (getline(trace,inst))
            {
                string tmp = inst.substr(0,8);
                int count = (int) tmp.length();
                unsigned long sum = 0;
                for(int j =count-1;j>=0;j--)
                {
                    if(tmp[j]>='0'&&tmp[j]<='9')
                    {
                        sum += (tmp[j]-48)*pow(16,count-j-1);
                    }
                    else if(tmp[j]>='a'&&tmp[j]<='f')
                    {
                        sum += (tmp[j]-87)*pow(16, count-j-1);
                    }else if(tmp[j]>='A'&&tmp[j]<='F')
                    {
                        sum += (tmp[j]-55)*pow(16, count-j-1);
                    }
                }
                Instruction[i] = bitset<32>(sum);
                string branch = inst.substr(9,1);
                Branch[i] = std::stoi(branch);
                i++;
            }
        }
        else cout<<"Unable to open file";
        trace.close();
    }
    
    void OutputPrediction(list<int> prediction)
    {
        ofstream traceout;
        traceout.open("trace.txt.out",std::ios_base::app);
        if (traceout.is_open())
        {
            list<int>::iterator j;
            for (j = prediction.begin(); j!= prediction.end(); j++)
            {
                traceout << *j << endl;
            }
        }
        else cout<<"Unable to open file";
        traceout.close();
    }
};

int char2int(list<char> history){
    list<char>::iterator his;
    int i = 0;
    int s = 0;
    for (his = history.begin(); his!= history.end(); his++)
    {
        s += (*his-'0')*pow(2, i);
        i++;
    }
    return s;
}

int main(int argc, const char * argv[]) {
    READFILE myINPUT;
    list<int> prediction;
    //prediction.push_back(1);
    
    //BHR
    int k = myINPUT.k;
    int m = myINPUT.m;
    vector<vector<bitset<2> > > BHR;
    BHR.resize(pow(2,k));
    for(int i = 0;i<BHR.size();i++){
        BHR[i].resize(pow(2, m));
        for(int j = 0;j<BHR[i].size();j++){
            BHR[i][j] = bitset<2>(3);           //initialized to 11
        }
    }
    list<char> history(k,'1');              //all takens at first
    for(int i = 0;i<myINPUT.Instruction.size();i++){
        int global = char2int(history);
        string inst = myINPUT.Instruction[i].to_string().substr(32-m,m);
        int local = (int) bitset<32> (inst).to_ulong();
        if(BHR[global][local].to_ulong()>=2){
            prediction.push_back(1);
        }else{
            prediction.push_back(0);
        }
        if(myINPUT.Branch[i]==1){
            if(BHR[global][local].to_ulong()>=1){
                BHR[global][local] = 3;
            }else if(BHR[global][local].to_ulong()==0){
                BHR[global][local] = 1;
            }
        }else{
            if(BHR[global][local].to_ulong()<=2){
                BHR[global][local] = 0;
            }else if(BHR[global][local].to_ulong()==3){
                BHR[global][local] = 2;
            }
        }
        history.pop_back();
        history.push_front(myINPUT.Branch[i]+48);
    }
    myINPUT.OutputPrediction(prediction);
    return 0;
}
