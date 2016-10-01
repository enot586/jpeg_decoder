#ifndef JPEGDECODER_H
#define JPEGDECODER_H

#include "JpegSections.h"


using namespace std;


class JpegDecoder {
  public:
    JpegDecoder(JpegSections& sections_);
    virtual ~JpegDecoder();

  protected:
    std::vector<int> HUFFSIZE_;
    std::vector<int> HUFFCODE_;
    std::vector<int> MINCODE_;
    std::vector<int> MAXCODE_;
    std::vector<int> VALPTR_;

    void GenerateHUFFSIZE(const uint8_t* BITS, std::vector<int>& HUFFSIZE);
    void GenerateHUFFCODE(const std::vector<int>& HUFFSIZE,std::vector<int>& HUFFCODE);
    void GenerateDecoderTables(const uint8_t* BITS, const std::vector<int>& HUFFCODE,
                               std::vector<int>& MINCODE, std::vector<int>& MAXCODE, std::vector<int>& VALPTR);

    uint8_t Decode(const std::vector<int>& MINCODE, const std::vector<int>& MAXCODE,
                   const std::vector<int>& VALPTR, const std::vector<int>& HUFFVAL);

    void DecodeNextBlock(ZZMatrix<int, 8, 8>& block);
    uint8_t ReadNextBit();

  private:
    JpegSections& sections;
    uint8_t currentByte = 0;
    uint8_t cnt = 0;
};

#endif // JPEGDECODER_H
