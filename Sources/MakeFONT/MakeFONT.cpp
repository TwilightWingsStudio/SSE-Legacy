/* Copyright (c) 2018 by ZCaliptium
Copyright (c) 2002-2012 Croteam Ltd.

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


// MakeFONT - Font table File Creator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <Engine/Engine.h>

void PrintHelp()
{
  printf("USAGE: MakeFont <texture_file> <char_width> <char_height>\n");
  printf("<char_order_file> [-A] [-M]\n");
  printf("\n");
  printf("texture_file: FULL PATH to texture file that represents font\n");
  printf("char_width: maximum width (in pixels) of single character\n");
  printf("char_height: maximum height (in pixels) of single character\n");
  printf("char_order_file: full path to ASCII file that shows\n");
  printf("                 graphical order of character in font texture\n");
  printf("-A: do not include alpha channel when determining character width \n");
  printf("-M: font will be created as monotype font \n");
  printf("\n");
  printf("NOTES: - out file will have the name as texture file, but \".fnt\" extension\n");
  printf("       - texture file must begin with character that will be a replacement for\n");
  printf("         each character that hasn't got definition in this texture file\n");
}

// --------------------------------------------------------------------------------------
// Business logic here.
// --------------------------------------------------------------------------------------
void SubMain( int argc, char *argv[])
{
  printf("\nMakeFONT - Font Tables Maker (2.52)\n");
  printf("             (C) 1999 CROTEAM Ltd\n");
  printf("             (C) 2018 ZCaliptium\n\n");

  // 5 to 7 parameters are allowed as input
  if((argc < 5) || (argc > 7))
  {
    PrintHelp();
    exit(EXIT_FAILURE);
  }

  // Initialize engine.
  SE_InitEngine("");

  // First input parameter is texture name.
  CTFileName fnTextureFileName = CTString(argv[1]);

  // Parameters 2 and 3 give us character dimensions.
  ULONG ulCharWidth = strtoul( argv[2], NULL, 10);
  ULONG ulCharHeight= strtoul( argv[3], NULL, 10);

  // Parameter 4 specifies text file for character arrangements.
  CTFileName fnOrderFile = CTString(argv[4]);

  BOOL bUseAlpha = TRUE;
  BOOL bMonotype = FALSE; // [SSE]

  if (argc > 5)
  {
    for (int k = 5; k <= argc; ++k)
    {
      if (!argv[k] || !*argv[k])
        break;

      switch (argv[k][1])
      {
        case 'a':
        case 'A': {
          bUseAlpha = FALSE;
        } break;
        
        case 'm':
        case 'M': {
          bMonotype = TRUE;
        } break;
        
        default: {
          printf("Unknown option %s.\n", argv[k]);
          exit(EXIT_FAILURE);
        }
      }
    }
  }
  
  printf(" - Alpha usage %s!\n", bUseAlpha ? "ON" : "OFF");
  if (bMonotype) {
    printf(" - Monotype mode ON!\n");
  }

  // font generation starts
  printf(" - Generating font table.\n");
  // try to create font
  CFontData fdFontData;

  try
  { 
    // Remove application path from font texture file.
    fnTextureFileName.RemoveApplicationPath_t();
    // Create font.
    fdFontData.Make_t( fnTextureFileName, ulCharWidth, ulCharHeight, fnOrderFile, bUseAlpha, bMonotype);
  }
  // Catch and report errors.
  catch(char *strError)
  {
    printf("! Cannot create font.\n  %s\n", strError);
    exit(EXIT_FAILURE);
  }

  // Save processed data.
  printf(" - Saving font table file.\n");

  // Create font name.
  CTFileName strFontFileName;
  strFontFileName = fnTextureFileName.FileDir() + fnTextureFileName.FileName() + ".fnt";

  // Try to.
  try
  {
    fdFontData.Save_t( strFontFileName);
  }
  catch(char *strError)
  {
    printf("! Cannot save font.\n  %s\n", strError);
    exit(EXIT_FAILURE);
  }

  printf(" - '%s' created successfuly.\n", strFontFileName);
  
  exit( EXIT_SUCCESS);
}

// --------------------------------------------------------------------------------------
// The entry point.
// --------------------------------------------------------------------------------------
int main( int argc, char *argv[])
{
  CTSTREAM_BEGIN
  {
    SubMain(argc, argv);
  }

  CTSTREAM_END;
  getch();

  return 0;
}
