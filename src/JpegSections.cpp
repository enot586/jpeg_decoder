#include "JpegSections.h"
#include <stdexcept>
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
    throw std::invalid_argument("Error: Input Can't find JPEG SOI");
  }

  jpegSOF = SearchSOF();

  if (jpegSOF == -1) {
    throw std::invalid_argument("Error: Can't find JPEG SOF");
  }

  if ( !ReadSOF(jpegSOF) ) {
    throw std::invalid_argument("Error: Invalid JPEG SOF0");
  }

  ReadDQT();
  ReadDHT();
  ReadSOS();
}

bool JpegSections::SearchSOI() {
  if ( !afile.good() )
    throw std::invalid_argument("Error: Input file error !");

  char  markerBuffer = 0;

  afile.seekg(0);

  while ( !afile.eof() ) {

    afile.read(&markerBuffer, 1);

    if ( (unsigned char)markerBuffer == SOI[0] ) {

      afile.read(&markerBuffer, 1);

      if ( (unsigned char)markerBuffer == SOI[1] ) {
        return true;
      }
    }
  }

  return false;
}

int JpegSections::SearchSOF() {
  if ( !afile.good() )
    throw std::invalid_argument("Error: Input file error !");

  char markerBuffer = 0;
  afile.seekg(0);

  while ( !afile.eof() ) {

    afile.read(&markerBuffer, 1);

    if ( (unsigned char)markerBuffer == SOF[0].marker[0] ) {

      afile.read(&markerBuffer, 1);

      if ( (((unsigned char)markerBuffer) >= SOF[0].marker[1]) &&
           (((unsigned char)markerBuffer) <= SOF[0].marker[1]+3) ) {

        int sofNumber = (unsigned char)markerBuffer - SOF[0].marker[1];

        if ( sofNumber < SOF.size() ) {
          return sofNumber;
        }
      }
    }
  }

  return (-1);
}

bool JpegSections::ReadSOF(int sofNumber) {

  if ( !afile.good() )
    throw std::invalid_argument("Error: Input file error !");

  if ( sofNumber > SOF.size() ) {
    throw std::invalid_argument("Error: Invalid SOF number!");
  }

  char markerBuffer = 0;

  afile.read(&markerBuffer, 1);
  SOF[sofNumber].Lf = ((unsigned char)markerBuffer) << 8;

  afile.read(&markerBuffer, 1);
  SOF[sofNumber].Lf |= markerBuffer;

  afile.read(&markerBuffer, 1);
  SOF[sofNumber].precision  = markerBuffer;

  afile.read(&markerBuffer, 1);
  SOF[sofNumber].Y = ((unsigned char)markerBuffer) << 8;
  afile.read(&markerBuffer, 1);
  SOF[sofNumber].Y |= (unsigned char)markerBuffer;

  afile.read(&markerBuffer, 1);
  SOF[sofNumber].X = ((unsigned char)markerBuffer) << 8;

  afile.read(&markerBuffer, 1);
  SOF[sofNumber].X |= (unsigned char)markerBuffer;

  afile.read(&markerBuffer, 1);
  SOF[sofNumber].Nf  = markerBuffer;

  ReadFrameComponents();

  return true;
}

void JpegSections::ReadFrameComponents() {
  if ( !afile.good() )
    throw std::invalid_argument("Error: Input file error !");

  if (SOF[jpegSOF].Nf > 3) {
    throw std::invalid_argument("Error: Too many image components in frame header !");
  }

  SOF[jpegSOF].components.resize( SOF[jpegSOF].Nf );

  char  byteBuffer = 0;

  for (int i = 0; i < SOF[jpegSOF].Nf; ++i) {
    afile.read(&byteBuffer, 1);
    SOF[jpegSOF].components[i].C = byteBuffer;

    afile.read(&byteBuffer, 1);
    SOF[jpegSOF].components[i].V = ((unsigned char)byteBuffer) & 0x0F;
    SOF[jpegSOF].components[i].H = (((unsigned char)byteBuffer) >> 4) & 0x0F;

    afile.read(&byteBuffer, 1);
    SOF[jpegSOF].components[i].Tq = byteBuffer;

    if (afile.eof()) {
      throw std::length_error("Error: Unsupported header foramt");
    }
  }
}

