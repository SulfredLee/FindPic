#include "FaceDetector.h"
using namespace cv;
using namespace std;
using namespace boost::filesystem;

FaceDetector::FaceDetector(){
    m_nThreadNumber = 4;
    m_vec_face_cascade.clear();
    m_vec_face_cascade.resize(m_nThreadNumber);
    for(int i = 0; i < m_nThreadNumber; i++){
        if(!m_vec_face_cascade[i].load("/usr/local/opencv/2.4.13.2/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml")){
            std::cout << "Error load face xml\n";
            exit(0);
        }
    }
}

FaceDetector::~FaceDetector(){
    for(int i = 0; i < m_nThreadNumber; i++){
        m_vecThreads[i].join();
    }
}

void FaceDetector::StartProcess(const std::string& strSrcPath
                                , const std::string& strResultPath){
    m_strResultPath = strResultPath;
    create_directory(path(strResultPath));
    GetFileList(strSrcPath, ".jpg", m_vecSourcePhotos);
    InitVecTH();
    for(int i = 0; i < m_nThreadNumber; i++){
        m_vecThreads.push_back(boost::thread(&FaceDetector::ProcessMain, this, i));
    }
}

void FaceDetector::GetFileList(const path& root
                               , const std::string& ext
                               , std::vector<path>& ret){
    ret.clear();
    if(!exists(root) || !is_directory(root)) return;

    recursive_directory_iterator it(root);
    recursive_directory_iterator endit;

    while(it != endit){
        if(is_regular_file(*it) && it->path().extension().string() == ext){
            ret.push_back(it->path());
        }
        ++it;
    }
}

void FaceDetector::DetectAndSave(cv::Mat frame
                                 , const std::string& strResultPath
                                 , const std::string& strPhotoName
                                 , int nThreadIndex){
    std::vector<Rect> faces;
    Mat frame_gray;

    cvtColor( frame, frame_gray, CV_BGR2GRAY );
    equalizeHist( frame_gray, frame_gray );

    //-- Detect faces
    m_vec_face_cascade[nThreadIndex].detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

    Rect roi;
    bool bFoundFace = false;
    for( size_t i = 0; i < faces.size(); i++ ){
        Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
        //ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );
        //rectangle(frame, Point(faces[i].x, faces[i].y), Point(faces[i].x + faces[i].width, faces[i].y + faces[i].height), Scalar(255, 0 ,255), 2);
        roi = Rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height);
        bFoundFace = true;
    }
    //-- Save what you got
    if(bFoundFace){
        imwrite(format("%s/%s", strResultPath.c_str(), strPhotoName.c_str()), frame(roi));
    }
}

void FaceDetector::InitVecTH(){
    m_vecTHSourcePhotos.clear();
    m_vecTHSourcePhotos.resize(m_nThreadNumber);
    for(size_t i = 0; i < m_vecSourcePhotos.size(); i++){
        m_vecTHSourcePhotos[i%m_nThreadNumber].push_back(m_vecSourcePhotos[i]);
    }
}

void FaceDetector::ProcessMain(int nThreadIndex){
    for(size_t i = 0; i < m_vecTHSourcePhotos[nThreadIndex].size(); i++){
        Mat testSample = cv::imread(m_vecTHSourcePhotos[nThreadIndex][i].string(), CV_LOAD_IMAGE_COLOR);
        if(!testSample.data){
            continue;
        }
        DetectAndSave(testSample
                      , m_strResultPath
                      , m_vecTHSourcePhotos[nThreadIndex][i].filename().string()
                      , nThreadIndex);
    }
}
