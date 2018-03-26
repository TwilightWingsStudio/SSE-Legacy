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

22549
%{
  #include "StdH.h"
%}

// diferent types of testing
enum ERCTestType {
  0 RCTT_NONE           "0 None (brushes only)", // do not test at all
  1 RCTT_SIMPLE         "1 Simple",              // do approximate testing
  2 RCTT_COLLISIONBOX   "2 Collision Box",       // do testing by collision box
  3 RCTT_FULL           "3 Full",                // do full testing
  4 RCTT_FULLSEETHROUGH "4 Full See Through",    // do full testing without entities marked as see through
};

class CRayCaster : CRationalEntity {
name      "Ray Caster";
thumbnail "Thumbnails\\RayCaster.tbn";
features  "HasName", "HasTarget", "IsTargetable";

properties:
   1 CTString m_strName          "Name" 'N' = "Ray Caster",
   2 CTString m_strDescription = "",

   3 BOOL m_bActive               "Active"            = TRUE,
   4 enum ERCTestType m_ercttType "Detection Type" = RCTT_NONE,
   5 CEntityPointer m_penTarget   "Target" 'T' COLOR(C_BROWN|0xFF),

   6 FLOAT m_fRange      "Range"            = 50.0F,
   7 FLOAT m_fTestR      "Ray Radius"       = 0.0F,
   8 FLOAT m_fHitOffset  "Hit Point Offset" = 0.0F,

  10 BOOL m_bAutoCast      "Auto Cast"          = FALSE,
  11 FLOAT m_fCastInterval "Auto Cast Interval" = 0.05f,

  // Variables for data output.
  20 CEntityPointer m_penHit "Hit Target",
  21 FLOAT3D m_vHitPos       "Hit Pos"      = FLOAT3D(0.0F, 0.0F, 0.0F),
  22 FLOAT m_fHitDistance    "Hit Distance" = 0.0F,

components:
   1 model   MODEL_RAYCASTER     "Models\\Editor\\RayCaster.mdl",
   2 texture TEXTURE_RAYCASTER   "Models\\Editor\\RayCaster.tex",

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
    ((CTString&)m_strDescription).PrintF("-><none>");

    if (m_penTarget != NULL) {
      ((CTString&)m_strDescription).PrintF("->%s", m_penTarget->GetName());
    }

    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Returns bytes of memory used by this object.
  // --------------------------------------------------------------------------------------
  SLONG GetUsedMemory(void)
  {
    // initial
    SLONG slUsedMemory = sizeof(CRayCaster) - sizeof(CRationalEntity) + CRationalEntity::GetUsedMemory();
    // add some more
    slUsedMemory += m_strDescription.Length();
    slUsedMemory += m_strName.Length();
    return slUsedMemory;
  }
  
  // --------------------------------------------------------------------------------------
  // Cast the ray.
  // --------------------------------------------------------------------------------------
  void DoCastRay()
  {
    if (!m_bActive) {
      return;
    }

    CCastRay crRay(this, GetPlacement());
    crRay.cr_bHitPortals = FALSE;
    crRay.cr_bHitTranslucentPortals = TRUE;

    // Fix its type.
    if (m_ercttType < RCTT_NONE || m_ercttType > RCTT_FULLSEETHROUGH) {
      m_ercttType = RCTT_NONE;
    }

    // Fix test radius.
    if (m_fTestR < 0) {
      m_fTestR = 0.0F;
    }

    crRay.cr_fTestR = m_fTestR;
    crRay.cr_ttHitModels = (CCastRay::TestType)m_ercttType;
    crRay.cr_bPhysical = FALSE;

    // Perform first cast,
    GetWorld()->CastRay(crRay);  

    INDEX ctCasts = 0;
    while (ctCasts < 10)
    {
      if (crRay.cr_penHit && crRay.cr_fHitDistance < 1.0f && !(crRay.cr_penHit->GetRenderType() != RT_BRUSH)){
        // next casts
        GetWorld()->ContinueCast(crRay);
      } else {
        break;
      }
        
      ctCasts++;
    }

    if (crRay.cr_penHit) {
      m_penHit = crRay.cr_penHit;
      m_vHitPos = crRay.cr_vHit;
      FLOAT fDistance = crRay.cr_fHitDistance;
      m_fHitDistance = fDistance;

      if(crRay.cr_fHitDistance > m_fRange){
        fDistance = m_fRange;
      }

      CPlacement3D plPos = GetPlacement();
      CPlacement3D plNew = CPlacement3D(FLOAT3D(0, 0, m_fHitOffset-fDistance), ANGLE3D(0, 0, 0));
      CPlacement3D plRotation = CPlacement3D(FLOAT3D(0, 0, 0), GetPlacement().pl_OrientationAngle);
      plNew.RelativeToAbsolute(plRotation);
      plPos.pl_PositionVector += plNew.pl_PositionVector;

      if (m_penTarget) {
        m_penTarget->SetPlacement(CPlacement3D(plPos.pl_PositionVector, m_penTarget->GetPlacement().pl_OrientationAngle));
      }

      m_vHitPos = plPos.pl_PositionVector;
    } else {
      m_fHitDistance = m_fRange;

      CPlacement3D plPos = GetPlacement();
      CPlacement3D plNew = CPlacement3D(FLOAT3D(0, 0, m_fHitOffset-m_fRange), ANGLE3D(0, 0, 0));
      CPlacement3D plRotation = CPlacement3D(FLOAT3D(0, 0, 0), GetPlacement().pl_OrientationAngle);
      plNew.RelativeToAbsolute(plRotation);
      plPos.pl_PositionVector += plNew.pl_PositionVector;

      if (m_penTarget) {
        m_penTarget->SetPlacement(CPlacement3D(plPos.pl_PositionVector, m_penTarget->GetPlacement().pl_OrientationAngle));
      }

      m_vHitPos = plPos.pl_PositionVector;
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
    SetModel(MODEL_RAYCASTER);
    SetModelMainTexture(TEXTURE_RAYCASTER);

    // Fix test radius.
    if (m_fTestR < 0) {
      m_fTestR = 0.0F;
    }
  
    autowait(0.1f);

    while (TRUE)
    {
      wait(m_fCastInterval)
      {
        on (EBegin) : { 
          if (m_bAutoCast) {
            DoCastRay();
          }

          resume;
        }

        on(ETrigger) : {
          DoCastRay();
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