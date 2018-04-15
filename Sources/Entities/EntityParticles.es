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

500
%{
#include "StdH.h"
%}

enum EntityParticlesType {
  1 EPT_ROMBOIDTRAIL        "Romboid Trail [01]",
  2 EPT_BOMBTRAIL           "Bomb Trail [02]",
  3 EPT_FIRECRACKERTRAIL    "Firecracker Trail [03]",
  4 EPT_SPIRALTRAIL         "Spiral Trail [04]",
  5 EPT_COLOREDSTARSTRAIL   "Colored Stars Trail [05]",
  6 EPT_WHITELINETRAIL      "White Line Trail [06]",
  7 EPT_FIREBALLTRAIL       "Fireball Trail [07]",
  8 EPT_GRENADETRAIL        "Grenade Trail [08]",
  9 EPT_CANNONBALL          "Cannon Ball [09]",
 10 EPT_LAVATRAIL           "Lava Trail [10]",
 11 EPT_LAVABOMBTRAIL       "Lava Bomb Trail [11]",
 12 EPT_ROCKETTRAIL         "Rocket Trail [12]",
 13 EPT_BLOODTRAIL          "Blood Trail [13]",
 14 EPT_GHOSTBUSTER         "Ghostbuster [14]",
 15 EPT_BURNING             "Burning [15]",
 16 EPT_REGENERATION        "Regeneration [16]",
 17 EPT_DEATH               "Death [17]",
 18 EPT_APPEARING           "Appearing [18]",
 19 EPT_BEASTTRAIL          "Beast Projectile Trail [19]",
 20 EPT_BEASTBIGTRAIL       "Beast Big Projectile Trail [20]",
 21 EPT_BEASTDEBRISTRAIL    "Beast Projectile Debris Trail [21]",
 22 EPT_AFTERBURNER         "After Burner [22]",
 23 EPT_ROCKETMOTORBURNING  "Rocket Motor Burning [23]",
 24 EPT_LIGHTNING           "Lightning [24]",
 25 EPT_RUNNINGDUST         "Running Dust [25]",
 26 EPT_AIRELEMENTAL        "Air Elemental [26]",
 27 EPT_EXOTECHLASER        "Exotech Larva Laser [27]",
 28 EPT_SNIPERRESIDUE       "Sniper Residue [28]",
 29 EPT_GLOWINGSWIRL        "Growing Swirl [29]",
 30 EPT_SUMMONERDISAPPEAR   "Summoner Disappear [30]",
 31 EPT_AIRELEMENTALCOMP    "Air Elemental Comp [31]",
 32 EPT_METEORTRAIL         "Meteor Trail [32]",
 33 EPT_LARVAENERGY         "Larva Energy [33]",
 34 EPT_BURNINGCOMP         "Burning Comp [34]",
 35 EPT_MODELGLOW           "Model Glow [35]",
 36 EPT_RUNAFTERBURNER      "Run After Burner [36]",
};

class CEntityParticles: CRationalEntity {
name      "Entity Particles";
thumbnail "Thumbnails\\EntityParticles.tbn";
features  "HasName", "HasTarget", "IsTargetable";

properties:

  1 CTString m_strName          "Name" 'N' = "Entity Particles",
  2 CTString m_strDescription = "",
  3 CEntityPointer m_penTarget  "Target" 'T' COLOR(C_dGREEN|0xFF),

  4 enum ParticleTexture m_ptTexture   "Texture" = PT_STAR01,
  5 FLOAT m_fSize                      "Size" = 1.0f,
  6 COLOR m_colColor                   "Color" = COLOR(C_WHITE|0xFF),
  7 enum EntityParticlesType m_eptType "Type" = EPT_ROMBOIDTRAIL,

  8 BOOL m_bInfiniteRender  "Infinite Render" = TRUE, // if FALSE, then several particles will use "m_fStartTime"
  9 FLOAT m_fStartTime      "Time Start" = 0.0f, // Trigger Event sets "m_fStartTime" to "_pTimer->CurrentTick()"
 10 FLOAT m_fAddTime        "Time Add" = 0.0f, // how much the particles should be visible (depends on the type)

