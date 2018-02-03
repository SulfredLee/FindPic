#ifndef FACEDETECTOR_H
#define FACEDETECTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <iostream>
#include <iomanip>

#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

class FaceDetector{
public:
    FaceDetector();
    ~FaceDetector();

    void StartProcess(const std::string& strSrcPath
                      , const std::string& strResultPath);
private:
    void GetFileList(const boost::filesystem::path& root
                     , const std::string& ext
                     , std::vector<boost::filesystem::path>& ret);
    void DetectAndSave(cv::Mat frame
                       , const std::string& strResultPath
                       , const std::string& strPhotoName
                       , int nThreadIndex);
    void InitVecTH();
    void ProcessMain(int nThreadIndex);
private:
    std::vector<boost::thread> m_vecThreads;
    int m_nThreadNumber;
    std::string m_strResultPath;
    std::vector<boost::filesystem::path> m_vecSourcePhotos;
    std::vector<std::vector<boost::filesystem::path> > m_vecTHSourcePhotos;
    std::vector<cv::CascadeClassifier> m_vec_face_cascade;
};

#endif
