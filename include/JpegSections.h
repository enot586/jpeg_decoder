#ifndef JPEGSECTIONS_H
#define JPEGSECTIONS_H

#include <memory>
#include <string>
#include <list>

using namespace std;

class JpegSections
{

  public:
    JpegSections();
    virtual ~JpegSections();

  private:
    const uint8_t  prefix[2] = { 0xFF, 0xD8 };

    struct  {
      const uint8_t  prefix[2] = { 0xFF, 0xFE };
      uint8_t  jpegCommentSectionSize[2] = { 0x00, 0x00 };
    } jpegCommentSection;

    struct  {
      const uint8_t  prefix[2] = { 0xFF, 0xDB };
      uint8_t  jpegDqtSize[2] = { 0x00, 0x00 };
      uint8_t  jpegDqtElementSize;
    } jpegDqtSection;

    struct  {
      const uint8_t  prefix[2] = { 0xFF, 0xC0 };
      uint8_t  jpegSOF0Size[2] = { 0x00, 0x00 };
      uint8_t  jpegSOF0Precision;
      uint8_t  jpegSOF0Height[2] = { 0x00, 0x00 };
      uint8_t  jpegSOF0Width[2] = { 0x00, 0x00 };
      uint8_t  jpegSOF0ComponentNum;
    } jpegSOF0Section;

    struct JpegSOF0Component {
      uint8_t jpegSOF0ComponentId;
      uint8_t jpegSOF0ComponentHsubsampling;
      uint8_t jpegSOF0ComponentVsubsampling;
      uint8_t jpegSOF0ComponentQTableId;
    };

    std::list<JpegSOF0Component> jpegSOF0Components;

    struct  {
      const uint8_t  prefix[2] = { 0xFF, 0xC4 };
      uint8_t  jpegDhtClass;
      uint8_t  jpegDhtId;
      uint8_t  jpegDhtCodeQuantity[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
      uint8_t  jpegDhtCode1;
      uint8_t  jpegDhtCode2;
    } jpegDhtSection;

    struct  {
      const uint8_t  prefix[2] = { 0xFF, 0xDA };
      uint8_t  jpegSOSHeaderSize[2] = { 0x00, 0x00 };
      uint8_t  jpegSOSComponentNum;
    } jpegSOSSection;

    struct JpegSOSComponent  {
      uint8_t jpegSOSComponentId;
      uint8_t jpegSOSComponentDhtDcId;
      uint8_t jpegSOSComponentDhtAcId;
    };

    std::list<JpegSOSComponent> jpegSOSComponents;

    const uint8_t jpegMagic1 = 0x00;
    const uint8_t jpegMagic2 = 0x3F;
    const uint8_t jpegMagic3 = 0x00;

    auto_ptr<uint8_t> pImageData;


    const uint8_t  jpegEndOfData[2] = { 0xFF, 0xD9 };


  public:

    void jpegReadSOF(const std::string& fileName);

};

#endif // JPEGSECTIONS_H
