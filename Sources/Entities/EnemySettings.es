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

4444
%{
#include "StdH.h"
%}

uses "Entities/EnemyBase";

enum EEnemySettingsType {
  0 EEST_TARGETTED  "Target [0]",
  1 EEST_PENCAUSED  "Target=penCaused [1]",
};

class CEnemySettingsEntity : CEntity {
name      "EnemySettings";
thumbnail "Thumbnails\\EnemySettings.tbn";
features  "HasName","IsTargetable";

properties:

   1 CTString m_strName          "Name" 'N' = "Enemy Settings",
   2 CTString m_strDescription = "",
 
   3 BOOL m_bActive              "Active" 'A'       = TRUE, 
   4 BOOL m_bDebugMessages       "Debug Messages" = FALSE, 
   
   5 CEntityPointer m_penTarget  "Target" 'T',
   6 enum EEnemySettingsType    m_eType "Type" = EEST_PENCAUSED,

  // Flag for stretch affecting damage and speed.

  10 INDEX m_iScore              "Stat. Reward Score" = -1,
  11 BOOL m_bCountAsKill         "Stat. Reward Kill" = TRUE,
  12 BOOL m_bCountAsEnemy        "Stat. Count Enemy" = TRUE,
  13 CEntityPointer m_penSwitch  "Switch",
  14 FLOAT m_fDamageTakeMul      "Take Damage Mul" = 1.0F,
  
  15 FLOAT m_fSpeedMultiplier    "Speed Multiplier" = 1.0f,
  
  16 BOOL m_bSilent              "Silent" = FALSE,
  
  20 BOOL m_bTouchSenseless      "Touch Senseless" = FALSE,
  
  25 BOOL m_bReceiveMessage      "Receive Message" = TRUE,
  
  //30 FLOAT m_fInitialHealth      "Health Initial" = -1.0F,
  //31 FLOAT m_fMaxHealth          "Health Max" = -1.0F,

  //40 FLOAT m_fDensity            "Density" = 1000.0F,
  
  50 CEntityPointer m_penDeathTarget  "On Death target" 'D',
  //51 CEntityPointer m_penDamageTarget "On Damage target",
  //52 CEntityPointer m_penSightTarget  "On Sight target",
  //53 CEntityPointer m_penWoundTarget  "On Wound target",
  
// --------------------------------------------------------------------------------------
// C O M P O N E N T S
// --------------------------------------------------------------------------------------
components:
  1 model   MODEL_PLAYERSETTINGS   "Models\\Editor\\EnemySettings.mdl",
  2 texture TEXTURE_PLAYERSETTINGS "Models\\Editor\\EnemySettings.tex",

// --------------------------------------------------------------------------------------
// F U N C T I O N S
// --------------------------------------------------------------------------------------
functions:
  // --------------------------------------------------------------------------------------
  // [SSE] Extended Engine API
  // Returns TRUE if main entity logic is active.
  // --------------------------------------------------------------------------------------
  virtual BOOL IsActive(void) const
  {
    return m_bActive;
  }

  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const
  {
    if (m_eType == EEST_TARGETTED) {
      ((CTString&)m_strDescription).PrintF("-> Target");
      
    } else if (m_eType == EEST_PENCAUSED) {
      ((CTString&)m_strDescription).PrintF("-> penCaused");
    }

    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Event Handler here.
  // --------------------------------------------------------------------------------------
  BOOL HandleEvent(const CEntityEvent &ee)
  {
    // on ETrigger
    if (ee.ee_slEvent == EVENTCODE_ETrigger) 
    {
      // If non-active...
      if (!m_bActive) {
        return CEntity::HandleEvent(ee); 
      }
      
      // Do shit...
      switch (m_eType)
      {
        // Target
        case EEST_TARGETTED: {
          if (m_bDebugMessages) {
            CPrintF("[ES] %s : Applying settings for Target...\n", GetName());
          }

          if (IsDerivedFromClass(m_penTarget, "Enemy Base")) {
            CEnemyBase *penEnemy = static_cast<CEnemyBase*>(&*m_penTarget);
            penEnemy->m_penSettings = this;

            if (m_bDebugMessages) {
              CPrintF("  done!\n");
            }
          } else {
            if (m_bDebugMessages) {
              CPrintF("  canceled! Target is NULL or not CPlayer!\n");
            }
          }

        } break;

        // Target=penCaused
        case EEST_PENCAUSED: {
          if (m_bDebugMessages) {
            CPrintF("[ES] %s : Applying settings for Target=penCaused...\n", GetName());
          }

          ETrigger eTrigger = ((ETrigger &) ee);
          CEntity *penCaused = eTrigger.penCaused;

          if (IsDerivedFromClass(penCaused, "Enemy Base")) {
            CEnemyBase *penEnemy = static_cast<CEnemyBase*>(penCaused);
            penEnemy->m_penSettings = this;

            if (m_bDebugMessages) {
              CPrintF("  done!\n");
            }
          } else {
            if (m_bDebugMessages) {
              CPrintF("  canceled! penCaused is NULL or not CEnemyBase!\n");
            }
          }
        } break;

        default: break;
      }

      return CEntity::HandleEvent(ee); 
    }

    // on EActivate
    if (ee.ee_slEvent == EVENTCODE_EActivate)
    {
      if (m_bDebugMessages) {
        CPrintF("[ES] %s : Activated!\n", GetName());
      }

      m_bActive = TRUE;
      return CEntity::HandleEvent(ee); 
    }

    // on EDeactivate
    if (ee.ee_slEvent == EVENTCODE_EDeactivate)
    {
      if (m_bDebugMessages) {
        CPrintF("[ES] %s : Deactivated!\n", GetName());
      }
      
      m_bActive = FALSE;
      return CEntity::HandleEvent(ee); 
    }

    return CEntity::HandleEvent(ee); 
  }

// --------------------------------------------------------------------------------------
// P R O C E D U R E S
// --------------------------------------------------------------------------------------
procedures:
  // --------------------------------------------------------------------------------------
  // Entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    // set the nodel
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_PLAYERSETTINGS);
    SetModelMainTexture(TEXTURE_PLAYERSETTINGS);

    return;
  }
};
