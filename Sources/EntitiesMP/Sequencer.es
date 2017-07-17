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

229
%{
  #include "StdH.h"
%}

enum ESeqType {
  0 ESEQT_NORMAL      "All from 1st [0]",
  1 ESEQT_RANDOM      "All from Random [1]",
  2 ESEQT_RANDOM_ONE  "Random One [2]",
};

class CSequencer : CRationalEntity {
name      "Sequencer";
thumbnail "Thumbnails\\Sequencer.tbn";
features  "HasName", "IsTargetable";

properties:
  // Commons
   1 CTString m_strName "Name" 'N'      = "Sequencer",
   3 CTString m_strDescription = "",
   4 BOOL m_bDebugMessages "Debug Messages" = FALSE,
   5 enum ESeqType m_etType "Type" = ESEQT_NORMAL,
   6 BOOL m_bActive                "Active" 'V' = TRUE,
   7 BOOL m_bAutoStart             "Auto start" 'A' = FALSE,
   
   // Additional Stuff
  10 INDEX m_ctMaxTrigs              "Max trigs" 'X' = -1, 
  11 BOOL m_bDestroyOnMaxTriggs      "Destroy on Max Triggs" = TRUE,
  12 FLOAT m_fMinTime                "Min Time between Trigs" = 0.0f,
   
  // Globalizers
  20 FLOAT m_tmDelayEach             "Delay Each" 'D' = 0.0f,
  21 enum EventEType m_eetEach       "Event Type Each" = EET_IGNORE,
  
  // Targets
  30 CEntityPointer m_penTarget01    "Target 01" 'T',
  31 CEntityPointer m_penTarget02    "Target 02" 'Y',
  32 CEntityPointer m_penTarget03    "Target 03" 'U',
  33 CEntityPointer m_penTarget04    "Target 04" 'I',
  34 CEntityPointer m_penTarget05    "Target 05" 'O',
  35 CEntityPointer m_penTarget06    "Target 06",
  36 CEntityPointer m_penTarget07    "Target 07",
  37 CEntityPointer m_penTarget08    "Target 08",
  38 CEntityPointer m_penTarget09    "Target 09",
  39 CEntityPointer m_penTarget10    "Target 10",
  
  // Event Types
  40 enum EventEType m_eetEvent01    "Event Type Target 01" 'G' = EET_TRIGGER,  // type of event to send
  41 enum EventEType m_eetEvent02    "Event Type Target 02" 'H' = EET_TRIGGER,
  42 enum EventEType m_eetEvent03    "Event Type Target 03" 'J' = EET_TRIGGER,
  43 enum EventEType m_eetEvent04    "Event Type Target 04" 'K' = EET_TRIGGER,
  44 enum EventEType m_eetEvent05    "Event Type Target 05" 'L' = EET_TRIGGER,
  45 enum EventEType m_eetEvent06    "Event Type Target 06" = EET_TRIGGER,
  46 enum EventEType m_eetEvent07    "Event Type Target 07" = EET_TRIGGER,
  47 enum EventEType m_eetEvent08    "Event Type Target 08" = EET_TRIGGER,
  48 enum EventEType m_eetEvent09    "Event Type Target 09" = EET_TRIGGER,
  49 enum EventEType m_eetEvent10    "Event Type Target 10" = EET_TRIGGER,

  // Delays
  50 FLOAT m_fDelay01                "Delay 01" = 0.0F,
  51 FLOAT m_fDelay02                "Delay 02" = 0.0F,
  52 FLOAT m_fDelay03                "Delay 03" = 0.0F,
  53 FLOAT m_fDelay04                "Delay 04" = 0.0F,
  54 FLOAT m_fDelay05                "Delay 05" = 0.0F,
  55 FLOAT m_fDelay06                "Delay 06" = 0.0F,
  56 FLOAT m_fDelay07                "Delay 07" = 0.0F,
  57 FLOAT m_fDelay08                "Delay 08" = 0.0F,
  58 FLOAT m_fDelay09                "Delay 09" = 0.0F,
  59 FLOAT m_fDelay10                "Delay 10" = 0.0F,

