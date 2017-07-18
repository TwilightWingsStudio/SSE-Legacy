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

2147
%{
  #include "StdH.h"
  #include "EntitiesMP/Player.h"
%}

class CShellExecutor : CRationalEntity {
name      "ShellExecutor";
thumbnail "Thumbnails\\ShellExecutor.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'      = "Shell Executor",
   3 CTString m_strDescription = "",
   
   4 BOOL m_bActive              "Active" 'A' = TRUE,
   5 BOOL m_bDebugMessages    "Debug Messages" = FALSE,
   6 CTString m_strConsoleCmd   "Console Command" = "",
   7 CTString m_strConsoleMsg   "Console Message" = "",

components:
  1 model   MODEL_TELEPORTER   "Models\\Editor\\ShellExecutor.mdl",
  2 texture TEXTURE_TELEPORTER "Models\\Editor\\ShellExecutor.tex",

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
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Called every time when entity receiving ETrigger entity event.
  // --------------------------------------------------------------------------------------
  void DoExecution()
  {
    if (m_strConsoleCmd != "") {
      if (m_strConsoleCmd.HasPrefix("net_") || m_strConsoleCmd.HasPrefix("ser_") || m_strConsoleCmd.HasPrefix("gam_") || m_strConsoleCmd.HasPrefix("cli_") || m_strConsoleCmd.HasPrefix("tex_") || m_strConsoleCmd.HasPrefix("Quit()")) {
        if (m_bDebugMessages) {
          CPrintF("[SE] %s : Error! Attempt to use not allowed shell command!\n", m_strName);
        }
      } else {
        if (m_bDebugMessages) {
          CPrintF("[SE] %s : Executing command: %s\n", m_strName, m_strConsoleCmd);
        }
        _pShell->Execute(m_strConsoleCmd);
      }
    } else {
      if (m_bDebugMessages) {
        CPrintF("[SE] %s : Console command field is empty!\n", m_strName);
      }
    }
    
    if (m_strConsoleMsg != "") {
      CPrintF(TRANS("%s\n"), m_strConsoleMsg);
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
    SetModel(MODEL_TELEPORTER);
    SetModelMainTexture(TEXTURE_TELEPORTER);
  
    autowait(0.1f);
  
    wait()
    {
      on (EBegin) : { 
        resume;
      }

      on(ETrigger) : {
        if (m_bActive) {
          DoExecution();
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

