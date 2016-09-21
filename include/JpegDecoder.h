#ifndef JPEGDECODER_H
#define JPEGDECODER_H

#include "JpegSections.h"


using namespace std;


class JpegDecoder {

  private:
  JpegSections sections;

  public:
  JpegDecoder();
  virtual ~JpegDecoder();

};

#endif // JPEGDECODER_H
