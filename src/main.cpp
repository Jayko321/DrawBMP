#include "BMPImage.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
		std::cerr << "Not enough arguments, please specify BMP file path.\n";
		return -1;
    }
    std::string filePath;
    filePath = argv[1];
    if (argc > 2) {
        std::cout << "Too many arguments, only " << filePath << " will be used to display a BMP file, everything else will be ignored.\n";
    }

    BMPImage image;
    if (auto error = image.openBMP(filePath)) {
        std::cerr << error.value() << "\n";
        return -1;
    }
    else
        image.displayBMP();

    image.closeBMP();
    return 0;
}
