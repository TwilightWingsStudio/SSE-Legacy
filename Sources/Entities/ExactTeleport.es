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
  #include "Entities/Player.h"
  #include "Entities/PlayerWeapons.h"
%}

enum EExTTargetType {
  0 EETTT_TARGET      "0 Target",
  1 EETTT_THIS        "1 This Entity",
  2 EETTT_PENCAUSED   "2 penCaused",
  3 EETTT_PENTARGET   "3 penTarget (targetted)",
};

class CExactTeleport: CRationalEntity {
name      "CExactTeleport";
thumbnail "Thumbnails\\ExactTeleport.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'      = "Exact Teleport",
   3 CTString m_strDescription = "",
   
   4 BOOL m_bActive              "Active" 'A' = TRUE,
   5 BOOL m_bDebugMessages "Debug Messages" = FALSE,

   6 CEntityPointer m_penEnityToTP   "Entity to Teleport" 'E',
   7 CEntityPointer m_penDestination "Destination Target" 'T',

  10 enum EExTTargetType m_eEntityType      "EtT Type" = EETTT_TARGET,
  11 enum EExTTargetType m_eDestinationType "Destination Type" = EETTT_TARGET,
   
  25 BOOL m_bForceStop              "Force Stop" 'F' = FALSE,
  26 BOOL m_bKeepOrientation        "Keep Orientation" = FALSE,
  27 BOOL m_bTelefrag               "Telefrag" 'F' = TRUE,

  30 BOOL m_bSpawnEffect "Spawn Effect" 'X' = FALSE,

components:
  1 model   MODEL_TELEPORTER   "Models\\Editor\\ExactTeleport.mdl",
  2 texture TEXTURE_TELEPORTER "Models\\Editor\\ExactTeleport.tex",
  3 class   CLASS_BASIC_EFFECT  "Classes\\BasicEffect.ecl",

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
  void DoTeleportation(CEntity *penCaused, CEntity *penTarget)
  {
    CEntity *penEntityToTP = m_penEnityToTP;
    CEntity *penDestination = m_penDestination;

    // Extended entity to teleport types.
    switch (m_eEntityType)
    {
      case EETTT_PENCAUSED: {
        if (penCaused) {
          penEntityToTP = penCaused;
        }
      } break;
      
      case EETTT_PENTARGET: {
        if (penTarget) {
          penEntityToTP = penTarget;
        }
      } break;
      
      default: break;
    }

    // If no any entity to teleport then stop.
    if (penEntityToTP == NULL) {
      if (m_bDebugMessages) {
        CPrintF(TRANS("[%s] : Haven't any entity to teleport!\n"), m_strName);
      }

      return;
    }
    
    // Extended teleport destination types.
    switch (m_eDestinationType)
    {
      case EETTT_PENCAUSED: {
        if (penCaused) {
          penDestination = penCaused;
        }
      } break;
      
      case EETTT_PENTARGET: {
        if (penTarget) {
          penDestination = penTarget;
        }
      } break;

      case EETTT_THIS: {
        penDestination = this;
      } break;

      default: break;
    }

    // If no destination where teleport then stop.
    if (penDestination == NULL) {
      if (m_bDebugMessages) {
        CPrintF(TRANS("[%s] : Haven't destination to teleport!\n"), m_strName);
      }

      return;
    }

    if (m_bDebugMessages) {
      if (penDestination == this) {
        CPrintF(TRANS("[%s] : Teleporting %s to %s location.\n"), m_strName, penEntityToTP->GetName().Undecorated(), penDestination->GetName().Undecorated());
      } else {
        CPrintF(TRANS("[%s] : Teleporting %s to this teleporter.\n"), m_strName, penEntityToTP->GetName().Undecorated());
      }
    }

    CPlacement3D pl = penDestination->GetPlacement();
    
    // If we need to kepp orientation of victim.
    if (m_bKeepOrientation) {
      pl.pl_OrientationAngle = penEntityToTP->GetPlacement().pl_OrientationAngle;
    }

    // Teleport back.
    penEntityToTP->Teleport(pl, m_bTelefrag);

    // If victim is movable and forcestop enabled then apply it.
    if (m_bForceStop && (penEntityToTP->GetPhysicsFlags()&EPF_MOVABLE) ) {
      ((CMovableEntity*)&*penEntityToTP)->ForceFullStop();
    }

    // If spawn effect enabled then spawn it.
    if (m_bSpawnEffect)
    {
      ESpawnEffect ese;
      ese.colMuliplier = C_WHITE|CT_OPAQUE;
      ese.betType = BET_TELEPORT;
      ese.vNormal = FLOAT3D(0,1,0);
      FLOATaabbox3D box;
      penEntityToTP->GetBoundingBox(box);
      FLOAT fEntitySize = box.Size().MaxNorm()*2;
      ese.vStretch = FLOAT3D(fEntitySize, fEntitySize, fEntitySize);

      CEntityPointer penEffect = CreateEntity(pl, CLASS_BASIC_EFFECT);
      penEffect->Initialize(ese);
    }
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
    SetModel(MODEL_TELEPORTER);
    SetModelMainTexture(TEXTURE_TELEPORTER);
    
    if (m_eEntityType == EETTT_THIS) {
      m_eEntityType = EETTT_TARGET;
    }

    autowait(0.1f);
  
    wait()
    {
      on (EBegin) : { 
        resume;
      }

      on(ETrigger eTrigger) :
      {
        if (m_bActive) {
          DoTeleportation(eTrigger.penCaused, NULL);
        }

        resume;
      }
      
      // [SSE] Entities - Targeted Event
      on (ETargeted eTargeted) : {
        if (m_bActive) {
          DoTeleportation(eTargeted.penCaused, eTargeted.penTarget);
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

