#include "JpegDecoder.h"
#include <iostream>
#include <algorithm>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>



JpegDecoder::JpegDecoder(JpegSections& sections_) : sections(sections_) {

  if ( sections.GetComponentsNumber() ) {
    dcDiff.resize( sections.GetComponentsNumber() );

    for (int i = 0; i < dcDiff.size(); ++i) {
      dcDiff[i] = 0;
    }
  } else {
    throw std::invalid_argument("Error: Unsupported amount of image components");
  }

  for (std::list<JpegSections::DHTTableElement>::iterator it = sections.DHT.tables.begin();
       it != sections.DHT.tables.end(); ++it) {
    HuffTableContext buffer;

    GenerateHUFFSIZE( it->L, buffer.HUFFSIZE );
    GenerateHUFFCODE( buffer.HUFFSIZE, buffer.HUFFCODE );
    GenerateDecoderTables( it->L,
                           buffer.HUFFCODE, buffer.MINCODE, buffer.MAXCODE );

    for (int i = 0; i < 16; ++i) {
      buffer.HUFFVAL[i].resize(it->V[i].size());
      copy ( it->V[i].begin(), it->V[i].end(), buffer.HUFFVAL[i].begin() );
      //std::cout << "HUFFVAL[" << i << "] size = " << buffer.HUFFVAL[i].size() << std::endl;
    }

    uint8_t huffmanTableId = (it->Tc << 4) | (it->Th);

    //insert new decode huffman tables
    huffDecodeTables[huffmanTableId] = buffer;
  }
}

JpegDecoder::~JpegDecoder() {
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
      HUFFSIZE[k] = i+1;
//      std::cout << "HUFFSIZE[" << std::dec << k << "] = 0x" << std::hex << HUFFSIZE[k] << endl;
      ++k;
      ++j;
    } else {
      ++i;
      j = 1;
    }
  }

  if (k != (totalSize) )
    throw std::length_error("Error: Unsupported header foramt");
}

void JpegDecoder::GenerateHUFFCODE(const std::vector<int>& HUFFSIZE,
                                   std::vector<int16_t>& HUFFCODE) {

  HUFFCODE.resize( HUFFSIZE.size() );

  for (int i = 0; i < HUFFCODE.size(); ++i) {
    HUFFCODE[i] = 0;
  }

  int16_t code = 0;
  int si = HUFFSIZE[0];
  int k = 0;

  while ( k < HUFFSIZE.size() ) {
    HUFFCODE[k] = code;
    //std::cout << "HUFFCODE[" << std::dec << k << "] = 0x" << std::hex << HUFFCODE[k] <<  "\tHUFFSIZE[" << std::dec << k << "] = 0x" << std::hex << HUFFSIZE[k] << endl;
    ++code;
    ++k;
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
                                        const std::vector<int16_t>& HUFFCODE,
                                        std::vector<int16_t>& MINCODE,
                                        std::vector<int16_t>& MAXCODE) {
  MAXCODE.resize(16);
  MINCODE.resize(16);

  for (int i = 0; i < 16; ++i) {
    MAXCODE[i] = 0;
    MINCODE[i] = 0;
  }

  int j = 0;

  for (int i = 0; i < 16; ++i) {
    if (BITS[i] == 0) {
      MAXCODE[i] = (-1);
    } else {
      MINCODE[i] = HUFFCODE[j];
      j = j + BITS[i] - 1;
      MAXCODE[i] = HUFFCODE[j];
      ++j;
    }

//    std::cout << "BITS[" << std::dec << i << "] = " << std::dec << (int)BITS[i];
//    std::cout << "\t MINCODE[" << std::dec << i << "] = 0x" << std::hex << MINCODE[i];
//    std::cout << "\t MAXCODE[" << std::dec << i << "] = 0x" << std::hex << MAXCODE[i];
//    std::cout << endl;
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
    } while (!cnt);
  }

  if (cnt) {
    uint8_t result = (currentByte & 0x80) >> 7; //reading MSB
    --cnt;
    currentByte = currentByte << 1;
    return result;
  }
}

uint8_t JpegDecoder::Decode(const std::vector<int16_t>& MINCODE,
                            const std::vector<int16_t>& MAXCODE,
                            const std::vector<int16_t> HUFFVAL[]) {
  int i = 1;
  int j = 0;
  uint16_t code = ReadNextBit();

  while (i <= 16) {
    if ( code > MAXCODE[i-1] ) {
      ++i;
      code = (code << 1) | ReadNextBit();
    } else {
      //std::cout << "size of HUFFVAL["<< i << "]=" << HUFFVAL[i].size() << std::endl;
      uint16_t offset = code - MINCODE[i-1];
      return HUFFVAL[i-1].at(offset);
    }
  }

  return 0;
}

int16_t JpegDecoder::EXTEND(int V, int T) {
  int16_t Vt = 1 << (T-1);

  if (V < Vt) {
    Vt = ((int16_t)(-1) << T) + 1;
    V+= Vt;
  }

  return V;
}

int JpegDecoder::DCDecode(int Cid) {
  int Td = sections.GetComponentTd(Cid);
  int huffTableId = (sections.HUFFMAN_TABLE_DC << 4) | Td;

  HuffTableContext huffDecodeTableSet = huffDecodeTables[huffTableId];

  //std::cout << "huffTableId = " << huffTableId << " " << "C = " << Cid<< endl;
  uint8_t DCValBitLength = Decode(huffDecodeTableSet.MINCODE, huffDecodeTableSet.MAXCODE,
                                  huffDecodeTableSet.HUFFVAL);

  if (DCValBitLength) {
    return DecodeFromBitLength(DCValBitLength);
  } else {
    return 0;
  }
}

