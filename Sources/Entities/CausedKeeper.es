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

1488
%{
  #include "StdH.h"
%}

class CCausedKeeper: CRationalEntity {
name      "CausedKeeper";
thumbnail "Thumbnails\\CausedKeeper.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'  = "Caused Keeper",
   3 CTString m_strDescription      = "",
   
   4 BOOL m_bActive         "Active" 'A' = TRUE,
   5 BOOL m_bDebugMessages  "Debug Messages" = FALSE,

   6 CEntityPointer m_penTarget "Target" 'T',

  10 CTString m_strOnlyClass "Class Only" = "",
  
  11 BOOL m_bResetOnNull  "Reset If penCaused=NULL" = TRUE,

components:
  1 model   MODEL_CAUSEDKEEPER   "Models\\Editor\\CausedKeeper.mdl",
  2 texture TEXTURE_CAUSEDKEEPER "Models\\Editor\\CausedKeeper.tex",

functions:
  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const
  {
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Saves penCaused into property if possible.
  // --------------------------------------------------------------------------------------
  void DoKeep(CEntity *penCaused)
  {
    if (penCaused == NULL)
    {
      if (m_bResetOnNull) {
        if (m_bDebugMessages) {
          CPrintF("[%s] : Setting Target to NULL.\n", m_strName);
        }

        m_penTarget = NULL; 
      }

      return;
    }
  
    // If class filter exists then apply it.
    if (m_strOnlyClass != "") {
      if (!IsDerivedFromClass(penCaused, m_strOnlyClass)) {
        return;
      }
    }

    if (m_bDebugMessages) {
      CPrintF("[%s] : Setting Target to [#%d][%s].\n", m_strName, penCaused->en_ulID, penCaused->GetName());
    }

    m_penTarget = penCaused;
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
    SetModel(MODEL_CAUSEDKEEPER);
    SetModelMainTexture(TEXTURE_CAUSEDKEEPER);
  
    autowait(0.1f);
  
    wait()
    {
      on (EBegin) : { 
        resume;
      }

      on (ETrigger eTrigger) :
      {
        if (m_bActive) {
          DoKeep(eTrigger.penCaused);
        }
        resume;
      }

      on (ETargeted eTargeted) :
      {
        if (m_bActive) {
          DoKeep(eTargeted.penCaused);
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

      on (EStop) : {
        if (!m_bActive) {
          resume;
        }

        if (m_bDebugMessages) {
          CPrintF("[%s] : Forced resetting Target to NULL!\n", m_strName);
        }

        m_penTarget = NULL;

        resume;
      }
    }
  }
};

