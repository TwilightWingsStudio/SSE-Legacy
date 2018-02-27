/* Copyright (c) 2017 by ZCaliptium

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

#ifndef SE_INCL_HUDASSETSHOLDER_H
#define SE_INCL_HUDASSETSHOLDER_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

class CHudAssetsContainer
{
  public:
    // Tile texture (one has corners, edges and center)
    CTextureObject m_toTile;

    // Vitality
    CTextureObject m_toHealth;

    CTextureObject m_toArmorSmall;
    CTextureObject m_toArmorMedium;
    CTextureObject m_toArmorLarge;
    
    CTextureObject m_toShields; // [SSE]
    CTextureObject m_toShieldsTop; // [SSE]

    // Status bar textures
    CTextureObject m_toOxygen;
    CTextureObject m_toScore;
    CTextureObject m_toHiScore;
    CTextureObject m_toMessage;
    CTextureObject m_toMana;
    CTextureObject m_toFrags;
    CTextureObject m_toDeaths;

    CTextureObject m_toExtraLive; // [SSE]
    CTextureObject m_toSwords; // [SSE]
    CTextureObject m_toKey; // [SSE] Better Keys
    CTextureObject m_toClock; // [SSE]

    CTextureObject m_toTestIken; // [SSE]

    // [SSE] Radar
    CTextureObject m_toRadarCircle;
    CTextureObject m_toRadarMask;
    CTextureObject m_toRadarBorder;
    CTextureObject m_toRadarDot;
    CTextureObject m_toRadarDotUp;
    CTextureObject m_toRadarDotDown;
    //
    
    // Ammo textures
    CTextureObject m_toAColt;
    CTextureObject m_atoAmmo[9];
    
    // Weapon textures
    CTextureObject m_toWFists; // [SSE] Gameplay - Fists Weapon
    CTextureObject m_toWKnife;
    CTextureObject m_toWColt;
    CTextureObject m_toWSingleShotgun;
    CTextureObject m_toWDoubleShotgun;
    CTextureObject m_toWTommygun;
    CTextureObject m_toWSniper;
    CTextureObject m_toWChainsaw;
    CTextureObject m_toWMinigun;
    CTextureObject m_toWRocketLauncher;
    CTextureObject m_toWGrenadeLauncher;
    CTextureObject m_toWFlamer;
    CTextureObject m_toWLaser;
    CTextureObject m_toWIronCannon;
    CTextureObject m_toWSeriousBomb; // [SSE] Gameplay - Serious Bomb Weapon
    
    // Sniper mask texture
    CTextureObject m_toSniperMask;
    CTextureObject m_toSniperWheel;
    CTextureObject m_toSniperArrow;
    CTextureObject m_toSniperEye;
    CTextureObject m_toSniperLed;
    
    // Powerup textures (ORDER IS THE SAME AS IN PLAYER.ES!)
    #define MAX_POWERUPS 5
    CTextureObject m_atoPowerups[MAX_POWERUPS];
};

extern CHudAssetsContainer *_pHAC;

#endif