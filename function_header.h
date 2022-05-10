// A Ikic 2020

int32_t fourByteValue(char byteArray[4])
{
  return (int32_t) (byteArray[0] |
                    byteArray[1] << 8 |
                    byteArray[2] << 16 |
                    byteArray[3] << 24 );
}

int16_t twoByteValue(char byteArray[2])
{
  return (int16_t) (byteArray[0] |
                    byteArray[1] << 8);
}

void populatePaletteWithPixel(PixelData posterizedPalette[],
                              uint8_t blue, uint8_t green, uint8_t red)
{
  uint8_t index = (blue + green + red) % POSTERIZED_PALETTE_SIZE;

  if (posterizedPalette[index].get_count() == 0)
    {
      posterizedPalette[index].set_blueNumber(blue);
      posterizedPalette[index].set_greenNumber(green);
      posterizedPalette[index].set_redNumber(red);
      posterizedPalette[index].count_plusOne();
    }

  else
  {
    if (posterizedPalette[index].get_blue() == blue &&
        posterizedPalette[index].get_green() == green &&
        posterizedPalette[index].get_red() == red)
      posterizedPalette[index].count_plusOne();

    else
    {
      bool newColor = true;

      for (uint8_t paletteIndex = 0; paletteIndex < POSTERIZED_PALETTE_SIZE; paletteIndex++)
        if (posterizedPalette[paletteIndex].get_blue() == blue &&
            posterizedPalette[paletteIndex].get_green() == green &&
            posterizedPalette[paletteIndex].get_red() == red)
          {
            newColor = false;
            posterizedPalette[paletteIndex].count_plusOne();
            break;
          }

      if (newColor == true)
        for (uint8_t paletteIndex = 0; paletteIndex < POSTERIZED_PALETTE_SIZE; paletteIndex++)
        {
          if (posterizedPalette[paletteIndex].get_count() == 0)
          {
            posterizedPalette[paletteIndex].set_blueNumber(blue);
            posterizedPalette[paletteIndex].set_greenNumber(green);
            posterizedPalette[paletteIndex].set_redNumber(red);
            posterizedPalette[paletteIndex].count_plusOne();
            break;
          }
        }
    }
  }
}

void obtainDominantColors(PixelData posterizedPalette[], PixelData dominantColorsArray[],
                          uint8_t dominantColors)
{
  for (uint8_t color = 0; color < dominantColors; color++)
  {
    int32_t maxCount = 0;
    int8_t maxIndex = -1;

    for (uint8_t index = 0; index < POSTERIZED_PALETTE_SIZE; index++)
      if (posterizedPalette[index].get_count() > maxCount &&
          posterizedPalette[index].get_goneThru() == 0)
      {
        maxCount = posterizedPalette[index].get_count();
        maxIndex = index;
      }

    if (maxIndex != -1)
    {
      posterizedPalette[maxIndex].set_goneThru(true);
      dominantColorsArray[color].set_countNum(maxCount);
      dominantColorsArray[color].set_blueNumber(posterizedPalette[maxIndex].get_blue() );
      dominantColorsArray[color].set_greenNumber(posterizedPalette[maxIndex].get_green() );
      dominantColorsArray[color].set_redNumber(posterizedPalette[maxIndex].get_red() );
    }
  }
}

bool isDifferentPixelColor(PixelData pixel, PixelData referencePixel)
{
  if (pixel.get_blue() != referencePixel.get_blue() ||
      pixel.get_green() != referencePixel.get_green() ||
      pixel.get_red() != referencePixel.get_red() )
    return true;
  else
    return false;
}

