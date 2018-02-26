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
#include "HUD_internal.h"
#include "HudAssetsContainer.h"

#include <Engine/Graphics/DrawPort.h>

#include <EntitiesMP/Player.h>
#include <EntitiesMP/PlayerWeapons.h>
#include <EntitiesMP/MusicHolder.h>
#include <EntitiesMP/EnemyBase.h>
#include <EntitiesMP/EnemyCounter.h>
#include <EntitiesMP/HudBossBarDisplay.h>

#define ENTITY_DEBUG

// armor & health constants
// NOTE: these _do not_ reflect easy/tourist maxvalue adjustments. that is by design!
#define TOP_ARMOR  100
#define TOP_HEALTH 100

static CListHead _lhAllHUDTextures;
extern CHudAssetsContainer *_pHAC = NULL;

// Cheats
extern INDEX cht_bEnable;
extern INDEX cht_bGod;
extern INDEX cht_bFly;
extern INDEX cht_bGhost;
extern INDEX cht_bInvisible;
extern FLOAT cht_fTranslationMultiplier;
extern INDEX cht_bAutoKill;  // [SSE] Cheats Expansion
extern INDEX cht_bInfiniteAmmo; // [SSE] Cheats Expansion

// Interface Control
extern INDEX hud_bShowInfo;
extern INDEX hud_bShowLatency;
extern INDEX hud_bShowMessages;
extern INDEX hud_iShowPlayers;
extern INDEX hud_iSortPlayers;
extern FLOAT hud_fOpacity;
extern FLOAT hud_fScaling;
extern FLOAT hud_tmWeaponsOnScreen;
extern INDEX hud_bShowMatchInfo;

// [SSE] HUD - Tweaks
extern INDEX hud_bColorCustomization;
extern INDEX hud_bTeamColorBasedColor;
extern INDEX hud_colBorders;

extern BOOL hud_bShowEmptyAmmoInList;

// [SSE] HUD - Sniper Scope Settings
extern BOOL hud_bSniperScopeDraw;
extern BOOL hud_bSniperScopeRenderFromQuarter;

extern FLOAT hud_fSniperScopeBaseOpacity;
extern FLOAT hud_fSniperScopeWheelOpacity;
extern FLOAT hud_fSniperScopeLedOpacity;
extern FLOAT hud_fSniperScopeZoomTextOpacity;
extern FLOAT hud_fSniperScopeZoomIconOpacity;
extern FLOAT hud_fSniperScopeRangeTextOpacity;
extern FLOAT hud_fSniperScopeRangeIconOpacity;

extern BOOL hud_bSniperScopeWheelColoring;
//

// [SSE]
extern BOOL hud_bGameDebugMonitor;
extern INDEX hud_iHUDType;
//

// [SSE] Radar
/*
extern BOOL hud_bRadarDraw;
*/
//

extern const INDEX aiWeaponsRemap[19];

// Base.
extern CDrawPort *_pDP = NULL;
extern PIX _pixDPWidth = 0;
extern PIX _pixDPHeight = 0;

extern FLOAT _fResolutionScaling = 1.0F;
extern FLOAT _fCustomScaling = 1.0F;

extern ULONG _ulAlphaHUD = C_WHITE|CT_OPAQUE;
extern ULONG _ulBrAlpha = C_WHITE|CT_OPAQUE;

extern TIME _tmNow = -1.0f;
extern TIME _tmLast = -1.0f;

// drawing variables
extern const CPlayer *_penPlayer = NULL;
static CPlayerWeapons *_penWeapons;

extern COLOR _colHUD = C_WHITE;
static COLOR _colHUDText;
extern CFontData &_fdNumbersFont = *new CFontData;

// array for pointers of all players
extern CPlayer *_apenPlayers[NET_MAXGAMEPLAYERS] = {0};

// [SSE] Default Texture
static CImageInfo _iiDefault; 
static CTextureData *_tdDefault = NULL;
//

static BOOL _bHUDFontsLoaded = FALSE; // [SSE] HUD No Crash If No Assets

extern struct ColorTransitionTable &_cttHUD = *(new ColorTransitionTable);

extern struct WeaponInfo _awiWeapons[18];
static struct AmmoInfo _aaiAmmo[9] = {
  { NULL, &_awiWeapons[5],  &_awiWeapons[6],  0, 0, 0, -9, FALSE }, //  0
  { NULL, &_awiWeapons[7],  &_awiWeapons[8],  0, 0, 0, -9, FALSE }, //  1
  { NULL, &_awiWeapons[9],  NULL,             0, 0, 0, -9, FALSE }, //  2
  { NULL, &_awiWeapons[10], NULL,             0, 0, 0, -9, FALSE }, //  3
  { NULL, &_awiWeapons[12], NULL,             0, 0, 0, -9, FALSE }, //  4
  { NULL, &_awiWeapons[13], NULL,             0, 0, 0, -9, FALSE }, //  5
  { NULL, &_awiWeapons[15], NULL,             0, 0, 0, -9, FALSE }, //  6
  { NULL, &_awiWeapons[14], NULL,             0, 0, 0, -9, FALSE }, //  7

  { NULL, &_awiWeapons[16], NULL,             0, 0, 0, -9, FALSE }, //  8
};

struct WeaponInfo _awiWeapons[18] = {
  { WEAPON_NONE,            NULL,  NULL,         FALSE },   //  0
  { WEAPON_FISTS,           NULL,  NULL,         FALSE },   //  1
  { WEAPON_KNIFE,           NULL,  NULL,         FALSE },   //  2
  { WEAPON_COLT,            NULL,  NULL,         FALSE },   //  3
  { WEAPON_DOUBLECOLT,      NULL,  NULL,         FALSE },   //  4
  { WEAPON_SINGLESHOTGUN,   NULL,  &_aaiAmmo[0], FALSE },   //  5
  { WEAPON_DOUBLESHOTGUN,   NULL,  &_aaiAmmo[0], FALSE },   //  6
  { WEAPON_TOMMYGUN,        NULL,  &_aaiAmmo[1], FALSE },   //  7
  { WEAPON_MINIGUN,         NULL,  &_aaiAmmo[1], FALSE },   //  8
  { WEAPON_ROCKETLAUNCHER,  NULL,  &_aaiAmmo[2], FALSE },   //  9
  { WEAPON_GRENADELAUNCHER, NULL,  &_aaiAmmo[3], FALSE },   // 10
  { WEAPON_CHAINSAW,        NULL,  NULL,         FALSE },   // 11
  { WEAPON_FLAMER,          NULL,  &_aaiAmmo[4], FALSE },   // 12
  { WEAPON_LASER,           NULL,  &_aaiAmmo[5], FALSE },   // 13
  { WEAPON_SNIPER,          NULL,  &_aaiAmmo[7], FALSE },   // 14
  { WEAPON_IRONCANNON,      NULL,  &_aaiAmmo[6], FALSE },   // 15
  { WEAPON_SERIOUSBOMB,     NULL,  &_aaiAmmo[8], FALSE },   // 16
  { WEAPON_NONE,            NULL,  NULL,         FALSE },   // 17

//{ WEAPON_PIPEBOMB,        &_pHAC->m_toWPipeBomb,        &_aaiAmmo[3], FALSE },   // 15
//{ WEAPON_GHOSTBUSTER,     &_pHAC->m_toWGhostBuster,     &_aaiAmmo[5], FALSE },   // 16
//{ WEAPON_NUKECANNON,      &_pHAC->m_toWNukeCannon,      &_aaiAmmo[7], FALSE },   // 17
};

// TODO: Do something with this dirty hack.
void InitAmmoWeaponIconsLinks()
{
  CPrintF("Defining Ammo Icon Links:\n");

  for (INDEX i = 0; i < 9; i++) {
    CPrintF("  %d\n", i);
    _aaiAmmo[i].ai_ptoAmmo = &_pHAC->m_atoAmmo[i];
  }

  CPrintF("Defining Weapon Icon Links:\n");
  _awiWeapons[1].wi_ptoWeapon = &_pHAC->m_toWFists;
  _awiWeapons[2].wi_ptoWeapon = &_pHAC->m_toWKnife;
  _awiWeapons[3].wi_ptoWeapon = &_pHAC->m_toWColt;
  _awiWeapons[4].wi_ptoWeapon = &_pHAC->m_toWColt;
  _awiWeapons[5].wi_ptoWeapon = &_pHAC->m_toWSingleShotgun;
  _awiWeapons[6].wi_ptoWeapon = &_pHAC->m_toWDoubleShotgun;
  _awiWeapons[7].wi_ptoWeapon = &_pHAC->m_toWTommygun;
  _awiWeapons[8].wi_ptoWeapon = &_pHAC->m_toWMinigun;
  _awiWeapons[9].wi_ptoWeapon = &_pHAC->m_toWRocketLauncher;
  _awiWeapons[10].wi_ptoWeapon = &_pHAC->m_toWGrenadeLauncher;
  _awiWeapons[11].wi_ptoWeapon = &_pHAC->m_toWChainsaw;
  _awiWeapons[12].wi_ptoWeapon = &_pHAC->m_toWFlamer;
  _awiWeapons[13].wi_ptoWeapon = &_pHAC->m_toWLaser;
  _awiWeapons[14].wi_ptoWeapon = &_pHAC->m_toWSniper;
  _awiWeapons[15].wi_ptoWeapon = &_pHAC->m_toWIronCannon;
  _awiWeapons[16].wi_ptoWeapon = &_pHAC->m_toWSeriousBomb;
}

// --------------------------------------------------------------------------------------
// Compare functions for qsort().
// --------------------------------------------------------------------------------------
static int qsort_CompareNames( const void *ppPEN0, const void *ppPEN1) {
  CPlayer &en0 = **(CPlayer**)ppPEN0;
  CPlayer &en1 = **(CPlayer**)ppPEN1;
  CTString strName0 = en0.GetPlayerName();
  CTString strName1 = en1.GetPlayerName();
  return strnicmp( strName0, strName1, 8);
}

static int qsort_CompareScores( const void *ppPEN0, const void *ppPEN1) {
  CPlayer &en0 = **(CPlayer**)ppPEN0;
  CPlayer &en1 = **(CPlayer**)ppPEN1;
  SLONG sl0 = en0.m_psGameStats.ps_iScore;
  SLONG sl1 = en1.m_psGameStats.ps_iScore;
  if (     sl0<sl1) return +1;
  else if (sl0>sl1) return -1;
  else              return  0;
}

static int qsort_CompareHealth( const void *ppPEN0, const void *ppPEN1) {
  CPlayer &en0 = **(CPlayer**)ppPEN0;
  CPlayer &en1 = **(CPlayer**)ppPEN1;
  SLONG sl0 = (SLONG)ceil(en0.GetHealth());
  SLONG sl1 = (SLONG)ceil(en1.GetHealth());
  if (     sl0<sl1) return +1;
  else if (sl0>sl1) return -1;
  else              return  0;
}

static int qsort_CompareManas( const void *ppPEN0, const void *ppPEN1) {
  CPlayer &en0 = **(CPlayer**)ppPEN0;
  CPlayer &en1 = **(CPlayer**)ppPEN1;
  SLONG sl0 = en0.m_iMana;
  SLONG sl1 = en1.m_iMana;
  if (     sl0<sl1) return +1;
  else if (sl0>sl1) return -1;
  else              return  0;
}

static int qsort_CompareDeaths( const void *ppPEN0, const void *ppPEN1) {
  CPlayer &en0 = **(CPlayer**)ppPEN0;
  CPlayer &en1 = **(CPlayer**)ppPEN1;
  SLONG sl0 = en0.m_psGameStats.ps_iDeaths;
  SLONG sl1 = en1.m_psGameStats.ps_iDeaths;
  if (     sl0<sl1) return +1;
  else if (sl0>sl1) return -1;
  else              return  0;
}

static int qsort_CompareFrags( const void *ppPEN0, const void *ppPEN1) {
  CPlayer &en0 = **(CPlayer**)ppPEN0;
  CPlayer &en1 = **(CPlayer**)ppPEN1;
  SLONG sl0 = en0.m_psGameStats.ps_iKills;
  SLONG sl1 = en1.m_psGameStats.ps_iKills;
  if (     sl0<sl1) return +1;
  else if (sl0>sl1) return -1;
  else              return -qsort_CompareDeaths(ppPEN0, ppPEN1);
}

static int qsort_CompareLatencies( const void *ppPEN0, const void *ppPEN1) {
  CPlayer &en0 = **(CPlayer**)ppPEN0;
  CPlayer &en1 = **(CPlayer**)ppPEN1;
  SLONG sl0 = (SLONG)ceil(en0.m_tmLatency);
  SLONG sl1 = (SLONG)ceil(en1.m_tmLatency);
  if (     sl0<sl1) return +1;
  else if (sl0>sl1) return -1;
  else              return  0;
}

static INDEX bit_count(INDEX iValue)
{
    INDEX ctSet = 0;

    while (iValue) {
        iValue = (iValue) & (iValue - 1);
        ctSet++;
    }

    return ctSet;
}

static COLOR HUD_GetTeamColor(INDEX iTeamID)
{
  switch (iTeamID)
  {
    case 1: return C_lBLUE; break;
    case 2: return C_lRED; break;
    case 3: return C_lGREEN; break;
    case 4: return C_lYELLOW; break;
  }

  return 0x80808000;
}

static COLOR HUD_GetInterfaceColor(const CPlayer *penPlayerOwner)
{
  if (hud_bColorCustomization) {
    return hud_colBorders;
  }
  
  const BOOL bTeamPlay = GetSP()->sp_ulGameModeFlags & GMF_TEAMPLAY;
  
  if (hud_bTeamColorBasedColor && bTeamPlay) {
    return HUD_GetTeamColor(penPlayerOwner->m_iTeamID);
  }
  
  return 0x4C80BB00; // Default Blue.
}

// --------------------------------------------------------------------------------------
// Prepare color transitions.
// --------------------------------------------------------------------------------------
static void HUD_PrepareColorTransitions( COLOR colFine, COLOR colHigh, COLOR colMedium, COLOR colLow,
                                     FLOAT fMediumHigh, FLOAT fLowMedium, BOOL bSmooth)
{
  _cttHUD.ctt_colFine     = colFine;
  _cttHUD.ctt_colHigh     = colHigh;
  _cttHUD.ctt_colMedium   = colMedium;
  _cttHUD.ctt_colLow      = colLow;
  _cttHUD.ctt_fMediumHigh = fMediumHigh;
  _cttHUD.ctt_fLowMedium  = fLowMedium;
  _cttHUD.ctt_bSmooth     = bSmooth;
}

// --------------------------------------------------------------------------------------
// Calculates shake ammount and color value depanding on value change.
// --------------------------------------------------------------------------------------
#define SHAKE_TIME (2.0f)
static COLOR HUD_AddShaker( PIX const pixAmmount, INDEX const iCurrentValue, INDEX &iLastValue,
                        TIME &tmChanged, FLOAT &fMoverX, FLOAT &fMoverY)
{
  // update shaking if needed
  fMoverX = fMoverY = 0.0f;
  const TIME tmNow = _pTimer->GetLerpedCurrentTick();
  if (iCurrentValue != iLastValue) {
    iLastValue = iCurrentValue;
    tmChanged  = tmNow;
  } else {
    // in case of loading (timer got reseted)
    tmChanged = ClampUp( tmChanged, tmNow);
  }

  // no shaker?
  const TIME tmDelta = tmNow - tmChanged;
  if (tmDelta > SHAKE_TIME) return NONE;
  ASSERT( tmDelta>=0);
  // shake, baby shake!
  const FLOAT fAmmount    = _fResolutionScaling * _fCustomScaling * pixAmmount;
  const FLOAT fMultiplier = (SHAKE_TIME-tmDelta)/SHAKE_TIME *fAmmount;
  const INDEX iRandomizer = (INDEX)(tmNow*511.0f)*fAmmount*iCurrentValue;
  const FLOAT fNormRnd1   = (FLOAT)((iRandomizer ^ (iRandomizer>>9)) & 1023) * 0.0009775f;  // 1/1023 - normalized
  const FLOAT fNormRnd2   = (FLOAT)((iRandomizer ^ (iRandomizer>>7)) & 1023) * 0.0009775f;  // 1/1023 - normalized
  fMoverX = (fNormRnd1 -0.5f) * fMultiplier;
  fMoverY = (fNormRnd2 -0.5f) * fMultiplier;
  // clamp to adjusted ammount (pixels relative to resolution and HUD scale
  fMoverX = Clamp( fMoverX, -fAmmount, fAmmount);
  fMoverY = Clamp( fMoverY, -fAmmount, fAmmount);
  if (tmDelta < SHAKE_TIME/3) return C_WHITE;
  else return NONE;
//return FloatToInt(tmDelta*4) & 1 ? C_WHITE : NONE;
}

// --------------------------------------------------------------------------------------
// Get current color from local color transitions table.
// --------------------------------------------------------------------------------------
extern COLOR HUD_GetCurrentColor( FLOAT fNormalizedValue)
{
  // if value is in 'low' zone just return plain 'low' alert color
  if (fNormalizedValue < _cttHUD.ctt_fLowMedium) return( _cttHUD.ctt_colLow & 0xFFFFFF00);
  // if value is in out of 'extreme' zone just return 'extreme' color
  if (fNormalizedValue > 1.0f) return( _cttHUD.ctt_colFine & 0xFFFFFF00);

  COLOR col;

  // should blend colors?
  if (_cttHUD.ctt_bSmooth)
  { // lets do some interpolations
    FLOAT fd, f1, f2;
    COLOR col1, col2;
    UBYTE ubH,ubS,ubV, ubH2,ubS2,ubV2;
    // determine two colors for interpolation
    if (fNormalizedValue > _cttHUD.ctt_fMediumHigh) {
      f1   = 1.0f;
      f2   = _cttHUD.ctt_fMediumHigh;
      col1 = _cttHUD.ctt_colHigh;
      col2 = _cttHUD.ctt_colMedium;
    } else { // fNormalizedValue > _cttHUD.ctt_fLowMedium == TRUE !
      f1   = _cttHUD.ctt_fMediumHigh;
      f2   = _cttHUD.ctt_fLowMedium;
      col1 = _cttHUD.ctt_colMedium;
      col2 = _cttHUD.ctt_colLow;
    }
    // determine interpolation strength
    fd = (fNormalizedValue-f2) / (f1-f2);
    // convert colors to HSV
    ColorToHSV( col1, ubH,  ubS,  ubV);
    ColorToHSV( col2, ubH2, ubS2, ubV2);
    // interpolate H, S and V components
    ubH = (UBYTE)(ubH*fd + ubH2*(1.0f-fd));
    ubS = (UBYTE)(ubS*fd + ubS2*(1.0f-fd));
    ubV = (UBYTE)(ubV*fd + ubV2*(1.0f-fd));
    // convert HSV back to COLOR
    col = HSVToColor( ubH, ubS, ubV);

  } else { // simple color picker
    col = _cttHUD.ctt_colMedium;
    if (fNormalizedValue > _cttHUD.ctt_fMediumHigh) col = _cttHUD.ctt_colHigh;
  }

  // all done
  return( col & 0xFFFFFF00);
}

// --------------------------------------------------------------------------------------
// Fill array with players' statistics (returns current number of players in game).
// --------------------------------------------------------------------------------------
extern INDEX HUD_SetAllPlayersStats( INDEX iSortKey)
{
  // determine maximum number of players for this session
  INDEX iPlayers    = 0;
  INDEX iMaxPlayers = _penPlayer->GetMaxPlayers();
  CPlayer *penCurrent;

  // loop thru potentional players
  for (INDEX i=0; i<iMaxPlayers; i++)
  { // ignore non-existent players
    penCurrent = (CPlayer*)&*_penPlayer->GetPlayerEntity(i);
    if (penCurrent == NULL) continue;
    // fill in player parameters
    _apenPlayers[iPlayers] = penCurrent;
    // advance to next real player
    iPlayers++;
  }

  // sort statistics by some key if needed
  switch( iSortKey) {
    case PSK_NAME:    qsort( _apenPlayers, iPlayers, sizeof(CPlayer*), qsort_CompareNames);   break;
    case PSK_SCORE:   qsort( _apenPlayers, iPlayers, sizeof(CPlayer*), qsort_CompareScores);  break;
    case PSK_HEALTH:  qsort( _apenPlayers, iPlayers, sizeof(CPlayer*), qsort_CompareHealth);  break;
    case PSK_MANA:    qsort( _apenPlayers, iPlayers, sizeof(CPlayer*), qsort_CompareManas);   break;
    case PSK_FRAGS:   qsort( _apenPlayers, iPlayers, sizeof(CPlayer*), qsort_CompareFrags);   break;
    case PSK_DEATHS:  qsort( _apenPlayers, iPlayers, sizeof(CPlayer*), qsort_CompareDeaths);  break;
    default:  break;  // invalid or NONE key specified so do nothing
  }

  // all done
  return iPlayers;
}

// ----------------------- drawing functions

// --------------------------------------------------------------------------------------
// Draw border with filter.
// --------------------------------------------------------------------------------------
static void HUD_DrawBorder( FLOAT fCenterX, FLOAT fCenterY, FLOAT fSizeX, FLOAT fSizeY, COLOR colTiles)
{
  if (_pHAC->m_toTile.GetData() == NULL) return;
  if (_pHAC->m_toTile.GetData()->ad_Anims == NULL) return;
  
  // determine location
  const FLOAT fCenterI  = fCenterX * _pixDPWidth  / 640.0f;
  const FLOAT fCenterJ  = fCenterY * _pixDPHeight / (480.0f * _pDP->dp_fWideAdjustment);
  const FLOAT fSizeI    = _fResolutionScaling * fSizeX;
  const FLOAT fSizeJ    = _fResolutionScaling * fSizeY;
  const FLOAT fTileSize = 8 * _fResolutionScaling * _fCustomScaling;

  // determine exact positions
  const FLOAT fLeft  = fCenterI  - fSizeI/2 -1;
  const FLOAT fRight = fCenterI  + fSizeI/2 +1;
  const FLOAT fUp    = fCenterJ  - fSizeJ/2 -1;
  const FLOAT fDown  = fCenterJ  + fSizeJ/2 +1;
  const FLOAT fLeftEnd  = fLeft  + fTileSize;
  const FLOAT fRightBeg = fRight - fTileSize;
  const FLOAT fUpEnd    = fUp    + fTileSize;
  const FLOAT fDownBeg  = fDown  - fTileSize;

  // prepare texture
  colTiles |= _ulAlphaHUD;

  // put corners
  _pDP->InitTexture(&_pHAC->m_toTile, TRUE); // clamping on!
  _pDP->AddTexture(fLeft, fUp,   fLeftEnd, fUpEnd,   colTiles);
  _pDP->AddTexture(fRight,fUp,   fRightBeg,fUpEnd,   colTiles);
  _pDP->AddTexture(fRight,fDown, fRightBeg,fDownBeg, colTiles);
  _pDP->AddTexture(fLeft, fDown, fLeftEnd, fDownBeg, colTiles);

  // put edges
  _pDP->AddTexture(fLeftEnd, fUp,    fRightBeg, fUpEnd,   0.4f, 0.0f, 0.6f, 1.0f, colTiles);
  _pDP->AddTexture(fLeftEnd, fDown,  fRightBeg, fDownBeg, 0.4f, 0.0f, 0.6f, 1.0f, colTiles);
  _pDP->AddTexture(fLeft,    fUpEnd, fLeftEnd,  fDownBeg, 0.0f, 0.4f, 1.0f, 0.6f, colTiles);
  _pDP->AddTexture(fRight,   fUpEnd, fRightBeg, fDownBeg, 0.0f, 0.4f, 1.0f, 0.6f, colTiles);

  // put center
  _pDP->AddTexture(fLeftEnd, fUpEnd, fRightBeg, fDownBeg, 0.4f, 0.4f, 0.6f, 0.6f, colTiles);
  _pDP->FlushRenderingQueue();
}

