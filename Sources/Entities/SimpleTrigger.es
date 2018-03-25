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

318
%{
  #include "StdH.h"
  extern INDEX ent_bReportBrokenChains;
%}

enum ESTType {
  0 STT_NORMAL     "Normal [0]",
  1 STT_RANDOM     "Random [1]",
  2 STT_PROCEDURAL "Procedural [2]",
};

enum ESTMaxTrigsBehavior {
  0 ESTMTB_DESTROY "Destroy the Trigger [0]",
  1 ESTMTB_KEEP    "Keep in the World [1]"  
};

class CSimpleTrigger: CRationalEntity {
name      "SimpleTrigger";
thumbnail "Thumbnails\\SimpleTrigger.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName              "Name" 'N' = "Simple Trigger",         // class name
   2 CTString m_strDescription = "",
   
   3 BOOL m_bActive                "Active" 'V' = TRUE,        // starts in active/inactive state
   4 BOOL m_bDebugMessages         "Debug Messages" = FALSE,

   5 CEntityPointer m_penTarget1   "Target 01" 'T' COLOR(C_RED|0xFF),                 // send event to entity
   6 CEntityPointer m_penTarget2   "Target 02" 'Y' COLOR(C_RED|0xFF),
   7 CEntityPointer m_penTarget3   "Target 03" 'U' COLOR(C_RED|0xFF),
   8 CEntityPointer m_penTarget4   "Target 04" 'I' COLOR(C_RED|0xFF),
   9 CEntityPointer m_penTarget5   "Target 05" 'O' COLOR(C_RED|0xFF),

  10 enum EventEType m_eetEvent1   "Event type Target 01" 'G' = EET_TRIGGER,  // type of event to send
  11 enum EventEType m_eetEvent2   "Event type Target 02" 'H' = EET_TRIGGER,
  12 enum EventEType m_eetEvent3   "Event type Target 03" 'J' = EET_TRIGGER,
  13 enum EventEType m_eetEvent4   "Event type Target 04" 'K' = EET_TRIGGER,
  14 enum EventEType m_eetEvent5   "Event type Target 05" 'L' = EET_TRIGGER,

  30 FLOAT m_fWaitTime             "Wait" 'W' = 0.0f,          // wait before send events
  31 BOOL m_bAutoStart             "Auto start" 'A' = FALSE,   // trigger auto starts

  40 CEntityPointer m_penCaused,     // who touched it last time
  41 CEntityPointer m_penTargetArg,
  
  42 INDEX m_ctMaxTrigs            "Max trigs" 'X' = -1, // how many times could trig
  43 enum ESTMaxTrigsBehavior m_eMaxTrigsBehavior "Max trigs Behavior" = ESTMTB_DESTROY,

  45 COLOR m_colColor              "Trigger Color" = C_WHITE,

  55 enum EventEType m_eetGlobal   "Event type Global" = EET_IGNORE,

