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
#include "GameMP/SEColors.h"

#include "HUD.h"

#include <Engine/Graphics/DrawPort.h>

// --------------------------------------------------------------------------------------
// Draws filled rectangle on anchor-based relative coords.
// --------------------------------------------------------------------------------------
extern void HUD_DrawAnchoredRectEx(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, COLOR colRect)
{
  FLOAT fOriginX = fPosX;
  FLOAT fOriginY = fPosY;
  
  FLOAT fModSizeX = fSizeX;
  FLOAT fModSizeY = fSizeY;
  
  switch (ehPos)
  {
    default: {
      fOriginX = fOriginX;
    } break;

    case EHHAT_CENTER: {
      fOriginX = _pixDPWidth / 2.0F + fOriginX - fSizeX / 2.0F;
    } break;

    case EHHAT_RIGHT: {
      fOriginX = _pixDPWidth - fOriginX - fSizeX;
    } break;
  }

  switch (evPos)
  {
    default: {
      fOriginY = fOriginY;
    } break;

    case EHVAT_MID: {
      fOriginY = _pixDPHeight / 2.0F + fOriginY - fSizeY;
    } break;

    case EHVAT_BOT: {
      fOriginY = _pixDPHeight - fOriginY - fSizeY;
    } break;
  }
  
  _pDP->Fill(fOriginX, fOriginY, fModSizeX, fModSizeY, colRect);
}

// --------------------------------------------------------------------------------------
// Draws filled rectangle on anchor-based relative coords.
// Uses relative rescalling.
// --------------------------------------------------------------------------------------
extern void HUD_DrawAnchoredRect(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, COLOR colRect)
{
  FLOAT fMul;
  
  if (_pixDPWidth > _pixDPHeight) {
    fMul = _pixDPHeight / 480.0F;
  } else {
    fMul = _pixDPWidth / 640.0F;
  }

  fPosX *= fMul;
  fPosY *= fMul;
  fSizeX *= fMul;
  fSizeY *= fMul;
  
  HUD_DrawAnchoredRectEx(fPosX, fPosY, fSizeX, fSizeY, ehPos, evPos, colRect);
}

// --------------------------------------------------------------------------------------
// Draws 1 pix outline around rectangle with given color on anchor-based relative coords.
// --------------------------------------------------------------------------------------
extern void HUD_DrawAnchoredRectOutlineEx(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, COLOR colRect)
{
  FLOAT fOriginX = fPosX;
  FLOAT fOriginY = fPosY;
  
  FLOAT fModSizeX = fSizeX;
  FLOAT fModSizeY = fSizeY;
  
  switch (ehPos)
  {
    default: {
      fOriginX = fOriginX;
    } break;

    case EHHAT_CENTER: {
      fOriginX = _pixDPWidth / 2.0F + fOriginX - fSizeX / 2.0F;
    } break;

    case EHHAT_RIGHT: {
      fOriginX = _pixDPWidth - fOriginX - fSizeX;
    } break;
  }

  switch (evPos)
  {
    default: {
      fOriginY = fOriginY;
    } break;

    case EHVAT_MID: {
      fOriginY = _pixDPHeight / 2.0F + fOriginY - fSizeY;
    } break;

    case EHVAT_BOT: {
      fOriginY = _pixDPHeight - fOriginY - fSizeY;
    } break;
  }
  
  _pDP->DrawBorder(fOriginX - 1, fOriginY - 1, fModSizeX + 2, fModSizeY + 2, colRect);
}

// --------------------------------------------------------------------------------------
// Draws 1 pix outline around rectangle with given color on anchor-based relative coords.
// Uses relative rescalling.
// --------------------------------------------------------------------------------------
extern void HUD_DrawAnchoredRectOutline(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, COLOR colRect)
{
  FLOAT fMul;
  
  if (_pixDPWidth > _pixDPHeight) {
    fMul = _pixDPHeight / 480.0F;
  } else {
    fMul = _pixDPWidth / 640.0F;
  }

  fPosX *= fMul;
  fPosY *= fMul;
  fSizeX *= fMul;
  fSizeY *= fMul;
  
  HUD_DrawAnchoredRectOutlineEx(fPosX, fPosY, fSizeX, fSizeY, ehPos, evPos, colRect);
}

