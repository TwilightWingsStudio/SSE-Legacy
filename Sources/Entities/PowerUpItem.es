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

808
%{
#include "StdH.h"
#include "Models/Items/ItemHolder/ItemHolder.h"
%}

uses "Entities/Item";
uses "Entities/Player";

// PowerUp type. 
enum PowerUpItemType {
  0 PUIT_INVISIB  "[0] Invisibility",
  1 PUIT_INVULNER "[1] Invulnerability",
  2 PUIT_DAMAGE   "[2] SeriousDamage",
  3 PUIT_SPEED    "[3] SeriousSpeed",
  4 PUIT_BOMB     "[4] SeriousBomb",
  
  // [SSE] Gameplay - Regeneration PowerUp
  5 PUIT_REGENERATION "[5] Regeneration",
};

// Event for sending through receive item.
event EPowerUp {
  CEntityPointer penItem,
  enum PowerUpItemType puitType,
  FLOAT fValue, // [SSE] PowerUps Drop
  BOOL bGenericPowerUp,
  BOOL bDurationStacking,
};

%{
  
const char *GetPowerUpPickMessage(enum PowerUpItemType puit)
{
  switch (puit)
  {
    case PUIT_INVISIB  : return TRANS("^cABE3FFInvisibility"); break;
    case PUIT_INVULNER : return TRANS("^c00B440Invulnerability"); break;
    case PUIT_DAMAGE   : return TRANS("^cFF0000Serious Damage!"); break;
    case PUIT_SPEED    : return TRANS("^cFF9400Serious Speed"); break;
    case PUIT_BOMB     : return TRANS("^cFF0000Serious Bomb!"); break;

    // [SSE] Gameplay - Regeneration PowerUp
    case PUIT_REGENERATION     : return TRANS("^cFF80FFRegeneration!"); break;
    
    default: return TRANS("unknown item"); break;
  };
}

%}

class CPowerUpItem : CItem 
{
name      "PowerUp Item";
thumbnail "Thumbnails\\PowerUpItem.tbn";

properties:
  1 enum PowerUpItemType m_puitType  "Type" 'Y' = PUIT_INVULNER,
//  3 INDEX m_iSoundComponent = 0,

components:
  0 class   CLASS_BASE      "Classes\\Item.ecl",

// ********* INVISIBILITY *********
  1 model   MODEL_INVISIB   "ModelsMP\\Items\\PowerUps\\Invisibility\\Invisibility.mdl",
// 2 texture TEXTURE_INVISIB "ModelsMP\\Items\\PowerUps\\Invisibility\\Invisibility.tex",

// ********* INVULNERABILITY *********
 10 model   MODEL_INVULNER  "ModelsMP\\Items\\PowerUps\\Invulnerability\\Invulnerability.mdl",
// 11 texture TEXTURE_INVULNER  "ModelsMP\\Items\\PowerUps\\Invulnerability\\Invulnerability.tex",

// ********* SERIOUS DAMAGE *********
 20 model   MODEL_DAMAGE    "ModelsMP\\Items\\PowerUps\\SeriousDamage\\SeriousDamage.mdl",
 21 texture TEXTURE_DAMAGE  "ModelsMP\\Items\\PowerUps\\SeriousDamage\\SeriousDamage.tex",

// ********* SERIOUS SPEED *********
 30 model   MODEL_SPEED     "ModelsMP\\Items\\PowerUps\\SeriousSpeed\\SeriousSpeed.mdl",
 31 texture TEXTURE_SPEED   "ModelsMP\\Items\\PowerUps\\SeriousSpeed\\SeriousSpeed.tex",

// ********* SERIOUS BOMB *********
 40 model   MODEL_BOMB      "ModelsMP\\Items\\PowerUps\\SeriousBomb\\SeriousBomb.mdl",
 41 texture TEXTURE_BOMB    "ModelsMP\\Items\\PowerUps\\SeriousBomb\\SeriousBomb.tex",

 // ********* MISC *********
 50 texture TEXTURE_SPECULAR_STRONG  "ModelsMP\\SpecularTextures\\Strong.tex",
 51 texture TEXTURE_SPECULAR_MEDIUM  "ModelsMP\\SpecularTextures\\Medium.tex",
 52 texture TEXTURE_REFLECTION_METAL "ModelsMP\\ReflectionTextures\\LightMetal01.tex",
 53 texture TEXTURE_REFLECTION_GOLD  "ModelsMP\\ReflectionTextures\\Gold01.tex",
 54 texture TEXTURE_REFLECTION_PUPLE "ModelsMP\\ReflectionTextures\\Purple01.tex",
 55 texture TEXTURE_FLARE "Models\\Items\\Flares\\Flare.tex",
 56 model   MODEL_FLARE   "Models\\Items\\Flares\\Flare.mdl",
 