  //
  94 INDEX m_iPos = 0,
 140 FLOAT m_tmLastTriggered = 0.0f,
 150 CEntityPointer m_penCaused,
  
components:
  1 model   MODEL_SEQUENCER   "Models\\Editor\\Sequencer.mdl",
  2 texture TEXTURE_SEQUENCER "Models\\Editor\\Sequencer.tex",

functions:
  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const {
    return m_strDescription;
  }
  
  // --------------------------------------------------------------------------------------
  // Just extended SendToTarget()
  // --------------------------------------------------------------------------------------
  void SendToTargetM(CEntity *penSendEvent, EventEType eetEventType, CEntity *penCaused) {
    if (m_eetEach == EET_IGNORE) {
      SendToTarget(penSendEvent, eetEventType, m_penCaused);
      return;
    }

    // If we have a globalizer then override EventType!
    SendToTarget(penSendEvent, m_eetEach, m_penCaused);
  }

  // --------------------------------------------------------------------------------------
  // Sends event to target by its index.
  // --------------------------------------------------------------------------------------
  void SendToSpecifiedTarget(void)
  {
    const CEntityPointer* apenTargets = &m_penTarget01;
    const EventEType* aeetEvents = &m_eetEvent01;
    
    if (m_iPos < 0 || m_iPos > 9) {
      return;
    }
    
    SendToTargetM(apenTargets[m_iPos], aeetEvents[m_iPos], m_penCaused);
  }
  
  // --------------------------------------------------------------------------------------
  // Post-actions for every type of Sequencer.
  // --------------------------------------------------------------------------------------
  void Stuff()
  {
    // if max trig count is used for counting
    if (m_ctMaxTrigs > 0) {
      // decrease count
      m_ctMaxTrigs -= 1;
      // if we trigged max times
      if ( m_ctMaxTrigs <= 0 && m_bDestroyOnMaxTriggs) {
        // cease to exist
        Destroy();
      }
    }
  }
  
procedures:
  // --------------------------------------------------------------------------------------
  // In active state entity acts like it must act.
  // --------------------------------------------------------------------------------------
  Active() {
    ASSERT(m_bActive);

    //main loop
    wait() {
      on (EBegin) : {
        // If auto start send event on init
        if (m_bAutoStart) {
          if (m_etType == ESEQT_NORMAL) {
            call Normal();
          } else if (m_etType == ESEQT_RANDOM) {
            call Random();
          } else if (m_etType == ESEQT_RANDOM_ONE) {
            call RandomOne();
          }
        }
        resume;
      }

      // re-roots start events as triggers
      on (EStart eStart) : {
        SendToTargetM(this, EET_TRIGGER, eStart.penCaused);
        resume;
      }

      // cascade trigger
      on (ETrigger eTrigger) : {
        if (m_fMinTime > 0 && m_tmLastTriggered != 0 && (_pTimer->CurrentTick() - m_fMinTime) < m_tmLastTriggered) {
          if (m_bDebugMessages) {
            CPrintF(TRANS("%s : received Trigger event, but min Time was not reached\n"), m_strName);
          }
          resume;
        }

        m_tmLastTriggered = _pTimer->CurrentTick();

        if (m_bDebugMessages) {
          if (eTrigger.penCaused != NULL) {
            CPrintF(TRANS("%s : triggered through %s\n"), m_strName, eTrigger.penCaused->GetName());
          }else{
            CPrintF(TRANS("%s : triggered through unknown entity\n"), m_strName);
          }
        }

        m_penCaused = eTrigger.penCaused;
        
        if (m_etType == ESEQT_NORMAL) {
          call Normal();
        } else if (m_etType == ESEQT_RANDOM) {
          call Random();
        } else if (m_etType == ESEQT_RANDOM_ONE) {
          call RandomOne();
        }
        
        resume;
      }

      // if deactivated
      on (EDeactivate) : {
        // go to inactive state
        m_bActive = FALSE;
        jump Inactive();
      }
    }
  };

