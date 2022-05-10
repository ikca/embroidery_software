// A Ikic 2020

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdint>

// user #defines for image color processing parameters
#define POSTERIZE_LEVEL 1
#define POSTERIZED_PALETTE_SIZE 60 // assuming <= 60 colors after posterizing
#define DOMINANT_COLORS_NUMBER 3
#define COLORS_TO_EXPORT_NUMBER 2

// user #defines for G-code export parameters
#define G_CODE_HEIGHT_MM  90
#define G_CODE_WIDTH_MM  90
#define G_CODE_Z_MM  15.708
#define G_CODE_STITCH_NUMBER 2
#define G_CODE_DECIMAL_POINTS 3

#define X_INVERSION_TOGGLE  0
#define Y_INVERSION_TOGGLE  0
#define Z_INVERSION_TOGGLE  0

#define CROSS_PATTERN_TOGGLE 1

#define MM_BETWEEN_STITCH_ROWS 0.2
#define STRAIGHT_STITCH_LENGTH_MM 0 // Set to 0 for long stitches

// #defines used in G-code processing (do not modify)
#if X_INVERSION_TOGGLE
#define X_INVERSION_FACTOR 1
#else
#define X_INVERSION_FACTOR -1
#endif

#if Y_INVERSION_TOGGLE
#define Y_INVERSION_FACTOR 1
#else
#define Y_INVERSION_FACTOR -1
#endif

#if Z_INVERSION_TOGGLE
#define Z_INVERSION_FACTOR 1
#else
#define Z_INVERSION_FACTOR -1
#endif

int32_t imageHeightInPixels = 0;
int32_t imageWidthInPixels = 0;

#define PIXEL_HEIGHT_MM ((double)G_CODE_HEIGHT_MM / (double)imageHeightInPixels)
#define HALF_IMAGE_HEIGHT_IN_PIXELS (imageHeightInPixels / 2)
#define HALF_IMAGE_WIDTH_IN_PIXELS (imageWidthInPixels / 2)

#include "class_header.h"
#include "function_header.h"