  93 enum ESTType m_eTType  "Type" = STT_NORMAL,
  94 INDEX m_iPos                 = 0,

components:
  1 model   MODEL_MARKER     "Models\\Editor\\SimpleTrigger.mdl",
  2 texture TEXTURE_MARKER   "Models\\Editor\\SimpleTrigger.tex"

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
  // The entity event handler.
  // --------------------------------------------------------------------------------------
  BOOL HandleEvent(const CEntityEvent &ee)
  {
    if (ee.ee_slEvent== EVENTCODE_EActivate) {
      if (!m_bActive)
      {
        m_bActive = TRUE;
        
        if (m_bDebugMessages) {
          CPrintF(TRANS("[%s] Activated!\n"), m_strName);
        }
      }
    }

    if (ee.ee_slEvent == EVENTCODE_EDeactivate) {
      if (m_bActive)
      {
        m_bActive = FALSE;

        if (m_bDebugMessages) {
          CPrintF(TRANS("[%s] Deactivated!\n"), m_strName);
        }
      }
    }

    return CRationalEntity::HandleEvent(ee);
  }

  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const
  {
    INDEX ctUsed = 0;

    if (m_penTarget1)  { ctUsed++; }
    if (m_penTarget2)  { ctUsed++; }
    if (m_penTarget3)  { ctUsed++; }
    if (m_penTarget4)  { ctUsed++; }
    if (m_penTarget5)  { ctUsed++; }
    
    ((CTString&)m_strDescription).PrintF("[%d/5] W=%.2f", ctUsed, m_fWaitTime);
    
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Extended function to use global event type.
  // --------------------------------------------------------------------------------------
  void SendToTargetM(CEntity *penSendEvent, EventEType eetEventType, CEntity *penCaused)
  {
    // If global is not set.
    if (m_eetGlobal == EET_IGNORE) {
      SendToTargetEx(penSendEvent, eetEventType, m_penCaused, m_penTargetArg);
      return;
    }

    // We have a global? Override eventtype!
    SendToTargetEx(penSendEvent, m_eetGlobal, m_penCaused, m_penTargetArg);
  }

  // --------------------------------------------------------------------------------------
  // Returns target 1 (there is no property 'm_penTarget').
  // --------------------------------------------------------------------------------------
  CEntity *GetTarget(void) const
  {
    return m_penTarget1;
  }

  // --------------------------------------------------------------------------------------
  // Returns bytes of memory used by this objectþ
  // --------------------------------------------------------------------------------------
  SLONG GetUsedMemory(void)
  {
    // initial
    SLONG slUsedMemory = sizeof(CSimpleTrigger) - sizeof(CRationalEntity) + CRationalEntity::GetUsedMemory();

    // add some more
    slUsedMemory += m_strName.Length();
    slUsedMemory += 1 * sizeof(CSoundObject);

    return slUsedMemory;
  }

  // --------------------------------------------------------------------------------------
  // Apply the color.
  // --------------------------------------------------------------------------------------
  BOOL AdjustShadingParameters(FLOAT3D &vLightDirection, COLOR &colLight, COLOR &colAmbient)
  {
    colAmbient = m_colColor;
    return true;
  }

  void Stuff(void)
  {
    // if max trig count is used for counting
    if (m_ctMaxTrigs > 0) {
      // decrease count
      m_ctMaxTrigs -= 1;
      // if we trigged max times
      if ( m_ctMaxTrigs <= 0 && m_eMaxTrigsBehavior == ESTMTB_DESTROY) {
        // cease to exist
        Destroy();
      }
    }

    return;
  }

  void UpdateCurrentPos(void)
  {
    CEntityPointer penCurrent;

    do {
      if (m_iPos < 5) {
        m_iPos++;
      }else{
        m_iPos = 1;
      }

      switch(m_iPos)
      {
        case 1: penCurrent = m_penTarget1; break;
        case 2: penCurrent = m_penTarget2; break;
        case 3: penCurrent = m_penTarget3; break;
        case 4: penCurrent = m_penTarget4; break;
        case 5: penCurrent = m_penTarget5; break;
      }
    } while (!penCurrent);

    return;
  }

  void SendToSpecifiedTarget(void)
  {
    switch(m_iPos)
    {
      case 1: SendToTargetM(m_penTarget1, m_eetEvent1, m_penCaused); break;
      case 2: SendToTargetM(m_penTarget2, m_eetEvent2, m_penCaused); break;
      case 3: SendToTargetM(m_penTarget3, m_eetEvent3, m_penCaused); break;
      case 4: SendToTargetM(m_penTarget4, m_eetEvent4, m_penCaused); break;
      case 5: SendToTargetM(m_penTarget5, m_eetEvent5, m_penCaused); break;
    }

    return;
  }

procedures:
  SendEventToTargets()
  {
    // if needed wait some time before event is send
    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime)
      {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
      
      if (m_bDebugMessages) {
        CPrintF(TRANS("[%s] : Finished Awaiting!\n"), m_strName);
      }
    }
    
    if (m_bDebugMessages) {
      CPrintF(TRANS("  Type=Normal\n"));
      CPrintF(TRANS("  Sending event to all targets!\n"));
    }

    // send event to all targets
    SendToTargetM(m_penTarget1, m_eetEvent1, m_penCaused);
    SendToTargetM(m_penTarget2, m_eetEvent2, m_penCaused);
    SendToTargetM(m_penTarget3, m_eetEvent3, m_penCaused);
    SendToTargetM(m_penTarget4, m_eetEvent4, m_penCaused);
    SendToTargetM(m_penTarget5, m_eetEvent5, m_penCaused);

    Stuff();
    return;
  };
  
