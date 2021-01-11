#include<opencv2/opencv.hpp>
#include<iostream>
#include <windows.h>
#include <mpi.h>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

const int KERNEL_WIDTH = 3;
const int KERNEL_HEIGHT = 3;
const double COEF = 3;
int COUNT_THREADS;

int PART_ROWS = 512;
int PART_COLS = 512;
// Кол-во квадратов, на которые делим
int N;
const uchar END_MSG = -10;

struct ImageInfo
{
    int num;
    Mat img;
    const int NULL_IMG = -1;

    ImageInfo() {
        num = NULL_IMG;
    };

    ImageInfo(Mat image, int num) {
        this->num = num;
        this->img = image;
    }

    bool isNull() {
        return num == NULL_IMG;
    }
};

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
        }
    }
    //Возвращаем отфильтрованное изображение
    return res;
}

// Разбить изображение img на квадраты PART_ROWSxPART_COLS
vector<ImageInfo> splitImage(Mat &img) {
    vector<ImageInfo> res;
    int deltaRows = PART_ROWS;
    int deltaCols = PART_COLS;
    int count = 0;
    int r = 0, c = 0;
    for (int r = 0; r < img.rows; r += deltaRows)
        for (int c = 0; c < img.cols; c += deltaCols)
        {
            cv::Mat tileCopy = img(cv::Range(r, min(r + deltaRows, img.rows)),
                cv::Range(c, min(c + deltaCols, img.cols)));
            res.push_back(ImageInfo(tileCopy, count++));
        }
    N = count;
    return res;
}