void setToClosestDominantColor(uint8_t dominantColors, PixelData & pixel,
                               PixelData dominantColorsArray[])
{
  int32_t maxColorDistance = 442; // 8-bit maximum RGB distance using 3D Pythagorean Theorem
  int32_t closestDominantIndex = 0;

  for (uint8_t index = 0; index < dominantColors; index++)
  {
    int32_t ColorDistance =
    sqrt(pow((pixel.get_blue() - dominantColorsArray[index].get_blue()), 2) +
         pow((pixel.get_green() - dominantColorsArray[index].get_green()), 2) +
         pow((pixel.get_red() - dominantColorsArray[index].get_red()), 2) );

    if (ColorDistance < maxColorDistance)
    {
      maxColorDistance = ColorDistance;
      closestDominantIndex = index;
    }
  }

  pixel.set_blueNumber(dominantColorsArray[closestDominantIndex].get_blue() );
  pixel.set_greenNumber(dominantColorsArray[closestDominantIndex].get_green() );
  pixel.set_redNumber(dominantColorsArray[closestDominantIndex].get_red() );
}

void copyPocket(Pocket & targetPocket, Pocket referencePocket)
{
  targetPocket.set_startX(referencePocket.get_startX() );
  targetPocket.set_endX(referencePocket.get_endX() );
  targetPocket.set_lineY(referencePocket.get_lineY() );
}

void pocketGenerator(PixelData * & exportColors, Pocket **  & assortedPocketsOfColors,
                     int32_t * & pocketsPerColor, PixelData ** pixelArray,
                     int32_t imageHeightInPixels, int32_t imageWidthInPixels)
{
  for (uint8_t color = 0; color < COLORS_TO_EXPORT_NUMBER; color++)
  {
    for (int32_t heightCount = 0; heightCount < imageHeightInPixels; heightCount++)
      for (int32_t widthCount = 0; widthCount < imageWidthInPixels; widthCount++)
      {
        if (isDifferentPixelColor(pixelArray[heightCount][widthCount], exportColors[color]) == 0 &&
            pixelArray[heightCount][widthCount].get_goneThru() == 0)
        {
          assortedPocketsOfColors[color][0].set_startX(widthCount);
          assortedPocketsOfColors[color][0].set_lineY(heightCount);
          pixelArray[heightCount][widthCount].set_goneThru(true);

          for (int32_t nextWidthCount = (widthCount + 1); nextWidthCount < imageWidthInPixels; nextWidthCount++)
          {
            if (isDifferentPixelColor(pixelArray[heightCount][nextWidthCount], exportColors[color]) == 1)
            {
              assortedPocketsOfColors[color][0].set_endX(nextWidthCount - 1);
              goto restOfPixelArray;
            }

            else
              pixelArray[heightCount][nextWidthCount].set_goneThru(true);
          }
        }
      }

   restOfPixelArray:

   for (int32_t heightCount = 0; heightCount < imageHeightInPixels; heightCount++)
    for (int32_t widthCount = 0; widthCount < imageWidthInPixels; widthCount++)
    {
      if (isDifferentPixelColor(pixelArray[heightCount][widthCount], exportColors[color]) == 0 &&
          pixelArray[heightCount][widthCount].get_goneThru() == 0)
      {
        pixelArray[heightCount][widthCount].set_goneThru(true);

        Pocket * temporaryArray;
        temporaryArray = new Pocket[pocketsPerColor[color]];

        for (int32_t index = 0; index < pocketsPerColor[color]; index++)
          copyPocket(temporaryArray[index], assortedPocketsOfColors[color][index] );

        delete [] assortedPocketsOfColors[color];
        pocketsPerColor[color] += 1;
        assortedPocketsOfColors[color] = new Pocket[(pocketsPerColor[color])];

        for (int32_t index = 0; index < (pocketsPerColor[color] - 1); index++)
            copyPocket(assortedPocketsOfColors[color][index], temporaryArray[index] );

        delete [] temporaryArray;

        assortedPocketsOfColors[color][pocketsPerColor[color]-1].set_startX(widthCount);
        assortedPocketsOfColors[color][pocketsPerColor[color]-1].set_lineY(heightCount);

        for (int32_t nextWidthCount = (widthCount + 1); nextWidthCount < imageWidthInPixels; nextWidthCount++)
        {
          if (isDifferentPixelColor(pixelArray[heightCount][nextWidthCount], exportColors[color]) == 1)
          {
            assortedPocketsOfColors[color][pocketsPerColor[color] - 1].set_endX(nextWidthCount - 1);
            break;
          }

          else
            pixelArray[heightCount][nextWidthCount].set_goneThru(true);
        }
      }
    }
  }
}