// --------------------------------------------------------------------------------------
// Draw icon texture (if color = NONE, use colortransitions structure).
// --------------------------------------------------------------------------------------
static void HUD_DrawIcon( FLOAT fCenterX, FLOAT fCenterY, CTextureObject &toIcon,
                          COLOR colDefault, FLOAT fNormValue, BOOL bBlink, PIX pixWidth = -1, PIX pixHeight = -1)
{
  if (toIcon.GetData() == NULL) return;
  if (toIcon.GetData()->ad_Anims == NULL) return;
  
  // determine color
  COLOR col = colDefault;
  if (col==NONE) col = HUD_GetCurrentColor( fNormValue);
  // determine blinking state
  if (bBlink && fNormValue<=(_cttHUD.ctt_fLowMedium/2)) {
    // activate blinking only if value is <= half the low edge
    INDEX iCurrentTime = (INDEX)(_tmNow*4);
    if (iCurrentTime&1) col = C_vdGRAY;
  }

  // determine location
  const FLOAT fCenterI = fCenterX * _pixDPWidth  / 640.0f;
  const FLOAT fCenterJ = fCenterY * _pixDPHeight / (480.0f * _pDP->dp_fWideAdjustment);

  // determine dimensions
  CTextureData *ptd = (CTextureData*)toIcon.GetData();
  
  if (pixWidth <= 0) {
    pixWidth = ptd->GetPixWidth();
  }
  
  if (pixHeight <= 0) {
    pixHeight = ptd->GetPixHeight();
  }
  
  const FLOAT fHalfSizeI = _fResolutionScaling*_fCustomScaling * pixWidth  * 0.5f;
  const FLOAT fHalfSizeJ = _fResolutionScaling*_fCustomScaling * pixHeight * 0.5f;

  // done
  _pDP->InitTexture( &toIcon);
  _pDP->AddTexture( fCenterI-fHalfSizeI, fCenterJ-fHalfSizeJ,
                    fCenterI+fHalfSizeI, fCenterJ+fHalfSizeJ, col|_ulAlphaHUD);
  _pDP->FlushRenderingQueue();
}

// --------------------------------------------------------------------------------------
// Draw text (or numbers, whatever).
// --------------------------------------------------------------------------------------
static void HUD_DrawText( FLOAT fCenterX, FLOAT fCenterY, const CTString &strText,
                          COLOR colDefault, FLOAT fNormValue)
{
  // determine color
  COLOR col = colDefault;
  if (col == NONE) {
    col = HUD_GetCurrentColor( fNormValue);
  }

  // determine location
  PIX pixCenterI = (PIX)(fCenterX*_pixDPWidth  / 640.0f);
  PIX pixCenterJ = (PIX)(fCenterY*_pixDPHeight / (480.0f * _pDP->dp_fWideAdjustment));

  // done
  _pDP->SetTextScaling( _fResolutionScaling * _fCustomScaling);
  _pDP->PutTextCXY( strText, pixCenterI, pixCenterJ, col|_ulAlphaHUD);
}

// --------------------------------------------------------------------------------------
// Draw bar.
// --------------------------------------------------------------------------------------
static void HUD_DrawBar( FLOAT fCenterX, FLOAT fCenterY, PIX pixSizeX, PIX pixSizeY,
                         enum BarOrientations eBarOrientation, COLOR colDefault, FLOAT fNormValue)
{
  // determine color
  COLOR col = colDefault;
  if (col==NONE) col = HUD_GetCurrentColor( fNormValue);

  // determine location and size
  PIX pixCenterI = (PIX)(fCenterX*_pixDPWidth  / 640.0f);
  PIX pixCenterJ = (PIX)(fCenterY*_pixDPHeight / (480.0f * _pDP->dp_fWideAdjustment));
  PIX pixSizeI   = (PIX)(_fResolutionScaling*pixSizeX);
  PIX pixSizeJ   = (PIX)(_fResolutionScaling*pixSizeY);

  // fill bar background area
  PIX pixLeft  = pixCenterI-pixSizeI/2;
  PIX pixUpper = pixCenterJ-pixSizeJ/2;

  // determine bar position and inner size
  switch( eBarOrientation) {
    case BO_UP:
      pixSizeJ *= fNormValue;
      break;
    case BO_DOWN:
      pixUpper  = pixUpper + (PIX)ceil(pixSizeJ * (1.0f-fNormValue));
      pixSizeJ *= fNormValue;
      break;
    case BO_LEFT:
      pixSizeI *= fNormValue;
      break;
    case BO_RIGHT:
      pixLeft   = pixLeft + (PIX)ceil(pixSizeI * (1.0f-fNormValue));
      pixSizeI *= fNormValue;
      break;
  }
  // done
  _pDP->Fill( pixLeft, pixUpper, pixSizeI, pixSizeJ, col|_ulAlphaHUD);
}

// --------------------------------------------------------------------------------------
static void HUD_DrawRotatedQuad( class CTextureObject *_pTO, FLOAT fX, FLOAT fY, FLOAT fSize, ANGLE aAngle, COLOR col)
{
  if (_pTO->GetData() == NULL) return;
  if (_pTO->GetData()->ad_Anims == NULL) return;
  
  FLOAT fSinA = Sin(aAngle);
  FLOAT fCosA = Cos(aAngle);
  FLOAT fSinPCos = fCosA*fSize+fSinA*fSize;
  FLOAT fSinMCos = fSinA*fSize-fCosA*fSize;
  FLOAT fI0, fJ0, fI1, fJ1, fI2, fJ2, fI3, fJ3;

  fI0 = fX-fSinPCos;  fJ0 = fY-fSinMCos;
  fI1 = fX+fSinMCos;  fJ1 = fY-fSinPCos;
  fI2 = fX+fSinPCos;  fJ2 = fY+fSinMCos;
  fI3 = fX-fSinMCos;  fJ3 = fY+fSinPCos;

  _pDP->InitTexture( _pTO);
  _pDP->AddTexture( fI0, fJ0, 0, 0, col,   fI1, fJ1, 0, 1, col,
                    fI2, fJ2, 1, 1, col,   fI3, fJ3, 1, 0, col);
  _pDP->FlushRenderingQueue();

}

// --------------------------------------------------------------------------------------
static void HUD_DrawAspectCorrectTextureCentered( class CTextureObject *_pTO, FLOAT fX, FLOAT fY, FLOAT fWidth, COLOR col)
{
  if (_pTO->GetData() == NULL) return;
  if (_pTO->GetData()->ad_Anims == NULL) return;
  
  CTextureData *ptd = (CTextureData*)_pTO->GetData();
  FLOAT fTexSizeI = ptd->GetPixWidth();
  FLOAT fTexSizeJ = ptd->GetPixHeight();
  FLOAT fHeight = fWidth*fTexSizeJ/fTexSizeJ;

  _pDP->InitTexture( _pTO);
  _pDP->AddTexture( fX-fWidth*0.5f, fY-fHeight*0.5f, fX+fWidth*0.5f, fY+fHeight*0.5f, 0, 0, 1, 1, col);
  _pDP->FlushRenderingQueue();
}

// --------------------------------------------------------------------------------------
// Draw sniper mask.
// --------------------------------------------------------------------------------------
static void HUD_DrawSniperMask( void )
{
  if (_pHAC->m_toSniperMask.GetData() == NULL) return;
  if (_pHAC->m_toSniperMask.GetData()->ad_Anims == NULL) return;
  
  // determine location
  const FLOAT fSizeI = _pixDPWidth;
  const FLOAT fSizeJ = _pixDPHeight;
  const FLOAT fCenterI = fSizeI/2;
  const FLOAT fCenterJ = fSizeJ/2;
  const FLOAT fBlackStrip = (fSizeI-fSizeJ)/2;

  COLOR colMask = C_WHITE|CT_OPAQUE;

  CTextureData *ptd = (CTextureData*)_pHAC->m_toSniperMask.GetData();
  const FLOAT fTexSizeI = ptd->GetPixWidth();
  const FLOAT fTexSizeJ = ptd->GetPixHeight();
  
  // [SSE] Sniper Scope
  {
    hud_fSniperScopeBaseOpacity = Clamp(hud_fSniperScopeBaseOpacity, 0.0f, 1.0f);
    hud_fSniperScopeWheelOpacity = Clamp(hud_fSniperScopeWheelOpacity, 0.0f, 1.0f);
    hud_fSniperScopeLedOpacity = Clamp(hud_fSniperScopeLedOpacity, 0.0f, 1.0f);
    hud_fSniperScopeRangeTextOpacity = Clamp(hud_fSniperScopeRangeTextOpacity, 0.0f, 1.0f);
    hud_fSniperScopeRangeIconOpacity = Clamp(hud_fSniperScopeRangeIconOpacity, 0.0f, 1.0f);
    hud_fSniperScopeZoomTextOpacity = Clamp(hud_fSniperScopeZoomTextOpacity, 0.0f, 1.0f);
    hud_fSniperScopeZoomIconOpacity = Clamp(hud_fSniperScopeZoomIconOpacity, 0.0f, 1.0f);
  }
  
  // main sniper mask
  if (hud_fSniperScopeBaseOpacity > 0.0F) {
    ULONG ulSniperScopeBaseOpacity = NormFloatToByte(hud_fSniperScopeBaseOpacity);

    colMask = C_WHITE|ulSniperScopeBaseOpacity;
  
    _pDP->InitTexture( &_pHAC->m_toSniperMask);

    if (hud_bSniperScopeRenderFromQuarter) 
    {
      _pDP->AddTexture( fBlackStrip, 0, fCenterI, fCenterJ, 0.98f, 0.02f, 0, 1.0f, colMask);
      _pDP->AddTexture( fCenterI, 0, fSizeI-fBlackStrip, fCenterJ, 0, 0.02f, 0.98f, 1.0f, colMask);
      _pDP->AddTexture( fBlackStrip, fCenterJ, fCenterI, fSizeJ, 0.98f, 1.0f, 0, 0.02f, colMask);
      _pDP->AddTexture( fCenterI, fCenterJ, fSizeI-fBlackStrip, fSizeJ, 0, 1, 0.98f, 0.02f, colMask);
    } else {
      _pDP->AddTexture( fBlackStrip, 0, fSizeI-fBlackStrip, fSizeJ, colMask);
    }

    _pDP->FlushRenderingQueue();
    _pDP->Fill( 0, 0, fBlackStrip/*+1*/, fSizeJ, C_BLACK|ulSniperScopeBaseOpacity);
    _pDP->Fill( fSizeI-fBlackStrip/*-1*/, 0, fBlackStrip+1, fSizeJ, C_BLACK|ulSniperScopeBaseOpacity);
  }

  colMask = LerpColor(SE_COL_BLUE_LIGHT, C_WHITE, 0.25f);

  FLOAT _fYResolutionScaling = (FLOAT)_pixDPHeight/480.0f;

  FLOAT fDistance = _penWeapons->m_fRayHitDistance;
  FLOAT aFOV = Lerp(_penWeapons->m_fSniperFOVlast, _penWeapons->m_fSniperFOV,
                    _pTimer->GetLerpFactor());
  CTString strTmp;
  
  FLOAT fZoom = 1.0f/tan(RadAngle(aFOV)*0.5f);  // 2.0 - 8.0

  // wheel
  if (hud_fSniperScopeWheelOpacity > 0.0F) {
    ULONG ulSniperScopeWheelOpacity = NormFloatToByte(hud_fSniperScopeWheelOpacity);

    FLOAT fAFact = (Clamp(aFOV, 14.2f, 53.1f)-14.2f)/(53.1f-14.2f); // only for zooms 2x-4x !!!!!!
    ANGLE aAngle = 314.0f+fAFact*292.0f;
    
    COLOR colWhell = colMask;
    
    if (hud_bSniperScopeWheelColoring && _penWeapons->m_penRayHit != NULL) {
      // if required, show enemy health thru crosshair color
      if (_penWeapons->m_fEnemyHealth > 0) {
        if (_penWeapons->m_fEnemyHealth < 0.25f) {
          colWhell = C_RED;
        } else if (_penWeapons->m_fEnemyHealth < 0.50f) {
          colWhell = C_ORANGE;
        } else if (_penWeapons->m_fEnemyHealth < 0.75f) {
          colWhell = C_YELLOW;
        } else {
          colWhell = C_GREEN;
        }
      }
    }
    
    HUD_DrawRotatedQuad(&_pHAC->m_toSniperWheel, fCenterI, fCenterJ, 40.0f*_fYResolutionScaling, aAngle, colWhell|ulSniperScopeWheelOpacity);
  }

  FLOAT fTM = _pTimer->GetLerpedCurrentTick();

  // reload indicator
  if (hud_fSniperScopeLedOpacity > 0.0F) {
    ULONG ulSniperScopeLedOpacity = NormFloatToByte(hud_fSniperScopeLedOpacity);

    COLOR colLED;

    if (_penWeapons->m_tmLastSniperFire+1.25f < fTM) { // blinking
      colLED = 0x44FF2200;
    } else {
      colLED = 0xFF442200;
    }
    
    HUD_DrawAspectCorrectTextureCentered(&_pHAC->m_toSniperLed, fCenterI-37.0f*_fYResolutionScaling,
      fCenterJ+36.0f*_fYResolutionScaling, 15.0f*_fYResolutionScaling, colLED|ulSniperScopeLedOpacity);
  }

  if (_fResolutionScaling >= 1.0f)
  {
    FLOAT _fIconSize;
    FLOAT _fLeftX,  _fLeftYU,  _fLeftYD;
    FLOAT _fRightX, _fRightYU, _fRightYD;

    if (_fResolutionScaling <= 1.3f) {
      _pDP->SetFont( _pfdConsoleFont);
      _pDP->SetTextAspect( 1.0f);
      _pDP->SetTextScaling(1.0f);
      _fIconSize = 22.8f;
      _fLeftX = 159.0f;
      _fLeftYU = 8.0f;
      _fLeftYD = 6.0f;
      _fRightX = 159.0f;
      _fRightYU = 11.0f;
      _fRightYD = 6.0f;
    } else {
      _pDP->SetFont( _pfdDisplayFont);
      _pDP->SetTextAspect( 1.0f);
      _pDP->SetTextScaling(0.7f*_fYResolutionScaling);
      _fIconSize = 19.0f;
      _fLeftX = 162.0f;
      _fLeftYU = 8.0f;
      _fLeftYD = 6.0f;
      _fRightX = 162.0f;
      _fRightYU = 11.0f;
      _fRightYD = 6.0f;
    }

    // rangefinder icon
    if (hud_fSniperScopeRangeIconOpacity > 0.0F) {
      ULONG ulSniperScopeRangeIconOpacity = NormFloatToByte(hud_fSniperScopeRangeIconOpacity);

      HUD_DrawAspectCorrectTextureCentered(&_pHAC->m_toSniperArrow, fCenterI-_fLeftX*_fYResolutionScaling,
        fCenterJ-_fLeftYU*_fYResolutionScaling, _fIconSize*_fYResolutionScaling, 0xFFCC3300|ulSniperScopeRangeIconOpacity );
    }

    // rangefinder text
    if (hud_fSniperScopeRangeTextOpacity > 0.0F) {
      ULONG ulSniperScopeRangeTextOpacity = NormFloatToByte(hud_fSniperScopeRangeTextOpacity);

      if (fDistance > 9999.9f) {
        strTmp.PrintF("---.-");
      } else {
        strTmp.PrintF("%.1f", fDistance);
      }

      _pDP->PutTextC( strTmp, fCenterI-_fLeftX*_fYResolutionScaling,
        fCenterJ+_fLeftYD*_fYResolutionScaling, colMask|ulSniperScopeRangeTextOpacity);
    }

    // zoom level icon
    if (hud_fSniperScopeZoomIconOpacity > 0.0F) {
      ULONG ulSniperScopeZoomIconOpacity = NormFloatToByte(hud_fSniperScopeZoomIconOpacity);

      HUD_DrawAspectCorrectTextureCentered(&_pHAC->m_toSniperEye,   fCenterI+_fRightX*_fYResolutionScaling,
        fCenterJ-_fRightYU*_fYResolutionScaling, _fIconSize*_fYResolutionScaling, 0xFFCC3300|ulSniperScopeZoomIconOpacity ); //SE_COL_ORANGE_L
    }

    // zoom level text
    if (hud_fSniperScopeZoomTextOpacity > 0.0F) {
      ULONG ulSniperScopeZoomTextOpacity = NormFloatToByte(hud_fSniperScopeZoomTextOpacity);

      strTmp.PrintF("%.1fx", fZoom);
      _pDP->PutTextC( strTmp, fCenterI+_fRightX*_fYResolutionScaling,
        fCenterJ+_fRightYD*_fYResolutionScaling, colMask|ulSniperScopeZoomTextOpacity);
    }
  }
}


// helper functions

// --------------------------------------------------------------------------------------
// Fill weapon and ammo table with current state.
// --------------------------------------------------------------------------------------
static void HUD_FillWeaponAmmoTables(void)
{
  // ammo quantities
  _aaiAmmo[0].ai_iAmmoAmmount    = _penWeapons->m_iShells;
  _aaiAmmo[0].ai_iMaxAmmoAmmount = _penWeapons->m_iMaxShells;
  _aaiAmmo[1].ai_iAmmoAmmount    = _penWeapons->m_iBullets;
  _aaiAmmo[1].ai_iMaxAmmoAmmount = _penWeapons->m_iMaxBullets;
  _aaiAmmo[2].ai_iAmmoAmmount    = _penWeapons->m_iRockets;
  _aaiAmmo[2].ai_iMaxAmmoAmmount = _penWeapons->m_iMaxRockets;
  _aaiAmmo[3].ai_iAmmoAmmount    = _penWeapons->m_iGrenades;
  _aaiAmmo[3].ai_iMaxAmmoAmmount = _penWeapons->m_iMaxGrenades;
  _aaiAmmo[4].ai_iAmmoAmmount    = _penWeapons->m_iNapalm;
  _aaiAmmo[4].ai_iMaxAmmoAmmount = _penWeapons->m_iMaxNapalm;
  _aaiAmmo[5].ai_iAmmoAmmount    = _penWeapons->m_iElectricity;
  _aaiAmmo[5].ai_iMaxAmmoAmmount = _penWeapons->m_iMaxElectricity;
  _aaiAmmo[6].ai_iAmmoAmmount    = _penWeapons->m_iIronBalls;
  _aaiAmmo[6].ai_iMaxAmmoAmmount = _penWeapons->m_iMaxIronBalls;
  _aaiAmmo[7].ai_iAmmoAmmount    = _penWeapons->m_iSniperBullets;
  _aaiAmmo[7].ai_iMaxAmmoAmmount = _penWeapons->m_iMaxSniperBullets;
  
  _aaiAmmo[8].ai_iAmmoAmmount    = _penWeapons->m_iSeriousBombs;
  _aaiAmmo[8].ai_iMaxAmmoAmmount = _penWeapons->m_iMaxSeriousBombs;

  // prepare ammo table for weapon possesion
  INDEX i, iAvailableWeapons = _penWeapons->m_iAvailableWeapons;
  for (i = 0; i < 8; i++) _aaiAmmo[i].ai_bHasWeapon = FALSE;

  // weapon possesion
  for (i = WEAPON_NONE + 1; i < WEAPON_LAST; i++)
  {
    if (_awiWeapons[i].wi_wtWeapon == WEAPON_NONE) {
      continue;
    }

    // regular weapons
    _awiWeapons[i].wi_bHasWeapon = _penWeapons->HasWeapon(_awiWeapons[i].wi_wtWeapon);

    if (_awiWeapons[i].wi_paiAmmo != NULL) {
      _awiWeapons[i].wi_paiAmmo->ai_bHasWeapon |= _awiWeapons[i].wi_bHasWeapon;
    }
  }
}


//<<<<<<< DEBUG FUNCTIONS >>>>>>>

//<<<<<<< DEBUG FUNCTIONS >>>>>>>



// --------------------------------------------------------------------------------------
// main

// render interface (frontend) to drawport
// (units are in pixels for 640x480 resolution - for other res HUD will be scalled automatically)
// --------------------------------------------------------------------------------------
extern void DrawOldHUD(const CPlayer *penPlayerCurrent, CDrawPort *pdpCurrent, BOOL bSnooping, const CPlayer *penPlayerOwner);
extern void DrawHybridHUD(const CPlayer *penPlayerCurrent, CDrawPort *pdpCurrent, BOOL bSnooping, const CPlayer *penPlayerOwner);
extern void DrawNewHUD( const CPlayer *penPlayerCurrent, CDrawPort *pdpCurrent, BOOL bSnooping, const CPlayer *penPlayerOwner);

extern void DrawHUD( const CPlayer *penPlayerCurrent, CDrawPort *pdpCurrent, BOOL bSnooping, const CPlayer *penPlayerOwner)
{
  hud_iHUDType = Clamp(hud_iHUDType, INDEX(0), INDEX(2));
  
  if (hud_iHUDType == 2) {
    DrawNewHUD(penPlayerCurrent, pdpCurrent, bSnooping, penPlayerOwner);
  } else if (hud_iHUDType == 1) {
    DrawHybridHUD(penPlayerCurrent, pdpCurrent, bSnooping, penPlayerOwner);
  } else {
    DrawOldHUD(penPlayerCurrent, pdpCurrent, bSnooping, penPlayerOwner);
  }
}

