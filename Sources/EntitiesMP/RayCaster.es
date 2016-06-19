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

22549
%{
  #include "StdH.h"
%}

class CRayCaster : CRationalEntity {
name      "Ray Caster";
thumbnail "Thumbnails\\RayCaster.tbn";
features  "HasName", "HasTarget", "IsTargetable";

properties:

   1 CTString m_strName          "Name" 'N' = "Ray Caster",
   3 CTString m_strDescription = "",
   2 CEntityPointer m_penTarget  "Target" 'T' COLOR(C_BROWN|0xFF),

   4 BOOL m_bActive  "Active"            = TRUE,
   5 FLOAT m_fRange  "Range"             = 50.0F,
   6 FLOAT3D m_vPos  "Hit Pos"           = FLOAT3D(0.0F, 0.0F, 0.0F),
   7 FLOAT m_fBack   "Distance from Hit" = 0.0F,
   8 BOOL m_bFirst                       = TRUE,

   9 CEntityPointer m_penHit "Hit Target",

components:
   1 model   MODEL_RAYCASTER     "Models\\Editor\\RayCaster.mdl",
   2 texture TEXTURE_RAYCASTER   "Models\\Editor\\RayCaster.tex",

functions:
  const CTString &GetDescription(void) const
  {
    ((CTString&)m_strDescription).PrintF("-><none>");
    if (m_penTarget != NULL) {
      ((CTString&)m_strDescription).PrintF("->%s", m_penTarget->GetName());
    }
    return m_strDescription;
  }

  // returns bytes of memory used by this object
  SLONG GetUsedMemory(void)
  {
    // initial
    SLONG slUsedMemory = sizeof(CRayCaster) - sizeof(CRationalEntity) + CRationalEntity::GetUsedMemory();
    // add some more
    slUsedMemory += m_strDescription.Length();
    slUsedMemory += m_strName.Length();
    return slUsedMemory;
  }

procedures:
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_RAYCASTER);
    SetModelMainTexture(TEXTURE_RAYCASTER);
  
    autowait(0.1f);

    while (TRUE) {
      if(m_bActive && m_penTarget){
        CCastRay crRay(this, GetPlacement());
        crRay.cr_bHitPortals = FALSE;
        crRay.cr_bHitTranslucentPortals = TRUE;
        crRay.cr_ttHitModels = CCastRay::TT_COLLISIONBOX;
        crRay.cr_bPhysical = FALSE;

        INDEX ctCasts = 0;
        while (ctCasts < 10){
          if (ctCasts == 0){
            // perform first cast
            GetWorld()->CastRay(crRay);  
          }
          if (crRay.cr_penHit&&crRay.cr_fHitDistance<1.0f&&!(crRay.cr_penHit->GetRenderType()!= RT_BRUSH)){
            // next casts
            GetWorld()->ContinueCast(crRay);
          } else {
            break;
          }
            
          ctCasts++;
        }

        if (crRay.cr_penHit) {
          m_penHit = crRay.cr_penHit;
          m_vPos = crRay.cr_vHit;
          FLOAT fDistance = crRay.cr_fHitDistance;

          if(crRay.cr_fHitDistance > m_fRange){
            fDistance = m_fRange;
          }
          CPlacement3D plPos = GetPlacement();
          CPlacement3D plNew = CPlacement3D(FLOAT3D(0,0,m_fBack-fDistance),ANGLE3D(0,0,0));
          CPlacement3D plRotation = CPlacement3D(FLOAT3D(0,0,0),GetPlacement().pl_OrientationAngle);
          plNew.RelativeToAbsolute(plRotation);
          plPos.pl_PositionVector+=plNew.pl_PositionVector;
          m_penTarget->SetPlacement(CPlacement3D(plPos.pl_PositionVector,m_penTarget->GetPlacement().pl_OrientationAngle));
          m_vPos=plPos.pl_PositionVector;
        } else { 
          CPlacement3D plPos = GetPlacement();
          CPlacement3D plNew = CPlacement3D(FLOAT3D(0,0,m_fBack-m_fRange),ANGLE3D(0,0,0));
          CPlacement3D plRotation = CPlacement3D(FLOAT3D(0,0,0),GetPlacement().pl_OrientationAngle);
          plNew.RelativeToAbsolute(plRotation);
          plPos.pl_PositionVector += plNew.pl_PositionVector;
          m_penTarget->SetPlacement(CPlacement3D(plPos.pl_PositionVector,m_penTarget->GetPlacement().pl_OrientationAngle));
          m_vPos = plPos.pl_PositionVector;
        }
      }

      wait(_pTimer->TickQuantum) {
        on (EActivate) : {
          m_bActive = TRUE;
          resume;
        }

        on (EDeactivate) : {
          m_bActive = FALSE;
          resume;
        }

        on(ETimer):{
          stop;
        }

        otherwise() : {
          resume;
        };
      };
    } // for while(TRUE)
  }
};