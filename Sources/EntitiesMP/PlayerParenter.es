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

101
%{
  #include "StdH.h"
%}

class CPlayerParenter : CRationalEntity {
name      "Player Parenter";
thumbnail "Thumbnails\\PlayerParenter.tbn";
features  "HasName", "IsTargetable";

properties:
  1 CTString m_strName "Name" 'N'          = "Player Parenter",

  2 FLOAT m_fTransX "X Offset" = 0.0F,
  3 FLOAT m_fTransY "Y Offset" = 0.0F,
  4 FLOAT m_fTransZ "Z Offset" = 0.0F,
  
  5 BOOL       m_bTeleport     "Teleport to player" = TRUE,
  6 BOOL       m_bCopy         "Copy Entity"        = FALSE,
  7 CEntityPointer m_penTarget "Entity to Parent",
  8 BOOL       m_bAbsolute     "Absolute Offset"    = FALSE,
  9 BOOL       m_bRotate       "Adjust Rotation"    = TRUE,
  
  10 FLOAT m_fRotX  "Heading Offset" = 0.0F,
  11 FLOAT m_fRotY  "Pitch Offset"   = 0.0F,
  12 FLOAT m_fRotZ  "Banking Offset" = 0.0F,
  
  13 BOOL m_bParentPlayerToEntity  "Parent Player to Entity" = FALSE,

components:
  1 model   MODEL_MARKER     "Models\\Editor\\PlayerParenter.mdl",
  2 texture TEXTURE_MARKER   "Models\\Editor\\PlayerParenter.tex"
  
functions:

procedures:
  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    // declare yourself as a brush
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);
    SetModel(MODEL_MARKER);
    SetModelMainTexture(TEXTURE_MARKER);

    while (TRUE)
    {
      wait() {
        on (EBegin) : {
          resume;
        }
        
        on (EStart eStart) :
        {
          if (eStart.penCaused == NULL) {
            resume;
          }
          
          if (!m_bParentPlayerToEntity) {
            //remove all parented stuff from player
            {FOREACHINDYNAMICCONTAINER(GetWorld()->wo_cenEntities, CEntity, iten) {
              CEntity *pen = iten;
              if (pen) {
                if (pen->GetParent() == eStart.penCaused) {
                  pen->SetParent(NULL);
                }
              }
            }}
          } else {
            if (eStart.penCaused != NULL) {
              eStart.penCaused->SetParent(NULL);
            }
          }

          resume;
        }

        on (ETrigger eTrigger) :
        {
          if (!m_penTarget) { resume;}

          CEntityPointer pen = m_penTarget;
          CEntityPointer penCaused = eTrigger.penCaused;
          CPlacement3D plWanted = pen->GetPlacement();
          CPlacement3D plPlayer = penCaused->GetPlacement();
    
          if (m_bParentPlayerToEntity) {
            CPlacement3D plTmp = plWanted;
            plWanted = plPlayer;
            plPlayer = plTmp;
            penCaused = m_penTarget;
            pen=eTrigger.penCaused;
          }
          
          if (m_bRotate) {
            plWanted.pl_OrientationAngle = plPlayer.pl_OrientationAngle;
            plWanted.pl_OrientationAngle(1) += m_fRotX;
            plWanted.pl_OrientationAngle(2) += m_fRotY;
            plWanted.pl_OrientationAngle(3) += m_fRotZ;
          }
          
          if (m_bTeleport) {
            plWanted.pl_PositionVector = plPlayer.pl_PositionVector;
            if (m_bAbsolute) {
              plWanted.pl_PositionVector(1) += m_fTransX;
              plWanted.pl_PositionVector(2) += m_fTransY;
              plWanted.pl_PositionVector(3) += m_fTransZ;
            } else {
              CPlacement3D plOffset = CPlacement3D(FLOAT3D(m_fTransX, m_fTransY,m_fTransZ), ANGLE3D(0, 0, 0));
              CPlacement3D plRotation = CPlacement3D(FLOAT3D(0.0F, 0.0F, 0.0F), plPlayer.pl_OrientationAngle);
              plOffset.RelativeToAbsolute(plRotation);
              plWanted.pl_PositionVector += plOffset.pl_PositionVector;
            }
          }
          
          if (m_bCopy) {
            pen = GetWorld()->CopyEntityInWorld(*pen,
              CPlacement3D(FLOAT3D(-32000.0F+FRnd()*200.0F, -32000.0F+FRnd()*200.0F, 0), ANGLE3D(0, 0, 0)));
          }
          
          pen->Teleport(plWanted, FALSE);
          pen->SetParent(penCaused);
          
          resume;
        }

        on (ETimer) : { stop; }
      }
    }

    return;
  }
};
