#ifndef JPEGSECTIONS_H
#define JPEGSECTIONS_H
#include <fstream>
#include <memory>
#include <string>
#include <list>
#include <vector>
#include "ZZMatrix.h"

using namespace std;

class JpegSections
{
  public:
    JpegSections(std::string& fileName);
    virtual ~JpegSections();

    uint8_t  ReadByteFromImage();

    int GetImageSizeX();
    int GetImageSizeY();

    int GetComponentsNumber();
    int GetComponentH(int Cid);
    int GetComponentV(int Cid);
    int GetComponentTq(int Cid);
    int GetComponentTd(int Cid);
    int GetComponentTa(int Cid);
    ZZMatrix<int, 8,8>& GetQTable(int Cid);

    int GetComponentHmax();
    int GetComponentVmax();



  protected:
    void  AssignFile(std::string& fileName);
    bool  SearchSOI();
    int   SearchSOF();
    bool  ReadSOF(int sofNumber);
    void  ReadFrameComponents();
    void  ReadSOS();
    void  ReadDQT();
    void  ReadDHT();
    void  GotoImage();

  public:
    const uint8_t RST0[2] = { 0xFF, 0xD0 };
    const uint8_t RST1[2] = { 0xFF, 0xD1 };
    const uint8_t RST2[2] = { 0xFF, 0xD2 };
    const uint8_t RST3[2] = { 0xFF, 0xD3 };
    const uint8_t RST4[2] = { 0xFF, 0xD4 };
    const uint8_t RST5[2] = { 0xFF, 0xD5 };
    const uint8_t RST6[2] = { 0xFF, 0xD6 };
    const uint8_t RST7[2] = { 0xFF, 0xD7 };

    const uint8_t  SOI[2] = { 0xFF, 0xD8 };

    struct  {
      const uint8_t  marker[2] = { 0xFF, 0xFE };
      uint8_t  jpegCommentSectionSize[2] = { 0x00, 0x00 };
    } jpegCommentSection;

    struct  {
      const uint8_t  marker[2] = { 0xFF, 0xDB };
      uint8_t  jpegDqtSize[2] = { 0x00, 0x00 };
      uint8_t  jpegDqtElementSize;
    } jpegDqtSection;

    struct JpegFrameComponent {
      uint8_t C;
      uint8_t H;
      uint8_t V;
      uint8_t Tq;
    };

    struct  JpegFrameHeader {
      const uint8_t  marker[2] = { 0xFF, 0xC0 };
      uint16_t  Lf;
      uint8_t  precision;
      uint16_t  Y;
      uint16_t  X;
      uint8_t  Nf;
      std::vector<JpegFrameComponent> components;
    };

    std::vector<JpegFrameHeader> SOF; //@TODO: уменьшить рамер, заменить массивом

    struct JpegSOSComponent  {
      uint8_t C;
      uint8_t Td;
      uint8_t Ta;
    };

    struct  {
      const uint8_t  marker[2] = { 0xFF, 0xDA };
      uint16_t  Ls;
      uint8_t  Ns;
      std::vector<JpegSOSComponent> scanComponents;
      uint8_t Ss;
      uint8_t Se;
      uint8_t Ah;
      uint8_t Al;
    } SOS;

    struct {
      const uint8_t  marker[2] = { 0xFF, 0xDA };
      uint16_t Ld;
      uint16_t NL;
    } DNL;

    struct DQTTableElement {
      uint8_t  Pq;
      uint8_t  Tq;
      //uint16_t  Q[64];
      ZZMatrix<int, 8,8> Q;
    };

    struct  {
      const uint8_t  marker[2] = { 0xFF, 0xDB };
      uint16_t  Lq;
      std::list<DQTTableElement> tables;
    } DQT;


    const uint8_t HUFFMAN_TABLE_DC = 0x00;
    const uint8_t HUFFMAN_TABLE_AC = 0x01;

    struct DHTTableElement {
      uint8_t  Tc;
      uint8_t  Th;
      uint8_t  L[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
      std::vector<uint8_t> V[16];
    };

    struct  {
      const uint8_t  marker[2] = { 0xFF, 0xC4 };
      uint16_t  Lh;
      std::list<DHTTableElement> tables;
    } DHT;

    const uint8_t  EOI[2] = { 0xFF, 0xD9 };

  protected:
    JpegFrameHeader& GetCurrentSof() {
      return SOF[jpegSOF];
    };

  private:
    std::ifstream  afile;
    int jpegSOF = 0;
    uint32_t offsetToData = 0;
};

#endif // JPEGSECTIONS_H
