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

3443
%{
  #include "StdH.h"
%}

enum EExDTargetType {
  0 EEDTT_TARGET      "1 Target",
  1 EEDTT_PENCAUSED   "2 penCaused",
  2 EEDTT_PENTARGET   "3 penTarget (targetted)",
};

class CExactDamager : CRationalEntity {
name      "CExactDamager";
thumbnail "Thumbnails\\ExactDamager.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'      = "Exact Damager",
   3 CTString m_strDescription = "",

   4 BOOL m_bActive        "Active" 'A'     = TRUE,
   5 BOOL m_bDebugMessages "Debug Messages" = FALSE,

   6 enum DamageType m_dmtType "Damage Type" = DMT_ABYSS,
   7 FLOAT m_fAmount           "Amount"      = 1000.0f,

   8 CEntityPointer m_penInflictor "Inflictor Target" 'Y',
   9 CEntityPointer m_penVictim    "Victim Target" 'E',
   
  10 enum EExDTargetType m_eInflictorType "Inflictor Type" = EEDTT_TARGET,
  11 enum EExDTargetType m_eVictimType    "Victim Type"    = EEDTT_TARGET,

components:
  1 model   MODEL_EXACTDAMAGER   "Models\\Editor\\ExactDamager.mdl",
  2 texture TEXTURE_EXACTDAMAGER "Models\\Editor\\ExactDamager.tex",

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
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Teleports selected entity to destination if possible.
  // --------------------------------------------------------------------------------------
  void DoDamage(CEntity *penCaused, CEntity *penTarget)
  {
    CEntity *penVictim = m_penVictim;
    CEntity *penInflictor = m_penInflictor;

    // Extended victim types.
    switch (m_eVictimType)
    {
      case EEDTT_PENCAUSED: {
        if (penCaused) {
          penVictim = penCaused;
        }
      } break;
      
      case EEDTT_PENTARGET: {
        if (penTarget) {
          penVictim = penTarget;
        }
      } break;
      
      default: break;
    }

    // If no any entity to damage then stop.
    if (penVictim == NULL) {
      if (m_bDebugMessages) {
        CPrintF(TRANS("[%s] : Haven't any entity to damage!\n"), m_strName);
      }

      return;
    }

    // Extended inflictor types.
    switch (m_eInflictorType)
    {
      case EEDTT_PENCAUSED: {
        if (penCaused) {
          penInflictor = penCaused;
        }
      } break;

      case EEDTT_PENTARGET: {
        if (penTarget) {
          penInflictor = penTarget;
        }
      } break;

      default: break;
    }

    // And finally deal damage to target.
    InflictDirectDamage(penVictim, penInflictor,  m_dmtType, m_fAmount, 
      penVictim->GetPlacement().pl_PositionVector, FLOAT3D(0, 1, 0));
  }
  
procedures:
  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_EXACTDAMAGER);
    SetModelMainTexture(TEXTURE_EXACTDAMAGER);
  
    autowait(0.1f);
  
    wait()
    {
      on (EBegin) : { 
        resume;
      }

      on(ETrigger eTrigger) :
      {
        if (m_bActive) {
          DoDamage(eTrigger.penCaused, NULL);
        }

        resume;
      }
      
      // [SSE] Entities - Targeted Event
      on (ETargeted eTargeted) : {
        if (m_bActive) {
          DoDamage(eTargeted.penCaused, eTargeted.penTarget);
        }

        resume;
      }

      on (EActivate) : {
        m_bActive = TRUE;
        resume;
      }

      on (EDeactivate) : {
        m_bActive = FALSE;
        resume;
      }
    }
  }
};

