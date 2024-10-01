#include "BMPImage.h"
#include <fstream>
#include <iostream>
#include <string>

//https://en.wikipedia.org/wiki/BMP_file_format

static std::string GetError(const std::ifstream& stream) {
	std::ios_base::iostate state = stream.rdstate();

	if (errno) 
		return strerror(errno);

	if (state & std::ios_base::eofbit) 
		return "Unexpected end of file reached.";
	if (state & std::ios_base::failbit) 
		return "Non-fatal I/O error occurred.";
	if (state & std::ios_base::badbit) 
		return "Fatal I/O error occurred.";

	return "No error";
}

std::optional<std::string> BMPImage::openBMP(const std::string& filePath) noexcept {
	std::ifstream stream{ filePath, std::fstream::binary | std::fstream::in | std::fstream::badbit | std::fstream::eofbit | std::fstream::failbit };
	if (!stream.is_open()) {
		return GetError(stream);
	}

	uint32_t compressionMethod{ 0 };
	uint32_t pixelArrayStart{ 0 };
	uint16_t bitsPerPixel{ 0 };
	std::string start;
	start.resize(2);

	if (!stream.seekg(0))
		return GetError(stream);
	if (!stream.read(start.data(), 2))
		return GetError(stream);
	if (start != "BM") {
		return "Incorrect file format.";
	}

	if (!stream.seekg(10))
		return GetError(stream);
	if (!stream.read((char*)&pixelArrayStart, sizeof pixelArrayStart))
		return GetError(stream);

	if (!stream.seekg(18))
		return GetError(stream);
	if (!stream.read((char*)&m_Width, sizeof m_Width))
		return GetError(stream);
	if (!stream.read((char*)&m_Height, sizeof m_Height))
		return GetError(stream);
	if (!stream.seekg(28))
		return GetError(stream);
	if (!stream.read((char*)&m_BitsPerPixel, sizeof m_BitsPerPixel))
		return GetError(stream);
	if (!stream.read((char*)&compressionMethod, sizeof compressionMethod))
		return GetError(stream);

	if (!m_Width || !m_Height) {
		return "Bitmap is empty";
	}
	if (!m_BitsPerPixel || !pixelArrayStart) {
		return "File is corrupted";
	}
	if (m_BitsPerPixel != 24 && m_BitsPerPixel != 32) {
		return "File format isn't supported";
	}

	switch (compressionMethod) {
	case 0: //no compression
	//case 1: //RLE8
	//case 2: //RLE4
		break;
	default:
		return "Bitmap uses compression method, that isn't supported.";
	}
	
	m_ImageData.resize(m_Height);
	for (auto& row : m_ImageData) {
		row.resize(m_Width);
	}

	stream.seekg(pixelArrayStart);
	for (size_t y = 0; y < m_ImageData.size(); y++) {
		for (size_t x = 0; x < m_ImageData[y].size(); x++) {
			if (!stream.read((char*)&m_ImageData[y][x], m_BitsPerPixel / 8)) {
				return GetError(stream);
			}
		}
		
		//Every row is aligned to 4 bytes, so if we have less than 4 bytes per pixel, we need to skip padding data
		size_t pos = (size_t)stream.tellg() - pixelArrayStart;
		if (pos % 4 != 0) {
			if (!stream.seekg((pos + pixelArrayStart) + pos % 4))
				return GetError(stream);
		}
	}
	m_CanDisplay = true;
	
	return std::nullopt;
}

void BMPImage::closeBMP() {
	m_ImageData.clear();
	m_BitsPerPixel = 0;
	m_Width = 0;
	m_Height = 0;
	m_CanDisplay = false;
}

void BMPImage::displayBMP() {
	if (!m_CanDisplay) return;

	for(int64_t y = m_ImageData.size() - 1; y >= 0; y--) {
		for (size_t x = 0; x < m_ImageData[y].size(); x++) {
			uint32_t color = m_ImageData[y][x];
			if (color == 0xFFFFFFFF || color == 0x00FFFFFF) {
				std::cout << "W";
				continue;
			}
			if (color == 0 || color == 0xFF000000) {
				std::cout << "B"; 
				continue;
			}
			color = m_ImageData[y][x];
		}
		std::cout << "\n";
	}
}
