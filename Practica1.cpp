
#include<opencv2/opencv.hpp>
#include<iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;



Mat image;

// Valores para Contraste
int alfa = 100;
int beta = 0;
int ecualize = 0;
// Valores para RGB
int r_slider = 255;
int g_slider = 255;
int b_slider = 255;
// Valores para poster
int posterMod = 0;
// Valores para distorsión
int barrel = 0;
int cushion = 0;


// Modifica los niveles de los valores r, g y b
void alien(int i, int j) {
    image.at<Vec3b>(i, j)[2] = image.at<Vec3b>(i, j)[2] + r_slider;
    image.at<Vec3b>(i, j)[1] = image.at<Vec3b>(i, j)[1] + g_slider;
    image.at<Vec3b>(i, j)[0] = image.at<Vec3b>(i, j)[0] + b_slider;
}

// APlica un módulo para reducir el número de colores
void poster(int i, int j) {
    if (posterMod == 0) {
        posterMod = 1;
    }
    image.at<Vec3b>(i, j)[2] = (image.at<Vec3b>(i, j)[2] / posterMod) * posterMod;
    image.at<Vec3b>(i, j)[1] = (image.at<Vec3b>(i, j)[1] / posterMod) * posterMod;
    image.at<Vec3b>(i, j)[0] = (image.at<Vec3b>(i, j)[0] / posterMod) * posterMod;
}

void contraste_brillo() {
    Mat aux = Mat::zeros(image.size(), image.type());
    // Realiza a cada pixel la operación alfa*pixel+beta
    image.convertTo(aux, -1, float(alfa) / 100, beta);
    image = aux;

}

void distort(int type) {
    Mat cloned = image.clone();
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
        {

            int r = pow(i - (image.rows/2), 2) + pow(j - (image.cols/2), 2);
            int xClone, yClone;
            if (type == 1) {    // Barrel
                xClone = i + (i - (image.rows / 2)) * (float(barrel) / 100000000000) * pow(r, 2);
                yClone = j + (j - (image.cols / 2)) * (float(barrel) / 100000000000) * pow(r, 2);
            }
            else if (type == 2) {   // Cushion
                xClone = i - (i - (image.rows / 2)) * (float(cushion) / 100000000000) * pow(r, 2);
                yClone = j - (j - (image.cols / 2)) * (float(cushion) / 100000000000) * pow(r, 2);
            }
            if (xClone<0 || xClone>=image.rows || yClone<0 || yClone>=image.cols) {
                image.at<Vec3b>(i, j) = { 0, 0, 0 };
            }
            else {
                image.at<Vec3b>(i, j) = cloned.at<Vec3b>(xClone, yClone);
            }

        }
    }
}

void representar(VideoCapture cap) {
    while (true)
    {
        r_slider = getTrackbarPos("r", "trackbar panel");
        g_slider = getTrackbarPos("g", "trackbar panel");
        b_slider = getTrackbarPos("b", "trackbar panel");
        alfa = getTrackbarPos("alfa", "trackbar panel");
        beta = getTrackbarPos("beta", "trackbar panel");
        ecualize = getTrackbarPos("ecualize", "trackbar panel");
        barrel = getTrackbarPos("barrel", "trackbar panel");
        cushion = getTrackbarPos("cushion", "trackbar panel");
        cap >> image;
        resize(image, image, Size(360, 360));


        for (int i = 0; i < image.rows; i++)
        {
            for (int j = 0; j < image.cols; j++)
            {

                alien(i, j);
                poster(i, j);
                /*image.at<Vec3b>(i, j)[2] = image.at<Vec3b>(i, j)[2] + r_slider;
                image.at<Vec3b>(i, j)[1] = image.at<Vec3b>(i, j)[1] + g_slider;
                image.at<Vec3b>(i, j)[0] = image.at<Vec3b>(i, j)[0] + b_slider;*/

            }
        }
        contraste_brillo();

        if (ecualize == 1) {
            cvtColor(image, image, COLOR_BGR2GRAY);
            equalizeHist(image, image);
        }

        distort(1);
        distort(2);

        imshow("Display window", image); // Show our image inside it
        waitKey(25); // Wait for a keystroke i

    }
}


int main()
{

    //Poster -> reducir categorías de valores (de 255 a 8, por ejemplo, modulo para cada cnal)
    //Distorsion -> k1 debe ser baja, 


    namedWindow("trackbar panel", WINDOW_FREERATIO);

    createTrackbar("alfa", "trackbar panel", &alfa, 300);
    createTrackbar("beta", "trackbar panel", &beta, 100);
    createTrackbar("ecualize", "trackbar panel", &ecualize, 1);
    createTrackbar("r", "trackbar panel", &r_slider, 255);
    createTrackbar("g", "trackbar panel", &g_slider, 255);
    createTrackbar("b", "trackbar panel", &b_slider, 255);
    createTrackbar("poster", "trackbar panel", &posterMod, 255);
    createTrackbar("barrel", "trackbar panel", &barrel, 100);
    createTrackbar("cushion", "trackbar panel", &cushion, 100);

    namedWindow("Display window"); // Create a window for display
    VideoCapture cap(0);

    if (!cap.isOpened())
    { // check if video device has been initialised
        std::cout << "cannot open camera";
    }

    representar(cap);

    return 0;

}
