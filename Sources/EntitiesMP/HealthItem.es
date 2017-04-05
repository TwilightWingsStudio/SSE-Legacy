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

801
%{
#include "StdH.h"
#include "Models/Items/ItemHolder/ItemHolder.h"
%}

uses "EntitiesMP/Item";

// Health type.
enum HealthItemType {
  0 HIT_PILL      "0 Pill (+1)",        // Pill health.
  1 HIT_SMALL     "1 Small (+10)",      // Small health.
  2 HIT_MEDIUM    "2 Medium (+25)",     // Medium health.
  3 HIT_LARGE     "3 Large (+50)",      // Large health.
  4 HIT_SUPER     "4 Super (+100)",     // Super health.
  5 HIT_MEGA      "5 Mega (+200)",      // Mega health.
  6 HIT_CAPSULE   "6 Capsule (+5)",     // Capsule health.
};

// Event for sending through receive item.
event EHealth {
  FLOAT fHealth,        // Health to receive.
  BOOL bOverTopHealth,  // Can be received over top health?
};

class CHealthItem : CItem {
name      "Health Item";
thumbnail "Thumbnails\\HealthItem.tbn";

properties:
  1 enum HealthItemType m_EhitType    "Type" 'Y' = HIT_SMALL, // Health type.
  2 BOOL m_bOverTopHealth                        = FALSE,     // Can be received over top health?
  3 INDEX m_iSoundComponent = 0,

components:
  0 class   CLASS_BASE        "Classes\\Item.ecl",

// ********* PILL HEALTH *********
  1 model   MODEL_PILL        "Models\\Items\\Health\\Pill\\Pill.mdl",
  2 texture TEXTURE_PILL      "Models\\Items\\Health\\Pill\\Pill.tex",
  3 texture TEXTURE_PILL_BUMP "Models\\Items\\Health\\Pill\\PillBump.tex",
  
  5 model   MODEL_CAPSULE     "Models\\Items\\Health\\Capsule\\Capsule.mdl",
  6 texture TEXTURE_CAPSULE   "Models\\Items\\Health\\Capsule\\Capsule.tex",

// ********* SMALL HEALTH *********
 10 model   MODEL_SMALL       "Models\\Items\\Health\\Small\\Small.mdl",
 11 texture TEXTURE_SMALL     "Models\\Items\\Health\\Small\\Small.tex",

// ********* MEDIUM HEALTH *********
 20 model   MODEL_MEDIUM      "Models\\Items\\Health\\Medium\\Medium.mdl",
 21 texture TEXTURE_MEDIUM    "Models\\Items\\Health\\Medium\\Medium.tex",

// ********* LARGE HEALTH *********
 30 model   MODEL_LARGE       "Models\\Items\\Health\\Large\\Large.mdl",
 31 texture TEXTURE_LARGE     "Models\\Items\\Health\\Large\\Large.tex",

// ********* SUPER HEALTH *********
 40 model   MODEL_SUPER     "Models\\Items\\Health\\Super\\Super.mdl",
 41 texture TEXTURE_SUPER   "Models\\Items\\Health\\Super\\Super.tex",

// ********* MISC *********
 50 texture TEXTURE_SPECULAR_STRONG "Models\\SpecularTextures\\Strong.tex",
 51 texture TEXTURE_SPECULAR_MEDIUM "Models\\SpecularTextures\\Medium.tex",
 52 texture TEXTURE_REFLECTION_LIGHTMETAL01 "Models\\ReflectionTextures\\LightMetal01.tex",
 53 texture TEXTURE_REFLECTION_GOLD01 "Models\\ReflectionTextures\\Gold01.tex",
 54 texture TEXTURE_REFLECTION_PUPLE01 "Models\\ReflectionTextures\\Purple01.tex",
 55 texture TEXTURE_FLARE "Models\\Items\\Flares\\Flare.tex",
 56 model   MODEL_FLARE "Models\\Items\\Flares\\Flare.mdl",

// ************** SOUNDS **************
301 sound   SOUND_PILL         "Sounds\\Items\\HealthPill.wav",
302 sound   SOUND_SMALL        "Sounds\\Items\\HealthSmall.wav",
303 sound   SOUND_MEDIUM       "Sounds\\Items\\HealthMedium.wav",
304 sound   SOUND_LARGE        "Sounds\\Items\\HealthLarge.wav",
305 sound   SOUND_SUPER        "Sounds\\Items\\HealthSuper.wav",

functions:
  // --------------------------------------------------------------------------------------
  // No comments.
  // --------------------------------------------------------------------------------------
  void Precache(void)
  {
    switch (m_EhitType)
    {
      case HIT_PILL:   PrecacheSound(SOUND_PILL  ); break;
      case HIT_SMALL:  PrecacheSound(SOUND_SMALL ); break;                                      
      case HIT_MEDIUM: PrecacheSound(SOUND_MEDIUM); break;
      case HIT_LARGE:  PrecacheSound(SOUND_LARGE ); break;
      case HIT_SUPER:  PrecacheSound(SOUND_SUPER ); break;
    }
  }

