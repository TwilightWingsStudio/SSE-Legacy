/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "stdh.h"

#include <Engine/Graphics/Font.h>
#include <Engine/Base/Stream.h>
#include <Engine/Graphics/Texture.h>
#include <Engine/Graphics/Color.h>
#include <Engine/Templates/Stock_CTextureData.h>

// Some default fonts.
CFontData *_pfdDisplayFont;
CFontData *_pfdConsoleFont;

// --------------------------------------------------------------------------------------
// Constructor deletes letter data.
// --------------------------------------------------------------------------------------
CFontCharData::CFontCharData(void)
{
  fcd_pixXOffset = 0;
  fcd_pixYOffset = 0;
  fcd_pixStart   = 0;
  fcd_pixEnd   = 0;
}

// --------------------------------------------------------------------------------------
// Read data from the stream.
// --------------------------------------------------------------------------------------
void CFontCharData::Read_t(  CTStream *inFile)
{
  (*inFile) >> fcd_pixXOffset;
  (*inFile) >> fcd_pixYOffset;
  (*inFile) >> fcd_pixStart;
  (*inFile) >> fcd_pixEnd;
}

// --------------------------------------------------------------------------------------
// Write data to the stream.
// --------------------------------------------------------------------------------------
void CFontCharData::Write_t( CTStream *outFile)
{
  (*outFile) << fcd_pixXOffset;
  (*outFile) << fcd_pixYOffset;
  (*outFile) << fcd_pixStart;
  (*outFile) << fcd_pixEnd;
}

CFontData::CFontData()
{
  fd_ptdTextureData = NULL;
  fd_fnTexture = CTString("");
  
  fd_bMonotype = FALSE;
  fd_pixCharSpacing = 0;
  fd_pixLineSpacing = 0;
}

CFontData::~CFontData()
{
  Clear();
}

void CFontData::Clear()
{
  if (fd_ptdTextureData != NULL) {
    fd_fnTexture = CTString("");
    _pTextureStock->Release(fd_ptdTextureData);
    fd_ptdTextureData = NULL;
  }
}

// --------------------------------------------------------------------------------------
// Read data from the stream.
// --------------------------------------------------------------------------------------
void CFontData::Read_t( CTStream *inFile) // throw char *
{
  // Clear current font data (if needed).
  Clear();
  
  CChunkID idChunk = inFile->PeekID_t();

  // Vanilla font data.
  if (idChunk == CChunkID("FTTF")) {
    Read_FTTF_t(inFile);

  // [SSE] EFND - Evolution FoNt Data.
  } else if (idChunk == CChunkID("EFND")) {
    Read_EFND_t(inFile);
    
  // Unknown signature.
  } else {
    ThrowF_t( TRANS("Invalid font data signature (\"%s\")\nfound while reading font data \"%s\"."),
        (char*)idChunk, (CTString&)inFile->GetDescription());
  }
  
  // load corresponding texture file
  fd_ptdTextureData = _pTextureStock->Obtain_t(fd_fnTexture);

  // reload corresponding texture if not loaded in largest mip-map
  fd_ptdTextureData->Force( TEX_CONSTANT);

  fd_fcdFontCharData[' '].fcd_pixStart = 0;
  SetSpaceWidth(0.5f);
}

// --------------------------------------------------------------------------------------
// Read data from the stream.
// --------------------------------------------------------------------------------------
void CFontData::Read_FTTF_t( CTStream *inFile) // throw char *
{
  // Read the filename of the corresponding texture file.
  inFile->ExpectID_t( CChunkID("FTTF"));
  (*inFile) >> fd_fnTexture;

  // Read maximum width and height of all letters.
  (*inFile) >> fd_pixCharWidth;
  (*inFile) >> fd_pixCharHeight;
  
  //CPrintF("Loading font '%s'\n", fd_fnTexture);
  
  // Read entire letter data table.
  for (INDEX iLetterData=0; iLetterData < 256; iLetterData ++) {
    fd_fcdFontCharData[ iLetterData].Read_t( inFile);
  }

  // initialize default font variables
  SetVariableWidth();
  SetCharSpacing(+1);
  SetLineSpacing(+1);
}

// --------------------------------------------------------------------------------------
// Write data to the stream.
// --------------------------------------------------------------------------------------
void CFontData::Write_t( CTStream *outFile) // throw char *
{
  ASSERT( fd_ptdTextureData != NULL);

  Write_EFND_t(outFile);
}