 80 model   MODEL_REGENERATION    "Models\\Items\\PowerUps\\Regeneration\\Regeneration.mdl",
 81 texture TEXTURE_REGENERATION  "Models\\Items\\PowerUps\\Regeneration\\Regeneration.tex",

// ************** SOUNDS **************
300 sound   SOUND_PICKUP   "SoundsMP\\Items\\PowerUp.wav",
301 sound   SOUND_INVISIB  "SoundsMP\\Items\\Invisibility.wav",
302 sound   SOUND_INVULNER "SoundsMP\\Items\\Invulnerability.wav",
303 sound   SOUND_DAMAGE   "SoundsMP\\Items\\SeriousDamage.wav",
304 sound   SOUND_SPEED    "SoundsMP\\Items\\SeriousSpeed.wav",
305 sound   SOUND_BOMB     "SoundsMP\\Items\\SeriousBomb.wav",

306 sound   SOUND_REGENERATION     "SoundsMP\\Items\\Regeneration.wav",

functions:
  // --------------------------------------------------------------------------------------
  // No comments.
  // --------------------------------------------------------------------------------------
  void Precache(void)
  {
    switch (m_puitType)
    {
      case PUIT_SPEED   :  PrecacheSound(SOUND_SPEED   );  break;
      case PUIT_INVISIB :  PrecacheSound(SOUND_INVISIB );  break;
      case PUIT_INVULNER:  PrecacheSound(SOUND_INVULNER);  break;
      case PUIT_DAMAGE  :  PrecacheSound(SOUND_DAMAGE  );  break;
      case PUIT_BOMB    :  PrecacheSound(SOUND_BOMB    );  break;

      // [SSE] Gameplay - Regeneration PowerUp
      case PUIT_REGENERATION  :  PrecacheSound(SOUND_REGENERATION);  break;
      
      default: PrecacheSound(SOUND_PICKUP); break;
    }
  }

  // --------------------------------------------------------------------------------------
  // Fill in entity statistics - for AI purposes only.
  // --------------------------------------------------------------------------------------
  BOOL FillEntityStatistics(EntityStats *pes)
  {
    pes->es_strName = "PowerUp"; 
    pes->es_ctCount = 1;
    pes->es_ctAmmount = 1;  // !!!!
    pes->es_fValue = 0;     // !!!!
    pes->es_iScore = 0;//m_iScore;
    
    switch (m_puitType)
    {
      case PUIT_INVISIB :  pes->es_strName += " invisibility";     break;
      case PUIT_INVULNER:  pes->es_strName += " invulnerability";  break;
      case PUIT_DAMAGE  :  pes->es_strName += " serious damage";   break;
      case PUIT_SPEED   :  pes->es_strName += " serious speed";    break;
      case PUIT_BOMB    :  pes->es_strName = "Serious Bomb!";

      // [SSE] Gameplay - Regeneration PowerUp
      case PUIT_REGENERATION   :  pes->es_strName += " regeneration";    break;
    }

    return TRUE;
  }

  // --------------------------------------------------------------------------------------
  // Render particles.
  // --------------------------------------------------------------------------------------
  void RenderParticles(void)
  {
    // No particles when not existing!
    if (GetRenderType() != CEntity::RT_MODEL) {
      return;
    }
    
    // No particles when in DM modes!
    if (GetSP()->sp_gmGameMode > CSessionProperties::GM_COOPERATIVE || !ShowItemParticles()) {
      return;
    }
    
    BOOL bOnGround = m_eotOscillation == 1;

    switch( m_puitType)
    {
      case PUIT_INVISIB:
        Particles_Stardust( this, 2.0F * 0.75F * m_fStretch, (bOnGround ? 0.75F : 1.00F * 0.75F) * m_fStretch, PT_STAR08, 320);
        break;
      case PUIT_INVULNER:
        Particles_Stardust( this, 2.0F * 0.75F * m_fStretch, (bOnGround ? 0.75F : 1.00F * 0.75F) * m_fStretch, PT_STAR08, 192);
        break;
      case PUIT_DAMAGE:
        Particles_Stardust( this, 1.0F * 0.75F * m_fStretch, (bOnGround ? 0.375F : 0.75F * 0.75F) * m_fStretch, PT_STAR08, 128);
        break;
      case PUIT_SPEED:
        Particles_Stardust( this, 1.0F * 0.75F * m_fStretch, (bOnGround ? 0.375F : 0.75F * 0.75F) * m_fStretch, PT_STAR08, 128);
        break;
      case PUIT_BOMB:
        Particles_Atomic(this, 2.0F * 0.75F * m_fStretch, (bOnGround ? 0.75F : 2.0F * 0.95F) * m_fStretch, PT_STAR05, 12);
        break;

      // [SSE] Gameplay - Regeneration PowerUp
      case PUIT_REGENERATION:
        Particles_Stardust( this, 2.0F * 0.75F * m_fStretch, (bOnGround ? 0.75F : 1.00F * 0.75F) * m_fStretch, PT_STAR08, 192);
        break;
    }
  }

