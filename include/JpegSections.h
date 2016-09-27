#ifndef JPEGSECTIONS_H
#define JPEGSECTIONS_H
#include <fstream>
#include <memory>
#include <string>
#include <list>
#include <vector>

using namespace std;

class JpegSections
{

  public:
    JpegSections();
    virtual ~JpegSections();

  private:
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

    struct  JpegFrameHeader {
      const uint8_t  marker[2] = { 0xFF, 0xC0 };
      uint8_t  Lf[2] = { 0x00, 0x00 };
      uint8_t  precision;
      uint8_t  Y[2] = { 0x00, 0x00 };
      uint8_t  X[2] = { 0x00, 0x00 };
      uint8_t  Nf;
    };

    std::vector<JpegFrameHeader> SOF; //@TODO: уменьшить рамер, заменить массивом

    struct JpegFrameComponent {
      uint8_t C;
      uint8_t H;
      uint8_t V;
      uint8_t Tq;
    };

    std::vector<JpegFrameComponent> jpegFrameComponents;

    struct JpegSOSComponent  {
      uint8_t C;
      uint8_t Td;
      uint8_t Ta;
    };

    struct  {
      const uint8_t  marker[2] = { 0xFF, 0xDA };
      uint8_t  Ls[2] = { 0x00, 0x00 };
      uint8_t  Ns;
      std::vector<JpegSOSComponent> scanComponents;
      uint8_t Ss;
      uint8_t Se;
      uint8_t Ah;
      uint8_t Al;
    } SOS;

    struct DQTTableElement {
      uint8_t  Pq;
      uint8_t  Tq;
      uint16_t  Q[64];
    };

    struct  {
      const uint8_t  marker[2] = { 0xFF, 0xDB };
      uint8_t  Lq[2] = { 0x00, 0x00 };
      std::list<DQTTableElement> tables;
    } DQT;


    struct DHTTableElement {
      uint8_t  Tc;
      uint8_t  Th;
      uint8_t  L[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
      std::list<uint8_t> V[16];
    };

    struct  {
      const uint8_t  marker[2] = { 0xFF, 0xC4 };
      uint8_t  Lh[2] = { 0x00, 0x00 };
      std::list<DHTTableElement> tables;
    } DHT;

    auto_ptr<uint8_t> pImageData;

    const uint8_t  EOI[2] = { 0xFF, 0xD9 };

    std::ifstream  afile;
    int jpegSOF = 0;

  public:
    void  AssignFile(std::string& fileName);
    bool  SearchSOI(std::ifstream&  afile);
    int   SearchSOF();
    bool  ReadSOF(int sofNumber, std::ifstream& afile);
    void  ReadFrameComponents(std::ifstream&  afile);
    void  ReadSOS(std::ifstream&  afile);
    void  ReadDQT(std::ifstream&  afile);
    void  ReadDHT(std::ifstream&  afile);

};

#endif // JPEGSECTIONS_H
