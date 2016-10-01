#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>

#include <iostream>

#include "JpegDecoder.h"
#include "ZZMatrix.h"

using namespace cv;
using namespace std;




int main(int argc, char *argv[])
{
  Mat img = imread("lena.jpg", CV_LOAD_IMAGE_COLOR);
  if(img.empty())
     return -1;


  try {
    string file("lena.jpg");
    JpegSections jpegHeader(file);
    JpegDecoder jpegDecoder(jpegHeader);

  } catch (std::exception& e) {

  }

  namedWindow( "lena", CV_WINDOW_AUTOSIZE );
  imshow("lena", img);

  waitKey(0);
  return 0;
}



