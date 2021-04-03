#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>
#include "pin.H"

struct Node{   
    // name of the function
    std::string label;
    // number of the function starting from 1
    unsigned int number = 1;
    // used to format the graph
    // 0 if the node is both a parent and a child, and 1 otherwise.
    bool show = true;
};

// map from function name to the function children
std::map<std::string, std::vector<Node>> parents;
// map to store all seen children nodes
std::map<ADDRINT, unsigned int> seenChildren;
// function number counter
unsigned int stepsCounter = 0;

// add function to functions map
void addToMap(std::string sourceName, std::string targetName, ADDRINT sourceAddress, ADDRINT targetAddress){
    stepsCounter++;
    Node calledFunction = {targetName,stepsCounter,1};
    auto parentFound = parents.find(sourceName);
    if(parentFound != parents.end()){   
        // if parent was found
        seenChildren.insert({targetAddress,stepsCounter});
        parentFound->second.push_back(calledFunction);
    }else{
        // new parent 
        auto childFound = seenChildren.find(sourceAddress);
        Node parent;
        bool parentFound = false;
        if(childFound != seenChildren.end()){
            // if the function calls other functions (parent) and it is a child of another function.
            // set show =0, and use the number the child node has
            parent = {sourceName,childFound->second,false};
            parentFound = true;
        }
        else if(sourceName=="main"){
            // create new parent "new root node"
            // main is the root function
            parent = {sourceName,++stepsCounter,true};
            parentFound =true;
        }
        if(parentFound==false){
            return;
        }
        std::vector<Node> v;
        v.push_back(parent);
        v.push_back(calledFunction);
        parents.insert({sourceName,v});
    }
}

// format the dot graph file
void createDotGraphOutput(int code, void *v){
    std::string graphFileName = "graph.txt";
    std::ofstream myfile;
    myfile.open (graphFileName.c_str());
    myfile << "digraph executedFunctions {\nforcelabels=true;\nedge [ style=dashed ];\n";
    // print labeles
    for(auto const& x : parents){
        for(auto & element : x.second){
            if(element.show==1){
                myfile << element.number << " [label=" << element.label << "]\n";                 
            }
        }
    }
    // print subgraphs
    for(auto const& x : parents){
        myfile << "subgraph cluster_" << x.first << "{\n";
        for(auto & element : x.second){
            if(element.show==1){
                myfile << element.number << "\n";              
            }
        }
        myfile << "}\n";
    }
    // print nodes connections
    Node pre;
    bool preFound =false;
    for(auto const& x : parents){
        for(auto const& element : x.second){
            if(preFound){
                myfile << pre.number << " -> " << element.number << "\n";
            }
            pre = element;
            preFound = true;
        }
        preFound = false;
    }
    myfile << "}\n";
    myfile.close();
}

// instrumentation function for each instruction
void instrumentInsn(INS ins, void *v){
    // check if instruction is a call instruction
    if(!INS_IsCall(ins) || !INS_IsDirectControlFlow(ins)){ 
        return;
    }
    // check if the instruction is in the main executable
    IMG img = IMG_FindByAddress(INS_Address(ins));
    if(!IMG_Valid(img) || !IMG_IsMainExecutable(img)){ 
        return;
    }
    RTN source = INS_Rtn(ins);
    ADDRINT sourceAddress = RTN_Address(source);
    std::string sourceName = RTN_Name(source);
    ADDRINT targetAddress = INS_DirectControlFlowTargetAddress(ins);
    std::string targetName = RTN_FindNameByAddress(targetAddress);
    addToMap(sourceName,targetName,sourceAddress,targetAddress);
}

int main(int argc, char *argv[]){
    PIN_InitSymbols();
    if(PIN_Init(argc,argv)) {
        return 1;
    }
    // instrument at instruction granularity
    INS_AddInstrumentFunction(instrumentInsn, NULL);
    PIN_AddFiniFunction(createDotGraphOutput, NULL); 
    PIN_StartProgram(); 
    return 0;
}
