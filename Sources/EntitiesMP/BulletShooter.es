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

21365
%{
  #include "StdH.h"
  #include "EntitiesMP/Bullet.h"
%}

class CBulletShooter : CRationalEntity {
name      "BulletShooter";
thumbnail "Thumbnails\\BulletShooter.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName    "Name"   = "Bullet Shooter",
   2 BOOL m_bActive        "Active" = TRUE,
   
   6 FLOAT m_fBulletDamage "Bullet Damage" = 10.0F,
   7 FLOAT m_fJitter       "Bullet Jitter" = 0.0F,
   8 FLOAT m_fBulletSize   "Bullet Size"   = 0.1F,
   9 FLOAT m_fRange        "Bullet Range"  = 200.0F,
  10 BOOL m_bTrail         "Bullet Trail"  = FALSE,
  
  15 CEntityPointer m_penIgnore "Ignore Entity",

  20 FLOAT m_fWait      "Min Fire Delay" = 0.1F,
  21 FLOAT m_tmFired = 0.0F,
  
  50 CTFileName m_fnSound  "Sound File" = "",
  51 CSoundObject m_soFire,
  
  52 RANGE m_rFallOffRange "Sound Fall-off" 'F'   = 100.0F,
  53 RANGE m_rHotSpotRange "Sound Hot-spot" 'H'   = 50.0F,
  54 FLOAT m_fSoundVolume  "Sound Volume"   'V'   = 1.0F,
  54 FLOAT m_fSoundPitch   "Sound Pitch"          = 1.0F,
  55 BOOL m_bSurround      "Sound Surround" 'R'   = FALSE,
  56 BOOL m_bVolumetric    "Sound Volumetric" 'O' = TRUE,

  {
    CAutoPrecacheSound m_aps;
  }
  
components:
  1 class  CLASS_BULLET      "Classes\\Bullet.ecl",

  6 model   MODEL_BSHOOTER   "Models\\Editor\\BulletShooter.mdl",
  7 texture TEXTURE_BSHOOTER "Models\\Editor\\BulletShooter.tex",
  
functions:

  void Precache(void) {
    m_aps.Precache(m_fnSound);
  }

  // apply mirror and stretch to the entity
  void MirrorAndStretch(FLOAT fStretch, BOOL bMirrorX) {
    // stretch its ranges
    m_rFallOffRange *= fStretch;
    m_rHotSpotRange *= fStretch;
  }

  // Fire the bullet in given direction with given properties.
  void Shoot(const CPlacement3D &pl)
  {
    CEntityPointer penBullet = CreateEntity(pl, CLASS_BULLET);
    // Initialize the bullet.
    EBulletInit eInit;
    if(m_penIgnore) {
      eInit.penOwner = m_penIgnore;
    } else {
      eInit.penOwner = this;
    }
  
    eInit.fDamage = m_fBulletDamage;
    penBullet->Initialize(eInit);
    ((CBullet&)*penBullet).CalcTarget(m_fRange);
    ((CBullet&)*penBullet).m_fBulletSize = m_fBulletSize;
    ((CBullet&)*penBullet).CalcJitterTarget(m_fJitter);
    ((CBullet&)*penBullet).LaunchBullet(TRUE, m_bTrail, TRUE);
    ((CBullet&)*penBullet).DestroyBullet();

    // Sound stuff.
    if (m_fSoundVolume > 0.0F && m_fnSound != "") {
      INDEX iPlayType = SOF_3D;
      if (m_bSurround) { iPlayType |= SOF_SURROUND; }
      if (m_bVolumetric) { iPlayType |= SOF_VOLUMETRIC; } 
      m_soFire.Set3DParameters(FLOAT(m_rFallOffRange), FLOAT(m_rHotSpotRange), m_fSoundVolume, m_fSoundPitch);
      PlaySound(m_soFire, m_fnSound, iPlayType);
    }
  }
procedures:

/************************************************************
 *                M  A  I  N    L  O  O  P                  *
 ************************************************************/
  Main(EVoid) {
    // validate range
    if (m_rHotSpotRange < 0.0f) { m_rHotSpotRange = 0.0f; }
    if (m_rFallOffRange < m_rHotSpotRange) { m_rFallOffRange = m_rHotSpotRange; }
  
    // validate volume
    if (m_fSoundVolume < FLOAT(SL_VOLUME_MIN)) { m_fSoundVolume = FLOAT(SL_VOLUME_MIN); }
    if (m_fSoundVolume > FLOAT(SL_VOLUME_MAX)) { m_fSoundVolume = FLOAT(SL_VOLUME_MAX); }

    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_BSHOOTER);
    SetModelMainTexture(TEXTURE_BSHOOTER);

    // spawn in world editor
    autowait(0.1f);
    
    wait() {
      on(EBegin) : {
        resume;
      };
    
      on(ETrigger eTrigger) : {
        if(m_bActive && m_tmFired + m_fWait < _pTimer->CurrentTick()) {
          m_tmFired = _pTimer->CurrentTick();
          Shoot(GetPlacement());
        }
    
        resume;
      }

      on(EActivate):{
        m_bActive=TRUE;
        resume;
      }

      on(EDeactivate):{
        m_bActive=FALSE;
        resume;
      }
      otherwise() : {resume;};
    }

    // cease to exist
    Destroy();

    return;
  }
};
