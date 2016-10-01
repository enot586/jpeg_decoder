#include "JpegDecoder.h"
#include <iostream>


JpegDecoder::JpegDecoder(JpegSections& sections_) : sections(sections_)
{
  GenerateHUFFSIZE( sections.DHT.tables.front().L, HUFFSIZE_ );
  GenerateHUFFCODE( HUFFSIZE_, HUFFCODE_ );
  GenerateDecoderTables( sections.DHT.tables.front().L,
                         HUFFCODE_, MINCODE_, MAXCODE_, VALPTR_ );
}

JpegDecoder::~JpegDecoder()
{
  //dtor
}

void JpegDecoder::GenerateHUFFSIZE(const uint8_t* BITS, std::vector<int>& HUFFSIZE) {
  int totalSize = 0;
  for (int i = 0; i < 16; ++i) {
    totalSize+= BITS[i];
  }

  HUFFSIZE.resize(totalSize+1);

  for (int i = 0; i < HUFFSIZE.size(); ++i) {
    HUFFSIZE[i] = 0;
  }

  int k = 0;
  int i = 0;
  int j = 1;

  while ( i < 16 ) {
    if (j <= BITS[i]) {
      HUFFSIZE[k] = i;
      std::cout << "HUFFSIZE["<<k<<"] = " << HUFFSIZE[k] << endl;
      ++k;
      ++j;
    } else {
      ++i;
      j = 1;
    }
  }

  HUFFSIZE[k] = 0;
}

void JpegDecoder::GenerateHUFFCODE(const std::vector<int>& HUFFSIZE,
                                   std::vector<int>& HUFFCODE) {

  HUFFCODE.resize( HUFFSIZE.size() );

  for (int i = 0; i < HUFFCODE.size(); ++i) {
    HUFFCODE[i] = 0;
  }

  int code = 0;
  int si = HUFFSIZE[0];

  for (int k = 0; k < HUFFSIZE.size(); ++k) {
    HUFFCODE[k] = code;
    std::cout << "HUFFCODE["<<k<<"] = " << HUFFCODE[k] << endl;
    ++code;

    while ( HUFFSIZE[k] != si ) {
      if ( HUFFSIZE[k] == 0 ) {
        return;
      }
      code = code << 1;
      ++si;
    }
  }
}

void JpegDecoder::GenerateDecoderTables(const uint8_t* BITS,
                                        const std::vector<int>& HUFFCODE,
                                        std::vector<int>& MINCODE,
                                        std::vector<int>& MAXCODE,
                                        std::vector<int>& VALPTR) {
  MAXCODE.resize(16);
  MINCODE.resize(16);
  VALPTR.resize(16);

  for (int i = 0; i < 16; ++i) {
    MAXCODE[i] = 0;
    MINCODE[i] = 0;
    VALPTR[i] = 0;
  }

  int j = 0;
  for (int i = 0; i < 16; ++i) {
    if (BITS[i] == 0) {
      MAXCODE[i] = -1;
    } else {
      VALPTR[i] = j;
      MINCODE[i] = HUFFCODE[j];
      j = j + BITS[i] - 1;
      MAXCODE[i] = HUFFCODE[j];
      std::cout << "MINCODE["<<i<<"] = " << MINCODE[i];
      std::cout << "\t MAXCODE["<<i<<"] = " << MAXCODE[i];
      std::cout << "\t VALPTR["<<i<<"] = " << VALPTR[i] << endl;
      ++j;
    }
  }
}

uint8_t JpegDecoder::ReadNextBit() {
  if (!cnt) {
    do {
      currentByte = sections.ReadByteFromImage();
      cnt = 8;

      if (currentByte == 0xFF) {
        uint8_t currentByte2 = sections.ReadByteFromImage();

        if ( (currentByte2 >= sections.RST0[1]) || (currentByte2 <= sections.RST7[1]) ) {
          //RSTn marker
          cnt = 0;
        } else if ( currentByte == sections.DNL.marker[1] ) {
          cnt = 0;
          //Process DNL
        } if (currentByte2 == 0) {
          //decode byte 0xFF of entropy
        } else {
          //Error
          cnt = 0;
          currentByte = 0;
          return 0x00;
        }
      }
    } while (!cnt /*&& !afile.eof()*/);
  }

  if (cnt) {
    uint8_t result = (currentByte & 0x80) >> 7; //reading MSB
    --cnt;
    currentByte = currentByte << 1;
    return result;
  }
}

uint8_t JpegDecoder::Decode(const std::vector<int>& MINCODE,
                            const std::vector<int>& MAXCODE,
                            const std::vector<int>& VALPTR,
                            const std::vector<int>& HUFFVAL) {
  int i = 0;
  int code = ReadNextBit();

  while (i < 16) {
    if (code > MAXCODE[i]) {
      ++i;
      code = (code << 1) | ReadNextBit();
    } else {
      int j = VALPTR[i];
      j = j+code-MINCODE[i];
      return HUFFVAL[j];
    }
  }

  return 0;
}

void JpegDecoder::DecodeNextBlock(ZZMatrix<int, 8, 8>& block) {






}