void JpegSections::ReadSOS() {
  if ( !afile.good() )
    throw std::invalid_argument("Error: Input file error !");

  char  markerBuffer = 0;

  afile.seekg(0);

  while ( !afile.eof() ) {

    afile.read(&markerBuffer, 1);

    if ( (unsigned char)markerBuffer == SOS.marker[0] ) {

      afile.read(&markerBuffer, 1);

      if ( (unsigned char)markerBuffer == SOS.marker[1] ) {
        afile.read(&markerBuffer, 1);
        SOS.Ls = ((unsigned char)markerBuffer) << 8;

        afile.read(&markerBuffer, 1);
        SOS.Ls|= (unsigned char)markerBuffer;

        afile.read(&markerBuffer, 1);
        SOS.Ns = markerBuffer;

        SOS.scanComponents.resize(SOS.Ns);

        for (int i = 0; i < SOS.Ns; ++i) {
          afile.read(&markerBuffer, 1);
          SOS.scanComponents[i].C  = markerBuffer;

          afile.read(&markerBuffer, 1);
          SOS.scanComponents[i].Ta  = ((unsigned char)markerBuffer) & 0x0F;
          SOS.scanComponents[i].Td  = ( ((unsigned char)markerBuffer) >> 4 ) & 0x0F;
        }

        afile.read(&markerBuffer, 1);
        SOS.Ss = markerBuffer;

        afile.read(&markerBuffer, 1);
        SOS.Se = markerBuffer;

        afile.read(&markerBuffer, 1);
        SOS.Al = ((unsigned char)markerBuffer) & 0x0F;
        SOS.Ah = (((unsigned char)markerBuffer) >> 4) & 0x0F;

        offsetToData = afile.tellg();
        return;
      }
    }
  }

  throw std::length_error("Error: Unsupported Start of Scan format");
}

void JpegSections::ReadDQT() {
  if ( !afile.good() )
    throw std::invalid_argument("Error: Input file error !");

  char  markerBuffer = 0;

  afile.seekg(0);

  uint16_t currentLength = 0;

  while ( !afile.eof() ) {

    afile.read(&markerBuffer, 1);

    if ( (unsigned char)markerBuffer == DQT.marker[0] ) {

      afile.read(&markerBuffer, 1);

      if ( (unsigned char)markerBuffer == DQT.marker[1] ) {

        afile.read(&markerBuffer, 1);
        DQT.Lq = ( ((unsigned char)markerBuffer) << 8 );

        afile.read(&markerBuffer, 1);
        DQT.Lq |= (unsigned char)markerBuffer;

        currentLength = 2;

        do {
          DQTTableElement table;

          afile.read(&markerBuffer, 1);
          table.Tq = ((unsigned char)markerBuffer) & 0x0F;
          table.Pq = ((unsigned char)markerBuffer) >> 4 ;
          ++currentLength;

          table.Q.Reset();

          int Qbuffer = 0;
          for (int i = 0; i < 64; ++i) {
            afile.read(&markerBuffer, 1);

            Qbuffer = markerBuffer;
            ++currentLength;

            if (table.Pq) {
              Qbuffer = ((unsigned char)Qbuffer) << 8;
              afile.read(&markerBuffer, 1);
              Qbuffer |= (unsigned char)markerBuffer;
              ++currentLength;
            }

            table.Q.Push(Qbuffer);
          }

          DQT.tables.push_back(table);

          if ( afile.eof() ) {
            throw std::length_error("Error: Unsupported DQT format");
          }
        } while ( currentLength < DQT.Lq );

        return;
      }
    }
  }

  throw std::length_error("Error: Unsupported DQT format");
}

void JpegSections::ReadDHT() {
  if ( !afile.good() )
    throw std::invalid_argument("Error: Input file error !");

  char  markerBuffer = 0;

  afile.seekg(0);

  uint16_t currentLength = 0;

  while ( !afile.eof() ) {

    afile.read(&markerBuffer, 1);

    if ( (unsigned char)markerBuffer == (DHT.marker[0]) ) {

      afile.read(&markerBuffer, 1);

      if ( (unsigned char)markerBuffer == (DHT.marker[1]) ) {

        afile.read(&markerBuffer, 1);
        DHT.Lh = ( ((unsigned char)markerBuffer) << 8 );

        afile.read(&markerBuffer, 1);
        DHT.Lh |= (unsigned char)markerBuffer;

        currentLength = 2;

        do {
          DHTTableElement table;

          afile.read(&markerBuffer, 1);
          table.Th  = ((unsigned char)markerBuffer) & 0x0F;
          table.Tc  = (unsigned char)markerBuffer >> 4;

          for (int i = 0; i < 16; ++i) {
            afile.read(&markerBuffer, 1);
            table.L[i] = markerBuffer;
          }

          currentLength+= 17;

          for (int j = 0; j < 16; ++j) {
            currentLength+= table.L[j];
            for (int i = 0 ; i < table.L[j]; ++i) {
              afile.read(&markerBuffer, 1);
              table.V[j].push_back(markerBuffer);
            }
          }

          DHT.tables.push_back(table);

          if  ( afile.eof() ) {
            throw std::length_error("Error: Unsupported DHT format");
          }

        } while ( currentLength < DHT.Lh );

        return;
      }
    }
  }

  throw std::length_error("Error: Unsupported DHT format");
}