  // --------------------------------------------------------------------------------------
  // Set health properties depending on health type.
  // --------------------------------------------------------------------------------------
  void SetProperties(void)
  {
    switch( m_puitType)
    {
      case PUIT_INVISIB: {
        ForceCollisionBoxIndexChange( ITEMHOLDER_COLLISION_BOX_BIG);
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 40.0F; 
        m_strDescription.PrintF("Invisibility");

        AddItem(  MODEL_INVISIB, TEXTURE_REFLECTION_METAL, 0, TEXTURE_SPECULAR_STRONG, 0);  // set appearance
        AddFlare( MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.2F, 0.0F), FLOAT3D(1.0F, 1.0F, 0.3F) );  // add flare
        StretchItem( FLOAT3D(1.0F*0.75F, 1.0F*0.75F, 1.0F*0.75F));
      } break;

      case PUIT_INVULNER: {
        ForceCollisionBoxIndexChange( ITEMHOLDER_COLLISION_BOX_BIG);
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 60.0F; 
        m_strDescription.PrintF("Invulnerability");

        AddItem(  MODEL_INVULNER, TEXTURE_REFLECTION_GOLD, TEXTURE_REFLECTION_METAL, TEXTURE_SPECULAR_MEDIUM, 0);  // set appearance
        AddFlare( MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.2F, 0.0F), FLOAT3D(1.0F, 1.0F, 0.3F) );  // add flare
        StretchItem( FLOAT3D(1.0F*0.75F, 1.0F*0.75F, 1.0F*0.75));
      } break;

      case PUIT_DAMAGE: {
        ForceCollisionBoxIndexChange( ITEMHOLDER_COLLISION_BOX_BIG);
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 40.0F; 
        m_strDescription.PrintF("SeriousDamage");

        AddItem(  MODEL_DAMAGE, TEXTURE_DAMAGE, 0, TEXTURE_SPECULAR_STRONG, 0);  // set appearance
        AddFlare( MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.2F, 0.0F), FLOAT3D(1.0F, 1.0F, 0.3F) );  // add flare
        StretchItem( FLOAT3D(1.0F*0.75F, 1.0F*0.75F, 1.0F*0.75));
      } break;

      case PUIT_SPEED: {
        ForceCollisionBoxIndexChange( ITEMHOLDER_COLLISION_BOX_BIG);
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 40.0F; 
        m_strDescription.PrintF("SeriousSpeed");

        AddItem(  MODEL_SPEED, TEXTURE_SPEED, 0, 0, 0);  // set appearance
        AddFlare( MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.2F, 0.0F), FLOAT3D(1.0F, 1.0F, 0.3F) );  // add flare
        StretchItem( FLOAT3D(1.0F*0.75F, 1.0F*0.75F, 1.0F*0.75));
      } break;

      case PUIT_BOMB: {
        ForceCollisionBoxIndexChange( ITEMHOLDER_COLLISION_BOX_BIG);
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 40.0F; 
        m_strDescription.PrintF("Serious Bomb!");

        AddItem(  MODEL_BOMB, TEXTURE_BOMB, 0, 0, 0);  // set appearance
        AddFlare( MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.2F, 0.0F), FLOAT3D(1.0F, 1.0F, 0.3F) );  // add flare
        StretchItem( FLOAT3D(1.0F*3.0F, 1.0F*3.0F, 1.0F*3.0));
      } break;
      
      // [SSE] Gameplay - Regeneration PowerUp
      case PUIT_REGENERATION: {
        ForceCollisionBoxIndexChange( ITEMHOLDER_COLLISION_BOX_BIG);
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 40.0F; 
        m_strDescription.PrintF("Regeneration");

        AddItem(  MODEL_REGENERATION, TEXTURE_REGENERATION, 0, 0, 0);  // set appearance
        AddFlare( MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.2F, 0.0F), FLOAT3D(1.0F, 1.0F, 0.3F) );  // add flare
        StretchItem( FLOAT3D(1.0F*0.75F, 1.0F*0.75F, 1.0F*0.75));
      } break;
    }
  };
  
  // --------------------------------------------------------------------------------------
  // Change item by session properties.
  // --------------------------------------------------------------------------------------
  void AdjustDifficulty(void)
  {
    // [SSE] PowerUps Allow
    if (!GetSP()->sp_bAllowPowerUps && m_penTarget == NULL) {
      Destroy();
    }
  }
 
