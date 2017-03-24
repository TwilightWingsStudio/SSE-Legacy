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

#include <Engine/Graphics/DrawPort.h>

#include <EntitiesMP/Player.h>
#include <EntitiesMP/PlayerWeapons.h>
#include <EntitiesMP/MusicHolder.h>
#include <EntitiesMP/EnemyBase.h>
#include <EntitiesMP/EnemyCounter.h>

#define ENTITY_DEBUG

// armor & health constants
// NOTE: these _do not_ reflect easy/tourist maxvalue adjustments. that is by design!
#define TOP_ARMOR  100
#define TOP_HEALTH 100

static CListHead _lhAllHUDTextures;

class CHUDTextureEntry
{
  public:
    CListNode he_lnNode;
    CTFileName he_fnTexture;
    CTextureObject *he_ptoTexture;
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

// Cheats
extern INDEX cht_bEnable;
extern INDEX cht_bGod;
extern INDEX cht_bFly;
extern INDEX cht_bGhost;
extern INDEX cht_bInvisible;
extern FLOAT cht_fTranslationMultiplier;
extern INDEX cht_bKillAllAura;  // [SSE] Cheats Expansion
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
//

// player statistics sorting keys
enum SortKeys {
  PSK_NAME    = 1,
  PSK_HEALTH  = 2,
  PSK_SCORE   = 3,
  PSK_MANA    = 4,
  PSK_FRAGS   = 5,
  PSK_DEATHS  = 6,
};

// where is the bar lowest value
enum BarOrientations {
  BO_LEFT  = 1,
  BO_RIGHT = 2,
  BO_UP    = 3,
  BO_DOWN  = 4,
};

extern const INDEX aiWeaponsRemap[19];

// drawing variables
static const CPlayer *_penPlayer;
static CPlayerWeapons *_penWeapons;
static CDrawPort *_pDP;
static PIX   _pixDPWidth, _pixDPHeight;
static FLOAT _fResolutionScaling;
static FLOAT _fCustomScaling;
static ULONG _ulAlphaHUD;
static COLOR _colHUD;
static COLOR _colHUDText;
static TIME  _tmNow = -1.0f;
static TIME  _tmLast = -1.0f;
static CFontData _fdNumbersFont;

// array for pointers of all players
extern CPlayer *_apenPlayers[NET_MAXGAMEPLAYERS] = {0};

// [SSE] Default Texture
static CImageInfo _iiDefault; 
static CTextureData *_tdDefault = NULL;
//

// status bar textures
static CTextureObject _toHealth;
static CTextureObject _toOxygen;
static CTextureObject _toScore;
static CTextureObject _toHiScore;
static CTextureObject _toMessage;
static CTextureObject _toMana;
static CTextureObject _toFrags;
static CTextureObject _toDeaths;
static CTextureObject _toArmorSmall;
static CTextureObject _toArmorMedium;
static CTextureObject _toArmorLarge;

static CTextureObject _toExtraLive; // [SSE]

static CTextureObject _toTestIken; // [SSE]

// ammo textures
static CTextureObject _toAColt;
static CTextureObject _toAShells;
static CTextureObject _toABullets;
static CTextureObject _toARockets;
static CTextureObject _toAGrenades;
static CTextureObject _toANapalm;
static CTextureObject _toAElectricity;
static CTextureObject _toAIronBall;
static CTextureObject _toASniperBullets;
static CTextureObject _toASeriousBomb;

// weapon textures
static CTextureObject _toWFists; // [SSE] Fists Weapon
static CTextureObject _toWKnife;
static CTextureObject _toWColt;
static CTextureObject _toWSingleShotgun;
static CTextureObject _toWDoubleShotgun;
static CTextureObject _toWTommygun;
static CTextureObject _toWSniper;
static CTextureObject _toWChainsaw;
static CTextureObject _toWMinigun;
static CTextureObject _toWRocketLauncher;
static CTextureObject _toWGrenadeLauncher;
static CTextureObject _toWFlamer;
static CTextureObject _toWLaser;
static CTextureObject _toWIronCannon;

// powerup textures (ORDER IS THE SAME AS IN PLAYER.ES!)
#define MAX_POWERUPS 4
static CTextureObject _atoPowerups[MAX_POWERUPS];

// tile texture (one has corners, edges and center)
static CTextureObject _toTile;

// sniper mask texture
static CTextureObject _toSniperMask;
static CTextureObject _toSniperWheel;
static CTextureObject _toSniperArrow;
static CTextureObject _toSniperEye;
static CTextureObject _toSniperLed;

static BOOL _bHUDFontsLoaded = FALSE; // [SSE] HUD No Crash If No Assets

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

static struct ColorTransitionTable _cttHUD;

// ammo's info structure
struct AmmoInfo {
  CTextureObject    *ai_ptoAmmo;
  struct WeaponInfo *ai_pwiWeapon1;
  struct WeaponInfo *ai_pwiWeapon2;
  INDEX ai_iAmmoAmmount;
  INDEX ai_iMaxAmmoAmmount;
  INDEX ai_iLastAmmoAmmount;
  TIME  ai_tmAmmoChanged;
  BOOL  ai_bHasWeapon;
};

// weapons' info structure
struct WeaponInfo {
  enum WeaponType  wi_wtWeapon;
  CTextureObject  *wi_ptoWeapon;
  struct AmmoInfo *wi_paiAmmo;
  BOOL wi_bHasWeapon;
};

extern struct WeaponInfo _awiWeapons[18];
static struct AmmoInfo _aaiAmmo[8] = {
  { &_toAShells,        &_awiWeapons[5],  &_awiWeapons[6],  0, 0, 0, -9, FALSE }, //  0
  { &_toABullets,       &_awiWeapons[7],  &_awiWeapons[8],  0, 0, 0, -9, FALSE }, //  1
  { &_toARockets,       &_awiWeapons[9],  NULL,             0, 0, 0, -9, FALSE }, //  2
  { &_toAGrenades,      &_awiWeapons[10],  NULL,             0, 0, 0, -9, FALSE }, //  3
  { &_toANapalm,        &_awiWeapons[12], NULL,             0, 0, 0, -9, FALSE }, //  4
  { &_toAElectricity,   &_awiWeapons[13], NULL,             0, 0, 0, -9, FALSE }, //  5
  { &_toAIronBall,      &_awiWeapons[15], NULL,             0, 0, 0, -9, FALSE }, //  6
  { &_toASniperBullets, &_awiWeapons[14], NULL,             0, 0, 0, -9, FALSE }, //  7
};

static const INDEX aiAmmoRemap[8] = { 0, 1, 2, 3, 4, 7, 5, 6 };

struct WeaponInfo _awiWeapons[18] = {
  { WEAPON_NONE,            NULL,                 NULL,         FALSE },   //  0
  { WEAPON_FISTS,           &_toWFists,           NULL,         FALSE },   //  1
  { WEAPON_KNIFE,           &_toWKnife,           NULL,         FALSE },   //  2
  { WEAPON_COLT,            &_toWColt,            NULL,         FALSE },   //  3
  { WEAPON_DOUBLECOLT,      &_toWColt,            NULL,         FALSE },   //  4
  { WEAPON_SINGLESHOTGUN,   &_toWSingleShotgun,   &_aaiAmmo[0], FALSE },   //  5
  { WEAPON_DOUBLESHOTGUN,   &_toWDoubleShotgun,   &_aaiAmmo[0], FALSE },   //  6
  { WEAPON_TOMMYGUN,        &_toWTommygun,        &_aaiAmmo[1], FALSE },   //  7
  { WEAPON_MINIGUN,         &_toWMinigun,         &_aaiAmmo[1], FALSE },   //  8
  { WEAPON_ROCKETLAUNCHER,  &_toWRocketLauncher,  &_aaiAmmo[2], FALSE },   //  9
  { WEAPON_GRENADELAUNCHER, &_toWGrenadeLauncher, &_aaiAmmo[3], FALSE },   // 10
  { WEAPON_CHAINSAW,        &_toWChainsaw,        NULL,         FALSE },   // 11
  { WEAPON_FLAMER,          &_toWFlamer,          &_aaiAmmo[4], FALSE },   // 12
  { WEAPON_LASER,           &_toWLaser,           &_aaiAmmo[5], FALSE },   // 13
  { WEAPON_SNIPER,          &_toWSniper,          &_aaiAmmo[7], FALSE },   // 14
  { WEAPON_IRONCANNON,      &_toWIronCannon,      &_aaiAmmo[6], FALSE },   // 15
//{ WEAPON_PIPEBOMB,        &_toWPipeBomb,        &_aaiAmmo[3], FALSE },   // 15
//{ WEAPON_GHOSTBUSTER,     &_toWGhostBuster,     &_aaiAmmo[5], FALSE },   // 16
//{ WEAPON_NUKECANNON,      &_toWNukeCannon,      &_aaiAmmo[7], FALSE },   // 17
  { WEAPON_NONE,            NULL,                 NULL,         FALSE },   // 16
  { WEAPON_NONE,            NULL,                 NULL,         FALSE },   // 17
};

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

// --------------------------------------------------------------------------------------
// Prepare color transitions.
// --------------------------------------------------------------------------------------
static void PrepareColorTransitions( COLOR colFine, COLOR colHigh, COLOR colMedium, COLOR colLow,
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
static COLOR AddShaker( PIX const pixAmmount, INDEX const iCurrentValue, INDEX &iLastValue,
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
static COLOR GetCurrentColor( FLOAT fNormalizedValue)
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
  }
  else
  { // simple color picker
    col = _cttHUD.ctt_colMedium;
    if (fNormalizedValue > _cttHUD.ctt_fMediumHigh) col = _cttHUD.ctt_colHigh;
  }
  // all done
  return( col & 0xFFFFFF00);
}

// --------------------------------------------------------------------------------------
// Fill array with players' statistics (returns current number of players in game).
// --------------------------------------------------------------------------------------
extern INDEX SetAllPlayersStats( INDEX iSortKey)
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
  if (_toTile.GetData() == NULL) return;
  if (_toTile.GetData()->ad_Anims == NULL) return;
  
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
  _pDP->InitTexture(&_toTile, TRUE); // clamping on!
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
  if (col==NONE) col = GetCurrentColor( fNormValue);
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
    col = GetCurrentColor( fNormValue);
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
  if (col==NONE) col = GetCurrentColor( fNormValue);

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
static void DrawRotatedQuad( class CTextureObject *_pTO, FLOAT fX, FLOAT fY, FLOAT fSize, ANGLE aAngle, COLOR col)
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
static void DrawAspectCorrectTextureCentered( class CTextureObject *_pTO, FLOAT fX, FLOAT fY, FLOAT fWidth, COLOR col)
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
  if (_toSniperMask.GetData() == NULL) return;
  if (_toSniperMask.GetData()->ad_Anims == NULL) return;
  