bool arePocketsInContact(Pocket pocket1, Pocket pocket2)
{
  if (abs(pocket1.get_lineY() - pocket2.get_lineY()) == 1)
    {
      if (abs(pocket1.get_startX() - pocket2.get_startX()) == 1 ||
          abs(pocket1.get_endX() - pocket2.get_endX()) == 1 ||
          abs(pocket1.get_startX() - pocket2.get_endX()) == 1 ||
          abs(pocket1.get_endX() - pocket2.get_startX()) == 1)
          return true;

      for (int32_t count1 = pocket1.get_startX(); count1 <= pocket1.get_endX(); count1++)
        for (int32_t count2 = pocket2.get_startX(); count2 <= pocket2.get_endX(); count2++)
        {
          if (count2 == count1)
            return true;
        }
    }

  return false;
}

void pocketToShapeOrganizer(Pocket *** & shapePockets, Pocket ** & assortedPocketsOfColors,
                            int32_t * & pocketsPerColor, int32_t ** & pocketsPerShape,
                            int32_t * & shapesPerColor)
{
   for (uint8_t color = 0; color < COLORS_TO_EXPORT_NUMBER; color++)
  {
    for (int32_t shape = 0; shape < shapesPerColor[color]; shape++)
   {
      for (int32_t pocket = 0; pocket < pocketsPerColor[color]; pocket++)
      {
        if (assortedPocketsOfColors[color][pocket].get_goneThru() == 0)
        {
          assortedPocketsOfColors[color][pocket].set_goneThru(true);

          copyPocket(shapePockets[color][shape][0], assortedPocketsOfColors[color][pocket] );

          pocketsPerShape[color][shape] += 1;
          break;
        }
      }

      for (int32_t count1 = 0; count1 < pocketsPerShape[color][shape]; count1++)
      {
        for (int32_t count2 = 0; count2 < pocketsPerColor[color]; count2++)
        {
          if (arePocketsInContact(shapePockets[color][shape][count1],
                        assortedPocketsOfColors[color][count2]) == 1 &&
                        assortedPocketsOfColors[color][count2].get_goneThru() == false)
          {
            assortedPocketsOfColors[color][count2].set_goneThru(true);
            Pocket * temporaryArray;
            temporaryArray = new Pocket [(pocketsPerShape[color][shape])];

            for (int32_t pocket = 0; pocket < pocketsPerShape[color][shape]; pocket++)
              copyPocket(temporaryArray[pocket], shapePockets[color][shape][pocket] );

            delete [] shapePockets[color][shape];
            pocketsPerShape[color][shape]++;
            shapePockets[color][shape] = new Pocket [pocketsPerShape[color][shape]];

            for (int32_t pocket = 0; pocket < (pocketsPerShape[color][shape] - 1); pocket++)
                copyPocket(shapePockets[color][shape][pocket], temporaryArray[pocket] );

            delete [] temporaryArray;
            temporaryArray = nullptr;

            copyPocket(shapePockets[color][shape][(pocketsPerShape[color][shape] - 1)],
                       assortedPocketsOfColors[color][count2] );
        }
      }
    }

    for (int32_t count = 0; count < pocketsPerColor[color]; count++)
    {
      if (assortedPocketsOfColors[color][count].get_goneThru() == 0)
      {
        // increase pocketsPerShape first
        int32_t * temporaryArray = new int32_t[shapesPerColor[color]];

        for (int32_t shape = 0; shape < shapesPerColor[color]; shape++)
          temporaryArray[shape] = pocketsPerShape[color][shape];

        delete [] pocketsPerShape[color];
        shapesPerColor[color]++;
        pocketsPerShape[color] = new int32_t[shapesPerColor[color]];

        for (int32_t shape = 0; shape < shapesPerColor[color] - 1; shape++)
          pocketsPerShape[color][shape] = temporaryArray[shape];

        pocketsPerShape[color][shapesPerColor[color] - 1] = 0;

        delete [] temporaryArray;
        temporaryArray = nullptr;

        // now increase shapePockets
        Pocket ** temporaryArray2;
        temporaryArray2 = new Pocket* [shapesPerColor[color]];

        for (int32_t shape = 0; shape < shapesPerColor[color]; shape++)
          temporaryArray2[shape] = new Pocket [pocketsPerShape[color][shape]];

        for (int32_t count1 = 0; count1 < shapesPerColor[color]; count1++)
          for (int32_t count2 = 0; count2 < pocketsPerShape[color][count1]; count2++)
            copyPocket(temporaryArray2[count1][count2], shapePockets[color][count1][count2] );

        delete [] shapePockets[color][shape];
        delete [] shapePockets[color];
        shapePockets[color] = new Pocket * [shapesPerColor[color]];
        for (int32_t count = 0; count < (shapesPerColor[color] - 1); count++)
          shapePockets[color][count] = new Pocket [pocketsPerShape[color][count]];

        shapePockets[color][shapesPerColor[color] - 1] = new Pocket [1];

        for (int32_t count1 = 0; count1 < (shapesPerColor[color] - 1); count1++)
          for (int32_t count2 = 0; count2 < pocketsPerShape[color][count1]; count2++)
               copyPocket(shapePockets[color][count1][count2], temporaryArray2[count1][count2]);

        for (int32_t index = 0; index < shapesPerColor[color]; index++)
          delete [] temporaryArray2[index];
        delete [] temporaryArray2;
        temporaryArray2 = nullptr;
        break;
      }
    }
   }
  }
}

