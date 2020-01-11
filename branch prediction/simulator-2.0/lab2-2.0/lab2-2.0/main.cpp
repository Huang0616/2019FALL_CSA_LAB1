#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <deque>
using namespace std;

int main (int argc, char** argv) {
    ifstream config;
    config.open(argv[1]);
    
    int m, k;
    config >> m >> k;
 
    config.close();

    ofstream out;
    string out_file_name = string(argv[2]) + ".out";
    out.open(out_file_name.c_str());
    
    ifstream trace;
    trace.open(argv[2]);
    

    vector<vector<int>> BHR;
    unsigned long index = pow(2,m);
    unsigned long history = pow(2, k);
    BHR.resize(history);
    for(int i = 0;i<history;i++){
        BHR[i].resize(index);
        for(int j = 0;j<index;j++){
            BHR[i][j] = 3;
        }
    }
    
    deque<bool> globe(k,true);
    
    while (!trace.eof()) {
        unsigned long pc; bool taken;
        trace >> std::hex >> pc >> taken;
        bool prediction;
        
        int glb = 0;
        for(int i=0;i<globe.size();i++){
            glb += globe[i]*pow(2,i);
        }
        unsigned long idx = pc%index;
        if(BHR[glb][idx]==3||BHR[glb][idx]==2){
            prediction = true;
            if(taken==1){
                BHR[glb][idx]=3;
            }else{
                BHR[glb][idx]=BHR[glb][idx]==3?2:0;
            }
        }else{
            prediction = false;
            if(taken==1){
                BHR[glb][idx]=BHR[glb][idx]==0?1:3;
            }else{
                BHR[glb][idx]=0;
            }
        }
        globe.pop_back();
        globe.push_front(taken);
        out << prediction << endl;
    }
     
    trace.close();
    out.close();
}
