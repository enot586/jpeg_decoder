#ifndef JPEGDECODER_H
#define JPEGDECODER_H

#include "JpegSections.h"

#include <map>
using namespace std;


class JpegDecoder {
  public:
    JpegDecoder(JpegSections& sections_);
    virtual ~JpegDecoder();
    void run();
  protected:

    struct HuffTableContext{;
      std::vector<int> HUFFSIZE;
      std::vector<uint16_t> HUFFCODE;
      std::vector<uint16_t> HUFFVAL[16];
      std::vector<uint16_t> MINCODE;
      std::vector<uint16_t> MAXCODE;
    };

    std::map < uint8_t, HuffTableContext > huffDecodeTables;

    void GenerateHUFFSIZE(const uint8_t* BITS, std::vector<int>& HUFFSIZE);
    void GenerateHUFFCODE(const std::vector<int>& HUFFSIZE,std::vector<uint16_t>& HUFFCODE);
    void GenerateDecoderTables(const uint8_t* BITS, const std::vector<uint16_t>& HUFFCODE,
                               std::vector<uint16_t>& MINCODE, std::vector<uint16_t>& MAXCODE);

    uint8_t Decode(const std::vector<uint16_t>& MINCODE, const std::vector<uint16_t>& MAXCODE,
                   const std::vector<uint16_t> HUFFVAL[]);

    void DecodeNextBlock(ZZMatrix<int, 8, 8>& block);
    void DecodeBlock(int Cid, ZZMatrix<int, 8, 8>& block);
    int16_t EXTEND(int V, int T);
    uint8_t ReadNextBit();

    int DCDecode(int Cid);
    int DecodeFromBitLength(int bitLength);

  private:
    JpegSections& sections;
    uint8_t currentByte = 0;
    uint8_t cnt = 0;

};

#endif // JPEGDECODER_H