// Склеить изображения
Mat concatImages(vector<pair<int, Mat>> & small_blocks, int rows, int cols) {
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

void sendImage(int destThread, ImageInfo info) {

    Mat& img = info.img;
    int rows = info.img.rows;
    int cols = info.img.cols;
    int type = info.img.type();
    int channels = info.img.channels();
    int imageNum = info.num;

    int bytes = rows * cols * channels;
    int size = bytes + sizeof(int) * 4;
    uchar *buffer = new uchar[size];

    memcpy(&buffer[0 * sizeof(int)], (uchar*)&rows, sizeof(int));
    memcpy(&buffer[1 * sizeof(int)], (uchar*)&cols, sizeof(int));
    memcpy(&buffer[2 * sizeof(int)], (uchar*)&type, sizeof(int));
    memcpy(&buffer[3 * sizeof(int)], (uchar*)&imageNum, sizeof(int));

    if (!img.isContinuous())
    {
        img = img.clone();
    }
    memcpy(&buffer[4 * sizeof(int)], img.data, bytes);
    MPI_Request request;
    MPI_Isend(buffer, size, MPI_UNSIGNED_CHAR, destThread, 0, MPI_COMM_WORLD, &request);
    MPI_Request_free(&request);
    //delete[] buffer;
}

ImageInfo receiveImage(int sourceThread) {
    MPI_Status status;
    int hasMessage;
    int size, rows, cols, type, channels, num;
    MPI_Probe(sourceThread, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    
    //Главный процесс может получить с любого источника, кроме себя самого
    if (sourceThread == MPI_ANY_SOURCE && status.MPI_SOURCE == 0)
        return ImageInfo();
    MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &size);

    uchar* buffer = new uchar[size];
    MPI_Recv(buffer,size, MPI_UNSIGNED_CHAR, sourceThread, 0, MPI_COMM_WORLD, &status);

    if(size == sizeof(uchar) && *buffer == END_MSG)
        return ImageInfo();

    memcpy((uchar*)&rows, &buffer[0 * sizeof(int)], sizeof(int));
    memcpy((uchar*)&cols, &buffer[1 * sizeof(int)], sizeof(int));
    memcpy((uchar*)&type, &buffer[2 * sizeof(int)], sizeof(int));
    memcpy((uchar*)&num, &buffer[3 * sizeof(int)], sizeof(int));
    // Make the mat
    Mat received = Mat(rows, cols, type, (uchar*)&buffer[4 * sizeof(int)]);
    //delete[] buffer;
    return ImageInfo(received, num);
}

void output(const string& prefix, const string &imageName,const Mat &img) {
    namedWindow(prefix + "output");
    imshow(prefix + "output", img);
    imwrite(prefix + imageName, img);
}

// Сообщить рабочим потокам, что работы больше не будет 
void sendExitMsg(int countThreads) {
    for(int i = 1; i < countThreads; i++)
        MPI_Send(&END_MSG, sizeof(uchar), MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD);
}

int main(int argc, char* argv[])
{
    int rank;
    Mat resultImage;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &COUNT_THREADS);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //cout << "The number of processes: " << COUNT_THREADS << " my number is " << rank << endl;
    if (rank == 0) {
       
        // Чтение изображения
        string IMAGE_NAME;
        const string PARALLEL_MPI_PREFIX = "parallel_mpi ";
        if(argc >= 2)
            IMAGE_NAME = argv[1];
        else
            IMAGE_NAME = "1.jpg";
        if (argc >= 2)
            PART_ROWS = PART_COLS = atoi(argv[2]);

        Mat img = imread(IMAGE_NAME);
        int rows = img.rows;
        int cols = img.cols;
        cout << "Size image: " << rows << 'x' << cols << endl;
        double startTime = MPI_Wtime();
        auto images = splitImage(img);
        if (N != images.size()){
            cerr << "error in sizes, check split function!!!" << endl;
            exit(0);
        }
        int curDestThread = 1;
        for (int i = 0; i < N; i++) {
            int imgNum = images[i].num;
            sendImage(curDestThread++, images[i]);
            //cout << "Was sended " << imgNum << " part of image from " << 0 << " thread in " << curDestThread << " thread" << endl;
            if (curDestThread == COUNT_THREADS)
                curDestThread = 1;
        }
        vector<pair<int, Mat>> resImgs;
        for (int i = 0; i < N; i++) {
            ImageInfo info = receiveImage(MPI_ANY_SOURCE); 
            if (!info.isNull()) {
                //cout << "Was received " << info.num << " part of image in " << 0 << " thread" << endl;
                resImgs.push_back(make_pair(info.num, info.img));
            }
            else{
                --i;
            }
        }
        double finishTime = MPI_Wtime();
        ofstream outFile("output.dat", ios::out | ios::binary | ios::app);
        outFile << "N = " << N << endl;
        outFile << "Size image: " << rows << 'x' << cols << endl;
        outFile << "Thread (worked) = " << COUNT_THREADS - 1 << endl;
        outFile << "MPI Time: " << round((finishTime - startTime) * 1000) << " ms" << endl;
        cout << "N = " << N << endl;
        cout << "Thread (worked) = " << COUNT_THREADS - 1 << endl;
        cout << "MPI Time: " << round((finishTime - startTime) * 1000) << " ms" << endl;
        cout << "=====================================================================" << endl;
        //test(img, outFile);
        namedWindow("input");
        imshow("input", img);

        output(PARALLEL_MPI_PREFIX, IMAGE_NAME, concatImages(resImgs, rows, cols));

        waitKey(0);
        outFile << "=====================================================================" << endl;
        outFile.close();
        sendExitMsg(COUNT_THREADS);
    }
    else {
        do {
            ImageInfo info = receiveImage(0);
            if (info.isNull())
                break;
            //cout << "Was received " << info.num << " part of image from " << 0 << " thread in " << rank << " thread" << endl;
            auto kernel = getKernel();
            Mat updatedImage = applyFilter(info.img, kernel);
            info.img = updatedImage;
            sendImage(0, info);
            //cout << "Was sended " << info.num << " part of image from " << rank << " thread in " << 0 << " thread" << endl;
        } while (true); //Один процесс может обработать несколько изображений
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    exit(0);
}