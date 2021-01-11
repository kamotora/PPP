#include<opencv2/opencv.hpp>
#include<iostream>

#include <windows.h>
#include <ppl.h>
#include <concurrent_vector.h>

#include <omp.h>

using namespace std;
using namespace cv;
using namespace concurrency;

const int KERNEL_WIDTH = 3;
const int KERNEL_HEIGHT = 3;
const double COEF = 3;
const int COUNT_THREADS = 4;
// Кол-во квадратов, на которые делим
const int N = 50;

// Получить ядро свёртки
vector<vector<double>> getKernel() {
    vector<vector<double>> kernel(KERNEL_WIDTH, vector<double>(KERNEL_HEIGHT));
    int size = KERNEL_WIDTH * KERNEL_HEIGHT - 1;
    for (int i = 0; i < KERNEL_WIDTH; i++)
        for (int j = 0; j < KERNEL_HEIGHT; j++)
            kernel[i][j] = -COEF / size;
    kernel[KERNEL_WIDTH / 2][KERNEL_HEIGHT / 2] = COEF + 1;
    return kernel;
}

// Для подсчёта времени выполнения
// Взято с официальной документации по concurrency
template <class Function>
__int64 time_call(Function&& f)
{
    __int64 begin = GetTickCount();
    f();
    return GetTickCount() - begin;
}

uchar value(double v)
{
    if (v < 0) return 0;
    if (v > 255) return 255;
    return (uchar)v;
}

// Применить фильтр к изображению imput
Mat applyFilter(Mat &input, vector<vector<double>> &kernel) {
    int rows = input.rows;
    int cols = input.cols;
    Mat res(rows, cols, input.type());
    int kernelRows = kernel.size();
    int kernelCols = kernel[0].size();

    //Производим вычисления
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            double  kSum = 0;
            double r = 0, g = 0, b = 0;
            for (int i = 0; i < kernelRows; i++)
            {
                for (int j = 0; j < kernelCols; j++)
                {
                    int pixelPosY = y + (i - (kernelRows / 2));
                    int pixelPosX = x + (j - (kernelCols / 2));
                    // Если выходим за границы изображения, 
                    // используем симметричный пиксель
                    if (pixelPosX < 0)
                        pixelPosX *= -1;
                    if (pixelPosY < 0)
                        pixelPosY *= -1;
                    if (pixelPosX >= cols)
                        pixelPosX = pixelPosX - (pixelPosX - cols + 1);
                    if (pixelPosY >= rows)
                        pixelPosY = pixelPosY - (pixelPosY - rows + 1);

                    double kernelVal = kernel[i][j];
                    kSum += kernelVal;
                    cv::Vec3b color = input.at<cv::Vec3b>(pixelPosY, pixelPosX);
                    r += color[2] * kernelVal;
                    g += color[1] * kernelVal;
                    b += color[0] * kernelVal;
                }
            }
            if (kSum <= 0)
                kSum = 1;

            // Нормировка
            r /= kSum;
            b /= kSum;
            g /= kSum;

            //Делаем новое изображение
            res.at<cv::Vec3b>(y, x) = cv::Vec3b(value(b), value(g), value(r));
            //cout << "BEFORE: " << input.at<cv::Vec3b>(y, x) << " AFTER: " << res.at<cv::Vec3b>(y, x) << endl;
        }
    }
    //Возвращаем отфильтрованное изображение
    return res;
}

// Разбить изображение img на N квадратов
// int - номер квадрата (чтобы потом правильно склеить)
// Mat - сам квадрат
vector<pair<int, Mat>> splitImage(Mat &img, int N) {
    vector<pair<int, Mat>> res;
    int deltaRows = img.rows / N;
    int deltaCols = img.cols / N;
    int count = 0;
    for (int r = 0; r < img.rows; r += deltaRows)
        for (int c = 0; c < img.cols; c += deltaCols)
        {
            cv::Mat tileCopy = img(cv::Range(r, min(r + deltaRows, img.rows)),
                cv::Range(c, min(c + deltaCols, img.cols)));
            res.push_back(make_pair(count++, tileCopy));
        }
    return res;
}