// --------------------------------------------------------------------------------------
// Function used for creating font data object
// --------------------------------------------------------------------------------------
void CFontData::Make_t( const CTFileName &fnTexture, PIX pixCharWidth, PIX pixCharHeight,
                        CTFileName &fnOrderFile, BOOL bUseAlpha, BOOL bMonotype)
{
  // Do it only if font has not been created already.
  ASSERT( fd_ptdTextureData == NULL);
  
  fd_bMonotype = bMonotype;

  // Remember texture name
  fd_fnTexture = fnTexture;

  // Load texture and cache width
  fd_ptdTextureData = _pTextureStock->Obtain_t(fd_fnTexture);
  fd_ptdTextureData->Force( TEX_STATIC|TEX_CONSTANT);
  PIX pixTexWidth = fd_ptdTextureData->GetPixWidth();

  // Load ascii order file (no application path necessary).
  CTString strLettersOrder;
  IgnoreApplicationPath();
  strLettersOrder.Load_t( fnOrderFile);
  UseApplicationPath();

  // Remember letter width and height.
  fd_pixCharWidth  = pixCharWidth;
  fd_pixCharHeight = pixCharHeight;

  // Determine address in memory where font definition begins in its larger mip-map.
  ULONG *pulFont = fd_ptdTextureData->td_pulFrames;
  ASSERT(pulFont != NULL);

  // Find number of letters in line (assuming that the 1st line represents the width of every line).
  INDEX iLettersInLine = 0;
  while ((strLettersOrder[iLettersInLine] != '\n') && iLettersInLine < strlen(strLettersOrder)) iLettersInLine++;
  if (iLettersInLine <= 0) FatalError( "Invalid font definition ASCII file.");

  // Determine pixelcheck mast depending of alpha channel usage.
  COLOR colPixMask = 0xFFFFFF00;  // FC is because of small tolerance for black 
  if (bUseAlpha) colPixMask = 0xFFFFFFFF;

  // How much we must add to jump to character down.
  PIX pixFontCharModulo = pixTexWidth * fd_pixCharHeight;

  // For all letters in font (ranging from space to last letter that user defined).
  INDEX iLetter = 0;
  INDEX iCurrentLetterLine = 0;

  while (iLetter < strlen(strLettersOrder))
  {
    // For letters in one line...
    for (INDEX iCurrentLetterColumn=0; iCurrentLetterColumn<iLettersInLine; iCurrentLetterColumn++)
    {
      // Test if we at the end of whole array.
      if (iLetter >= strlen(strLettersOrder)) break;

      // Get char params.
      unsigned char chrLetter = strLettersOrder[iLetter++];

      // Reset current letter's width.
      PIX pixCurrentStart = fd_pixCharWidth;
      PIX pixCurrentEnd   = 0;

      // [SSE]
      if (fd_bMonotype) {
        fd_fcdFontCharData[chrLetter].fcd_pixStart = 0;
        fd_fcdFontCharData[chrLetter].fcd_pixEnd   = fd_pixCharWidth;

      } else {
        // for all of the letter's lines
        for( INDEX iPixelLine=0; iPixelLine<fd_pixCharHeight; iPixelLine++)
        { // for all of the letter's pixels
          for( INDEX iPixelColumn=0; iPixelColumn<fd_pixCharWidth; iPixelColumn++)
          {
            // Calculate current pixel's adress in font's texture.
            ULONG *puwPixel = (ULONG*)( pulFont + pixFontCharModulo * iCurrentLetterLine +  // calc right letter line
                                        fd_pixCharWidth * iCurrentLetterColumn +            // move to right letter column
                                        pixTexWidth * iPixelLine +   // move trough pixel lines of one letter
                                        iPixelColumn);               // move trough pixel columns of one letter

            // If we test alpha channel and alpha value is not 0.
            if (ByteSwap(*puwPixel) & colPixMask)
            {
              // If examined pixel is narrower in letter than last opaque pixel found, remember it as left-most pixel.
              if (iPixelColumn < pixCurrentStart) pixCurrentStart = iPixelColumn;
              // If examined pixel is wider in letter than last opaque pixel found, remember it as right-most pixel.
              if (iPixelColumn > pixCurrentEnd) pixCurrentEnd = iPixelColumn;
            }
          }
        }

        fd_fcdFontCharData[chrLetter].fcd_pixStart = pixCurrentStart;
        fd_fcdFontCharData[chrLetter].fcd_pixEnd   = pixCurrentEnd +1;
      }

      // Letter's data is stored into table on appropriate place.
      fd_fcdFontCharData[chrLetter].fcd_pixXOffset = iCurrentLetterColumn * fd_pixCharWidth;
      fd_fcdFontCharData[chrLetter].fcd_pixYOffset = iCurrentLetterLine   * fd_pixCharHeight;
    }

    iCurrentLetterLine++; // Advance to next line in text file.
    iLetter++;            // Skip carriage return.
  }

  // Set default space width.
  fd_fcdFontCharData[' '].fcd_pixStart = 0;
  SetSpaceWidth(0.5f);

  // All done.
  SetVariableWidth();
  _pTextureStock->Release( fd_ptdTextureData);
}

