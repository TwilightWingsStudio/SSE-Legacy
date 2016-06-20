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

2101
%{
  #include "StdH.h"
%}

class CMovableBrush : CMovableBrushEntity {
name      "Movable Brush";
thumbnail "Thumbnails\\MovingBrush.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName    "Name" 'N'         = "Movable Brush",
   
   2 FLOAT m_fTransX       "X Translation"    = 0.0f,
   3 FLOAT m_fTransY       "Y Translation"    = 0.0f,
   4 FLOAT m_fTransZ       "Z Translation"    = 0.0f,
   
   5 FLOAT m_fRotH         "Heading Rotation" = 0.0f,
   6 FLOAT m_fRotP         "Pitch Rotation"   = 0.0f,
   7 FLOAT m_fRotB         "Banking Rotation" = 0.0f,
   
   8 BOOL m_bActive         "Active"               = TRUE,
   9 BOOL m_bZoning         "Zoning"               = FALSE,
  10 BOOL m_bDynamicShadows "Dynamic Shadows"      = FALSE,
  11 BOOL m_bRelative       "Relative Translation" = TRUE,

  12 FLOAT m_fbTransX = 0.0f,
  13 FLOAT m_fbTransY = 0.0f,
  14 FLOAT m_fbTransZ = 0.0f,
  15 FLOAT m_fbRotH   = 0.0f,
  16 FLOAT m_fbRotP   = 0.0f,
  17 FLOAT m_fbRotB   = 0.0f,

  18 FLOAT m_fAcceleration "Acceleration" = 1000,
  19 FLOAT m_fDeceleration "Deceleration" = 1000,
  
  65 FLOAT m_fHealth       "Health" 'H' = -1.0f,

  81 flags ClasificationBits m_cbClassificationBits "Clasification bits" 'C' = 0,
  82 flags VisibilityBits m_vbVisibilityBits        "Visibility bits" 'V'    = 0,

components:

functions:
  // get visibility tweaking bits
  ULONG GetVisTweaks(void)
  {
    return m_cbClassificationBits|m_vbVisibilityBits;
  }

  /* Receive damage */
  void ReceiveDamage(CEntity *penInflictor, enum DamageType dmtType,
    FLOAT fDamageAmmount, const FLOAT3D &vHitPoint, const FLOAT3D &vDirection) 
  {
    // if not destroyable
    if(m_fHealth < 0) {
      // ignore damages
      return;
    }

    // react only on explosions
    if( (dmtType == DMT_EXPLOSION) ||
        (dmtType == DMT_PROJECTILE) ||
        (dmtType == DMT_CANNONBALL) )
    {
      CMovableBrushEntity::ReceiveDamage(penInflictor, dmtType, fDamageAmmount, vHitPoint, vDirection);
    }
  };

  void PreMoving()
  {
    // start moving
    if(m_bActive){
      SetDesiredTranslation(FLOAT3D(m_fTransX, m_fTransY, m_fTransZ));
      SetDesiredRotation(FLOAT3D(m_fRotH, m_fRotP, m_fRotB));
    }else{
      SetDesiredTranslation(FLOAT3D(0.0F, 0.0F, 0.0F));
      SetDesiredRotation(FLOAT3D(0.0F, 0.0F, 0.0F));
    }
    CMovableBrushEntity::PreMoving();
  }

procedures:
  Main() {
    // declare yourself as a brush
    InitAsBrush();
    SetPhysicsFlags(EPF_ONBLOCK_PUSH|EPF_RT_SYNCHRONIZED|EPF_MOVABLE);
    SetCollisionFlags(ECF_BRUSH);
    SetHealth(m_fHealth);

    m_fbTransX = m_fTransX;
    m_fbTransY = m_fTransY;
    m_fbTransZ = m_fTransZ;
    m_fbRotH = m_fRotH;
    m_fbRotP = m_fRotP;
    m_fbRotB = m_fRotB;

    // set zoning flag
    if (m_bZoning) {
      SetFlags(GetFlags()|ENF_ZONING);
    } else {
      SetFlags(GetFlags()&~ENF_ZONING);
    }

    // set dynamic shadows as needed
    if (m_bDynamicShadows) {
      SetFlags(GetFlags()|ENF_DYNAMICSHADOWS);
    } else {
      SetFlags(GetFlags()&~ENF_DYNAMICSHADOWS);
    }

    // stop moving brush
    ForceFullStop();

    autowait(0.1f);

    AddToMovers();

    while (TRUE) {
      wait(_pTimer->TickQuantum) {
        on (EBegin) : {
          resume;
        }

        on (EActivate) : {
          m_bActive=TRUE;
          resume;
        }

        on (EDeactivate) : {
          m_bActive=FALSE;
          resume;
        }
    
        on (EDeath eDeath) : {
          
          m_fHealth = -1;
          ForceFullStop();
      
          // notify engine to kickstart entities that are cached in stationary position,
          // before we turn off, so they can fall
          NotifyCollisionChanged();

          SetFlags( GetFlags()|ENF_HIDDEN);
          SetCollisionFlags(ECF_IMMATERIAL);
        }

        on (ETimer) : { stop; }
      }

      en_fAcceleration = m_fAcceleration;
      en_fDeceleration = m_fDeceleration;

      if (m_bRelative) {
        SetPhysicsFlags(EPF_ONBLOCK_PUSH|EPF_RT_SYNCHRONIZED|EPF_MOVABLE);
      } else {
        SetPhysicsFlags(EPF_ONBLOCK_PUSH|EPF_ABSOLUTETRANSLATE|EPF_RT_SYNCHRONIZED|EPF_MOVABLE);
      }

      // start moving
      if (m_bActive) {      
        SetDesiredTranslation(FLOAT3D(m_fTransX, m_fTransY, m_fTransZ));
        SetDesiredRotation(FLOAT3D(m_fRotH, m_fRotP, m_fRotB));
      } else {
        SetDesiredTranslation(FLOAT3D(0.0F, 0.0F, 0.0F));
        SetDesiredRotation(FLOAT3D(0.0F, 0.0F, 0.0F));
      }
    } // for while(TRUE)

    return;
  }
};
