#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include <iostream>

#include "JpegDecoder.h"
#include "ZZMatrix.h"

using namespace cv;
using namespace std;


JpegSections jpeg;


int main(int argc, char *argv[])
{

  ZZMatrix<int, 5, 5> m;

  m.Init(0);

  for (int i = 0; i < 25; ++i) {
      int g;
    std::cin >> g;
    m.Push(g);

    for (int y = 0; y < 5; ++y) {
      for (int x = 0; x < 5; ++x) {
        std::cout << m.Get(y,x) << " ";

      }
      std::cout << endl;
    }
    std::cout << endl;
  }

/*
    Mat img = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
    if(img.empty())
       return -1;

    string file("lena.jpg");

    try {
      jpeg.AssignFile( file );
    } catch (std::invalid_argument& e) {

    }

    namedWindow( "lena", CV_WINDOW_AUTOSIZE );
    imshow("lena", img);
*/
    waitKey(0);
    return 0;
}



