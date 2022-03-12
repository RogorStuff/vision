#include <opencv2/opencv.hpp>
#include <iostream>
#include "math.h"

#define PI 3.14159265

// using namespaces to nullify use of cv::function(); syntax and std::function();
using namespace std;
using namespace cv;

// Funcion tomada de https://bitwizx.tistory.com/65
Mat norm_img_8bit(Mat& src) { 
    double min, max; 
    minMaxIdx(src, &min, &max); 
    Mat dst = src - min; 
    dst.convertTo(dst, CV_8UC1, 255.0 / (max - min)); 
    return dst; 
}


int main()
{
    // Parte 1

    // Leemos y mostramos la imagen original
    Mat img = imread("poster.pgm");
    imshow("original Image", img);
    waitKey(0);

    // Convertimos la imagen a escala de grises, y aplicamos un filtro gausiano de blur
    Mat img_gray;
    cvtColor(img, img_gray, COLOR_BGR2GRAY);
    Mat img_blur;
    int gradient = 0;
    GaussianBlur(img_gray, img_blur, Size(3, 3), gradient);

    // Buscamos los bordes mediante sobel y los mostramos
    Mat sobelx, sobely;
    Sobel(img_blur, sobelx, CV_64F, 1, 0, 3);
    Sobel(img_blur, sobely, CV_64F, 0, 1, 3);

    imshow("Sobel X", norm_img_8bit(sobelx));
    waitKey(0);
    imshow("Sobel Y", norm_img_8bit(sobely));
    waitKey(0);

    // Calculo de la orientación y magnitud de la imagen
    Mat mag, orient;

    // Instrucciones tomadas de https://bitwizx.tistory.com/65
    phase(sobelx, sobely, orient, true);
    mag = sobelx.mul(sobelx) + sobely.mul(sobely);
    cv::sqrt(mag, mag);

    imshow("Magnitude", norm_img_8bit(mag));
    imshow("Orientation", norm_img_8bit(orient));
    waitKey(0);

    //Parte 2

    // Cargamos la imagen en escala de grises, 
    Mat imgPasillo = imread("pasillo2.pgm", IMREAD_GRAYSCALE);
    imshow("original corridor", imgPasillo);
    waitKey(0);

    // Detección de bordes mediante Canny
    Mat imageCanny;
    Canny(imgPasillo, imageCanny, 50, 200, 3);

    // https://docs.opencv.org/3.4/d9/db0/tutorial_hough_lines.html
    vector<Vec2f> lines, approvedLines; // Resultados de la detección de líneas
    HoughLines(imageCanny, lines, 1, CV_PI / 270, 100, 0, 0); // Detección de líneas

    // Dibujamos las líneas en la imagen
    for (size_t i = 0; i < lines.size(); i++)
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        pt1.x = cvRound(x0 + 1000 * (-b));
        pt1.y = cvRound(y0 + 1000 * (a));
        pt2.x = cvRound(x0 - 1000 * (-b));
        pt2.y = cvRound(y0 - 1000 * (a));

        if (theta > 3.1415F) {
            theta = theta - 3.1415;
        }
        if ((theta > 0.2 && theta < 3.0) && !(theta > 1.5 && theta < 1.65 )) {
            if (theta > 0.0f) {
                //line(imgPasillo, pt1, pt2, Scalar(0, 0, 255), 1, LINE_AA);
                approvedLines.push_back(lines[i]);
            }
        }
    }

    float puntoMedio=0;
    float valoresEnMedio=0;
    for (size_t i = 0; i < approvedLines.size(); i++)
    {
        for (size_t j = i; j < approvedLines.size(); j++)
        {
            float rho = approvedLines[i][0], theta = approvedLines[i][1];
            float rho2 = approvedLines[j][0], theta2 = approvedLines[j][1];

            // https://stackoverflow.com/questions/383480/intersection-of-two-lines-defined-in-rho-theta-parameterization
            float ct1 = cosf(theta);      //matrix element a
            float st1 = sinf(theta);      //b
            float ct2 = cosf(theta2);     //c
            float st2 = sinf(theta2);     //d
            float d = ct1 * st2 - st1 * ct2;        //determinative (rearranged matrix for inverse)
            if (abs(d) > 0.2f) {
                //int x = (int)((st2 * rho - st1 * rho2) / d);
                int y = (int)((-ct2 * rho + ct1 * rho2) / d);
                puntoMedio += y;
                valoresEnMedio++;
                //circle(imgPasillo, Point(x, y), 5, Scalar(0, 255, 0));
            }
        }
    }
    puntoMedio = puntoMedio / valoresEnMedio;
    line(imgPasillo, Point(0,puntoMedio), Point(imgPasillo.cols, puntoMedio), Scalar(0, 0, 255), 5, LINE_AA);

    imshow("Punto central de un pasillo", imgPasillo);
    waitKey(0);


    destroyAllWindows();
    return 0;
}