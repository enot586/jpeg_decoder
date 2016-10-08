#include "JpegDecoder.h"
#include <iostream>
#include <algorithm>



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
//      std::cout << "HUFFVAL[" << i << "] size = " << buffer.HUFFVAL[i].size() << std::endl;
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
      HUFFSIZE[k] = i;
//      std::cout << "HUFFSIZE[" << std::dec << k << "] = 0x" << std::hex << HUFFSIZE[k] << endl;
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
                                   std::vector<uint16_t>& HUFFCODE) {

  HUFFCODE.resize( HUFFSIZE.size() );

  for (int i = 0; i < HUFFCODE.size(); ++i) {
    HUFFCODE[i] = 0;
  }

  int16_t code = 0;
  int si = HUFFSIZE[0];

  for (int k = 0; k < HUFFSIZE.size()-1; ++k) {
    HUFFCODE[k] = code;
 //   std::cout << "HUFFCODE[" << std::dec << k << "] = 0x" << std::hex << HUFFCODE[k] << endl;
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
                                        const std::vector<uint16_t>& HUFFCODE,
                                        std::vector<uint16_t>& MINCODE,
                                        std::vector<uint16_t>& MAXCODE) {
  MAXCODE.resize(16);
  MINCODE.resize(16);

  for (int i = 0; i < 16; ++i) {
    MAXCODE[i] = 0;
    MINCODE[i] = 0;
  }

  int j = 0;

  for (int i = 0; i < 16; ++i) {
    if (BITS[i] == 0) {
      MAXCODE[i] = (int16_t)(-1);
    } else {
      MINCODE[i] = HUFFCODE[j];
      j = j + BITS[i] - 1;
      MAXCODE[i] = HUFFCODE[j];
//      std::cout << "BITS[" << std::dec << i << "] = " << std::dec << (int)BITS[i];
//      std::cout << "\t MINCODE[" << std::dec << i << "] = 0x" << std::hex << MINCODE[i];
//      std::cout << "\t MAXCODE[" << std::dec << i << "] = 0x" << std::hex << MAXCODE[i];
//      std::cout << endl;
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
    } while (!cnt);
  }

  if (cnt) {
    uint8_t result = (currentByte & 0x80) >> 7; //reading MSB
    --cnt;
    currentByte = currentByte << 1;
    return result;
  }
}

uint8_t JpegDecoder::Decode(const std::vector<uint16_t>& MINCODE,
                            const std::vector<uint16_t>& MAXCODE,
                            const std::vector<uint16_t> HUFFVAL[]) {
  int i = 1;
  uint16_t code = ReadNextBit();

  while (i < 16) {
    if (code > MAXCODE[i]) {
      ++i;
      code = (code << 1) | ReadNextBit();
    } else {
      //std::cout << "size of HUFFVAL["<< i << "]=" << HUFFVAL[i].size() << std::endl;
      uint16_t offset = code - MINCODE[i];
      return HUFFVAL[i].at(offset);
    }
  }

  return 0;
}

int16_t JpegDecoder::EXTEND(int V, int T) {
  int16_t Vt = 2 << (T-1);

  if (V < Vt) {
    Vt = ((uint16_t)(-1) << T) + 1;
    V+= Vt;
  }

  return V;
}

int JpegDecoder::DCDecode(int Cid) {
  int Td = sections.GetComponentTd(Cid);
  int huffTableId = (sections.HUFFMAN_TABLE_DC << 4) | Td;

  HuffTableContext huffDecodeTableSet = huffDecodeTables[huffTableId];

  uint8_t DCValBitLength = Decode(huffDecodeTableSet.MINCODE, huffDecodeTableSet.MAXCODE,
                                  huffDecodeTableSet.HUFFVAL);

  return DecodeFromBitLength(DCValBitLength);
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

void JpegDecoder::DecodeBlock(int Cid, ZZMatrix<int, 8, 8>& block) {

  ZZMatrix<int, 8, 8>& Q = sections.GetQTable(Cid);

  int DCAmpl = DCDecode(Cid);

  //Decode DPCM for DC coefficients
  dcDiff.at(Cid-1)+= DCAmpl;

  //Dequantization
  dcDiff.at(Cid-1) = dcDiff.at(Cid-1) * Q.Get( block.GetCurrentY(),
                                               block.GetCurrentX() );

  block.Push( dcDiff.at(Cid-1) );

  int Ta = sections.GetComponentTa(Cid);
  int huffTableId = (sections.HUFFMAN_TABLE_AC << 4) | Ta;

  HuffTableContext huffDecodeTableSet = huffDecodeTables[huffTableId];

  uint8_t code = 0;

  // Number of AC elements == 63
  for (int k = 0; k < 62; ++k) {
    code = Decode(huffDecodeTableSet.MINCODE, huffDecodeTableSet.MAXCODE,
                  huffDecodeTableSet.HUFFVAL);

    int SSSS = code % 16;
    int RRRR = code >> 4;

    if (SSSS) {
      k+= RRRR;

      int ACAmpl = DecodeFromBitLength(SSSS);
      ACAmpl = ACAmpl * Q.Get( block.GetCurrentY(),
                               block.GetCurrentX() );

      block.Push( ACAmpl );
    } else if (RRRR == 15) {
      k+= 16;
    } else {
      break;
    }
  }
}

void JpegDecoder::DecodeNextBlock(ZZMatrix<int, 8, 8>& block) {

  ZZMatrix<int, 8, 8> currentBlock;

  for (int i = 1; i <= sections.GetComponentsNumber(); ++i) {
    int totalAmountCiComponents = sections.GetComponentH(i) * sections.GetComponentV(i);

    currentBlock.Reset();
    currentBlock.Init(0);

    for (int j = 1; j <= totalAmountCiComponents; ++j) {
      DecodeBlock(i, currentBlock);
      currentBlock.Print();
      std::cout << endl;
    }
  }

}

void JpegDecoder::run() {
  ZZMatrix<int, 8, 8> block;
  DecodeNextBlock(block);

}
