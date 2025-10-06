#include <iostream>
#include "HBuffer/HBuffer.hpp"

int main(int argc, char** argv){
    HBuffer hBuffer = "HEllo WOrld";
    HBuffer bufdfe(new char[5000], 5000, true, false);
    std::cout << hBuffer.GetCStr() << std::endl;
    std::cout << "HBuffer " << (hBuffer.StartsWith("HEl") ? "does" : "doesn't") << " start with Hel"<<std::endl;
    std::cout << "HBuffer " << (hBuffer.StartsWith("Hll") ? "does" : "doesn't") << " start with Hll"<<std::endl;
    std::cout << "HBuffer string at 4 with 5 characters is " << hBuffer.SubString(4, 5).GetCStr()<<std::endl;
}