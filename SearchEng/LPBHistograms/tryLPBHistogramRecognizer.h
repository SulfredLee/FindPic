#ifndef TRYEIGENFACERECOGNIZER_H
#define TRYEIGENFACERECOGNIZER_H
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <iostream>
#include <iomanip>

#include <boost/filesystem.hpp>
class TryLPBHistogramRecognizer{
public:
    TryLPBHistogramRecognizer();
    ~TryLPBHistogramRecognizer();

    void processMatching(const std::string& strTargetCSV
                         , const std::string& strSrcPath
                         , const std::string& strResultPath
                         , bool bIsSaveMean
                         , const int& nThreshold);
private:
    void GetFileList(const boost::filesystem::path& root
                     , const std::string& ext
                     , std::vector<boost::filesystem::path>& ret);
    cv::Mat norm_0_255(cv::InputArray _src);
    void read_csv(const std::string& filename
                  , std::vector<cv::Mat>& images
                  , std::vector<int>& labels
                  , char separator = ';');
    void OutputResult(const std::string& strResultPath, const int& nThreshold);
private:
    std::vector<boost::filesystem::path> m_vecSourcePhotos;
    std::multimap<double,boost::filesystem::path> m_multimapResult; // key: difference, value: photo path
};

#endif