  // --------------------------------------------------------------------------------------
  // In inactive state entity ignores all events except activation event.
  // --------------------------------------------------------------------------------------
  Inactive() {
    ASSERT(!m_bActive);
    while (TRUE) {
      // wait
      wait() {
        // if activated
        on (EActivate) : {
          // go to active state
          m_bActive = TRUE;
          jump Active();
        }
        otherwise() : {
          resume;
        };
      };

      // wait a bit to recover
      autowait(0.1f);
    }
  }

  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_SEQUENCER);
    SetModelMainTexture(TEXTURE_SEQUENCER);
    
    // spawn in world editor
    autowait(0.1F);

    // go into active or inactive state
    if (m_bActive) {
      jump Active();
    } else {
      jump Inactive();
    }
    
    // cease to exist
    Destroy();
  
    return;
  }

  // --------------------------------------------------------------------------------------
  // Starts and executes sequence from first target.
  // --------------------------------------------------------------------------------------
  Normal()
  {
    m_iPos = 0;

    if (m_bDebugMessages) {
      CPrintF(TRANS("  Type=AllFromFirst\n"));
      CPrintF(TRANS("  Starting sequence from Target %d\n"), 1);
    }

    // Cycle through all targets...
    while (m_iPos < 10) {
      const FLOAT *afDelays = &m_fDelay01;

      // If we have delay for target then do it!
      if ((afDelays[m_iPos] + m_tmDelayEach) > 0.0f) {
        wait (afDelays[m_iPos] + m_tmDelayEach) {
          on (EBegin) : { resume; }
          on (ETimer) : { stop; }
          on (EDeactivate) : { pass; }
          otherwise(): { resume; }
        }
      }

      SendToSpecifiedTarget();
      //CPrintF("  KURWA %d \n", m_iPos+1);
      m_iPos++;
    }
    
    Stuff();
    
    return;
  }

  // --------------------------------------------------------------------------------------
  // Selects one of 10 targets and starting sequence from it.
  // --------------------------------------------------------------------------------------
  Random()
  {
    m_iPos = IRnd()%10;

    if (m_bDebugMessages) {
      CPrintF(TRANS("  Type=AllFromRandom\n"));
      CPrintF(TRANS("  Starting sequence from Target %d\n"), m_iPos + 1);
    }

    // Cycle through all targets...
    while (m_iPos < 10) {
      const FLOAT *afDelays = &m_fDelay01;

      // If we have delay for target then do it!
      if ((afDelays[m_iPos] + m_tmDelayEach) > 0.0f) {
        wait (afDelays[m_iPos] + m_tmDelayEach) {
          on (EBegin) : { resume; }
          on (ETimer) : { stop; }
          on (EDeactivate) : { pass; }
          otherwise(): { resume; }
        }
      }

      SendToSpecifiedTarget();
      //CPrintF("  KURWA %d \n", m_iPos+1);
      m_iPos++;
    }
    
    Stuff();
    
    return;
  }

  // --------------------------------------------------------------------------------------
  // Selects one of 10 targets and sending event to it.
  // --------------------------------------------------------------------------------------
  RandomOne()
  {
    const FLOAT *afDelays = &m_fDelay01;

    m_iPos = IRnd()%10;
    
    if (m_bDebugMessages) {
      CPrintF(TRANS("  Type=RandomOne\n"));
      CPrintF(TRANS("  Selected Target=%d\n"), m_iPos + 1);
    }

    // If we have delay for specified target then delay this shit.
    if ((afDelays[m_iPos] + m_tmDelayEach) > 0.0f) {
      if (m_bDebugMessages) {
        CPrintF(TRANS("  Awaiting delay to send event to Target %d\n"), m_iPos + 1);
      }

      wait (afDelays[m_iPos] + m_tmDelayEach) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    } else {
      if (m_bDebugMessages) {
        CPrintF(TRANS("  Sending event to Target %d\n"), m_iPos + 1);
      }
    }

    SendToSpecifiedTarget();
    Stuff();

    return;
  }
};
