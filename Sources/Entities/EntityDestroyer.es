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

5002
%{
#include "StdH.h"
%}

enum EDType {
  0 EDT_TARGET "Target [0]",
  1 EDT_RANGED "Ranged [1]",
  2 EDT_BOX    "Area [2]"
};

class CEntityDestroyer: CRationalEntity {
name      "Entity Destroyer";
thumbnail "Thumbnails\\EntityDestroyer.tbn";
features  "HasName", "HasTarget", "IsTargetable";

properties:
  1 CTString m_strName          "Name" 'N' = "Entity Destroyer",
  2 CTString m_strDescription = "",

  3 BOOL m_bActive        "Active" 'A' = TRUE,
  4 BOOL m_bDebugMessages "Debug Messages" = FALSE,
  
  5 enum EDType m_edtType      "Type" 'Y' = EDT_TARGET,
  6 CEntityPointer m_penTarget "Entity to Destroy",

  7 RANGE m_fDestroyRange          "Destroy Range" = 1.0f,
  8 FLOATaabbox3D m_boxDestroyArea "Destroy Area" = FLOATaabbox3D(FLOAT3D(0,0,0), FLOAT3D(1,1,1)),

  9 CTString m_strOnlyClass "Only Class" = "",
 10 CTString m_strOnlyName  "Only Name" = "",
 11 BOOL m_bIgnorePenCaused "Don't destroy penCaused" = FALSE,
 12 BOOL m_bIgnoreEditor    "Don't destroy editor entities" = FALSE,

components:
  1 model   MODEL_MARKER     "Models\\Editor\\EntityDestroyer.mdl",
  2 texture TEXTURE_MARKER   "Models\\Editor\\EntityDestroyer.tex",

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
    if (m_edtType == EDT_TARGET) {
      if (m_penTarget == NULL) {
        ((CTString&)m_strDescription).PrintF("-><none>");
      } else {
        ((CTString&)m_strDescription).PrintF("->%s", m_penTarget->GetName());
      }
    } else if (m_edtType == EDT_RANGED) {
      ((CTString&)m_strDescription).PrintF("Range: %f", m_fDestroyRange);
    } else {
      ((CTString&)m_strDescription).PrintF("Box: %f,%f,%f",
        m_boxDestroyArea.Size()(1), m_boxDestroyArea.Size()(2), m_boxDestroyArea.Size()(3));
    }
    return m_strDescription;
  }
  
  // --------------------------------------------------------------------------------------
  // The entity event handler.
  // --------------------------------------------------------------------------------------
  BOOL HandleEvent(const CEntityEvent &ee)
  {
    // Activation/Deactivation
    if (ee.ee_slEvent == EVENTCODE_EActivate) {
      m_bActive = TRUE;
    }
    if (ee.ee_slEvent == EVENTCODE_EDeactivate) {
      m_bActive = FALSE;
    }

    // Entity destroying
    if (ee.ee_slEvent == EVENTCODE_ETrigger) {
      if (m_bActive)
      {
        BOOL bNoClass = m_strOnlyClass == "" ? TRUE : FALSE;
        BOOL bNoName = m_strOnlyName == "" ? TRUE : FALSE;

        INDEX iDeletedEntities = 0;

        switch (m_edtType) {
        // in range
        case EDT_RANGED:
          FOREACHINDYNAMICCONTAINER(GetWorld()->wo_cenEntities, CEntity, iten)
          {
            CEntity *pen = iten;

            if (pen == NULL || pen == this || (m_bIgnorePenCaused && pen == ((ETrigger &)ee).penCaused)) {
              continue;
            }
            
            if ((((pen->en_RenderType == RT_EDITORMODEL || pen->en_RenderType == RT_SKAEDITORMODEL) && m_bIgnoreEditor) && m_bIgnoreEditor)
                || pen->en_RenderType == RT_BRUSH) {
              continue;
            }
      
            CPlacement3D plEntity = pen->GetPlacement();
            plEntity.AbsoluteToRelative(this->GetPlacement());

            if (plEntity.pl_PositionVector.Length() <= ClampDn(m_fDestroyRange, 0.0f)
            && (bNoClass || IsDerivedFromClass(pen, m_strOnlyClass)) && (bNoName || m_strOnlyName == pen->GetName())
            && !pen->IsPlayerEntity())
            {
              pen->Destroy();
              iDeletedEntities++;
            }
          }

          if (m_bDebugMessages) {
            if (iDeletedEntities > 0) {
              CPrintF("[%s]: Destroyed %i entity(es) in range '%f'\n", m_strName, iDeletedEntities, m_fDestroyRange);
            } else {
              CPrintF("[%s]: No entities to destroy in range '%f'\n", m_strName, m_fDestroyRange);
            }
          }
          break;
        // area
        case EDT_BOX:
          FOREACHINDYNAMICCONTAINER(GetWorld()->wo_cenEntities, CEntity, iten)
          {
            CEntity *pen = iten;

            if (pen == NULL || pen == this || (m_bIgnorePenCaused && pen == ((ETrigger &)ee).penCaused)) {
              continue;
            }
            
            if ((((pen->en_RenderType == RT_EDITORMODEL || pen->en_RenderType == RT_SKAEDITORMODEL) && m_bIgnoreEditor) && m_bIgnoreEditor)
                || pen->en_RenderType == RT_BRUSH) {
              continue;
            }
      
            CPlacement3D plEntity = pen->GetPlacement();
            plEntity.AbsoluteToRelative(this->GetPlacement());

            if (plEntity.pl_PositionVector(1) >= m_boxDestroyArea.Min()(1) && plEntity.pl_PositionVector(1) <= m_boxDestroyArea.Max()(1)
            &&  plEntity.pl_PositionVector(2) >= m_boxDestroyArea.Min()(2) && plEntity.pl_PositionVector(2) <= m_boxDestroyArea.Max()(2)
            &&  plEntity.pl_PositionVector(3) >= m_boxDestroyArea.Min()(3) && plEntity.pl_PositionVector(3) <= m_boxDestroyArea.Max()(3)
            && (bNoClass || IsDerivedFromClass(pen, m_strOnlyClass)) && (bNoName || m_strOnlyName == pen->GetName())
            && !pen->IsPlayerEntity())
            {
              pen->Destroy();
              iDeletedEntities++;
            }
          }

          if (m_bDebugMessages) {
            if (iDeletedEntities > 0) {
              CPrintF("[%s]: Destroyed %i entity(es) in the area\n", m_strName, iDeletedEntities);
            } else {
              CPrintF("[%s]: No entities to destroy in the area\n", m_strName);
            }
          }
          break;
        // target
        default: {
          if (m_penTarget == NULL || m_penTarget->GetFlags()&ENF_DELETED) {
            if (m_bDebugMessages) {
              CPrintF("[%s]: No entity to destroy!\n", m_strName);
            }
          } else if (m_penTarget == this && !(GetFlags()&ENF_DELETED)) {
            if (m_bDebugMessages) {
              CPrintF("[%s]: Entity can't destroy itself!\n", m_strName);
            }
          } else {
            CEntity *pen = m_penTarget;
            CTString strDeletedEntity = pen->GetName();

            if (m_bIgnorePenCaused && pen == ((ETrigger &)ee).penCaused) {
              if (m_bDebugMessages) {
                CPrintF("[%s]: Can't destroy penCaused entity!\n", m_strName, strDeletedEntity);
              }
              return CRationalEntity::HandleEvent(ee);
            }

            BOOL bEditorEntity = ((pen->en_RenderType == RT_EDITORMODEL || pen->en_RenderType == RT_SKAEDITORMODEL) && m_bIgnoreEditor);
            
            if ((bNoClass || IsDerivedFromClass(pen, m_strOnlyClass)) && (bNoName || m_strOnlyName == pen->GetName())
            && pen->en_RenderType != RT_BRUSH && !bEditorEntity && !pen->IsPlayerEntity())
            {
              pen->Destroy();

              if (m_bDebugMessages) {
                CPrintF("[%s]: Destroyed '%s' entity\n", m_strName, strDeletedEntity);
              }
            } else {
              if (m_bDebugMessages) {
                CPrintF("[%s]: Can't destroy '%s' due to wrong class/name or it's a brush\n", m_strName, strDeletedEntity);
              }
            }
          }
        }}
      }
    }

    return CRationalEntity::HandleEvent(ee);
  }

procedures:
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_MARKER);
    SetModelMainTexture(TEXTURE_MARKER);
  }
};

