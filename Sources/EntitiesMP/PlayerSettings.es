3333
%{
#include "StdH.h"
%}

uses "EntitiesMP/Player";

enum EPlayerSettingsType {
  0 EPST_TARGETTED  "0 Target",
  1 EPST_PENCAUSED  "1 Target=penCaused",
  2 EPST_ALLPLAYERS "2 Affect All Players",
};
 
enum EPSHPRegenValueType {
  0 PSHRT_HEALTH_POINTS         "0 Health Points",
  1 PSHRT_PERCENT_OF_CURHEALTH  "1 % of CurrentHealth",
  2 PSHRT_PERCENT_OF_TOPHEALTH  "2 % of TopHealth",
  3 PSHRT_PERCENT_OF_MAXHEALTH  "3 % of MaxHealth",
};

enum EPSAPRegenValueType {
  0 PSART_ARMOR_POINTS         "0 Armor Points",
  1 PSART_PERCENT_OF_CURARMOR  "1 % of CurrentArmor",
  2 PSART_PERCENT_OF_TOPARMOR  "2 % of TopArmor",
  3 PSART_PERCENT_OF_MAXARMOR  "3 % of MaxArmor",
};

class CPlayerSettingsEntity: CEntity {
name      "PlayerSettings";
thumbnail "Thumbnails\\PlayerSettings.tbn";
features  "HasName","IsTargetable";

properties:

   1 CTString m_strName          "Name" 'N' = "Player Settings",
   2 CTString m_strDescription = "Player Settings",
 
   3 BOOL m_bActive              "Active" 'A'       = TRUE, 
   4 BOOL m_bDebugMessages       "Debug Messages" = FALSE, 
   
   5 CEntityPointer m_penTarget  "Target" 'T',
   6 enum EPlayerSettingsType  m_epstType "Type" = EPST_TARGETTED,
   7 BOOL m_bUntilDeath          "Until Death" = FALSE,
   
  // Physics 
  10 FLOAT m_fMaxHoldBreath      "Max Hold Breath" = 60.0F,

  11 FLOAT m_fDensity            "Density (Stand/Dive)" = 1000.0F,
  12 FLOAT m_fDensitySwim        "Density (Swim)" = 500.0F,
  13 FLOAT m_fDensityDead        "Density (Dead)" = 400.0F,

  14 FLOAT m_fMaxJumpControl        "Max Jump Control" = 0.5F,
  15 FLOAT m_fJumpControlMultiplier "Mul Jump Control" = 0.5F,
  16 FLOAT m_fAcceleration          "Acceleration" = 100.0f,
  17 FLOAT m_fDeceleration          "Deceleration" = 60.0f,

  18 FLOAT m_fStepUpHeight      "Step Up Height" = 0.5F,
  19 FLOAT m_fStepDownHeight    "Step Down Height" = -1.0F,
  20 FLOAT m_fCollisionDamageFactor "Mul Collision Damage" = 20.0F,
  21 FLOAT m_fCollisionSpeedLimit "Collision Speed Limit" = 20.0F,
  
  // Speed Multipliers
  30 FLOAT m_fSpeedAllMultiplier "Mul Speed All" = 1.0F,
  31 FLOAT m_fSpeedForwardMultiplier "Mul Speed Forward" = 1.0F,
  32 FLOAT m_fSpeedBackwardMultiplier "Mul Speed Backward" = 1.0F,
  33 FLOAT m_fSpeedLeftMultiplier "Mul Speed Left" = 1.0F,
  34 FLOAT m_fSpeedRightMultiplier "Mul Speed Right" = 1.0F,
  35 FLOAT m_fSpeedUpMultiplier "Mul Speed Up" = 1.0F,
  36 FLOAT m_fSpeedDownMultiplier "Mul Speed Down" = 1.0F,
   
  // Receive / Pickup Multipliers
  40 FLOAT m_fDamageReceiveMul  "Mul Receive Damage" = 1.0F,
  41 FLOAT m_fHeathPickUpMul    "Mul Health PickUp" = 1.0F,
  42 FLOAT m_fArmorPickUpMul    "Mul Armor PickUp" = 1.0F,
 
  43 FLOAT m_fScoreReceiveMul   "Mul Receive Score" = 1.0F,
  
  50 FLOAT m_fArmorAbsorbtionMul "Mul Armor Absorbtion" = 0.66F,
   
  //59 FLOAT m_fMaxGravitySpeed   "Max Gravity Speed" = 70.0F,
  
  // PickUp disable/enable
  60 BOOL m_bCanPickUpHealth    "Can PickUp Health" = TRUE,
  61 BOOL m_bCanPickUpArmor     "Can PickUp Armor" = TRUE,
  62 BOOL m_bCanPickUpKeys      "Can PickUp Keys" = TRUE,
  63 BOOL m_bCanPickUpPUPS      "Can PickUp PowerUPs" = TRUE,
  64 BOOL m_bCanPickUpWeapons   "Can PickUp Weapons" = TRUE,
  65 BOOL m_bCanPickUpAmmo      "Can PickUp Ammo" = TRUE,
 
