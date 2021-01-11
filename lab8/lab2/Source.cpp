#include <ppl.h>
#include <concurrent_vector.h>
#include <chrono>
#include <omp.h>
#include <ctime>
#include<opencv2/opencv.hpp>

using namespace concurrency;
using namespace std;
using namespace cv;

// Для подсчёта времени выполнения
// Взято с официальной документации по concurrency
template <class Function>
__int64 time_call(Function&& f)
{
    __int64 begin = GetTickCount();
    f();
    return GetTickCount() - begin;
}

vector<pair<int, Mat>> splitImage2(Mat& img) {
    vector<pair<int, Mat>> res;
    int deltaRows = PART_ROWS;
    int deltaCols = PART_COLS;
    int count = 0;
    int r = 0, c = 0;
    for (int r = 0; r < img.rows; r += deltaRows)
        for (int c = 0; c < img.cols; c += deltaCols)
        {
            cv::Mat tileCopy = img(cv::Range(r, min(r + deltaRows, img.rows)),
                cv::Range(c, min(c + deltaCols, img.cols)));
            res.push_back(make_pair(count++, tileCopy));
        }
    N = count;
    return res;
}

// Склеить изображения
Mat concatImages2(concurrent_vector<pair<int, Mat>>& small_blocks, int rows, int cols) {
    // Сортируем по возрастанию номеров, чтобы склеить по порядку
    sort(small_blocks.begin(), small_blocks.end(), [&](pair<int, Mat>& i, pair<int, Mat>& j) {
        return i.first < j.first;
        });
    Mat res(rows, cols, (small_blocks[0].second).type());
    int x = 0, y = 0;
    for (pair<int, Mat> p : small_blocks)
    {
        Mat img = p.second;
        cv::Mat roi = res(cv::Rect(x, y, img.cols, img.rows));
        img.copyTo(roi);
        x += img.cols;
        if (x >= cols) {
            y += img.rows;
            x = 0;
        }

    }
    return res;
}



// Параллельный алгоритм
Mat parallel_ppl(Mat& input, ofstream& outFile) {
    int rows = input.rows;
    int cols = input.cols;

    concurrent_vector<pair<int, Mat>> res;
    auto kernel = getKernel();
    Mat outputImg;
    __int64 elapsed = time_call([&]
        {
            //Делим изображение
            auto imgs = splitImage(input);
            parallel_for_each(begin(imgs), end(imgs), [&](ImageInfo& info) {
                res.push_back(make_pair(info.num, applyFilter(info.img, kernel)));
                });
            //Склеиваем
            outputImg = concatImages2(res, rows, cols);
        });
    outFile << "parallel_ppl time: " << elapsed << " ms" << endl;
    return outputImg;
}


void omp_simple_for(vector<pair<int, Mat>>& imgs, concurrent_vector<pair<int, Mat>>& res, vector<vector<double>>& kernel) {
    int imgsCount = imgs.size();
#pragma omp parallel for
    for (int i = 0; i < imgsCount; i++) {
        //cout << omp_get_thread_num();
        res.push_back(make_pair(imgs[i].first, applyFilter(imgs[i].second, kernel)));
    }
}

void omp_for_with_num_threads(vector<pair<int, Mat>>& imgs, concurrent_vector<pair<int, Mat>>& res, vector<vector<double>>& kernel) {
    int imgsCount = imgs.size();
#pragma omp parallel for num_threads(COUNT_THREADS)
    for (int i = 0; i < imgsCount; i++) {
        //cout << omp_get_thread_num();
        res.push_back(make_pair(imgs[i].first, applyFilter(imgs[i].second, kernel)));
    }
}

void omp_for_dynamic(vector<pair<int, Mat>>& imgs, concurrent_vector<pair<int, Mat>>& res, vector<vector<double>>& kernel) {
    int imgsCount = imgs.size();
#pragma omp parallel for schedule(dynamic, COUNT_THREADS)
    for (int i = 0; i < imgsCount; i++) {
        //cout << omp_get_thread_num();
        res.push_back(make_pair(imgs[i].first, applyFilter(imgs[i].second, kernel)));
    }
}

// Параллельный алгоритм
Mat parallel_omp(Mat& input, const string& funcName, ofstream& outFile, function<void(vector<pair<int, Mat>>& imgs, concurrent_vector<pair<int, Mat>>& res, vector<vector<double>>& kernel)> ompFunc) {
    int rows = input.rows;
    int cols = input.cols;


    concurrent_vector<pair<int, Mat>> res;
    auto kernel = getKernel();
    Mat outputImg;

    //Делим изображение
    double start_time = omp_get_wtime();
    auto imgs = splitImage2(input);
    ompFunc(imgs, res, kernel);
    //Склеиваем
    outputImg = concatImages2(res, rows, cols);
    outFile << funcName << " time: " << (omp_get_wtime() - start_time) * 1000 << " ms" << endl;
    return outputImg;
}

// Последовательный алгоритм
Mat serial(Mat& input, ofstream& outFile) {
    vector<Mat> imgs = { input };
    vector<Mat> res;
    auto kernel = getKernel();
    __int64 elapsed = time_call([&]
        {
            for_each(begin(imgs), end(imgs), [&](Mat& input) {
                res.push_back(applyFilter(input, kernel));
                });
        });
    outFile << "serial time: " << elapsed << " ms" << endl;
    return res[0];
}

void test(Mat img, ofstream& outFile) {
    const string PARALLEL_PPL_PREFIX = "parallel_ppl ";
    const string PARALLEL_OMP_PREFIX = "parallel_omp";
    const string SIMPLE_FOR_PREFIX = "_simple_for";
    const string STATIC_SCHEDULE_PREFIX = "_static";
    const string DYNAMIC_SCHEDULE_PREFIX = "_dynamic";
    const string SERIAL_PREFIX = "serial ";

    auto serialRes = serial(img, outFile);
    auto parallelPplRes = parallel_ppl(img, outFile);
    auto parallelOmpRes1 = parallel_omp(img, PARALLEL_OMP_PREFIX + SIMPLE_FOR_PREFIX, outFile, omp_simple_for);
    auto parallelOmpRes2 = parallel_omp(img, PARALLEL_OMP_PREFIX + STATIC_SCHEDULE_PREFIX, outFile, omp_for_with_num_threads);
    auto parallelOmpRes3 = parallel_omp(img, PARALLEL_OMP_PREFIX + DYNAMIC_SCHEDULE_PREFIX, outFile, omp_for_dynamic);
}