 11 BOOL m_bActive              "Active" = TRUE,
 12 BOOL m_bRenderOnlyOnTarget  "Render only on Target" = FALSE, // don't render particles if m_penTarget == NULL

 20 FLOAT m_fParameter1   "Param FLOAT 1" = 0.0f,
 21 FLOAT m_fParameter2   "Param FLOAT 2" = 0.0f,
 22 INDEX m_iParameter    "Param INDEX" = 0,
 23 FLOAT3D m_vParameters "Param FLOAT3D" = FLOAT3D(0, 0, 0),

components:

  1 model   MODEL_MARKER     "Models\\Editor\\Axis.mdl",
  2 texture TEXTURE_MARKER   "Models\\Editor\\Vector.tex"


functions:
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
  // Renders particles for this entity.
  // --------------------------------------------------------------------------------------
  void RenderParticles(void)
  {
    if (!m_bActive) {
      return;
    }

    // don't render particles if there's no target entity
    if ((!m_penTarget || m_penTarget->GetFlags()&ENF_DELETED) && m_bRenderOnlyOnTarget) {
      return;
    }

    if (m_fSize < 0.01f) {
      m_fSize = 0.01f;
    }

    CEntity *pen = this;
    FLOAT tmNow = m_fStartTime;
    FLOAT tmAdd = m_fStartTime + m_fAddTime;

	  if (m_penTarget != NULL && !(m_penTarget->GetFlags()&ENF_DELETED)) {
      pen = m_penTarget;
    }

    if (m_bInfiniteRender) {
      tmNow = _pTimer->CurrentTick();
      tmAdd = _pTimer->CurrentTick() + m_fAddTime;
    }

    FLOAT3D vSrc = GetLerpedPlacement().pl_PositionVector;
    FLOAT3D vDst = pen->GetLerpedPlacement().pl_PositionVector;

    switch (m_eptType)
    {
      case EPT_ROMBOIDTRAIL: Particles_RomboidTrail(pen); break;
      case EPT_BOMBTRAIL: Particles_BombTrail(pen); break;
      case EPT_FIRECRACKERTRAIL: Particles_FirecrackerTrail(pen); break;
      case EPT_SPIRALTRAIL: Particles_SpiralTrail(pen); break;
      case EPT_COLOREDSTARSTRAIL: Particles_ColoredStarsTrail(pen); break;
      case EPT_WHITELINETRAIL: Particles_WhiteLineTrail(pen); break;
      case EPT_FIREBALLTRAIL: Particles_Fireball01Trail(pen); break;
      case EPT_GRENADETRAIL: Particles_GrenadeTrail(pen, m_colColor); break;
      case EPT_CANNONBALL: Particles_CannonBall(pen, m_fParameter1); break;
      case EPT_LAVATRAIL: Particles_LavaTrail(pen); break;
      case EPT_LAVABOMBTRAIL: Particles_LavaBombTrail(pen, m_fSize); break;
      case EPT_ROCKETTRAIL: Particles_RocketTrail(pen, m_fSize, m_colColor); break;
      case EPT_BLOODTRAIL: Particles_BloodTrail(pen); break;
      case EPT_GHOSTBUSTER:
        if (vSrc != vDst) {
          Particles_Ghostbuster(vSrc, vDst, ClampDn(m_iParameter, INDEX(1)), m_fSize);
        }
        break;
      case EPT_BURNING: Particles_Burning(pen, m_fSize, tmAdd); break;
      case EPT_REGENERATION: Particles_Regeneration(pen, tmNow, tmAdd, m_fParameter1, Clamp(m_iParameter, INDEX(0), INDEX(1))); break;
      case EPT_DEATH: Particles_Death(pen, tmAdd); break;
      case EPT_APPEARING: Particles_Appearing(pen, tmAdd); break;
      case EPT_BEASTTRAIL: Particles_BeastProjectileTrail(pen, m_fSize, m_fParameter1, ClampDn(m_iParameter, INDEX(1))); break;
      case EPT_BEASTBIGTRAIL: Particles_BeastBigProjectileTrail(pen, m_fSize, m_fParameter2, m_fParameter1, ClampDn(m_iParameter, INDEX(1))); break;
      case EPT_BEASTDEBRISTRAIL: Particles_BeastProjectileDebrisTrail(pen, m_fSize); break;
      case EPT_AFTERBURNER: Particles_AfterBurner(pen, tmAdd, m_fSize, Clamp(m_iParameter, INDEX(0), INDEX(2))); break;
      case EPT_ROCKETMOTORBURNING: Particles_RocketMotorBurning(pen, tmAdd, m_vParameters, m_fSize, FLOAT(m_iParameter)); break;
      case EPT_LIGHTNING:
        if (vSrc != vDst) {
          Particles_Lightning(vSrc, vDst, tmAdd);
        }
        break;
      case EPT_RUNNINGDUST: Particles_RunningDust(pen); break;
      case EPT_AIRELEMENTAL: Particles_AirElemental(pen, m_fSize, m_fParameter1, tmAdd, m_colColor); break;
      case EPT_EXOTECHLASER:
        if (vSrc != vDst) {
          Particles_ExotechLarvaLaser(pen, vSrc, vDst);
        }
        break;
      case EPT_SNIPERRESIDUE:
        if (vSrc != vDst) {
          Particles_SniperResidue(pen, vSrc, vDst);
        }
        break;
      case EPT_GLOWINGSWIRL: Particles_GrowingSwirl(pen, m_fSize, tmAdd); break;
      case EPT_SUMMONERDISAPPEAR: Particles_SummonerDisappear(pen, tmAdd); break;
      case EPT_AIRELEMENTALCOMP: Particles_AirElemental_Comp(pen->GetModelObject(), m_fSize, m_fParameter1, GetLerpedPlacement()); break;
      case EPT_METEORTRAIL: Particles_MeteorTrail(pen, m_fSize, m_fParameter1, m_vParameters); break;
      case EPT_LARVAENERGY: Particles_LarvaEnergy(pen, m_vParameters); break;
      case EPT_BURNINGCOMP: Particles_Burning_Comp(pen->GetModelObject(), m_fSize, GetLerpedPlacement()); break;
      case EPT_MODELGLOW:
        Particles_ModelGlow2(pen->GetModelObject(), GetLerpedPlacement(), tmAdd, m_ptTexture, m_fSize, FLOAT(ClampDn(m_iParameter, INDEX(1))),
          m_fParameter1, m_colColor);
        break;
      case EPT_RUNAFTERBURNER: Particles_RunAfterBurner(pen, tmAdd, m_fSize, Clamp(m_iParameter, INDEX(0), INDEX(2))); break;
    }
  };

