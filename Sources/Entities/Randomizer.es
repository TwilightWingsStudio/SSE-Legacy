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

229
%{
  #include "StdH.h"
  #include "Entities/Player.h"
  #include "Entities/PlayerWeapons.h"
%}

// diferent types of testing
enum ERndValueType {
  0 ERVT_INDEX    "0 Index",
  1 ERVT_FLOAT    "1 Float",
};

class CRandomizer : CRationalEntity {
name      "Randomizer";
thumbnail "Thumbnails\\Randomizer.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'      = "Randomizer",
   3 CTString m_strDescription = "",
   
   4 BOOL m_bActive              "Active" 'A' = TRUE,
   5 BOOL m_bDebugMessages "Debug Messages" = FALSE,
   6 enum ERndValueType ervType "Type" = ERVT_INDEX,
   
   8 FLOAT m_fMinValue     "Min Value" = 0.0F,
   9 FLOAT m_fMaxValue     "Max Value" = 1.0F,
  10 FLOAT m_fOutput       "Output Value" = 0.0F,

components:
  1 model   MODEL_RANDOMIZER   "Models\\Editor\\Randomizer.mdl",
  2 texture TEXTURE_RANDOMIZER "Models\\Editor\\Randomizer.tex",

functions:
  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const {
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Generates these fucked random numbers!
  // --------------------------------------------------------------------------------------
  void DoRandom(const ETrigger &eTrigger)
  {
    if (ervType == ERVT_INDEX) {
      m_fOutput = IRnd()%((INDEX)(ceil(m_fMaxValue) - ceil(m_fMinValue) + 1)) + ceil(m_fMinValue);
      if (m_bDebugMessages) {
        CPrintF("%s : Generated random INDEX[%d, %d] = %d\n", m_strName, (INDEX)ceil(m_fMinValue), (INDEX)ceil(m_fMaxValue), (INDEX)m_fOutput);
      }
    } else {
      m_fOutput = FRnd() * (m_fMaxValue - m_fMinValue) + m_fMinValue;
      if (m_bDebugMessages) {
        CPrintF("%s : Generated random FLOAT[%f, %f] = %f\n", m_strName, m_fMinValue, m_fMaxValue, m_fOutput);
      }
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
    
    // Retard protection.
    m_fMinValue = ClampDn(m_fMinValue, 0.0F);
    m_fMaxValue = ClampDn(m_fMaxValue, m_fMinValue);

    // Set appearance.
    SetModel(MODEL_RANDOMIZER);
    SetModelMainTexture(TEXTURE_RANDOMIZER);
  
    autowait(0.1f);
  
    wait()
    {
      on (EBegin) : { 
        resume;
      }

      on(ETrigger eTrigger) : {
        if (m_bActive) {
          DoRandom(eTrigger);
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
    }
  }
};

