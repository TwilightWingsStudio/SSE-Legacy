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

3444
%{
  #include "StdH.h"
%}

uses "Entities/Player";

enum EPlayerVitalitySettingsType {
  0 EPVST_TARGETTED  "0 Target",
  1 EPVST_PENCAUSED  "1 Target=penCaused",
  2 EPVST_ALLPLAYERS "2 Affect All Players",
};
 
enum EPSRegenValueType {
  0 PSRT_EXACT_POINTS         "0 Exact Points",
  1 PSRT_PERCENT_OF_CURVALUE  "1 % of CurrentValue",
  2 PSRT_PERCENT_OF_TOPVALUE  "2 % of TopValue",
  3 PSRT_PERCENT_OF_MAXVALUE  "3 % of MaxValue",
};

class CPlayerVitalitySettingsEntity : CEntity {
name      "PlayerVitalitySettings";
thumbnail "Thumbnails\\PlayerVitalitySettings.tbn";
features  "HasName","IsTargetable";

properties:

   1 CTString m_strName          "Name" 'N' = "Pl. Vitality Settings",
   2 CTString m_strDescription = "Pl. Vitality Settings",
 
   3 BOOL m_bActive              "Active" 'A'       = TRUE, 
   4 BOOL m_bDebugMessages       "Debug Messages" = FALSE, 
   
   5 CEntityPointer m_penTarget  "Target" 'T',
   6 enum EPlayerVitalitySettingsType  m_epstType "Type" = EPVST_PENCAUSED,
   7 BOOL m_bUntilDeath          "Until Death" = FALSE,

  // Receive & Pickup Multipliers
  40 FLOAT m_fDamageReceiveMul  "Mul Damage All" = 1.0F,
  41 FLOAT m_fSelfDamageMul     "Mul Damage Self" = 1.0F,

  42 FLOAT m_fHealthPickUpMul    "Mul Health PickUp" = 1.0F,
  43 FLOAT m_fArmorPickUpMul    "Mul Armor PickUp" = 1.0F,
  
  50 FLOAT m_fArmorAbsorbtionMul "Mul Armor Absorbtion" = 0.66F,

  // Health, Armor & Shields
  70 FLOAT m_fTopHealth "Health Top" = 100.0F,
  71 FLOAT m_fMaxHealth "Health Max" = 200.0F,

  75 FLOAT m_fTopArmor  "Armor Top" = 100.0F,
  76 FLOAT m_fMaxArmor  "Armor Max" = 200.0F,

  80 FLOAT m_fTopShields "Shields Top" = 100.0F,
  81 FLOAT m_fMaxShields "Shields Max" = 200.0F,

  90 BOOL m_bCanDie     "Can Die" = TRUE,
 
  // Health Regeneration
 120 FLOAT m_fHealthRegenValue  "HP Regen. Val" = 0.0F, 
 121 FLOAT m_fHealthRegenMin    "HP Regen. Min" = 0.0F, 
 122 FLOAT m_fHealthRegenMax    "HP Regen. Max" = 0.0F, 
 123 FLOAT m_fHealthRegenTimer  "HP Regen. Timer" = 0.0F,
 125 enum EPSRegenValueType m_etrvtHealthRegenValType "HP Regen. Val Type" = PSRT_EXACT_POINTS,
 126 enum EPSRegenValueType m_etrvtHealthRegenMinType "HP Regen. Min Type" = PSRT_EXACT_POINTS,
 127 enum EPSRegenValueType m_etrvtHealthRegenMaxType "HP Regen. Max Type" = PSRT_EXACT_POINTS,
  
  // Armor Regeneration
 130 FLOAT m_fArmorRegenValue  "AP Regen. Val" = 0.0F, 
 131 FLOAT m_fArmorRegenMin    "AP Regen. Min" = 0.0F, 
 132 FLOAT m_fArmorRegenMax    "AP Regen. Max" = 0.0F, 
 133 FLOAT m_fArmorRegenTimer  "AP Regen. Timer" = 0.0F,
 135 enum EPSRegenValueType m_etrvtArmorRegenValType "AP Regen. Val Type" = PSRT_EXACT_POINTS,
 136 enum EPSRegenValueType m_etrvtArmorRegenMinType "AP Regen. Min Type" = PSRT_EXACT_POINTS,
 137 enum EPSRegenValueType m_etrvtArmorRegenMaxType "AP Regen. Max Type" = PSRT_EXACT_POINTS,

// --------------------------------------------------------------------------------------
// C O M P O N E N T S
// --------------------------------------------------------------------------------------
components:
  1 model   MODEL_PLAYERSETTINGS   "Models\\Editor\\PlayerVitalitySettings.mdl",
  2 texture TEXTURE_PLAYERSETTINGS "Models\\Editor\\PlayerVitalitySettings.tex",

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
    if (m_bUntilDeath) {
      ((CTString&)m_strDescription).PrintF("[UD]");
    } else {
      ((CTString&)m_strDescription).PrintF("[Always]");
    }
    
