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

20503
%{
  #include "StdH.h"
  #include "EntitiesMP/MovingBrush.h"
  #include "EntitiesMP/MovingBrushMarker.h"
  #include "EntitiesMP/MovableBrush.h"
%}

uses "EntitiesMP/MovableModel";

class CMBTeleporter: CRationalEntity {
name      "MBTeleporter";
thumbnail "Thumbnails\\MBTeleporter.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName         "Name" 'N' = "MB Teleporter",         // class name

   3 CEntityPointer m_penMB     "Moving/Movable Brush/Model" 'T' COLOR(C_RED|0xFF),                 // send event to entity
   4 CEntityPointer m_penMarker "Moving Brush Marker" 'Y' COLOR(C_RED|0xFF),
  
components:
   1 model   MODEL_MBTELEPORTER   "Models\\Editor\\MovingBrushTeleporter.mdl",
   2 texture TEXTURE_MBTELEPORTER "Models\\Editor\\MovingBrushTeleporter.tex",

functions:                                        
  // returns bytes of memory used by this object
  SLONG GetUsedMemory(void)
  {
    // initial
    SLONG slUsedMemory = sizeof(CMBTeleporter) - sizeof(CRationalEntity) + CRationalEntity::GetUsedMemory();
    // add some more

    slUsedMemory += m_strName.Length();

    return slUsedMemory;
  }

procedures:
  Reset() {
    if (!m_penMB || !m_penMarker || (!IsOfClass(m_penMB, "Movable Brush") && !IsOfClass(m_penMB, "Moving Brush") && !IsOfClass(m_penMB, "MovingBrush") && !IsOfClass(m_penMB,"Movable Model")) || !IsOfClass(m_penMarker, "Moving Brush Marker")) {
      WarningMessage("MB Teleporter: No Targets selected or Targets of wrong class");
      return EReturn();
    }

    if (IsOfClass(m_penMB,"Moving Brush")) {
      CMovingBrush mb = (CMovingBrush&)*m_penMB;

      mb.MovingOn();
      mb.en_plLastPlacement=m_penMarker->GetPlacement();
      mb.ForceFullStop();
      mb.m_soStart.Stop();
      mb.m_soStop.Stop();
      mb.m_soFollow.Stop();
      mb.m_bMoveToMarker = FALSE;
      mb.MovingOff();
      mb.m_penTarget = ((CMovingBrushMarker&)*m_penMarker).m_penTarget;
      mb.m_bMoving = FALSE;
      mb.m_bValidMarker = TRUE;
      //mb.m_bForceStop = TRUE;
      //mb.m_bStopMoving = TRUE;
      //mb.SendEvent(EEnd());
    
    } else if (IsOfClass(m_penMB, "Movable Brush")) {
      CMovableBrush mb = (CMovableBrush&)*m_penMB;
      
      mb.m_fTransX = mb.m_fbTransX;
      mb.m_fTransY = mb.m_fbTransY;
      mb.m_fTransZ = mb.m_fbTransZ;

      mb.m_fRotH = mb.m_fbRotH;
      mb.m_fRotP = mb.m_fbRotP;
      mb.m_fRotB = mb.m_fbRotB;

    } else if (IsOfClass(m_penMB, "Movable Model")) {
      CMovableModel mm = (CMovableModel&)*m_penMB;

      mm.m_fTransX = mm.m_fbTransX;
      mm.m_fTransY = mm.m_fbTransY;
      mm.m_fTransZ = mm.m_fbTransZ;

      mm.m_fRotH = mm.m_fbRotH;
      mm.m_fRotP = mm.m_fbRotP;
      mm.m_fRotB = mm.m_fbRotB;
    }
    
    m_penMB->Teleport(m_penMarker->GetPlacement(), FALSE);
    return;
  }

  Main() {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_MBTELEPORTER);
    SetModelMainTexture(TEXTURE_MBTELEPORTER);

    // spawn in world editor
    autowait(0.1F);

    while (TRUE) {
      wait(_pTimer->TickQuantum) {
        on (EBegin) : {
          resume;
        }
    
        on (ETrigger) : {
          call Reset();
          resume;
        }
    
        on (ETimer) : {
          stop;
        }
      }
    }

    return;
  };
};
