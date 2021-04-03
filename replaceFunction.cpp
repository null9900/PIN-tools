#include <iostream>
#include "pin.H"

int replacment(){
    std::cout<< "function replaced" << std::endl;
    return 1;
}

void ImageLoad(IMG img, void *v){

    RTN rtn = RTN_FindByName(img, "function1");
    if (RTN_Valid(rtn) && RTN_IsSafeForProbedReplacement(rtn)){   
        // replace targeted function
        RTN_ReplaceProbed(rtn, AFUNPTR(replacment));
    }   
}

int main(int argc, char *argv[]){
    PIN_InitSymbols();
    if (PIN_Init(argc, argv)){
        return 1;
    }
    // instrument images
    IMG_AddInstrumentFunction(ImageLoad, 0);
    // start PIN in probe mode
    PIN_StartProgramProbed();
    return 0;
}