    if (m_epstType == EPVST_TARGETTED) {
      ((CTString&)m_strDescription).PrintF("%s-> Target", m_strDescription);
      
    } else if (m_epstType == EPVST_PENCAUSED) {
      ((CTString&)m_strDescription).PrintF("%s-> penCaused", m_strDescription);

    } else {
      ((CTString&)m_strDescription).PrintF("%s-> AllPlayers", m_strDescription);
      
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
      
      // retard protection
      m_fMaxHealth = ClampDn(m_fMaxHealth, 0.0F);
      m_fTopHealth = Clamp(m_fTopHealth, 0.0F, m_fMaxHealth);

      m_fMaxArmor = ClampDn(m_fMaxArmor, 0.0F);
      m_fTopArmor = Clamp(m_fTopArmor, 0.0F, m_fMaxArmor);

      // Do shit...
      switch (m_epstType) {
        // Target
        case EPVST_TARGETTED: {
          if (m_bDebugMessages) {
            CPrintF("[PVS] %s : Applying settings for Target...\n", GetName());
          }

          if (IsOfClass(m_penTarget, "Player")) {
            CPlayer *penPlayer = (CPlayer*)&*m_penTarget;
            penPlayer->m_penVitalitySettings = this;

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
        case EPVST_PENCAUSED: {
          if (m_bDebugMessages) {
            CPrintF("[PVS] %s : Applying settings for Target=penCaused...\n", GetName());
          }

          ETrigger eTrigger = ((ETrigger &) ee);
          CEntity *penCaused = eTrigger.penCaused;

          if (IsOfClass(penCaused, "Player")) {
            CPlayer *penPlayer = (CPlayer*)penCaused;
            penPlayer->m_penVitalitySettings = this;

            if (m_bDebugMessages) {
              CPrintF("  done!\n");
            }
          } else {
            if (m_bDebugMessages) {
              CPrintF("  canceled! penCaused is NULL or not CPlayer!\n");
            }
          }
        } break;

        // All players
        case EPVST_ALLPLAYERS: {
          if (m_bDebugMessages) {
            CPrintF("[PVS] %s : Applying settings for all players...\n", GetName());
          }

          INDEX ctPlayers = CEntity::GetMaxPlayers(); 
          
          // Cycle through all players...
          for (INDEX iPlayer = 0; iPlayer < ctPlayers; iPlayer++) { 
            CPlayer *penPlayer = (CPlayer *)CEntity::GetPlayerEntity(iPlayer); 
          
            // Skip invalid players...
            if (penPlayer == NULL) { 
              continue;
            }

            penPlayer->m_penVitalitySettings = this;
            CPrintF("  applied for PLID #%d\n", iPlayer);
          }

          CPrintF("  done!\n");
        } break;

        default: break;
      }

      return CEntity::HandleEvent(ee); 
    }

    // on EActivate
    if (ee.ee_slEvent == EVENTCODE_EActivate)
    {
      if (m_bDebugMessages) {
        CPrintF("[PVS] %s : Activated!\n", GetName());
      }

      m_bActive = TRUE;
      return CEntity::HandleEvent(ee); 
    }

    // on EDeactivate
    if (ee.ee_slEvent == EVENTCODE_EDeactivate)
    {
      if (m_bDebugMessages) {
        CPrintF("[PVS] %s : Deactivated!\n", GetName());
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
    // retard protection
    m_fMaxHealth = ClampDn(m_fMaxHealth, 0.0F);
    m_fTopHealth = Clamp(m_fTopHealth, 0.0F, m_fMaxHealth);

    m_fMaxArmor = ClampDn(m_fMaxArmor, 0.0F);
    m_fTopArmor = Clamp(m_fTopArmor, 0.0F, m_fMaxArmor);

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