  // Health and Armor
  70 FLOAT m_fTopHealth "Health Top" = 100.0F,
  71 FLOAT m_fMaxHealth "Health Max" = 200.0F,
 
  75 FLOAT m_fTopArmor  "Armor Top" = 100.0F,
  76 FLOAT m_fMaxArmor  "Armor Max" = 200.0F,
 
  // Health Regeneration
 120 FLOAT m_fHealthRegenValue  "HP Regen. Val" = 0.0F, 
 121 FLOAT m_fHealthRegenMin    "HP Regen. Min" = 0.0F, 
 122 FLOAT m_fHealthRegenMax    "HP Regen. Max" = 0.0F, 
 123 FLOAT m_fHealthRegenTimer  "HP Regen. Timer" = 0.0F,
 125 enum EPSHPRegenValueType m_etrvtHealthRegenValType "HP Regen. Val Type" = PSHRT_HEALTH_POINTS,
 126 enum EPSHPRegenValueType m_etrvtHealthRegenMinType "HP Regen. Min Type" = PSHRT_HEALTH_POINTS,
 127 enum EPSHPRegenValueType m_etrvtHealthRegenMaxType "HP Regen. Max Type" = PSHRT_HEALTH_POINTS,
  
  // Armor Regeneration
 130 FLOAT m_fArmorRegenValue  "AP Regen. Val" = 0.0F, 
 131 FLOAT m_fArmorRegenMin    "AP Regen. Min" = 0.0F, 
 132 FLOAT m_fArmorRegenMax    "AP Regen. Max" = 0.0F, 
 133 FLOAT m_fArmorRegenTimer  "AP Regen. Timer" = 0.0F,
 135 enum EPSAPRegenValueType m_etrvtArmorRegenValType "AP Regen. Val Type" = PSART_ARMOR_POINTS,
 136 enum EPSAPRegenValueType m_etrvtArmorRegenMinType "AP Regen. Min Type" = PSART_ARMOR_POINTS,
 137 enum EPSAPRegenValueType m_etrvtArmorRegenMaxType "AP Regen. Max Type" = PSART_ARMOR_POINTS,

// --------------------------------------------------------------------------------------
// C O M P O N E N T S
// --------------------------------------------------------------------------------------
components:
  1 model   MODEL_PLAYERSETTINGS   "Models\\Editor\\PlayerSettings.mdl",
  2 texture TEXTURE_PLAYERSETTINGS "Models\\Editor\\PlayerSettings.tex",

// --------------------------------------------------------------------------------------
// F U N C T I O N S
// --------------------------------------------------------------------------------------
functions:
  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const
  {
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
      
      m_fMaxHoldBreath = ClampDn(m_fMaxHoldBreath, 0.0F);

      // Do shit...
      switch (m_epstType) {
        // Target
        case EPST_TARGETTED: {
          if (m_bDebugMessages) {
            CPrintF("[PS] %s : Applying settings for Target...\n", GetName());
          }

          if (IsOfClass(m_penTarget, "Player")) {
            CPlayer *penPlayer = (CPlayer*)&*m_penTarget;
            penPlayer->m_penSettings = this;

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
        case EPST_PENCAUSED: {
          if (m_bDebugMessages) {
            CPrintF("[PS] %s : Applying settings for Target=penCaused...\n", GetName());
          }

          ETrigger eTrigger = ((ETrigger &) ee);
          CEntity *penCaused = eTrigger.penCaused;

          if (IsOfClass(penCaused, "Player")) {
            CPlayer *penPlayer = (CPlayer*)penCaused;
            penPlayer->m_penSettings = this;

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
        case EPST_ALLPLAYERS: {
          if (m_bDebugMessages) {
            CPrintF("[PS] %s : Applying settings for all players...\n", GetName());
          }

          INDEX ctPlayers = CEntity::GetMaxPlayers(); 
          
          // Cycle through all players...
          for (INDEX iPlayer = 0; iPlayer < ctPlayers; iPlayer++) { 
            CPlayer *penPlayer = (CPlayer *)CEntity::GetPlayerEntity(iPlayer); 
          
            // Skip invalid players...
            if (penPlayer == NULL) { 
              continue;
            }

            penPlayer->m_penSettings = this;
            CPrintF("  applied for PLID #%d\n", iPlayer);
          }

          CPrintF("  done!\n");
        } break;

        default: break;
      }

      return CEntity::HandleEvent(ee); 
    }

    // on EActivate
    if (ee.ee_slEvent == EVENTCODE_EActivate) {
      m_bActive = TRUE;
      return CEntity::HandleEvent(ee); 
    }

    // on EDeactivate
    if (ee.ee_slEvent == EVENTCODE_EDeactivate) {
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

    m_fMaxHoldBreath = ClampDn(m_fMaxHoldBreath, 0.0F);

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