// --------------------------------------------------------------------------------------
// Draws filled bar on anchor-based relative coords.
// --------------------------------------------------------------------------------------
extern void HUD_DrawAnchoredBarEx(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos,
    enum BarOrientations eBarOrientation, COLOR colBar, FLOAT fNormValue)
{
  FLOAT fOriginX = fPosX;
  FLOAT fOriginY = fPosY;
  
  FLOAT fModSizeX = fSizeX;
  FLOAT fModSizeY = fSizeY;
  
  FLOAT fFilledSizeX = fSizeX;
  FLOAT fFilledSizeY = fSizeY;
  
  // determine color
  COLOR col = colBar;
  if (col == NONE) {
    col = GetCurrentColor( fNormValue);
  }
  
  switch (ehPos)
  {
    default: {
      fOriginX = fOriginX;
    } break;

    case EHHAT_CENTER: {
      fOriginX = _pixDPWidth / 2.0F + fOriginX - fSizeX / 2.0F;
    } break;

    case EHHAT_RIGHT: {
      fOriginX = _pixDPWidth - fOriginX - fSizeX;
    } break;
  }

  switch (evPos)
  {
    default: {
      fOriginY = fOriginY;
    } break;

    case EHVAT_MID: {
      fOriginY = _pixDPHeight / 2.0F + fOriginY - fSizeY;
    } break;

    case EHVAT_BOT: {
      fOriginY = _pixDPHeight - fOriginY - fSizeY;
    } break;
  }
  
  switch (eBarOrientation)
  {
    case BO_LEFT: {
      fFilledSizeX *= fNormValue;
      if (fNormValue > 0.0F) fFilledSizeX = ClampDn(fFilledSizeX, 2.0F);

      fModSizeX = fFilledSizeX;
    } break;

    case BO_RIGHT: {
      fFilledSizeX *= fNormValue;
      if (fNormValue > 0.0F) fFilledSizeX = ClampDn(fFilledSizeX, 2.0F);

      fModSizeX = fFilledSizeX;
      fOriginX += fSizeX - fFilledSizeX;
    } break;

    case BO_UP: {
      fFilledSizeY *= fNormValue;
      if (fNormValue > 0.0F) fFilledSizeY = ClampDn(fFilledSizeY, 2.0F);

      fModSizeY = fFilledSizeY;
    } break;

    case BO_DOWN: {
      fFilledSizeY *= fNormValue;
      if (fNormValue > 0.0F) fFilledSizeY = ClampDn(fFilledSizeY, 2.0F);
      
      fModSizeY = fFilledSizeY;
      fOriginY += fSizeY - fFilledSizeY;
      fOriginY = ceil(fOriginY);
    } break;
  }
  
  _pDP->Fill(fOriginX, fOriginY, fModSizeX, fModSizeY, col|_ulAlphaHUD);
}

// --------------------------------------------------------------------------------------
// Draws filled bar on anchor-based relative coords.
// Uses relative rescalling.
// --------------------------------------------------------------------------------------
extern void HUD_DrawAnchoredBar(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos,
    enum BarOrientations eBarOrientation, COLOR colBar, FLOAT fNormValue)
{
  FLOAT fMul;

  if (_pixDPWidth > _pixDPHeight) {
    fMul = _pixDPHeight / 480.0F;
  } else {
    fMul = _pixDPWidth / 640.0F;
  }

  fPosX *= fMul;
  fPosY *= fMul;
  fSizeX *= fMul;
  fSizeY *= fMul;
  
  HUD_DrawAnchoredBarEx(fPosX, fPosY, fSizeX, fSizeY, ehPos, evPos, eBarOrientation, colBar, fNormValue);
}