  // --------------------------------------------------------------------------------------
  // Returns bytes of memory used by this object.
  // --------------------------------------------------------------------------------------
  SLONG GetUsedMemory(void)
  {
    // Initial.
    SLONG slUsedMemory = sizeof(CEntityParticles) - sizeof(CRationalEntity) + CRationalEntity::GetUsedMemory();

    // Add some more.
    slUsedMemory += m_strName.Length();
    slUsedMemory += m_strDescription.Length();

    return slUsedMemory;
  }

procedures:
  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    // Disallow negative and 0 sizes.
    if (m_fSize < 0.01f) {
      m_fSize = 0.01f;
    }

    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_MARKER);
    SetModelMainTexture(TEXTURE_MARKER);

    // Spawn in world editor.
    autowait(0.1f);

    wait()
    {
      on (EBegin) : {
        resume;
      };

      // reset render time
      on (ETrigger) :
      {
        m_fStartTime = _pTimer->CurrentTick();
        resume;
      }

      // change target
      on (ETargeted eTargeted) :
      {
        m_penTarget = eTargeted.penTarget;
        resume;
      }

      on (EActivate) :{
        m_bActive = TRUE;
        resume;
      }

      on (EDeactivate) :{
        m_bActive = FALSE;
        resume;
      }

      otherwise() : { resume; };
    }

    // Cease to exist.
    Destroy();

    return;
  }
};

