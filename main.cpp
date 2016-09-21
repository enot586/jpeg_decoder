#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include "JpegDecoder.h"

using namespace cv;
using namespace std;


JpegSections jpeg;


int main(int argc, char *argv[])
{
    Mat img = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
    if(img.empty())
       return -1;

    string file("lena.jpg");

    jpeg.jpegReadSOF( file );

    namedWindow( "lena", CV_WINDOW_AUTOSIZE );
    imshow("lena", img);

    waitKey(0);
    return 0;
}