int JpegDecoder::DecodeFromBitLength(int bitLength) {
  int Ampl = 0;

  for (int i = 0; i < bitLength; ++i) {
    Ampl = Ampl << 1;
    Ampl |= ReadNextBit();
  }

  if (Ampl) {
    Ampl = EXTEND(Ampl, bitLength);
  }

  return Ampl;
}

void JpegDecoder::DecodeBlock(int Cid, ZZMatrix<int>& block) {

  block.Reset();
  int iniVal = 0;
  block.Init(iniVal);

  ZZMatrix<int>& Q = sections.GetQTable(Cid);

  int DCAmpl = DCDecode(Cid);

  //Decode DPCM for DC coefficients
  if (DCAmpl) {
    dcDiff.at(Cid-1)+= DCAmpl;
  }

  //Dequantization
  int dequantDc = dcDiff.at(Cid-1) * Q.Get( 0, 0 );

  block.Push( dequantDc );

  int Ta = sections.GetComponentTa(Cid);
  int huffTableId = (sections.HUFFMAN_TABLE_AC << 4) | Ta;

  HuffTableContext huffDecodeTableSet = huffDecodeTables[huffTableId];

  uint8_t code = 0;

  // Number of AC elements == 63
  int k = 1;
  do {
    code = Decode(huffDecodeTableSet.MINCODE, huffDecodeTableSet.MAXCODE,
                  huffDecodeTableSet.HUFFVAL);

    int SSSS = code % 16;
    int RRRR = code >> 4;

    if (SSSS) {
      for (int g = 0; g < RRRR; ++g) {
        block.Push( 0 );
      }
      k+= RRRR;

      int ACAmpl = DecodeFromBitLength(SSSS);
      ACAmpl = ACAmpl * Q.Get( block.GetCurrentY(),
                               block.GetCurrentX() );

      block.Push( ACAmpl );

      ++k;
    } else if (RRRR == 15) {

      for (int g = 0; g < 16; ++g) {
        block.Push( 0 );
      }

      k+= 16;
    } else {
      //EOB?
      break;
    }
  } while ( k <= 63 );
}

int JpegDecoder::DecodeNextBlock(cv::Mat& result) {

  ZZMatrix<int> currentBlock(8,8);
  cv::Mat decodingResult;

  if (!currentCid) {
    currentCid = 1;
    currentCidCounter = 0;
  }

  int totalAmountCiComponents = sections.GetComponentH(currentCid) * sections.GetComponentV(currentCid);

  if (currentCidCounter < totalAmountCiComponents) {
    ++currentCidCounter;
  } else {
    if ( currentCid < sections.GetComponentsNumber() ) {
      ++currentCid;
    } else {
      currentCid = 1;
    }
    currentCidCounter = 1;
  }

  DecodeBlock(currentCid, currentBlock);

  currentBlock.ConvertTo_CV32FC1(decodingResult);

  //std::cout << "C[" << currentCid << "] = " << endl << " " << decodingResult << endl << endl;
  cv::dct(decodingResult, decodingResult, cv::DCT_INVERSE);

  //DCT shift
  decodingResult+= 128;
  decodingResult.convertTo(result, CV_8U);

  return currentCid;

}

void JpegDecoder::MergeMat(ZZMatrix<cv::Mat>& zz, cv::Mat& result) {
  cv::Mat resultRow;

  cv::Mat zr = zz.Get(0,0);
  zr.copyTo(result);
  for (int x = 1; x < zz.Columns(); ++x) {
    hconcat(result, zz.Get(0,x), result);
  }

  for (int y = 1; y < zz.Rows(); ++y) {
    zr = zz.Get(y,0);
    zr.copyTo(resultRow);

    for (int x = 1; x < zz.Columns(); ++x) {
      hconcat(resultRow, zz.Get(y,x), resultRow);
    }

    vconcat(result, resultRow, result);
    resultRow.release();
  }
}

void JpegDecoder::run() {
  cv::Mat resultImage;
  cv::Mat resultRow;
  cv::Mat resultRowRow;
  cv::Mat block;

  int height = sections.GetImageSizeY() / 8;
  int width = sections.GetImageSizeX() / 8;

  if (sections.GetImageSizeY() % 8)
    ++height;

  if (sections.GetImageSizeX() % 8)
    ++width;

  int sizeComponent[3];
  ZZMatrix<cv::Mat>* components[3];

  for (int i = 0; i < sections.GetComponentsNumber(); ++i) {
    int comH = sections.GetComponentH(i+1);
    int comV = sections.GetComponentV(i+1);
    sizeComponent[i] = comH*comV;
    components[i] = new ZZMatrix<cv::Mat>(comV, comH);
  }

  for (int w = 0; w < height; ++w) {
    for (int g = 0; g < width/2; ++g) {
      for (int i = 0; i < sections.GetComponentsNumber(); ++i) {
        for (int j = 0; j < sizeComponent[i]; ++j) {
          DecodeNextBlock(block);
          //std::cout << "C[" << i << "]=" << endl << block << endl << endl;
          components[i]->Push(block);
        }

        components[1]->Reset();
        components[2]->Reset();

        MergeMat(*components[0], resultRow);
      }
      if (resultImage.empty()) {
        resultRow.copyTo(resultImage);
      } else {
        cv::hconcat(resultImage, resultRow, resultImage);
      }

      resultRow.release();
    }
    if (resultRowRow.empty()) {
      resultImage.copyTo(resultRowRow);
    } else {
      cv::vconcat(resultRowRow, resultImage, resultRowRow);
    }

    resultImage.release();
  }


  cv::namedWindow( "lena", CV_WINDOW_AUTOSIZE );
  cv::imshow("lena", resultRowRow);

  cv::waitKey(0);

}
