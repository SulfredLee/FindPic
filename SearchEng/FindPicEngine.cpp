#include "FindPicEngine.h"
using namespace boost::filesystem;

FindPicEngine::FindPicEngine(){
    m_multimapResult.resize(4);
}

FindPicEngine::~FindPicEngine(){
}

void FindPicEngine::StartFinding(const std::string& strTargetPath
                                 , const std::string& strSourcePath
                                 , const std::string& strResultPath
                                 , const int& nThreshold){
    GetFileList(strTargetPath, ".jpg", m_vecTargetPhotos);
    GetFileList(strSourcePath, ".jpg", m_vecSourcePhotos);
    PrepareTargetHist();
    DoCompare();
    OutputResult(strResultPath, nThreshold);
}

void FindPicEngine::GetFileList(const path& root
                                , const std::string& ext
                                , std::vector<path>& ret){
    ret.clear();
    if(!exists(root) || !is_directory(root)) return;

    recursive_directory_iterator it(root);
    recursive_directory_iterator endit;

    while(it != endit){
        if(is_regular_file(*it) && it->path().extension().string() == ext){
            //ret.push_back(it->path().filename().string());
            ret.push_back(it->path());
        }
        ++it;
    }
}

template<class T>
void FindPicEngine::PrintVector(const std::vector<T>& inVec){
    for(size_t i = 0; i < inVec.size(); i++){
        std::cout << inVec[i] << std::endl;
    }
}

void FindPicEngine::PrepareTargetHist(){
    m_vecHistTarget.clear();
    for(size_t i = 0; i < m_vecTargetPhotos.size(); i++){
        cv::Mat SrcPic = cv::imread(m_vecTargetPhotos[i].string(), 1);
        // Convert to HSV
        cv::Mat HSVPic;
        cv::cvtColor(SrcPic, HSVPic, cv::COLOR_BGR2HSV, 0);
        // Using 50 bins for hue and 60 for saturation
        int nh_bins = 50; int ns_bins = 60;
        int histSize[] = { nh_bins, ns_bins };
        // hue varies from 0 to 179, saturation from 0 to 255
        float fh_ranges[] = { 0, 180 };
        float fs_ranges[] = { 0, 256 };
        const float* pfranges[] = { fh_ranges, fs_ranges };
        // Use the o-th and 1-st channels
        int channels[] = { 0, 1 };
        // Calculate the histograms for the HSV images
        cv::MatND HistTemp;
        cv::calcHist(&HSVPic, 1, channels, cv::Mat(), HistTemp, 2, histSize, pfranges, true, false);
        cv::normalize(HistTemp, HistTemp, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
        // result
        m_vecHistTarget.push_back(HistTemp);
    }
}

void FindPicEngine::DoCompare(){
    m_multimapResult.clear();
    m_multimapResult.resize(4);
    // Using 50 bins for hue and 60 for saturation
    int nh_bins = 50; int ns_bins = 60;
    int histSize[] = { nh_bins, ns_bins };
    // hue varies from 0 to 179, saturation from 0 to 255
    float fh_ranges[] = { 0, 180 };
    float fs_ranges[] = { 0, 256 };
    const float* pfranges[] = { fh_ranges, fs_ranges };
    // Use the o-th and 1-st channels
    int channels[] = { 0, 1 };
    cv::Mat SrcPic;
    cv::Mat HSVPic;
    cv::MatND HistTemp;
    for(size_t i = 0; i < m_vecSourcePhotos.size(); i++){
        SrcPic = cv::imread(m_vecSourcePhotos[i].string(), 1);
        if(!SrcPic.data){
            std::cout << "Now: " << i << " Total: " << m_vecSourcePhotos.size()
                      << " " << m_vecSourcePhotos[i].string()
                      << " cannot open" << std::endl;
            continue;
        }
        // Convert to HSV
        cv::cvtColor(SrcPic, HSVPic, cv::COLOR_BGR2HSV, 0);
        // Calculate the histograms for the HSV images
        cv::calcHist(&HSVPic, 1, channels, cv::Mat(), HistTemp, 2, histSize, pfranges, true, false);
        cv::normalize(HistTemp, HistTemp, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
        // do compare
        double dCorrelation = 0;
        for(int nCompareMethod = 0; nCompareMethod < 4; nCompareMethod++){
            for(size_t j = 0; j < m_vecHistTarget.size(); j++){
                if(nCompareMethod == 0 || nCompareMethod == 2){
                    dCorrelation = std::max(dCorrelation, cv::compareHist(m_vecHistTarget[j], HistTemp, nCompareMethod));
                }
                else{
                    dCorrelation = std::min(dCorrelation, cv::compareHist(m_vecHistTarget[j], HistTemp, nCompareMethod));
                }
            }
            // get result
            m_multimapResult[nCompareMethod].insert(std::make_pair(dCorrelation, m_vecSourcePhotos[i]));
        }
        if(i % 1000 == 0){
            std::cout << "Now: " << i << " Total: " << m_vecSourcePhotos.size() << std::endl;
        }
    }
}

void FindPicEngine::OutputResult(const std::string& strResultPath, const int& nThreshold){
    for(size_t i = 0; i < m_multimapResult.size(); i++){
        int nCount = 0;
        std::string strTempResult = strResultPath + "_" + std::to_string(i);
        create_directory(path(strTempResult));
        // handle result text filename
        std::ofstream FH(strTempResult + "/result.txt", std::ios::out);
        if(i == 0 || i == 2){
            for(auto it = m_multimapResult[i].rbegin(); it != m_multimapResult[i].rend(); it++){
                if(nCount < nThreshold){
                    // output to result folder
                    std::stringstream ss;
                    ss << strTempResult << "/";
                    ss << std::setfill('0') << std::setw(7);
                    ss << nCount << "_" << it->second.filename().string();
                    copy_file(it->second, path(ss.str()));
                    // handle result text
                    FH << it->first << "," << it->second.filename().string() << std::endl;
                }
                else{
                    break;
                }
                nCount++;
            }
        }
        else{
            for(auto it = m_multimapResult[i].begin(); it != m_multimapResult[i].end(); it++){
                if(nCount < nThreshold){
                    // output to result folder
                    std::stringstream ss;
                    ss << strTempResult << "/";
                    ss << std::setfill('0') << std::setw(7);
                    ss << nCount << "_" << it->second.filename().string();
                    copy_file(it->second, path(ss.str()));
                    // handle result text
                    FH << it->first << "," << it->second.filename().string() << std::endl;
                }
                else{
                    break;
                }
                nCount++;
            }
        }
        FH.close();
    }
}