  Procedural()
  {
    UpdateCurrentPos();

    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
      
      if (m_bDebugMessages) {
        CPrintF(TRANS("[%s] : Finished Awaiting!\n"), m_strName);
      }
    }

    if (m_bDebugMessages) {
      CPrintF(TRANS("  Type=Procedural\n"));
      CPrintF(TRANS("  Sending event to Target %d\n"), m_iPos);
    }

    SendToSpecifiedTarget();
    Stuff();
    return;
  }

  // --------------------------------------------------------------------------------------
  // The random trigger state.
  // --------------------------------------------------------------------------------------
  Random()
  {
    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
      
      if (m_bDebugMessages) {
        CPrintF(TRANS("[%s] : Finished Awaiting!\n"), m_strName);
      }
    }

    m_iPos = IRnd() % 5 + 1;

    if (m_bDebugMessages) {
      CPrintF(TRANS("  Type=Random\n"));
      CPrintF(TRANS("  Sending event to Target %d\n"), m_iPos);
    }

    SendToSpecifiedTarget();
    Stuff();
    return;
  }

  // --------------------------------------------------------------------------------------
  // The active state.
  // --------------------------------------------------------------------------------------
  Active()
  {
    //main loop
    wait()
    {
      on (EBegin) : {
        // if auto start send event on init
        if (m_bActive && m_bAutoStart) {
          if (m_eTType == STT_NORMAL) {
            call SendEventToTargets();
          } else if (m_eTType == STT_PROCEDURAL) {
            call Procedural();
          } else if (m_eTType == STT_RANDOM) {
            call Random();
          }
        }
        resume;
      }

      // re-roots start events as triggers
      on (EStart eStart) : {
        SendToTarget(this, EET_TRIGGER, eStart.penCaused); // [SSE] - Here must be SendToTarget not SendToTargetM!!!
        resume;
      }

      // cascade trigger
      on (ETrigger eTrigger) :
      {
        if (!m_bActive) {
          resume;
        }

        if (m_bDebugMessages) {
          if (eTrigger.penCaused != NULL) {
            CPrintF("[%s] : Received ETrigger(-> %d)\n", m_strName, eTrigger.penCaused->en_ulID);
          } else {
            CPrintF("[%s] : Received ETrigger(-> NULL)\n", m_strName);
          }
          
          if (eTrigger.penCaused) {
            CPrintF("  penCaused.Name = '%s'\n", eTrigger.penCaused->GetName().Undecorated());
          }
        }

        if (m_ctMaxTrigs > 0 || m_ctMaxTrigs == -1) {
          m_penCaused = eTrigger.penCaused;
          m_penTargetArg = NULL;

          if (m_eTType == STT_NORMAL) {
            call SendEventToTargets();
          } else if (m_eTType == STT_PROCEDURAL) {
            call Procedural();
          } else if (m_eTType == STT_RANDOM) {
            call Random();
          }
        }

        resume;
      }

      // [SSE] Entities - Targeted Event
      on (ETargeted eTargeted) :
      {
        if (!m_bActive) {
          resume;
        }

        if (m_bDebugMessages) {
          CPrintF("[%s] : Received ETargeted(-> %d, -> %d)\n", m_strName, eTargeted.penCaused ? eTargeted.penCaused->en_ulID : 0, eTargeted.penTarget ? eTargeted.penTarget->en_ulID : 0);
          
          if (eTargeted.penCaused) {
            CPrintF("  penCaused.Name = '%s'\n", eTargeted.penCaused->GetName().Undecorated());
          }
          
          if (eTargeted.penTarget) {
            CPrintF("  penTarget.Name = '%s'\n", eTargeted.penTarget->GetName().Undecorated());
          }
        }

        if (m_ctMaxTrigs > 0 || m_ctMaxTrigs == -1) {
          m_penCaused = eTargeted.penCaused;
          m_penTargetArg = eTargeted.penTarget;

          if (m_eTType == STT_NORMAL) {
            call SendEventToTargets();
          } else if (m_eTType == STT_PROCEDURAL) {
            call Procedural();
          } else if (m_eTType == STT_RANDOM) {
            call Random();
          }
        }

        resume;
      }
    }
  };

  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_MARKER);
    SetModelMainTexture(TEXTURE_MARKER);

    // spawn in world editor
    autowait(0.1F);

    jump Active();

    // cease to exist
    Destroy();

    return;
  };
};