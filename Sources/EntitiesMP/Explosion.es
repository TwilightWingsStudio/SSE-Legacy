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

234
%{
  #include "StdH.h"
%}

uses "EntitiesMP/BasicEffects";

enum ExplosionType {
  0 BOM_ROCKET   "Rocket",      // rocketlauncher
  1 BOM_GRENADE  "Grenade",     // grenades launcher
  2 BOM_CANNON   "CannonИall",  // iron cannon ball
};

%{
  void CExplosion_Precache(void)
  {
    CDLLEntityClass *pdec = &CExplosion_DLLClass;
    pdec->PrecacheClass(CLASS_BASIC_EFFECT, BET_ROCKET);
    pdec->PrecacheClass(CLASS_BASIC_EFFECT, BET_GRENADE);
    pdec->PrecacheClass(CLASS_BASIC_EFFECT, BET_CANNON);
    pdec->PrecacheClass(CLASS_BASIC_EFFECT, BET_EXPLOSIONSTAIN);
    pdec->PrecacheClass(CLASS_BASIC_EFFECT, BET_CANNONEXPLOSIONSTAIN);
    pdec->PrecacheClass(CLASS_BASIC_EFFECT, BET_SHOCKWAVE);
    pdec->PrecacheClass(CLASS_BASIC_EFFECT, BET_CANNONSHOCKWAVE);
    pdec->PrecacheClass(CLASS_BASIC_EFFECT, BET_ROCKET_PLANE);
    pdec->PrecacheClass(CLASS_BASIC_EFFECT, BET_GRENADE_PLANE);
  }
  
  void CExplosion_OnInitClass(void)
  {
    CExplosion_Precache();
  }
  
  // easing the use of cannon explosions
  #define STRETCH_3  FLOAT3D(3.0f,3.0f,3.0f)
  #define STRETCH_4  FLOAT3D(4.0f,4.0f,4.0f)
%}

class CExplosion: CRationalEntity {
  name "Explosion";
  thumbnail "Thumbnails\\Explosion.tbn";
  features "IsTargetable", "HasName", "ImplementsOnInitClass";

properties:
  1 CTString m_strName            "Name" 'N' = "Explosion",          // class name
  2 enum ExplosionType m_exlType  "Type" 'E' = BOM_ROCKET, // what kind of explosion

 10 FLOAT m_fStretch              "Size Multiplier" 'S' = 1.0f,
 11 FLOAT m_fDamage               "Damage" 'D' = 0.0f,
 12 FLOAT m_fWait                 "Waiting Time" 'W' = 0.0f,
 13 INDEX m_ctMaxTrigs            "Max Trigs" 'M' = 1,

 30 CSoundObject m_soEffect,    // explosion special effect

components:
  1 model   MODEL_EXPLOSION         "Models\\Editor\\Explosion.mdl",
  2 texture TEXTURE_EXPLOSION       "Models\\Editor\\Explosion.tex",

  // the explosions themselves
  3 class   CLASS_BASIC_EFFECT  "Classes\\BasicEffect.ecl",

functions:
  // spawn effect
  void SpawnExplosionEffect(const CPlacement3D &plEffect, const ESpawnEffect &eSpawnEffect)
  {
    CEntityPointer penEffect = CreateEntity(plEffect, CLASS_BASIC_EFFECT);
    penEffect->Initialize(eSpawnEffect);

    return;
  };