void sortPocketOrderOfShape(Pocket *** & shapePockets, int32_t ** & pocketsPerShape,
                            uint8_t color, int32_t shape)
{
  int32_t temporaryArrayIndex = -1;

  Pocket * temporaryArray = new Pocket [(pocketsPerShape[color][shape])];

  for (int32_t pocket = 0; pocket < pocketsPerShape[color][shape]; pocket++)
    shapePockets[color][shape][pocket].set_goneThru(false);

  for (int32_t pocket = 0; pocket < pocketsPerShape[color][shape]; pocket++)
  {
    if (shapePockets[color][shape][pocket].get_goneThru() == false)
    {
      shapePockets[color][shape][pocket].set_goneThru(true);
      temporaryArrayIndex += 1;
      copyPocket(temporaryArray[temporaryArrayIndex], shapePockets[color][shape][pocket]);

      restOfPockets:
      for (int32_t count2 = 0; count2 < pocketsPerShape[color][shape]; count2++)
      {
        if (arePocketsInContact(temporaryArray[temporaryArrayIndex], shapePockets[color][shape][count2]) == 1
            && shapePockets[color][shape][count2].get_goneThru() == 0)
          {
            shapePockets[color][shape][count2].set_goneThru(true);
            temporaryArrayIndex += 1;

            copyPocket(temporaryArray[temporaryArrayIndex], shapePockets[color][shape][count2] );

            goto restOfPockets;
          }
      }
    }
  }

  for (int32_t pocket = 0; pocket < pocketsPerShape[color][shape]; pocket++)
    {
      shapePockets[color][shape][pocket].set_goneThru(false);
      copyPocket(shapePockets[color][shape][pocket], temporaryArray[pocket] );
    }

  delete [] temporaryArray;
  temporaryArray = nullptr;
}

