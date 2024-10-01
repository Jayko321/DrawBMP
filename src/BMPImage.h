#pragma once
#include <string>
#include <vector>
#include <optional>

class BMPImage {
public:
	[[nodiscard]]
	std::optional<std::string> openBMP(const std::string& filePath) noexcept;
	void closeBMP();

	void displayBMP();
private:
	std::vector<std::vector<uint32_t>> m_ImageData{};
	uint32_t m_Width{ 0 }, m_Height{ 0 };
	uint16_t m_BitsPerPixel{ 0 };
	bool m_CanDisplay{ false };
};

