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


#include "stdafx.h"
#include "LCDDrawing.h"
#include <locale.h>

#define USECUSTOMTEXT 0

extern CGame *_pGame;

#if USECUSTOMTEXT
  static CTString _strCustomText = "";
#endif
static CDrawPort *_pdpLoadingHook = NULL;  // drawport for loading hook
extern BOOL _bUserBreakEnabled;
extern BOOL map_bIsFirstEncounter;

#define MAXLOADINGSCREENS 3

static CTextureObject _atoGenericLoadingScreens[MAXLOADINGSCREENS];
static INDEX _iGenericLoadingScreens = -1;
static BOOL _bGenericLoadingScreensError = FALSE;

#define REFRESHTIME (0.2f)

void RemapLevelNames(INDEX &iLevel)
{
  switch( iLevel) {
  case 10:  iLevel =  1;  break;
  case 11:  iLevel =  2;  break;
  case 12:  iLevel =  3;  break;
  case 13:  iLevel =  4;  break;
  case 14:  iLevel =  5;  break;
  case 15:  iLevel =  6;  break;
  case 21:  iLevel =  7;  break;
  case 22:  iLevel =  8;  break;
  case 23:  iLevel =  9;  break;
  case 24:  iLevel = 10;  break;
  case 31:  iLevel = 11;  break;
  case 32:  iLevel = 12;  break;
  case 33:  iLevel = 13;  break;
  default:  iLevel = -1;  break;
  }
}

static void ObtainGenericLoadingScreens()
{
  try
  {
    _atoGenericLoadingScreens[0].SetData_t( CTFILENAME("Textures\\LoadingScreens\\Generic\\Generic_01.tex"));
    _iGenericLoadingScreens++;
    
    _atoGenericLoadingScreens[1].SetData_t( CTFILENAME("Textures\\LoadingScreens\\Generic\\Generic_02.tex"));
    _iGenericLoadingScreens++;
 
    _atoGenericLoadingScreens[2].SetData_t( CTFILENAME("Textures\\LoadingScreens\\Generic\\Generic_03.tex"));
    _iGenericLoadingScreens++;
    
    ((CTextureData*)_atoGenericLoadingScreens[0].GetData())->Force(TEX_CONSTANT);
    ((CTextureData*)_atoGenericLoadingScreens[1].GetData())->Force(TEX_CONSTANT);
    ((CTextureData*)_atoGenericLoadingScreens[2].GetData())->Force(TEX_CONSTANT);
  } catch (char *strError) {
    CPrintF("Warning! %s\n", strError);
    _bGenericLoadingScreensError = TRUE;
  }
}

