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

438
%{
  #include "StdH.h"
  #include <Engine/Scripts/ScriptEngine.h>
%}

class CScriptHolder: CRationalEntity {
name      "ScriptHolder";
thumbnail "Thumbnails\\ScriptHolder.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'      = "Script Holder",
   3 CTString m_strDescription = "",
   
   4 BOOL m_bActive           "Active" 'A' = TRUE,
   5 BOOL m_bDebugMessage     "Debug Messages" = FALSE,
   6 CTFileName m_fnScript    "Script File"    'S' = CTFILENAME(""),

components:
  1 model   MODEL_SCRIPTHOLDER   "Models\\Editor\\ScriptHolder.mdl",
  2 texture TEXTURE_SCRIPTHOLDER "Models\\Editor\\ScriptHolder.tex",

functions:
  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const {
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Called every time when entity receiving ETrigger entity event.
  // --------------------------------------------------------------------------------------
  void DoExecution(const ETrigger &eTrigger)
  {
    INDEX aiPins[5];
    
    if (m_fnScript != "" && FileExists(m_fnScript)) {
      _pScriptEngine->ExecEntityScript(this, m_fnScript, eTrigger.penCaused, aiPins);
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

    // set appearance
    SetModel(MODEL_SCRIPTHOLDER);
    SetModelMainTexture(TEXTURE_SCRIPTHOLDER);
  
    autowait(0.1f);
  
    wait()
    {
      on (EBegin) : { 
        resume;
      }

      on (ETrigger eTrigger) : {
        if (m_bActive) {
          DoExecution(eTrigger);
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