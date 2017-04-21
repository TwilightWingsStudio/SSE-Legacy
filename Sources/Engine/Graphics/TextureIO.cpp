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

#include "StdH.h"

#include <Engine/Graphics/Texture.h>

#include <Engine/Base/Stream.h>
#include <Engine/Base/Timer.h>
#include <Engine/Base/Console.h>
#include <Engine/Math/Functions.h>
#include <Engine/Graphics/GfxLibrary.h>
#include <Engine/Graphics/ImageInfo.h>
#include <Engine/Graphics/TextureEffects.h>

#include <Engine/Templates/DynamicArray.h>
#include <Engine/Templates/DynamicArray.cpp>
#include <Engine/Templates/Stock_CtextureData.h>
#include <Engine/Templates/StaticArray.cpp>

#include <Engine/Base/Statistics_internal.h>

#define TEXTURE_VERSION_MINIMAL 3
#define TEXTURE_VERSION 4

extern BOOL _bExport;

extern void Convert( CTextureData *pTD);
extern ULONG GetEffectBufferSize( CTextureData *pTD);
extern void InitEffectBufferDimensions( CTextureData *pTD);
extern void FreeEffectBuffers( CTextureData *pTD);
extern ULONG AllocEffectBuffers( CTextureData *pTD);

extern INDEX tex_iNormalQuality;
extern INDEX tex_iAnimationQuality;
extern INDEX tex_iNormalSize;
extern INDEX tex_iAnimationSize;
extern INDEX tex_iEffectSize;

extern INDEX tex_iDithering;
extern INDEX tex_iFiltering;       

extern INDEX gap_bAllowSingleMipmap;
extern FLOAT gfx_tmProbeDecay;


// internal routines for texture::read routine

// test mipmap if it can be equilized
#define EQUALIZER_TRESHOLD 3
static BOOL IsEqualized( ULONG *pulMipmap, INDEX pixMipSize)
{
  // determine components and calc averages
  COLOR col;
  ULONG ulR=0, ulG=0, ulB=0;
  for( INDEX iPix=0; iPix<pixMipSize; iPix++) {
    col  = ByteSwap(pulMipmap[iPix]);
    ulR += (col&CT_RMASK)>>CT_RSHIFT;
    ulG += (col&CT_GMASK)>>CT_GSHIFT;
    ulB += (col&CT_BMASK)>>CT_BSHIFT;
  }
  ulR /= pixMipSize;
  ulG /= pixMipSize;
  ulB /= pixMipSize;
  const ULONG ulLoEdge = 127-EQUALIZER_TRESHOLD;
  const ULONG ulHiEdge = 128+EQUALIZER_TRESHOLD;
  BOOL bEqulized = FALSE;
  if( ulR>ulLoEdge && ulR<ulHiEdge &&
      ulG>ulLoEdge && ulG<ulHiEdge &&
      ulB>ulLoEdge && ulB<ulHiEdge) bEqulized = TRUE;
  return bEqulized;
}

// test mipmap if it can be transparent
#define TRANS_TRESHOLD 7
static BOOL IsTransparent( ULONG *pulMipmap, INDEX pixMipSize)
{
  COLOR col;
  ULONG ulA;
  // determine transparency
  for( INDEX iPix=0; iPix<pixMipSize; iPix++) {
    col = ByteSwap(pulMipmap[iPix]);
    ulA = (col&CT_AMASK)>>CT_ASHIFT;
    if( ulA>TRANS_TRESHOLD && ulA<(255-TRANS_TRESHOLD)) return FALSE;
  }
  // transparent!
  return TRUE;
}

// test mipmap whether it is grayscaled
static BOOL IsGray( ULONG *pulMipmap, INDEX pixMipSize)
{
  // loop thru texels
  for( INDEX iPix=0; iPix<pixMipSize; iPix++) {
    COLOR col = ByteSwap(pulMipmap[iPix]);
    if( !IsGray(col)) return FALSE; // colored
  } // grayscaled
  return TRUE;
}

