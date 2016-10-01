#include "JpegSections.h"
#include <stdexcept>
#include <exception>
#include <exception>
#include <iostream>

using namespace std;

JpegSections::JpegSections(std::string& fileName)
{
  SOF.resize(9);
  AssignFile(fileName);
}

JpegSections::~JpegSections()
{
  //dctor
}

void JpegSections::AssignFile(std::string& fileName) {
  afile.open(fileName, ios::binary);

  if ( !SearchSOI() ) {
    throw std::invalid_argument("Input Can't find JPEG SOI");
  }

  jpegSOF = SearchSOF();

  if (jpegSOF == -1) {
    throw std::invalid_argument("Can't find JPEG SOF");
  }

  if ( !ReadSOF(jpegSOF) ) {
    throw std::invalid_argument("Invalid JPEG SOF0");
  }

  ReadDQT();
  ReadDHT();
  ReadSOS();
}

bool JpegSections::SearchSOI() {
  if ( !afile.good() )
    throw std::invalid_argument("Input file error !");

  uint8_t markerBuffer = 0;

  while ( !afile.eof() ) {
    afile >> markerBuffer;

    if (markerBuffer == SOI[0]) {
      afile >> markerBuffer;
      if (markerBuffer == SOI[1]) {
        return true;
      }
    }
  }

  return false;
}

int JpegSections::SearchSOF() {
  if ( !afile.good() )
    throw std::invalid_argument("Input file error !");

  uint32_t buffer = 0;
  uint8_t markerBuffer = 0;

  while ( !afile.eof() ) {
    afile >> markerBuffer;

    if (markerBuffer == SOF[0].marker[0]) {
      afile >> markerBuffer;
      int sofNumber = markerBuffer - SOF[0].marker[1];
      if ( sofNumber < SOF.size() ) {
        return sofNumber;
      }
    }
  }

  return (-1);
}

bool JpegSections::ReadSOF(int sofNumber) {

  if ( !afile.good() )
    throw std::invalid_argument("Input file error !");

  if ( sofNumber > SOF.size() ) {
    throw std::invalid_argument("Invalid SOF number!");
  }

  uint8_t markerBuffer = 0;

  afile >> markerBuffer;
  SOF[sofNumber].Lf[0] = markerBuffer;

  afile >> markerBuffer;
  SOF[sofNumber].Lf[1] = markerBuffer;

  afile >> markerBuffer;
  SOF[sofNumber].precision  = markerBuffer;

  afile >> markerBuffer;
  SOF[sofNumber].Y[0] = markerBuffer;
  afile >> markerBuffer;
  SOF[sofNumber].Y[1] = markerBuffer;

  afile >> markerBuffer;
  SOF[sofNumber].X[0] = markerBuffer;

  afile >> markerBuffer;
  SOF[sofNumber].X[1] = markerBuffer;

  afile >> markerBuffer;
  SOF[sofNumber].Nf  = markerBuffer;

  ReadFrameComponents();

  return true;
}

void JpegSections::ReadFrameComponents() {
  if ( !afile.good() )
    throw std::invalid_argument("Input file error !");

  if (SOF[jpegSOF].Nf > 3) {
    throw std::invalid_argument("Too many image components in frame header !");
  }

  SOF[jpegSOF].components.resize(SOF[jpegSOF].Nf);

  uint8_t byteBuffer = 0;

  for (int i = 0; i < SOF[jpegSOF].Nf; ++i) {
    afile >> byteBuffer;
    SOF[jpegSOF].components[i].C = byteBuffer;

    afile >> byteBuffer;
    SOF[jpegSOF].components[i].H = (byteBuffer & 0x0F);
    SOF[jpegSOF].components[i].V = (byteBuffer >> 4) & 0x0F;

    afile >> byteBuffer;
    SOF[jpegSOF].components[i].Tq = byteBuffer;

    if (afile.eof()) {
      break;
    }
  }
}