extern void DrawNewHUD( const CPlayer *penPlayerCurrent, CDrawPort *pdpCurrent, BOOL bSnooping, const CPlayer *penPlayerOwner)
{
  // no player - no info, sorry
  if (penPlayerCurrent == NULL || (penPlayerCurrent->GetFlags()&ENF_DELETED)) return;
  
  if (!_bHUDFontsLoaded) return;

  // if snooping and owner player ins NULL, return
  if (bSnooping && penPlayerOwner == NULL) return;

  // find last values in case of predictor
  CPlayer *penLast = (CPlayer*)penPlayerCurrent;
  if (penPlayerCurrent->IsPredictor()) penLast = (CPlayer*)(((CPlayer*)penPlayerCurrent)->GetPredicted());
  ASSERT(penLast != NULL);
  if (penLast == NULL) return; // !!!! just in case

  // cache local variables
  hud_fOpacity = Clamp( hud_fOpacity, 0.1f, 1.0f);
  hud_fScaling = Clamp( hud_fScaling, 0.5f, 1.2f);
  
  _penPlayer  = penPlayerCurrent;
  _penWeapons = (CPlayerWeapons*)&*_penPlayer->m_penWeapons;
  _pDP        = pdpCurrent;
  _pixDPWidth   = _pDP->GetWidth();
  _pixDPHeight  = _pDP->GetHeight();

  _fCustomScaling     = hud_fScaling;
  _fResolutionScaling = (FLOAT)_pixDPWidth / 640.0f;
  _colHUD     = HUD_GetInterfaceColor(penPlayerOwner);
  _colHUDText = SE_COL_ORANGE_LIGHT;
  _ulAlphaHUD = NormFloatToByte(hud_fOpacity);
  _tmNow = _pTimer->CurrentTick();

  // determine hud colorization;
  COLOR colMax = SE_COL_BLUEGREEN_LT;
  COLOR colTop = SE_COL_ORANGE_LIGHT;
  COLOR colMid = LerpColor(colTop, C_RED, 0.5f);
  COLOR colDefault = NONE;
  
  INDEX iCurrentWeapon = _penWeapons->m_iCurrentWeapon;
  INDEX iWantedWeapon  = _penWeapons->m_iWantedWeapon;
  
  const BOOL bSinglePlay  =  GetSP()->sp_bSinglePlayer;
  const BOOL bCooperative =  GetSP()->sp_bCooperative && !bSinglePlay;
  const BOOL bScoreMatch  = !GetSP()->sp_bCooperative && !GetSP()->sp_bUseFrags;
  const BOOL bFragMatch   = !GetSP()->sp_bCooperative &&  GetSP()->sp_bUseFrags;
  const BOOL bTeamDeathMatch = (GetSP()->sp_ulGameModeFlags & GMF_TEAMPLAY) && GetSP()->sp_bUseFrags; // [SSE] Team DeathMatch
  const BOOL bSharedLives = GetSP()->sp_bSharedLives;
  const BOOL bSharedKeys = GetSP()->sp_bSharedKeys;

  const INDEX ctTeams = GetSP()->sp_ctTeams;
  
  FLOAT fMoverX, fMoverY;

  _ulBrAlpha = NormFloatToByte(hud_fOpacity * 0.5F);

  // draw sniper mask (original mask even if snooping)
  if (((CPlayerWeapons*)&*penPlayerOwner->m_penWeapons)->m_iCurrentWeapon == WEAPON_SNIPER && ((CPlayerWeapons*)&*penPlayerOwner->m_penWeapons)->m_bSniping && hud_bSniperScopeDraw)
  {
    HUD_DrawSniperMask();
  }

  FLOAT fValue = 0.0F;

  _pfdDisplayFont->SetVariableWidth();
  _pDP->SetFont( &_fdNumbersFont);
  _pDP->SetTextCharSpacing(1);

  // Health
  {
    fValue = Clamp(ceil(_penPlayer->GetHealth()), 0.0f, 999.0F);
    
    HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
    colDefault = HUD_AddShaker( 5, fValue, penLast->m_iLastHealth, penLast->m_tmHealthChanged, fMoverX, fMoverY);

    HUD_DrawAnchoredRect(8, 8, 32, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect(44, 8, 80, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon(8, 8, 32, 32, EHHAT_LEFT, EHVAT_BOT, _pHAC->m_toHealth, C_WHITE|CT_OPAQUE, fValue / TOP_HEALTH, FALSE);

    HUD_DrawAnchoredRectOutline( 8, 8, 32, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(44, 8, 80, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);

    CTString strHealth;
    strHealth.PrintF("%.0f", fValue);
    HUD_DrawAnchoredTextInRect(FLOAT2D(44, 8), FLOAT2D(80, 32), EHHAT_LEFT, EHVAT_BOT, strHealth, colDefault, fValue / TOP_HEALTH);
  }
  
  PIX pixShieldsVO = 0;

  // Armor
  if (_penPlayer->m_fArmor > 0.0F) {
    pixShieldsVO = 40;

    fValue = Clamp(ceil(_penPlayer->m_fArmor), 0.0f, 999.0F);
    CTextureObject *ptoCurrentArmor = NULL;
      
    if (fValue <= 50.5f) {
      ptoCurrentArmor = &_pHAC->m_toArmorSmall;
    } else if (fValue<=100.5f) {
      ptoCurrentArmor = &_pHAC->m_toArmorMedium;
    } else {
      ptoCurrentArmor = &_pHAC->m_toArmorLarge;
    }
    
    HUD_PrepareColorTransitions( colMax, colTop, colMid, C_lGRAY, 0.5f, 0.25f, FALSE);
    colDefault = HUD_AddShaker( 5, fValue, penLast->m_iLastArmor, penLast->m_tmArmorChanged, fMoverX, fMoverY);

    HUD_DrawAnchoredRect( 8, 48, 32, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect(44, 48, 80, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon(8, 48, 32, 32, EHHAT_LEFT, EHVAT_BOT, *ptoCurrentArmor, C_WHITE|CT_OPAQUE, fValue / TOP_ARMOR, FALSE); // Icon

    HUD_DrawAnchoredRectOutline( 8, 48, 32, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(44, 48, 80, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
    
    CTString strArmor;
    strArmor.PrintF("%.0f", fValue);
    HUD_DrawAnchoredTextInRect(FLOAT2D(44, 48), FLOAT2D(80, 32), EHHAT_LEFT, EHVAT_BOT, strArmor, colDefault, fValue / TOP_ARMOR);
  }
  
  // Shields
  if (_penPlayer->m_fShields > 0.0F) {
    fValue = Clamp(ceil(_penPlayer->m_fShields), 0.0f, 999.0F);
    CTextureObject *ptoCurrentArmor = &_pHAC->m_toShields;
    
    HUD_PrepareColorTransitions( colMax, colTop, colMid, C_lGRAY, 0.5f, 0.25f, FALSE);
    //colDefault = HUD_AddShaker( 5, fValue, penLast->m_iLastArmor, penLast->m_tmArmorChanged, fMoverX, fMoverY);

    HUD_DrawAnchoredRect( 8, 48 + pixShieldsVO, 32, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect(44, 48 + pixShieldsVO, 80, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon(8, 48 + pixShieldsVO, 32, 32, EHHAT_LEFT, EHVAT_BOT, *ptoCurrentArmor, C_WHITE|CT_OPAQUE, fValue / TOP_ARMOR, FALSE); // Icon
    HUD_DrawAnchroredIcon(8, 48 + pixShieldsVO, 32, 32, EHHAT_LEFT, EHVAT_BOT, _pHAC->m_toShieldsTop, C_GREEN|CT_OPAQUE, fValue / TOP_ARMOR, FALSE); // Icon

    HUD_DrawAnchoredRectOutline( 8, 48 + pixShieldsVO, 32, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(44, 48 + pixShieldsVO, 80, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
    
    CTString strShields;
    strShields.PrintF("%.0f", fValue);
    HUD_DrawAnchoredTextInRect(FLOAT2D(44, 48 + pixShieldsVO), FLOAT2D(80, 32), EHHAT_LEFT, EHVAT_BOT, strShields, colDefault, fValue / TOP_ARMOR);
  }
  
  // Current Weapon and its Ammo
  {
    CTextureObject *ptoCurrentWeapon = NULL;
    ptoCurrentWeapon = _awiWeapons[iCurrentWeapon].wi_ptoWeapon;
    
    if (ptoCurrentWeapon != NULL)
    {
      HUD_DrawAnchoredRect (-60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon(-60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, *ptoCurrentWeapon, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
      HUD_DrawAnchoredRectOutline(-60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, _colHUD|_ulAlphaHUD);
    }

    CTextureObject *ptoCurrentAmmo = NULL;
    BOOL bAnyColt = iCurrentWeapon == WEAPON_COLT || iCurrentWeapon == WEAPON_DOUBLECOLT;

    AmmoInfo *paiCurrent = _awiWeapons[iCurrentWeapon].wi_paiAmmo;
    if (paiCurrent != NULL) ptoCurrentAmmo = paiCurrent->ai_ptoAmmo;

    if (bAnyColt || (ptoCurrentAmmo != NULL && !GetSP()->sp_bInfiniteAmmo))
    {
      FLOAT fMaxValue = _penWeapons->GetMaxAmmo();
      fValue = _penWeapons->GetAmmo();

      CTString strCurrentAmmo;
      strCurrentAmmo.PrintF( "%d", (SLONG)ceil(fValue));
      HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.30f, 0.15f, FALSE);
        
      if (bAnyColt) {
        ptoCurrentAmmo = &_pHAC->m_toAColt;
      }
      
      HUD_DrawAnchoredRect(  0 - 32, 8, 80, 32, EHHAT_CENTER, EHVAT_BOT, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchoredRectOutline(  0 - 32, 8, 80, 32, EHHAT_CENTER, EHVAT_BOT, _colHUD|_ulAlphaHUD);
      HUD_DrawAnchoredTextInRect(FLOAT2D(0 - 32, 8), FLOAT2D(80, 32), EHHAT_CENTER, EHVAT_BOT, strCurrentAmmo, NONE, fValue / fMaxValue);
      
      if (ptoCurrentAmmo != NULL)
      {
        HUD_DrawAnchoredRect (60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, C_BLACK|_ulBrAlpha);
        HUD_DrawAnchroredIcon(60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, *ptoCurrentAmmo, C_WHITE|CT_OPAQUE, fValue / fMaxValue, TRUE); // Icon
        HUD_DrawAnchoredRectOutline(60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, _colHUD|_ulAlphaHUD);
      }
    }
  }
  
  HUD_FillWeaponAmmoTables();
  
  // if weapon change is in progress
  hud_tmWeaponsOnScreen = Clamp( hud_tmWeaponsOnScreen, 0.0f, 10.0f);
  if ((_tmNow - _penWeapons->m_tmWeaponChangeRequired) < hud_tmWeaponsOnScreen)
  {
    CTextureObject *ptoWantedWeapon = NULL;
    ptoWantedWeapon = _awiWeapons[iWantedWeapon].wi_ptoWeapon;
    
    INDEX i;
    
    // determine number of weapons that player has
    INDEX ctWeapons = 0;
    for (i = WEAPON_NONE + 1; i < WEAPON_LAST; i++)
    {
      if (_awiWeapons[i].wi_wtWeapon!=WEAPON_NONE && _awiWeapons[i].wi_wtWeapon!=WEAPON_DOUBLECOLT && _awiWeapons[i].wi_bHasWeapon)
      {
        ctWeapons++;
      }
    }
    
    FLOAT fHorOffset = (ctWeapons * (32 + 4) - 4) / 2.0F - 16;
    fHorOffset = -fHorOffset;
    
    // display all available weapons
    for (INDEX ii = WEAPON_NONE+1; ii < WEAPON_LAST; ii++)
    {
      i = aiWeaponsRemap[ii];

      // skip if hasn't got this weapon
      if (_awiWeapons[i].wi_wtWeapon==WEAPON_NONE || _awiWeapons[i].wi_wtWeapon==WEAPON_DOUBLECOLT || !_awiWeapons[i].wi_bHasWeapon) continue;

      COLOR colBorder = _colHUD;
      COLOR colIcon = 0xccddff00;
      
      // weapon that is currently selected has different colors
      if (ptoWantedWeapon == _awiWeapons[i].wi_ptoWeapon)
      {
        colIcon = 0xffcc0000;
        colBorder = 0xffcc0000;
      }
      
      if (_awiWeapons[i].wi_paiAmmo != NULL && _awiWeapons[i].wi_paiAmmo->ai_iAmmoAmmount == 0)
      {
        colBorder = 0x22334400;
        colIcon = 0x22334400;
      }

      HUD_DrawAnchoredRect (fHorOffset, 120, 32, 32, EHHAT_CENTER, EHVAT_MID, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon(fHorOffset, 120, 32, 32, EHHAT_CENTER, EHVAT_MID, *_awiWeapons[i].wi_ptoWeapon, colIcon|CT_OPAQUE, 1.0F, FALSE); // Icon
      HUD_DrawAnchoredRectOutline(fHorOffset, 120, 32, 32, EHHAT_CENTER, EHVAT_MID, colBorder|_ulAlphaHUD);
      fHorOffset += 32 + 4;
    }
  }
  
  INDEX iScore = _penPlayer->m_psGameStats.ps_iScore;
  INDEX iMana  = _penPlayer->m_iMana;
  
  if (bFragMatch) {
    iScore = _penPlayer->m_psGameStats.ps_iKills;
    iMana  = _penPlayer->m_psGameStats.ps_iDeaths;

  } else if (bCooperative) {
    INDEX iScoreSum = 0;
    
    INDEX ctMaxPlayers = CEntity::GetMaxPlayers();
    
    for (INDEX iPlayer = 0; iPlayer < ctMaxPlayers; iPlayer++)
    {
      CEntity *penEntity = CEntity::GetPlayerEntity(iPlayer);
      
      // If player is invalid then skip him.
      if (penEntity == NULL) {
        continue;
      }
      
      iScoreSum += static_cast<CPlayer*>(penEntity)->m_psGameStats.ps_iScore;
    }

    iScore = iScoreSum; // in case of coop play, show squad (common) score
  }
  
  #define agafasga
  #ifdef agafasga
  // LSC Up - Score / Frags
  {
    CTString strScore;
    strScore.PrintF("%d", iScore);
    
    HUD_DrawAnchoredRect ( 8,  8,  16, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect (28,  8, 104, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( 8,  8,  16, 16, EHHAT_LEFT, EHVAT_TOP, _pHAC->m_toFrags, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
    
    HUD_DrawAnchoredRectOutline( 8, 8,  16, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(28, 8, 104, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredTextInRect (FLOAT2D(28, 8), FLOAT2D(104, 16), EHHAT_LEFT, EHVAT_TOP, strScore, C_lGRAY, 1.0F);
  }

  // LSC Down - SP/Coop = Extra Lives
  if (bCooperative && GetSP()->sp_ctCredits >= 0)
  {
    CTString strLives;
    strLives.PrintF( "%d", bSharedLives ? GetSP()->sp_ctCreditsLeft : _penPlayer->m_iLives);

    HUD_DrawAnchoredRect ( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect (28, 28, 104, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, _pHAC->m_toExtraLive, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
    
    HUD_DrawAnchoredRectOutline( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(28, 28, 104, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredTextInRect (FLOAT2D(28, 28), FLOAT2D(104, 16), EHHAT_LEFT, EHVAT_TOP, strLives, C_lGRAY, 1.0F);
  }
  
  // LSC Down - DM/SM = Death/Mana
  if (bFragMatch || bScoreMatch)
  {
    CTString strMana;
    strMana.PrintF( "%d", iMana);

    HUD_DrawAnchoredRect ( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect (28, 28, 104, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, _pHAC->m_toDeaths, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
    
    HUD_DrawAnchoredRectOutline( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(28, 28, 104, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredTextInRect (FLOAT2D(28, 28), FLOAT2D(104, 16), EHHAT_LEFT, EHVAT_TOP, strMana, C_lGRAY, 1.0F);
  }
  
  // set font and prepare font parameters
  _pfdDisplayFont->SetFixedWidth();
  _pDP->SetFont( _pfdDisplayFont);
  _pDP->SetTextLineSpacing(0);
  
  // Time Limit
  if ((bScoreMatch || bFragMatch) && hud_bShowMatchInfo && GetSP()->sp_iTimeLimit > 0)
  {
    FLOAT fTimeLeft = ClampDn(GetSP()->sp_iTimeLimit*60.0f - _pNetwork->GetGameTime(), 0.0f);
    
    CTString strTime;
    strTime.PrintF( "%s", TimeToString(fTimeLeft));

    HUD_DrawAnchoredRect ( 8, 48,  16, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect (28, 48, 104, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( 8, 48,  16, 16, EHHAT_LEFT, EHVAT_TOP, _pHAC->m_toClock, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
    
    HUD_DrawAnchoredRectOutline( 8, 48,  16, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(28, 48, 104, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredTextInRect (FLOAT2D(28, 48 + 2), FLOAT2D(104, 16), EHHAT_LEFT, EHVAT_TOP, strTime, C_WHITE|_ulAlphaHUD, 1.0F, 0.75F);
  }
  
  _pfdDisplayFont->SetVariableWidth();
  _pDP->SetFont( &_fdNumbersFont);
  _pDP->SetTextCharSpacing(1);
  #endif
  
  // [SSE] Radar
  #ifdef RADARAMA
  {
    HUD_DrawAnchroredIcon( 8, 8,  128, 128, EHHAT_LEFT, EHVAT_TOP, _pHAC->m_toRadarCircle, C_WHITE|_ulBrAlpha, 1.0F, FALSE);
    HUD_DrawAnchroredIcon( 8, 8,  128, 128, EHHAT_LEFT, EHVAT_TOP, _pHAC->m_toRadarMask,   _colHUD|_ulAlphaHUD, 1.0F, FALSE);
    HUD_DrawAnchroredIcon( 8, 8,  128, 128, EHHAT_LEFT, EHVAT_TOP, _pHAC->m_toRadarBorder, _colHUD|_ulAlphaHUD, 1.0F, FALSE);
    
    CPlacement3D plView = _penPlayer->en_plViewpoint;
    ANGLE3D a3dPlayerLookDir = ANGLE3D(((CEntity&)*_penPlayer).GetPlacement().pl_OrientationAngle(1) + plView.pl_OrientationAngle(1),0,0);
    CPlacement3D plPlayer(((CEntity&)*_penPlayer).GetPlacement().pl_PositionVector, a3dPlayerLookDir);
    
    FOREACHINDYNAMICCONTAINER(((CEntity&)*_penPlayer).GetWorld()->wo_cenEntities, CEntity, iten)
    {
      CEntity *pen = iten;
      
      if (pen == NULL) continue;
      if (pen->IsDead()) continue;
      
      if (IsDerivedFromClass(pen, "Enemy Base"))
      {
        CEnemyBase *penEB = static_cast<CEnemyBase*>(pen);
        
        if (penEB->m_bTemplate) continue;
        
        CPlacement3D plEnemy;
        plEnemy.pl_PositionVector = pen->GetPlacement().pl_PositionVector;
        plEnemy.pl_OrientationAngle = FLOAT3D(0, 0, 0);
        plEnemy.AbsoluteToRelativeSmooth(plPlayer);
        
				FLOAT3D vEnemyFlat = plEnemy.pl_PositionVector;
				vEnemyFlat(2) = 0;
        
        #define RRAD 50.0F
        #define RHDIFF 5.0F
        
				if (plEnemy.pl_PositionVector.Length() > RRAD) 
				{
          vEnemyFlat = vEnemyFlat.Normalize() * RRAD;
				}
        
        if (plEnemy.pl_PositionVector.Length() < 200) {
          CTextureObject *ptoDot = &_pHAC->m_toRadarDot;
          
          if (Abs(plEnemy.pl_PositionVector(2)) > RHDIFF) {
            if (plEnemy.pl_PositionVector(2) > 0) {
              ptoDot = &_pHAC->m_toRadarDotUp;
            } else if (plEnemy.pl_PositionVector(2) < 0) {
              ptoDot = &_pHAC->m_toRadarDotDown;
            }
          }

          HUD_DrawAnchroredIcon( 8 + 64 + vEnemyFlat(1) - 4, 8 + 64 + vEnemyFlat(3) - 4,  8, 8, EHHAT_LEFT, EHVAT_TOP, *ptoDot, C_RED|_ulAlphaHUD, 1.0F, FALSE);
        }
      }
    }
  }
  #endif
  
  // CSC Up
  if (bSinglePlay || bCooperative)
  {
    CTString strHiScore;
    strHiScore.PrintF( "%d", Max(_penPlayer->m_iHighScore, _penPlayer->m_psGameStats.ps_iScore));
    BOOL bBeating = _penPlayer->m_psGameStats.ps_iScore > _penPlayer->m_iHighScore;
    
    HUD_DrawAnchoredRect ( -54, 8,  16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( -54, 8,  16, 16, EHHAT_CENTER, EHVAT_TOP, _pHAC->m_toHiScore, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
    HUD_DrawAnchoredRect (  10, 8, 104, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    
    HUD_DrawAnchoredRectOutline(-54, 8,  16, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline( 10, 8, 104, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    
    HUD_DrawAnchoredTextInRect (FLOAT2D(10, 8), FLOAT2D(104, 16), EHHAT_CENTER, EHVAT_TOP, strHiScore, NONE, bBeating ? 0.0f : 1.0f);
  }

  // TeamPlay Frags
  if (bTeamDeathMatch)
  {
    CTString astrTeamScore[4];
    const COLOR aColTeams[] = { C_lBLUE, C_lRED, C_lGREEN, C_lYELLOW };
    INDEX aiTeamOrder[] = { 1, 2, 3, 4 };

    COLOR col = _colHUD;
    COLOR colIcon = C_WHITE;
    
    const INDEX iTeamID = _penPlayer->m_iTeamID;
    
    // If team id is in range 1-4 then we can set the color.
    if (iTeamID > 0 && iTeamID < 5) {
      colIcon = aColTeams[iTeamID - 1];
    }
    
    astrTeamScore[0].PrintF("%d", GetSP()->sp_iTeamScore1);
    astrTeamScore[1].PrintF("%d", GetSP()->sp_iTeamScore2);
    astrTeamScore[2].PrintF("%d", GetSP()->sp_iTeamScore3);
    astrTeamScore[3].PrintF("%d", GetSP()->sp_iTeamScore4);
    
    switch (iTeamID)
    {
      case 2: {
        aiTeamOrder[0] = 2;
        aiTeamOrder[1] = 1;
      } break;

      case 3: {
        aiTeamOrder[0] = 3;
        aiTeamOrder[1] = 1;
        aiTeamOrder[2] = 2;
        aiTeamOrder[3] = 4;
      } break;

      case 4: {
        aiTeamOrder[0] = 4;
        aiTeamOrder[1] = 1;
        aiTeamOrder[2] = 2;
        aiTeamOrder[3] = 3;
      } break;
    }

    PIX pixIconBoxShift = 0;
    PIX pixThirdTeamPosX = -38;
    
    // If 3 teams then put box into the center.
    if (ctTeams == 3) {
      pixThirdTeamPosX = 0;
    }
    
    // First Team
    HUD_DrawAnchoredRect (-38, 8, 52, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRectOutline(-38, 8, 52, 16, EHHAT_CENTER, EHVAT_TOP, col|_ulAlphaHUD);
    HUD_DrawAnchoredTextInRect (FLOAT2D(-38, 8), FLOAT2D(52, 16), EHHAT_CENTER, EHVAT_TOP, astrTeamScore[aiTeamOrder[0] - 1], aColTeams[aiTeamOrder[0] - 1]|_ulAlphaHUD, 1.0F);

    // Second Team
    HUD_DrawAnchoredRect (38, 8, 52, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRectOutline(38, 8, 52, 16, EHHAT_CENTER, EHVAT_TOP, col|_ulAlphaHUD);
    HUD_DrawAnchoredTextInRect (FLOAT2D(38, 8), FLOAT2D(52, 16), EHHAT_CENTER, EHVAT_TOP, astrTeamScore[aiTeamOrder[1] - 1], aColTeams[aiTeamOrder[1] - 1]|_ulAlphaHUD, 1.0F);
    
    // Third Team
    if (ctTeams >= 3) {
      HUD_DrawAnchoredRect (pixThirdTeamPosX, 28, 52, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchoredRectOutline(pixThirdTeamPosX, 28, 52, 16, EHHAT_CENTER, EHVAT_TOP, col|_ulAlphaHUD);
      HUD_DrawAnchoredTextInRect (FLOAT2D(pixThirdTeamPosX, 28), FLOAT2D(52, 16), EHHAT_CENTER, EHVAT_TOP, astrTeamScore[aiTeamOrder[2] - 1], aColTeams[aiTeamOrder[2] - 1]|_ulAlphaHUD, 1.0F);
    }

    // Fourth Team
    if (ctTeams >= 4) {
      HUD_DrawAnchoredRect (38, 28, 52, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchoredRectOutline(38, 28, 52, 16, EHHAT_CENTER, EHVAT_TOP, col|_ulAlphaHUD);
      HUD_DrawAnchoredTextInRect (FLOAT2D(38, 28), FLOAT2D(52, 16), EHHAT_CENTER, EHVAT_TOP, astrTeamScore[aiTeamOrder[3] - 1], aColTeams[aiTeamOrder[3] - 1]|_ulAlphaHUD, 1.0F);
      
      pixIconBoxShift += 10;
    }

    // Swords Icon Box
    HUD_DrawAnchoredRect ( 0, 8 + pixIconBoxShift, 16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( 0, 8 + pixIconBoxShift, 16, 16, EHHAT_CENTER, EHVAT_TOP, _pHAC->m_toSwords, colIcon|CT_OPAQUE, 0.0F, FALSE); // Icon
    HUD_DrawAnchoredRectOutline(0, 8 + pixIconBoxShift, 16, 16, EHHAT_CENTER, EHVAT_TOP, col|_ulAlphaHUD);
  }

  // CSC Down
  //HUD_DrawAnchoredRect(-53, 28, 16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  //HUD_DrawAnchoredRect(  9, 28, 104, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  
  // BossBar
  if (_penPlayer->m_penMainMusicHolder != NULL)
  {
    FLOAT fNormValue = 0.0F;
    
    CMusicHolder &mh = (CMusicHolder&)*_penPlayer->m_penMainMusicHolder;
    fNormValue = 0;

    if (mh.m_penBoss != NULL && (mh.m_penBoss->en_ulFlags&ENF_ALIVE)) {
      CEnemyBase &eb = (CEnemyBase&)*mh.m_penBoss;
      ASSERT( eb.m_fMaxHealth>0);
      fValue = eb.GetHealth();
      fNormValue = fValue/eb.m_fMaxHealth;
    }
    
    CTextureObject *ptoBossIcon = &_pHAC->m_toHealth;

    // Q6785
    // [SSE] HudBosBarDisplay Entity
    if (mh.m_penBossBarDisplay != NULL) {
      CHudBossBarDisplay &bbd = (CHudBossBarDisplay&)*mh.m_penBossBarDisplay;
      
      if (bbd.m_iValueIO > 0) {
        fValue = bbd.m_iValueIO;
        fNormValue = fValue / bbd.m_iMaxValue;
      }
      
      if (bbd.m_moTextureHolder.mo_toTexture.GetData() != NULL) {
        ptoBossIcon = &bbd.m_moTextureHolder.mo_toTexture;
      }
      
    } else if (mh.m_penCounter != NULL) {
      CEnemyCounter &ec = (CEnemyCounter&)*mh.m_penCounter;

      if (ec.m_iCount > 0) {
        fValue = ec.m_iCount;
        fNormValue = fValue / ec.m_iCountFrom;
      }
    }
    
    fNormValue = Clamp(fNormValue, 0.0F, 1.0F);

    if (fNormValue > 0)
    {
      HUD_PrepareColorTransitions( colMax, colMax, colTop, C_RED, 0.5f, 0.25f, FALSE); // prepare and draw boss energy info

      HUD_DrawAnchoredRect( -130, 48, 16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon(-130, 48, 16, 16, EHHAT_CENTER, EHVAT_TOP, *ptoBossIcon, C_WHITE|CT_OPAQUE, 1.0F, FALSE);
      HUD_DrawAnchoredRect(   10, 48, 256, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      
      HUD_DrawAnchoredBar(10, 48, 256, 16, EHHAT_CENTER, EHVAT_TOP, BO_LEFT, NONE, fNormValue);

      HUD_DrawAnchoredRectOutline(-130, 48,  16, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
      HUD_DrawAnchoredRectOutline(  10, 48, 256, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    }
  }

  const TIME tmMaxHoldBreath = _penPlayer->en_tmMaxHoldBreath;
  
  // Oxygen
  if (tmMaxHoldBreath > 0.0F)
  {
    FLOAT fValue = tmMaxHoldBreath - (_pTimer->CurrentTick() - _penPlayer->en_tmLastBreathed);

    if (_penPlayer->IsConnected() && (_penPlayer->GetFlags()&ENF_ALIVE) && fValue < (tmMaxHoldBreath - 1.0F))
    {
      HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
      FLOAT fNormValue = fValue / tmMaxHoldBreath;
      fNormValue = ClampDn(fNormValue, 0.0f);
      
      HUD_DrawAnchoredRect (-34, 68, 16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchoredRect ( 10, 68, 64, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon(-34, 68, 16, 16, EHHAT_CENTER, EHVAT_TOP, _pHAC->m_toOxygen, C_WHITE|CT_OPAQUE, fNormValue, TRUE); // Icon
      
      HUD_DrawAnchoredBar(10, 68, 64, 16, EHHAT_CENTER, EHVAT_TOP, BO_LEFT, NONE, fNormValue);
      
      HUD_DrawAnchoredRectOutline(-34, 68, 16, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
      HUD_DrawAnchoredRectOutline( 10, 68, 64, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    }
  }
  
  BOOL bMessagesBoxDrawn = FALSE;
  
  // Right Top - Messages
  if (bSinglePlay || bCooperative)
  {
    // prepare and draw unread messages
    if (hud_bShowMessages && _penPlayer->m_ctUnreadMessages > 0)
    {
      bMessagesBoxDrawn = TRUE;
      
      CTString strMessages;
      strMessages.PrintF( "%d", _penPlayer->m_ctUnreadMessages);

      const FLOAT tmIn = 0.5f;
      const FLOAT tmOut = 0.5f;
      const FLOAT tmStay = 1.0f;
      FLOAT tmDelta = _pTimer->GetLerpedCurrentTick()-_penPlayer->m_tmAnimateInbox;
      COLOR col = _colHUD;

      if (tmDelta > 0 && tmDelta < (tmIn + tmStay + tmOut) && bSinglePlay)
      {
        FLOAT fRatio = 0.0f;

        if (tmDelta < tmIn) {
          fRatio = ClampUp(tmDelta / tmIn, 1.0F);
        } else if (tmDelta>tmIn+tmStay) {
          fRatio = (tmIn+tmStay+tmOut-tmDelta) / tmOut;
        } else {
          fRatio = 1.0f;
        }

        col = LerpColor(_colHUD, C_WHITE|0xFF, fRatio);
      }
      
      HUD_DrawAnchoredRect ( 8, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchoredRect (28, 8, 52, 16, EHHAT_RIGHT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon( 8, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, _pHAC->m_toMessage, C_WHITE|CT_OPAQUE, 0.0F, TRUE); // Icon
      
      HUD_DrawAnchoredRectOutline( 8, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, col|_ulAlphaHUD);
      HUD_DrawAnchoredRectOutline(28, 8, 52, 16, EHHAT_RIGHT, EHVAT_TOP, col|_ulAlphaHUD);
      
      HUD_DrawAnchoredTextInRect (FLOAT2D(28, 8), FLOAT2D(52, 16), EHHAT_RIGHT, EHVAT_TOP, strMessages, col|_ulAlphaHUD, 1.0F);
    }
  }
  
  BOOL bHasAnyKey = bSharedKeys ? GetSP()->sp_ulPickedKeys : _penPlayer->m_ulKeys;
  
  // Right Top - Keys
  if (bHasAnyKey) {
    PIX pixShiftX = 0;
    
    if (bMessagesBoxDrawn) {
      pixShiftX = 76;
    }

    COLOR col = _colHUD;

    CTString strKeys;
    strKeys.PrintF("%d", bSharedKeys ? bit_count(GetSP()->sp_ulPickedKeys) : bit_count(_penPlayer->m_ulKeys));
    
    HUD_DrawAnchoredRect ( 8 + pixShiftX, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect (28 + pixShiftX, 8, 52, 16, EHHAT_RIGHT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( 8 + pixShiftX, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, _pHAC->m_toKey, C_WHITE|CT_OPAQUE, 0.0F, FALSE); // Icon
    
    HUD_DrawAnchoredRectOutline( 8 + pixShiftX, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, col|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(28 + pixShiftX, 8, 52, 16, EHHAT_RIGHT, EHVAT_TOP, col|_ulAlphaHUD);
    
    HUD_DrawAnchoredTextInRect (FLOAT2D(28 + pixShiftX, 8), FLOAT2D(52, 16), EHHAT_RIGHT, EHVAT_TOP, strKeys, C_lGRAY|_ulAlphaHUD, 1.0F);
  }
  
  // PowerUps dock.
  {
    HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.66f, 0.33f, FALSE);
    
    TIME *ptmPowerups = (TIME*)&_penPlayer->m_tmInvisibility;
    TIME *ptmPowerupsMax = (TIME*)&_penPlayer->m_tmInvisibilityMax;
	
    INDEX iCol = 0;

    for (INDEX i = 0; i < 4; i++)
    {
      INDEX ii = 3 - i;
      
      // skip if not active
      const TIME tmDelta = ptmPowerups[ii] - _tmNow;
      if (tmDelta <= 0) continue;

      FLOAT fNormValue = Clamp(tmDelta, 0.0F, ptmPowerupsMax[ii]) / ptmPowerupsMax[ii];
      HUD_DrawAnchoredRect (8 + (24 + 3) * iCol, 48, 24, 24, EHHAT_RIGHT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon(8 + (24 + 3) * iCol, 48, 24, 24, EHHAT_RIGHT, EHVAT_BOT, _pHAC->m_atoPowerups[ii], C_WHITE|CT_OPAQUE, fNormValue, TRUE); // Icon
      
      HUD_DrawAnchoredBar(8 + (24 + 3) * iCol, 48, 4, 24, EHHAT_RIGHT, EHVAT_BOT, BO_DOWN, NONE, fNormValue);
      
      HUD_DrawAnchoredRectOutline(8 + (24 + 3) * iCol, 48, 24, 24, EHHAT_RIGHT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
	  
      // Play sound if icon is flashing.
      if (fNormValue<=(_cttHUD.ctt_fLowMedium/2))
      {
        // activate blinking only if value is <= half the low edge
        INDEX iLastTime = (INDEX)(_tmLast * 4);
        INDEX iCurrentTime = (INDEX)(_tmNow * 4);

        if (iCurrentTime&1 & !(iLastTime&1)) {
          ((CPlayer *)penPlayerCurrent)->PlayPowerUpSound();
        }
      }

      iCol++;
    }
  }

  // Ammo dock.
  if (!GetSP()->sp_bInfiniteAmmo)
  {
    HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
    
    FLOAT fNormValue = 0.0F;
    
    INDEX iCol = 0;

    for (INDEX i = 8; i >= 0; i--)
    {
      CTextureObject *ptoAmmo = &_pHAC->m_atoAmmo[8]; // TODO: Magic number here.

      INDEX ii = 8 - i;
      
      COLOR colIcon = C_WHITE;
      COLOR colBorder = _colHUD;
      COLOR colBar = NONE;

      if (i < 8)
      {
        ii = aiAmmoRemap[i];
        
        AmmoInfo &ai = _aaiAmmo[ii];
        
        // if no ammo and hasn't got that weapon - just skip this ammo // [SSE] Or if you don't want to see empty ammo types.
        if (ai.ai_iAmmoAmmount <= 0 && (!ai.ai_bHasWeapon || !hud_bShowEmptyAmmoInList)) continue;

        if (ai.ai_iAmmoAmmount <= 0) colIcon = C_mdGRAY;
        
        fNormValue = (FLOAT)ai.ai_iAmmoAmmount / ai.ai_iMaxAmmoAmmount;
        
        ptoAmmo = ai.ai_ptoAmmo;

      } else {
        INDEX iBombCount = _penWeapons->m_iSeriousBombs;
        
        #define BOMB_FIRE_TIME 1.5f
        BOOL  bBombFiring = penPlayerCurrent->m_tmSeriousBombFired + BOMB_FIRE_TIME > _pTimer->GetLerpedCurrentTick();
        
        if (bBombFiring) {
          iBombCount++;
          iBombCount = ClampUp(iBombCount, INDEX(3));
          
          FLOAT fFactor = (_pTimer->GetLerpedCurrentTick() - penPlayerCurrent->m_tmSeriousBombFired) / BOMB_FIRE_TIME;
          colBorder = LerpColor(colBorder, C_RED, fFactor);
          colIcon = LerpColor(colIcon, C_RED, fFactor);
          colBar = LerpColor(colBar, C_RED, fFactor) | _ulAlphaHUD;
        }

        fNormValue = iBombCount / 3.0F;

        if (fNormValue <= 0) {
          continue;
        }
      }

      HUD_DrawAnchoredRect( 8 + (24 + 3) * iCol, 8, 24, 24, EHHAT_RIGHT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon( 8 + (24 + 3) * iCol, 8, 24, 24, EHHAT_RIGHT, EHVAT_BOT, *ptoAmmo, colIcon|CT_OPAQUE, 1.0F, TRUE); // Icon
      
      HUD_DrawAnchoredBar( 8 + (24 + 3) * iCol, 8, 4, 24, EHHAT_RIGHT, EHVAT_BOT, BO_DOWN, colBar, fNormValue);
      
      HUD_DrawAnchoredRectOutline(8 + (24 + 3) * iCol, 8, 24, 24, EHHAT_RIGHT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
      
      iCol++;
    }
  }
  
  // in the end, remember the current time so it can be used in the next frame
  _tmLast = _tmNow;

  if (hud_bGameDebugMonitor)
  {
    HUD_DrawDebugMonitor();
  }
}

extern void DrawHybridHUD(const CPlayer *penPlayerCurrent, CDrawPort *pdpCurrent, BOOL bSnooping, const CPlayer *penPlayerOwner)
{
  // no player - no info, sorry
  if (penPlayerCurrent == NULL || (penPlayerCurrent->GetFlags()&ENF_DELETED)) return;
  
  if (!_bHUDFontsLoaded) return;

  // if snooping and owner player ins NULL, return
  if (bSnooping && penPlayerOwner == NULL) return;

  // find last values in case of predictor
  CPlayer *penLast = (CPlayer*)penPlayerCurrent;
  if (penPlayerCurrent->IsPredictor()) penLast = (CPlayer*)(((CPlayer*)penPlayerCurrent)->GetPredicted());
  ASSERT(penLast != NULL);
  if (penLast == NULL) return; // !!!! just in case

  // cache local variables
  hud_fOpacity = Clamp( hud_fOpacity, 0.1f, 1.0f);
  hud_fScaling = Clamp( hud_fScaling, 0.5f, 1.2f);
  
  _penPlayer  = penPlayerCurrent;
  _penWeapons = (CPlayerWeapons*)&*_penPlayer->m_penWeapons;
  _pDP        = pdpCurrent;
  _pixDPWidth   = _pDP->GetWidth();
  _pixDPHeight  = _pDP->GetHeight();

  _fCustomScaling     = hud_fScaling;
  _fResolutionScaling = (FLOAT)_pixDPWidth / 640.0f;
  _colHUD     = HUD_GetInterfaceColor(penPlayerOwner);
  _colHUDText = SE_COL_ORANGE_LIGHT;
  _ulAlphaHUD = NormFloatToByte(hud_fOpacity);
  _ulBrAlpha = NormFloatToByte(hud_fOpacity * 0.5F);
  _tmNow = _pTimer->CurrentTick();
  
  INDEX iCurrentWeapon = _penWeapons->m_iCurrentWeapon;
  INDEX iWantedWeapon  = _penWeapons->m_iWantedWeapon;
  
  const BOOL bTeamDeathMatch = (GetSP()->sp_ulGameModeFlags & GMF_TEAMPLAY) && GetSP()->sp_bUseFrags; // [SSE] Team DeathMatch
  const BOOL bSharedLives = GetSP()->sp_bSharedLives;
  const BOOL bSharedKeys = GetSP()->sp_bSharedKeys;
  
  const INDEX ctTeams = GetSP()->sp_ctTeams;
  
  FLOAT fMoverX, fMoverY;

  // determine hud colorization;
  COLOR colMax = SE_COL_BLUEGREEN_LT;
  COLOR colTop = SE_COL_ORANGE_LIGHT;
  COLOR colMid = LerpColor(colTop, C_RED, 0.5f);
  COLOR colDefault = NONE;
  
  // adjust borders color in case of spying mode
  COLOR colBorder = _colHUD;

  if (bSnooping) {
    colBorder = SE_COL_ORANGE_NEUTRAL;
    if (((ULONG)(_tmNow*5))&1) {
      //colBorder = (colBorder>>1) & 0x7F7F7F00; // darken flash and scale
      colBorder = SE_COL_ORANGE_DARK;
      _fCustomScaling *= 0.933f;
    }
  }

  // draw sniper mask (original mask even if snooping)
  if (((CPlayerWeapons*)&*penPlayerOwner->m_penWeapons)->m_iCurrentWeapon == WEAPON_SNIPER && ((CPlayerWeapons*)&*penPlayerOwner->m_penWeapons)->m_bSniping && hud_bSniperScopeDraw)
  {
    HUD_DrawSniperMask();
  }

  // prepare font and text dimensions
  CTString strValue;
  PIX pixCharWidth;
  _pDP->SetFont( &_fdNumbersFont);
  pixCharWidth = _fdNumbersFont.GetWidth() + _fdNumbersFont.GetCharSpacing() +1;
  FLOAT fChrUnit = pixCharWidth * _fCustomScaling;

  const PIX pixTopBound    = 6;
  const PIX pixLeftBound   = 6;
  const PIX pixBottomBound = (480 * _pDP->dp_fWideAdjustment) -pixTopBound;
  const PIX pixRightBound  = 640-pixLeftBound;

  FLOAT fOneUnit  = (32+0) * _fCustomScaling;  // unit size
  FLOAT fAdvUnit  = (32+4) * _fCustomScaling;  // unit advancer
  FLOAT fNextUnit = (32+8) * _fCustomScaling;  // unit advancer
  FLOAT fHalfUnit = fOneUnit * 0.5f;

  FLOAT fValue = 0.0F;

  // Health
  {
    fValue = Clamp(ceil(_penPlayer->GetHealth()), 0.0f, 999.0F);
    
    HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
    colDefault = HUD_AddShaker( 5, fValue, penLast->m_iLastHealth, penLast->m_tmHealthChanged, fMoverX, fMoverY);

    HUD_DrawAnchoredRect(8, 8, 32, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect(44, 8, 80, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon(8, 8, 32, 32, EHHAT_LEFT, EHVAT_BOT, _pHAC->m_toHealth, C_WHITE|CT_OPAQUE, fValue / TOP_HEALTH, FALSE);

    HUD_DrawAnchoredRectOutline( 8, 8, 32, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(44, 8, 80, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);

    CTString strHealth;
    strHealth.PrintF("%.0f", fValue);
    HUD_DrawAnchoredTextInRect(FLOAT2D(44, 8), FLOAT2D(80, 32), EHHAT_LEFT, EHVAT_BOT, strHealth, colDefault, fValue / TOP_HEALTH);
  }

  PIX pixShieldsVO = 0;

  // Armor
  if (_penPlayer->m_fArmor > 0.0F) {
    pixShieldsVO = 40;

    fValue = Clamp(ceil(_penPlayer->m_fArmor), 0.0f, 999.0F);
    CTextureObject *ptoCurrentArmor = NULL;
      
    if (fValue <= 50.5f) {
      ptoCurrentArmor = &_pHAC->m_toArmorSmall;
    } else if (fValue<=100.5f) {
      ptoCurrentArmor = &_pHAC->m_toArmorMedium;
    } else {
      ptoCurrentArmor = &_pHAC->m_toArmorLarge;
    }
    
    HUD_PrepareColorTransitions( colMax, colTop, colMid, C_lGRAY, 0.5f, 0.25f, FALSE);
    colDefault = HUD_AddShaker( 5, fValue, penLast->m_iLastArmor, penLast->m_tmArmorChanged, fMoverX, fMoverY);

    HUD_DrawAnchoredRect( 8, 48, 32, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect(44, 48, 80, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon(8, 48, 32, 32, EHHAT_LEFT, EHVAT_BOT, *ptoCurrentArmor, C_WHITE|CT_OPAQUE, fValue / TOP_ARMOR, FALSE); // Icon

    HUD_DrawAnchoredRectOutline( 8, 48, 32, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(44, 48, 80, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
    
    CTString strArmor;
    strArmor.PrintF("%.0f", fValue);
    HUD_DrawAnchoredTextInRect(FLOAT2D(44, 48), FLOAT2D(80, 32), EHHAT_LEFT, EHVAT_BOT, strArmor, colDefault, fValue / TOP_ARMOR);
  }
  
  // Shields
  if (_penPlayer->m_fShields > 0.0F) {
    fValue = Clamp(ceil(_penPlayer->m_fShields), 0.0f, 999.0F);
    CTextureObject *ptoCurrentArmor = &_pHAC->m_toShields;
    
    HUD_PrepareColorTransitions( colMax, colTop, colMid, C_lGRAY, 0.5f, 0.25f, FALSE);
    //colDefault = HUD_AddShaker( 5, fValue, penLast->m_iLastArmor, penLast->m_tmArmorChanged, fMoverX, fMoverY);

    HUD_DrawAnchoredRect( 8, 48 + pixShieldsVO, 32, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect(44, 48 + pixShieldsVO, 80, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon(8, 48 + pixShieldsVO, 32, 32, EHHAT_LEFT, EHVAT_BOT, *ptoCurrentArmor, C_WHITE|CT_OPAQUE, fValue / TOP_ARMOR, FALSE); // Icon

    HUD_DrawAnchoredRectOutline( 8, 48 + pixShieldsVO, 32, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(44, 48 + pixShieldsVO, 80, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
    
    CTString strShields;
    strShields.PrintF("%.0f", fValue);
    HUD_DrawAnchoredTextInRect(FLOAT2D(44, 48 + pixShieldsVO), FLOAT2D(80, 32), EHHAT_LEFT, EHVAT_BOT, strShields, colDefault, fValue / TOP_ARMOR);
  }

  CTextureObject *ptoCurrentAmmo = NULL;
  
  // Current Weapon and its Ammo
  {
    CTextureObject *ptoCurrentWeapon = NULL;
    ptoCurrentWeapon = _awiWeapons[iCurrentWeapon].wi_ptoWeapon;
    
    if (ptoCurrentWeapon != NULL)
    {
      HUD_DrawAnchoredRect (-60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon(-60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, *ptoCurrentWeapon, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
      HUD_DrawAnchoredRectOutline(-60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, _colHUD|_ulAlphaHUD);
    }

    
    BOOL bAnyColt = iCurrentWeapon == WEAPON_COLT || iCurrentWeapon == WEAPON_DOUBLECOLT;
    BOOL bSeriousBomb = iCurrentWeapon == WEAPON_SERIOUSBOMB;

    AmmoInfo *paiCurrent = _awiWeapons[iCurrentWeapon].wi_paiAmmo;
    if (paiCurrent != NULL) ptoCurrentAmmo = paiCurrent->ai_ptoAmmo;

    if (bAnyColt || bSeriousBomb || (ptoCurrentAmmo != NULL && !GetSP()->sp_bInfiniteAmmo))
    {
      FLOAT fMaxValue = _penWeapons->GetMaxAmmo();
      fValue = _penWeapons->GetAmmo();

      CTString strCurrentAmmo;
      strCurrentAmmo.PrintF( "%d", (SLONG)ceil(fValue));
      HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.30f, 0.15f, FALSE);
      
      // [SSE] Colt Bullets
      if (bAnyColt) {
        ptoCurrentAmmo = &_pHAC->m_toAColt;
      }
      
      HUD_DrawAnchoredRect(  0 - 32, 8, 80, 32, EHHAT_CENTER, EHVAT_BOT, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchoredRectOutline(  0 - 32, 8, 80, 32, EHHAT_CENTER, EHVAT_BOT, _colHUD|_ulAlphaHUD);
      HUD_DrawAnchoredTextInRect(FLOAT2D(0 - 32, 8), FLOAT2D(80, 32), EHHAT_CENTER, EHVAT_BOT, strCurrentAmmo, NONE, fValue / fMaxValue);
      
      if (ptoCurrentAmmo != NULL)
      {
        HUD_DrawAnchoredRect (60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, C_BLACK|_ulBrAlpha);
        HUD_DrawAnchroredIcon(60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, *ptoCurrentAmmo, C_WHITE|CT_OPAQUE, fValue / fMaxValue, TRUE); // Icon
        HUD_DrawAnchoredRectOutline(60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, _colHUD|_ulAlphaHUD);
      }
    }
  }

  CTextureObject *ptoWantedWeapon = NULL;
  ptoWantedWeapon  = _awiWeapons[iWantedWeapon].wi_ptoWeapon;
  
  HUD_FillWeaponAmmoTables();

  // Ammo dock.
  if (!GetSP()->sp_bInfiniteAmmo)
  {
    HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
    
    FLOAT fNormValue = 0.0F;
    
    INDEX iCol = 0;

    for (INDEX i = 8; i >= 0; i--)
    {
      CTextureObject *ptoAmmo = &_pHAC->m_atoAmmo[8]; // TODO: Magic number here.
      
      INDEX ii = 8 - i;
      
      COLOR colIcon = C_WHITE;
      COLOR colBorder = _colHUD;
      COLOR colBar = NONE;

      if (i < 8)
      {
        ii = aiAmmoRemap[i];

        AmmoInfo &ai = _aaiAmmo[ii];
        
        // if no ammo and hasn't got that weapon - just skip this ammo // [SSE] Or if you don't want to see empty ammo types.
        if (ai.ai_iAmmoAmmount <= 0 && (!ai.ai_bHasWeapon || !hud_bShowEmptyAmmoInList)) continue;

        if (ai.ai_iAmmoAmmount <= 0) colIcon = C_mdGRAY;
        
        fNormValue = (FLOAT)ai.ai_iAmmoAmmount / ai.ai_iMaxAmmoAmmount;
        
        ptoAmmo = ai.ai_ptoAmmo;

      } else {
        INDEX iBombCount = _penWeapons->m_iSeriousBombs;
        
        #define BOMB_FIRE_TIME 1.5f
        BOOL  bBombFiring = penPlayerCurrent->m_tmSeriousBombFired + BOMB_FIRE_TIME > _pTimer->GetLerpedCurrentTick();
        
        if (bBombFiring) {
          iBombCount++;
          iBombCount = ClampUp(iBombCount, INDEX(3));
          
          FLOAT fFactor = (_pTimer->GetLerpedCurrentTick() - penPlayerCurrent->m_tmSeriousBombFired) / BOMB_FIRE_TIME;
          colBorder = LerpColor(colBorder, C_RED, fFactor);
          colIcon = LerpColor(colIcon, C_RED, fFactor);
          colBar = LerpColor(colBar, C_RED, fFactor) | _ulAlphaHUD;
        }

        fNormValue = iBombCount / 3.0F;

        if (fNormValue <= 0) {
          continue;
        }
      }

      HUD_DrawAnchoredRect( 8 + (24 + 3) * iCol, 8, 24, 24, EHHAT_RIGHT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon( 8 + (24 + 3) * iCol, 8, 24, 24, EHHAT_RIGHT, EHVAT_BOT, *ptoAmmo, colIcon|CT_OPAQUE, 1.0F, TRUE); // Icon
      
      HUD_DrawAnchoredBar( 8 + (24 + 3) * iCol, 8, 4, 24, EHHAT_RIGHT, EHVAT_BOT, BO_DOWN, colBar, fNormValue);
      
      HUD_DrawAnchoredRectOutline(8 + (24 + 3) * iCol, 8, 24, 24, EHHAT_RIGHT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
      
      iCol++;
    }
  }

  // PowerUps dock.
  {
    HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.66f, 0.33f, FALSE);
    
    TIME *ptmPowerups = (TIME*)&_penPlayer->m_tmInvisibility;
    TIME *ptmPowerupsMax = (TIME*)&_penPlayer->m_tmInvisibilityMax;
	
    INDEX iCol = 0;

    for (INDEX i = 0; i < 4; i++)
    {
      INDEX ii = 3 - i;
      
      // skip if not active
      const TIME tmDelta = ptmPowerups[ii] - _tmNow;
      if (tmDelta <= 0) continue;

      FLOAT fNormValue = Clamp(tmDelta, 0.0F, ptmPowerupsMax[ii]) / ptmPowerupsMax[ii];
      HUD_DrawAnchoredRect (8 + (24 + 3) * iCol, 48, 24, 24, EHHAT_RIGHT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon(8 + (24 + 3) * iCol, 48, 24, 24, EHHAT_RIGHT, EHVAT_BOT, _pHAC->m_atoPowerups[ii], C_WHITE|CT_OPAQUE, fNormValue, TRUE); // Icon
      
      HUD_DrawAnchoredBar(8 + (24 + 3) * iCol, 48, 4, 24, EHHAT_RIGHT, EHVAT_BOT, BO_DOWN, NONE, fNormValue);
      
      HUD_DrawAnchoredRectOutline(8 + (24 + 3) * iCol, 48, 24, 24, EHHAT_RIGHT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
	  
      // Play sound if icon is flashing.
      if (fNormValue<=(_cttHUD.ctt_fLowMedium/2))
      {
        // activate blinking only if value is <= half the low edge
        INDEX iLastTime = (INDEX)(_tmLast * 4);
        INDEX iCurrentTime = (INDEX)(_tmNow * 4);

        if (iCurrentTime&1 & !(iLastTime&1)) {
          ((CPlayer *)penPlayerCurrent)->PlayPowerUpSound();
        }
      }

      iCol++;
    }
  }

  // if weapon change is in progress
  _fCustomScaling = hud_fScaling;
  // if weapon change is in progress
  hud_tmWeaponsOnScreen = Clamp( hud_tmWeaponsOnScreen, 0.0f, 10.0f);
  if ((_tmNow - _penWeapons->m_tmWeaponChangeRequired) < hud_tmWeaponsOnScreen)
  {
    CTextureObject *ptoWantedWeapon = NULL;
    ptoWantedWeapon = _awiWeapons[iWantedWeapon].wi_ptoWeapon;
    
    INDEX i;
    
    // determine number of weapons that player has
    INDEX ctWeapons = 0;
    for (i = WEAPON_NONE + 1; i < WEAPON_LAST; i++)
    {
      if (_awiWeapons[i].wi_wtWeapon!=WEAPON_NONE && _awiWeapons[i].wi_wtWeapon!=WEAPON_DOUBLECOLT && _awiWeapons[i].wi_bHasWeapon)
      {
        ctWeapons++;
      }
    }
    
    FLOAT fHorOffset = (ctWeapons * (32 + 4) - 4) / 2.0F - 16;
    fHorOffset = -fHorOffset;
    
    // display all available weapons
    for (INDEX ii = WEAPON_NONE+1; ii < WEAPON_LAST; ii++)
    {
      i = aiWeaponsRemap[ii];

      // skip if hasn't got this weapon
      if (_awiWeapons[i].wi_wtWeapon==WEAPON_NONE || _awiWeapons[i].wi_wtWeapon==WEAPON_DOUBLECOLT || !_awiWeapons[i].wi_bHasWeapon) continue;

      COLOR colBorder = _colHUD;
      COLOR colIcon = 0xccddff00;
      
      // weapon that is currently selected has different colors
      if (ptoWantedWeapon == _awiWeapons[i].wi_ptoWeapon)
      {
        colIcon = 0xffcc0000;
        colBorder = 0xffcc0000;
      }
      
      if (_awiWeapons[i].wi_paiAmmo != NULL && _awiWeapons[i].wi_paiAmmo->ai_iAmmoAmmount == 0)
      {
        colBorder = 0x22334400;
        colIcon = 0x22334400;
      }

      HUD_DrawAnchoredRect (fHorOffset, 120, 32, 32, EHHAT_CENTER, EHVAT_MID, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon(fHorOffset, 120, 32, 32, EHHAT_CENTER, EHVAT_MID, *_awiWeapons[i].wi_ptoWeapon, colIcon|CT_OPAQUE, 1.0F, FALSE); // Icon
      HUD_DrawAnchoredRectOutline(fHorOffset, 120, 32, 32, EHHAT_CENTER, EHVAT_MID, colBorder|_ulAlphaHUD);
      fHorOffset += 32 + 4;
    }
  }
  
  // TeamPlay Frags
  if (bTeamDeathMatch)
  {
    CTString astrTeamScore[4];
    const COLOR aColTeams[] = { C_lBLUE, C_lRED, C_lGREEN, C_lYELLOW };
    INDEX aiTeamOrder[] = { 1, 2, 3, 4 };

    COLOR col = _colHUD;
    COLOR colIcon = C_WHITE;
    
    const INDEX iTeamID = _penPlayer->m_iTeamID;
    
    // If team id is in range 1-4 then we can set the color.
    if (iTeamID > 0 && iTeamID < 5) {
      colIcon = aColTeams[iTeamID - 1];
    }
    
    astrTeamScore[0].PrintF("%d", GetSP()->sp_iTeamScore1);
    astrTeamScore[1].PrintF("%d", GetSP()->sp_iTeamScore2);
    astrTeamScore[2].PrintF("%d", GetSP()->sp_iTeamScore3);
    astrTeamScore[3].PrintF("%d", GetSP()->sp_iTeamScore4);
    
    switch (iTeamID)
    {
      case 2: {
        aiTeamOrder[0] = 2;
        aiTeamOrder[1] = 1;
      } break;

      case 3: {
        aiTeamOrder[0] = 3;
        aiTeamOrder[1] = 1;
        aiTeamOrder[2] = 2;
        aiTeamOrder[3] = 4;
      } break;

      case 4: {
        aiTeamOrder[0] = 4;
        aiTeamOrder[1] = 1;
        aiTeamOrder[2] = 2;
        aiTeamOrder[3] = 3;
      } break;
    }

    PIX pixIconBoxShift = 0;
    PIX pixThirdTeamPosX = -38;
    
    // If 3 teams then put box into the center.
    if (ctTeams == 3) {
      pixThirdTeamPosX = 0;
    }
    
    // First Team
    HUD_DrawAnchoredRect (-38, 8, 52, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRectOutline(-38, 8, 52, 16, EHHAT_CENTER, EHVAT_TOP, col|_ulAlphaHUD);
    HUD_DrawAnchoredTextInRect (FLOAT2D(-38, 8), FLOAT2D(52, 16), EHHAT_CENTER, EHVAT_TOP, astrTeamScore[aiTeamOrder[0] - 1], aColTeams[aiTeamOrder[0] - 1]|_ulAlphaHUD, 1.0F);

    // Second Team
    HUD_DrawAnchoredRect (38, 8, 52, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRectOutline(38, 8, 52, 16, EHHAT_CENTER, EHVAT_TOP, col|_ulAlphaHUD);
    HUD_DrawAnchoredTextInRect (FLOAT2D(38, 8), FLOAT2D(52, 16), EHHAT_CENTER, EHVAT_TOP, astrTeamScore[aiTeamOrder[1] - 1], aColTeams[aiTeamOrder[1] - 1]|_ulAlphaHUD, 1.0F);
    
    // Third Team
    if (ctTeams >= 3) {
      HUD_DrawAnchoredRect (pixThirdTeamPosX, 28, 52, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchoredRectOutline(pixThirdTeamPosX, 28, 52, 16, EHHAT_CENTER, EHVAT_TOP, col|_ulAlphaHUD);
      HUD_DrawAnchoredTextInRect (FLOAT2D(pixThirdTeamPosX, 28), FLOAT2D(52, 16), EHHAT_CENTER, EHVAT_TOP, astrTeamScore[aiTeamOrder[2] - 1], aColTeams[aiTeamOrder[2] - 1]|_ulAlphaHUD, 1.0F);
    }

    // Fourth Team
    if (ctTeams >= 4) {
      HUD_DrawAnchoredRect (38, 28, 52, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchoredRectOutline(38, 28, 52, 16, EHHAT_CENTER, EHVAT_TOP, col|_ulAlphaHUD);
      HUD_DrawAnchoredTextInRect (FLOAT2D(38, 28), FLOAT2D(52, 16), EHHAT_CENTER, EHVAT_TOP, astrTeamScore[aiTeamOrder[3] - 1], aColTeams[aiTeamOrder[3] - 1]|_ulAlphaHUD, 1.0F);
      
      pixIconBoxShift += 10;
    }

    // Swords Icon Box
    HUD_DrawAnchoredRect ( 0, 8 + pixIconBoxShift, 16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( 0, 8 + pixIconBoxShift, 16, 16, EHHAT_CENTER, EHVAT_TOP, _pHAC->m_toSwords, colIcon|CT_OPAQUE, 0.0F, FALSE); // Icon
    HUD_DrawAnchoredRectOutline(0, 8 + pixIconBoxShift, 16, 16, EHHAT_CENTER, EHVAT_TOP, col|_ulAlphaHUD);
  }
  
  // BossBar
  if (_penPlayer->m_penMainMusicHolder != NULL)
  {
    FLOAT fNormValue = 0.0F;
    
    CMusicHolder &mh = (CMusicHolder&)*_penPlayer->m_penMainMusicHolder;
    fNormValue = 0;

    if (mh.m_penBoss != NULL && (mh.m_penBoss->en_ulFlags&ENF_ALIVE)) {
      CEnemyBase &eb = (CEnemyBase&)*mh.m_penBoss;
      ASSERT( eb.m_fMaxHealth>0);
      fValue = eb.GetHealth();
      fNormValue = fValue/eb.m_fMaxHealth;
    }
    
    CTextureObject *ptoBossIcon = &_pHAC->m_toHealth;

    // Q6785
    // [SSE] HudBosBarDisplay Entity
    if (mh.m_penBossBarDisplay != NULL) {
      CHudBossBarDisplay &bbd = (CHudBossBarDisplay&)*mh.m_penBossBarDisplay;
      
      if (bbd.m_iValueIO > 0) {
        fValue = bbd.m_iValueIO;
        fNormValue = fValue / bbd.m_iMaxValue;
      }
      
      if (bbd.m_moTextureHolder.mo_toTexture.GetData() != NULL) {
        ptoBossIcon = &bbd.m_moTextureHolder.mo_toTexture;
      }
      
    } else if (mh.m_penCounter != NULL) {
      CEnemyCounter &ec = (CEnemyCounter&)*mh.m_penCounter;

      if (ec.m_iCount > 0) {
        fValue = ec.m_iCount;
        fNormValue = fValue / ec.m_iCountFrom;
      }
    }
    
    fNormValue = Clamp(fNormValue, 0.0F, 1.0F);

    if (fNormValue > 0)
    {
      HUD_PrepareColorTransitions( colMax, colMax, colTop, C_RED, 0.5f, 0.25f, FALSE); // prepare and draw boss energy info

      HUD_DrawAnchoredRect( -130, 48, 16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon(-130, 48, 16, 16, EHHAT_CENTER, EHVAT_TOP, *ptoBossIcon, C_WHITE|CT_OPAQUE, 1.0F, FALSE);
      HUD_DrawAnchoredRect(   10, 48, 256, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      
      HUD_DrawAnchoredBar(10, 48, 256, 16, EHHAT_CENTER, EHVAT_TOP, BO_LEFT, NONE, fNormValue);

      HUD_DrawAnchoredRectOutline(-130, 48,  16, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
      HUD_DrawAnchoredRectOutline(  10, 48, 256, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    }
  }

  const TIME tmMaxHoldBreath = _penPlayer->en_tmMaxHoldBreath;
  
  // Oxygen
  if (tmMaxHoldBreath > 0.0F)
  {
    FLOAT fValue = tmMaxHoldBreath - (_pTimer->CurrentTick() - _penPlayer->en_tmLastBreathed);

    if (_penPlayer->IsConnected() && (_penPlayer->GetFlags()&ENF_ALIVE) && fValue < (tmMaxHoldBreath - 1.0F))
    {
      HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
      FLOAT fNormValue = fValue / tmMaxHoldBreath;
      fNormValue = ClampDn(fNormValue, 0.0f);
      
      HUD_DrawAnchoredRect (-34, 68, 16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchoredRect ( 10, 68, 64, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon(-34, 68, 16, 16, EHHAT_CENTER, EHVAT_TOP, _pHAC->m_toOxygen, C_WHITE|CT_OPAQUE, fNormValue, TRUE); // Icon
      
      HUD_DrawAnchoredBar(10, 68, 64, 16, EHHAT_CENTER, EHVAT_TOP, BO_LEFT, NONE, fNormValue);
      
      HUD_DrawAnchoredRectOutline(-34, 68, 16, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
      HUD_DrawAnchoredRectOutline( 10, 68, 64, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    }
  }

  // determine scaling of normal text and play mode
  const FLOAT fTextScale  = (_fResolutionScaling+1) *0.5f;
  const BOOL bSinglePlay  =  GetSP()->sp_bSinglePlayer;
  const BOOL bCooperative =  GetSP()->sp_bCooperative && !bSinglePlay;
  const BOOL bScoreMatch  = !GetSP()->sp_bCooperative && !GetSP()->sp_bUseFrags;
  const BOOL bFragMatch   = !GetSP()->sp_bCooperative &&  GetSP()->sp_bUseFrags;
  COLOR colMana, colFrags, colDeaths, colHealth, colArmor, colPing;
  COLOR colScore  = _colHUD;
  INDEX iScoreSum = 0;

  // if not in single player mode, we'll have to calc (and maybe printout) other players' info
  if (!bSinglePlay)
  {
    // set font and prepare font parameters
    _pfdDisplayFont->SetVariableWidth();
    _pDP->SetFont( _pfdDisplayFont);
    _pDP->SetTextScaling( fTextScale);
    FLOAT fCharHeight = (_pfdDisplayFont->GetHeight()-2)*fTextScale;
    // generate and sort by mana list of active players
    BOOL bMaxScore=TRUE, bMaxMana=TRUE, bMaxFrags=TRUE, bMaxDeaths=TRUE;
    hud_iSortPlayers = Clamp( hud_iSortPlayers, -1L, 6L);
    SortKeys eKey = (SortKeys)hud_iSortPlayers;
    if (hud_iSortPlayers==-1) {
           if (bCooperative) eKey = PSK_HEALTH;
      else if (bScoreMatch)  eKey = PSK_SCORE;
      else if (bFragMatch)   eKey = PSK_FRAGS;
      else { ASSERT(FALSE);  eKey = PSK_NAME; }
    }
    if (bCooperative) eKey = (SortKeys)Clamp( (INDEX)eKey, 0L, 3L);
    if (eKey==PSK_HEALTH && (bScoreMatch || bFragMatch)) { eKey = PSK_NAME; }; // prevent health snooping in deathmatch
    INDEX iPlayers = HUD_SetAllPlayersStats(eKey);

    // loop thru players
    for (INDEX i=0; i<iPlayers; i++)
    {
      // get player name and mana
      CPlayer *penPlayer = _apenPlayers[i];
      const CTString strName = penPlayer->GetPlayerName();
      const INDEX iScore  = penPlayer->m_psGameStats.ps_iScore;
      const INDEX iMana   = penPlayer->m_iMana;
      const INDEX iFrags  = penPlayer->m_psGameStats.ps_iKills;
      const INDEX iDeaths = penPlayer->m_psGameStats.ps_iDeaths;
      const INDEX iHealth = ClampDn( (INDEX)ceil( penPlayer->GetHealth()), 0L);
      const INDEX iArmor  = ClampDn( (INDEX)ceil( penPlayer->m_fArmor),    0L);
      const INDEX iPing = ceil(penPlayer->en_tmPing*1000.0f);

      CTString strScore, strMana, strFrags, strDeaths, strHealth, strArmor, strPing;
      strScore.PrintF(  "%d", iScore);
      strMana.PrintF(   "%d", iMana);
      strFrags.PrintF(  "%d", iFrags);
      strDeaths.PrintF( "%d", iDeaths);
      
      // [SSE]
      if (iArmor >= 10000) {
        strArmor.PrintF( "%.1fk", iArmor / 1000.0F);
      } else if (iArmor >= 1000) {
        strArmor.PrintF( "%.2fk", iArmor / 1000.0F);
      } else {
        strArmor.PrintF( "%d", iArmor);
      }

      if (iHealth >= 10000) {
        strHealth.PrintF( "%.1fk", iHealth / 1000.0F);
      } else if (iHealth >= 1000) {
        strHealth.PrintF( "%.2fk", iHealth / 1000.0F);
      } else {
        strHealth.PrintF( "%d", iHealth);
      }
      //
      
			if ( iPing > 300 ) {
				colPing = C_RED;
        strPing.PrintF("^f9/^r^c808080///");

      } else if (iPing > 200) {
				colPing = C_RED;
        strPing.PrintF("/^c808080///");
        
      } else if (iPing > 140) {
				colPing = C_ORANGE;
        strPing.PrintF("//^c808080//");

			} else if ( iPing > 80 ) {
				colPing = C_YELLOW;
        strPing.PrintF("///^c808080/");

			} else {
        strPing.PrintF("////");
				colPing = C_GREEN;
			}

      // detemine corresponding colors
      colHealth = C_mlRED;
      colMana = colScore = colFrags = colDeaths = colArmor = C_lGRAY;
      if (iMana   > _penPlayer->m_iMana)                      { bMaxMana   = FALSE; colMana   = C_WHITE; }
      if (iScore  > _penPlayer->m_psGameStats.ps_iScore)      { bMaxScore  = FALSE; colScore  = C_WHITE; }
      if (iFrags  > _penPlayer->m_psGameStats.ps_iKills)      { bMaxFrags  = FALSE; colFrags  = C_WHITE; }
      if (iDeaths > _penPlayer->m_psGameStats.ps_iDeaths)     { bMaxDeaths = FALSE; colDeaths = C_WHITE; }
      if (penPlayer == _penPlayer) colScore = colMana = colFrags = colDeaths = _colHUD; // current player
      if (iHealth > 25) colHealth = _colHUD;
      if (iArmor  > 25) colArmor  = _colHUD;

      // eventually print it out
      if (hud_iShowPlayers==1 || hud_iShowPlayers==-1 && !bSinglePlay)
      {
        // printout location and info aren't the same for deathmatch and coop play
        const FLOAT fCharWidth = (PIX)((_pfdDisplayFont->GetWidth()-2) *fTextScale);

        if (bCooperative) {
          _pDP->PutTextR( strName+":", _pixDPWidth - 12 *fCharWidth, fCharHeight*i+fOneUnit*2, colScore |_ulAlphaHUD);
          
          if (penPlayer->GetFlags()&ENF_ALIVE) {
            _pDP->PutText(  "/",         _pixDPWidth-8*fCharWidth, fCharHeight*i+fOneUnit*2, _colHUD  |_ulAlphaHUD);
            _pDP->PutTextC( strHealth,   _pixDPWidth-10*fCharWidth, fCharHeight*i+fOneUnit*2, colHealth|_ulAlphaHUD);
            _pDP->PutTextC( strArmor,    _pixDPWidth-6*fCharWidth, fCharHeight*i+fOneUnit*2, colArmor |_ulAlphaHUD);
          } else {
            _pDP->PutTextC(  TRANS("DEAD"), _pixDPWidth-8*fCharWidth, fCharHeight*i+fOneUnit*2, C_RED|_ulAlphaHUD); // [SSE] No Stupid Zeros If Player Dead
          }
          
          _pDP->PutTextC( strPing,     _pixDPWidth- 2 * fCharWidth, fCharHeight * i + fOneUnit * 2, colPing|_ulAlphaHUD);

        } else if (bScoreMatch) {
          _pDP->PutTextR( strName+":", _pixDPWidth - 14 * fCharWidth, fCharHeight*i+fOneUnit*2, _colHUD |_ulAlphaHUD);
          _pDP->PutText(  "/",         _pixDPWidth- 9 * fCharWidth, fCharHeight*i+fOneUnit*2, _colHUD |_ulAlphaHUD);
          _pDP->PutTextC( strScore,    _pixDPWidth- 12 * fCharWidth, fCharHeight*i+fOneUnit*2, colScore|_ulAlphaHUD);
          _pDP->PutTextC( strMana,     _pixDPWidth- 6 * fCharWidth, fCharHeight*i+fOneUnit*2, colMana |_ulAlphaHUD);
          _pDP->PutTextC( strPing,     _pixDPWidth- 2 * fCharWidth, fCharHeight * i + fOneUnit * 2, colPing|_ulAlphaHUD);

        } else { // fragmatch!
          CTString strNameString;

          // [SSE]
          if (bTeamDeathMatch) {
            INDEX iTeamID = penPlayer->m_iTeamID;
            
            switch (iTeamID)
            {
              case 1: strNameString.PrintF("^c7F7FFF> ^r%s", strName); break; // Blue
              case 2: strNameString.PrintF("^cFF7F7F> ^r%s", strName); break; // Red
              case 3: strNameString.PrintF("^c7FFF7F> ^r%s", strName); break; // Green
              case 4: strNameString.PrintF("^cFFFF7F> ^r%s", strName); break; // Blue
              
              default: strNameString.PrintF("^cAAAAAA> ^r%s", strName); break; // Unassigned - Gray
            }

          } else {
            strNameString = strName;
          }

          _pDP->PutTextR( strNameString+":", _pixDPWidth-12 * fCharWidth, fCharHeight * i + fOneUnit * 2, _colHUD  |_ulAlphaHUD);
          _pDP->PutText(  "/",         _pixDPWidth-8 * fCharWidth, fCharHeight * i + fOneUnit * 2, _colHUD  |_ulAlphaHUD);
          _pDP->PutTextC( strFrags,    _pixDPWidth-10 * fCharWidth, fCharHeight * i + fOneUnit * 2, colFrags |_ulAlphaHUD);
          _pDP->PutTextC( strDeaths,   _pixDPWidth-6 * fCharWidth, fCharHeight * i + fOneUnit * 2, colDeaths|_ulAlphaHUD);
          _pDP->PutTextC( strPing,      _pixDPWidth-2 * fCharWidth, fCharHeight * i + fOneUnit * 2, colPing|_ulAlphaHUD);
        }
      }

      // calculate summ of scores (for coop mode)
      iScoreSum += iScore;
    }

    // draw remaining time if time based death- or scorematch
    if ((bScoreMatch || bFragMatch) && hud_bShowMatchInfo)
    {
      CTString strLimitsInfo = "";

      extern INDEX HUD_SetAllPlayersStats( INDEX iSortKey);
      // fill players table
      const INDEX ctPlayers = HUD_SetAllPlayersStats(bFragMatch?5:3); // sort by frags or by score
      // find maximum frags/score that one player has
      INDEX iMaxFrags = LowerLimit(INDEX(0));
      INDEX iMaxScore = LowerLimit(INDEX(0));

      {for(INDEX iPlayer=0; iPlayer<ctPlayers; iPlayer++) {
        CPlayer *penPlayer = _apenPlayers[iPlayer];
        iMaxFrags = Max(iMaxFrags, penPlayer->m_psLevelStats.ps_iKills);
        iMaxScore = Max(iMaxScore, penPlayer->m_psLevelStats.ps_iScore);
      }}

      // [SSE] TeamDeathmatch
      if (bTeamDeathMatch)
      {
        iMaxFrags = Max(GetSP()->sp_iTeamScore1, GetSP()->sp_iTeamScore2);
        
        if (ctTeams >= 3) iMaxFrags = Max(iMaxFrags, GetSP()->sp_iTeamScore3);
        if (ctTeams >= 4) iMaxFrags = Max(iMaxFrags, GetSP()->sp_iTeamScore4);
      }

      if (GetSP()->sp_iFragLimit>0) {
        INDEX iFragsLeft = ClampDn(GetSP()->sp_iFragLimit-iMaxFrags, INDEX(0));
        strLimitsInfo.PrintF("%s^cFFFFFF%s: %d\n", strLimitsInfo, TRANS("FRAGS LEFT"), iFragsLeft);
      }

      if (GetSP()->sp_iScoreLimit>0) {
        INDEX iScoreLeft = ClampDn(GetSP()->sp_iScoreLimit-iMaxScore, INDEX(0));
        strLimitsInfo.PrintF("%s^cFFFFFF%s: %d\n", strLimitsInfo, TRANS("SCORE LEFT"), iScoreLeft);
      }

      _pfdDisplayFont->SetFixedWidth();
      _pDP->SetFont( _pfdDisplayFont);
      _pDP->SetTextScaling( fTextScale*0.8f );
      _pDP->SetTextCharSpacing( -2.0f*fTextScale);
      _pDP->PutText( strLimitsInfo, 5.0f*_pixDPWidth/640.0f, 68.0f*_pixDPWidth/640.0f, C_WHITE|CT_OPAQUE);
    }


    // prepare color for local player printouts
    bMaxScore  ? colScore  = C_WHITE : colScore  = C_lGRAY;
    bMaxMana   ? colMana   = C_WHITE : colMana   = C_lGRAY;
    bMaxFrags  ? colFrags  = C_WHITE : colFrags  = C_lGRAY;
    bMaxDeaths ? colDeaths = C_WHITE : colDeaths = C_lGRAY;
  }

  // printout player latency if needed
  if (hud_bShowLatency) {
    CTString strLatency;
    strLatency.PrintF( "%4.0fms", _penPlayer->m_tmLatency*1000.0f);
    PIX pixFontHeight = (PIX)(_pfdDisplayFont->GetHeight() *fTextScale +fTextScale+1);
    _pfdDisplayFont->SetFixedWidth();
    _pDP->SetFont( _pfdDisplayFont);
    _pDP->SetTextScaling( fTextScale);
    _pDP->SetTextCharSpacing( -2.0f*fTextScale);
    _pDP->PutTextR( strLatency, _pixDPWidth, _pixDPHeight-pixFontHeight, C_WHITE|CT_OPAQUE);
  }

  // restore font defaults
  _pfdDisplayFont->SetVariableWidth();
  _pDP->SetFont( &_fdNumbersFont);
  _pDP->SetTextCharSpacing(1);

  // prepare output strings and formats depending on game type
  FLOAT fWidthAdj = 8;
  INDEX iScore = _penPlayer->m_psGameStats.ps_iScore;
  INDEX iMana  = _penPlayer->m_iMana;
  if (bFragMatch) {
    if (!hud_bShowMatchInfo) { fWidthAdj = 4; }
    iScore = _penPlayer->m_psGameStats.ps_iKills;
    iMana  = _penPlayer->m_psGameStats.ps_iDeaths;
  } else if (bCooperative) {
    // in case of coop play, show squad (common) score
    iScore = iScoreSum;
  }

  // LSC Up - Score / Frags
  {
    CTString strScore;
    strScore.PrintF("%d", iScore);
    
    HUD_DrawAnchoredRect ( 8,  8,  16, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect (28,  8, 104, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( 8,  8,  16, 16, EHHAT_LEFT, EHVAT_TOP, _pHAC->m_toFrags, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
    
    HUD_DrawAnchoredRectOutline( 8, 8,  16, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(28, 8, 104, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredTextInRect (FLOAT2D(28, 8), FLOAT2D(104, 16), EHHAT_LEFT, EHVAT_TOP, strScore, C_lGRAY, 1.0F);
  }

  // LSC Down - SP/Coop = Extra Lives
  if (bCooperative && GetSP()->sp_ctCredits >= 0)
  {
    CTString strLives;
    strLives.PrintF( "%d", bSharedLives ? GetSP()->sp_ctCreditsLeft : _penPlayer->m_iLives);

    HUD_DrawAnchoredRect ( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect (28, 28, 104, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, _pHAC->m_toExtraLive, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
    
    HUD_DrawAnchoredRectOutline( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(28, 28, 104, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredTextInRect (FLOAT2D(28, 28), FLOAT2D(104, 16), EHHAT_LEFT, EHVAT_TOP, strLives, C_lGRAY, 1.0F);

    if (GetSP()->sp_iScoreForExtraLive > 0)
    {
      CTString strLimitsInfo;
      
      FLOAT fLiveCostMultipleir = bSharedLives ? GetSP()->sp_fLiveCostMultiplier : _penPlayer->m_fLiveCostMultiplier;
      INDEX iLiveCost = GetSP()->sp_iScoreForExtraLive * fLiveCostMultipleir;
      INDEX iAccumulateScore = bSharedLives ? GetSP()->sp_iScoreForExtraLiveAccum : _penPlayer->m_iScoreAccumulated;
      INDEX iScoreLeft = ClampDn(iLiveCost - iAccumulateScore, INDEX(0));

      strLimitsInfo.PrintF("%s^cFFFFFF%s: %d\n", strLimitsInfo, TRANS("SCORE LEFT"), iScoreLeft);
      
      _pfdDisplayFont->SetFixedWidth();
      _pDP->SetFont( _pfdDisplayFont);
      _pDP->SetTextScaling( fTextScale*0.8f );
      _pDP->SetTextCharSpacing( -2.0f*fTextScale);
      _pDP->PutText( strLimitsInfo, 5.0f*_pixDPWidth/640.0f, 48.0f*_pixDPWidth/640.0f, C_WHITE|CT_OPAQUE);
      
      _pfdDisplayFont->SetVariableWidth();
      _pDP->SetFont( &_fdNumbersFont);
      _pDP->SetTextCharSpacing(1);
    }
  }
  
  // LSC Down - DM/SM = Death/Mana
  if (bFragMatch || bScoreMatch)
  {
    CTString strMana;
    strMana.PrintF( "%d", iMana);

    HUD_DrawAnchoredRect ( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect (28, 28, 104, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, _pHAC->m_toDeaths, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
    
    HUD_DrawAnchoredRectOutline( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(28, 28, 104, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredTextInRect (FLOAT2D(28, 28), FLOAT2D(104, 16), EHHAT_LEFT, EHVAT_TOP, strMana, C_lGRAY, 1.0F);
  }
  
  // set font and prepare font parameters
  _pfdDisplayFont->SetFixedWidth();
  _pDP->SetFont( _pfdDisplayFont);
  _pDP->SetTextLineSpacing(0);
  
  // Time Limit
  if ((bScoreMatch || bFragMatch) && hud_bShowMatchInfo && GetSP()->sp_iTimeLimit > 0)
  {
    FLOAT fTimeLeft = ClampDn(GetSP()->sp_iTimeLimit*60.0f - _pNetwork->GetGameTime(), 0.0f);
    
    CTString strTime;
    strTime.PrintF( "%s", TimeToString(fTimeLeft));

    HUD_DrawAnchoredRect ( 8, 48,  16, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect (28, 48, 104, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( 8, 48,  16, 16, EHHAT_LEFT, EHVAT_TOP, _pHAC->m_toClock, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
    
    HUD_DrawAnchoredRectOutline( 8, 48,  16, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(28, 48, 104, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredTextInRect (FLOAT2D(28, 48 + 2), FLOAT2D(104, 16), EHHAT_LEFT, EHVAT_TOP, strTime, C_WHITE|_ulAlphaHUD, 1.0F, 0.75F);
  }
  
  _pfdDisplayFont->SetVariableWidth();
  _pDP->SetFont( &_fdNumbersFont);
  _pDP->SetTextCharSpacing(1);

  // If single player or cooperative mode then draw HiScore info.
  if (bSinglePlay || bCooperative)
  {
    CTString strHiScore;
    strHiScore.PrintF( "%d", Max(_penPlayer->m_iHighScore, _penPlayer->m_psGameStats.ps_iScore));
    BOOL bBeating = _penPlayer->m_psGameStats.ps_iScore > _penPlayer->m_iHighScore;
    
    HUD_DrawAnchoredRect ( -54, 8,  16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( -54, 8,  16, 16, EHHAT_CENTER, EHVAT_TOP, _pHAC->m_toHiScore, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
    HUD_DrawAnchoredRect (  10, 8, 104, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    
    HUD_DrawAnchoredRectOutline(-54, 8,  16, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline( 10, 8, 104, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
    
    HUD_DrawAnchoredTextInRect (FLOAT2D(10, 8), FLOAT2D(104, 16), EHHAT_CENTER, EHVAT_TOP, strHiScore, NONE, bBeating ? 0.0f : 1.0f);
  }
  
  BOOL bMessagesBoxDrawn = FALSE;
  
  // Right Top - Messages
  if (bSinglePlay || bCooperative)
  {
    // prepare and draw unread messages
    if (hud_bShowMessages && _penPlayer->m_ctUnreadMessages > 0)
    {
      bMessagesBoxDrawn = TRUE;
      
      CTString strMessages;
      strMessages.PrintF( "%d", _penPlayer->m_ctUnreadMessages);

      const FLOAT tmIn = 0.5f;
      const FLOAT tmOut = 0.5f;
      const FLOAT tmStay = 1.0f;
      FLOAT tmDelta = _pTimer->GetLerpedCurrentTick()-_penPlayer->m_tmAnimateInbox;
      COLOR col = _colHUD;

      if (tmDelta > 0 && tmDelta < (tmIn + tmStay + tmOut) && bSinglePlay)
      {
        FLOAT fRatio = 0.0f;

        if (tmDelta < tmIn) {
          fRatio = ClampUp(tmDelta / tmIn, 1.0F);
        } else if (tmDelta>tmIn+tmStay) {
          fRatio = (tmIn+tmStay+tmOut-tmDelta) / tmOut;
        } else {
          fRatio = 1.0f;
        }

        col = LerpColor(_colHUD, C_WHITE|0xFF, fRatio);
      }
      
      HUD_DrawAnchoredRect ( 8, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchoredRect (28, 8, 52, 16, EHHAT_RIGHT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchroredIcon( 8, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, _pHAC->m_toMessage, C_WHITE|CT_OPAQUE, 0.0F, TRUE); // Icon
      
      HUD_DrawAnchoredRectOutline( 8, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, col|_ulAlphaHUD);
      HUD_DrawAnchoredRectOutline(28, 8, 52, 16, EHHAT_RIGHT, EHVAT_TOP, col|_ulAlphaHUD);
      
      HUD_DrawAnchoredTextInRect (FLOAT2D(28, 8), FLOAT2D(52, 16), EHHAT_RIGHT, EHVAT_TOP, strMessages, col|_ulAlphaHUD, 1.0F);
    }
  }
  
  BOOL bHasAnyKey = bSharedKeys ? GetSP()->sp_ulPickedKeys : _penPlayer->m_ulKeys;
  
  // Right Top - Keys
  if (bHasAnyKey) {
    PIX pixShiftX = 0;
    
    if (bMessagesBoxDrawn) {
      pixShiftX = 76;
    }

    COLOR col = _colHUD;

    CTString strKeys;
    strKeys.PrintF("%d", bSharedKeys ? bit_count(GetSP()->sp_ulPickedKeys) : bit_count(_penPlayer->m_ulKeys));
    
    HUD_DrawAnchoredRect ( 8 + pixShiftX, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchoredRect (28 + pixShiftX, 8, 52, 16, EHHAT_RIGHT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon( 8 + pixShiftX, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, _pHAC->m_toKey, C_WHITE|CT_OPAQUE, 0.0F, FALSE); // Icon
    
    HUD_DrawAnchoredRectOutline( 8 + pixShiftX, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, col|_ulAlphaHUD);
    HUD_DrawAnchoredRectOutline(28 + pixShiftX, 8, 52, 16, EHHAT_RIGHT, EHVAT_TOP, col|_ulAlphaHUD);
    
    HUD_DrawAnchoredTextInRect (FLOAT2D(28 + pixShiftX, 8), FLOAT2D(52, 16), EHHAT_RIGHT, EHVAT_TOP, strKeys, C_lGRAY|_ulAlphaHUD, 1.0F);
  }

  #ifdef ENTITY_DEBUG
  // if entity debug is on, draw entity stack
  HUD_DrawEntityStack();
  #endif

  // draw cheat modes if singleplayer
  if (GetSP()->sp_ctMaxPlayers == 1) {
    INDEX iLine=1;
    ULONG ulAlpha = sin(_tmNow*16)*96 +128;
    PIX pixFontHeight = _pfdConsoleFont->fd_pixCharHeight;
    const COLOR colCheat = _colHUDText;
    _pDP->SetFont( _pfdConsoleFont);
    _pDP->SetTextScaling( 1.0f);
    const FLOAT fchtTM = cht_fTranslationMultiplier; // for text formatting sake :)

    if (fchtTM > 1.0f)     { _pDP->PutTextR( "turbo",     _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bGhost)        { _pDP->PutTextR( "ghost",     _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bFly)          { _pDP->PutTextR( "fly",       _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bGod)          { _pDP->PutTextR( "god",       _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bInfiniteAmmo) { _pDP->PutTextR( "noammo",    _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bAutoKill)     { _pDP->PutTextR( "autokill",  _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bInvisible)    { _pDP->PutTextR( "invisible", _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
  }

  // in the end, remember the current time so it can be used in the next frame
  _tmLast = _tmNow;

  if (hud_bGameDebugMonitor)
  {
    HUD_DrawDebugMonitor();
  }
}

extern void DrawOldHUD(const CPlayer *penPlayerCurrent, CDrawPort *pdpCurrent, BOOL bSnooping, const CPlayer *penPlayerOwner)
{
  // no player - no info, sorry
  if (penPlayerCurrent == NULL || (penPlayerCurrent->GetFlags()&ENF_DELETED)) return;
  
  if (!_bHUDFontsLoaded) return;

  // if snooping and owner player ins NULL, return
  if (bSnooping && penPlayerOwner == NULL) return;

  // find last values in case of predictor
  CPlayer *penLast = (CPlayer*)penPlayerCurrent;
  if (penPlayerCurrent->IsPredictor()) penLast = (CPlayer*)(((CPlayer*)penPlayerCurrent)->GetPredicted());
  ASSERT(penLast != NULL);
  if (penLast == NULL) return; // !!!! just in case

  // cache local variables
  hud_fOpacity = Clamp( hud_fOpacity, 0.1f, 1.0f);
  hud_fScaling = Clamp( hud_fScaling, 0.5f, 1.2f);
  
  _penPlayer  = penPlayerCurrent;
  _penWeapons = (CPlayerWeapons*)&*_penPlayer->m_penWeapons;
  _pDP        = pdpCurrent;
  _pixDPWidth   = _pDP->GetWidth();
  _pixDPHeight  = _pDP->GetHeight();

  _fCustomScaling     = hud_fScaling;
  _fResolutionScaling = (FLOAT)_pixDPWidth / 640.0f;
  _colHUD     = HUD_GetInterfaceColor(penPlayerOwner);
  _colHUDText = SE_COL_ORANGE_LIGHT;
  _ulAlphaHUD = NormFloatToByte(hud_fOpacity);
  _ulBrAlpha = NormFloatToByte(hud_fOpacity * 0.5F);
  _tmNow = _pTimer->CurrentTick();
  
  const BOOL bSharedLives = GetSP()->sp_bSharedLives; 
  const BOOL bSharedKeys = GetSP()->sp_bSharedKeys;

  // determine hud colorization;
  COLOR colMax = SE_COL_BLUEGREEN_LT;
  COLOR colTop = SE_COL_ORANGE_LIGHT;
  COLOR colMid = LerpColor(colTop, C_RED, 0.5f);
  
  // adjust borders color in case of spying mode
  COLOR colBorder = _colHUD;

  if (bSnooping) {
    colBorder = SE_COL_ORANGE_NEUTRAL;
    if (((ULONG)(_tmNow*5))&1) {
      //colBorder = (colBorder>>1) & 0x7F7F7F00; // darken flash and scale
      colBorder = SE_COL_ORANGE_DARK;
      _fCustomScaling *= 0.933f;
    }
  }

  // draw sniper mask (original mask even if snooping)
  if (((CPlayerWeapons*)&*penPlayerOwner->m_penWeapons)->m_iCurrentWeapon == WEAPON_SNIPER && ((CPlayerWeapons*)&*penPlayerOwner->m_penWeapons)->m_bSniping && hud_bSniperScopeDraw)
  {
    HUD_DrawSniperMask();
  }

  // prepare font and text dimensions
  CTString strValue;
  PIX pixCharWidth;
  FLOAT fValue, fNormValue, fCol, fRow;
  _pDP->SetFont( &_fdNumbersFont);
  pixCharWidth = _fdNumbersFont.GetWidth() + _fdNumbersFont.GetCharSpacing() +1;
  FLOAT fChrUnit = pixCharWidth * _fCustomScaling;

  const PIX pixTopBound    = 6;
  const PIX pixLeftBound   = 6;
  const PIX pixBottomBound = (480 * _pDP->dp_fWideAdjustment) -pixTopBound;
  const PIX pixRightBound  = 640-pixLeftBound;
  FLOAT fOneUnit  = (32+0) * _fCustomScaling;  // unit size
  FLOAT fAdvUnit  = (32+4) * _fCustomScaling;  // unit advancer
  FLOAT fNextUnit = (32+8) * _fCustomScaling;  // unit advancer
  FLOAT fHalfUnit = fOneUnit * 0.5f;
  FLOAT fMoverX, fMoverY;
  COLOR colDefault;

  // prepare and draw health info
  fValue = ClampDn( _penPlayer->GetHealth(), 0.0f);  // never show negative health
  fNormValue = fValue / TOP_HEALTH;
  strValue.PrintF( "%d", (SLONG)ceil(fValue));
  HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
  fRow = pixBottomBound-fHalfUnit;
  fCol = pixLeftBound+fHalfUnit;
  colDefault = HUD_AddShaker( 5, fValue, penLast->m_iLastHealth, penLast->m_tmHealthChanged, fMoverX, fMoverY);
  HUD_DrawBorder( fCol+fMoverX, fRow+fMoverY, fOneUnit, fOneUnit, colBorder);
  fCol += fAdvUnit+fChrUnit*3/2 -fHalfUnit;
  HUD_DrawBorder( fCol, fRow, fChrUnit*3, fOneUnit, colBorder);
  HUD_DrawText( fCol, fRow, strValue, colDefault, fNormValue);
  fCol -= fAdvUnit+fChrUnit*3/2 -fHalfUnit;
  HUD_DrawIcon( fCol+fMoverX, fRow+fMoverY, _pHAC->m_toHealth, C_WHITE /*_colHUD*/, fNormValue, TRUE, 32, 32);

  // prepare and draw armor info (eventually)
  fValue = _penPlayer->m_fArmor;
  if (fValue > 0.0f) {
    fNormValue = fValue/TOP_ARMOR;
    strValue.PrintF( "%d", (SLONG)ceil(fValue));
    HUD_PrepareColorTransitions( colMax, colTop, colMid, C_lGRAY, 0.5f, 0.25f, FALSE);
    fRow = pixBottomBound- (fNextUnit+fHalfUnit);//*_pDP->dp_fWideAdjustment;
    fCol = pixLeftBound+    fHalfUnit;
    colDefault = HUD_AddShaker( 3, fValue, penLast->m_iLastArmor, penLast->m_tmArmorChanged, fMoverX, fMoverY);
    HUD_DrawBorder( fCol+fMoverX, fRow+fMoverY, fOneUnit, fOneUnit, colBorder);
    fCol += fAdvUnit+fChrUnit*3/2 -fHalfUnit;
    HUD_DrawBorder( fCol, fRow, fChrUnit*3, fOneUnit, colBorder);
    HUD_DrawText( fCol, fRow, strValue, NONE, fNormValue);
    fCol -= fAdvUnit+fChrUnit*3/2 -fHalfUnit;
    if (fValue<=50.5f) {
      HUD_DrawIcon( fCol+fMoverX, fRow+fMoverY, _pHAC->m_toArmorSmall, C_WHITE /*_colHUD*/, fNormValue, FALSE, 32, 32);
    } else if (fValue<=100.5f) {
      HUD_DrawIcon( fCol+fMoverX, fRow+fMoverY, _pHAC->m_toArmorMedium, C_WHITE /*_colHUD*/, fNormValue, FALSE, 32, 32);
    } else {
      HUD_DrawIcon( fCol+fMoverX, fRow+fMoverY, _pHAC->m_toArmorLarge, C_WHITE /*_colHUD*/, fNormValue, FALSE, 32, 32);
    }
  }

  // prepare and draw ammo and weapon info
  CTextureObject *ptoCurrentAmmo=NULL, *ptoCurrentWeapon=NULL, *ptoWantedWeapon=NULL;
  INDEX iCurrentWeapon = _penWeapons->m_iCurrentWeapon;
  INDEX iWantedWeapon  = _penWeapons->m_iWantedWeapon;
  // determine corresponding ammo and weapon texture component
  ptoCurrentWeapon = _awiWeapons[iCurrentWeapon].wi_ptoWeapon;
  ptoWantedWeapon  = _awiWeapons[iWantedWeapon].wi_ptoWeapon;

  AmmoInfo *paiCurrent = _awiWeapons[iCurrentWeapon].wi_paiAmmo;
  if (paiCurrent != NULL) ptoCurrentAmmo = paiCurrent->ai_ptoAmmo;
  
  BOOL bAnyColt = iCurrentWeapon == WEAPON_COLT || iCurrentWeapon == WEAPON_DOUBLECOLT;

  // draw complete weapon info if knife isn't current weapon
  if (bAnyColt || (ptoCurrentAmmo != NULL && !GetSP()->sp_bInfiniteAmmo)) {
    // determine ammo quantities
    FLOAT fMaxValue = _penWeapons->GetMaxAmmo();
    fValue = _penWeapons->GetAmmo();
    fNormValue = fValue / fMaxValue;
    strValue.PrintF( "%d", (SLONG)ceil(fValue));
    HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.30f, 0.15f, FALSE);
    BOOL bDrawAmmoIcon = _fCustomScaling<=1.0f;

    // [SSE] Colt Bullets
    if (bAnyColt) {
      ptoCurrentAmmo = &_pHAC->m_toAColt;
    }

    // draw ammo, value and weapon
    fRow = pixBottomBound-fHalfUnit;
    fCol = 175 + fHalfUnit;
    colDefault = HUD_AddShaker( 4, fValue, penLast->m_iLastAmmo, penLast->m_tmAmmoChanged, fMoverX, fMoverY);
    HUD_DrawBorder( fCol+fMoverX, fRow+fMoverY, fOneUnit, fOneUnit, colBorder);
    fCol += fAdvUnit+fChrUnit*3/2 -fHalfUnit;
    HUD_DrawBorder( fCol, fRow, fChrUnit*3, fOneUnit, colBorder);

    if (bDrawAmmoIcon)
    {
      fCol += fAdvUnit+fChrUnit*3/2 -fHalfUnit;
      HUD_DrawBorder( fCol, fRow, fOneUnit, fOneUnit, colBorder);
      HUD_DrawIcon( fCol, fRow, *ptoCurrentAmmo, C_WHITE /*_colHUD*/, fNormValue, TRUE, 32, 32);
      fCol -= fAdvUnit+fChrUnit*3/2 -fHalfUnit;
    }

    HUD_DrawText( fCol, fRow, strValue, NONE, fNormValue);
    fCol -= fAdvUnit+fChrUnit*3/2 -fHalfUnit;
    HUD_DrawIcon( fCol+fMoverX, fRow+fMoverY, *ptoCurrentWeapon, C_WHITE /*_colHUD*/, fNormValue, !bDrawAmmoIcon, 32, 32);

  } else if (ptoCurrentWeapon != NULL) {
    // draw only knife or colt icons (ammo is irrelevant)
    fRow = pixBottomBound-fHalfUnit;
    fCol = 205 + fHalfUnit;
    HUD_DrawBorder( fCol, fRow, fOneUnit, fOneUnit, colBorder);
    HUD_DrawIcon(   fCol, fRow, *ptoCurrentWeapon, C_WHITE /*_colHUD*/, fNormValue, FALSE, 32, 32);
  }

  // display all ammo infos
  INDEX i;
  FLOAT fAdv;
  COLOR colIcon, colBar;
  HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
  // reduce the size of icon slightly
  _fCustomScaling = ClampDn( _fCustomScaling*0.8f, 0.5f);
  const FLOAT fOneUnitS  = fOneUnit  * 0.8f;
  const FLOAT fAdvUnitS  = fAdvUnit  * 0.8f;
  const FLOAT fNextUnitS = fNextUnit * 0.8f;
  const FLOAT fHalfUnitS = fHalfUnit * 0.8f;

  // prepare postition and ammo quantities
  fRow = pixBottomBound-fHalfUnitS;
  fCol = pixRightBound -fHalfUnitS;
  const FLOAT fBarPos = fHalfUnitS * 0.7f;
  HUD_FillWeaponAmmoTables();

  FLOAT fBombCount = _penWeapons->m_iSeriousBombs;
  BOOL  bBombFiring = FALSE;
  // draw serious bomb
#define BOMB_FIRE_TIME 1.5f
  if (penPlayerCurrent->m_tmSeriousBombFired+BOMB_FIRE_TIME>_pTimer->GetLerpedCurrentTick()) {
    fBombCount++;
    if (fBombCount > 3) { fBombCount = 3; }
    bBombFiring = TRUE;
  }

  if (fBombCount > 0) {
    fNormValue = (FLOAT) fBombCount / 3.0f;
    COLOR colBombBorder = _colHUD;
    COLOR colBombIcon = C_WHITE;
    COLOR colBombBar = _colHUDText;
    
    if (fBombCount == 1) {
      colBombBar = C_RED;
    }

    if (bBombFiring) {
      FLOAT fFactor = (_pTimer->GetLerpedCurrentTick() - penPlayerCurrent->m_tmSeriousBombFired)/BOMB_FIRE_TIME;
      colBombBorder = LerpColor(colBombBorder, C_RED, fFactor);
      colBombIcon = LerpColor(colBombIcon, C_RED, fFactor);
      colBombBar = LerpColor(colBombBar, C_RED, fFactor);
    }

    HUD_DrawBorder( fCol,         fRow, fOneUnitS, fOneUnitS, colBombBorder);
    HUD_DrawIcon(   fCol,         fRow, _pHAC->m_atoAmmo[8], colBombIcon, fNormValue, FALSE, 32, 32);
    HUD_DrawBar(    fCol+fBarPos, fRow, fOneUnitS/5, fOneUnitS-2, BO_DOWN, colBombBar, fNormValue);
    // make space for serious bomb
    fCol -= fAdvUnitS;
  }

  // loop thru all ammo types
  if (!GetSP()->sp_bInfiniteAmmo)
  {
    for (INDEX ii = 7; ii >= 0; ii--)
    {
      i = aiAmmoRemap[ii];
      
      AmmoInfo &ai = _aaiAmmo[i];
      
      // if no ammo and hasn't got that weapon - just skip this ammo // [SSE] Or if you don't want to see empty ammo types.
      if (ai.ai_iAmmoAmmount <= 0 && (!ai.ai_bHasWeapon || !hud_bShowEmptyAmmoInList)) continue;

      // display ammo info
      colIcon = C_WHITE /*_colHUD*/;
      if (ptoCurrentAmmo == ai.ai_ptoAmmo) colIcon = C_WHITE;
      if (ai.ai_iAmmoAmmount <= 0) colIcon = C_mdGRAY;

      fNormValue = (FLOAT)ai.ai_iAmmoAmmount / ai.ai_iMaxAmmoAmmount;
      colBar = HUD_AddShaker( 4, ai.ai_iAmmoAmmount, ai.ai_iLastAmmoAmmount, ai.ai_tmAmmoChanged, fMoverX, fMoverY);
      HUD_DrawBorder( fCol,         fRow+fMoverY, fOneUnitS, fOneUnitS, colBorder);
      HUD_DrawIcon(   fCol,         fRow+fMoverY, *_aaiAmmo[i].ai_ptoAmmo, colIcon, fNormValue, FALSE, 32, 32);
      HUD_DrawBar(    fCol+fBarPos, fRow+fMoverY, fOneUnitS/5, fOneUnitS-2, BO_DOWN, colBar, fNormValue);
      // advance to next position
      fCol -= fAdvUnitS;
    }
  }

  // draw powerup(s) if needed
  HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.66f, 0.33f, FALSE);
  TIME *ptmPowerups = (TIME*)&_penPlayer->m_tmInvisibility;
  TIME *ptmPowerupsMax = (TIME*)&_penPlayer->m_tmInvisibilityMax;
  fRow = pixBottomBound-fOneUnitS-fAdvUnitS;
  fCol = pixRightBound -fHalfUnitS;

  for (i=0; i<MAX_POWERUPS; i++)
  {
    // skip if not active
    const TIME tmDelta = ptmPowerups[i] - _tmNow;
    if (tmDelta<=0) continue;
    fNormValue = tmDelta / ptmPowerupsMax[i];
    // draw icon and a little bar
    HUD_DrawBorder( fCol,         fRow, fOneUnitS, fOneUnitS, colBorder);
    HUD_DrawIcon(   fCol,         fRow, _pHAC->m_atoPowerups[i], C_WHITE /*_colHUD*/, fNormValue, TRUE, 32, 32);
    HUD_DrawBar(    fCol+fBarPos, fRow, fOneUnitS/5, fOneUnitS-2, BO_DOWN, NONE, fNormValue);
    // play sound if icon is flashing
    if (fNormValue<=(_cttHUD.ctt_fLowMedium/2)) {
      // activate blinking only if value is <= half the low edge
      INDEX iLastTime = (INDEX)(_tmLast*4);
      INDEX iCurrentTime = (INDEX)(_tmNow*4);
      if (iCurrentTime&1 & !(iLastTime&1)) {
        ((CPlayer *)penPlayerCurrent)->PlayPowerUpSound();
      }
    }
    // advance to next position
    fCol -= fAdvUnitS;
  }


  // if weapon change is in progress
  _fCustomScaling = hud_fScaling;
  hud_tmWeaponsOnScreen = Clamp( hud_tmWeaponsOnScreen, 0.0f, 10.0f);
  if ((_tmNow - _penWeapons->m_tmWeaponChangeRequired) < hud_tmWeaponsOnScreen) {
    // determine number of weapons that player has
    INDEX ctWeapons = 0;
    for (i=WEAPON_NONE+1; i<WEAPON_LAST; i++) {
      if (_awiWeapons[i].wi_wtWeapon!=WEAPON_NONE && _awiWeapons[i].wi_wtWeapon!=WEAPON_DOUBLECOLT &&
          _awiWeapons[i].wi_bHasWeapon) ctWeapons++;
    }
    // display all available weapons
    fRow = pixBottomBound - fHalfUnit - 3*fNextUnit;
    fCol = 320.0f - (ctWeapons*fAdvUnit-fHalfUnit)/2.0f;
    // display all available weapons
    for (INDEX ii=WEAPON_NONE+1; ii<WEAPON_LAST; ii++) {
      i = aiWeaponsRemap[ii];
      // skip if hasn't got this weapon
      if (_awiWeapons[i].wi_wtWeapon==WEAPON_NONE || _awiWeapons[i].wi_wtWeapon==WEAPON_DOUBLECOLT
         || !_awiWeapons[i].wi_bHasWeapon) continue;
      // display weapon icon
      COLOR colBorder = _colHUD;
      colIcon = 0xccddff00;
      // weapon that is currently selected has different colors
      if (ptoWantedWeapon == _awiWeapons[i].wi_ptoWeapon) {
        colIcon = 0xffcc0000;
        colBorder = 0xffcc0000;
      }
      // no ammo
      if (_awiWeapons[i].wi_paiAmmo!=NULL && _awiWeapons[i].wi_paiAmmo->ai_iAmmoAmmount==0) {
        HUD_DrawBorder( fCol, fRow, fOneUnit, fOneUnit, 0x22334400);
        HUD_DrawIcon(   fCol, fRow, *_awiWeapons[i].wi_ptoWeapon, 0x22334400, 1.0f, FALSE, 32, 32);
      // yes ammo
      } else {
        HUD_DrawBorder( fCol, fRow, fOneUnit, fOneUnit, colBorder);
        HUD_DrawIcon(   fCol, fRow, *_awiWeapons[i].wi_ptoWeapon, colIcon, 1.0f, FALSE, 32, 32);
      }
      // advance to next position
      fCol += fAdvUnit;
    }
  }


  // reduce icon sizes a bit
  const FLOAT fUpperSize = ClampDn(_fCustomScaling*0.5f, 0.5f)/_fCustomScaling;
  _fCustomScaling*=fUpperSize;
  ASSERT( _fCustomScaling>=0.5f);
  fChrUnit  *= fUpperSize;
  fOneUnit  *= fUpperSize;
  fHalfUnit *= fUpperSize;
  fAdvUnit  *= fUpperSize;
  fNextUnit *= fUpperSize;

  
  BOOL bOxygenOnScreen = FALSE;
  const TIME tmMaxHoldBreath = _penPlayer->en_tmMaxHoldBreath;
  
  if (tmMaxHoldBreath > 0.0F)
  {
    fValue = tmMaxHoldBreath - (_pTimer->CurrentTick() - _penPlayer->en_tmLastBreathed);

    if (_penPlayer->IsConnected() && (_penPlayer->GetFlags()&ENF_ALIVE) && fValue < (tmMaxHoldBreath - 1.0F)) {
      // prepare and draw oxygen info
      fRow = pixTopBound + fOneUnit + fNextUnit;
      fCol = 280.0f;
      fAdv = fAdvUnit + fOneUnit*4/2 - fHalfUnit;
      HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
      fNormValue = fValue / tmMaxHoldBreath;
      fNormValue = ClampDn(fNormValue, 0.0f);

      HUD_DrawBorder( fCol,      fRow, fOneUnit,         fOneUnit, colBorder);
      HUD_DrawBorder( fCol+fAdv, fRow, fOneUnit*4,       fOneUnit, colBorder);
      HUD_DrawBar(    fCol+fAdv, fRow, fOneUnit*4*0.975, fOneUnit*0.9375, BO_LEFT, NONE, fNormValue);
      HUD_DrawIcon(   fCol,      fRow, _pHAC->m_toOxygen, C_WHITE /*_colHUD*/, fNormValue, TRUE, 32, 32);
      bOxygenOnScreen = TRUE;
    }
  }

  // draw boss energy if needed
  if (_penPlayer->m_penMainMusicHolder!=NULL) {
    CMusicHolder &mh = (CMusicHolder&)*_penPlayer->m_penMainMusicHolder;
    fNormValue = 0;

    if (mh.m_penBoss!=NULL && (mh.m_penBoss->en_ulFlags&ENF_ALIVE)) {
      CEnemyBase &eb = (CEnemyBase&)*mh.m_penBoss;
      ASSERT( eb.m_fMaxHealth>0);
      fValue = eb.GetHealth();
      fNormValue = fValue/eb.m_fMaxHealth;
    }
    
    CTextureObject *ptoBossIcon = &_pHAC->m_toHealth;
    
    // Q6785
    // [SSE] HudBosBarDisplay Entity
    if (mh.m_penBossBarDisplay != NULL) {
      CHudBossBarDisplay &bbd = (CHudBossBarDisplay&)*mh.m_penBossBarDisplay;
      
      if (bbd.m_iValueIO > 0) {
        fValue = bbd.m_iValueIO;
        fNormValue = fValue / bbd.m_iMaxValue;
      }
      
      if (bbd.m_moTextureHolder.mo_toTexture.GetData() != NULL) {
        ptoBossIcon = &bbd.m_moTextureHolder.mo_toTexture;
      }
      
    } else if (mh.m_penCounter != NULL) {
      CEnemyCounter &ec = (CEnemyCounter&)*mh.m_penCounter;

      if (ec.m_iCount > 0) {
        fValue = ec.m_iCount;
        fNormValue = fValue / ec.m_iCountFrom;
      }
    }
    
    fNormValue = Clamp(fNormValue, 0.0F, 1.0F);

    if (fNormValue > 0)
    {
      // prepare and draw boss energy info
      //HUD_PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
      HUD_PrepareColorTransitions( colMax, colMax, colTop, C_RED, 0.5f, 0.25f, FALSE);

      fRow = pixTopBound + fOneUnit + fNextUnit;
      fCol = 184.0f;
      fAdv = fAdvUnit+ fOneUnit*16/2 -fHalfUnit;
      if (bOxygenOnScreen) fRow += fNextUnit;
      HUD_DrawBorder( fCol,      fRow, fOneUnit,          fOneUnit, colBorder);
      HUD_DrawBorder( fCol+fAdv, fRow, fOneUnit*16,       fOneUnit, colBorder);
      HUD_DrawBar(    fCol+fAdv, fRow, fOneUnit*16*0.995, fOneUnit*0.9375, BO_LEFT, NONE, fNormValue);
      HUD_DrawIcon(   fCol,      fRow, *ptoBossIcon, C_WHITE /*_colHUD*/, fNormValue, FALSE, 32, 32);
    }
  }


  // determine scaling of normal text and play mode
  const FLOAT fTextScale  = (_fResolutionScaling+1) *0.5f;
  const BOOL bSinglePlay  =  GetSP()->sp_bSinglePlayer;
  const BOOL bCooperative =  GetSP()->sp_bCooperative && !bSinglePlay;
  const BOOL bScoreMatch  = !GetSP()->sp_bCooperative && !GetSP()->sp_bUseFrags;
  const BOOL bFragMatch   = !GetSP()->sp_bCooperative &&  GetSP()->sp_bUseFrags;
  const BOOL bTeamDeathMatch = (GetSP()->sp_ulGameModeFlags & GMF_TEAMPLAY) && GetSP()->sp_bUseFrags; // [SSE] Team DeathMatch
  
  const INDEX ctTeams = GetSP()->sp_ctTeams;
  
  COLOR colMana, colFrags, colDeaths, colHealth, colArmor, colPing;
  COLOR colScore  = _colHUD;
  INDEX iScoreSum = 0;

  // if not in single player mode, we'll have to calc (and maybe printout) other players' info
  if (!bSinglePlay)
  {
    // set font and prepare font parameters
    _pfdDisplayFont->SetVariableWidth();
    _pDP->SetFont( _pfdDisplayFont);
    _pDP->SetTextScaling( fTextScale);
    FLOAT fCharHeight = (_pfdDisplayFont->GetHeight()-2)*fTextScale;
    // generate and sort by mana list of active players
    BOOL bMaxScore=TRUE, bMaxMana=TRUE, bMaxFrags=TRUE, bMaxDeaths=TRUE;
    hud_iSortPlayers = Clamp( hud_iSortPlayers, -1L, 6L);
    SortKeys eKey = (SortKeys)hud_iSortPlayers;
    if (hud_iSortPlayers==-1) {
           if (bCooperative) eKey = PSK_HEALTH;
      else if (bScoreMatch)  eKey = PSK_SCORE;
      else if (bFragMatch)   eKey = PSK_FRAGS;
      else { ASSERT(FALSE);  eKey = PSK_NAME; }
    }
    if (bCooperative) eKey = (SortKeys)Clamp( (INDEX)eKey, 0L, 3L);
    if (eKey==PSK_HEALTH && (bScoreMatch || bFragMatch)) { eKey = PSK_NAME; }; // prevent health snooping in deathmatch
    INDEX iPlayers = HUD_SetAllPlayersStats(eKey);

    // loop thru players
    for (INDEX i=0; i<iPlayers; i++)
    {
      // get player name and mana
      CPlayer *penPlayer = _apenPlayers[i];
      const CTString strName = penPlayer->GetPlayerName();
      const INDEX iScore  = penPlayer->m_psGameStats.ps_iScore;
      const INDEX iMana   = penPlayer->m_iMana;
      const INDEX iFrags  = penPlayer->m_psGameStats.ps_iKills;
      const INDEX iDeaths = penPlayer->m_psGameStats.ps_iDeaths;
      const INDEX iHealth = ClampDn( (INDEX)ceil( penPlayer->GetHealth()), 0L);
      const INDEX iArmor  = ClampDn( (INDEX)ceil( penPlayer->m_fArmor),    0L);
      const INDEX iPing = ceil(penPlayer->en_tmPing*1000.0f);

      CTString strScore, strMana, strFrags, strDeaths, strHealth, strArmor, strPing;
      strScore.PrintF(  "%d", iScore);
      strMana.PrintF(   "%d", iMana);
      strFrags.PrintF(  "%d", iFrags);
      strDeaths.PrintF( "%d", iDeaths);
      
      // [SSE]
      if (iArmor >= 10000) {
        strArmor.PrintF( "%.1fk", iArmor / 1000.0F);
      } else if (iArmor >= 1000) {
        strArmor.PrintF( "%.2fk", iArmor / 1000.0F);
      } else {
        strArmor.PrintF( "%d", iArmor);
      }

      if (iHealth >= 10000) {
        strHealth.PrintF( "%.1fk", iHealth / 1000.0F);
      } else if (iHealth >= 1000) {
        strHealth.PrintF( "%.2fk", iHealth / 1000.0F);
      } else {
        strHealth.PrintF( "%d", iHealth);
      }
      //
      
			if ( iPing > 300 ) {
				colPing = C_RED;
        strPing.PrintF("^f9/^r^c808080///");

      } else if (iPing > 200) {
				colPing = C_RED;
        strPing.PrintF("/^c808080///");
        
      } else if (iPing > 140) {
				colPing = C_ORANGE;
        strPing.PrintF("//^c808080//");

			} else if ( iPing > 80 ) {
				colPing = C_YELLOW;
        strPing.PrintF("///^c808080/");

			} else {
        strPing.PrintF("////");
				colPing = C_GREEN;
			}

      // detemine corresponding colors
      colHealth = C_mlRED;
      colMana = colScore = colFrags = colDeaths = colArmor = C_lGRAY;
      if (iMana   > _penPlayer->m_iMana)                      { bMaxMana   = FALSE; colMana   = C_WHITE; }
      if (iScore  > _penPlayer->m_psGameStats.ps_iScore)      { bMaxScore  = FALSE; colScore  = C_WHITE; }
      if (iFrags  > _penPlayer->m_psGameStats.ps_iKills)      { bMaxFrags  = FALSE; colFrags  = C_WHITE; }
      if (iDeaths > _penPlayer->m_psGameStats.ps_iDeaths)     { bMaxDeaths = FALSE; colDeaths = C_WHITE; }
      if (penPlayer == _penPlayer) colScore = colMana = colFrags = colDeaths = _colHUD; // current player
      if (iHealth > 25) colHealth = _colHUD;
      if (iArmor  > 25) colArmor  = _colHUD;

      // eventually print it out
      if (hud_iShowPlayers==1 || hud_iShowPlayers==-1 && !bSinglePlay)
      {
        // printout location and info aren't the same for deathmatch and coop play
        const FLOAT fCharWidth = (PIX)((_pfdDisplayFont->GetWidth()-2) *fTextScale);

        if (bCooperative) {
          _pDP->PutTextR( strName+":", _pixDPWidth - 12 *fCharWidth, fCharHeight*i+fOneUnit*2, colScore |_ulAlphaHUD);
          
          if (penPlayer->GetFlags()&ENF_ALIVE) {
            _pDP->PutText(  "/",         _pixDPWidth - 8*fCharWidth, fCharHeight*i+fOneUnit*2, _colHUD  |_ulAlphaHUD);
            _pDP->PutTextC( strHealth,   _pixDPWidth - 10*fCharWidth, fCharHeight*i+fOneUnit*2, colHealth|_ulAlphaHUD);
            _pDP->PutTextC( strArmor,    _pixDPWidth - 6*fCharWidth, fCharHeight*i+fOneUnit*2, colArmor |_ulAlphaHUD);
          } else {
            _pDP->PutTextC(  TRANS("DEAD"), _pixDPWidth-8*fCharWidth, fCharHeight*i+fOneUnit*2, C_RED|_ulAlphaHUD); // [SSE] No Stupid Zeros If Player Dead
          }
          
          _pDP->PutTextC( strPing,     _pixDPWidth - 2 * fCharWidth, fCharHeight * i + fOneUnit * 2, colPing|_ulAlphaHUD);

        } else if (bScoreMatch) {
          _pDP->PutTextR( strName+":", _pixDPWidth - 14 * fCharWidth, fCharHeight*i+fOneUnit*2, _colHUD |_ulAlphaHUD);
          _pDP->PutText(  "/",         _pixDPWidth - 9 * fCharWidth, fCharHeight*i+fOneUnit*2, _colHUD |_ulAlphaHUD);
          _pDP->PutTextC( strScore,    _pixDPWidth - 12 * fCharWidth, fCharHeight*i+fOneUnit*2, colScore|_ulAlphaHUD);
          _pDP->PutTextC( strMana,     _pixDPWidth - 6 * fCharWidth, fCharHeight*i+fOneUnit*2, colMana |_ulAlphaHUD);
          _pDP->PutTextC( strPing,     _pixDPWidth - 2 * fCharWidth, fCharHeight * i + fOneUnit * 2, colPing|_ulAlphaHUD);

        } else { // fragmatch!
          CTString strNameString;
        
          // [SSE]
          if (bTeamDeathMatch) {
            INDEX iTeamID = penPlayer->m_iTeamID;
            
            switch (iTeamID)
            {
              case 1: strNameString.PrintF("^c7F7FFF> ^r%s", strName); break; // Blue
              case 2: strNameString.PrintF("^cFF7F7F> ^r%s", strName); break; // Red
              case 3: strNameString.PrintF("^c7FFF7F> ^r%s", strName); break; // Green
              case 4: strNameString.PrintF("^cFFFF7F> ^r%s", strName); break; // Blue
              
              default: strNameString.PrintF("^cAAAAAA> ^r%s", strName); break; // Unassigned - Gray
            }
          } else {
            strNameString = strName;
          }
        
          _pDP->PutTextR( strNameString+":", _pixDPWidth-12 * fCharWidth, fCharHeight * i + fOneUnit * 2, _colHUD  |_ulAlphaHUD);
          _pDP->PutText(  "/",         _pixDPWidth-8 * fCharWidth, fCharHeight * i + fOneUnit * 2, _colHUD  |_ulAlphaHUD);
          _pDP->PutTextC( strFrags,    _pixDPWidth-10 * fCharWidth, fCharHeight * i + fOneUnit * 2, colFrags |_ulAlphaHUD);
          _pDP->PutTextC( strDeaths,   _pixDPWidth-6 * fCharWidth, fCharHeight * i + fOneUnit * 2, colDeaths|_ulAlphaHUD);
          _pDP->PutTextC( strPing,      _pixDPWidth-2 * fCharWidth, fCharHeight * i + fOneUnit * 2, colPing|_ulAlphaHUD);
        }
      }

      // calculate summ of scores (for coop mode)
      iScoreSum += iScore;
    }

    // draw remaining time if time based death- or scorematch
    if ((bScoreMatch || bFragMatch) && hud_bShowMatchInfo)
    {
      CTString strLimitsInfo = "";

      if (GetSP()->sp_iTimeLimit > 0) {
        FLOAT fTimeLeft = ClampDn(GetSP()->sp_iTimeLimit*60.0f - _pNetwork->GetGameTime(), 0.0f);
        strLimitsInfo.PrintF("%s^cFFFFFF%s: %s\n", strLimitsInfo, TRANS("TIME LEFT"), TimeToString(fTimeLeft));
      }

      extern INDEX HUD_SetAllPlayersStats( INDEX iSortKey);
      // fill players table
      const INDEX ctPlayers = HUD_SetAllPlayersStats(bFragMatch?5:3); // sort by frags or by score
      // find maximum frags/score that one player has
      INDEX iMaxFrags = LowerLimit(INDEX(0));
      INDEX iMaxScore = LowerLimit(INDEX(0));

      {for(INDEX iPlayer=0; iPlayer<ctPlayers; iPlayer++) {
        CPlayer *penPlayer = _apenPlayers[iPlayer];
        iMaxFrags = Max(iMaxFrags, penPlayer->m_psLevelStats.ps_iKills);
        iMaxScore = Max(iMaxScore, penPlayer->m_psLevelStats.ps_iScore);
      }}

      // [SSE] TeamDeathmatch
      if (bTeamDeathMatch)
      {        
        iMaxFrags = Max(GetSP()->sp_iTeamScore1, GetSP()->sp_iTeamScore2);
        
        if (ctTeams >= 3) iMaxFrags = Max(iMaxFrags, GetSP()->sp_iTeamScore3);
        if (ctTeams >= 4) iMaxFrags = Max(iMaxFrags, GetSP()->sp_iTeamScore4);
      }

      if (GetSP()->sp_iFragLimit>0) {
        INDEX iFragsLeft = ClampDn(GetSP()->sp_iFragLimit-iMaxFrags, INDEX(0));
        strLimitsInfo.PrintF("%s^cFFFFFF%s: %d\n", strLimitsInfo, TRANS("FRAGS LEFT"), iFragsLeft);
      }

      if (GetSP()->sp_iScoreLimit>0) {
        INDEX iScoreLeft = ClampDn(GetSP()->sp_iScoreLimit-iMaxScore, INDEX(0));
        strLimitsInfo.PrintF("%s^cFFFFFF%s: %d\n", strLimitsInfo, TRANS("SCORE LEFT"), iScoreLeft);
      }

      _pfdDisplayFont->SetFixedWidth();
      _pDP->SetFont( _pfdDisplayFont);
      _pDP->SetTextScaling( fTextScale*0.8f );
      _pDP->SetTextCharSpacing( -2.0f*fTextScale);
      _pDP->PutText( strLimitsInfo, 5.0f*_pixDPWidth/640.0f, 48.0f*_pixDPWidth/640.0f, C_WHITE|CT_OPAQUE);
    }


    // prepare color for local player printouts
    bMaxScore  ? colScore  = C_WHITE : colScore  = C_lGRAY;
    bMaxMana   ? colMana   = C_WHITE : colMana   = C_lGRAY;
    bMaxFrags  ? colFrags  = C_WHITE : colFrags  = C_lGRAY;
    bMaxDeaths ? colDeaths = C_WHITE : colDeaths = C_lGRAY;
  }

  // printout player latency if needed
  if (hud_bShowLatency) {
    CTString strLatency;
    strLatency.PrintF( "%4.0fms", _penPlayer->m_tmLatency*1000.0f);
    PIX pixFontHeight = (PIX)(_pfdDisplayFont->GetHeight() *fTextScale +fTextScale+1);
    _pfdDisplayFont->SetFixedWidth();
    _pDP->SetFont( _pfdDisplayFont);
    _pDP->SetTextScaling( fTextScale);
    _pDP->SetTextCharSpacing( -2.0f*fTextScale);
    _pDP->PutTextR( strLatency, _pixDPWidth, _pixDPHeight-pixFontHeight, C_WHITE|CT_OPAQUE);
  }

  // restore font defaults
  _pfdDisplayFont->SetVariableWidth();
  _pDP->SetFont( &_fdNumbersFont);
  _pDP->SetTextCharSpacing(1);

  // prepare output strings and formats depending on game type
  FLOAT fWidthAdj = 8;
  INDEX iScore = _penPlayer->m_psGameStats.ps_iScore;
  INDEX iMana  = _penPlayer->m_iMana;
  if (bFragMatch) {
    if (!hud_bShowMatchInfo) { fWidthAdj = 4; }
    iScore = _penPlayer->m_psGameStats.ps_iKills;
    iMana  = _penPlayer->m_psGameStats.ps_iDeaths;
  } else if (bCooperative) {
    // in case of coop play, show squad (common) score
    iScore = iScoreSum;
  }

  // prepare and draw score or frags info
  strValue.PrintF( "%d", iScore);
  fRow = pixTopBound + fHalfUnit;
  fCol = pixLeftBound + fHalfUnit;
  fAdv = fAdvUnit+ fChrUnit*fWidthAdj/2 -fHalfUnit;
  HUD_DrawBorder( fCol,      fRow, fOneUnit,           fOneUnit, colBorder);
  HUD_DrawBorder( fCol+fAdv, fRow, fChrUnit*fWidthAdj, fOneUnit, colBorder);
  HUD_DrawText(   fCol+fAdv, fRow, strValue, colScore, 1.0f);
  HUD_DrawIcon(   fCol,      fRow, _pHAC->m_toFrags, C_WHITE /*colScore*/, 1.0f, FALSE, 32, 32);
  
  // [SSE] Extra Lives System
  if (bCooperative && GetSP()->sp_ctCredits >= 0)
  {
    strValue.PrintF( "%d", bSharedLives ? GetSP()->sp_ctCreditsLeft : _penPlayer->m_iLives);
    fRow = pixTopBound  + fNextUnit+fHalfUnit;
    fCol = pixLeftBound + fHalfUnit;
    fAdv = fAdvUnit+ fChrUnit*fWidthAdj/2 -fHalfUnit;

    HUD_DrawBorder( fCol,      fRow, fOneUnit,           fOneUnit, colBorder);
    HUD_DrawBorder( fCol+fAdv, fRow, fChrUnit*fWidthAdj, fOneUnit, colBorder);
    HUD_DrawText(   fCol+fAdv, fRow, strValue,  C_lGRAY, 1.0f);
    HUD_DrawIcon(   fCol,      fRow, _pHAC->m_toExtraLive, C_WHITE /*colMana*/, 1.0f, FALSE, 32, 32);
    
    if (GetSP()->sp_iScoreForExtraLive > 0)
    {
      CTString strLimitsInfo;
      
      FLOAT fLiveCostMultipleir = bSharedLives ? GetSP()->sp_fLiveCostMultiplier : _penPlayer->m_fLiveCostMultiplier;
      INDEX iLiveCost = GetSP()->sp_iScoreForExtraLive * fLiveCostMultipleir;
      INDEX iAccumulateScore = bSharedLives ? GetSP()->sp_iScoreForExtraLiveAccum : _penPlayer->m_iScoreAccumulated;
      INDEX iScoreLeft = ClampDn(iLiveCost - iAccumulateScore, INDEX(0));

      strLimitsInfo.PrintF("%s^cFFFFFF%s: %d\n", strLimitsInfo, TRANS("SCORE LEFT"), iScoreLeft);
      
      _pfdDisplayFont->SetFixedWidth();
      _pDP->SetFont( _pfdDisplayFont);
      _pDP->SetTextScaling( fTextScale*0.8f );
      _pDP->SetTextCharSpacing( -2.0f*fTextScale);
      _pDP->PutText( strLimitsInfo, 5.0f*_pixDPWidth/640.0f, 48.0f*_pixDPWidth/640.0f, C_WHITE|CT_OPAQUE);
      
      _pfdDisplayFont->SetVariableWidth();
      _pDP->SetFont( &_fdNumbersFont);
      _pDP->SetTextCharSpacing(1);
    }
  }
  //

  // eventually draw mana info
  if (bScoreMatch || bFragMatch)
  {
    strValue.PrintF( "%d", iMana);
    fRow = pixTopBound  + fNextUnit+fHalfUnit;
    fCol = pixLeftBound + fHalfUnit;
    fAdv = fAdvUnit+ fChrUnit*fWidthAdj/2 -fHalfUnit;

    HUD_DrawBorder( fCol,      fRow, fOneUnit,           fOneUnit, colBorder);
    HUD_DrawBorder( fCol+fAdv, fRow, fChrUnit*fWidthAdj, fOneUnit, colBorder);
    HUD_DrawText(   fCol+fAdv, fRow, strValue,  colMana, 1.0f);
    HUD_DrawIcon(   fCol,      fRow, _pHAC->m_toDeaths, C_WHITE /*colMana*/, 1.0f, FALSE, 32, 32);
  }

  // if single player or cooperative mode
  if (bSinglePlay || bCooperative)
  {
    // prepare and draw hiscore info
    strValue.PrintF( "%d", Max(_penPlayer->m_iHighScore, _penPlayer->m_psGameStats.ps_iScore));
    BOOL bBeating = _penPlayer->m_psGameStats.ps_iScore>_penPlayer->m_iHighScore;
    fRow = pixTopBound+fHalfUnit;
    fCol = 320.0f-fOneUnit-fChrUnit*8/2;
    fAdv = fAdvUnit+ fChrUnit*8/2 -fHalfUnit;
    HUD_DrawBorder( fCol,      fRow, fOneUnit,   fOneUnit, colBorder);
    HUD_DrawBorder( fCol+fAdv, fRow, fChrUnit*8, fOneUnit, colBorder);
    HUD_DrawText(   fCol+fAdv, fRow, strValue, NONE, bBeating ? 0.0f : 1.0f);
    HUD_DrawIcon(   fCol,      fRow, _pHAC->m_toHiScore, C_WHITE /*_colHUD*/, 1.0f, FALSE, 32, 32);

    // prepare and draw unread messages
    if (hud_bShowMessages && _penPlayer->m_ctUnreadMessages > 0) {
      strValue.PrintF( "%d", _penPlayer->m_ctUnreadMessages);
      fRow = pixTopBound+fHalfUnit;
      fCol = pixRightBound-fHalfUnit-fAdvUnit-fChrUnit*4;
      const FLOAT tmIn = 0.5f;
      const FLOAT tmOut = 0.5f;
      const FLOAT tmStay = 2.0f;
      FLOAT tmDelta = _pTimer->GetLerpedCurrentTick()-_penPlayer->m_tmAnimateInbox;
      COLOR col = _colHUD;
      if (tmDelta > 0 && tmDelta<(tmIn+tmStay+tmOut) && bSinglePlay) {
        FLOAT fRatio = 0.0f;
        if (tmDelta < tmIn) {
          fRatio = tmDelta/tmIn;
        } else if (tmDelta>tmIn+tmStay) {
          fRatio = (tmIn+tmStay+tmOut-tmDelta)/tmOut;
        } else {
          fRatio = 1.0f;
        }
        fRow+=fAdvUnit*5*fRatio;
        fCol-=fAdvUnit*15*fRatio;
        col = LerpColor(_colHUD, C_WHITE|0xFF, fRatio);
      }
      fAdv = fAdvUnit+ fChrUnit*4/2 -fHalfUnit;
      HUD_DrawBorder( fCol,      fRow, fOneUnit,   fOneUnit, col);
      HUD_DrawBorder( fCol+fAdv, fRow, fChrUnit*4, fOneUnit, col);
      HUD_DrawText(   fCol+fAdv, fRow, strValue,   col, 1.0f);
      HUD_DrawIcon(   fCol,      fRow, _pHAC->m_toMessage, C_WHITE /*col*/, 0.0f, TRUE, 32 ,32);
    }
  }

  #ifdef ENTITY_DEBUG
  // if entity debug is on, draw entity stack
  HUD_DrawEntityStack();
  #endif

  // draw cheat modes if singleplayer
  if (GetSP()->sp_ctMaxPlayers == 1) {
    INDEX iLine=1;
    ULONG ulAlpha = sin(_tmNow*16)*96 +128;
    PIX pixFontHeight = _pfdConsoleFont->fd_pixCharHeight;
    const COLOR colCheat = _colHUDText;
    _pDP->SetFont( _pfdConsoleFont);
    _pDP->SetTextScaling( 1.0f);
    const FLOAT fchtTM = cht_fTranslationMultiplier; // for text formatting sake :)

    if (fchtTM > 1.0f)     { _pDP->PutTextR( "turbo",     _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bGhost)        { _pDP->PutTextR( "ghost",     _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bFly)          { _pDP->PutTextR( "fly",       _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bGod)          { _pDP->PutTextR( "god",       _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bInfiniteAmmo) { _pDP->PutTextR( "noammo",    _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bAutoKill)     { _pDP->PutTextR( "autokill",  _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bInvisible)    { _pDP->PutTextR( "invisible", _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
  }

  // in the end, remember the current time so it can be used in the next frame
  _tmLast = _tmNow;

  if (hud_bGameDebugMonitor)
  {
    HUD_DrawDebugMonitor();
  }
}

static void InitDefaultTexture()
{
  /*
  if (_tdDefault == NULL)
  {
    PIX pixDefaultWidth = 128;
    PIX pixDefaultHeight = 128;
    INDEX ctDefaultBPP = 24;
    
    _iiDefault.ii_Width = pixDefaultWidth;
    _iiDefault.ii_Height = pixDefaultHeight;
    _iiDefault.ii_BitsPerPixel = 24;
    
    _iiDefault.ii_Picture = (UBYTE*)AllocMemory( pixDefaultWidth * pixDefaultHeight * ctDefaultBPP);
    
    for (INDEX j = 0; j < pixDefaultHeight * ctDefaultBPP/8; j++)
    {
      for (INDEX i = 0; i < pixDefaultWidth * ctDefaultBPP/8; i++)
      {
        _iiDefault.ii_Picture[j * pixDefaultWidth + i] = 0xFF;
        
        
        if (i < pixDefaultWidth / 2) {
          CPrintF("WHITE\n");
        } else {
          CPrintF("BLACK\n");
          _iiDefault.ii_Picture[j * pixDefaultWidth + i] = 0x00;
        }
      }
    }
    
    _tdDefault = new CTextureData();
    _tdDefault->Create_t( &_iiDefault, 128, MAX_MEX_LOG2, FALSE);
  }*/
}


static void HUD_RegisterTexture(CTextureObject *pto, const CTFileName &fnmTexture)
{
  CHUDTextureEntry *pte = new CHUDTextureEntry();
  pte->he_fnTexture = fnmTexture;
  pte->he_ptoTexture = pto;
  
  _lhAllHUDTextures.AddTail(pte->he_lnNode);
}

static void HUD_LoadTextures()
{
  FOREACHINLIST(CHUDTextureEntry, he_lnNode, _lhAllHUDTextures, itthe)
  {
    CHUDTextureEntry &hte = *itthe;
    
    if (hte.he_ptoTexture == NULL) {
      FatalError("HUD : Was added CHUDTextureEntry with he_ptoTexture == NULL!");
      break;
    }
    
    try {
      hte.he_ptoTexture->SetData_t(hte.he_fnTexture);
    } catch( char *strError) {
      CPrintF("  %s\n", strError);
      hte.he_ptoTexture->SetData(NULL);
    }
  }
}

static void HUD_ForceTextures()
{
  FOREACHINLIST(CHUDTextureEntry, he_lnNode, _lhAllHUDTextures, itthe)
  {
    CHUDTextureEntry &hte = *itthe;
    
    if (hte.he_ptoTexture == NULL) {
      FatalError("HUD : Was added CHUDTextureEntry with he_ptoTexture == NULL!");
      break;
    }
    
    if (hte.he_ptoTexture->GetData() == NULL) {
      continue;
    }
    
    ((CTextureData*)hte.he_ptoTexture->GetData())->Force(TEX_CONSTANT);
  }
}

static BOOL _bTexturesRegistered = FALSE;

static void HUD_RegisterTextures()
{
  if (_bTexturesRegistered) {
    return;
  }
  
  // initialize tile texture
  HUD_RegisterTexture(&_pHAC->m_toTile, CTFILENAME("Textures\\Interface\\Tile.tex"));
  
  HUD_RegisterTexture(&_pHAC->m_toHealth,  CTFILENAME("Textures\\Interface\\HSuper.tex"));
  HUD_RegisterTexture(&_pHAC->m_toOxygen,  CTFILENAME("Textures\\Interface\\Oxygen-2.tex"));
  HUD_RegisterTexture(&_pHAC->m_toFrags,   CTFILENAME("Textures\\Interface\\IBead.tex"));
  HUD_RegisterTexture(&_pHAC->m_toDeaths,  CTFILENAME("Textures\\Interface\\ISkull.tex"));
  HUD_RegisterTexture(&_pHAC->m_toScore,   CTFILENAME("Textures\\Interface\\IScore.tex"));
  HUD_RegisterTexture(&_pHAC->m_toHiScore, CTFILENAME("Textures\\Interface\\IHiScore.tex"));
  HUD_RegisterTexture(&_pHAC->m_toMessage, CTFILENAME("Textures\\Interface\\IMessage.tex"));
  HUD_RegisterTexture(&_pHAC->m_toMana,    CTFILENAME("Textures\\Interface\\IValue.tex"));

  HUD_RegisterTexture(&_pHAC->m_toArmorSmall,  CTFILENAME("Textures\\Interface\\ArSmall.tex"));
  HUD_RegisterTexture(&_pHAC->m_toArmorMedium, CTFILENAME("Textures\\Interface\\ArMedium.tex"));
  HUD_RegisterTexture(&_pHAC->m_toArmorLarge,  CTFILENAME("Textures\\Interface\\ArStrong.tex"));
  
  HUD_RegisterTexture(&_pHAC->m_toExtraLive,  CTFILENAME("Textures\\Interface\\IExtraLive.tex"));
  HUD_RegisterTexture(&_pHAC->m_toSwords,     CTFILENAME("Textures\\Interface\\ISwords.tex"));
  HUD_RegisterTexture(&_pHAC->m_toKey,        CTFILENAME("Textures\\Interface\\IKey.tex")); // [SSE] Better Keys
  HUD_RegisterTexture(&_pHAC->m_toClock,      CTFILENAME("Textures\\Interface\\IClock.tex"));
  HUD_RegisterTexture(&_pHAC->m_toShields,    CTFILENAME("Textures\\Interface\\Shields.tex")); // [SSE] Shields
  HUD_RegisterTexture(&_pHAC->m_toShieldsTop, CTFILENAME("Textures\\Interface\\ShieldsTop.tex")); // [SSE] Shields

  // [SSE] Radar
  HUD_RegisterTexture(&_pHAC->m_toRadarCircle,  CTFILENAME("Textures\\Interface\\RadarCircle.tex"));
  HUD_RegisterTexture(&_pHAC->m_toRadarMask,    CTFILENAME("Textures\\Interface\\RadarMask.tex"));
  HUD_RegisterTexture(&_pHAC->m_toRadarBorder,  CTFILENAME("Textures\\Interface\\RadarBorder.tex"));
  HUD_RegisterTexture(&_pHAC->m_toRadarDot,     CTFILENAME("Textures\\Interface\\RadarDot.tex"));
  HUD_RegisterTexture(&_pHAC->m_toRadarDotUp,   CTFILENAME("Textures\\Interface\\RadarDotUp.tex"));
  HUD_RegisterTexture(&_pHAC->m_toRadarDotDown, CTFILENAME("Textures\\Interface\\RadarDotDown.tex"));
  //

  // initialize ammo textures
  HUD_RegisterTexture(&_pHAC->m_toAColt,          CTFILENAME("Textures\\Interface\\AmColt.tex"));

  HUD_RegisterTexture(&_pHAC->m_atoAmmo[0], CTFILENAME("Textures\\Interface\\AmShells.tex"));
  HUD_RegisterTexture(&_pHAC->m_atoAmmo[1], CTFILENAME("Textures\\Interface\\AmBullets.tex"));
  HUD_RegisterTexture(&_pHAC->m_atoAmmo[2], CTFILENAME("Textures\\Interface\\AmRockets.tex"));
  HUD_RegisterTexture(&_pHAC->m_atoAmmo[3], CTFILENAME("Textures\\Interface\\AmGrenades.tex"));
  HUD_RegisterTexture(&_pHAC->m_atoAmmo[4], CTFILENAME("Textures\\Interface\\AmFuelReservoir.tex"));
  HUD_RegisterTexture(&_pHAC->m_atoAmmo[5], CTFILENAME("Textures\\Interface\\AmElectricity.tex"));
  HUD_RegisterTexture(&_pHAC->m_atoAmmo[6], CTFILENAME("Textures\\Interface\\AmCannonBall.tex"));
  HUD_RegisterTexture(&_pHAC->m_atoAmmo[7], CTFILENAME("Textures\\Interface\\AmSniperBullets.tex"));
  HUD_RegisterTexture(&_pHAC->m_atoAmmo[8], CTFILENAME("Textures\\Interface\\AmSeriousBomb.tex"));

  // initialize weapon textures
  HUD_RegisterTexture(&_pHAC->m_toWFists,           CTFILENAME("Textures\\Interface\\WFists.tex")); // [SSE] Fists Weapon
  HUD_RegisterTexture(&_pHAC->m_toWKnife,           CTFILENAME("Textures\\Interface\\WKnife.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWColt,            CTFILENAME("Textures\\Interface\\WColt.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWSingleShotgun,   CTFILENAME("Textures\\Interface\\WSingleShotgun.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWDoubleShotgun,   CTFILENAME("Textures\\Interface\\WDoubleShotgun.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWTommygun,        CTFILENAME("Textures\\Interface\\WTommygun.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWMinigun,         CTFILENAME("Textures\\Interface\\WMinigun.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWRocketLauncher,  CTFILENAME("Textures\\Interface\\WRocketLauncher.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWGrenadeLauncher, CTFILENAME("Textures\\Interface\\WGrenadeLauncher.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWLaser,           CTFILENAME("Textures\\Interface\\WLaser.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWIronCannon,      CTFILENAME("Textures\\Interface\\WCannon.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWChainsaw,        CTFILENAME("Textures\\Interface\\WChainsaw.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWSniper,          CTFILENAME("Textures\\Interface\\WSniper.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWFlamer,          CTFILENAME("Textures\\Interface\\WFlamer.tex"));
  HUD_RegisterTexture(&_pHAC->m_toWSeriousBomb,     CTFILENAME("Textures\\Interface\\WSeriousBomb.tex")); // [SSE] Gameplay - Serious Bomb Weapon

  // initialize powerup textures (DO NOT CHANGE ORDER!)
  HUD_RegisterTexture(&_pHAC->m_atoPowerups[0],    CTFILENAME("Textures\\Interface\\PInvisibility.tex"));
  HUD_RegisterTexture(&_pHAC->m_atoPowerups[1],    CTFILENAME("Textures\\Interface\\PInvulnerability.tex"));
  HUD_RegisterTexture(&_pHAC->m_atoPowerups[2],    CTFILENAME("Textures\\Interface\\PSeriousDamage.tex"));
  HUD_RegisterTexture(&_pHAC->m_atoPowerups[3],    CTFILENAME("Textures\\Interface\\PSeriousSpeed.tex"));

  // initialize sniper mask texture
  HUD_RegisterTexture(&_pHAC->m_toSniperMask,   CTFILENAME("Textures\\Interface\\SniperMask.tex"));
  HUD_RegisterTexture(&_pHAC->m_toSniperWheel,  CTFILENAME("Textures\\Interface\\SniperWheel.tex"));
  HUD_RegisterTexture(&_pHAC->m_toSniperArrow,  CTFILENAME("Textures\\Interface\\SniperArrow.tex"));
  HUD_RegisterTexture(&_pHAC->m_toSniperEye,    CTFILENAME("Textures\\Interface\\SniperEye.tex"));
  HUD_RegisterTexture(&_pHAC->m_toSniperLed,    CTFILENAME("Textures\\Interface\\SniperLed.tex"));
  
  _bTexturesRegistered = TRUE;
}

// --------------------------------------------------------------------------------------
// Initialize all that's needed for drawing the HUD.
// --------------------------------------------------------------------------------------
extern void InitHUD(void)
{
  if (_pHAC == NULL) {
    _pHAC = new CHudAssetsContainer();
    InitAmmoWeaponIconsLinks();
  }
  
  InitDefaultTexture();
  
  CPrintF("Loading HUD assets...\n");

  // Try to load all stuff.
  if (!_bHUDFontsLoaded)
  {
    try {
      // initialize and load HUD numbers font
      DECLARE_CTFILENAME( fnFont, "Fonts\\Numbers3.fnt");
      _fdNumbersFont.Load_t( fnFont);
      //_fdNumbersFont.SetCharSpacing(0);
    } catch( char *strError) {
      CPrintF("  failed! %s\n", strError);
      _bHUDFontsLoaded = FALSE;
      return;
    }
  }

  _bHUDFontsLoaded = TRUE;

  HUD_RegisterTextures();
  HUD_LoadTextures();
  HUD_ForceTextures();

  CPrintF("  done.\n");
}

static void ReloadTextureData_t(CTextureObject *tobj)
{
  if (tobj == NULL) {
    return;
  }

  if (tobj->GetData() == NULL) {
    return;
  }

  CTextureData *ptd = (CTextureData*)tobj->GetData();
  ptd->Reload();
  ptd->td_tpLocal.Clear();
}

extern void HUD_ReloadSS(void* pArgs)
{
  InitHUD();
  
  CPrintF("Refreshing HUD assets...\n");
  
  FOREACHINLIST(CHUDTextureEntry, he_lnNode, _lhAllHUDTextures, ithte)
  {
    CHUDTextureEntry &hte = *ithte;
    
    try {
      ReloadTextureData_t(hte.he_ptoTexture);
    } catch( char *strError) {
      CPrintF("  Reload Failed: %s\n", strError);
    }
  }
}

// --------------------------------------------------------------------------------------
// Clean up.
// --------------------------------------------------------------------------------------
extern void EndHUD(void) {}