// returns format in what texture will be uploaded (regarding console vars)
static ULONG DetermineInternalFormat( CTextureData *pTD)
{
  // cache some vars
  extern INDEX gap_bAllowGrayTextures;
  BOOL bGrayTexture  = gap_bAllowGrayTextures && (pTD->td_ulFlags&TEX_GRAY);
  BOOL bAlphaChannel = pTD->td_ulFlags & TEX_ALPHACHANNEL;
  PIX  pixTexSize    = pTD->GetPixWidth() * pTD->GetPixHeight();

  // choose internal texture format for alpha textures
  INDEX iQuality;
  ULONG ulInternalFormat;
  if( bAlphaChannel)
  {
    iQuality = pTD->td_ctFrames>1 ? TS.ts_iAnimQualityA : TS.ts_iNormQualityA;
    ulInternalFormat = TS.ts_tfRGBA4;
    switch( iQuality) {
    case 3:  case 2:  ulInternalFormat = TS.ts_tfRGBA8;  break;                       // uploaded as 32 bit or compressed
    case 1:  break;                                                                   // uploaded as 16 bit (default)
    case 0:  if( pTD->td_ulFlags&TEX_32BIT) ulInternalFormat = TS.ts_tfRGBA8;  break; // uploaded optimally
    default: ASSERTALWAYS( "Unexpected texture type found.");  break;
    }
    // adjust quality by size
    if( pixTexSize<=32*32 && ulInternalFormat==TS.ts_tfRGBA4) ulInternalFormat = TS.ts_tfRGBA8;
    // do eventual adjustment of internal format for grayscale textures
    if( bGrayTexture) ulInternalFormat = TS.ts_tfLA8;
    // handle case of forced internal format (for texture cration process only!)
    if( _iTexForcedQuality==16) ulInternalFormat = TS.ts_tfRGBA4;
    if( _iTexForcedQuality==32) ulInternalFormat = TS.ts_tfRGBA8;
    // do eventual adjustment of transparent textures
    if( (pTD->td_ulFlags&TEX_TRANSPARENT) && ulInternalFormat==TS.ts_tfRGBA4) ulInternalFormat = TS.ts_tfRGB5A1;
  }

  // choose internal texture format for opaque textures
  else
  {
    iQuality = pTD->td_ctFrames>1 ? TS.ts_iAnimQualityO : TS.ts_iNormQualityO;
    ulInternalFormat = TS.ts_tfRGB5;
    switch( iQuality) {
    case 3:  case 2:  ulInternalFormat = TS.ts_tfRGB8;  break;                        // uploaded as 32 bit or compressed
    case 1:  break;                                                                   // uploaded as 16 bit (default)
    case 0:  if( pTD->td_ulFlags&TEX_32BIT) ulInternalFormat = TS.ts_tfRGB8;  break;  // uploaded optimally
    default: ASSERTALWAYS( "Unexpected texture type found.");  break;
    }
    // adjust quality by size
    if( pixTexSize<=32*32 && ulInternalFormat==TS.ts_tfRGB5) ulInternalFormat = TS.ts_tfRGB8;
    // do eventual adjustment of internal format for grayscale textures
    if( bGrayTexture) ulInternalFormat = TS.ts_tfL8;
    // handle case of forced internal format (for texture cration process only!)
    if( _iTexForcedQuality==16) ulInternalFormat = TS.ts_tfRGB5;
    if( _iTexForcedQuality==32) ulInternalFormat = TS.ts_tfRGB8;
  }

  // adjust format to compressed if needed and allowed
  if( iQuality==3 && pixTexSize>=64*64) {
    if( ulInternalFormat==TS.ts_tfRGB8
     || ulInternalFormat==TS.ts_tfRGB5
     || ulInternalFormat==TS.ts_tfRGB5A1) ulInternalFormat = TS.ts_tfCRGB;
    if( ulInternalFormat==TS.ts_tfRGBA8
     || ulInternalFormat==TS.ts_tfRGBA4)  ulInternalFormat = TS.ts_tfCRGBA;
  }
  // all done
  return ulInternalFormat;
}