  // a single cannon explosion
  void Explosion(FLOAT3D vCenter,
                 const FLOAT3D &vStretchExplosion,
                 const FLOAT3D &vStretchShockwave,
                 const FLOAT3D &vStretchStain,
                 BOOL bHasExplosion,
                 BOOL bHasShockWave,
                 BOOL bHasStain,
                 BOOL bHasLight)
  {
    ESpawnEffect ese;
    FLOAT3D vOnPlane;
    FLOATplane3D vPlaneNormal;
    FLOAT fDistanceToEdge;
  
    // explosion
    if (bHasExplosion) {
      ese.colMuliplier = C_WHITE|CT_OPAQUE;
    
      if (bHasLight) {
          ese.betType = BET_CANNON;
      } else {
          ese.betType = BET_CANNON_NOLIGHT;
      }
    
      ese.vStretch = vStretchExplosion;
      CPlacement3D plHandle = GetPlacement();
      plHandle.pl_PositionVector+=vCenter;
      SpawnExplosionEffect(plHandle, ese);
    }
  
    // on plane
    if (GetNearestPolygon(vOnPlane, vPlaneNormal, fDistanceToEdge)) {
      if ((vOnPlane-GetPlacement().pl_PositionVector).Length() < 3.5f) {
        if (bHasStain) {
          // wall stain
          ese.colMuliplier = C_WHITE|CT_OPAQUE;
          ese.betType = BET_CANNONEXPLOSIONSTAIN;
          ese.vNormal = FLOAT3D(vPlaneNormal);
          ese.vStretch = vStretchShockwave;
          SpawnExplosionEffect(CPlacement3D(vOnPlane, ANGLE3D(0, 0, 0)), ese);
        }
    
        if (bHasShockWave) {
           // shock wave horizontal
           ese.colMuliplier = C_WHITE|CT_OPAQUE;
           ese.betType = BET_CANNONSHOCKWAVE;
           ese.vNormal = FLOAT3D(vPlaneNormal);
           ese.vStretch = vStretchShockwave;
           SpawnExplosionEffect(CPlacement3D(vOnPlane, ANGLE3D(0, 0, 0)), ese);
        }
      }
    }

    return;
  }

procedures:
  // rocket explosion
  RocketExplodeEntity()
  {
    ESpawnEffect ese;
    FLOAT3D vPoint;
    FLOATplane3D vPlaneNormal;
    FLOAT fDistanceToEdge;

    ese.colMuliplier = C_WHITE|CT_OPAQUE;
    ese.betType = BET_ROCKET;
    ese.vStretch = FLOAT3D(1,1,1)*m_fStretch;
    SpawnExplosionEffect(GetPlacement(), ese);

    // if there are any polygons nearby
    if (GetNearestPolygon(vPoint, vPlaneNormal, fDistanceToEdge)) {
        if ((vPoint-GetPlacement().pl_PositionVector).Length() < 3.5f*m_fStretch) {
          // stain
          ese.betType = BET_EXPLOSIONSTAIN;
          ese.vNormal = FLOAT3D(vPlaneNormal);
          SpawnExplosionEffect(CPlacement3D(vPoint, ANGLE3D(0, 0, 0)), ese);
          // shock wave
          ese.betType = BET_SHOCKWAVE;
          ese.vNormal = FLOAT3D(vPlaneNormal);
          SpawnExplosionEffect(CPlacement3D(vPoint, ANGLE3D(0, 0, 0)), ese);
          // second explosion on plane
          ese.betType = BET_ROCKET_PLANE;
          ese.vNormal = FLOAT3D(vPlaneNormal);
          SpawnExplosionEffect(CPlacement3D(vPoint+ese.vNormal/50.0f, ANGLE3D(0, 0, 0)), ese);
      }
    }
    
    // done with special effects
    // moving on to damage
    InflictRangeDamage(this, DMT_EXPLOSION, m_fDamage,
       GetPlacement().pl_PositionVector, 4.0f*m_fStretch, 8.0f*m_fStretch);
    
    return;
  };


