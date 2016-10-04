#ifndef JPEGDECODER_H
#define JPEGDECODER_H

#include "JpegSections.h"

#include <map>
using namespace std;


class JpegDecoder {
  public:
    JpegDecoder(JpegSections& sections_);
    virtual ~JpegDecoder();

  protected:

    struct HuffTableContext{;
      std::vector<int> HUFFSIZE;
      std::vector<int> HUFFCODE;
      std::vector<int> HUFFVAL[16];
      std::vector<int> MINCODE;
      std::vector<int> MAXCODE;
    };

    std::map < uint8_t, HuffTableContext > huffDecodeTables;

    void GenerateHUFFSIZE(const uint8_t* BITS, std::vector<int>& HUFFSIZE);
    void GenerateHUFFCODE(const std::vector<int>& HUFFSIZE,std::vector<int>& HUFFCODE);
    void GenerateDecoderTables(const uint8_t* BITS, const std::vector<int>& HUFFCODE,
                               std::vector<int>& MINCODE, std::vector<int>& MAXCODE);

    uint8_t Decode(const std::vector<int>& MINCODE, const std::vector<int>& MAXCODE,
                   const std::vector<int> HUFFVAL[]);

    void DecodeNextBlock(ZZMatrix<int, 8, 8>& block);
    void DecodeBlock(int Cid, ZZMatrix<int, 8, 8>& block);
    int EXTEND(int V, int T);
    uint8_t ReadNextBit();

    int DCDecode(int Cid);
    int DecodeFromBitLength(int bitLength);

  private:
    JpegSections& sections;
    uint8_t currentByte = 0;
    uint8_t cnt = 0;
};

#endif // JPEGDECODER_H