// remove mipmaps from texture that are not needed (exceeds maximum supported dimension)
static void RemoveOversizedMipmaps( CTextureData *pTD)
{
  // if this is an effect texture, leave as it is
  if( pTD->td_ptegEffect != NULL) return;
  pTD->td_ulFlags &= ~TEX_DISPOSED;

  // determine and clamp to max allowed texture dimension and size
  PIX pixClampAreaSize = (pTD->td_ctFrames>1) ? TS.ts_pixAnimSize : TS.ts_pixNormSize;
  // constant textures doesn't need clamping to area, but still must be clamped to max HW dimension!
  if( pTD->td_ulFlags & TEX_CONSTANT) pixClampAreaSize = 4096*4096; 

  // determine dimensions of finest mip-map
  PIX pixSizeU = pTD->GetPixWidth();
  PIX pixSizeV = pTD->GetPixHeight();
  // determine number of mip-maps to skip
  INDEX ctSkipMips = ClampTextureSize( pixClampAreaSize, _pGfx->gl_pixMaxTextureDimension,
                                       pixSizeU, pixSizeV);
  // return if no need to remove mip-maps
  if( ctSkipMips==0) return;
  // check for mip overhead
  INDEX ctMips = GetNoOfMipmaps( pixSizeU, pixSizeV);
  while( ctMips<=ctSkipMips) ctSkipMips--;

  // determine memory size and allocate memory for rest mip-maps
  SLONG slRemovedMipsSize = GetMipmapOffset( ctSkipMips, pixSizeU, pixSizeV) *BYTES_PER_TEXEL;
  SLONG slNewFrameSize    = pTD->td_slFrameSize-slRemovedMipsSize;
  ULONG *pulNewFrames = (ULONG*)AllocMemory( slNewFrameSize * pTD->td_ctFrames);
  ULONG *pulNewFrame  = pulNewFrames;
  ULONG *pulOldFrame  = pTD->td_pulFrames + (slRemovedMipsSize/BYTES_PER_TEXEL);

  // copy only needed mip-maps from each frame
  for( INDEX iFr=0; iFr<pTD->td_ctFrames; iFr++) {
    memcpy( pulNewFrame, pulOldFrame, slNewFrameSize);
    pulNewFrame += slNewFrameSize/BYTES_PER_TEXEL;
    pulOldFrame += pTD->td_slFrameSize/BYTES_PER_TEXEL;
  }

  // free old frames memory
  FreeMemory( pTD->td_pulFrames);
  // adjust texture parameters
  pTD->td_pulFrames       = pulNewFrames;
  pTD->td_slFrameSize     = slNewFrameSize;
  pTD->td_iFirstMipLevel += ctSkipMips;
  pTD->td_ctFineMipLevels = ClampDn( (INDEX)(pTD->td_ctFineMipLevels-ctSkipMips), (INDEX)1);

  // mark that this texture had some mip maps disposed
  pTD->td_ulFlags |= TEX_DISPOSED;
}

