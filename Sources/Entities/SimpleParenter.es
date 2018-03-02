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

523
%{
  #include "StdH.h"
%}

enum ESPUsePenCausedAs
{
  0 ESP_NONE   "0 None",
  1 ESP_ENTITY "1 Entity to Parent",
  2 ESP_PARENT "2 Parent to",
};

class CSimpleParenter : CRationalEntity {
name      "Simple Parenter";
thumbnail "Thumbnails\\SimpleParenter.tbn";
features  "HasName", "IsTargetable";

properties:
  1 CTString m_strName          "Name" 'N' = "Simple Parenter",
  2 CTString m_strDescription = "",
  3 BOOL m_bActive              "Active" 'V' = TRUE,
  4 BOOL m_bDebugMessages       "Debug Messages" = FALSE, 
  
  5 BOOL m_bTeleport            "Teleport Entity" = TRUE,
  6 BOOL m_bCopy                "Copy Entity"     = FALSE,
  7 BOOL m_bAbsoluteOffset      "Absolute Offset" = FALSE,
  8 BOOL m_bRotate              "Adjust Rotation" = TRUE,
  9 CEntityPointer m_penTarget  "Entity to Parent" 'T',
 10 CEntityPointer m_penParent  "Parent to Entity" 'Y',
 
 20 FLOAT m_fOffsetX "X Offset" = 0.0F,
 21 FLOAT m_fOffsetY "Y Offset" = 0.0F,
 22 FLOAT m_fOffsetZ "Z Offset" = 0.0F,

 23 FLOAT m_fOffsetH "Heading Offset" = 0.0F,
 24 FLOAT m_fOffsetP "Pitch Offset" = 0.0F,
 25 FLOAT m_fOffsetB "Banking Offset" = 0.0F,
 
 30 enum ESPUsePenCausedAs m_eUsePenCausedAs "Use penCaused as ..." = ESP_NONE,

components:

  1 model   MODEL_SIMPLEPARENTER    "Models\\Editor\\SimpleParenter.mdl",
  2 texture TEXTURE_SIMPLEPARENTER  "Models\\Editor\\SimpleParenter.tex"
  
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
    CTString strTarget = "<none>";
    CTString strParent = "<none>";

    if (m_penTarget != NULL){
      strTarget = m_penTarget->GetName();
    }

    if (m_penParent != NULL) {
      strParent = m_penParent->GetName();
    }

    ((CTString&)m_strDescription).PrintF("%s->%s", strTarget, strParent);
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Parents entity to target.
  // --------------------------------------------------------------------------------------
  void DoParent(const ETrigger &eTrigger)
  {
    CEntityPointer penParent = m_penParent;
    CEntityPointer penTarget = m_penTarget;

    switch (m_eUsePenCausedAs)
    {
      case ESP_ENTITY: {
        penTarget = eTrigger.penCaused;
      } break;

      case ESP_PARENT: {
        penParent = eTrigger.penCaused;
      } break;
    }

    if (!penTarget || !penParent)
    {
      if (m_bDebugMessages) {
        CPrintF("[SP] %s : unnable to parent! Invalid target or entity to parent!\n", GetName());
      }

      return;
    }

    CPlacement3D plParent = penParent->GetPlacement();
    CPlacement3D plEntity = penTarget->GetPlacement();
    
    if (m_bRotate) {
      plEntity.pl_OrientationAngle = plParent.pl_OrientationAngle + ANGLE3D(m_fOffsetH, m_fOffsetP, m_fOffsetB);
    }
    
    if (m_bTeleport)
    {
      plEntity.pl_PositionVector = plParent.pl_PositionVector;

      if (m_bAbsoluteOffset) {
        plEntity.pl_PositionVector += FLOAT3D(m_fOffsetX, m_fOffsetY, m_fOffsetZ);
      } else {
        CPlacement3D plOffset = CPlacement3D(FLOAT3D(m_fOffsetX, m_fOffsetY, m_fOffsetZ), ANGLE3D(0, 0, 0));
        CPlacement3D plRotation = CPlacement3D(FLOAT3D(0.0F, 0.0F, 0.0F), plParent.pl_OrientationAngle);
        plOffset.RelativeToAbsolute(plRotation);
        plEntity.pl_PositionVector += plOffset.pl_PositionVector;
      }
    }
    
    if (m_bCopy)
    {
      penTarget = GetWorld()->CopyEntityInWorld(*penTarget,
        CPlacement3D(FLOAT3D(-32000.0F+FRnd()*200.0F, -32000.0F+FRnd()*200.0F, 0), ANGLE3D(0, 0, 0)));
    }
    
    penTarget->Teleport(plEntity, FALSE);
    penTarget->SetParent(penParent);
    
    if (m_bDebugMessages) {
      if (m_bCopy) {
        CPrintF("[SP] %s : Successfully parented copy of entity!\n", GetName());
      } else {
        CPrintF("[SP] %s : Successfully parented!\n", GetName());
      }
    }
  }
  
  // --------------------------------------------------------------------------------------
  // Unparents entity from target.
  // --------------------------------------------------------------------------------------
  void DoUnparent(const EStart &eStart)
  {
    CEntityPointer penUnparentFrom = eStart.penCaused;

    // unparent everything from m_penTarget (ESP_ENTITY), m_penParent (ESP_PARENT) or penCaused (ESP_NONE)
    switch (m_eUsePenCausedAs)
    {
      case ESP_ENTITY: {
        penUnparentFrom = m_penTarget;
      } break;

      case ESP_PARENT: {
        penUnparentFrom = m_penParent;
      } break;
    }

    if (!penUnparentFrom) { return; }

    // unparent everything from the entity
    {FOREACHINDYNAMICCONTAINER(GetWorld()->wo_cenEntities, CEntity, iten) {
      CEntity *pen = iten;
      if (pen) {
        if (pen->GetParent() == penUnparentFrom) {
          pen->SetParent(NULL);
        }
      }
    }}
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
    SetModel(MODEL_SIMPLEPARENTER);
    SetModelMainTexture(TEXTURE_SIMPLEPARENTER);

    wait()
    {
      on( EStart eStart) :
      {
        if (!m_bActive) {
          resume;
        }
        
        DoUnparent(eStart);

        resume;
      }

      on (ETrigger eTrigger) :
      {
        if (!m_bActive) {
          resume;
        }
        
        DoParent(eTrigger);

        resume;
      }
      
      on (EActivate) :
      {
        if (m_bDebugMessages) {
          CPrintF("[SP] %s : Activated!\n", GetName());
        }
        
        m_bActive = TRUE;
        resume;
      }
      
      on (EDeactivate) :
      {
        if (m_bDebugMessages) {
          CPrintF("[SP] %s : Deactivated!\n", GetName());
        }
      
        m_bActive = FALSE;
        resume;
      }
    }

    return;
  }
};
