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

807
%{
#include "StdH.h"
#include "Models/Items/ItemHolder/ItemHolder.h"
%}

uses "EntitiesMP/Item";

// Event for sending through receive item.
event EMessageItem {
  CTFileName fnmMessage,
};

%{
%}

class CMessageItem : CItem {
name      "MessageItem";
thumbnail "Thumbnails\\MessageItem.tbn";

properties:
  1 CTString m_strName          "Name" 'N' = "MessageItem",
  2 CTString m_strDescription = "",
  3 CTFileName m_fnmMessage  "Message" 'M' = CTString(""),
  4 INDEX m_iSoundComponent = 0,

components:
  0 class   CLASS_BASE        "Classes\\Item.ecl",

  1 model   MODEL_PERGAMENT      "Models\\Items\\Pergament\\Pergament.mdl",
  2 texture TEXTURE_PERGAMENT    "Models\\Items\\Pergament\\Pergament.tex",

  // ********* MISC *********
255 texture TEXTURE_FLARE       "Models\\Items\\Flares\\Flare.tex",
256 model   MODEL_FLARE         "Models\\Items\\Flares\\Flare.mdl",

// ************** SOUNDS **************
301 sound   SOUND_KEY         "Sounds\\Items\\Key.wav",

functions:
  // --------------------------------------------------------------------------------------
  // No comments.
  // --------------------------------------------------------------------------------------
  void Precache(void) {
    PrecacheSound(SOUND_KEY);
  }

  // --------------------------------------------------------------------------------------
  // Fill in entity statistics - for AI purposes only.
  // --------------------------------------------------------------------------------------
  BOOL FillEntityStatistics(EntityStats *pes)
  {
    pes->es_strName = "Pergament";
    pes->es_ctCount = 1;
    pes->es_ctAmmount = 1;
    pes->es_fValue = 1;
    pes->es_iScore = 0;//m_iScore;

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

    Particles_Stardust(this, 0.9F, m_eotOscillation == 1 ? 0.5F : 0.70F, PT_STAR08, 32);
  }

  // --------------------------------------------------------------------------------------
  // Set item properties.
  // --------------------------------------------------------------------------------------
  void SetProperties(void)
  {
    m_fRespawnTime = (m_fCustomRespawnTime > 0.0F) ? m_fCustomRespawnTime : 10.0F; 
    m_strDescription = m_fnmMessage.FileName();

    // set appearance
    AddItem(MODEL_PERGAMENT, TEXTURE_PERGAMENT, 0, 0, 0);

    // add flare
    AddFlare(MODEL_FLARE, TEXTURE_FLARE, FLOAT3D(0.0F, 0.2f, 0.0F), FLOAT3D(1.0F, 1.0F, 0.3F) );
    StretchItem(FLOAT3D(1.0F, 1.0F, 1.0F));
    m_iSoundComponent = SOUND_KEY;
  };

procedures:
  // --------------------------------------------------------------------------------------
  // Called every time when any player trying to pick up item.
  // --------------------------------------------------------------------------------------
  ItemCollected(EPass epass) : CItem::ItemCollected
  {
    ASSERT(epass.penOther != NULL);

    // send key to entity
    EMessageItem eMessage;
    eMessage.fnmMessage = m_fnmMessage;

    // if health is received
    if (epass.penOther->ReceiveItem(eMessage)) {
      // play the pickup sound
      m_soPick.Set3DParameters(50.0F, 1.0F, 1.0F, 1.0F);
      PlaySound(m_soPick, m_iSoundComponent, SOF_3D);
      m_fPickSoundLen = GetSoundLength(m_iSoundComponent);
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
      StartModelAnim((m_eotOscillation-1), AOF_LOOPING|AOF_NORESTART);
    }

    ForceCollisionBoxIndexChange(ITEMHOLDER_COLLISION_BOX_SMALL);
    SetProperties();  // set properties

    jump CItem::ItemLoop();
  };
};