// reads 32/24-bit texture from file and eventually converts it to 8-bit pixel format
void CTextureData::Read_t( CTStream *inFile)
{
  //ASSERT( inFile->GetDescription() != "Textures\\Test\\BetterQuality\\FloorWS08.tex");

  // reset texture (blank all except some flags)
  Clear();

  // determine API
  const GfxAPIType eAPI = _pGfx->gl_eCurrentAPI;
#ifdef SE1_D3D
  ASSERT( eAPI==GAT_OGL || eAPI==GAT_D3D || eAPI==GAT_NONE);
#else // SE1_D3D
  ASSERT( eAPI==GAT_OGL || eAPI==GAT_NONE);
#endif // SE1_D3D

  // determine driver context presence (must have at least 1 texture unit!)
  const BOOL bHasContext = (_pGfx->gl_ctRealTextureUnits>0);
  
  CChunkID cidPeek = inFile->PeekID_t();
  
  // [SSE] Better Error Messages
  if (cidPeek == CChunkID("SIGS") || cidPeek == CChunkID("WRKS") || cidPeek == CChunkID("CTSE")) {
    CTFileName fnTex = inFile->GetDescription();
    ThrowF_t(TRANS("File: '%s'\nThis file was created by one of new engines!\nSo it cannot be loaded!"), (CTString&)fnTex);
    return;
  }

  // read version
  INDEX iVersion;
  inFile->ExpectID_t("TVER");
  *inFile >> iVersion;

  // [SSE] Better Error Messages
  if (iVersion < TEXTURE_VERSION_MINIMAL) {
    CTFileName fnTex = inFile->GetDescription();
    ThrowF_t(TRANS("File: '%s'\nTexture format version is too old!\nMinimal supported: %d\n\nSo texture has version %d and cannot be loaded!"), (CTString&)fnTex, TEXTURE_VERSION_MINIMAL, iVersion);
    return;
  }
  
  // [SSE] Better Error Messages
  if (iVersion > TEXTURE_VERSION) {
    CTFileName fnTex = inFile->GetDescription();
    ThrowF_t(TRANS("File: '%s'\nTexture format version is too new!\nMaximum supported: %d\n\nSo texture has version %d and cannot be loaded!"), (CTString&)fnTex, TEXTURE_VERSION, iVersion);
    return;
  }
  
  // mark if this texture was loaded form the old format
  if( iVersion==3) td_ulFlags |= TEX_WASOLD;
  BOOL bResetEffectBuffers = FALSE;
  BOOL bFramesLoaded = FALSE;
  BOOL bCompressed = FALSE; // [SSE] Compressed Textures
	SLONG slCompressedFrameSize = 0; // [SSE] Compressed Textures
  BOOL bAlphaChannel = FALSE;

  // loop trough file and react according to chunk ID
  do
  {
    // obtain chunk id
    CChunkID idChunk = inFile->GetID_t();
    if( idChunk == CChunkID("    ")) {
      // we should stop reading when an invalid chunk has been encountered
      break;
    }

    // if this is chunk containing texture data
    if( idChunk == CChunkID("TDAT"))
    {
      // read data describing texture
      ULONG ulFlags=0;
      INDEX ctMipLevels;
      *inFile >> ulFlags;
      *inFile >> td_mexWidth;
      *inFile >> td_mexHeight;
      *inFile >> td_ctFineMipLevels;
      if( iVersion!=4) *inFile >> ctMipLevels;
      *inFile >> td_iFirstMipLevel;
      if( iVersion!=4) *inFile >> td_slFrameSize;
      *inFile >> td_ctFrames;

      td_ulFlags |= ulFlags; // combine flags

      bAlphaChannel = td_ulFlags&TEX_ALPHACHANNEL;
      bCompressed = td_ulFlags&TEX_COMPRESSED; // [SSE] Compressed Textures
      
      if (bCompressed) {
        CTFileName fnTex = inFile->GetDescription();
        ThrowF_t(TRANS("File: '%s'\n\nTexture contains compressed data!\nSo texture cannot be loaded because compressed textures currently unsupported!"), (CTString&)fnTex);
        return;
      }

      // determine frame size
      if (iVersion == 4) td_slFrameSize = GetMipmapOffset( 15, GetPixWidth(), GetPixHeight()) * BYTES_PER_TEXEL;

    }
    // if this is chunk containing raw frames
    else if( idChunk == CChunkID("FRMS")) 
    {
      if (bCompressed) {
        CTFileName fnTex = inFile->GetDescription();
        ThrowF_t(TRANS("File: '%s'\n\nError while loading texture!\nTexture contains uncompressed data while it is marked in flags as compressed!"), (CTString&)fnTex);
        return;
      }
      
      // if no driver is present and texture is not static
      if( !(bHasContext || td_ulFlags&TEX_STATIC))
      { // determine frames' size
        SLONG slSkipSize = td_slFrameSize;
        if( iVersion==4) {
          slSkipSize = GetPixWidth()*GetPixHeight();
          if( bAlphaChannel) slSkipSize *=4;
          else slSkipSize *=3;
        } 
        // just seek over frames (skip it)
        inFile->Seek_t( slSkipSize*td_ctFrames, CTStream::SD_CUR);
        continue;
      }

      // calculate texture size for corresponding texture format and allocate memory
      SLONG slTexSize = td_slFrameSize * td_ctFrames;
      td_pulFrames = (ULONG*)AllocMemory( slTexSize);

      // if older version
      if( iVersion==3) {
        // alloc memory block and read mip-maps
        inFile->Read_t( td_pulFrames, slTexSize);
      } 
      // if current version
      else {
        PIX pixFrameSizeOnDisk = GetPixWidth()*GetPixHeight();
        for( INDEX iFr=0; iFr<td_ctFrames; iFr++)
        { // loop thru frames
          ULONG *pulCurrentFrame = td_pulFrames + (iFr * td_slFrameSize/BYTES_PER_TEXEL);
          if( bAlphaChannel) {
            // read texture with alpha channel from file
            inFile->Read_t( pulCurrentFrame, pixFrameSizeOnDisk *4);
          } else {
            // read texture without alpha channel from file
            inFile->Read_t( pulCurrentFrame, pixFrameSizeOnDisk *3);
            // add opaque alpha channel
            AddAlphaChannel( (UBYTE*)pulCurrentFrame, pulCurrentFrame, pixFrameSizeOnDisk);
          }
        }
      }
      bFramesLoaded = TRUE;
    }
    // If this chunk containing  compressed frames.
    else if (idChunk == CChunkID("FRMC"))
    {
      // If we have uncompressed texture then it should NOT have compressed data!
      if (!bCompressed) {
        CTFileName fnTex = inFile->GetDescription();
        ThrowF_t(TRANS("File: '%s'\n\nError while loading texture!\nTexture contains compressed data while it is NOT marked in flags as compressed!"), (CTString&)fnTex);
        return;
      }
      
      const SLONG slCompressedDataSize = slCompressedFrameSize * td_ctFrames;
      
			// If no driver is present then just skip the frames.
			if (!bHasContext) {
				inFile->Seek_t( slCompressedDataSize, CTStream::SD_CUR);

			} else {
        // TODO: [ZCaliptium] Write code later.
      }
    }
    // if this is chunk containing texture animation data
    else if( idChunk == CChunkID("ANIM"))
    {
      // read corresponding animation(s)
      CAnimData::Read_t( inFile);
    }
    // if this is chunk containing base texture name
    else if( idChunk == CChunkID("BAST"))
    {
      CTFileName fnBaseTexture;
      // read file name of base texture
      *inFile >> fnBaseTexture;
      // if there is base texture, obtain it
      if( fnBaseTexture != "") {
        // must not be the same as base texture
        CTFileName fnTex = inFile->GetDescription();
        if( fnTex == fnBaseTexture) {
          // generate exception
          ThrowF_t( TRANS("Texture \"%s\" has same name as its base texture."), (CTString&)fnTex);
        } else {
          // obtain base texture
          td_ptdBaseTexture = _pTextureStock->Obtain_t( fnBaseTexture);
        }
      }
      // force base to be static by default
      td_ptdBaseTexture->Force(TEX_STATIC);
    }
    // if this is chunk containing saved effect buffers
    else if( idChunk == CChunkID("FXBF")) 
    { // skip chunk in old versions
      bResetEffectBuffers = TRUE;
      if( iVersion!=4) {
        inFile->Seek_t( 2* GetPixWidth()*GetPixHeight() *sizeof(SWORD), CTStream::SD_CUR);
      } else {
        ASSERT( td_pixBufferWidth>0 && td_pixBufferHeight>0);
        ULONG ulSize = AllocEffectBuffers(this);
        if( td_ptegEffect->IsWater()) ulSize*=2;
        inFile->Seek_t( 2*ulSize, CTStream::SD_CUR);
      }
    }
    else if( idChunk == CChunkID("FXB2")) 
    { // read effect buffers
      ASSERT( td_pixBufferWidth>0 && td_pixBufferHeight>0);
      ULONG ulSize = AllocEffectBuffers(this);
      inFile->Read_t( td_pubBuffer1, ulSize);
      inFile->Read_t( td_pubBuffer2, ulSize);
    }
    // if this is chunk containing effect data
    else if( idChunk == CChunkID("FXDT"))
    { // read effect class
      ULONG ulGlobalEffect;
      *inFile >> ulGlobalEffect;
      // read effect buffer dimensions
      if( iVersion==4) *inFile >> td_pixBufferWidth;
      if( iVersion==4) *inFile >> td_pixBufferHeight;
      // allocate memory for texture effect struct
      td_ptegEffect = new CTextureEffectGlobal(this, ulGlobalEffect);
      // skip global properties for old format effect texture
      if( iVersion!=4) inFile->Seek_t( 64*sizeof(char), CTStream::SD_CUR);
      // read count of effect sources
      INDEX ctEffectSources;
      *inFile >> ctEffectSources;
      // add requested number of members to effect source array
      CTextureEffectSource *pEffectSources = td_ptegEffect->teg_atesEffectSources.New( ctEffectSources);

      // read whole dynamic array of effect sources
      FOREACHINDYNAMICARRAY( td_ptegEffect->teg_atesEffectSources, CTextureEffectSource, itEffectSource)
      {
        // read type of effect source
        *inFile >> (ULONG) itEffectSource->tes_ulEffectSourceType;
        // read structure holding effect source properties
        inFile->Read_t( &itEffectSource->tes_tespEffectSourceProperties, sizeof(struct TextureEffectSourceProperties));
        // remember pointer to global effect
        itEffectSource->tes_ptegGlobalEffect = td_ptegEffect;
        // read count of effect pixels
        INDEX ctEffectSourcePixels;
        *inFile >> ctEffectSourcePixels;
        // if there are any effect pixels
        if (ctEffectSourcePixels>0) {
          // alocate needed ammount of members
          itEffectSource->tes_atepPixels.New( ctEffectSourcePixels);
          // read all effect pixels in one block
          inFile->Read_t( &itEffectSource->tes_atepPixels[0], sizeof(struct TextureEffectPixel)*ctEffectSourcePixels);
        }
      }
      // allocate memory for effect frame buffer
      SLONG slFrameSize = GetMipmapOffset( 15, GetPixWidth(), GetPixHeight()) *BYTES_PER_TEXEL;
      td_pulFrames = (ULONG*)AllocMemory( slFrameSize);
      // remember once again new frame size just for the sake of old effect textures
      td_slFrameSize = slFrameSize;
      // mark that effect texture needs to be static
      td_ulFlags |= TEX_STATIC;
    }
    // if this is chunk containing data about detail texture
    else if( idChunk == CChunkID("DTLT"))
    { // skip chunk (this is here only for compatibility reasons)
      CTFileName fnTmp;
      *inFile >> fnTmp;
    }
    else
    {
      ThrowF_t( TRANS("Unrecognisable chunk ID (\"%s\") found while reading texture \"%s\"."),
                (char*)idChunk, (CTString&)inFile->GetDescription() );
    }
  }
  // until we didn't reach end of file
  while( !inFile->AtEOF());

  // reset effect buffers if needed
  if( bResetEffectBuffers) {
    InitEffectBufferDimensions(this);
    AllocEffectBuffers(this);
  }

  // were done if frames weren't loaded or effect texture has been read
  if( !bFramesLoaded || td_ptegEffect!=NULL) return;

  // if texture is in old format, convert it to current format
  if( iVersion==3) Convert(this);
  PIX pixWidth  = GetPixWidth();
  PIX pixHeight = GetPixHeight();
  PIX pixTexSize = pixWidth*pixHeight;
  PIX pixFrameSize = td_slFrameSize>>2; // /BYTES_PER_TEXEL;
  INDEX iFrame;

  // test first mipmap for transparency (i.e. is one bit of alpha channel enough?)
  // (must test it before filtering and/or mipmap reduction gets to this texture)
  if( bAlphaChannel) {
    td_ulFlags |= TEX_TRANSPARENT; 
    for( iFrame=0; iFrame<td_ctFrames; iFrame++) {
      ULONG *pulCurrentFrame = td_pulFrames + iFrame*pixFrameSize;
      if( !IsTransparent( pulCurrentFrame, pixTexSize)) {
        // no need to test other frames if one found that isn't gray
        td_ulFlags &= ~TEX_TRANSPARENT; 
        break; 
      }
    }
  }
  // generate texture mip-maps for each frame (in version 4 they're no longer kept in file)
  // and eventually adjust texture saturation, do filtering and/or dithering
  tex_iFiltering = Clamp( tex_iFiltering, -6L, +6L);
  INDEX iTexFilter = tex_iFiltering;
  if( _bExport || (td_ulFlags&TEX_CONSTANT)) iTexFilter = 0; // don't filter constants and textures for exporting
  if( iTexFilter) td_ulFlags |= TEX_FILTERED;

  // eventually saturate texture
  if( !_bExport && !(td_ulFlags&TEX_KEEPCOLOR) && (_slTexSaturation!=256 || _slTexHueShift!=0)) {
    td_ulFlags |= TEX_SATURATED;
    for( iFrame=0; iFrame<td_ctFrames; iFrame++) {
      ULONG *pulCurrentFrame = td_pulFrames + iFrame*pixFrameSize;
      AdjustBitmapColor( pulCurrentFrame, pulCurrentFrame, pixWidth, pixHeight, _slTexHueShift, _slTexSaturation);
    }
  }
  // make mipmaps
  for( iFrame=0; iFrame<td_ctFrames; iFrame++) { 
    ULONG *pulCurrentFrame = td_pulFrames + iFrame*pixFrameSize;
    MakeMipmaps( td_ctFineMipLevels, pulCurrentFrame, pixWidth,pixHeight, iTexFilter);
  }

  // remove mipmaps from texture that are not needed and update texture size
  if( !_bExport) RemoveOversizedMipmaps(this);

  // do some additional mipmap adjustments if needed
  pixWidth  = GetPixWidth();
  pixHeight = GetPixHeight();
  pixTexSize = pixWidth*pixHeight;
  pixFrameSize = td_slFrameSize>>2; // /BYTES_PER_TEXEL;

  // eventually colorize mipmaps
  extern INDEX tex_bColorizeMipmaps;
  if( !_bExport && tex_bColorizeMipmaps && !(td_ulFlags&TEX_CONSTANT)) {
    td_ulFlags |= TEX_COLORIZED;
    for( iFrame=0; iFrame<td_ctFrames; iFrame++) {
      ULONG *pulCurrentFrame = td_pulFrames + iFrame*pixFrameSize;
      ColorizeMipmaps( 1, pulCurrentFrame, pixWidth, pixHeight);
    }
  } // if not colorized, test if texture is gray
  else {
    td_ulFlags |= TEX_GRAY; 
    for( iFrame=0; iFrame<td_ctFrames; iFrame++) {
      ULONG *pulCurrentFrame = td_pulFrames + iFrame*pixFrameSize;
      if( !IsGray( pulCurrentFrame, pixTexSize)) {
        // no need to test other frames if one found that isn't gray
        td_ulFlags &= ~TEX_GRAY; 
        break; 
      }
    }
  } // test texture for equality (i.e. determine if texture could be discardable in shade mode when in lowest mipmap)
  if( td_ctFrames<2 && (!gap_bAllowSingleMipmap || td_ctFineMipLevels>1))
  { // get last mipmap pointer
    INDEX ctLastPixels   = Max(pixWidth,pixHeight) / Min(pixWidth,pixHeight);
    ULONG *pulLastMipMap = td_pulFrames + td_slFrameSize/BYTES_PER_TEXEL - ctLastPixels;
    if( IsEqualized( pulLastMipMap, ctLastPixels)) td_ulFlags |= TEX_EQUALIZED;
  }

  // prepare dithering type
  td_ulInternalFormat = DetermineInternalFormat(this);
  tex_iDithering = Clamp( tex_iDithering, 0L, 10L); 
  INDEX iDitherType = 0;
  if( !(td_ulFlags&TEX_STATIC) || !(td_ulFlags&TEX_CONSTANT)) { // only non-static-constant textures can be dithered
    extern INDEX AdjustDitheringType_OGL(    GLenum eFormat, INDEX iDitheringType);
    if( eAPI==GAT_OGL) iDitherType = AdjustDitheringType_OGL(    (GLenum)td_ulInternalFormat, tex_iDithering);
#ifdef SE1_D3D
    extern INDEX AdjustDitheringType_D3D( D3DFORMAT eFormat, INDEX iDitheringType);
    if( eAPI==GAT_D3D) iDitherType = AdjustDitheringType_D3D( (D3DFORMAT)td_ulInternalFormat, tex_iDithering);
#endif // SE1_D3D
  }
  // eventually dither texture
  if( !_bExport && iDitherType!=0) {
    td_ulFlags |= TEX_DITHERED;
    for( iFrame=0; iFrame<td_ctFrames; iFrame++) {
      ULONG *pulCurrentFrame = td_pulFrames + iFrame*pixFrameSize;
      DitherMipmaps( iDitherType, pulCurrentFrame, pulCurrentFrame, pixWidth, pixHeight);
    }
  }
  // upload texture if not static and API is active
  // (or, in the other hand, better not - this could cause reloading due to force() after obtain())
  if( !_bExport && bHasContext && !(td_ulFlags&TEX_STATIC)) SetAsCurrent();
}