  // determine location
  const FLOAT fSizeI = _pixDPWidth;
  const FLOAT fSizeJ = _pixDPHeight;
  const FLOAT fCenterI = fSizeI/2;
  const FLOAT fCenterJ = fSizeJ/2;
  const FLOAT fBlackStrip = (fSizeI-fSizeJ)/2;

  COLOR colMask = C_WHITE|CT_OPAQUE;

  CTextureData *ptd = (CTextureData*)_toSniperMask.GetData();
  const FLOAT fTexSizeI = ptd->GetPixWidth();
  const FLOAT fTexSizeJ = ptd->GetPixHeight();
  
  // [SSE] Sniper Scope
  hud_fSniperScopeBaseOpacity = Clamp(hud_fSniperScopeBaseOpacity, 0.0f, 1.0f);
  hud_fSniperScopeWheelOpacity = Clamp(hud_fSniperScopeWheelOpacity, 0.0f, 1.0f);
  hud_fSniperScopeLedOpacity = Clamp(hud_fSniperScopeLedOpacity, 0.0f, 1.0f);
  hud_fSniperScopeRangeTextOpacity = Clamp(hud_fSniperScopeRangeTextOpacity, 0.0f, 1.0f);
  hud_fSniperScopeRangeIconOpacity = Clamp(hud_fSniperScopeRangeIconOpacity, 0.0f, 1.0f);
  hud_fSniperScopeZoomTextOpacity = Clamp(hud_fSniperScopeZoomTextOpacity, 0.0f, 1.0f);
  hud_fSniperScopeZoomIconOpacity = Clamp(hud_fSniperScopeZoomIconOpacity, 0.0f, 1.0f);