void JpegSections::GotoImage() {
  /*if (SOF[jpegSOF].X || SOF[jpegSOF].Y)*/ {
    afile.seekg(offsetToData);
  }
}

uint8_t  JpegSections::ReadByteFromImage() {
  char  buffer;
  afile.read(&buffer, 1);
  return (unsigned char)buffer;
}

int JpegSections::GetComponentsNumber() {
  return  GetCurrentSof().Nf;
}

int JpegSections::GetComponentH(int Cid) {
  if ((Cid == 0) || (Cid > GetCurrentSof().Nf))
      throw std::invalid_argument("Error: Invalid coponent id");

  if (GetCurrentSof().components[Cid-1].C == Cid) {
    return GetCurrentSof().components[Cid-1].H;
  }

  throw std::invalid_argument("Error: Invalid coponent id");
}

int JpegSections::GetComponentV(int Cid) {
  if ((Cid == 0) || (Cid > GetCurrentSof().Nf))
    throw std::invalid_argument("Error: Invalid coponent id");

  if (GetCurrentSof().components[Cid-1].C == Cid) {
    return GetCurrentSof().components[Cid-1].V;
  }

  throw std::invalid_argument("Error: Invalid coponent id");
}

int JpegSections::GetComponentTq(int Cid) {
  if ((Cid == 0) || (Cid > GetCurrentSof().Nf))
    throw std::invalid_argument("Error: Invalid coponent id");

  if (GetCurrentSof().components[Cid-1].C == Cid) {
    return GetCurrentSof().components[Cid-1].Tq;
  }

  throw std::invalid_argument("Error: Invalid coponent id");
}

int JpegSections::GetComponentTd(int Cid) {
  if ((Cid == 0) || (Cid > GetCurrentSof().Nf))
    throw std::invalid_argument("Error: Invalid coponent id");

  if (SOS.scanComponents[Cid-1].C == Cid) {
    return SOS.scanComponents[Cid-1].Td;
  }

  throw std::invalid_argument("Error: Invalid coponent id");
}

int JpegSections::GetComponentTa(int Cid) {
  if ((Cid == 0) || (Cid > GetCurrentSof().Nf))
    throw std::invalid_argument("Error: Invalid coponent id");

  if (SOS.scanComponents[Cid-1].C == Cid) {
    return SOS.scanComponents[Cid-1].Ta;
  }

  throw std::invalid_argument("Error: Invalid coponent id");
}

ZZMatrix<int> & JpegSections::GetQTable(int Cid) {
  int Tq = GetComponentTq(Cid);

  for (std::list<DQTTableElement>::iterator it= DQT.tables.begin();
        it != DQT.tables.end(); ++it) {
    if (it->Tq == Tq) {
      return it->Q;
    }
  }

  throw std::invalid_argument("QTable not found");
}

int JpegSections::GetComponentHmax() {
  int Hmax = 0;
  for (int i = 0; i < GetCurrentSof().components.size(); ++i) {
    int current = GetCurrentSof().components[i].H;
    if ( current > Hmax) Hmax = current;
  }

  return Hmax;
}

int JpegSections::GetComponentVmax() {
  int Vmax = 0;
  for (int i = 0; i < GetCurrentSof().components.size(); ++i) {
    int current = GetCurrentSof().components[i].V;
    if ( current > Vmax) Vmax = current;
  }

  return Vmax;
}

int JpegSections::GetImageSizeX() {
  return GetCurrentSof().X;
};

int JpegSections::GetImageSizeY() {
  return GetCurrentSof().Y;
}

