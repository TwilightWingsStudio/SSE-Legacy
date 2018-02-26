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

229
%{
#include "StdH.h"
%}

class CRangeDamager: CRationalEntity {
name      "RangeDamager";
thumbnail "Thumbnails\\RangeDamager.tbn";
features  "HasName", "IsTargetable";

properties:
  1 CTString m_strName          "Name" 'N' = "Range Damager",
  2 CTString m_strDescription = "",

  3 BOOL m_bActive            "Active" 'A' = TRUE,
  4 BOOL m_bDebugMessages "Debug Messages" = FALSE,
  
  5 enum DamageType m_dmtType "Type" 'Y' = DMT_DAMAGER,
  6 FLOAT m_fAmmount          "Amount" 'C' = 1000.0f,

  7 CEntityPointer m_penInflictor "Inflictor" 'I' COLOR(C_CYAN|0xFF),
  8 CEntityPointer m_penSource "Source" 'S' COLOR(C_RED|0xFF),
  
 10 RANGE m_rFallOffRange    "Fall-off" 'F' = 8.0f,
 11 RANGE m_rHotSpotRange    "Hot-spot" 'H' = 0.0f,
 
 20 BOOL m_bDamageFromPenCaused "Inflictor=penCaused" = FALSE,
 21 BOOL m_bDamageFromThis      "Inflictor=this" = FALSE,
 22 BOOL m_bSourceIsPenCaused   "Source=penCaused" = TRUE,

components:
  1 model   MODEL_TELEPORT     "Models\\Editor\\RangeDamager.mdl",
  2 texture TEXTURE_TELEPORT   "Models\\Editor\\RangeDamager.tex",

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
    ((CTString&)m_strDescription).PrintF("%s:%g", DamageType_enum.NameForValue(INDEX(m_dmtType)), m_fAmmount);
    return m_strDescription;
  }
  
  void DealDamage(const ETrigger &eTrigger)
  {
    CEntity *penCaused = eTrigger.penCaused;
    CEntity *penInflictor = m_penInflictor;

    if (m_bDebugMessages) {
      CPrintF("[RD] %s : Damaging with Type=%s; Amount=%f;\n  HotSpot=%f; FallOff=%f;\n", GetName(), DamageType_enum.NameForValue(INDEX(m_dmtType)), m_fAmmount, m_rHotSpotRange, m_rFallOffRange);
    }

    if (m_bDamageFromThis) {
      penInflictor = this;

      if (m_bDebugMessages) {
        CPrintF("  penInflictor=this;\n", GetName());
      }
    }

    if (m_bDamageFromPenCaused) {
      penInflictor = penCaused;

      if (m_bDebugMessages) {
        CPrintF("  penInflictor=penCaused;\n", GetName());
      }
    }

    CEntity *penSource = this;

    if (m_penSource != NULL) {
      penSource = m_penSource;

      if (m_bDebugMessages) {
        CPrintF("  penSource=m_penSource;\n");
      }
    }

    if (m_bSourceIsPenCaused && penCaused != NULL) {
      penSource = penCaused;
 
      if (m_bDebugMessages) {
        CPrintF("  penSource=penCaused;\n");
      }
    }

    if (m_bDebugMessages) {
      if (penSource == this) {
        CPrintF("  penSource=this;\n");
      }
    }

    FLOAT3D vPosition = penSource->GetPlacement().pl_PositionVector;

    if (m_bDebugMessages) {
      CPrintF("  penSource.Position(X=%f, Y=%f, Z=%f)\n", vPosition(1), vPosition(2), vPosition(3));
    }

    InflictRangeDamage(penInflictor, m_dmtType, m_fAmmount, vPosition, m_rHotSpotRange, m_rFallOffRange);

    if (m_bDebugMessages) {
      CPrintF("  done!\n", GetName());
    }
  }

procedures:
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_TELEPORT);
    SetModelMainTexture(TEXTURE_TELEPORT);

    // wait for someone to trigger you and then damage it
    wait() {
      on (EActivate) : {
        m_bActive = TRUE;
        resume;
      }

      on (EDeactivate) : {
        m_bActive = FALSE;
        resume;
      }

      on (ETrigger eTrigger) : {
        if (m_bActive) {
          DealDamage(eTrigger);
        }

        resume;
      }

      otherwise() : {
        resume;
      };
    }
  }
};