  // main sniper mask
  if (hud_fSniperScopeBaseOpacity > 0.0F) {
    ULONG ulSniperScopeBaseOpacity = NormFloatToByte(hud_fSniperScopeBaseOpacity);

    colMask = C_WHITE|ulSniperScopeBaseOpacity;
  
    _pDP->InitTexture( &_toSniperMask);

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
    
    DrawRotatedQuad(&_toSniperWheel, fCenterI, fCenterJ, 40.0f*_fYResolutionScaling, aAngle, colWhell|ulSniperScopeWheelOpacity);
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
    
    DrawAspectCorrectTextureCentered(&_toSniperLed, fCenterI-37.0f*_fYResolutionScaling,
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

      DrawAspectCorrectTextureCentered(&_toSniperArrow, fCenterI-_fLeftX*_fYResolutionScaling,
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

      DrawAspectCorrectTextureCentered(&_toSniperEye,   fCenterI+_fRightX*_fYResolutionScaling,
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
static void FillWeaponAmmoTables(void)
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

#ifdef ENTITY_DEBUG
CRationalEntity *DBG_prenStackOutputEntity = NULL;
#endif
void HUD_SetEntityForStackDisplay(CRationalEntity *pren)
{
#ifdef ENTITY_DEBUG
  DBG_prenStackOutputEntity = pren;
#endif
  return;
}

#ifdef ENTITY_DEBUG
static void HUD_DrawEntityStack()
{
  CTString strTemp;
  PIX pixFontHeight;
  ULONG pixTextBottom;

  if (tmp_ai[9]==12345)
  {
    if (DBG_prenStackOutputEntity!=NULL)
    {
      pixFontHeight = _pfdConsoleFont->fd_pixCharHeight;
      pixTextBottom = _pixDPHeight*0.83;
      _pDP->SetFont( _pfdConsoleFont);
      _pDP->SetTextScaling( 1.0f);

      INDEX ctStates = DBG_prenStackOutputEntity->en_stslStateStack.Count();
      strTemp.PrintF("-- stack of '%s'(%s)@%gs\n", DBG_prenStackOutputEntity->GetName(),
        DBG_prenStackOutputEntity->en_pecClass->ec_pdecDLLClass->dec_strName,
        _pTimer->CurrentTick());
      _pDP->PutText( strTemp, 1, pixTextBottom-pixFontHeight*(ctStates+1), _colHUD|_ulAlphaHUD);

      for(INDEX iState=ctStates-1; iState>=0; iState--) {
        SLONG slState = DBG_prenStackOutputEntity->en_stslStateStack[iState];
        strTemp.PrintF("0x%08x %s\n", slState,
          DBG_prenStackOutputEntity->en_pecClass->ec_pdecDLLClass->HandlerNameForState(slState));
        _pDP->PutText( strTemp, 1, pixTextBottom-pixFontHeight*(iState+1), _colHUD|_ulAlphaHUD);
      }
    }
  }
}
#endif
//<<<<<<< DEBUG FUNCTIONS >>>>>>>

static void HUD_DrawAnchoredRectEx(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, COLOR colRect)
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

static void HUD_DrawAnchoredRect(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, COLOR colRect)
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

static void HUD_DrawAnchoredRectOutlineEx(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, COLOR colRect)
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

static void HUD_DrawAnchoredRectOutline(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, COLOR colRect)
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

static void HUD_DrawAnchroredIconEx(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, CTextureObject &toIcon, COLOR colDefault, FLOAT fNormValue, BOOL bBlink)
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
      fOriginY = _pixDPHeight / 2.0F + fOriginY;
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

static void HUD_DrawAnchroredIcon(FLOAT fPosX, FLOAT fPosY, FLOAT fSizeX, FLOAT fSizeY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, CTextureObject &toIcon, COLOR colDefault, FLOAT fNormValue, BOOL bBlink)
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

static void HUD_DrawAnchoredTextEx( FLOAT fPosX, FLOAT fPosY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, const CTString &strText, COLOR colDefault, FLOAT fNormValue)
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

static void HUD_DrawAnchoredText( FLOAT fPosX, FLOAT fPosY, EHUDHorAnchorType ehPos, EHUDVerAnchorType evPos, const CTString &strText, COLOR colDefault, FLOAT fNormValue)
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

// --------------------------------------------------------------------------------------
// main

// render interface (frontend) to drawport
// (units are in pixels for 640x480 resolution - for other res HUD will be scalled automatically)
// --------------------------------------------------------------------------------------
//#define INC_TEST_IKEN
//#define NEW_HUD
#ifdef NEW_HUD
extern void DrawHUD( const CPlayer *penPlayerCurrent, CDrawPort *pdpCurrent, BOOL bSnooping, const CPlayer *penPlayerOwner)
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
  _colHUD     = 0x4C80BB00;
  _colHUDText = SE_COL_ORANGE_LIGHT;
  _ulAlphaHUD = NormFloatToByte(hud_fOpacity);
  _tmNow = _pTimer->CurrentTick();

  // determine hud colorization;
  COLOR colMax = SE_COL_BLUEGREEN_LT;
  COLOR colTop = SE_COL_ORANGE_LIGHT;
  COLOR colMid = LerpColor(colTop, C_RED, 0.5f);
  
  INDEX iCurrentWeapon = _penWeapons->m_iCurrentWeapon;

  ULONG _ulBrAlpha = NormFloatToByte(hud_fOpacity * 0.5F);
  
  FLOAT fValue = 0.0F;

  //_pfdDisplayFont->SetVariableWidth();
  //_pDP->SetFont( &_fdNumbersFont);
  //_pDP->SetTextCharSpacing(1);
  //
  //CTString strGovno;
  //strGovno.PrintF("100");
  //
  //PIX pixCharWidth = _fdNumbersFont.GetWidth() + _fdNumbersFont.GetCharSpacing() + 1;
  //HUD_DrawAnchoredText(44, 8, EHHAT_LEFT, EHVAT_BOT, strGovno, C_GREEN|255, 1.0F);

  // Health
  fValue = fValue = ClampDn( _penPlayer->GetHealth(), 0.0f);
  
  HUD_DrawAnchoredRect(8, 8, 32, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchoredRect(44, 8, 80, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchroredIcon(8, 8, 32, 32, EHHAT_LEFT, EHVAT_BOT, _toHealth, C_WHITE|CT_OPAQUE, fValue / TOP_HEALTH, FALSE);

  HUD_DrawAnchoredRectOutline( 8, 8, 32, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
  HUD_DrawAnchoredRectOutline(44, 8, 80, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);

  // Armor
  fValue = _penPlayer->m_fArmor;
  
  CTextureObject *ptoCurrentArmor = NULL;
    
  if (fValue <= 50.5f) {
    ptoCurrentArmor = &_toArmorSmall;
  } else if (fValue<=100.5f) {
    ptoCurrentArmor = &_toArmorMedium;
  } else {
    ptoCurrentArmor = &_toArmorLarge;
  }
  
  HUD_DrawAnchoredRect( 8, 48, 32, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchoredRect(44, 48, 80, 32, EHHAT_LEFT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchroredIcon(8, 48, 32, 32, EHHAT_LEFT, EHVAT_BOT, *ptoCurrentArmor, C_WHITE|CT_OPAQUE, fValue / TOP_ARMOR, FALSE); // Icon

  HUD_DrawAnchoredRectOutline( 8, 48, 32, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
  HUD_DrawAnchoredRectOutline(44, 48, 80, 32, EHHAT_LEFT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
  
  // Current Weapon
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
      if (bAnyColt) {
        ptoCurrentAmmo = &_toAColt;
      }
      
      HUD_DrawAnchoredRect(  0 - 32, 8, 80, 32, EHHAT_CENTER, EHVAT_BOT, C_BLACK|_ulBrAlpha);
      HUD_DrawAnchoredRectOutline(  0 - 32, 8, 80, 32, EHHAT_CENTER, EHVAT_BOT, _colHUD|_ulAlphaHUD);
      
      if (ptoCurrentAmmo != NULL)
      {
        HUD_DrawAnchoredRect (60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, C_BLACK|_ulBrAlpha);
        HUD_DrawAnchroredIcon(60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, *ptoCurrentAmmo, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
        HUD_DrawAnchoredRectOutline(60 - 32, 8, 32, 32, EHHAT_CENTER, EHVAT_BOT, _colHUD|_ulAlphaHUD);
      }
    }
  }
  
  // LSC Up
  HUD_DrawAnchoredRect ( 8,  8,  16, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchoredRect (28,  8, 104, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchroredIcon( 8,  8,  16, 16, EHHAT_LEFT, EHVAT_TOP, _toFrags, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
  
  HUD_DrawAnchoredRectOutline( 8, 8,  16, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
  HUD_DrawAnchoredRectOutline(28, 8, 104, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
  
  // LSC Down
  HUD_DrawAnchoredRect ( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchoredRect (28, 28, 104, 16, EHHAT_LEFT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchroredIcon( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, _toExtraLive, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
  
  HUD_DrawAnchoredRectOutline( 8, 28,  16, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
  HUD_DrawAnchoredRectOutline(28, 28, 104, 16, EHHAT_LEFT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
  
  // CSC Up
  HUD_DrawAnchoredRect ( -54, 8,  16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchroredIcon( -54, 8,  16, 16, EHHAT_CENTER, EHVAT_TOP, _toHiScore, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
  HUD_DrawAnchoredRect (  10, 8, 104, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  
  HUD_DrawAnchoredRectOutline(-54, 8,  16, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
  HUD_DrawAnchoredRectOutline( 10, 8, 104, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
  
  // CSC Down
  //HUD_DrawAnchoredRect(-53, 28, 16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  //HUD_DrawAnchoredRect(  9, 28, 104, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  
  // BossBar
  HUD_DrawAnchoredRect( -130, 48, 16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchroredIcon(-130, 48, 16, 16, EHHAT_CENTER, EHVAT_TOP, _toHealth, C_WHITE|CT_OPAQUE, 1.0F, FALSE);
  HUD_DrawAnchoredRect(   10, 48, 256, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  
  HUD_DrawAnchoredRectOutline(-130, 48,  16, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
  HUD_DrawAnchoredRectOutline(  10, 48, 256, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
  
  // Oxygen
  HUD_DrawAnchoredRect (-34, 68, 16, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchoredRect ( 10, 68, 64, 16, EHHAT_CENTER, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchroredIcon(-34, 68, 16, 16, EHHAT_CENTER, EHVAT_TOP, _toOxygen, C_WHITE|CT_OPAQUE, 1.0F, FALSE); // Icon
  
  HUD_DrawAnchoredRectOutline(-34, 68, 16, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
  HUD_DrawAnchoredRectOutline( 10, 68, 64, 16, EHHAT_CENTER, EHVAT_TOP, _colHUD|_ulAlphaHUD);
  
  // Right - Messages
  HUD_DrawAnchoredRect ( 8, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchoredRect (28, 8, 52, 16, EHHAT_RIGHT, EHVAT_TOP, C_BLACK|_ulBrAlpha);
  HUD_DrawAnchroredIcon( 8, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, _toMessage, C_WHITE|CT_OPAQUE, 0.0F, TRUE); // Icon
  
  HUD_DrawAnchoredRectOutline( 8, 8, 16, 16, EHHAT_RIGHT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
  HUD_DrawAnchoredRectOutline(28, 8, 52, 16, EHHAT_RIGHT, EHVAT_TOP, _colHUD|_ulAlphaHUD);
  
  for (INDEX i = 0; i < 4; i++)
  {
    INDEX ii = 3 - i;

    HUD_DrawAnchoredRect (8 + (24 + 3) * i, 48, 24, 24, EHHAT_RIGHT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    HUD_DrawAnchroredIcon(8 + (24 + 3) * i, 48, 24, 24, EHHAT_RIGHT, EHVAT_BOT, _atoPowerups[ii], C_WHITE|CT_OPAQUE, 1.0F, TRUE); // Icon
    HUD_DrawAnchoredRectOutline(8 + (24 + 3) * i, 48, 24, 24, EHHAT_RIGHT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
  }
  
  for (INDEX i = 8; i >= 0; i--)
  {
    
    HUD_DrawAnchoredRect( 8 + (24 + 3) * i, 8, 24, 24, EHHAT_RIGHT, EHVAT_BOT, C_BLACK|_ulBrAlpha);
    
    CTextureObject *ptoAmmo = &_toASeriousBomb;
    
    INDEX ii = 8 - i;
    if (ii < 8) {
      ptoAmmo = _aaiAmmo[ii].ai_ptoAmmo;
    }

    HUD_DrawAnchroredIcon( 8 + (24 + 3) * i, 8, 24, 24, EHHAT_RIGHT, EHVAT_BOT, *ptoAmmo, C_WHITE|CT_OPAQUE, 1.0F, TRUE); // Icon
    HUD_DrawAnchoredRectOutline(8 + (24 + 3) * i, 8, 24, 24, EHHAT_RIGHT, EHVAT_BOT, _colHUD|_ulAlphaHUD);
  }

  /*
  HUD_DrawAnchoredRect(16, 16, 32, 32, EHHAT_LEFT, EHVAT_TOP, C_RED|255);
  HUD_DrawAnchoredRect(16, 16, 32, 32, EHHAT_LEFT, EHVAT_MID, C_RED|255);
  HUD_DrawAnchoredRect(16, 16, 32, 32, EHHAT_LEFT, EHVAT_BOT, C_RED|255);
  
  HUD_DrawAnchoredRect(16, 16, 32, 32, EHHAT_CENTER, EHVAT_TOP, C_RED|255);
  HUD_DrawAnchoredRect(16, 16, 32, 32, EHHAT_CENTER, EHVAT_MID, C_RED|255);
  HUD_DrawAnchoredRect(16, 16, 32, 32, EHHAT_CENTER, EHVAT_BOT, C_RED|255);
  
  HUD_DrawAnchoredRect(16, 16, 32, 32, EHHAT_RIGHT, EHVAT_TOP, C_RED|255);
  HUD_DrawAnchoredRect(16, 16, 32, 32, EHHAT_RIGHT, EHVAT_BOT, C_RED|255);
  HUD_DrawAnchoredRect(16, 16, 32, 32, EHHAT_RIGHT, EHVAT_MID, C_RED|255);
  */

  //HUD_DrawAnchroredIcon(16, 16, 32, 32, EHHAT_LEFT,   EHVAT_TOP, _toTestIken, C_WHITE|64, 1.0F, FALSE);
  //HUD_DrawAnchroredIcon(16, 16, 32, 32, EHHAT_LEFT,   EHVAT_MID, _toTestIken, C_WHITE|64, 1.0F, FALSE);
  //HUD_DrawAnchroredIcon(16, 16, 32, 32, EHHAT_LEFT,   EHVAT_BOT, _toTestIken, C_WHITE|64, 1.0F, FALSE);
  //HUD_DrawAnchroredIcon( 0, 16, 32, 32, EHHAT_CENTER, EHVAT_TOP, _toTestIken, C_WHITE|64, 1.0F, FALSE);
  //HUD_DrawAnchroredIcon( 0,  0, 32, 32, EHHAT_CENTER, EHVAT_MID, _toTestIken, C_WHITE|64, 1.0F, FALSE);
  //HUD_DrawAnchroredIcon( 0, 16, 32, 32, EHHAT_CENTER, EHVAT_BOT, _toTestIken, C_WHITE|64, 1.0F, FALSE);
  //HUD_DrawAnchroredIcon(16, 16, 32, 32, EHHAT_RIGHT,  EHVAT_TOP, _toTestIken, C_WHITE|64, 1.0F, FALSE);
  //HUD_DrawAnchroredIcon(16, 16, 32, 32, EHHAT_RIGHT,  EHVAT_BOT, _toTestIken, C_WHITE|64, 1.0F, FALSE);
  //HUD_DrawAnchroredIcon(16, 16, 32, 32, EHHAT_RIGHT,  EHVAT_MID, _toTestIken, C_WHITE|64, 1.0F, FALSE);

  //CTString strGovno;
  //strGovno.PrintF("[]");

  //_pfdDisplayFont->SetVariableWidth();
  //_pDP->SetFont( _pfdDisplayFont);  

  //HUD_DrawAnchoredText(16, 16, EHHAT_LEFT,   EHVAT_TOP, strGovno, C_GREEN|255, 1.0F);
  //HUD_DrawAnchoredText(16, 16, EHHAT_LEFT,   EHVAT_MID, strGovno, C_GREEN|255, 1.0F);
  //HUD_DrawAnchoredText(16, 16, EHHAT_LEFT,   EHVAT_BOT, strGovno, C_GREEN|255, 1.0F);
  //HUD_DrawAnchoredText( 0, 16,   EHHAT_CENTER, EHVAT_TOP, strGovno, C_GREEN|255, 1.0F);
  //HUD_DrawAnchoredText( 0, 0,   EHHAT_CENTER, EHVAT_MID, strGovno, C_GREEN|255, 1.0F);
  //HUD_DrawAnchoredText( 0, 16,   EHHAT_CENTER, EHVAT_BOT, strGovno, C_GREEN|255, 1.0F);
  //HUD_DrawAnchoredText(16, 16, EHHAT_RIGHT,  EHVAT_TOP, strGovno, C_GREEN|255, 1.0F);
  //HUD_DrawAnchoredText(16, 16, EHHAT_RIGHT,  EHVAT_BOT, strGovno, C_GREEN|255, 1.0F);
  //HUD_DrawAnchoredText(16, 16, EHHAT_RIGHT,  EHVAT_MID, strGovno, C_GREEN|255, 1.0F);
}

#else
void DrawHUD(const CPlayer *penPlayerCurrent, CDrawPort *pdpCurrent, BOOL bSnooping, const CPlayer *penPlayerOwner)
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
  _colHUD     = 0x4C80BB00;
  _colHUDText = SE_COL_ORANGE_LIGHT;
  _ulAlphaHUD = NormFloatToByte(hud_fOpacity);
  _tmNow = _pTimer->CurrentTick();

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
  if (((CPlayerWeapons*)&*penPlayerOwner->m_penWeapons)->m_iCurrentWeapon == WEAPON_SNIPER
    && ((CPlayerWeapons*)&*penPlayerOwner->m_penWeapons)->m_bSniping && hud_bSniperScopeDraw) {
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
  fNormValue = fValue/TOP_HEALTH;
  strValue.PrintF( "%d", (SLONG)ceil(fValue));
  PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
  fRow = pixBottomBound-fHalfUnit;
  fCol = pixLeftBound+fHalfUnit;
  colDefault = AddShaker( 5, fValue, penLast->m_iLastHealth, penLast->m_tmHealthChanged, fMoverX, fMoverY);
  HUD_DrawBorder( fCol+fMoverX, fRow+fMoverY, fOneUnit, fOneUnit, colBorder);
  fCol += fAdvUnit+fChrUnit*3/2 -fHalfUnit;
  HUD_DrawBorder( fCol, fRow, fChrUnit*3, fOneUnit, colBorder);
  HUD_DrawText( fCol, fRow, strValue, colDefault, fNormValue);
  fCol -= fAdvUnit+fChrUnit*3/2 -fHalfUnit;
  HUD_DrawIcon( fCol+fMoverX, fRow+fMoverY, _toHealth, C_WHITE /*_colHUD*/, fNormValue, TRUE, 32, 32);

  // prepare and draw armor info (eventually)
  fValue = _penPlayer->m_fArmor;
  if (fValue > 0.0f) {
    fNormValue = fValue/TOP_ARMOR;
    strValue.PrintF( "%d", (SLONG)ceil(fValue));
    PrepareColorTransitions( colMax, colTop, colMid, C_lGRAY, 0.5f, 0.25f, FALSE);
    fRow = pixBottomBound- (fNextUnit+fHalfUnit);//*_pDP->dp_fWideAdjustment;
    fCol = pixLeftBound+    fHalfUnit;
    colDefault = AddShaker( 3, fValue, penLast->m_iLastArmor, penLast->m_tmArmorChanged, fMoverX, fMoverY);
    HUD_DrawBorder( fCol+fMoverX, fRow+fMoverY, fOneUnit, fOneUnit, colBorder);
    fCol += fAdvUnit+fChrUnit*3/2 -fHalfUnit;
    HUD_DrawBorder( fCol, fRow, fChrUnit*3, fOneUnit, colBorder);
    HUD_DrawText( fCol, fRow, strValue, NONE, fNormValue);
    fCol -= fAdvUnit+fChrUnit*3/2 -fHalfUnit;
    if (fValue<=50.5f) {
      HUD_DrawIcon( fCol+fMoverX, fRow+fMoverY, _toArmorSmall, C_WHITE /*_colHUD*/, fNormValue, FALSE, 32, 32);
    } else if (fValue<=100.5f) {
      HUD_DrawIcon( fCol+fMoverX, fRow+fMoverY, _toArmorMedium, C_WHITE /*_colHUD*/, fNormValue, FALSE, 32, 32);
    } else {
      HUD_DrawIcon( fCol+fMoverX, fRow+fMoverY, _toArmorLarge, C_WHITE /*_colHUD*/, fNormValue, FALSE, 32, 32);
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
    PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.30f, 0.15f, FALSE);
    BOOL bDrawAmmoIcon = _fCustomScaling<=1.0f;

    // [SSE] Colt Bullets
    if (bAnyColt) {
      ptoCurrentAmmo = &_toAColt;
    }

    // draw ammo, value and weapon
    fRow = pixBottomBound-fHalfUnit;
    fCol = 175 + fHalfUnit;
    colDefault = AddShaker( 4, fValue, penLast->m_iLastAmmo, penLast->m_tmAmmoChanged, fMoverX, fMoverY);
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
  PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
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
  FillWeaponAmmoTables();

  FLOAT fBombCount = penPlayerCurrent->m_iSeriousBombCount;
  BOOL  bBombFiring = FALSE;
  // draw serious bomb
#define BOMB_FIRE_TIME 1.5f
  if (penPlayerCurrent->m_tmSeriousBombFired+BOMB_FIRE_TIME>_pTimer->GetLerpedCurrentTick()) {
    fBombCount++;
    if (fBombCount>3) { fBombCount = 3; }
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
    HUD_DrawIcon(   fCol,         fRow, _toASeriousBomb, colBombIcon, fNormValue, FALSE, 32, 32);
    HUD_DrawBar(    fCol+fBarPos, fRow, fOneUnitS/5, fOneUnitS-2, BO_DOWN, colBombBar, fNormValue);
    // make space for serious bomb
    fCol -= fAdvUnitS;
  }

  // loop thru all ammo types
  if (!GetSP()->sp_bInfiniteAmmo)
  {
    for (INDEX ii = 7; ii>=0; ii--)
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
      colBar = AddShaker( 4, ai.ai_iAmmoAmmount, ai.ai_iLastAmmoAmmount, ai.ai_tmAmmoChanged, fMoverX, fMoverY);
      HUD_DrawBorder( fCol,         fRow+fMoverY, fOneUnitS, fOneUnitS, colBorder);
      HUD_DrawIcon(   fCol,         fRow+fMoverY, *_aaiAmmo[i].ai_ptoAmmo, colIcon, fNormValue, FALSE, 32, 32);
      HUD_DrawBar(    fCol+fBarPos, fRow+fMoverY, fOneUnitS/5, fOneUnitS-2, BO_DOWN, colBar, fNormValue);
      // advance to next position
      fCol -= fAdvUnitS;
    }
  }

  // draw powerup(s) if needed
  PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.66f, 0.33f, FALSE);
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
    HUD_DrawIcon(   fCol,         fRow, _atoPowerups[i], C_WHITE /*_colHUD*/, fNormValue, TRUE, 32, 32);
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
      PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
      fNormValue = fValue / tmMaxHoldBreath;
      fNormValue = ClampDn(fNormValue, 0.0f);

      HUD_DrawBorder( fCol,      fRow, fOneUnit,         fOneUnit, colBorder);
      HUD_DrawBorder( fCol+fAdv, fRow, fOneUnit*4,       fOneUnit, colBorder);
      HUD_DrawBar(    fCol+fAdv, fRow, fOneUnit*4*0.975, fOneUnit*0.9375, BO_LEFT, NONE, fNormValue);
      HUD_DrawIcon(   fCol,      fRow, _toOxygen, C_WHITE /*_colHUD*/, fNormValue, TRUE, 32, 32);
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
    if (mh.m_penCounter!=NULL) {
      CEnemyCounter &ec = (CEnemyCounter&)*mh.m_penCounter;
      if (ec.m_iCount>0) {
        fValue = ec.m_iCount;
        fNormValue = fValue/ec.m_iCountFrom;
      }
    }
    if (fNormValue>0) {
      // prepare and draw boss energy info
      //PrepareColorTransitions( colMax, colTop, colMid, C_RED, 0.5f, 0.25f, FALSE);
      PrepareColorTransitions( colMax, colMax, colTop, C_RED, 0.5f, 0.25f, FALSE);

      fRow = pixTopBound + fOneUnit + fNextUnit;
      fCol = 184.0f;
      fAdv = fAdvUnit+ fOneUnit*16/2 -fHalfUnit;
      if (bOxygenOnScreen) fRow += fNextUnit;
      HUD_DrawBorder( fCol,      fRow, fOneUnit,          fOneUnit, colBorder);
      HUD_DrawBorder( fCol+fAdv, fRow, fOneUnit*16,       fOneUnit, colBorder);
      HUD_DrawBar(    fCol+fAdv, fRow, fOneUnit*16*0.995, fOneUnit*0.9375, BO_LEFT, NONE, fNormValue);
      HUD_DrawIcon(   fCol,      fRow, _toHealth, C_WHITE /*_colHUD*/, fNormValue, FALSE, 32, 32);
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
    INDEX iPlayers = SetAllPlayersStats(eKey);

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
          _pDP->PutTextR( strName+":", _pixDPWidth-12 * fCharWidth, fCharHeight * i + fOneUnit * 2, _colHUD  |_ulAlphaHUD);
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
    if ((bScoreMatch || bFragMatch) && hud_bShowMatchInfo) {
      CTString strLimitsInfo="";
      if (GetSP()->sp_iTimeLimit>0) {
        FLOAT fTimeLeft = ClampDn(GetSP()->sp_iTimeLimit*60.0f - _pNetwork->GetGameTime(), 0.0f);
        strLimitsInfo.PrintF("%s^cFFFFFF%s: %s\n", strLimitsInfo, TRANS("TIME LEFT"), TimeToString(fTimeLeft));
      }
      extern INDEX SetAllPlayersStats( INDEX iSortKey);
      // fill players table
      const INDEX ctPlayers = SetAllPlayersStats(bFragMatch?5:3); // sort by frags or by score
      // find maximum frags/score that one player has
      INDEX iMaxFrags = LowerLimit(INDEX(0));
      INDEX iMaxScore = LowerLimit(INDEX(0));
      {for(INDEX iPlayer=0; iPlayer<ctPlayers; iPlayer++) {
        CPlayer *penPlayer = _apenPlayers[iPlayer];
        iMaxFrags = Max(iMaxFrags, penPlayer->m_psLevelStats.ps_iKills);
        iMaxScore = Max(iMaxScore, penPlayer->m_psLevelStats.ps_iScore);
      }}
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
  HUD_DrawIcon(   fCol,      fRow, _toFrags, C_WHITE /*colScore*/, 1.0f, FALSE, 32, 32);
  
  // [SSE] Extra Lives System
  if ((bSinglePlay || bCooperative) && GetSP()->sp_ctCredits >= 0)
  {
    BOOL bSharedLives = GetSP()->sp_bSharedLives; 
    strValue.PrintF( "%d", bSharedLives ? GetSP()->sp_ctCreditsLeft : _penPlayer->m_iLives);
    fRow = pixTopBound  + fNextUnit+fHalfUnit;
    fCol = pixLeftBound + fHalfUnit;
    fAdv = fAdvUnit+ fChrUnit*fWidthAdj/2 -fHalfUnit;

    HUD_DrawBorder( fCol,      fRow, fOneUnit,           fOneUnit, colBorder);
    HUD_DrawBorder( fCol+fAdv, fRow, fChrUnit*fWidthAdj, fOneUnit, colBorder);
    HUD_DrawText(   fCol+fAdv, fRow, strValue,  C_lGRAY, 1.0f);
    HUD_DrawIcon(   fCol,      fRow, _toExtraLive, C_WHITE /*colMana*/, 1.0f, FALSE, 32, 32);
    
    if (GetSP()->sp_iScoreForExtraLive > 0)
    {
      CTString strLimitsInfo;
      
      INDEX iScoreLeft = ClampDn(GetSP()->sp_iScoreForExtraLive - (bSharedLives ? GetSP()->sp_iScoreForExtraLiveAccum : _penPlayer->m_iScoreAccumulated), INDEX(0));
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
    HUD_DrawIcon(   fCol,      fRow, _toDeaths, C_WHITE /*colMana*/, 1.0f, FALSE, 32, 32);
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
    HUD_DrawIcon(   fCol,      fRow, _toHiScore, C_WHITE /*_colHUD*/, 1.0f, FALSE, 32, 32);

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
      HUD_DrawIcon(   fCol,      fRow, _toMessage, C_WHITE /*col*/, 0.0f, TRUE, 32 ,32);
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
    if (cht_bKillAllAura)  { _pDP->PutTextR( "killaura",  _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
    if (cht_bInvisible)    { _pDP->PutTextR( "invisible", _pixDPWidth-1, _pixDPHeight-pixFontHeight*iLine, colCheat|ulAlpha); iLine++; }
  }

  // in the end, remember the current time so it can be used in the next frame
  _tmLast = _tmNow;

  if (hud_bGameDebugMonitor)
  {
    FLOAT fExtraEnemyStrength = GetSP()->sp_fExtraEnemyStrength;
    FLOAT fExtraStrengthPerPlayer = GetSP()->sp_fExtraEnemyStrengthPerPlayer;
    INDEX ctCredits = GetSP()->sp_ctCredits;
    BOOL bSharedLives = GetSP()->sp_bSharedLives;

    INDEX ctMaxPlayers = CEntity::GetMaxPlayers();
    INDEX ctAlive = 0;
    INDEX ctDead = 0;
    INDEX ctDeadCanRespawn = 0;
    
    for (INDEX iPlayer = 0; iPlayer < ctMaxPlayers; iPlayer++)
    {
      CEntity *penEntity = CEntity::GetPlayerEntity(iPlayer);
      
      // If player is invalid then skip him.
      if (penEntity == NULL) {
        continue;
      }
      
      if (penEntity->IsAlive()) {
        ctAlive++;
      } else {
        ctDead++;
        
        if (!bSharedLives) {
          CPlayer *penPlayer = static_cast<CPlayer*>(penEntity);
          if (penPlayer->m_iLives > 0) {
            ctDeadCanRespawn++;
          }
        }
      }
    }
    
    if (bSharedLives)
    {
      INDEX ctCreditsLeft = GetSP()->sp_ctCreditsLeft;

      // If all dead can be respawned then count them all.
      if (ctDead <= ctCreditsLeft) {
        ctDeadCanRespawn = ctDead;
      } else { // If not enough lives for all.
        ctDeadCanRespawn = ctCreditsLeft;
      }
    }
    
    CTString strReport;
 
    strReport += "^bDEBUG MONITOR^r\n";
    strReport += "\n";
    strReport += "[Common]\n";
    strReport.PrintF("%s^cCCCCCCPlayers: %02d / %02d\n", strReport, ctAlive + ctDead, ctMaxPlayers);
    strReport += "\n";
    strReport.PrintF("%s^c00FF00Alive: %02d\n", strReport, ctAlive);
    strReport.PrintF("%s^cFF0000Dead:  %02d\n", strReport, ctDead);

    if (ctCredits >= 0) {
      strReport.PrintF("%s^cCCCCCCCan Respawn: %02d\n", strReport, ctDeadCanRespawn);
    }

    strReport += "\n\n";
    strReport += "^r[^cFFFFFFDDA System^r]^cCCCC00\n";
    strReport.PrintF("%sEES:   %.2f\n", strReport, fExtraEnemyStrength);
    strReport.PrintF("%sEESPP: %.2f\n", strReport, fExtraStrengthPerPlayer);
    strReport.PrintF("%sDamage Mul: %.2f\n", strReport, GetGameDamageMultiplier());
    
    _pDP->SetFont( _pfdConsoleFont);
    _pDP->SetTextAspect( 1.0f);
    _pDP->SetTextScaling(1.0f);

    _pDP->Fill(_pixDPWidth - 128 ,0, _pixDPWidth, _pixDPHeight, C_BLACK|0, C_BLACK|128, C_BLACK|0, C_BLACK|128);
    _pDP->PutText( strReport, _pixDPWidth - 128 + 4, 65, C_WHITE|CT_OPAQUE);
  }
}
#endif


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
  HUD_RegisterTexture(&_toTile, CTFILENAME("Textures\\Interface\\Tile.tex"));
  
  HUD_RegisterTexture(&_toHealth,  CTFILENAME("TexturesMP\\Interface\\HSuper.tex"));
  HUD_RegisterTexture(&_toOxygen,  CTFILENAME("TexturesMP\\Interface\\Oxygen-2.tex"));
  HUD_RegisterTexture(&_toFrags,   CTFILENAME("TexturesMP\\Interface\\IBead.tex"));
  HUD_RegisterTexture(&_toDeaths,  CTFILENAME("TexturesMP\\Interface\\ISkull.tex"));
  HUD_RegisterTexture(&_toScore,   CTFILENAME("TexturesMP\\Interface\\IScore.tex"));
  HUD_RegisterTexture(&_toHiScore, CTFILENAME("TexturesMP\\Interface\\IHiScore.tex"));
  HUD_RegisterTexture(&_toMessage, CTFILENAME("TexturesMP\\Interface\\IMessage.tex"));
  HUD_RegisterTexture(&_toMana,    CTFILENAME("TexturesMP\\Interface\\IValue.tex"));

  HUD_RegisterTexture(&_toArmorSmall,  CTFILENAME("TexturesMP\\Interface\\ArSmall.tex"));
  HUD_RegisterTexture(&_toArmorMedium, CTFILENAME("TexturesMP\\Interface\\ArMedium.tex"));
  HUD_RegisterTexture(&_toArmorLarge,  CTFILENAME("TexturesMP\\Interface\\ArStrong.tex"));
  
  HUD_RegisterTexture(&_toExtraLive,  CTFILENAME("TexturesMP\\Interface\\IExtraLive.tex"));

  // initialize ammo textures
  HUD_RegisterTexture(&_toAColt,          CTFILENAME("TexturesMP\\Interface\\AmColt.tex"));
  HUD_RegisterTexture(&_toAShells,        CTFILENAME("TexturesMP\\Interface\\AmShells.tex"));
  HUD_RegisterTexture(&_toABullets,       CTFILENAME("TexturesMP\\Interface\\AmBullets.tex"));
  HUD_RegisterTexture(&_toARockets,       CTFILENAME("TexturesMP\\Interface\\AmRockets.tex"));
  HUD_RegisterTexture(&_toAGrenades,      CTFILENAME("TexturesMP\\Interface\\AmGrenades.tex"));
  HUD_RegisterTexture(&_toANapalm,        CTFILENAME("TexturesMP\\Interface\\AmFuelReservoir.tex"));
  HUD_RegisterTexture(&_toAElectricity,   CTFILENAME("TexturesMP\\Interface\\AmElectricity.tex"));
  HUD_RegisterTexture(&_toAIronBall,      CTFILENAME("TexturesMP\\Interface\\AmCannonBall.tex"));
  HUD_RegisterTexture(&_toASniperBullets, CTFILENAME("TexturesMP\\Interface\\AmSniperBullets.tex"));
  HUD_RegisterTexture(&_toASeriousBomb,   CTFILENAME("TexturesMP\\Interface\\AmSeriousBomb.tex"));

  // initialize weapon textures
  HUD_RegisterTexture(&_toWFists,           CTFILENAME("TexturesMP\\Interface\\WFists.tex")); // [SSE] Fists Weapon
  HUD_RegisterTexture(&_toWKnife,           CTFILENAME("TexturesMP\\Interface\\WKnife.tex"));
  HUD_RegisterTexture(&_toWColt,            CTFILENAME("TexturesMP\\Interface\\WColt.tex"));
  HUD_RegisterTexture(&_toWSingleShotgun,   CTFILENAME("TexturesMP\\Interface\\WSingleShotgun.tex"));
  HUD_RegisterTexture(&_toWDoubleShotgun,   CTFILENAME("TexturesMP\\Interface\\WDoubleShotgun.tex"));
  HUD_RegisterTexture(&_toWTommygun,        CTFILENAME("TexturesMP\\Interface\\WTommygun.tex"));
  HUD_RegisterTexture(&_toWMinigun,         CTFILENAME("TexturesMP\\Interface\\WMinigun.tex"));
  HUD_RegisterTexture(&_toWRocketLauncher,  CTFILENAME("TexturesMP\\Interface\\WRocketLauncher.tex"));
  HUD_RegisterTexture(&_toWGrenadeLauncher, CTFILENAME("TexturesMP\\Interface\\WGrenadeLauncher.tex"));
  HUD_RegisterTexture(&_toWLaser,           CTFILENAME("TexturesMP\\Interface\\WLaser.tex"));
  HUD_RegisterTexture(&_toWIronCannon,      CTFILENAME("TexturesMP\\Interface\\WCannon.tex"));
  HUD_RegisterTexture(&_toWChainsaw,        CTFILENAME("TexturesMP\\Interface\\WChainsaw.tex"));
  HUD_RegisterTexture(&_toWSniper,          CTFILENAME("TexturesMP\\Interface\\WSniper.tex"));
  HUD_RegisterTexture(&_toWFlamer,          CTFILENAME("TexturesMP\\Interface\\WFlamer.tex"));

  // initialize powerup textures (DO NOT CHANGE ORDER!)
  HUD_RegisterTexture(&_atoPowerups[0],    CTFILENAME("TexturesMP\\Interface\\PInvisibility.tex"));
  HUD_RegisterTexture(&_atoPowerups[1],    CTFILENAME("TexturesMP\\Interface\\PInvulnerability.tex"));
  HUD_RegisterTexture(&_atoPowerups[2],    CTFILENAME("TexturesMP\\Interface\\PSeriousDamage.tex"));
  HUD_RegisterTexture(&_atoPowerups[3],    CTFILENAME("TexturesMP\\Interface\\PSeriousSpeed.tex"));

  // initialize sniper mask texture
  HUD_RegisterTexture(&_toSniperMask,   CTFILENAME("TexturesMP\\Interface\\SniperMask.tex"));
  HUD_RegisterTexture(&_toSniperWheel,  CTFILENAME("TexturesMP\\Interface\\SniperWheel.tex"));
  HUD_RegisterTexture(&_toSniperArrow,  CTFILENAME("TexturesMP\\Interface\\SniperArrow.tex"));
  HUD_RegisterTexture(&_toSniperEye,    CTFILENAME("TexturesMP\\Interface\\SniperEye.tex"));
  HUD_RegisterTexture(&_toSniperLed,    CTFILENAME("TexturesMP\\Interface\\SniperLed.tex"));
  
  _bTexturesRegistered = TRUE;
}

// --------------------------------------------------------------------------------------
// Initialize all that's needed for drawing the HUD.
// --------------------------------------------------------------------------------------
extern void InitHUD(void)
{
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
