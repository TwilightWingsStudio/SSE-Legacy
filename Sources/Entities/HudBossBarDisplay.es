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

1256
%{
  #include "StdH.h"
  #include <Entities/MusicHolder.h>
%}

class CHudBossBarDisplay : CRationalEntity {
name      "CHudBossBarDisplay";
thumbnail "Thumbnails\\HudBossBarDisplay.tbn";
features "HasName", "IsTargetable";

properties:
  1 CTString m_strName        "Name" 'N' = "",
  
  4 INDEX m_iStartValue "Start Value" 'A' = 100,
  5 INDEX m_iValueIO    "IO Value" = -1,
  6 INDEX m_iMaxValue   "Max Value" = 100,
  7 INDEX m_iStep       "Step Value" = 1,
  
 10 CTFileName m_fnmIcon "Icon File" 'P' = CTString("") features(EPROPF_READONLY),
 
 11 BOOL m_bStopOnZero  "Stop on Zero" = TRUE,
 12 BOOL m_bStopOnFull  "Stop on Full" = TRUE,
 13 BOOL m_bIncremental "Incremental" = TRUE,

 19 CEntityPointer m_penMainMusicHolder,
 20 CModelObject m_moTextureHolder, 

components:
 1 model   MODEL_MARKER     "Models\\Editor\\HudBossBarDisplay.mdl",
 2 texture TEXTURE_MARKER   "Models\\Editor\\HudBossBarDisplay.tex"

functions:
  class CMusicHolder *GetMusicHolder()
  {
    if (m_penMainMusicHolder == NULL) {
      m_penMainMusicHolder = _pNetwork->GetEntityWithName("MusicHolder", 0);
    }

    return (CMusicHolder *)&*m_penMainMusicHolder;
  }

  void StartCounting(void)
  {
    CMusicHolder *pmh = GetMusicHolder();

    if (pmh == NULL) {
      return;
    }

    pmh->m_penBossBarDisplay = this;
    m_iValueIO = m_iStartValue;
  }

  void DoStep(void)
  {
    if (m_bIncremental) {
      m_iValueIO += m_iStep;
      m_iValueIO = ClampUp(m_iValueIO, m_iMaxValue);
      return;
    }
    
    if (m_iValueIO > 0) {
      m_iValueIO -= m_iStep;
      m_iValueIO = ClampDn(m_iValueIO, INDEX(0));
    }
  }

  void StopCounting(void)
  {
    CMusicHolder *pmh = GetMusicHolder();

    if (pmh == NULL) {
      return;
    }

    m_iValueIO = -1;
    pmh->m_penBossBarDisplay = NULL;
  }

procedures:
  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main(EVoid)
  {
    // declare yourself as a model
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_MARKER);
    SetModelMainTexture(TEXTURE_MARKER);
    
    // Checks
    m_iValueIO = -1;
    m_iStep = Abs(m_iStep);
    m_iMaxValue = Abs(m_iMaxValue);
    m_iStartValue = Clamp(m_iStartValue, INDEX(0), m_iMaxValue);
    
    // If selected icon then try to load it!
    if (m_fnmIcon != CTString(""))
    {
      try {
        m_moTextureHolder.mo_toTexture.SetData_t(m_fnmIcon);
      } catch (char *strError) {
        WarningMessage(strError);
      }
    }

    autowait(0.1f);

    wait()
    {
      on (EBegin) :
      {
        resume;
      }

      // when started
      on (EStart) :
      {
        StartCounting();
        resume;
      }

      // when stopped
      on (EStop) :
      {
        StopCounting();
        resume;
      }

      // when triggered 
      on (ETrigger) :
      {
        // If not started yet then start.
        if (m_iValueIO == -1) {
          StartCounting();
        }

        DoStep();
        
        // If filled then stop.
        if (m_iValueIO >= m_iMaxValue && m_bStopOnFull) {
          StopCounting();
        }

        // If finished then stop.
        if (m_iValueIO == 0 && m_bStopOnZero) {
          StopCounting();
        }

        resume;
      }
    }

    return;
  }
};
