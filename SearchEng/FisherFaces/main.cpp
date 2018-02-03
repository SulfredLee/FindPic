#include "main.h"

int main(int argc, char* argv[]){
    TryFisherFaceRecognizer tryFisherFaceRecognizer;
    std::cout << "Hello World" << std::endl;

    tryFisherFaceRecognizer.processMatching("./target/input.csv", "./pic_face", "./FisherFaceResult", true, 100);
    return 0;
}
