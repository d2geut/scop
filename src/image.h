#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "common.h"

CLASS_PTR(Image)
class Image {
public:
    static ImageUPtr Load(const std::string& filepath);
    ~Image();

    const uint8_t* GetData() const { return m_data; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannelCount() const { return m_channelCount; }

private:
    Image() {};
    bool LoadBMP(const std::string& filepath);
    int32_t m_width { 0 };
    int32_t m_height { 0 };
    uint16_t m_channelCount {0};
    uint8_t* m_data { nullptr };
};

#endif
