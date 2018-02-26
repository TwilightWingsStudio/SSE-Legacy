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

8051
%{
  #include "StdH.h"
  #include "Models/Items/ItemHolder/ItemHolder.h"
  #include "Entities/SoundHolder.h"
%}

uses "Entities/Item";
uses "Entities/KeyItem";

class CGenericKeyItem : CItem {
name      "CGenericKeyItem";
thumbnail "Thumbnails\\GenericKeyItem.tbn";
features  "IsImportant";

properties:
  1 INDEX m_iKeyID "Key ID" = 0,
  2 CTFileName m_fnModel      "Model" 'M'   = CTFILENAME("Models\\Editor\\Axis.mdl")   features(EPROPF_READONLY),
  3 CTFileName m_fnTexture    "Texture" 'T' = CTFILENAME("Models\\Editor\\Vector.tex") features(EPROPF_READONLY),
  4 CEntityPointer m_penSoundPick "Sound Pick Up",
  5 CTStringTrans m_strPickUpMessage "Message" = "Key",
  
 10 FLOAT m_fParticlesSize "Particles Size" = 1.0F,
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
    pes->es_strName.PrintF("CGenericKeyItem[%d]", m_iKeyID);
    pes->es_ctCount = 1;
    pes->es_ctAmmount = 1;
    pes->es_fValue = 1;
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

    Particles_Stardust(this, m_fParticlesSize, (bOnGround ? m_fParticlesSize / 2 : m_fParticlesHeight), PT_STAR08, 64); 
  }

  // --------------------------------------------------------------------------------------
  // Set health properties depending on type.
  // --------------------------------------------------------------------------------------
  void SetProperties(void)
  {
    m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 10.0F;
    m_iKeyID = Clamp(m_iKeyID, INDEX(0), INDEX(31));

    m_strDescription.PrintF("Key %d", m_iKeyID);

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

    // Prepare key to send it to entity.
    EKey eKey;
    eKey.penItem = this;
    eKey.iKeyID = m_iKeyID;
    eKey.bGenericKey = TRUE;

    // If key is received...
    if (epass.penOther->ReceiveItem(eKey))
    {
      if(_pNetwork->IsPlayerLocal(epass.penOther)) {IFeel_PlayEffect("PU_Key");}


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
