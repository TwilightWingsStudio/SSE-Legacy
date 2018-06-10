/* Copyright (c) 2018 by ZCaliptium

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

#include "StdH.h"

#include <Engine/Graphics/Font.h>
#include <Engine/Base/Stream.h>
#include <Engine/Base/Console.h>

#define EFND_CURRENT_VERSION 1

// --------------------------------------------------------------------------------------
// Variable width font character position data.
// --------------------------------------------------------------------------------------
class ENGINE_API CFontVarCharData
{
  public:
    USHORT fcd_iOffsetX, fcd_iOffsetY;
    USHORT fcd_iStartX, fcd_iEndX;
    
  public:
    // Constructor.
    CFontVarCharData(void);

    // Simple stream functions.
    void Read_t(  CTStream *inFile);
    void Write_t( CTStream *outFile);
};

// --------------------------------------------------------------------------------------
// Constructor.
// --------------------------------------------------------------------------------------
CFontVarCharData::CFontVarCharData()
{
  fcd_iOffsetX = 0;
  fcd_iOffsetY = 0;
  fcd_iStartX = 0;
  fcd_iEndX = 0;
}

// --------------------------------------------------------------------------------------
// Read data from the stream.
// --------------------------------------------------------------------------------------
void CFontVarCharData::Read_t(CTStream *inFile)
{
  (*inFile) >> fcd_iOffsetX;
  (*inFile) >> fcd_iOffsetY;
  (*inFile) >> fcd_iStartX;
  (*inFile) >> fcd_iEndX;
}

// --------------------------------------------------------------------------------------
// Write data to the stream.
// --------------------------------------------------------------------------------------
void CFontVarCharData::Write_t(CTStream *outFile)
{
  (*outFile) << fcd_iOffsetX;
  (*outFile) << fcd_iOffsetY;
  (*outFile) << fcd_iStartX;
  (*outFile) << fcd_iEndX;
}

// --------------------------------------------------------------------------------------
// Monotype font character position data.
// --------------------------------------------------------------------------------------
class ENGINE_API CFontMonotypeCharData
{
  public:
    UBYTE fcd_iCol, fcd_iRow;
    
  public:
    // Constructor.
    CFontMonotypeCharData(void);

    // Simple stream functions.
    void Read_t(  CTStream *inFile);
    void Write_t( CTStream *outFile);
};

// --------------------------------------------------------------------------------------
// Constructor.
// --------------------------------------------------------------------------------------
CFontMonotypeCharData::CFontMonotypeCharData()
{
  fcd_iCol = 0;
  fcd_iRow = 0;
}

// --------------------------------------------------------------------------------------
// Read data from the stream.
// --------------------------------------------------------------------------------------
void CFontMonotypeCharData::Read_t(CTStream *inFile)
{
  (*inFile) >> fcd_iCol;
  (*inFile) >> fcd_iRow;
}

// --------------------------------------------------------------------------------------
// Write data to the stream.
// --------------------------------------------------------------------------------------
void CFontMonotypeCharData::Write_t(CTStream *outFile)
{
  (*outFile) << fcd_iCol;
  (*outFile) << fcd_iRow;
}

// --------------------------------------------------------------------------------------
// Read data from the stream.
// --------------------------------------------------------------------------------------
void CFontData::Read_EFND_t(CTStream *inFile) // throw char *
{
  inFile->ExpectID_t(CChunkID("EFND"));
  
  ULONG ulFormatVersion;
  (*inFile) >> ulFormatVersion;

  // Check the format version. 
  if (ulFormatVersion > EFND_CURRENT_VERSION) {
    ThrowF_t("Invalid EFND format version. Version number is too high (%lu)!", ulFormatVersion);
  }

  CFontMonotypeCharData *apMonotypeCharData = NULL;
  CFontVarCharData *apVarCharData = NULL;
  BOOL bCharDataTableFound = FALSE;

  // Iterate through data chunks.
  do
  {
    CChunkID idChunk = inFile->GetID_t(); // Obtain chunk id.
    
    // Stop reading if invalid chunk encountered (EOF).
    if( idChunk == CChunkID("    ")) {
      break;
    }
    
    // Texture File Name.
    if (idChunk == CChunkID("TFNM"))
    {
      (*inFile) >> fd_fnTexture;
    }

    // Font Settings.
    else if (idChunk == CChunkID("FSGS"))
    {
      (*inFile) >> fd_pixCharWidth; // Maximum width of all characters.
      (*inFile) >> fd_pixCharHeight; // Maximum height of all characters.
      (*inFile) >> fd_pixCharSpacing;
      (*inFile) >> fd_pixLineSpacing;
    }
    
    // Read variable char data table.
    else if (idChunk == CChunkID("VCDT"))
    {
      if (bCharDataTableFound) {
        ThrowF_t( TRANS("File format violation!\nSecond char data table chunk (\"%s\") found while reading Evolution font data \"%s\"."), (char*)idChunk,
            (CTString&)inFile->GetDescription() );
      }
      
      apVarCharData = new CFontVarCharData[256];

      for (INDEX iCharData = 0; iCharData < 256; iCharData++) {
        apVarCharData[iCharData].Read_t(inFile);
      }
      
      bCharDataTableFound = TRUE;
    }
    
    // Read monotype char data table.
    else if (idChunk == CChunkID("MCDT"))
    {
      if (bCharDataTableFound) {
        ThrowF_t( TRANS("File format violation!\nSecond char data table chunk (\"%s\") found while reading Evolution font data \"%s\"."), (char*)idChunk,
            (CTString&)inFile->GetDescription() );
      }
      
      apMonotypeCharData = new CFontMonotypeCharData[256];

      for (INDEX iCharData = 0; iCharData < 256; iCharData++) {
        apMonotypeCharData[iCharData].Read_t(inFile);
      }
      
      bCharDataTableFound = TRUE;
    }

    // Error.
    else
    {
      ThrowF_t( TRANS("Unrecognisable chunk ID (\"%s\") found at %d while reading Evolution font data \"%s\"."),
                (char*)idChunk, inFile->GetPos_t(), (CTString&)inFile->GetDescription() );
    }

  } while( !inFile->AtEOF()); // Until we didn't reach EOF.

  // Generate variable width table from the monotype table.
  if (apMonotypeCharData)
  {
    for (INDEX iCharData = 0; iCharData < 256; iCharData++)
    {
      fd_fcdFontCharData[iCharData].fcd_pixXOffset = apMonotypeCharData[iCharData].fcd_iCol * fd_pixCharWidth;
      fd_fcdFontCharData[iCharData].fcd_pixYOffset = apMonotypeCharData[iCharData].fcd_iRow * fd_pixCharHeight;
      fd_fcdFontCharData[iCharData].fcd_pixStart   = 0;
      fd_fcdFontCharData[iCharData].fcd_pixEnd     = fd_pixCharWidth;

      //if (iCharData > 31) {
      //  CPrintF("  FD: %3d ('%c') | %3d - %3d | %3d - %3d | CRX %d %d\n", iCharData, iCharData, fd_fcdFontCharData[iCharData].fcd_pixXOffset, fd_fcdFontCharData[iCharData].fcd_pixYOffset, fd_fcdFontCharData[iCharData].fcd_pixStart, fd_fcdFontCharData[iCharData].fcd_pixEnd,
      //      apMonotypeCharData[iCharData].fcd_iCol, apMonotypeCharData[iCharData].fcd_iRow);
      //}
    }

    fd_bMonotype = TRUE;

    delete apMonotypeCharData;
  }

  // Convert class instances.
  if (apVarCharData)
  {
    for (INDEX iCharData = 0; iCharData < 256; iCharData++)
    {
      fd_fcdFontCharData[iCharData].fcd_pixXOffset = apVarCharData[iCharData].fcd_iOffsetX;
      fd_fcdFontCharData[iCharData].fcd_pixYOffset = apVarCharData[iCharData].fcd_iOffsetY;
      fd_fcdFontCharData[iCharData].fcd_pixStart   = apVarCharData[iCharData].fcd_iStartX;
      fd_fcdFontCharData[iCharData].fcd_pixEnd     = apVarCharData[iCharData].fcd_iEndX;
    }

    delete apVarCharData;
  }
}

// --------------------------------------------------------------------------------------
// Write data to the stream.
// --------------------------------------------------------------------------------------
void CFontData::Write_EFND_t(CTStream *outFile) // throw char *
{
  outFile->WriteID_t(CChunkID("EFND"));
  (*outFile) << EFND_CURRENT_VERSION;
  
  outFile->WriteID_t(CChunkID("TFNM"));
  (*outFile) << fd_fnTexture;

  outFile->WriteID_t(CChunkID("FSGS"));
  (*outFile) << fd_pixCharWidth;
  (*outFile) << fd_pixCharHeight;
  (*outFile) << fd_pixCharSpacing;
  (*outFile) << fd_pixLineSpacing;
  
  if (fd_bMonotype) {
    outFile->WriteID_t(CChunkID("MCDT"));
    
    CFontMonotypeCharData cdMonotypeCharData;
    for (INDEX iCharData = 0; iCharData < 256; iCharData++)
    {
      cdMonotypeCharData.fcd_iCol = fd_fcdFontCharData[iCharData].fcd_pixXOffset / fd_pixCharWidth;
      cdMonotypeCharData.fcd_iRow = fd_fcdFontCharData[iCharData].fcd_pixYOffset / fd_pixCharHeight;
      
      cdMonotypeCharData.Write_t(outFile);
      
      //if (iCharData > 31) {
      //  printf("  FD: %3d ('%c') | %3d - %3d | %3d - %3d | CRX %d %d\n", iCharData, iCharData, fd_fcdFontCharData[iCharData].fcd_pixXOffset, fd_fcdFontCharData[iCharData].fcd_pixYOffset, fd_fcdFontCharData[iCharData].fcd_pixStart, fd_fcdFontCharData[iCharData].fcd_pixEnd,
      //      cdMonotypeCharData.fcd_iCol, cdMonotypeCharData.fcd_iRow);
      //}
    }
  } else {
    outFile->WriteID_t(CChunkID("VCDT"));

    CFontVarCharData cdVarCharData;

    for (INDEX iCharData = 0; iCharData < 256; iCharData++)
    {
      cdVarCharData.fcd_iOffsetX = fd_fcdFontCharData[iCharData].fcd_pixXOffset;
      cdVarCharData.fcd_iOffsetY = fd_fcdFontCharData[iCharData].fcd_pixYOffset;
      cdVarCharData.fcd_iStartX = fd_fcdFontCharData[iCharData].fcd_pixStart;
      cdVarCharData.fcd_iEndX = fd_fcdFontCharData[iCharData].fcd_pixEnd;
      
      cdVarCharData.Write_t(outFile);
    }
  }
}