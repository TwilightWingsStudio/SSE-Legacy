/* Copyright (c) 2018 by ZCaliptium.

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

8072
%{
  #include "StdH.h"
  #include "Models/Items/ItemHolder/ItemHolder.h"
  #include "Entities/SoundHolder.h"
%}

uses "Entities/Item";
uses "Entities/AmmoItem";

class CGenericAmmoItem : CItem {
name      "CGenericAmmoItem";
thumbnail "Thumbnails\\GenericAmmoItem.tbn";
features  "IsImportant";

properties:
  1 enum AmmoItemType  m_EaitType    "Type" 'Y' = AIT_SHELLS,     // health type
  2 CTFileName m_fnModel      "Model" 'M'   = CTFILENAME("Models\\Editor\\Axis.mdl")   features(EPROPF_READONLY),
  3 CTFileName m_fnTexture    "Texture" 'T' = CTFILENAME("Models\\Editor\\Vector.tex") features(EPROPF_READONLY),
  4 CEntityPointer m_penSoundPick    "Sound Pick Up",

  6 INDEX m_iAmount                  "Amount"  = 1,

 10 FLOAT m_fParticlesSize   "Particles Size" = 1.0F,
 11 FLOAT m_fParticlesHeight "Particles Height" = 1.0F,

components:
  0 class   CLASS_BASE        "Classes\\Item.ecl",
  
// ************** FLARE FOR EFFECT **************
100 texture TEXTURE_FLARE  "Models\\Items\\Flares\\Flare.tex",
101 model   MODEL_FLARE    "Models\\Items\\Flares\\Flare.mdl",

functions:
  // --------------------------------------------------------------------------------------
  // No comments.
  // --------------------------------------------------------------------------------------
  void Precache(void) {}

  // --------------------------------------------------------------------------------------
  // Fill in entity statistics - for AI purposes only.
  // --------------------------------------------------------------------------------------
  BOOL FillEntityStatistics(EntityStats *pes)
  {
    pes->es_strName = "CGenericAmmoItem ";
    
    pes->es_ctCount = 1;
    pes->es_ctAmmount = m_iAmount;
    pes->es_fValue = 0;
    pes->es_iScore = 0;//m_iScore;
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

    Particles_Spiral(this, 2.0F * 0.75F * m_fStretch, (bOnGround ? 0.75F : 1.25F*0.75F) * m_fStretch, PT_STAR04, 8); //
  }

  // --------------------------------------------------------------------------------------
  // Set health properties depending on type.
  // --------------------------------------------------------------------------------------
  void SetProperties(void)
  {
    m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 30.0F;

    if (m_fnModel == "") {
      m_fnModel=CTFILENAME("Models\\Editor\\Axis.mdl");
    }
    
    CAttachmentModelObject *pamo = GetModelObject()->AddAttachmentModel(ITEMHOLDER_ATTACHMENT_ITEM);
    pamo->amo_moModelObject.SetData_t(m_fnModel);
    pamo->amo_moModelObject.AutoSetAttachments();
    
    try {
      pamo->amo_moModelObject.mo_toTexture.SetData_t(m_fnTexture);
    } catch (char *strError) {
      WarningMessage(strError);
    }

    // add flare
    AddFlare(MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.2F, 0.0F), FLOAT3D(1.0F, 1.0F, 0.3F) );
    StretchItem(FLOAT3D(1.0F, 1.0F, 1.0F));
  };

procedures:
  // --------------------------------------------------------------------------------------
  // Called every time when any player trying to pick up item.
  // --------------------------------------------------------------------------------------
  ItemCollected(EPass epass) : CItem::ItemCollected
  {
    ASSERT(epass.penOther != NULL);

    // Prepare ammo to send it to entity.
    EAmmoItem eAmmo;
    eAmmo.EaitType = m_EaitType;
    eAmmo.iQuantity = (INDEX) m_iAmount;

    // If health is received...
    if (epass.penOther->ReceiveItem(eAmmo))
    {
      if(_pNetwork->IsPlayerLocal(epass.penOther)) {IFeel_PlayEffect("PU_Ammo");}

      // If we have SoundHolder selected then play pick up sound.
      if (m_penSoundPick != NULL)
      {
        CSoundHolder &sh = (CSoundHolder&)*m_penSoundPick;
        m_soPick.Set3DParameters(FLOAT(sh.m_rFallOffRange), FLOAT(sh.m_rHotSpotRange), sh.m_fVolume, sh.m_fPitch);
        PlaySound(m_soPick, sh.m_fnSound, sh.m_iPlayType);
        m_fPickSoundLen = 2.0F; // TODO: Maybe change it in future.
      }

      jump CItem::ItemReceived();
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
      StartModelAnim((m_eotOscillation - 1), AOF_LOOPING|AOF_NORESTART);
    }

    ForceCollisionBoxIndexChange(ITEMHOLDER_COLLISION_BOX_BIG);
    SetProperties(); // Set properties.

    jump CItem::ItemLoop();
  };
};
