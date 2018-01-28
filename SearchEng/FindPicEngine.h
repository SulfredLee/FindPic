#ifndef FINDPICENGINE_H
#define FINDPICENGINE_H
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <fstream>

#include <boost/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class FindPicEngine{
public:
    FindPicEngine();
    ~FindPicEngine();

    void StartFinding(const std::string& strTargetPath
                      , const std::string& strSourcePath
                      , const std::string& strResultPath
                      , const int& nThreshold);
private:
    void GetFileList(const boost::filesystem::path& root
                     , const std::string& ext
                     , std::vector<boost::filesystem::path>& ret);
    template<class T>
    void PrintVector(const std::vector<T>& inVec);
    void PrepareTargetHist();
    void DoCompare();
    void OutputResult(const std::string& strResultPath, const int& nThreshold);
private:
    std::vector<boost::filesystem::path> m_vecSourcePhotos;
    std::vector<boost::filesystem::path> m_vecTargetPhotos;
    std::vector<std::multimap<double,boost::filesystem::path> > m_multimapResult; // key: difference, value: photo path
    std::vector<cv::MatND> m_vecHistTarget;
};
#endif
