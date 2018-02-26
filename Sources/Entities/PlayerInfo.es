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

229
%{
  #include "StdH.h"
  #include "Entities/Player.h"
  #include "Entities/PlayerWeapons.h"
%}

class CPlayerInfo: CRationalEntity {
name      "Player Info";
thumbnail "Thumbnails\\PlayerInfo.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'      = "Player Info",
   3 CTString m_strDescription = "",
   
   5 BOOL m_bDebugMessages "Debug Messages" = FALSE,
   
   6 INDEX m_iIOPlayerIndex   "IO Player ID" = -1,
   7 INDEX m_iCurrentPlayerIndex        = -1,

  10 CEntityPointer m_penIOTarget  "IO Player Target", // used both for input and output, m_penCurrentTarget used to compare for change
  11 CEntityPointer m_penCurrentTarget,
  
  //14 FLOAT m_fUpdateInterval   "Update Interval" 'W' = 0.1f,
  15 BOOL m_bPerTickCheck      "Per Tick Check" = FALSE,
  16 BOOL m_bStaticPlayerIndex "Disable Player ID Output" = FALSE,
  
  // info comes now
  19 BOOL m_bPlayerExists      "Is Player Exists" = FALSE,
  20 FLOAT m_fHealth           "Player Health" = -1.0f,
  21 FLOAT m_fArmor            "Player Armor"  = -1.0f,
  25 BOOL m_bDead              "Is Dead"       = FALSE,
  35 CTString m_strPlayerName  "Player Name" = "",
  
  50 INDEX m_iAvailableWeapons "W Available Weapons" = 0,
  51 INDEX m_iCurrentWeapon    "W Current Weapon" = 0,
  
components:
  1 model   MODEL_PLAYERINFO   "Models\\Editor\\PlayerInfo.mdl",
  2 texture TEXTURE_PLAYERINFO "Models\\Editor\\PlayerInfo.tex",

functions:
  const CTString &GetDescription(void) const {
    ((CTString&)m_strDescription).PrintF("IO Player ID = %d", m_iIOPlayerIndex);
    return m_strDescription;
  }

  void ResetInfo()
  {
    // Reset both indexes.
    if (!m_bStaticPlayerIndex) {
      m_iIOPlayerIndex = -1;
    }

    m_iCurrentPlayerIndex = -1;

    // Reset target.
    m_penIOTarget = NULL;

    // Reset stats
    m_fHealth = -1.0f;
    m_bDead = FALSE;
    m_fArmor = -1.0f;

    m_strPlayerName = "";

    // Reset weapons.
    m_iAvailableWeapons = 0;
    m_iCurrentWeapon = 0;
  }
  
  void ApplyInfo()
  {
    CPlayer &penPlayer = (CPlayer&)*m_penCurrentTarget;

    m_fHealth = penPlayer.GetHealth();
    m_fArmor = penPlayer.GetArmor();
    m_strPlayerName = penPlayer.GetName();
    m_bDead = !(penPlayer.GetFlags()&ENF_ALIVE);

    if (m_bDebugMessages) {
      CPrintF("[%s] : Applied:\n", GetName());
      CPrintF("  Player Index: %d\n", m_iCurrentPlayerIndex);
      CPrintF("  Name: %s\n", m_strPlayerName);
      CPrintF("  Health: %f\n", m_fHealth);
      CPrintF("  Armor: %f\n", m_fArmor);
      CPrintF("  Is Dead: %d\n", m_bDead);
    }

    CPlayerWeapons *penWeapons = penPlayer.GetPlayerWeapons();

    // For safety.
    if (penWeapons == NULL) {
      if (m_bDebugMessages) {
        CPrintF("  Weapons are unavailable!\n", m_fArmor);
      }
      return;
    }

    m_iCurrentWeapon = penWeapons->m_iCurrentWeapon;
    m_iAvailableWeapons = penWeapons->m_iAvailableWeapons;

    if (m_bDebugMessages) {
      CPrintF("  Current Weapon ID: %d\n", m_iCurrentWeapon);
      CPrintF("  Available Weapons: %d\n", m_iAvailableWeapons);
    }
  }

  void UpdateInfo()
  {
    // If player index changed since last time.
    if (m_bStaticPlayerIndex || m_iCurrentPlayerIndex != m_iIOPlayerIndex) {
      // If valid input player index.
      if ((m_iIOPlayerIndex > -1) && (m_iIOPlayerIndex < 16))
      {
        // Update index.
        m_iCurrentPlayerIndex = m_iIOPlayerIndex;

        CEntity *pen = GetPlayerEntity(m_iIOPlayerIndex);

        // Update targets.
        m_penIOTarget = pen;
        m_penCurrentTarget = pen;
      // If invalid input player index then remove pointer.
      } else {
        m_penCurrentTarget = NULL;
      }
    // If player pointer changed since last time.
    } else if (m_penIOTarget && m_penIOTarget != m_penCurrentTarget) {
      // If valid class.
      if (IsOfClass(m_penIOTarget, "Player")) {
        // Update target.
        m_penCurrentTarget = m_penIOTarget;

        // Update indexes.
        m_iIOPlayerIndex = ((CPlayer&)*m_penCurrentTarget).GetMyPlayerIndex();
        m_iCurrentPlayerIndex = m_iIOPlayerIndex;
      // If invalid class.
      } else {
        m_penCurrentTarget = NULL;
      }
    }

    // If we have target then update info.
    if (m_penCurrentTarget) {
      m_bPlayerExists = TRUE;
      ApplyInfo();
    // Else reset info.
    } else {
      m_bPlayerExists = FALSE;
      ResetInfo();
    }
  };
  
procedures:
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_PLAYERINFO);
    SetModelMainTexture(TEXTURE_PLAYERINFO);
  
    autowait(0.1f);
  
    while (TRUE)
    {
      wait(_pTimer->TickQuantum)
      {
        on (EBegin) : { 
          if (m_bPerTickCheck) {
            UpdateInfo();
          }
          resume;
        }

        on(ETrigger) : {
          UpdateInfo();
          resume;
        }

        on(ETimer) : { stop; }
      }
    }
  }
};