// writes texutre to file
void CTextureData::Write_t( CTStream *outFile)   // throw char *
{
  // cannot write textures that have been mangled somehow
  _bExport = FALSE;
  if( td_ptegEffect==NULL && IsModified()) throw( TRANS("Cannot write texture that has modified frames."));

  // must not have base texture with same name
  if( td_ptdBaseTexture != NULL) {
    CTFileName fnTex = outFile->GetDescription();
    if( fnTex == td_ptdBaseTexture->GetName()) {
      ThrowF_t( TRANS("Texture \"%s\" has same name as its base texture."), (CTString&)fnTex);
    }
  }

  // write version
  INDEX iVersion = 4;
  outFile->WriteID_t("TVER");
  *outFile << iVersion;

  // isolate required flags
  ULONG ulFlags = td_ulFlags & (TEX_ALPHACHANNEL|TEX_32BIT);
  BOOL bAlphaChannel = td_ulFlags&TEX_ALPHACHANNEL;

  // write chunk containing texture data
  outFile->WriteID_t( CChunkID("TDAT"));
  // write data describing texture
  *outFile << ulFlags;
  *outFile << td_mexWidth;
  *outFile << td_mexHeight;
  *outFile << td_ctFineMipLevels;
  *outFile << td_iFirstMipLevel;
  *outFile << td_ctFrames;

  // if global effect struct exists in texture, don't save frames
  if( td_ptegEffect==NULL)
  { // write chunk containing raw frames
    ASSERT( td_ctFrames>0);
    ASSERT( td_pulFrames!=NULL);
    outFile->WriteID_t( CChunkID("FRMS"));
    PIX pixFrSize = GetPixWidth()*GetPixHeight();
    // eventually prepare temp buffer in case of frames without alpha channel
    UBYTE *pubTmp = NULL;
    if( !bAlphaChannel) pubTmp = (UBYTE*)AllocMemory( pixFrSize*3);
    // write frames without mip-maps (just write the largest one)
    for( INDEX iFr=0; iFr<td_ctFrames; iFr++ )
    { // determine write params
      ULONG *pulCurrentFrame = td_pulFrames + (iFr * td_slFrameSize/BYTES_PER_TEXEL);
      if( bAlphaChannel) { // write frame with alpha channel
        outFile->Write_t( pulCurrentFrame, pixFrSize *4);
      } else { // write frame without alpha channel
        RemoveAlphaChannel( pulCurrentFrame, pubTmp, pixFrSize);
        outFile->Write_t( pubTmp, pixFrSize *3);
      }
    }
    // no need for temp buffer anymore
    if( pubTmp!=NULL) FreeMemory(pubTmp);
  }
  // if exists global effect struct in texture
  else 
  { // write chunk containing effect data
    outFile->WriteID_t( CChunkID("FXDT"));
    // write effect class
    *outFile << td_ptegEffect->teg_ulEffectType;
    // write effect buffer dimensions
    *outFile << td_pixBufferWidth;
    *outFile << td_pixBufferHeight;
    // write count of effect sources
    *outFile << td_ptegEffect->teg_atesEffectSources.Count();

    // write whole dynamic array of effect sources
    FOREACHINDYNAMICARRAY(td_ptegEffect->teg_atesEffectSources, CTextureEffectSource, itEffectSource)
    { // write type of effect source
      *outFile << itEffectSource->tes_ulEffectSourceType;
      // write structure holding effect source properties
      outFile->Write_t( &itEffectSource->tes_tespEffectSourceProperties, sizeof( struct TextureEffectSourceProperties));
      INDEX ctEffectSourcePixels = itEffectSource->tes_atepPixels.Count();
      // write count of effect pixels
      *outFile << ctEffectSourcePixels;
      // if there are any effect pixels
      if( ctEffectSourcePixels>0) {
        // write all effect pixels in one block
        outFile->Write_t( &itEffectSource->tes_atepPixels[0], sizeof(struct TextureEffectPixel)*ctEffectSourcePixels);
      }
    }
    // if effect buffers are valid
    if( td_pubBuffer1!=NULL && td_pubBuffer2!=NULL)
    { // write chunk containing effect buffers
      outFile->WriteID_t( CChunkID("FXB2"));
      ULONG ulSize = GetEffectBufferSize(this);
      // write effect buffers
      outFile->Write_t( td_pubBuffer1, ulSize);
      outFile->Write_t( td_pubBuffer2, ulSize);
    }
  }
  // write chunk containing texture animation data
  outFile->WriteID_t( CChunkID("ANIM"));
  // write corresponding animation(s)
  CAnimData::Write_t( outFile);

  // if this texture has base texture
  if( td_ptdBaseTexture != NULL)
  { // write chunk containing base texture file name
    outFile->WriteID_t( CChunkID("BAST"));
    // write file name of base texture
    *outFile << td_ptdBaseTexture->GetName();
  }
}