  // --------------------------------------------------------------------------------------
  // Fill in entity statistics - for AI purposes only.
  // --------------------------------------------------------------------------------------
  BOOL FillEntityStatistics(EntityStats *pes)
  {
    pes->es_strName = "Health"; 
    pes->es_ctCount = 1;
    pes->es_ctAmmount = m_fValue;
    pes->es_fValue = m_fValue;
    pes->es_iScore = 0;//m_iScore;
    
    switch (m_EhitType)
    {
      case HIT_PILL:  pes->es_strName+=" pill";   break;
      case HIT_SMALL: pes->es_strName+=" small";  break;
      case HIT_MEDIUM:pes->es_strName+=" medium"; break;
      case HIT_LARGE: pes->es_strName+=" large";  break;
      case HIT_SUPER: pes->es_strName+=" super";  break;
    }

    return TRUE;
  }

  // --------------------------------------------------------------------------------------
  // Render particles.
  // --------------------------------------------------------------------------------------
  void RenderParticles(void) {
    // No particles when not existing!
    if (GetRenderType() != CEntity::RT_MODEL) {
      return;
    }
    
    // No particles when in DM modes!
    if (GetSP()->sp_gmGameMode > CSessionProperties::GM_COOPERATIVE || !ShowItemParticles()) {
      return;
    }
    
    BOOL bOnGround = m_eotOscillation == 1;

    switch (m_EhitType)
    {
      case HIT_PILL:
        Particles_Stardust(this, 0.9F * 0.75F * m_fStretch, (bOnGround ? 0.375F : 0.70F*0.75F) * m_fStretch, PT_STAR08, 32);
        break;
      case HIT_SMALL:
        Particles_Stardust(this, 1.0F * 0.75F * m_fStretch, (bOnGround ? 0.375F : 0.75F*0.75F) * m_fStretch, PT_STAR08, 128);
        break;
      case HIT_MEDIUM:
        Particles_Stardust(this, 1.0F * 0.75F * m_fStretch, (bOnGround ? 0.375F : 0.75F*0.75F) * m_fStretch, PT_STAR08, 128);
        break;
      case HIT_LARGE:
        Particles_Stardust(this, 2.0F * 0.75F * m_fStretch, (bOnGround ? 0.75F : 1.0F*0.75F) * m_fStretch, PT_STAR08, 192);
        break;
      case HIT_SUPER:
        Particles_Stardust(this, 2.3F * 0.75F * m_fStretch, (bOnGround ? 0.8625F : 1.5F*0.75F) * m_fStretch, PT_STAR08, 320);
        break;
      // [SSE] More Health Item Types
      case HIT_CAPSULE:
        Particles_Stardust(this, 0.9F * 0.75F * m_fStretch, (bOnGround ? 0.375F : 0.70F*0.75F) * m_fStretch, PT_STAR08, 32);
        break;
    }
  }

