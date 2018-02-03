#include "main.h"

int main(int argc, char* agrv[]){
    FaceDetector app;
    app.StartProcess("./pic", "./pic_face");
    return 0;
}