// export finest mipmap of one texture's frame to imageinfo
void CTextureData::Export_t( class CImageInfo &iiExportedImage, INDEX iFrame)
{
  // check for right frame number and non-effect texture type
  ASSERT( iFrame<td_ctFrames && td_ptegEffect==NULL);
  if( iFrame>=td_ctFrames) throw( TRANS("Texture frame that is to be exported doesn't exist."));

  // reload without modifications
  _bExport = TRUE;
  Reload();
  ASSERT( td_pulFrames!=NULL);

  // prepare miplevel and mipmap offset
  PIX pixWidth  = GetPixWidth();
  PIX pixHeight = GetPixHeight();
  // export header to image info structure
  iiExportedImage.Clear();
  iiExportedImage.ii_Width  = pixWidth;
  iiExportedImage.ii_Height = pixHeight;
  iiExportedImage.ii_BitsPerPixel = (td_ulFlags&TEX_ALPHACHANNEL) ? 32 : 24;

  // prepare the texture for exporting (with or without alpha channel)
  ULONG *pulFrame = td_pulFrames + td_slFrameSize*iFrame/BYTES_PER_TEXEL;
  PIX  pixMipSize = pixWidth*pixHeight;
  SLONG slMipSize = pixMipSize * iiExportedImage.ii_BitsPerPixel/8;
  iiExportedImage.ii_Picture = (UBYTE*)AllocMemory( slMipSize);
  // export frame
  if( td_ulFlags&TEX_ALPHACHANNEL) {
    memcpy( iiExportedImage.ii_Picture, pulFrame, slMipSize);
  } else {
    RemoveAlphaChannel( pulFrame, iiExportedImage.ii_Picture, pixMipSize);
  }

  // reload as it was
  _bExport = FALSE;
  Reload();
}