static void LoadingHook_t(CProgressHookInfo *pphi)
{
  // if user presses escape
  ULONG ulCheckFlags = 0x8000;
  if (pphi->phi_fCompleted>0) {
    ulCheckFlags |= 0x0001;
  }
  if (_bUserBreakEnabled && (GetAsyncKeyState(VK_ESCAPE)&ulCheckFlags)) {
    // break loading
    throw TRANS("User break!");
  }

#if USECUSTOMTEXT
  // if no custom loading text
  if (_strCustomText=="") {
    // load it
    try {
      _strCustomText.Load_t(CTFILENAME("Data\\LoadingText.txt"));
    } catch (char *strError) {
      _strCustomText = strError;
    }
  }
#endif

  // measure time since last call
  static CTimerValue tvLast(0I64);
  CTimerValue tvNow = _pTimer->GetHighPrecisionTimer();

  // if not first or final update, and not enough time passed
  if (pphi->phi_fCompleted!=0 && pphi->phi_fCompleted!=1 &&
     (tvNow-tvLast).GetSeconds() < REFRESHTIME) {
    // do nothing
    return;
  }
  tvLast = tvNow;

  // skip if cannot lock drawport
  CDrawPort *pdp = _pdpLoadingHook;                           
  ASSERT(pdp!=NULL);
  CDrawPort dpHook(pdp, TRUE);
  if( !dpHook.Lock()) return;

  // clear screen
  dpHook.Fill(C_BLACK|255);

  // get session properties currently loading
  CSessionProperties *psp = (CSessionProperties *)_pNetwork->GetSessionProperties();
  ULONG ulLevelMask = psp->sp_ulLevelsMask;
  if (psp->sp_bCooperative) {
    INDEX iLevel = -1;
    INDEX iLevelNext = -1;
    CTString strLevelName = _pNetwork->ga_fnmWorld.FileName();
    CTString strNextLevelName = _pNetwork->ga_fnmNextLevel.FileName();
    
    // second encounter
    INDEX u, v;
    u = v = -1;
    strLevelName.ScanF("%01d_%01d_", &u, &v);
    iLevel = u*10+v;
    RemapLevelNames(iLevel);
    u = v = -1;
    strNextLevelName.ScanF("%01d_%01d_", &u, &v);
    iLevelNext = u*10+v;
    RemapLevelNames(iLevelNext);

    // first encounter
    if(iLevel == -1) {
      strLevelName.ScanF("%02d_", &iLevel);
      strNextLevelName.ScanF("%02d_", &iLevelNext);

      if(iLevel != -1) {
        map_bIsFirstEncounter = TRUE;
      }
    } else {
      map_bIsFirstEncounter = FALSE;
    }
   
    if (iLevel>0) {
      ulLevelMask|=1<<(iLevel-1);
    }
    if (iLevelNext>0) {
      ulLevelMask|=1<<(iLevelNext-1);
    }
  }

  if (ulLevelMask != 0 && !_pNetwork->IsPlayingDemo()) {
    // map hook
    extern void RenderMap( CDrawPort *pdp, ULONG ulLevelMask, CProgressHookInfo *pphi);
    RenderMap(&dpHook, ulLevelMask, pphi);

    // finish rendering
    dpHook.Unlock();
    dpHook.dp_Raster->ra_pvpViewPort->SwapBuffers();

    // keep current time
    tvLast = _pTimer->GetHighPrecisionTimer();
    return;
  }

  if (!_bGenericLoadingScreensError && _iGenericLoadingScreens < 0) {
    ObtainGenericLoadingScreens();
  }
  
  if (_iGenericLoadingScreens > 0 && !GetSP()->sp_bQuickTest) {
    pdp->PutTexture(&_atoGenericLoadingScreens[1], PIXaabbox2D( PIX2D(0, 0), PIX2D(pdp->GetWidth(), pdp->GetHeight())), C_WHITE|255);
  }
  
  // Get sizes.
  PIX pixSizeI = dpHook.GetWidth();
  PIX pixSizeJ = dpHook.GetHeight();
  CFontData *pfd = _pfdDisplayFont;

  COLOR colOutline = 0x5acdFF00|0xff; 
  COLOR colBar = 0x2d667f00|0xFF;

  // Bar background
  dpHook.Fill(0, pixSizeJ-30, pixSizeI, 30, C_BLACK|128); 
 
  // BAR
  dpHook.DrawBorder(8, pixSizeJ-22, pixSizeI-16, 14, colOutline);
  dpHook.Fill(11, pixSizeJ-19, (pixSizeI-22)*pphi->phi_fCompleted, 8, colBar);
  
  dpHook.SetFont( _pfdDisplayFont);
  dpHook.SetTextScaling( 1.0f);
  dpHook.SetTextAspect( 1.0f);
  
  // Fill Data
  setlocale(LC_ALL, "");
  CTString strDesc(0, "%s", pphi->phi_strDescription);
  strupr((char*)(const char*)strDesc);
  
  setlocale(LC_ALL, "C");
  CTString strPerc(0, "%3.0f%%", pphi->phi_fCompleted*100);
  
  CTString strExtra = "";
  BOOL bExtraData = FALSE;
  
  switch (pphi->phi_eExtraDataType)
  {
    case CProgressHookInfo::EDT_VALUE: {
      strExtra.PrintF("%lu / %lu", pphi->phi_ulExtraCompleted, pphi->phi_ulExtraExpected);
      bExtraData = TRUE;
    } break;
 
    case CProgressHookInfo::EDT_SIZE: {
      strExtra.PrintF("%.1f k / %.1f k", pphi->phi_ulExtraCompleted / 1024.0F, pphi->phi_ulExtraExpected / 1024.0F);
      bExtraData = TRUE;
    } break;
    
    default: break;
  }

  //
  PIX pixCharSizeI = pfd->fd_pixCharWidth + pfd->fd_pixCharSpacing;
  PIX pixCharSizeJ = pfd->fd_pixCharHeight + pfd->fd_pixLineSpacing;
  
  // State
  dpHook.Fill(0, pixSizeJ-44, 256, 14, C_BLACK|128); 
  
  dpHook.InitTexture( NULL);
  dpHook.AddTriangle((PIX)256, (PIX)(pixSizeJ-30), 256, (PIX)(pixSizeJ-44), 264, (PIX)(pixSizeJ-30), C_BLACK|128);
  dpHook.FlushRenderingQueue();
  
  if (bExtraData)
  {
    // Extra
    dpHook.Fill(0, 0, 128, 20, C_BLACK|128); 
    
    dpHook.InitTexture( NULL);
    dpHook.AddTriangle(128, 0, 128, 20, 136, 0, C_BLACK|128);
    dpHook.FlushRenderingQueue();
    
    dpHook.PutText(strExtra, 8, 4, colOutline);
  }

  // Percent
  PIX pixPercPanelSizeI = 48;
  PIX pixPercPanelOffsetI = -16; // e[-640; 0]
 
  dpHook.InitTexture( NULL);
  dpHook.AddTriangle((PIX)(pixSizeI-72), (PIX)(pixSizeJ-30), (PIX)(pixSizeI-64), (PIX)(pixSizeJ-30), (PIX)(pixSizeI-64), (PIX)(pixSizeJ-44), C_BLACK|128); 
  dpHook.AddTriangle((PIX)(pixSizeI-16), (PIX)(pixSizeJ-30), (PIX)(pixSizeI-8), (PIX)(pixSizeJ-30), (PIX)(pixSizeI-16), (PIX)(pixSizeJ-44), C_BLACK|128);
  dpHook.FlushRenderingQueue();
 
  dpHook.Fill(pixSizeI+pixPercPanelOffsetI-pixPercPanelSizeI, pixSizeJ-44, pixPercPanelSizeI, 14, C_BLACK|128); 
  
  dpHook.PutText(strDesc, 8, pixSizeJ-47+pixCharSizeJ/2, colOutline); // Description
  dpHook.PutTextC(strPerc, pixSizeI+pixPercPanelOffsetI-pixPercPanelSizeI/2, pixSizeJ-47+pixCharSizeJ/2, colOutline); // Percent
  
  dpHook.Unlock();
  // finish rendering
  dpHook.dp_Raster->ra_pvpViewPort->SwapBuffers();

  // keep current time
  tvLast = _pTimer->GetHighPrecisionTimer();
}

// loading hook functions
void CGame::EnableLoadingHook(CDrawPort *pdpDrawport)
{
  _pdpLoadingHook = pdpDrawport;
  SetProgressHook(LoadingHook_t);
}

void CGame::DisableLoadingHook(void)
{
  SetProgressHook(NULL);
  _pdpLoadingHook = NULL;
}
