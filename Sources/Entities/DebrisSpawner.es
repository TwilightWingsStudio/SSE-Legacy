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

5001
%{
  #include "StdH.h"
%}

uses "Entities/Debris";
uses "Entities/ModelHolder2";

class CDebrisSpawner : CEntity {
name      "Debris Spawner";
thumbnail "Thumbnails\\DebrisSpawner.tbn";
features  "HasName", "IsTargetable";

properties:

  1 CTString m_strName          "Name" 'N' = "Debris Spawner",
  2 CTString m_strDescription = "",

  3 BOOL m_bActive              "Active" = TRUE,
  4 BOOL m_bDebugMessages       "Debug Messages" = FALSE,
  5 INDEX m_iCount              "Count" = 1,

  6 RANGE m_rXSpawn             "RND X Spawn" = 0,
  7 RANGE m_rYSpawn             "RND Y Spawn" = 0,
  8 RANGE m_rZSpawn             "RND Z Spawn" = 0,

  9 FLOAT m_fThrowStrength      "Throw Strength" = 0,
 10 FLOAT m_fThrowAngle         "Throw Angle" = 0,

 11 FLOAT m_fSize               "Size" = 1,
 12 FLOAT m_fRNDSizeFactor      "RND Size Factor" = 0,

 13 CEntityPointer m_penTarget              "Debris Model",
 14 enum EntityInfoBodyType m_eibtType      "Debris Type" = EIBT_ROCK,
 15 enum DebrisParticlesType m_dptParticles "Debris Particles" = DPT_NONE,
 16 enum BasicEffectType m_betEffect        "Debris Stain" = BET_NONE,

 17 BOOL m_bDebrisImmaterial    "Immaterial ASAP" = FALSE,
 18 FLOAT m_fDustStretch        "Dust Fall Stretch" = 0,
 19 ANGLE3D m_aRotation         "Rotation Speed" = ANGLE3D(180, 180, 180)

components:

  1 model   MODEL_MARKER     "Models\\Editor\\DebrisSpawner.mdl",
  2 texture TEXTURE_MARKER   "Models\\Editor\\DebrisSpawner.tex",

  3 class CLASS_DEBRIS       "Classes\\Debris.ecl"

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
  // Precaches debris effects
  // --------------------------------------------------------------------------------------
  void Precache(void)
  {
    PrecacheClass(CLASS_DEBRIS);
  }

  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const
  {
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Returns bytes of memory used by this object
  // --------------------------------------------------------------------------------------
  SLONG GetUsedMemory(void)
  {
    // initial
    SLONG slUsedMemory = sizeof(CDebrisSpawner) - sizeof(CEntity) + CEntity::GetUsedMemory();
    // add some more
    slUsedMemory += m_strName.Length();
    slUsedMemory += m_strDescription.Length();
    return slUsedMemory;
  }

  // --------------------------------------------------------------------------------------
  // Debris model
  // --------------------------------------------------------------------------------------
  class CModelHolder2 *GetModelHolder2(void)
  {
    return (CModelHolder2*)&*m_penTarget;
  }
  
  // --------------------------------------------------------------------------------------
  // Spawn debrises!
  // --------------------------------------------------------------------------------------
  void DoSpawn()
  {
    if (!m_bActive) {
      return;
    }

    // If missing debris MH2.
    if (m_penTarget == NULL || m_penTarget->GetFlags()&ENF_DELETED)
    {
      if (m_bDebugMessages) {
        CPrintF("[%s] : Unable to get debris model due to empty target/deleted ModelHolder2!\n", m_strName);
      }

      return;
    }

    // Not a ModelHolder2
    if (!IsDerivedFromClass(m_penTarget, "ModelHolder2"))
    {
      if (m_bDebugMessages) {
        CPrintF("[%s] : Unable to get debris model from a non-ModelHolder2 entity!\n", m_strName);
      }

      return;
    }

    // Spawn debris
    if (m_bDebugMessages) {
      CPrintF("[%s] : Spawning debris...\n", m_strName);
    }

    // If count is wrong.
    if (m_iCount <= 0) {
      if (m_bDebugMessages) {
        CPrintF("  Debris count is less or equals to 0\n");
      }

      return;
    }

    INDEX iSpawnedDebris = 0;

    for(INDEX iDebris = 0; iDebris < m_iCount; iDebris++)
    {
      ANGLE3D angLaunch = ANGLE3D(FRnd()*m_fThrowAngle-m_fThrowAngle/2,
                                  FRnd()*m_fThrowAngle-m_fThrowAngle/2,
                                  FRnd()*m_fThrowAngle-m_fThrowAngle/2);
      FLOAT3D vLaunchDir;
      AnglesToDirectionVector(GetPlacement().pl_OrientationAngle+angLaunch, vLaunchDir);
      vLaunchDir = vLaunchDir*m_fThrowStrength;

      FLOAT fDebrisSize = Clamp(m_fSize, 0.01f, 100.0f)+(FRnd()*2-1)*m_fRNDSizeFactor;

      Debris_Begin(m_eibtType, m_dptParticles, m_betEffect, 1.0f, vLaunchDir, FLOAT3D(0, 0, 0), 0.0f, 0.0f);
      
      // Stretch.
      FLOAT3D vStretch(GetModelHolder2()->m_fStretchX * GetModelHolder2()->m_fStretchAll * fDebrisSize,
                       GetModelHolder2()->m_fStretchY * GetModelHolder2()->m_fStretchAll * fDebrisSize,
                       GetModelHolder2()->m_fStretchZ * GetModelHolder2()->m_fStretchAll * fDebrisSize);

      // Spawn placement.
      CPlacement3D plSpawn;
      plSpawn = CPlacement3D(GetPlacement().pl_PositionVector + FLOAT3D(FRnd()*m_rXSpawn, FRnd()*m_rYSpawn, FRnd()*m_rZSpawn), GetPlacement().pl_OrientationAngle);

      if (Debris_Spawn_Template(m_eibtType, m_dptParticles, m_betEffect,
          GetModelHolder2(), this, GetModelHolder2(), vStretch, 1.0f, plSpawn, vLaunchDir,
          // Rotation speed
          ANGLE3D(FRnd()*m_aRotation(1)-m_aRotation(1)/2,
                  FRnd()*m_aRotation(2)-m_aRotation(2)/2,
                  FRnd()*m_aRotation(3)-m_aRotation(3)/2),
          m_bDebrisImmaterial, Clamp(m_fDustStretch, 0.0f, 100.0f), C_WHITE|CT_OPAQUE)) {
        iSpawnedDebris++; // Count each piece
      }
    }

    if (iSpawnedDebris <= 0) {
      if (m_bDebugMessages) {
        CPrintF("  Unable to spawn debris\n", m_iCount);
      }
    } else {
      if (m_bDebugMessages) {
        CPrintF("  Spawned %i debris\n", iSpawnedDebris);
      }
    }
  }

  // --------------------------------------------------------------------------------------
  // The entity event handler.
  // --------------------------------------------------------------------------------------
  BOOL HandleEvent(const CEntityEvent &ee)
  {
    if (ee.ee_slEvent == EVENTCODE_ETrigger || ee.ee_slEvent == EVENTCODE_EStart) {
      DoSpawn();
    }

    return CEntity::HandleEvent(ee);
  }

procedures:
  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    m_iCount = ClampDn(m_iCount, INDEX(0));
    
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_MARKER);
    SetModelMainTexture(TEXTURE_MARKER);

    return;
  }
};