void JpegSections::ReadSOS() {
  if ( !afile.good() )
    throw std::invalid_argument("Input file error !");

  uint8_t markerBuffer = 0;

  afile.seekg(0);

  while ( !afile.eof() ) {

    afile >> markerBuffer;

    if ( markerBuffer == SOS.marker[0] ) {
      afile >> markerBuffer;
      if ( markerBuffer == SOS.marker[1] ) {

        afile >> markerBuffer;
        SOS.Ls[0] = markerBuffer;

        afile >> markerBuffer;
        SOS.Ls[1] = markerBuffer;

        afile >> markerBuffer;
        SOS.Ns = markerBuffer;

        SOS.scanComponents.resize(SOS.Ns);

        for (int i = 0; i < SOS.Ns; ++i) {
          afile >> markerBuffer;
          SOS.scanComponents[i].C  = markerBuffer;

          afile >> markerBuffer;
          SOS.scanComponents[i].Td  = markerBuffer & 0x0F;
          SOS.scanComponents[i].Ta  = (markerBuffer >> 4) & 0x0F;
        }

        afile >> markerBuffer;
        SOS.Ss = markerBuffer;

        afile >> markerBuffer;
        SOS.Se = markerBuffer;

        afile >> markerBuffer;
        SOS.Ah = markerBuffer & 0x0F;;
        SOS.Al = (markerBuffer >> 4) & 0x0F;

        offsetToData = afile.tellg();
        return;
      }
    }
  }
}

void JpegSections::ReadDQT() {
  if ( !afile.good() )
    throw std::invalid_argument("Input file error !");

  uint8_t markerBuffer = 0;

  afile.seekg(0);

  uint16_t currentLength = 0;

  while ( !afile.eof() ) {
    afile >> markerBuffer;

    if ( markerBuffer == DQT.marker[0] ) {
      afile >> markerBuffer;
      if ( markerBuffer == DQT.marker[1] ) {

        afile >> markerBuffer;
        DQT.Lq[0] = markerBuffer;

        afile >> markerBuffer;
        DQT.Lq[1] = markerBuffer;

        currentLength = 2;

        do {

          DQTTableElement table;

          afile >> markerBuffer;
          table.Pq  = markerBuffer & 0x0F;
          table.Tq  = (markerBuffer >> 4) & 0x0F;
          ++currentLength;

          int Qbuffer = 0;
          for (int i = 0; i < 64; ++i) {
            uint8_t byteBuffer = 0;
            afile >> byteBuffer;
            //table.Q[i] = markerBuffer;
            Qbuffer = byteBuffer;
            ++currentLength;

            if (table.Pq) {
              Qbuffer = Qbuffer << 8;
              afile >> byteBuffer;
              //table.Q[i]|= markerBuffer << 8;
              Qbuffer |= byteBuffer;
              ++currentLength;
            }

            table.Q.Push(Qbuffer);
          }

          DQT.tables.push_back(table);
        } while ( (currentLength < ((DQT.Lq[0] << 8) | DQT.Lq[1])) && !afile.eof() );

        return;
      }
    }
  }
}

void JpegSections::ReadDHT() {
  if ( !afile.good() )
    throw std::invalid_argument("Input file error !");

  uint8_t markerBuffer = 0;

  afile.seekg(0);

  uint16_t currentLength = 0;

  while ( !afile.eof() ) {
    afile >> markerBuffer;

    if ( markerBuffer == DHT.marker[0] ) {
      afile >> markerBuffer;
      if ( markerBuffer == DHT.marker[1] ) {

        afile >> markerBuffer;
        DHT.Lh[0] = markerBuffer;

        afile >> markerBuffer;
        DHT.Lh[1] = markerBuffer;

        currentLength = 2;

        do {
          DHTTableElement table;

          afile >> markerBuffer;
          table.Tc  = markerBuffer & 0x0F;
          table.Th  = (markerBuffer >> 4) & 0x0F;

          for (int i = 0; i < 16; ++i) {
            afile >> markerBuffer;
            table.L[i] = markerBuffer;
          }

          currentLength+= 17;

          for (int j = 0; j < 16; ++j) {
            currentLength+= table.L[j];
            for (int i = 0 ; i < table.L[j]; ++i) {
              afile >> markerBuffer;
              table.V[j].push_back(markerBuffer);
            }
          }

          DHT.tables.push_back(table);
        } while ( (currentLength < ((DHT.Lh[0] << 8) | DHT.Lh[1])) && !afile.eof() );

        return;
      }
    }
  }
}

void JpegSections::GotoImage() {
  /*if (SOF[jpegSOF].X || SOF[jpegSOF].Y)*/ {
    afile.seekg(offsetToData);
  }
}

uint8_t  JpegSections::ReadByteFromImage() {
  uint8_t buffer;
  afile >> buffer;
  return buffer;
}
