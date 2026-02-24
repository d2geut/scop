#include "image.h"
#include <fstream>
#include <iostream>
#include <memory.h>

ImageUPtr Image::Load(const std::string& filepath) {
    auto image = ImageUPtr(new Image());
    if (!image->LoadBMP(filepath))
        return nullptr;
    return std::move(image);
}

Image::~Image() {
    if (m_data) {
        free(m_data);
        m_data = nullptr;
    }
}

bool Image::LoadBMP(const std::string& filepath) {
    // bmp file name format check
    if (!isBMPFile(filepath)) {
        std::cerr << "failed to open BMP file: " << filepath << " is not BMP file" << std::endl;
        return false;
    }

    // file open & bmp file format check
    auto result = LoadTextFile(filepath);
    if (!result.has_value())
        return false;

    auto& filedata = result.value();
    if (*reinterpret_cast<uint16_t*>(&filedata[0]) != 0x4D42) { // bftype ∞À¡ı
        std::cerr << "falied to open BMP file: " << filepath << " is invaild BMP file" << std::endl;
        return false;
    }
    uint32_t bfOffBits = *reinterpret_cast<uint32_t*>(&filedata[10]);
    int32_t biWidth = *reinterpret_cast<int32_t*>(&filedata[18]);
    int32_t biHeight = *reinterpret_cast<int32_t*>(&filedata[22]);
    uint16_t biBitCount = *reinterpret_cast<uint16_t*>(&filedata[28]);
    uint32_t biCompression = *reinterpret_cast<uint32_t*>(&filedata[30]);

    if ((biBitCount != 24 && biBitCount != 32) || biCompression != 0) {
        std::cout << "biCompression: " << biCompression << std::endl;
        std::cerr << "falied to open BMP file: " << filepath << " is not supported BMP file" << std::endl;
        return false;
    }

    // set image data
    m_width = biWidth;
    m_height = abs(biHeight);
    m_channelCount = biBitCount / 8;
    m_data = (uint8_t*)malloc(m_width * m_height * m_channelCount);
    memset(m_data, 0, sizeof(m_width * m_height * m_channelCount));
    if (!m_data)
        return false;

    // pixel data setting
    int32_t rowSize = (m_width * m_channelCount + 3) & (~3);
    for (int i = 0; i < m_height; i++) {
        memcpy(m_data + i * m_width * m_channelCount, filedata.c_str() + bfOffBits + i * rowSize, m_width * m_channelCount);
    }

    // BGR to RGB
    for (int i = 0; i < m_width * m_height; i++) {
        uint8_t* ptr = m_data + i * m_channelCount;
        std::swap(ptr[0], ptr[2]);
    }

    return true;
}