procedures:
  // --------------------------------------------------------------------------------------
  // Called every time when any player trying to pick up item.
  // --------------------------------------------------------------------------------------
  ItemCollected( EPass epass) : CItem::ItemCollected
  {
    ASSERT(epass.penOther != NULL);
 
    // Don't pick up more bombs than you can carry.
    if (m_puitType == PUIT_BOMB)
    {
      if (IsOfClass(epass.penOther, "Player"))
      {
        if (((CPlayer &)*epass.penOther).m_iSeriousBombCount >= 3) {
          return;
        }
      }
    }

    if (!(m_bPickupOnce || m_bRespawn))
    {
      BOOL bWasPicked = MarkPickedBy(epass.penOther);

      // If already picked by this player then don't pick again.
      if (bWasPicked) {
        return;
      }
    }

    // Prepare PowerUp to send it to entity.
    EPowerUp ePowerUp;
    ePowerUp.penItem = NULL;
    ePowerUp.puitType = m_puitType;
    ePowerUp.fValue = m_fValue; // [SSE] PowerUps Drop
    ePowerUp.bGenericPowerUp = FALSE;
    ePowerUp.bDurationStacking = m_bDropped;

    // If powerup is received...
    if (epass.penOther->ReceiveItem(ePowerUp))
    {
      if (_pNetwork->IsPlayerLocal(epass.penOther))
      {
        switch (m_puitType)
        {
          case PUIT_INVISIB:  IFeel_PlayEffect("PU_Invulnerability"); break;
          case PUIT_INVULNER: IFeel_PlayEffect("PU_Invulnerability"); break;
          case PUIT_DAMAGE:   IFeel_PlayEffect("PU_Invulnerability"); break;
          case PUIT_SPEED:    IFeel_PlayEffect("PU_FastShoes"); break; 
          case PUIT_BOMB:     IFeel_PlayEffect("PU_SeriousBomb"); break;

          case PUIT_REGENERATION: IFeel_PlayEffect("PU_Invulnerability"); break;
        }
      }
      
      // Play the pickup sound.
      m_soPick.Set3DParameters(50.0F, 1.0F, 2.0F, 1.0F);

      if (m_puitType == PUIT_BOMB) {
        PlaySound(m_soPick, SOUND_BOMB, SOF_3D);
        m_fPickSoundLen = GetSoundLength(SOUND_BOMB);

      } else if (m_puitType == PUIT_SPEED) {
        PlaySound(m_soPick, SOUND_SPEED, SOF_3D);
        m_fPickSoundLen = GetSoundLength(SOUND_SPEED);
        
      } else if (m_puitType == PUIT_INVISIB) {
        PlaySound(m_soPick, SOUND_INVISIB, SOF_3D);
        m_fPickSoundLen = GetSoundLength(SOUND_INVISIB);
        
      } else if (m_puitType == PUIT_INVULNER) {
        PlaySound(m_soPick, SOUND_INVULNER, SOF_3D);
        m_fPickSoundLen = GetSoundLength(SOUND_INVULNER);
        
      } else if (m_puitType == PUIT_DAMAGE) {
        PlaySound(m_soPick, SOUND_DAMAGE, SOF_3D);
        m_fPickSoundLen = GetSoundLength(SOUND_DAMAGE);
        
      // [SSE] Gameplay - Regeneration PowerUp
      } else if (m_puitType == PUIT_REGENERATION) {
        PlaySound(m_soPick, SOUND_REGENERATION, SOF_3D);
        m_fPickSoundLen = GetSoundLength(SOUND_REGENERATION);
        
      } else if (TRUE) {
        PlaySound(m_soPick, SOUND_PICKUP, SOF_3D);
        m_fPickSoundLen = GetSoundLength(SOUND_PICKUP);
      }

      if (m_bDropped || m_bPickupOnce || m_bRespawn) {
        jump CItem::ItemReceived();
      }
    }
    return;
  };

  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    Initialize();     // Initialize base class.
    
    // [SSE] Standart Items Expansion
    if (m_eotOscillation <= 0) {
      StartModelAnim(ITEMHOLDER_ANIM_SMALLOSCILATION, AOF_LOOPING|AOF_NORESTART);
    } else {
      StartModelAnim((m_eotOscillation-1), AOF_LOOPING|AOF_NORESTART);
    }
    
    SetProperties();  // Set properties.

    // [SSE] PowerUps Drop
    if (!m_bDropped) {
      jump CItem::ItemLoop();
    } else if (TRUE) {
      wait() {
        on (EBegin) : {
          SpawnReminder(this, m_fRespawnTime, 0);
          call CItem::ItemLoop();
        }

        on (EReminder) : {
          SendEvent(EEnd()); 
          resume;
        }
      }
    }
  };
};
