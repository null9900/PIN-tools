#include <string>
#include <iostream>
#include <sstream>
#include "pin.H"

// get the real path of a file from the file descriptor
std::string getPathFromFd(unsigned int fd){
    std::string fdPath = "/proc/self/fd/";
    std::ostringstream ss;
    ss << fd;
    fdPath = fdPath+ss.str();
    std::string filePath(realpath(fdPath.c_str(), NULL));
    return filePath;
}

// printing the arguments of the write syscall
void printWriteSyscallArgs(unsigned int fd, std::string buffer){
    std::string filePath = getPathFromFd(fd);
    std::cout << "Found write syscall" << std::endl;
    std::cout << "Target file: "<< filePath << std::endl;
    std::cout << "Writting buffer: "<< buffer << std::endl;
}

// called before entering any syscall
void syscallEntry(THREADID threadIndex, CONTEXT *ctxt, SYSCALL_STANDARD std, void *v){
    ADDRINT sysCallNumber = PIN_GetSyscallNumber(ctxt, std);
    // if syscall is write
    if(sysCallNumber==1){
        // getting the arguments of the write sys call, the fd and the buffer
        ADDRINT arg0 = PIN_GetSyscallArgument(ctxt, std, 0);
        ADDRINT arg1 = PIN_GetSyscallArgument(ctxt, std, 1);
        std::string buffer((char*)arg1);
        printWriteSyscallArgs((unsigned int)arg0,buffer);
    }
}

int main(int argc, char *argv[]){
    if (PIN_Init(argc, argv)){
        return 1;
    }
    // hook syscalls
    PIN_AddSyscallEntryFunction(syscallEntry, 0);
    PIN_StartProgram();
    return 0;
}