// Склеить изображения
Mat concatImages(concurrent_vector<pair<int, Mat>> & small_blocks, int rows, int cols) {
    // Сортируем по возрастанию номеров, чтобы склеить по порядку
    sort(small_blocks.begin(), small_blocks.end(), [&](pair<int, Mat>& i, pair<int, Mat>& j){
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
Mat parallel_ppl(Mat& input) {
    int rows = input.rows;
    int cols = input.cols;

    concurrent_vector<pair<int, Mat>> res;
    auto kernel = getKernel();
    Mat outputImg;
    __int64 elapsed = time_call([&]
        {
            //Делим изображение
            auto imgs = splitImage(input, N);
            parallel_for_each(begin(imgs), end(imgs), [&](pair<int, Mat>& pair) {
                res.push_back(make_pair(pair.first, applyFilter(pair.second, kernel)));
                });
            //Склеиваем
            outputImg = concatImages(res, rows, cols);
        });
    wcout << L"parallel_ppl time: " << elapsed << L" ms" << endl;
    return outputImg;
}


void omp_simple_for(vector<pair<int,Mat>> &imgs, concurrent_vector<pair<int, Mat>> &res, vector<vector<double>> &kernel) {
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
Mat parallel_omp(Mat& input, const string& funcName, function<void(vector<pair<int, Mat>>& imgs, concurrent_vector<pair<int, Mat>>& res, vector<vector<double>>& kernel)> ompFunc) {
    int rows = input.rows;
    int cols = input.cols;


    concurrent_vector<pair<int, Mat>> res;
    auto kernel = getKernel();
    Mat outputImg;

    //Делим изображение
    double start_time = omp_get_wtime();
    auto imgs = splitImage(input, N);
    ompFunc(imgs, res, kernel);
    //Склеиваем
    outputImg = concatImages(res, rows, cols);
    cout << funcName << " time: " << (omp_get_wtime() - start_time) * 1000 << " ms" << endl;
    return outputImg;
}

// Последовательный алгоритм
Mat serial(Mat& input) {
    vector<Mat> imgs = { input };
    vector<Mat> res;
    auto kernel = getKernel();
    __int64 elapsed = time_call([&]
    {
        for_each(begin(imgs), end(imgs), [&](Mat &input) {
            res.push_back(applyFilter(input,kernel));
            });
    });
    wcout << L"serial time: " << elapsed << L" ms" << endl;
    return res[0];
}

void output(const string& prefix, const string &imageName, Mat img) {
    namedWindow(prefix + "output");
    imshow(prefix + "output", img);
    imwrite(prefix + imageName, img);
}

int main(int argc, char* argv[])
{
    //Чтение изображения
    string IMAGE_NAME;
    const string PARALLEL_PPL_PREFIX = "parallel_ppl ";
    const string PARALLEL_OMP_PREFIX = "parallel_omp";
    const string SIMPLE_FOR_PREFIX = "_simple_for";
    const string STATIC_SCHEDULE_PREFIX = "_static";
    const string DYNAMIC_SCHEDULE_PREFIX = "_dynamic";
    const string SERIAL_PREFIX = "serial ";
    if (argc <= 1)
        IMAGE_NAME = "1.jpg";
    else
        IMAGE_NAME = argv[1];
    Mat img = imread(IMAGE_NAME);
    int rows = img.rows;
    int cols = img.cols;
    cout << "Size image: " << rows << 'x' << cols << endl;
    //Показать изобраение
    auto serialRes = serial(img);
    auto parallelPplRes = parallel_ppl(img);
    auto parallelOmpRes1 = parallel_omp(img, PARALLEL_OMP_PREFIX+SIMPLE_FOR_PREFIX, omp_simple_for);
    auto parallelOmpRes2 = parallel_omp(img, PARALLEL_OMP_PREFIX + STATIC_SCHEDULE_PREFIX, omp_for_with_num_threads);
    auto parallelOmpRes3 = parallel_omp(img, PARALLEL_OMP_PREFIX + DYNAMIC_SCHEDULE_PREFIX, omp_for_dynamic);
    cout << COUNT_THREADS << " threads " << endl;
    cout <<  "N = " << N << endl;
    namedWindow("input");
    imshow("input", img);
    

    //output(SERIAL_PREFIX, IMAGE_NAME, serialRes);

    output(PARALLEL_PPL_PREFIX, IMAGE_NAME, parallelPplRes);

    output(PARALLEL_OMP_PREFIX + SIMPLE_FOR_PREFIX, IMAGE_NAME, parallelOmpRes1);

    output(PARALLEL_OMP_PREFIX + STATIC_SCHEDULE_PREFIX, IMAGE_NAME, parallelOmpRes2);

    output(PARALLEL_OMP_PREFIX + DYNAMIC_SCHEDULE_PREFIX, IMAGE_NAME, parallelOmpRes3);
    
    waitKey(0);
    return 0;
}
