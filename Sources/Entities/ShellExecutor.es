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

2147
%{
  #include "StdH.h"
  #include "Entities/Player.h"
%}

enum EShellExecutorSide
{
  0 ESES_BOTH       "Client & Server [0]",
  1 ESES_SERVERONLY "Server Only [1]",
};

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
   8 enum EShellExecutorSide m_eType "Type" = ESES_BOTH,

components:
  1 model   MODEL_EXECUTOR   "Models\\Editor\\ShellExecutor.mdl",
  2 texture TEXTURE_EXECUTOR "Models\\Editor\\ShellExecutor.tex",

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
  // Called every time when it is triggered.
  // --------------------------------------------------------------------------------------
  void DoExecution()
  {
    if (m_strConsoleCmd != "") {
      if (m_strConsoleCmd.HasPrefix("net_") || m_strConsoleCmd.HasPrefix("ser_") || m_strConsoleCmd.HasPrefix("wld_")  || m_strConsoleCmd.HasPrefix("gam_") || m_strConsoleCmd.HasPrefix("cli_") || m_strConsoleCmd.HasPrefix("tex_") || m_strConsoleCmd.HasPrefix("Quit()")) {
        if (m_bDebugMessages) {
          CPrintF("[SE] %s : Error! Attempt to use not allowed shell command!\n", m_strName);
        }
      } else {
        if (m_eType == ESES_BOTH || (m_eType == ESES_SERVERONLY && _pNetwork->IsServer())) {
          if (m_bDebugMessages) {
            CPrintF("[SE] %s : Executing command: %s\n", m_strName, m_strConsoleCmd);
          }

          _pShell->Execute(m_strConsoleCmd + ";");
        } else {
          if (m_bDebugMessages) {
            CPrintF("[SE] %s : Command not executed! Not a server!\n", m_strName);
          }
        }
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

    // Set appearance.
    SetModel(MODEL_EXECUTOR);
    SetModelMainTexture(TEXTURE_EXECUTOR);
  
    autowait(0.1f);
  
    wait()
    {
      on (EBegin) : { 
        resume;
      }

      on (ETrigger) :
      {
        if (m_bActive) {
          DoExecution();
        }
        resume;
      }

      on (ETargeted) :
      {
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