void exportPocketG_Code(std::ofstream & gCodeExportFile, Pocket *** shapePockets, int32_t color, int32_t shape,
                        int32_t pocket, int8_t startEndSwitch)
{
  if (startEndSwitch == 1)
  {
    gCodeExportFile << std::fixed << std::setprecision(G_CODE_DECIMAL_POINTS) << "G0 X"
    << X_INVERSION_FACTOR * PIXEL_HEIGHT_MM * (shapePockets[color][shape][pocket].get_startX() - HALF_IMAGE_WIDTH_IN_PIXELS) << " Y"
    << Y_INVERSION_FACTOR * PIXEL_HEIGHT_MM * ( (-1) * shapePockets[color][shape][pocket].get_lineY() + HALF_IMAGE_HEIGHT_IN_PIXELS )
    << std::endl;
    for (uint8_t count = 1; count <= G_CODE_STITCH_NUMBER; count++ )
      gCodeExportFile << "G0 Z" << Z_INVERSION_FACTOR * G_CODE_Z_MM  << std::endl << "G92 Z0" << std::endl;

    if (STRAIGHT_STITCH_LENGTH_MM != 0)
    {
      int32_t straightStitchQuotient = abs(
      PIXEL_HEIGHT_MM*(int32_t)(shapePockets[color][shape][pocket].get_startX() - HALF_IMAGE_WIDTH_IN_PIXELS) -
      PIXEL_HEIGHT_MM*(int32_t)(shapePockets[color][shape][pocket].get_endX() - HALF_IMAGE_WIDTH_IN_PIXELS) ) /
      STRAIGHT_STITCH_LENGTH_MM;

     for (int32_t stitchCount = 1; stitchCount <= straightStitchQuotient; stitchCount++)
       {
         gCodeExportFile << std::fixed << std::setprecision(G_CODE_DECIMAL_POINTS) << "G0 X"
         << X_INVERSION_FACTOR * PIXEL_HEIGHT_MM * (shapePockets[color][shape][pocket].get_startX() - HALF_IMAGE_WIDTH_IN_PIXELS) + (stitchCount * STRAIGHT_STITCH_LENGTH_MM)
         << " Y" << Y_INVERSION_FACTOR * PIXEL_HEIGHT_MM * ( (-1) * shapePockets[color][shape][pocket].get_lineY() + HALF_IMAGE_HEIGHT_IN_PIXELS ) << std::endl
         << "G0 Z" << Z_INVERSION_FACTOR * G_CODE_Z_MM << std::endl << "G92 Z0" << std::endl;
       }
    }

    gCodeExportFile << std::fixed << std::setprecision(G_CODE_DECIMAL_POINTS) << "G0 X"
    << X_INVERSION_FACTOR * PIXEL_HEIGHT_MM * (shapePockets[color][shape][pocket].get_endX() - HALF_IMAGE_WIDTH_IN_PIXELS) << " Y"
    << Y_INVERSION_FACTOR * PIXEL_HEIGHT_MM * ( (-1) * shapePockets[color][shape][pocket].get_lineY() + HALF_IMAGE_HEIGHT_IN_PIXELS ) << std::endl;
    for (uint8_t count = 1; count <= G_CODE_STITCH_NUMBER; count++ )
      gCodeExportFile << "G0 Z" << Z_INVERSION_FACTOR * G_CODE_Z_MM  << std::endl << "G92 Z0" << std::endl;
  }

  else
  {
    gCodeExportFile << std::fixed << std::setprecision(G_CODE_DECIMAL_POINTS) << "G0 X"
    << X_INVERSION_FACTOR * PIXEL_HEIGHT_MM * (shapePockets[color][shape][pocket].get_endX() - HALF_IMAGE_WIDTH_IN_PIXELS) << " Y"
    << Y_INVERSION_FACTOR * PIXEL_HEIGHT_MM * ( (-1) * shapePockets[color][shape][pocket].get_lineY() + HALF_IMAGE_HEIGHT_IN_PIXELS )
    << std::endl;
    for (uint8_t count = 1; count <= G_CODE_STITCH_NUMBER; count++ )
      gCodeExportFile << "G0 Z" << Z_INVERSION_FACTOR * G_CODE_Z_MM << std::endl << "G92 Z0" << std::endl;

    if (STRAIGHT_STITCH_LENGTH_MM != 0)
    {
      int32_t straightStitchQuotient = abs(
      PIXEL_HEIGHT_MM*(shapePockets[color][shape][pocket].get_startX() - HALF_IMAGE_WIDTH_IN_PIXELS) -
      PIXEL_HEIGHT_MM*(shapePockets[color][shape][pocket].get_endX() - HALF_IMAGE_WIDTH_IN_PIXELS) ) /
      STRAIGHT_STITCH_LENGTH_MM;

      for (int32_t stitchCount = 1; stitchCount <= straightStitchQuotient; stitchCount++)
      {
       gCodeExportFile << std::fixed << std::setprecision(G_CODE_DECIMAL_POINTS) << "G0 X"
       << X_INVERSION_FACTOR * PIXEL_HEIGHT_MM * (shapePockets[color][shape][pocket].get_endX() - HALF_IMAGE_WIDTH_IN_PIXELS) + (stitchCount * STRAIGHT_STITCH_LENGTH_MM)
       << " Y" << Y_INVERSION_FACTOR * PIXEL_HEIGHT_MM * ( (-1) * shapePockets[color][shape][pocket].get_lineY() + HALF_IMAGE_HEIGHT_IN_PIXELS ) << std::endl
       << "G0 Z" << Z_INVERSION_FACTOR * G_CODE_Z_MM << std::endl << "G92 Z0" << std::endl;
      }
    }

    gCodeExportFile << std::fixed << std::setprecision(G_CODE_DECIMAL_POINTS) << "G0 X"
    << X_INVERSION_FACTOR * PIXEL_HEIGHT_MM * (shapePockets[color][shape][pocket].get_startX() - HALF_IMAGE_WIDTH_IN_PIXELS) << " Y"
    << Y_INVERSION_FACTOR * PIXEL_HEIGHT_MM * ( (-1) * shapePockets[color][shape][pocket].get_lineY() + HALF_IMAGE_HEIGHT_IN_PIXELS ) << std::endl;
    for (uint8_t count = 1; count <= G_CODE_STITCH_NUMBER; count++ )
      gCodeExportFile << "G0 Z" << Z_INVERSION_FACTOR * G_CODE_Z_MM << std::endl << "G92 Z0" << std::endl;
  }
}

