#include "main.h"

int main(int argc, char* argv[]){
    TryEigenFaceRecognizer tryEigenFaceRecognizer;
    std::cout << "Hello World" << std::endl;

    tryEigenFaceRecognizer.processMatching("./target/input.csv", "./pic_face", "./EigenFaceResult", true, 100);
    return 0;
}