  // --------------------------------------------------------------------------------------
  // Set health properties depending on health type.
  // --------------------------------------------------------------------------------------
  void SetProperties(void)
  {
    switch (m_EhitType)
    {
      case HIT_PILL: {
        ForceCollisionBoxIndexChange(ITEMHOLDER_COLLISION_BOX_SMALL);
        m_fValue = 1.0F;
        m_bOverTopHealth = TRUE;
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 10.0F; 
        m_strDescription.PrintF("Pill - H:%g  T:%g", m_fValue, m_fRespawnTime);
        
        // set appearance
        AddItem(MODEL_PILL, TEXTURE_PILL, 0, TEXTURE_SPECULAR_STRONG, TEXTURE_PILL_BUMP);
        // add flare
        AddFlare(MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.2F, 0.0F), FLOAT3D(1.0F, 1.0F, 0.3F) );
        StretchItem(FLOAT3D(1.0F*0.75F, 1.0F*0.75F, 1.0F*0.75F));
        m_iSoundComponent = SOUND_PILL;
      } break;

      case HIT_SMALL: {
        ForceCollisionBoxIndexChange(ITEMHOLDER_COLLISION_BOX_MEDIUM);
        m_fValue = 10.0F;
        m_bOverTopHealth = FALSE;
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 10.0F; 
        m_strDescription.PrintF("Small - H:%g  T:%g", m_fValue, m_fRespawnTime);
        
        // set appearance
        AddItem(MODEL_SMALL, TEXTURE_SMALL, TEXTURE_REFLECTION_LIGHTMETAL01, TEXTURE_SPECULAR_MEDIUM, 0);
        // add flare
        AddFlare(MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.4F, 0.0F), FLOAT3D(2.0F, 2.0F, 0.4F) );
        StretchItem(FLOAT3D(1.0F*0.75F, 1.0F*0.75F, 1.0F*0.75F));
        m_iSoundComponent = SOUND_SMALL;
      } break;

      case HIT_MEDIUM: {
        ForceCollisionBoxIndexChange(ITEMHOLDER_COLLISION_BOX_MEDIUM);
        m_fValue = 25.0F;
        m_bOverTopHealth = FALSE;
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 25.0F; 
        m_strDescription.PrintF("Medium - H:%g  T:%g", m_fValue, m_fRespawnTime);
        
        // set appearance
        AddItem(MODEL_MEDIUM, TEXTURE_MEDIUM, TEXTURE_REFLECTION_LIGHTMETAL01, TEXTURE_SPECULAR_MEDIUM, 0);
        // add flare
        AddFlare(MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.6F, 0.0F), FLOAT3D(2.5F, 2.5F, 0.5F) );
        StretchItem(FLOAT3D(1.5F*0.75F, 1.5F*0.75F, 1.5F*0.75));
        m_iSoundComponent = SOUND_MEDIUM;
      } break;

      case HIT_LARGE: {
        ForceCollisionBoxIndexChange(ITEMHOLDER_COLLISION_BOX_MEDIUM);
        m_fValue = 50.0F;
        m_bOverTopHealth = FALSE;
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 60.0F; 
        m_strDescription.PrintF("Large - H:%g  T:%g", m_fValue, m_fRespawnTime);
        
        // set appearance
        AddItem(MODEL_LARGE, TEXTURE_LARGE, TEXTURE_REFLECTION_GOLD01, TEXTURE_SPECULAR_STRONG, 0);
        // add flare
        AddFlare(MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.8F, 0.0F), FLOAT3D(2.8F, 2.8F, 1.0F) );
        StretchItem(FLOAT3D(1.2F*0.75F, 1.2F*0.75F, 1.2F*0.75F));
        m_iSoundComponent = SOUND_LARGE;
      } break;

      case HIT_SUPER: {
        ForceCollisionBoxIndexChange(ITEMHOLDER_COLLISION_BOX_MEDIUM);
        m_fValue = 100.0F;
        m_bOverTopHealth = TRUE;
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 120.0F; 
        m_strDescription.PrintF("Super - H:%g  T:%g", m_fValue, m_fRespawnTime);
        
        // set appearance
        AddItem(MODEL_SUPER, TEXTURE_SUPER, 0, TEXTURE_SPECULAR_MEDIUM, 0);
        // add flare
        AddFlare(MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 1.0F, 0.0F), FLOAT3D(3.0F, 3.0F, 1.0F) );
        StretchItem(FLOAT3D(1.0F*0.75F, 1.0F*0.75F, 1.0F*0.75));
        CModelObject &mo = GetModelObject()->GetAttachmentModel(ITEMHOLDER_ATTACHMENT_ITEM)->amo_moModelObject;
        mo.PlayAnim(0, AOF_LOOPING);
        m_iSoundComponent = SOUND_SUPER;
      } break;
      
      case HIT_CAPSULE: {
        ForceCollisionBoxIndexChange(ITEMHOLDER_COLLISION_BOX_SMALL);
        m_fValue = 5.0F;
        m_bOverTopHealth = TRUE;
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 10.0F; 
        m_strDescription.PrintF("Capsule - H:%g  T:%g", m_fValue, m_fRespawnTime);
        
        // set appearance
        AddItem(MODEL_CAPSULE, TEXTURE_CAPSULE, 0, TEXTURE_SPECULAR_STRONG, 0);
        // add flare
        AddFlare(MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.2F, 0.0F), FLOAT3D(1.0F, 1.0F, 0.3F) );
        StretchItem(FLOAT3D(1.0F*0.75F, 1.0F*0.75F, 1.0F*0.75F));
        m_iSoundComponent = SOUND_PILL;
      } break;
      
      case HIT_MEGA: {
        ForceCollisionBoxIndexChange(ITEMHOLDER_COLLISION_BOX_MEDIUM);
        m_fValue = 200.0F;
        m_bOverTopHealth = TRUE;
        m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 120.0F; 
        m_strDescription.PrintF("Mega - H:%g  T:%g", m_fValue, m_fRespawnTime);
        
        // set appearance
        AddItem(MODEL_SUPER, TEXTURE_SUPER, 0, TEXTURE_SPECULAR_MEDIUM, 0);
        // add flare
        AddFlare(MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 1.0F, 0.0F), FLOAT3D(3.0F, 3.0F, 1.0F) );
        StretchItem(FLOAT3D(1.0F*0.75F, 1.0F*0.75F, 1.0F*0.75));
        CModelObject &mo = GetModelObject()->GetAttachmentModel(ITEMHOLDER_ATTACHMENT_ITEM)->amo_moModelObject;
        mo.PlayAnim(0, AOF_LOOPING);
        m_iSoundComponent = SOUND_SUPER;
      } break;
    }
  };

  // --------------------------------------------------------------------------------------
  // Change item by session properties.
  // --------------------------------------------------------------------------------------
  void AdjustDifficulty(void)
  {
    if (!GetSP()->sp_bAllowHealth && m_penTarget == NULL) {
      Destroy();
    }
  }