// --------------------------------------------------------------------------------------
// Draws icon on anchor-based relative coordinates.
// --------------------------------------------------------------------------------------
extern void HUD_DrawAnchroredIconEx(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, CTextureObject &toIcon, COLOR colDefault, FLOAT fNormValue, BOOL bBlink)
{
  if (toIcon.GetData() == NULL) return;
  if (toIcon.GetData()->ad_Anims == NULL) return;
  
  // Determine color
  COLOR col = colDefault;
  if (col == NONE) col = GetCurrentColor( fNormValue);

  // determine blinking state
  if (bBlink && fNormValue<=(_cttHUD.ctt_fLowMedium/2)) {
    // activate blinking only if value is <= half the low edge
    INDEX iCurrentTime = (INDEX)(_tmNow*4);
    if (iCurrentTime&1) col = C_vdGRAY;
  }
  
  FLOAT fOriginX = fPosX;
  FLOAT fOriginY = fPosY;
  
  
  const FLOAT fHalfSizeI = fSizeX * 0.5F;
  const FLOAT fHalfSizeJ = fSizeY * 0.5F;
  
  switch (ehPos)
  {
    default: {
      fOriginX = fOriginX + fHalfSizeI;
    } break;

    case EHHAT_CENTER: {
      fOriginX = _pixDPWidth / 2.0F + fOriginX;
    } break;

    case EHHAT_RIGHT: {
      fOriginX = _pixDPWidth - fOriginX - fHalfSizeI;
    } break;
  }

  switch (evPos)
  {
    default: {
      fOriginY = fOriginY + fHalfSizeJ;
    } break;

    case EHVAT_MID: {
      fOriginY = _pixDPHeight / 2.0F + fOriginY - fHalfSizeJ;
    } break;

    case EHVAT_BOT: {
      fOriginY = _pixDPHeight - fOriginY - fHalfSizeJ;
    } break;
  }

  // done
  _pDP->InitTexture( &toIcon);
  _pDP->AddTexture( fOriginX - fHalfSizeI, fOriginY - fHalfSizeI, fOriginX + fHalfSizeI, fOriginY + fHalfSizeJ, col);
  _pDP->FlushRenderingQueue();
}

// --------------------------------------------------------------------------------------
// Draws icon on anchor-based relative coordinates.
// Uses relative rescalling.
// --------------------------------------------------------------------------------------
extern void HUD_DrawAnchroredIcon(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, CTextureObject &toIcon, COLOR colDefault, FLOAT fNormValue, BOOL bBlink)
{
  if (toIcon.GetData() == NULL) return;
  if (toIcon.GetData()->ad_Anims == NULL) return;
  
  FLOAT fMul;
  
  if (_pixDPWidth > _pixDPHeight) {
    fMul = _pixDPHeight / 480.0F;
  } else {
    fMul = _pixDPWidth / 640.0F;
  }

  fPosX *= fMul;
  fPosY *= fMul;
  fSizeX *= fMul;
  fSizeY *= fMul;
  
  HUD_DrawAnchroredIconEx(fPosX, fPosY, fSizeX, fSizeY, ehPos, evPos, toIcon, colDefault, fNormValue, bBlink);
}

