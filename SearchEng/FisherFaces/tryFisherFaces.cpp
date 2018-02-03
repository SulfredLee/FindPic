#include "tryFisherFaces.h"
using namespace cv;
using namespace std;
using namespace boost::filesystem;

TryFisherFaceRecognizer::TryFisherFaceRecognizer(){
    
}

TryFisherFaceRecognizer::~TryFisherFaceRecognizer(){
    
}

void TryFisherFaceRecognizer::processMatching(const std::string& strTargetCSV
                                             , const std::string& strSrcPath
                                             , const std::string& strResultPath
                                             , bool bIsSaveMean
                                             , const int& nThreshold){
    string output_folder = strResultPath;
    create_directory(path(strResultPath));
    // Get the path to your CSV.
    string fn_csv = strTargetCSV;
    // These vectors hold the images and corresponding labels.
    vector<Mat> images;
    vector<int> labels;
    // Read in the data. This can fail if no valid
    // input filename is given.
    try {
        read_csv(fn_csv, images, labels);
    } catch (cv::Exception& e) {
        cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
        // nothing more we can do
        exit(1);
    }
    GetFileList(strSrcPath, ".jpg", m_vecSourcePhotos);
    // Quit if there are not enough images for this demo.
    if(images.size() <= 1) {
        string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
        CV_Error(CV_StsError, error_message);
    }
    for(size_t i = 0; i < images.size(); i++){
        Size size(100,100);
        resize(images[i], images[i], size);
    }
    // Get the height from the first image. We'll need this
    // later in code to reshape the images to their original
    // size:
    int height = images[0].rows;
    // The following lines create an Eigenfaces model for
    // face recognition and train it with the images and
    // labels read from the given CSV file.
    // This here is a full PCA, if you just want to keep
    // 10 principal components (read Eigenfaces), then call
    // the factory method like this:
    //
    //      cv::createEigenFaceRecognizer(10);
    //
    // If you want to create a FaceRecognizer with a
    // confidence threshold (e.g. 123.0), call it with:
    //
    //      cv::createEigenFaceRecognizer(10, 123.0);
    //
    // If you want to use _all_ Eigenfaces and have a threshold,
    // then call the method like this:
    //
    //      cv::createEigenFaceRecognizer(0, 123.0);
    //
    Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
    model->train(images, labels);
    //// Here is how to get the eigenvalues of this Eigenfaces model:
    //Mat eigenvalues = model->getMat("eigenvalues");
    //// And we can do the same to display the Eigenvectors (read Eigenfaces):
    //Mat W = model->getMat("eigenvectors");
    //// Get the sample mean from the training data
    //Mat mean = model->getMat("mean");
    //// Display or save:
    //if(!bIsSaveMean) {
    //    imshow("mean", norm_0_255(mean.reshape(1, images[0].rows)));
    //} else {
    //    imwrite(format("%s/mean.png", output_folder.c_str()), norm_0_255(mean.reshape(1, images[0].rows)));
    //}
    //// Display or save the Eigenfaces:
    //for (int i = 0; i < min(10, W.cols); i++) {
    //    string msg = format("Eigenvalue #%d = %.5f", i, eigenvalues.at<double>(i));
    //    cout << msg << endl;
    //    // get eigenvector #i
    //    Mat ev = W.col(i).clone();
    //    // Reshape to original size & normalize to [0...255] for imshow.
    //    Mat grayscale = norm_0_255(ev.reshape(1, height));
    //    // Show the image & apply a Jet colormap for better sensing.
    //    Mat cgrayscale;
    //    applyColorMap(grayscale, cgrayscale, COLORMAP_JET);
    //    // Display or save:
    //    if(!bIsSaveMean) {
    //        imshow(format("eigenface_%d", i), cgrayscale);
    //    } else {
    //        imwrite(format("%s/eigenface_%d.png", output_folder.c_str(), i), norm_0_255(cgrayscale));
    //    }
    //}
    //
    //// Display or save the image reconstruction at some predefined steps:
    //for(int num_components = min(W.cols, 10); num_components < min(W.cols, 300); num_components+=15) {
    //    // slice the eigenvectors from the model
    //    Mat evs = Mat(W, Range::all(), Range(0, num_components));
    //    Mat projection = subspaceProject(evs, mean, images[0].reshape(1,1));
    //    Mat reconstruction = subspaceReconstruct(evs, mean, projection);
    //    // Normalize the result:
    //    reconstruction = norm_0_255(reconstruction.reshape(1, images[0].rows));
    //    // Display or save:
    //    if(!bIsSaveMean) {
    //        imshow(format("eigenface_reconstruction_%d", num_components), reconstruction);
    //    } else {
    //        imwrite(format("%s/eigenface_reconstruction_%d.png", output_folder.c_str(), num_components), reconstruction);
    //    }
    //}
    // The following lines simply get the last images from
    // your dataset and remove it from the vector. This is
    // done, so that the training data (which we learn the
    // cv::FaceRecognizer on) and the test data we test
    // the model with, do not overlap.
    for (size_t i = 0; i < m_vecSourcePhotos.size(); i++) {
        Mat srcSample = cv::imread(m_vecSourcePhotos[i].string(), 0);
        if(!srcSample.data){
            std::cout << "Now: " << i << " Total: " << m_vecSourcePhotos.size()
                      << " " << m_vecSourcePhotos[i].string()
                      << " cannot open" << std::endl;
            continue;
        }
        Size size(100,100);
        Mat testSample;
        resize(srcSample, testSample, size);
        // The following line predicts the label of a given
        // test image:
        int predictedLabel = -1;
        double confidence = 0.0;
        model->predict(testSample, predictedLabel, confidence);
        // get result
        m_multimapResult.insert(std::make_pair(confidence, m_vecSourcePhotos[i]));
        //
        // To get the confidence of a prediction call the model with:
        //
        //      int predictedLabel = -1;
        //      double confidence = 0.0;
        //      model->predict(testSample, predictedLabel, confidence);
        //
        if(i % 1000 == 0){
            std::cout << "Now: " << i << " Total: " << m_vecSourcePhotos.size() << std::endl;
        }
    }
    OutputResult(strResultPath, nThreshold);
}

void TryFisherFaceRecognizer::GetFileList(const path& root
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

cv::Mat TryFisherFaceRecognizer::norm_0_255(InputArray _src){
    Mat src = _src.getMat();
    // Create and return normalized image:
    Mat dst;
    switch(src.channels()) {
    case 1:
        cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
        break;
    case 3:
        cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
        break;
    default:
        src.copyTo(dst);
        break;
    }
    return dst;
}

void TryFisherFaceRecognizer::read_csv(const std::string& filename
                                      , std::vector<Mat>& images
                                      , std::vector<int>& labels
                                      , char separator){
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}

void TryFisherFaceRecognizer::OutputResult(const std::string& strResultPath, const int& nThreshold){
    std::ofstream FH(strResultPath + "/result.txt", std::ios::out);
    int nCount = 0;
    //for(auto it = m_multimapResult.rbegin(); it != m_multimapResult.rend(); it++){
    for(auto it = m_multimapResult.begin(); it != m_multimapResult.end(); it++){
        // handle result text filename

        if(nCount < nThreshold){
            // output to result folder
            std::stringstream ss;
            ss << strResultPath << "/";
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
    FH.close();
}