void exportShapeG_Code(std::ofstream & gCodeExportFile, Pocket *** shapePockets,
                      int32_t ** pocketsPerShape, int32_t color, int32_t shape)
{

  uint8_t pocketGapFactor = rint(MM_BETWEEN_STITCH_ROWS / PIXEL_HEIGHT_MM);
  int32_t pocketsToSkipCounter = 0;
  /* Factor and counter are used to skip pockets to
     achieve desired width between lines */

  int8_t startEndSwitch = 1;
  int8_t startEndSwitchMultiplier = -1;
  if (CROSS_PATTERN_TOGGLE == 1)
    startEndSwitchMultiplier = 1;

  exportPocketG_Code(gCodeExportFile, shapePockets, color, shape, 0, startEndSwitch);

  startEndSwitch *= startEndSwitchMultiplier;

  for (int32_t pocket = 1; pocket < (pocketsPerShape[color][shape] - 1); pocket++)
  {
   if (arePocketsInContact(shapePockets[color][shape][pocket],
                 shapePockets[color][shape][pocket - 1]) == 1)
      {
       if (pocketsToSkipCounter != pocketGapFactor)
        pocketsToSkipCounter += 1;

       else
       {
         pocketsToSkipCounter = 0;
         startEndSwitch *= startEndSwitchMultiplier;
         exportPocketG_Code(gCodeExportFile, shapePockets, color, shape, pocket, startEndSwitch);
       }
      }

  else
    {
      pocketsToSkipCounter = 0;
      startEndSwitch = 1;
      exportPocketG_Code(gCodeExportFile, shapePockets, color, shape, pocket, startEndSwitch);
    }
  }

  exportPocketG_Code(gCodeExportFile, shapePockets, color, shape, (pocketsPerShape[color][shape] - 1), startEndSwitch);
}
