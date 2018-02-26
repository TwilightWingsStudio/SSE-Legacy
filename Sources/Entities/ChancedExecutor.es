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

479
%{
  #include "StdH.h"
%}

class CChancedExecutor : CRationalEntity {
name      "CChancedExecutor";
thumbnail "Thumbnails\\ChancedExecutor.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'      = "Chanced Executor",
   3 CTString m_strDescription = "",
   
   4 BOOL m_bActive               "Active" 'A' = TRUE,
   5 BOOL m_bDebugMessages "Debug Messages" = FALSE,
   
   6 FLOAT m_fChance "Chance (0-1)" = 0.0F,
   
  10 CEntityPointer m_penSuccessTarget    "Success Target" 'T',
  11 CEntityPointer m_penFailTarget       "Fail Target" 'F',
  12 enum EventEType m_eetSuccess  "Event type Success" = EET_TRIGGER,
  13 enum EventEType m_eetFail     "Event type Fail" = EET_TRIGGER,
  
components:
  1 model   MODEL_EXECUTOR   "Models\\Editor\\ChancedExecutor.mdl",
  2 texture TEXTURE_EXECUTOR "Models\\Editor\\ChancedExecutor.tex",

functions:
  // --------------------------------------------------------------------------------------
  // [SSE] Extended Engine API
  // Returns TRUE if main entity logic is active.
  // --------------------------------------------------------------------------------------
  virtual BOOL IsActive(void) const
  {
    return m_bActive;
  }

  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const
  {
    ((CTString&)m_strDescription).PrintF("%.4f <=", m_fChance);
    
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Called every time when entity receiving ETrigger entity event.
  // --------------------------------------------------------------------------------------
  void DoExecution(const ETrigger &eTrigger)
  {
    FLOAT fGenFloat = FRnd();
    
    if (fGenFloat <= m_fChance) {
      if (m_bDebugMessages) {
        CPrintF("%s : Success! (%.2f <= %.2f)\n", GetName(), fGenFloat, m_fChance);
        
        if (m_penSuccessTarget) {
          CPrintF("  Sent event to '%s'.\n", m_penSuccessTarget->GetName());
        } else {
          CPrintF("  No target!\n");
        }
      }

      if (m_penSuccessTarget) {
        SendToTarget(m_penSuccessTarget, m_eetSuccess, eTrigger.penCaused);
      }
    } else {
      if (m_bDebugMessages) {
        CPrintF("%s : Fail! (%.2f > %.2f)\n", GetName(), fGenFloat, m_fChance);
        
        if (m_penSuccessTarget) {
          CPrintF("  Sent event to '%s'.\n", m_penSuccessTarget->GetName());
        } else {
          CPrintF("  No target!\n");
        }
      }

      if (m_penFailTarget) {
        SendToTarget(m_penFailTarget, m_eetFail, eTrigger.penCaused);
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

    // set appearance
    SetModel(MODEL_EXECUTOR);
    SetModelMainTexture(TEXTURE_EXECUTOR);
    
    m_fChance = Clamp(m_fChance, 0.0001F, 1.0F);
  
    autowait(0.1f);
  
    wait()
    {
      on (EBegin) : { 
        resume;
      }

      on(ETrigger eTrigger) : {
        if (m_bActive) {
          DoExecution(eTrigger);
        }

        resume;
      }

      on (EActivate) : {
        if (m_bDebugMessages) {
          CPrintF("%s : Activated!\n", GetName());
        }
        
        m_bActive = TRUE;
        resume;
      }

      on (EDeactivate) : {
        if (m_bDebugMessages) {
          CPrintF("%s : Deactivated!\n", GetName());
        }

        m_bActive = FALSE;
        resume;
      }
    }
  }
};