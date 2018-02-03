#include "main.h"

int main(int argc, char* argv[]){
    FindPicEngine FindingWorker;
    std::cout << "Hello World" << std::endl;

    FindingWorker.StartFinding("./target", "./pic", "./result", 100);
    return 0;
}
