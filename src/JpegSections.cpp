#include "JpegSections.h"
#include <fstream>
#include <exception>
#include <iostream>

using namespace std;

JpegSections::JpegSections()
{
  //ctor
}

JpegSections::~JpegSections()
{
  //dtor
  this->jpegSOF0Components.clear();
  this->jpegSOSComponents.clear();
}

void JpegSections::jpegReadSOF(const std::string& fileName) {

  std::ifstream  afile;
  afile.open(fileName , ios::in);

  uint8_t markerBuffer;
  int i = 0;
  int bytesNumber = 0;

  while ( !afile.eof() ) {
    try {
      afile >> markerBuffer;

      if (afile.eof()) {
        break;
      }

      if (markerBuffer == jpegSOF0Section.prefix[0]) {

        afile >> markerBuffer;

        if (markerBuffer == jpegSOF0Section.prefix[1]) {
          afile >> markerBuffer;
          jpegSOF0Section.jpegSOF0Size[0] = markerBuffer;

          afile >> markerBuffer;
          jpegSOF0Section.jpegSOF0Size[1] = markerBuffer;

          afile >> markerBuffer;
          jpegSOF0Section.jpegSOF0Precision  = markerBuffer;

          afile >> markerBuffer;
          jpegSOF0Section.jpegSOF0Height[0] = markerBuffer;
          afile >> markerBuffer;
          jpegSOF0Section.jpegSOF0Height[1] = markerBuffer;

          afile >> markerBuffer;
          jpegSOF0Section.jpegSOF0Width[0] = markerBuffer;

          afile >> markerBuffer;
          jpegSOF0Section.jpegSOF0Width[1] = markerBuffer;

          afile >> markerBuffer;
          jpegSOF0Section.jpegSOF0ComponentNum  = markerBuffer;

          return;
        }
      }
    } catch (std::exception& e) {
      std::cout << "Exception !\n";
    }
  }
}
