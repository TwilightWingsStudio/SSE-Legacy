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

21365
%{
  #include "StdH.h"
  #include "Entities/Bullet.h"
  #include "Entities/SoundHolder.h"
%}

enum EBSUsePenCausedAs {
  0 EBSUP_NONE        "0 None",
  1 EBSUP_INFLICTOR   "1 Inflictor",
  2 EBSUP_DESTINATION "2 Target",
};

class CBulletShooter : CRationalEntity {
name      "BulletShooter";
thumbnail "Thumbnails\\BulletShooter.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName    "Name"   = "Bullet Shooter",
   2 BOOL m_bActive        "Active" = TRUE,
   
   6 FLOAT m_fBulletDamage "Bullet Damage" = 10.0F,
   7 FLOAT m_fJitter       "Bullet Jitter" = 0.0F,
   8 FLOAT m_fBulletSize   "Bullet Size" = 0.1F,
   9 FLOAT m_fRange        "Bullet Range"  = 200.0F,
  10 BOOL  m_bBulletTrail  "Bullet Trail"  = FALSE,
  
  15 CEntityPointer m_penInflictor "Inflictor",
  16 CEntityPointer m_penTarget    "Target",

  17 enum TETargetType m_teInflictorType "Inflictor Type" = TETT_TARGET,
  18 enum TETargetType m_teTargetType    "Target Type"    = TETT_TARGET,
  
  20 FLOAT m_fWait      "Min Fire Delay" = 0.1F,
  21 FLOAT m_tmFired = 0.0F,
  
  50 CEntityPointer m_penSoundFire  "Sound Fire",
  51 CSoundObject m_soFire,

  {
    CAutoPrecacheSound m_aps;
  }
  
components:
  1 class  CLASS_BULLET      "Classes\\Bullet.ecl",

  6 model   MODEL_BULLETSHOOTER   "Models\\Editor\\BulletShooter.mdl",
  7 texture TEXTURE_BULLETSHOOTER "Models\\Editor\\BulletShooter.tex",
  
functions:

  // --------------------------------------------------------------------------------------
  // Precache entity components.
  // --------------------------------------------------------------------------------------
  void Precache(void)
  {
    PrecacheClass(CLASS_BULLET);
  }

  // --------------------------------------------------------------------------------------
  // Apply mirror and stretch to the entity.
  // --------------------------------------------------------------------------------------
  void MirrorAndStretch(FLOAT fStretch, BOOL bMirrorX) {}
  
  // --------------------------------------------------------------------------------------
  // Fires the bullet in given direction with given properties.
  // --------------------------------------------------------------------------------------
  void ShootBullet(CEntity *penCaused, CEntity *penTargetArg, const CPlacement3D &pl)
  {
    CEntity *penInflictor = m_penInflictor;
    CEntity *penTarget = m_penTarget;
    
    CEntityPointer penBullet = CreateEntity(pl, CLASS_BULLET);

    // Initialize the bullet.
    EBulletInit eInit;
    
    // Extended inflictor types.
    switch (m_teInflictorType)
    {
      case TETT_THIS: {
        penInflictor = this;
      } break;
      
      case TETT_PENCAUSED: {
        penInflictor = penCaused;
      } break;
      
      case TETT_PENTARGET: {
        penInflictor = penTargetArg;
      } break;
    }

    // Extended target types.
    switch (m_teTargetType)
    {
      case TETT_PENCAUSED: {
        penTarget = penCaused;
      } break;
      
      case TETT_PENTARGET: {
        penTarget = penTargetArg;
      } break;
    }

    if (penInflictor == NULL) {
      penInflictor = this;
    }

    eInit.penOwner = penInflictor;
    eInit.fDamage = m_fBulletDamage;
    penBullet->Initialize(eInit);

    if (penTarget) {
      ((CBullet&)*penBullet).CalcTarget(penTarget, m_fRange);      
    } else {
      ((CBullet&)*penBullet).CalcTarget(m_fRange);
    }

    ((CBullet&)*penBullet).m_fBulletSize = m_fBulletSize;
    ((CBullet&)*penBullet).CalcJitterTarget(m_fJitter);
    ((CBullet&)*penBullet).LaunchBullet(TRUE, m_bBulletTrail, TRUE);
    ((CBullet&)*penBullet).DestroyBullet();
  }

  // --------------------------------------------------------------------------------------
  // Calls firing method and plays sound if any sound selected.
  // --------------------------------------------------------------------------------------
  void DoShoot(CEntity *penCaused, CEntity *penTarget)
  {
    ShootBullet(penCaused, penTarget, GetPlacement());

    // If we have SoundHolder selected then shooting sound.
    if (m_penSoundFire != NULL)
    {
      CSoundHolder &sh = (CSoundHolder&)*m_penSoundFire;
      m_soFire.Set3DParameters(FLOAT(sh.m_rFallOffRange), FLOAT(sh.m_rHotSpotRange), sh.m_fVolume, sh.m_fPitch);
      PlaySound(m_soFire, sh.m_fnSound, sh.m_iPlayType);
    }
  }

procedures:

/************************************************************
 *                M  A  I  N    L  O  O  P                  *
 ************************************************************/
  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main(EVoid)
  {
    // Validate Sound.
    if (m_penSoundFire != NULL && !IsDerivedFromClass(m_penSoundFire, "SoundHolder")) {
      m_penSoundFire = NULL;
      WarningMessage("Only SoundHolder can be selected as ""Sound Fire"" for BulletShooter!");
    }
    
    // Validate Target Type.
    if (m_teTargetType == TETT_THIS) {
      m_teTargetType = TETT_TARGET;
    }

    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // Set appearance.
    SetModel(MODEL_BULLETSHOOTER);
    SetModelMainTexture(TEXTURE_BULLETSHOOTER);

    // Spawn in world editor.
    autowait(0.1f);
    
    wait()
    {
      on (EBegin) : {
        resume;
      };
    
      on (ETrigger eTrigger) :
      {
        if (m_bActive && m_tmFired + m_fWait < _pTimer->CurrentTick()) {
          m_tmFired = _pTimer->CurrentTick();
          DoShoot(eTrigger.penCaused, NULL);
        }
    
        resume;
      }
      
      // [SSE] Entities - Targeted Event
      on (ETargeted eTargeted) :
      {
        if (m_bActive && m_tmFired + m_fWait < _pTimer->CurrentTick()) {
          m_tmFired = _pTimer->CurrentTick();
          DoShoot(eTargeted.penCaused, eTargeted.penTarget);
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

      otherwise() : { resume; };
    }

    // Cease to exist.
    Destroy();

    return;
  }
};
