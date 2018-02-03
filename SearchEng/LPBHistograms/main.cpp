#include "main.h"

int main(int argc, char* argv[]){
    TryLPBHistogramRecognizer tryLPBHistogramRecognizer;
    std::cout << "Hello World" << std::endl;

    tryLPBHistogramRecognizer.processMatching("./target/input.csv", "./pic_face", "./LPBHistogramResult", true, 100);
    return 0;
}