int main()
{
  std::fstream imageFile;
  imageFile.open ("ladyLiberty.bmp", std::ios::in | std::ios::out | std::ios::binary);

  if (!imageFile)
  {
    std::cout << "Error loading image.";
    return EXIT_FAILURE;
  }

  char * fillerArray1 = new char [10];
  imageFile.read(fillerArray1, 10);

  char * pixelDataOffset = new char [4];
  imageFile.read(pixelDataOffset, 4);
  int32_t dataOffset = fourByteValue(pixelDataOffset);

  char * headerSize = new char [4];
  imageFile.read(headerSize, 4);

  char * imageWidth = new char [4];
  imageFile.read(imageWidth, 4);
  imageWidthInPixels = fourByteValue(imageWidth);

  char * imageHeight = new char [4];
  imageFile.read(imageHeight, 4);
  imageHeightInPixels = fourByteValue(imageHeight);

  char * Planes = new char [2];
  imageFile.read(Planes, 2);

  char * bitsPerPixel = new char [2];
  imageFile.read(bitsPerPixel, 2);

  int16_t bytesPerPixel = twoByteValue(bitsPerPixel) / 8;

  char * fillerArray2 = new char [24];
  imageFile.read(fillerArray2, 24);

  char * fillerArray3 = new char [(dataOffset - (40+14))];
  imageFile.read(fillerArray3, (dataOffset - (40+14)) );

  delete [] fillerArray1;
  delete [] pixelDataOffset;
  delete [] headerSize;
  delete [] imageWidth;
  delete [] imageHeight;
  delete [] Planes;
  delete [] bitsPerPixel;
  delete [] fillerArray2;
  delete [] fillerArray3;

  fillerArray1 = nullptr;
  pixelDataOffset = nullptr;
  headerSize = nullptr;
  imageWidth = nullptr;
  imageHeight = nullptr;
  Planes = nullptr;
  bitsPerPixel = nullptr;
  fillerArray2 = nullptr;
  fillerArray3 = nullptr;

  PixelData ** pixelArray = new PixelData * [imageHeightInPixels];
    for (int32_t heightCount = 0; heightCount < imageHeightInPixels; heightCount++)
      pixelArray[heightCount] = new PixelData[imageWidthInPixels];

  PixelData * posterizedPalette = new PixelData [POSTERIZED_PALETTE_SIZE];

  char pixelBuffer[4];

  for (int32_t heightCount = imageHeightInPixels - 1; heightCount >= 0; heightCount--)
    for (int32_t widthCount = 0; widthCount < imageWidthInPixels; widthCount++)
    {
      imageFile.read(pixelBuffer, bytesPerPixel);
      pixelArray[heightCount][widthCount].set_BGR(pixelBuffer);

      // GIMP posterizing method
      double blueDecimal = pixelArray[heightCount][widthCount].get_blue() / 255.0;
      double greenDecimal = pixelArray[heightCount][widthCount].get_green() / 255.0;
      double redDecimal = pixelArray[heightCount][widthCount].get_red() / 255.0;

      uint8_t bluePosterized = 255 * rint(blueDecimal*POSTERIZE_LEVEL) / POSTERIZE_LEVEL;
      uint8_t greenPosterized = 255 * rint(greenDecimal*POSTERIZE_LEVEL) / POSTERIZE_LEVEL;
      uint8_t redPosterized = 255 * rint(redDecimal*POSTERIZE_LEVEL) / POSTERIZE_LEVEL;

      pixelArray[heightCount][widthCount].set_blueNumber(bluePosterized);
      pixelArray[heightCount][widthCount].set_greenNumber(greenPosterized);
      pixelArray[heightCount][widthCount].set_redNumber(redPosterized);

      populatePaletteWithPixel(posterizedPalette, bluePosterized, greenPosterized, redPosterized);
    }

  PixelData * dominantColorsArray = new PixelData[DOMINANT_COLORS_NUMBER];

  obtainDominantColors(posterizedPalette, dominantColorsArray, DOMINANT_COLORS_NUMBER);

  delete [] posterizedPalette;

  for (int32_t heightCount = (imageHeightInPixels - 1); heightCount >= 0; heightCount--)
      for (int32_t widthCount = 0; widthCount < imageWidthInPixels; widthCount++)
        setToClosestDominantColor(DOMINANT_COLORS_NUMBER, pixelArray[heightCount][widthCount], dominantColorsArray);

  imageFile.close();

  PixelData * exportColors = new PixelData[COLORS_TO_EXPORT_NUMBER];

  /* exportColors consists of colors from dominantColorsArray
     and their respective red, green and blue values. */

  //White:
  exportColors[0].set_blueNumber(255);
  exportColors[0].set_greenNumber(255);
  exportColors[0].set_redNumber(255);

 // Red:
  exportColors[1].set_blueNumber(0);
  exportColors[1].set_greenNumber(0);
  exportColors[1].set_redNumber(255);

  int32_t * pocketsPerColor = new int32_t[COLORS_TO_EXPORT_NUMBER];
  for (uint8_t color = 0; color < COLORS_TO_EXPORT_NUMBER; color++)
    pocketsPerColor[color] = 1;

  Pocket ** assortedPocketsOfColors = new Pocket * [COLORS_TO_EXPORT_NUMBER];
  for (uint8_t color = 0; color < COLORS_TO_EXPORT_NUMBER; color++)
    assortedPocketsOfColors[color] = new Pocket[pocketsPerColor[color]];

  pocketGenerator(exportColors, assortedPocketsOfColors, pocketsPerColor, pixelArray,
                  imageHeightInPixels, imageWidthInPixels);

  delete [] exportColors;

  for (int32_t heightCount = 0; heightCount < imageHeightInPixels; heightCount++)
         delete [] pixelArray[heightCount];
  delete [] pixelArray;
  pixelArray = nullptr;

  int32_t * shapesPerColor = new int32_t [COLORS_TO_EXPORT_NUMBER];
  for (uint8_t color = 0; color < COLORS_TO_EXPORT_NUMBER; color++)
    shapesPerColor[color] = 1;

  int32_t ** pocketsPerShape = new int32_t * [COLORS_TO_EXPORT_NUMBER];
  for (uint8_t color = 0; color < COLORS_TO_EXPORT_NUMBER; color++)
  {
    pocketsPerShape[color] = new int32_t [shapesPerColor[color]];
    for (int32_t shape = 0; shape < shapesPerColor[color]; shape++)
      pocketsPerShape[color][shape] = 0;
  }

  Pocket *** shapePockets = new Pocket ** [COLORS_TO_EXPORT_NUMBER];
  for (uint8_t color = 0; color < COLORS_TO_EXPORT_NUMBER; color++)
  {
    shapePockets[color] = new Pocket * [1];
    shapePockets[color][0] = new Pocket [1];
  }

  pocketToShapeOrganizer(shapePockets, assortedPocketsOfColors, pocketsPerColor,
                         pocketsPerShape, shapesPerColor);

  for (uint8_t color = 0; color < COLORS_TO_EXPORT_NUMBER; color++)
    delete [] assortedPocketsOfColors[color];
  delete [] assortedPocketsOfColors;
  assortedPocketsOfColors = nullptr;

  for (uint8_t color = 0; color < COLORS_TO_EXPORT_NUMBER; color++)
    for (int32_t shape = 0; shape < shapesPerColor[color]; shape++)
      sortPocketOrderOfShape(shapePockets, pocketsPerShape, color, shape);

  std::ofstream gCodeExportFile("ladyLibertyG_CodeOutput.txt");
  gCodeExportFile << "G90" << std::endl << "G21" << std::endl << "G0 X0.0 Y0.0" << std::endl;

  for (uint8_t shape = 0; shape < shapesPerColor[0]; shape++)
    exportShapeG_Code(gCodeExportFile, shapePockets, pocketsPerShape, 0, shape);

  return EXIT_SUCCESS;
}


