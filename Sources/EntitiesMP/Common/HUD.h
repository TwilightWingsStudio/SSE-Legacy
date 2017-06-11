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

#ifndef SE_INCL_HUD_H
#define SE_INCL_HUD_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// all info about color transitions
struct ColorTransitionTable {
  COLOR ctt_colFine;      // color for values over 1.0
  COLOR ctt_colHigh;      // color for values from 1.0 to 'fMedium'
  COLOR ctt_colMedium;    // color for values from 'fMedium' to 'fLow'
  COLOR ctt_colLow;       // color for values under fLow
  FLOAT ctt_fMediumHigh;  // when to switch to high color   (normalized float!)
  FLOAT ctt_fLowMedium;   // when to switch to medium color (normalized float!)
  BOOL  ctt_bSmooth;      // should colors have smooth transition
};

enum EHUDHorAnchorType
{
  EHHAT_LEFT = 0,
  EHHAT_CENTER = 1,
  EHHAT_RIGHT = 2,
};

enum EHUDVerAnchorType
{
  EHVAT_TOP = 0,
  EHVAT_MID = 1,
  EHVAT_BOT = 2,
};

// where is the bar lowest value
enum BarOrientations {
  BO_LEFT  = 1,
  BO_RIGHT = 2,
  BO_UP    = 3,
  BO_DOWN  = 4,
};

extern CDrawPort *_pDP;
extern PIX _pixDPWidth;
extern PIX _pixDPHeight;

extern FLOAT _fResolutionScaling;
extern FLOAT _fCustomScaling;

extern CFontData &_fdNumbersFont;

extern ULONG _ulAlphaHUD;
extern ULONG _ulBrAlpha;

extern TIME _tmNow;
extern TIME _tmLast;

extern struct ColorTransitionTable &_cttHUD;

extern COLOR GetCurrentColor( FLOAT fNormalizedValue);

extern void HUD_DrawAnchoredRectEx(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, COLOR colRect);
extern void HUD_DrawAnchoredRect(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, COLOR colRect);
extern void HUD_DrawAnchoredRectOutlineEx(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, COLOR colRect);
extern void HUD_DrawAnchoredRectOutline(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, COLOR colRect);
extern void HUD_DrawAnchroredIconEx(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, CTextureObject &toIcon, COLOR colDefault, FLOAT fNormValue, BOOL bBlink);
extern void HUD_DrawAnchroredIcon(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, CTextureObject &toIcon, COLOR colDefault, FLOAT fNormValue, BOOL bBlink);
extern void HUD_DrawAnchoredTextEx( FLOAT fPosX, FLOAT fPosY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, const CTString &strText, COLOR colDefault, FLOAT fNormValue);
extern void HUD_DrawAnchoredText( FLOAT fPosX, FLOAT fPosY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, const CTString &strText, COLOR colDefault, FLOAT fNormValue);
extern void HUD_DrawAnchoredTextInRectEx( FLOAT2D vPos, FLOAT2D vSize, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, const CTString &strText, COLOR colDefault, FLOAT fNormValue, FLOAT fFontSizeMul = 1.0F);
extern void HUD_DrawAnchoredTextInRect( FLOAT2D vPos, FLOAT2D vSize, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, const CTString &strText, COLOR colDefault, FLOAT fNormValue, FLOAT fFontSizeMul = 1.0F);

extern void HUD_DrawAnchoredBarEx(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos,
    enum BarOrientations eBarOrientation, COLOR colBar, FLOAT fNormValue);
extern void HUD_DrawAnchoredBar(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos,
    enum BarOrientations eBarOrientation, COLOR colBar, FLOAT fNormValue);
	
extern void HUD_DrawDebugMonitor();
extern void HUD_DrawEntityStack();

#endif