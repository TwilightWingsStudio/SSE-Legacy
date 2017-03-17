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
  
  #define SL_PITCH_MIN (0.01F)
  #define SL_PITCH_MAX (100.0F)
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

  17 enum EBSUsePenCausedAs m_eUsePenCausedAs "Use penCaused as ..." = EBSUP_NONE,

  20 FLOAT m_fWait      "Min Fire Delay" = 0.1F,
  21 FLOAT m_tmFired = 0.0F,
  
  50 CTFileName m_fnSound  "Sound File" = CTString(""),
  51 CSoundObject m_soFire,
  
  52 RANGE m_rFallOffRange "Sound Fall-off" 'F'   = 100.0F,
  53 RANGE m_rHotSpotRange "Sound Hot-spot" 'H'   = 50.0F,
  54 FLOAT m_fSoundVolume  "Sound Volume"   'V'   = 1.0F,
  55 FLOAT m_fSoundPitch   "Sound Pitch"          = 1.0F,
  56 BOOL m_bSurround      "Sound Surround" 'R'   = FALSE,
  57 BOOL m_bVolumetric    "Sound Volumetric" 'O' = TRUE,

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

    m_aps.Precache(m_fnSound);
  }

  // --------------------------------------------------------------------------------------
  // Apply mirror and stretch to the entity.
  // --------------------------------------------------------------------------------------
  void MirrorAndStretch(FLOAT fStretch, BOOL bMirrorX)
  {
    // Stretch its ranges.
    m_rFallOffRange *= fStretch;
    m_rHotSpotRange *= fStretch;
  }
  
  // --------------------------------------------------------------------------------------
  // Fires the bullet in given direction with given properties.
  // --------------------------------------------------------------------------------------
  void ShootBullet(const ETrigger &eTrigger, const CPlacement3D &pl)
  {
    CEntity *penInflictor = m_penInflictor;
    CEntity *penTarget = m_penTarget;
    
    CEntityPointer penBullet = CreateEntity(pl, CLASS_BULLET);

    // Initialize the bullet.
    EBulletInit eInit;
    
    if (m_eUsePenCausedAs == EBSUP_INFLICTOR) {
      penInflictor = eTrigger.penCaused;
    } else if (m_eUsePenCausedAs == EBSUP_DESTINATION) {
      penTarget = eTrigger.penCaused;
    }

    if (penInflictor) {
      eInit.penOwner = penInflictor;
    } else {
      eInit.penOwner = this;
    }
  
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
  void Shoot(const ETrigger &eTrigger, const CPlacement3D &pl)
  {
    ShootBullet(eTrigger, pl);

    // Play shooting sound.
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
  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main(EVoid)
  {
    // Validate range.
    if (m_rHotSpotRange < 0.0f) { m_rHotSpotRange = 0.0f; }
    if (m_rFallOffRange < m_rHotSpotRange) { m_rFallOffRange = m_rHotSpotRange; }
  
    // Validate volume.
    if (m_fSoundVolume < FLOAT(SL_VOLUME_MIN)) { m_fSoundVolume = FLOAT(SL_VOLUME_MIN); }
    if (m_fSoundVolume > FLOAT(SL_VOLUME_MAX)) { m_fSoundVolume = FLOAT(SL_VOLUME_MAX); }

    // Validate pitch.
    if (m_fSoundPitch < FLOAT(SL_PITCH_MIN)) { m_fSoundPitch = FLOAT(SL_PITCH_MIN); }
    if (m_fSoundPitch > FLOAT(SL_PITCH_MAX)) { m_fSoundPitch = FLOAT(SL_PITCH_MAX); }

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
      on(EBegin) : {
        resume;
      };
    
      on(ETrigger eTrigger) :
      {
        if (m_bActive && m_tmFired + m_fWait < _pTimer->CurrentTick()) {
          m_tmFired = _pTimer->CurrentTick();
          Shoot(eTrigger, GetPlacement());
        }
    
        resume;
      }

      on(EActivate):{
        m_bActive = TRUE;
        resume;
      }

      on(EDeactivate):{
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
