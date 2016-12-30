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

389
%{
  #include "StdH.h"
  #include "EntitiesMP/Projectile.h"
%}

class CFlameInflictor: CRationalEntity {
name      "FlameInflictor";
thumbnail "Thumbnails\\FlameInflictor.tbn";
features  "HasName", "IsTargetable";

properties:
  1 CTString m_strName          "Name" 'N'  = "FlameInflictor",
  2 CTString m_strDescription               = "",
  3 INDEX m_iStrength       "Strength" 'S'  = 1,
  5 CEntityPointer m_penToDamage  "Target" 'T' COLOR(0xFF6218FF),
  6 CEntityPointer m_Inflictor "Inflictor" 'I' COLOR(0xAA6016FF),
  7 BOOL m_bInflictorPenCaused "Inflictor=penCaused" 'C' = FALSE,

components:
  1 model   MODEL_INEDITOR     "Models\\Editor\\FlameInflictor.mdl",
  2 texture TEXTURE_INEDITOR   "Models\\Editor\\FlameInflictor.tex",
  3 class CLASS_FLAME "Classes\\Flame.ecl",

functions:
  // --------------------------------------------------------------------------------------
  // No comments.
  // --------------------------------------------------------------------------------------
  void Precache(void)
  {
    PrecacheClass(CLASS_FLAME);
  }

  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const {
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Puts victim on fire.
  // --------------------------------------------------------------------------------------
  void DoEffect(CEntity* penInflictor, CEntity* penVictim)
  {
    if (penVictim != NULL)
    {
      CPlacement3D placeP3D = penVictim->GetPlacement();
      const FLOAT3D placeF3D = placeP3D.pl_PositionVector;
      SpawnFlame(penInflictor, penVictim, placeF3D);
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

    // Set appearance
    SetModel(MODEL_INEDITOR);
    SetModelMainTexture(TEXTURE_INEDITOR);

    m_iStrength = Clamp(m_iStrength, INDEX(0), 100);
    
    while (TRUE)
    {
      wait()
      {
        on (ETrigger eTrigger) :
        {
          int i = m_iStrength;
          CEntity* penInflictor = this;
          CEntity* penVictim    = m_penToDamage;

          if (m_Inflictor) {
            penInflictor = m_Inflictor;
          } else if (m_bInflictorPenCaused){
            penInflictor = eTrigger.penCaused;
          }

          if (penVictim == NULL) {
            penVictim = eTrigger.penCaused;
          }
      
          /* Calling SpawnFlame() on the same entity twice causes it to burn, that means taking periodic damage over time,
          * hence strength >= 2 will cause the entity to take that periodic damage.
          */
          while (i > 0) {
            DoEffect(penInflictor, penVictim);
            i--;
          }
      
          stop;
        }

        otherwise() : {
          resume;
        };
      };
    }
  }
};