procedures:
  // --------------------------------------------------------------------------------------
  // Called every time when any player trying to pick up item.
  // --------------------------------------------------------------------------------------
  ItemCollected(EPass epass) : CItem::ItemCollected {
    ASSERT(epass.penOther!=NULL);

    // If health stays...
    if (GetSP()->sp_bHealthArmorStays && !(m_bPickupOnce || m_bRespawn))
    {
      BOOL bWasPicked = MarkPickedBy(epass.penOther);

      // If already picked by this player then don't pick again.
      if (bWasPicked) {
        return;
      }
    }

    // Prepare health to send it to entity.
    EHealth eHealth;
    eHealth.fHealth = m_fValue;
    eHealth.bOverTopHealth = m_bOverTopHealth;

    // If health is received...
    if (epass.penOther->ReceiveItem(eHealth)) {
      if (_pNetwork->IsPlayerLocal(epass.penOther))
      {
        switch (m_EhitType)
        {
          case HIT_PILL:  IFeel_PlayEffect("PU_HealthPill"); break;
          case HIT_SMALL: IFeel_PlayEffect("PU_HealthSmall"); break;
          case HIT_MEDIUM:IFeel_PlayEffect("PU_HealthMedium"); break;
          case HIT_LARGE: IFeel_PlayEffect("PU_HealthLarge"); break;
          case HIT_SUPER: IFeel_PlayEffect("PU_HealthSuper"); break;
        }
      }

      // Play the pickup sound.
      m_soPick.Set3DParameters(50.0F, 1.0F, 1.0F, 1.0F);
      PlaySound(m_soPick, m_iSoundComponent, SOF_3D);
      m_fPickSoundLen = GetSoundLength(m_iSoundComponent);

      if (!GetSP()->sp_bHealthArmorStays || (m_bPickupOnce || m_bRespawn)) {
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
    Initialize();     // initialize base class
    
    // [SSE] Standart Items Expansion
    if (m_eotOscillation <= 0) {
      StartModelAnim(ITEMHOLDER_ANIM_SMALLOSCILATION, AOF_LOOPING|AOF_NORESTART);
    } else {
      StartModelAnim((m_eotOscillation-1), AOF_LOOPING|AOF_NORESTART);
    }
    
    SetProperties();  // set properties

    jump CItem::ItemLoop();
  };
};