extern void HUD_DrawAnchoredTextEx( FLOAT fPosX, FLOAT fPosY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, const CTString &strText, COLOR colDefault, FLOAT fNormValue)
{
  if (_pDP->dp_FontData == NULL) return; // TODO: Maybe make assert.
  
  // determine color
  COLOR col = colDefault;
  if (col == NONE) {
    col = GetCurrentColor( fNormValue);
  }

  FLOAT fOriginX = fPosX;
  FLOAT fOriginY = fPosY;
  
  FLOAT fTextScale = _fResolutionScaling * _fCustomScaling;
  
  FLOAT fCharHeight = _pfdDisplayFont->GetHeight() * fTextScale;

  _pDP->SetTextScaling(fTextScale);
  
  switch (ehPos)
  {
    default: {
      fOriginX = fOriginX + _pDP->GetTextWidth(strText) / 2;
    } break;

    case EHHAT_CENTER: {
      fOriginX = _pixDPWidth / 2.0F + fOriginX;
    } break;

    case EHHAT_RIGHT: {
      fOriginX = _pixDPWidth - fOriginX - _pDP->GetTextWidth(strText) / 2.0F;
    } break;
  }

  switch (evPos)
  {
    default: {
      fOriginY = fOriginY + fCharHeight / 2.0F;
    } break;

    case EHVAT_MID: {
      fOriginY = _pixDPHeight / 2.0F + fOriginY;
    } break;

    case EHVAT_BOT: {
      fOriginY = _pixDPHeight - fOriginY - fCharHeight;
    } break;
  }

  // done
  _pDP->PutTextCXY( strText, fOriginX, fOriginY, col|_ulAlphaHUD);
}

extern void HUD_DrawAnchoredText( FLOAT fPosX, FLOAT fPosY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, const CTString &strText, COLOR colDefault, FLOAT fNormValue)
{
  FLOAT fMul;
  
  if (_pixDPWidth > _pixDPHeight) {
    fMul = _pixDPHeight / 480.0F;
  } else {
    fMul = _pixDPWidth / 640.0F;
  }

  fPosX *= fMul;
  fPosY *= fMul;
  
  HUD_DrawAnchoredTextEx(fPosX, fPosY, ehPos, evPos, strText, colDefault, fNormValue);
}

extern void HUD_DrawAnchoredTextInRectEx(FLOAT2D vPos, FLOAT2D vSize, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, const CTString &strText, COLOR colDefault, FLOAT fNormValue, FLOAT fFontSizeMul)
{
  if (_pDP->dp_FontData == NULL) return; // TODO: Maybe make assert.
  
  // determine color
  COLOR col = colDefault;
  if (col == NONE) {
    col = GetCurrentColor( fNormValue);
  }

  FLOAT fOriginX = vPos(1);
  FLOAT fOriginY = vPos(2);
  
  FLOAT fTextScale = vSize(2) / _pDP->dp_FontData->GetHeight();

  _pDP->SetTextScaling(fTextScale * fFontSizeMul);
  
  switch (ehPos)
  {
    default: {
      fOriginX = fOriginX + vSize(1) / 2.0F;
    } break;

    case EHHAT_CENTER: {
      fOriginX = _pixDPWidth / 2.0F + fOriginX;
    } break;

    case EHHAT_RIGHT: {
      fOriginX = _pixDPWidth - fOriginX - vSize(1) / 2.0F;
    } break;
  }

  switch (evPos)
  {
    default: {
      fOriginY = fOriginY + vSize(2) / 2.0F;
    } break;

    case EHVAT_MID: {
      fOriginY = _pixDPHeight / 2.0F + fOriginY;
    } break;

    case EHVAT_BOT: {
      fOriginY = _pixDPHeight - fOriginY - vSize(2) / 2.0F;
    } break;
  }

  // done
  _pDP->PutTextCXY( strText, fOriginX, fOriginY, col|_ulAlphaHUD);
}

extern void HUD_DrawAnchoredTextInRect(FLOAT2D vPos, FLOAT2D vSize, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, const CTString &strText, COLOR colDefault, FLOAT fNormValue, FLOAT fFontSizeMul)
{
  FLOAT fMul;
  
  if (_pixDPWidth > _pixDPHeight) {
    fMul = _pixDPHeight / 480.0F;
  } else {
    fMul = _pixDPWidth / 640.0F;
  }

  vPos *= fMul;
  vSize *= fMul;
  
  HUD_DrawAnchoredTextInRectEx(vPos, vSize, ehPos, evPos, strText, colDefault, fNormValue, fFontSizeMul);
}