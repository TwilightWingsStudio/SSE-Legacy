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
  #include "EntitiesMP/Player.h"
%}

class CPlayerInfo: CRationalEntity {
name      "Player Info";
thumbnail "Thumbnails\\PlayerInfo.tbn";
features  "HasName", "IsTargetable", "HasTarget";

properties:
  1 CTString m_strName "Name" 'N'      = "Player Info",
  2 INDEX m_iPlayer    "Player Number" = -1,
  3 INDEX m_iCurrent                   = -1,
  4 CEntityPointer m_penTarget  "Player Target", // used both for input and output, m_penCurrent used to compare for change
  5 CEntityPointer m_penCurrent,
  
  6 BOOL m_bPerTickCheck "Per Tick Check" = FALSE,
  
  // info comes now
  20 FLOAT m_fHealth "Player Health" = -1.0f,
  21 FLOAT m_fArmor  "Player Armor"  = -1.0f,
  25 BOOL m_bDead    "Is Dead"       = FALSE,
  35 CTString m_strPlayerName  "Player Name" = "",

components:
  1 model   MODEL_PLAYERINFO   "Models\\Editor\\PlayerInfo.mdl",
  2 texture TEXTURE_PLAYERINFO "Models\\Editor\\PlayerInfo.tex",

functions:
  void UpdateInfo()
  {
    INDEX i = 0;

    //if player changed since last time
    if (m_iCurrent != m_iPlayer) {
      // for each entity in the world
      {FOREACHINDYNAMICCONTAINER(GetWorld()->wo_cenEntities, CEntity, iten) {
        CEntity *pen = iten;
        if (IsDerivedFromClass(pen, "Player")) {
          if (i == m_iPlayer) {
            m_iCurrent = i;
            m_penTarget = pen;
            m_penCurrent = pen;
            break;
          }
          i++;
        }
      }}
    } else if (m_penTarget != m_penCurrent && m_penTarget) {
      m_penCurrent = m_penTarget;

      // for each entity in the world
      {FOREACHINDYNAMICCONTAINER(GetWorld()->wo_cenEntities, CEntity, iten) {
        CEntity *pen = iten;
        if (IsDerivedFromClass(pen, "Player")) {
          if (pen==m_penTarget) {
            m_iPlayer = i;
            m_iCurrent = i;
            break;
          }
          i++;
        }
      }}
    }

    if (m_penCurrent) {
      CPlayer &penPlayer = (CPlayer&)*m_penCurrent;

      m_fHealth = penPlayer.GetHealth();
      m_fArmor = penPlayer.m_fArmor;
      m_strPlayerName = penPlayer.m_strName;
      m_bDead = !(penPlayer.GetFlags()&ENF_ALIVE);
    } else {
      m_fHealth = -1.0f;
      m_fArmor = -1.0f;
      m_strPlayerName = "";
      m_bDead = FALSE;
      m_iPlayer = -1;
      m_iCurrent = -1;
      m_penTarget = NULL;
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
  
    while (TRUE) {
      wait(_pTimer->TickQuantum) {
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