  // grenade explosion
  GrenadeExplodeEntity()
  {
    ESpawnEffect ese;
    FLOAT3D vPoint;
    FLOATplane3D vPlaneNormal;
    FLOAT fDistanceToEdge;

    ese.colMuliplier = C_WHITE|CT_OPAQUE;
    ese.betType = BET_GRENADE;
    ese.vStretch = FLOAT3D(1,1,1)*m_fStretch;
    SpawnExplosionEffect(GetPlacement(), ese);

    // if there are any polygons nearby
    if (GetNearestPolygon(vPoint, vPlaneNormal, fDistanceToEdge)) {
        if ((vPoint-GetPlacement().pl_PositionVector).Length() < 3.5f*m_fStretch) {
          // stain
          ese.betType = BET_EXPLOSIONSTAIN;
          ese.vNormal = FLOAT3D(vPlaneNormal);
          SpawnExplosionEffect(CPlacement3D(vPoint, ANGLE3D(0, 0, 0)), ese);
          // shock wave
          ese.betType = BET_SHOCKWAVE;
          ese.vNormal = FLOAT3D(vPlaneNormal);
          SpawnExplosionEffect(CPlacement3D(vPoint, ANGLE3D(0, 0, 0)), ese);
          // second explosion on plane
          ese.betType = BET_GRENADE_PLANE;
          ese.vNormal = FLOAT3D(vPlaneNormal);
          SpawnExplosionEffect(CPlacement3D(vPoint+ese.vNormal/50.0f, ANGLE3D(0, 0, 0)), ese);
      }
    }

    // done with special effects
    // moving on to damage
    InflictRangeDamage(this, DMT_EXPLOSION, m_fDamage,
       GetPlacement().pl_PositionVector, 4.0f*m_fStretch, 8.0f*m_fStretch);
    
    return;
  };


  // cannonball explosion
  CannonExplodeEntity()
  {
    // a cannon's explosion is actually FOUR explosions
    Explosion( FLOAT3D(0.0f,0.0f,0.0f),   STRETCH_3*m_fStretch, STRETCH_3*m_fStretch, STRETCH_4*m_fStretch, TRUE, TRUE,  TRUE, TRUE);
    Explosion( FLOAT3D(1.0f,1.5f,1.5f),   STRETCH_3*m_fStretch, STRETCH_3*m_fStretch, STRETCH_4*m_fStretch, TRUE, FALSE, FALSE, FALSE);
    Explosion( FLOAT3D(-2.0f,1.0f,-1.5f), STRETCH_3*m_fStretch, STRETCH_3*m_fStretch, STRETCH_4*m_fStretch, TRUE, FALSE, FALSE, FALSE);
    Explosion( FLOAT3D(-1.0f,0.5f,1.0f),  STRETCH_4*m_fStretch, STRETCH_4*m_fStretch, STRETCH_4*m_fStretch, TRUE, FALSE, FALSE, FALSE);
    
  // done with special effects
    // moving on to damage
    InflictRangeDamage(this, DMT_EXPLOSION, m_fDamage,
       GetPlacement().pl_PositionVector, 2.0f*m_fStretch, 12.0f*m_fStretch);
    
    return;
  };

  Detonate()
  {
    // wait if required
    if (m_fWait > 0.0f) {
      wait (m_fWait) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }

    // if we're using a maximum amount of triggering, self destruct
    if (m_ctMaxTrigs >= 0) {
      m_ctMaxTrigs--;
      if (m_ctMaxTrigs < 0) {
        Destroy();
        return;
      }
    }
    
    // doing the appropriate explosion
    switch (m_exlType) {
      case BOM_ROCKET: {
        jump RocketExplodeEntity();
        break;
      }
      
      case BOM_GRENADE: {
        jump GrenadeExplodeEntity();
        break;
      }

      case BOM_CANNON: {
        jump CannonExplodeEntity();
        break;
      }
    }

    return; // though we'll never get here
  }

  // main loop
  // in case I want to implement inactive/active modes
  Active()
  {
    // infinite loop, until explosion is destroyed
    while(TRUE) {
      wait() {
        on(ETrigger eTrigger): {
          call Detonate();
        }
      }
    } 
  }
        
  Main() { 
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);
  
    // set appearance
    SetModel(MODEL_EXPLOSION);
    SetModelMainTexture(TEXTURE_EXPLOSION);

    // spawn in world editor
    autowait(0.1f);
    jump Active();

    Destroy();
    return;
  }
};