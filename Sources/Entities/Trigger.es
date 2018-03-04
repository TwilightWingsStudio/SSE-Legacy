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

/*
  NOTE: New procedures must be after the Main().
  If you change this order you lose compatibility with the old maps.

  If you need to use only this entity from SSE addon don't forget
  about piece of code related to m_bCountAsSecret in the MusicHolder
  entity.
*/

205
%{
  #include "StdH.h"
  extern INDEX ent_bReportBrokenChains;
%}

enum ETType {
  0 TT_NORMAL     "Normal [0]",
  1 TT_DELAYED    "Delayed [1]",
  2 TT_PROCEDURAL "Procedural [2]",
  3 TT_RANDOM     "Random [3]",
  4 TT_DELRAND    "Delayed randomly [4]",
};

class CTrigger: CRationalEntity {
name      "Trigger";
thumbnail "Thumbnails\\Trigger.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName              "Name" 'N' = "Trigger",         // class name
   2 CTString m_strDescription = "",

   3 CEntityPointer m_penTarget1     "Target 01" 'T' COLOR(C_RED|0xFF),                 // send event to entity
   4 CEntityPointer m_penTarget2     "Target 02" 'Y' COLOR(C_RED|0xFF),
   5 CEntityPointer m_penTarget3     "Target 03" 'U' COLOR(C_RED|0xFF),
   6 CEntityPointer m_penTarget4     "Target 04" 'I' COLOR(C_RED|0xFF),
   7 CEntityPointer m_penTarget5     "Target 05" 'O' COLOR(C_RED|0xFF),
  20 CEntityPointer m_penTarget6     "Target 06" COLOR(C_RED|0xFF),
  21 CEntityPointer m_penTarget7     "Target 07" COLOR(C_RED|0xFF),
  22 CEntityPointer m_penTarget8     "Target 08" COLOR(C_RED|0xFF),
  23 CEntityPointer m_penTarget9     "Target 09" COLOR(C_RED|0xFF),
  24 CEntityPointer m_penTarget10    "Target 10" COLOR(C_RED|0xFF),
   8 enum EventEType m_eetEvent1     "Event type Target 01" 'G' = EET_TRIGGER,  // type of event to send
   9 enum EventEType m_eetEvent2     "Event type Target 02" 'H' = EET_TRIGGER,
  10 enum EventEType m_eetEvent3     "Event type Target 03" 'J' = EET_TRIGGER,
  11 enum EventEType m_eetEvent4     "Event type Target 04" 'K' = EET_TRIGGER,
  12 enum EventEType m_eetEvent5     "Event type Target 05" 'L' = EET_TRIGGER,
  50 enum EventEType m_eetEvent6     "Event type Target 06" = EET_TRIGGER,
  51 enum EventEType m_eetEvent7     "Event type Target 07" = EET_TRIGGER,
  52 enum EventEType m_eetEvent8     "Event type Target 08" = EET_TRIGGER,
  53 enum EventEType m_eetEvent9     "Event type Target 09" = EET_TRIGGER,
  54 enum EventEType m_eetEvent10    "Event type Target 10" = EET_TRIGGER,
  13 CTStringTrans m_strMessage      "Message" 'M' = "",     // message
  14 FLOAT m_fMessageTime            "Message time" = 3.0f,  // how long is message on screen
  15 enum MessageSound m_mssMessageSound "Message sound" = MSS_NONE, // message sound
  16 FLOAT m_fScore                  "Score" 'S' = 0.0f,

  30 FLOAT m_fWaitTime             "Wait" 'W' = 0.0f,          // wait before send events
  31 BOOL m_bAutoStart             "Auto start" 'A' = FALSE,   // trigger auto starts
  32 INDEX m_iCount                "Count" 'C' = 1,            // count before send events
  33 BOOL m_bUseCount              "Count use" = FALSE,        // use count to send events
  34 BOOL m_bReuseCount            "Count reuse" = FALSE,      // reuse counter after reaching 0
  35 BOOL m_bTellCount             "Count tell" = FALSE,       // tell remaining count to player
  36 BOOL m_bActive                "Active" 'V' = TRUE,        // starts in active/inactive state
  37 RANGE m_fSendRange            "Send Range" 'R' = 1.0f,    // for sending event in range
  38 enum EventEType m_eetRange    "Event type Range" = EET_IGNORE,  // type of event to send in range

  40 INDEX m_iCountTmp = 0,          // use count value to determine when to send events
  41 CEntityPointer m_penCaused,     // who touched it last time
  42 INDEX m_ctMaxTrigs            "Max trigs" 'X' = -1, // how many times could trig

  45 COLOR m_colColor              "Trigger Color" = C_WHITE,

  55 enum EventEType m_eetGlobal   "Event type Global" = EET_IGNORE,

  93 enum ETType m_eTType  "Type" = TT_NORMAL,
  94 INDEX m_iPos                 = 0,

  95 BOOL m_bDebugMessages        "Debug Messages" = FALSE,

 100 CTString m_strCausedOnlyByClass "Triggerable only by class" = "",

 106 BOOL m_bMessageForAll            "Message for all Players" = FALSE,
 107 BOOL m_bMessageShowTriggererName "Message %s=penCaused" = FALSE,

 110 FLOAT m_tmLastTriggered = 0.0f,
 111 FLOAT m_fMinTime           "Min Time between Trigs" = 0.0f,
 112 FLOAT m_fRandDelayFactor   "Random delay factor"    = 0.5F,
 113 FLOAT m_fWaitInternal = 1.0f,

 122 BOOL m_bDestroyOnMaxTriggs "Destroy on Max Triggs" = TRUE,

 125 CTString m_strTellCountMsg "Count tell message" = "%d more to go...",

 130 BOOL m_bCountAsSecret "Count as secret" = TRUE,
 131 BOOL m_bGiveScoreOnce "Give score once" = TRUE,

components:
  1 model   MODEL_MARKER     "Models\\Editor\\Trigger.mdl",
  2 texture TEXTURE_MARKER   "Models\\Editor\\Camera.tex"

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
    INDEX ctUsed = 0;

    if (m_penTarget1)  { ctUsed++; }
    if (m_penTarget2)  { ctUsed++; }
    if (m_penTarget3)  { ctUsed++; }
    if (m_penTarget4)  { ctUsed++; }
    if (m_penTarget5)  { ctUsed++; }
    if (m_penTarget6)  { ctUsed++; }
    if (m_penTarget7)  { ctUsed++; }
    if (m_penTarget8)  { ctUsed++; }
    if (m_penTarget9)  { ctUsed++; }
    if (m_penTarget10) { ctUsed++; }
    
    ((CTString&)m_strDescription).PrintF("[%d/10]", ctUsed);
    
    return m_strDescription;
  }
  
  // --------------------------------------------------------------------------------------
  // Returns number of used targets. For debugging stuff.
  // --------------------------------------------------------------------------------------
  INDEX GetNumbersOfUsedTargets()
  {
    INDEX ctUsed = 0;
    
    if (m_penTarget1  && m_eetEvent1  != EET_IGNORE)   { ctUsed++; }
    if (m_penTarget2  && m_eetEvent2  != EET_IGNORE)   { ctUsed++; }
    if (m_penTarget3  && m_eetEvent3  != EET_IGNORE)   { ctUsed++; }
    if (m_penTarget4  && m_eetEvent4  != EET_IGNORE)   { ctUsed++; }
    if (m_penTarget5  && m_eetEvent5  != EET_IGNORE)   { ctUsed++; }
    if (m_penTarget6  && m_eetEvent6  != EET_IGNORE)   { ctUsed++; }
    if (m_penTarget7  && m_eetEvent7  != EET_IGNORE)   { ctUsed++; }
    if (m_penTarget8  && m_eetEvent8  != EET_IGNORE)   { ctUsed++; }
    if (m_penTarget9  && m_eetEvent9  != EET_IGNORE)   { ctUsed++; }
    if (m_penTarget10 && m_eetEvent10 != EET_IGNORE) { ctUsed++; }
    
    return ctUsed;
  }
  
  void SendToTargetM(CEntity *penSendEvent, EventEType eetEventType, CEntity *penCaused)
  {
    if (m_eetGlobal == EET_IGNORE) {
      SendToTarget(penSendEvent, eetEventType, m_penCaused);
      return;
    }
    // We have a global? Override eventtype!
    SendToTarget(penSendEvent, m_eetGlobal, m_penCaused);
  }

  // --------------------------------------------------------------------------------------
  // get target 1 (there is no property 'm_penTarget')
  // --------------------------------------------------------------------------------------
  CEntity *GetTarget(void) const
  {
    return m_penTarget1;
  }

  // --------------------------------------------------------------------------------------
  // Returns bytes of memory used by this object.
  // --------------------------------------------------------------------------------------
  SLONG GetUsedMemory(void)
  {
    // initial
    SLONG slUsedMemory = sizeof(CTrigger) - sizeof(CRationalEntity) + CRationalEntity::GetUsedMemory();
    // add some more
    slUsedMemory += m_strMessage.Length();
    slUsedMemory += m_strName.Length();
    slUsedMemory += 1* sizeof(CSoundObject);
    slUsedMemory += m_strCausedOnlyByClass.Length();
    slUsedMemory += m_strTellCountMsg.Length();
    return slUsedMemory;
  }

  BOOL AdjustShadingParameters(FLOAT3D &vLightDirection, COLOR &colLight, COLOR &colAmbient)
  {
    colAmbient = m_colColor;
    return true;
  }

  void Stuff(void)
  {
    // if there is event to send in range
    if (m_eetRange != EET_IGNORE) {
      SendInRange(this, m_eetRange, FLOATaabbox3D(GetPlacement().pl_PositionVector, m_fSendRange));
    }

    // if trigger gives/steals score
    if (m_fScore != 0) {
      CEntity *penCaused = FixupCausedToPlayer(this, m_penCaused);

      // if we have causer
      if (penCaused != NULL) {
        // send the score
        EReceiveScore eScore;
        eScore.iPoints = m_fScore;
        penCaused->SendEvent(eScore);

        if (m_bCountAsSecret) {
          penCaused->SendEvent(ESecretFound());
        }
      }

      // kill score to never be reported again
      // m_bCountAsSecret because multiple calls can break the statistics calculation.
      if (m_bGiveScoreOnce || m_bCountAsSecret) {
        m_fScore = 0;
      }
    }

    if (m_strMessage != "") {
      CEntity *penCaused = FixupCausedToPlayer(this, m_penCaused);

      CTString strCookedMessage;

      if (m_bMessageShowTriggererName) {
        strCookedMessage.PrintF(TranslateConst(m_strMessage), (penCaused ? penCaused->GetName() : "<null>"));
      } else {
        strCookedMessage = TranslateConst(m_strMessage);
      }

      if (m_bMessageForAll) {
        for(INDEX i = 0; i < GetMaxPlayers(); i++) {
          CEntity *penPlayer = GetPlayerEntity(i);
          if (penPlayer != NULL) {
            PrintCenterMessage(this, penPlayer,
            strCookedMessage,
            m_fMessageTime, m_mssMessageSound);
          }
        }
      } else {
        PrintCenterMessage(this, m_penCaused,
          strCookedMessage,
          m_fMessageTime, m_mssMessageSound);
      }
    }

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

    return;
  }

  void UpdateCurrentPos(void)
  {
    CEntityPointer penCurrent;

    do {
      if (m_iPos < 10) {
        m_iPos++;
      } else {
        m_iPos = 1;
      }

      switch(m_iPos) {
        case 1: penCurrent = m_penTarget1; break;
        case 2: penCurrent = m_penTarget2; break;
        case 3: penCurrent = m_penTarget3; break;
        case 4: penCurrent = m_penTarget4; break;
        case 5: penCurrent = m_penTarget5; break;
        case 6: penCurrent = m_penTarget6; break;
        case 7: penCurrent = m_penTarget7; break;
        case 8: penCurrent = m_penTarget8; break;
        case 9: penCurrent = m_penTarget9; break;
        case 10: penCurrent = m_penTarget10; break;
      }
    } while (!penCurrent);

    return;
  }

  void SendToSpecifiedTarget(void)
  {
    switch (m_iPos)
    {
      case 1: SendToTargetM(m_penTarget1, m_eetEvent1, m_penCaused); break;
      case 2: SendToTargetM(m_penTarget2, m_eetEvent2, m_penCaused); break;
      case 3: SendToTargetM(m_penTarget3, m_eetEvent3, m_penCaused); break;
      case 4: SendToTargetM(m_penTarget4, m_eetEvent4, m_penCaused); break;
      case 5: SendToTargetM(m_penTarget5, m_eetEvent5, m_penCaused); break;
      case 6: SendToTargetM(m_penTarget6, m_eetEvent6, m_penCaused); break;
      case 7: SendToTargetM(m_penTarget7, m_eetEvent7, m_penCaused); break;
      case 8: SendToTargetM(m_penTarget8, m_eetEvent8, m_penCaused); break;
      case 9: SendToTargetM(m_penTarget9, m_eetEvent9, m_penCaused); break;
      case 10: SendToTargetM(m_penTarget10, m_eetEvent10, m_penCaused); break;
    }

    return;
  }

procedures:
  SendEventToTargets()
  {
    // if needed wait some time before event is send
    if (m_fWaitTime > 0.0f)
    {
      if (m_bDebugMessages) {
        CPrintF("[%s] : Processing delay %.2f second(s).\n", m_strName, m_fWaitTime);
      }
      
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    
    if (m_bDebugMessages)
    {
      INDEX ctUsed = GetNumbersOfUsedTargets();

      if (ctUsed == 0) {
        CPrintF("[%s] : Haven't targets to send events.\n", m_strName);
      } else {
        CPrintF("[%s] : Sending events to %d targets.\n", m_strName, ctUsed);
      }
    }

    // send event to all targets
    SendToTargetM(m_penTarget1, m_eetEvent1, m_penCaused);
    SendToTargetM(m_penTarget2, m_eetEvent2, m_penCaused);
    SendToTargetM(m_penTarget3, m_eetEvent3, m_penCaused);
    SendToTargetM(m_penTarget4, m_eetEvent4, m_penCaused);
    SendToTargetM(m_penTarget5, m_eetEvent5, m_penCaused);
    SendToTargetM(m_penTarget6, m_eetEvent6, m_penCaused);
    SendToTargetM(m_penTarget7, m_eetEvent7, m_penCaused);
    SendToTargetM(m_penTarget8, m_eetEvent8, m_penCaused);
    SendToTargetM(m_penTarget9, m_eetEvent9, m_penCaused);
    SendToTargetM(m_penTarget10, m_eetEvent10, m_penCaused);

    Stuff();
    return;
  };

  Active()
  {
    ASSERT(m_bActive);
    // store count start value
    m_iCountTmp = m_iCount;

    //main loop
    wait()
    {
      on (EBegin) : {
        // if auto start send event on init
        if (m_bAutoStart)
        {
          if (m_bDebugMessages) {
            CPrintF(TRANS("[%s] Autostarted!\n"), m_strName);
          }
          
          if (m_eTType == TT_NORMAL) {
            call SendEventToTargets();
          } else if (m_eTType == TT_DELAYED) {
            call Delayed();
          } else if (m_eTType == TT_PROCEDURAL) {
            call Procedural();
          } else if (m_eTType == TT_RANDOM) {
            call Random();
          } else if (m_eTType == TT_DELRAND) {
            call DelayedRandomly();
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
      on (ETrigger eTrigger) : {
        if (m_fMinTime > 0 && m_tmLastTriggered != 0 && (_pTimer->CurrentTick() - m_fMinTime) < m_tmLastTriggered) {
          if (m_bDebugMessages) {
            CPrintF(TRANS("[%s] : received Trigger event, but min Time was not reached\n"), m_strName);
          }
          resume;
        }

        if (!(m_strCausedOnlyByClass == "" || IsOfClass(eTrigger.penCaused, m_strCausedOnlyByClass))) {
          if (m_bDebugMessages) {
            CPrintF(TRANS("[%s] : received Trigger event, but from wrong class\n"),m_strName);
          }
          resume;
        }

        m_tmLastTriggered = _pTimer->CurrentTick();

        if (m_bDebugMessages) {
          if (eTrigger.penCaused != NULL) {
            CPrintF(TRANS("[%s] : triggered by %s\n"), m_strName, eTrigger.penCaused->GetName());
          } else {
            CPrintF(TRANS("[%s] : triggered by unknown entity\n"), m_strName);
          }
        }

        if (m_ctMaxTrigs > 0 || m_ctMaxTrigs == -1) {
          m_penCaused = eTrigger.penCaused;
          // if using count
          if (m_bUseCount) {
            // count reach lowest value
            if (m_iCountTmp > 0) {
              // decrease count
              m_iCountTmp--;
              // send event if count is less than one (is zero)
              if (m_iCountTmp < 1) {
                if (m_bReuseCount) {
                  m_iCountTmp = m_iCount;
                } else {
                  m_iCountTmp = 0;
                }
                if (m_eTType == TT_NORMAL) {
                  call SendEventToTargets();
                } else if (m_eTType == TT_DELAYED) {
                  call Delayed();
                } else if (m_eTType == TT_PROCEDURAL) {
                  call Procedural();
                } else if (m_eTType == TT_RANDOM) {
                  call Random();
                } else if (m_eTType == TT_DELRAND) {
                  call DelayedRandomly();
                }
              } else if (m_bTellCount && m_strTellCountMsg != "") {
                CTString strRemaining = TranslateConst(m_strTellCountMsg);
                CTString strReplace, strReplace2;

                strReplace.PrintF("%d", m_iCountTmp);
                strReplace2.PrintF("%d", m_iCount - m_iCountTmp);

                while (strRemaining.ReplaceSubstr("%d", strReplace)) {}
                while(strRemaining.ReplaceSubstr("%D",strReplace2)) {}
                PrintCenterMessage(this, m_penCaused, strRemaining, 3.0f, MSS_INFO);
              }
            }
          // else send event
          } else {
            if (m_eTType == TT_NORMAL) {
              call SendEventToTargets();
            } else if (m_eTType == TT_DELAYED) {
              call Delayed();
            } else if (m_eTType == TT_PROCEDURAL) {
              call Procedural();
            } else if (m_eTType == TT_RANDOM) {
              call Random();
            } else if (m_eTType == TT_DELRAND) {
              call DelayedRandomly();
            }
          }
        }
        resume;
      }

      // if deactivated
      on (EDeactivate) : {
        // go to inactive state
        m_bActive = FALSE;
        jump Inactive();
      }

      // [SSE] Entities - Targeted Event
      on (ETargeted eTargeted) :
      { 
        SendToTarget(this, EET_TRIGGER, eTargeted.penCaused);
        
        resume;
      }
    }
  };

  Inactive()
  {
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
    SetModel(MODEL_MARKER);
    SetModelMainTexture(TEXTURE_MARKER);

    m_fSendRange = ClampDn(m_fSendRange, 0.01F);
    m_fRandDelayFactor = Clamp(m_fRandDelayFactor, 0.0F, 1.0F);

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
  };

  Delayed()
  {
    if (m_bDebugMessages) {
      CPrintF(TRANS("  Type=Delayed\n"));
    }

    Stuff();

    // if needed wait some time before event is send
    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget1, m_eetEvent1, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget2, m_eetEvent2, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget3, m_eetEvent3, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget4, m_eetEvent4, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget5, m_eetEvent5, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget6, m_eetEvent6, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget7, m_eetEvent7, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget8, m_eetEvent8, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget9, m_eetEvent9, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      wait (m_fWaitTime) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget10, m_eetEvent10, m_penCaused);

    return;
  };

  DelayedRandomly()
  {
    if (m_bDebugMessages) {
      CPrintF(TRANS("  Type=DelayedRandomly\n"));
    }

    Stuff();

    // if needed wait some time before event is send
    if (m_fWaitTime > 0.0f) {
      m_fWaitInternal=m_fWaitTime+((FRnd()*m_fWaitTime*m_fRandDelayFactor)*((FRnd()>0.5f)?1.0f:-1.0f));
      if (m_fWaitInternal<=0.0f) { m_fWaitInternal=0.05f; }

      wait (m_fWaitInternal) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget1, m_eetEvent1, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      m_fWaitInternal=m_fWaitTime+((FRnd()*m_fWaitTime*m_fRandDelayFactor)*((FRnd()>0.5f)?1.0f:-1.0f));
      if (m_fWaitInternal<=0.0f) { m_fWaitInternal=0.05f; }

        wait (m_fWaitInternal) {
          on (EBegin) : { resume; }
          on (ETimer) : { stop; }
          on (EDeactivate) : { pass; }
          otherwise(): { resume; }
        }
    }
    SendToTargetM(m_penTarget2, m_eetEvent2, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      m_fWaitInternal=m_fWaitTime+((FRnd()*m_fWaitTime*m_fRandDelayFactor)*((FRnd()>0.5f)?1.0f:-1.0f));
      if (m_fWaitInternal<=0.0f) { m_fWaitInternal=0.05f; }

      wait (m_fWaitInternal) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget3, m_eetEvent3, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      m_fWaitInternal=m_fWaitTime+((FRnd()*m_fWaitTime*m_fRandDelayFactor)*((FRnd()>0.5f)?1.0f:-1.0f));
      if (m_fWaitInternal<=0.0f) { m_fWaitInternal=0.05f; }

      wait (m_fWaitInternal) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget4, m_eetEvent4, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      m_fWaitInternal=m_fWaitTime+((FRnd()*m_fWaitTime*m_fRandDelayFactor)*((FRnd()>0.5f)?1.0f:-1.0f));
      if (m_fWaitInternal<=0.0f) { m_fWaitInternal=0.05f; }

      wait (m_fWaitInternal) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget5, m_eetEvent5, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      m_fWaitInternal=m_fWaitTime+((FRnd()*m_fWaitTime*m_fRandDelayFactor)*((FRnd()>0.5f)?1.0f:-1.0f));
      if (m_fWaitInternal<=0.0f) { m_fWaitInternal=0.05f; }

      wait (m_fWaitInternal) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget6, m_eetEvent6, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      m_fWaitInternal=m_fWaitTime+((FRnd()*m_fWaitTime*m_fRandDelayFactor)*((FRnd()>0.5f)?1.0f:-1.0f));
      if (m_fWaitInternal<=0.0f) { m_fWaitInternal=0.05f; }

      wait (m_fWaitInternal) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget7, m_eetEvent7, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      m_fWaitInternal=m_fWaitTime+((FRnd()*m_fWaitTime*m_fRandDelayFactor)*((FRnd()>0.5f)?1.0f:-1.0f));
      if (m_fWaitInternal<=0.0f) { m_fWaitInternal=0.05f; }

      wait (m_fWaitInternal) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget8, m_eetEvent8, m_penCaused);

    if (m_fWaitTime > 0.0f) {
    m_fWaitInternal=m_fWaitTime+((FRnd()*m_fWaitTime*m_fRandDelayFactor)*((FRnd()>0.5f)?1.0f:-1.0f));
      if (m_fWaitInternal<=0.0f) { m_fWaitInternal=0.05f; }

      wait (m_fWaitInternal) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget9, m_eetEvent9, m_penCaused);

    if (m_fWaitTime > 0.0f) {
      m_fWaitInternal=m_fWaitTime+((FRnd()*m_fWaitTime*m_fRandDelayFactor)*((FRnd()>0.5f)?1.0f:-1.0f));
      if (m_fWaitInternal<=0.0f) { m_fWaitInternal=0.05f; }
      wait (m_fWaitInternal) {
        on (EBegin) : { resume; }
        on (ETimer) : { stop; }
        on (EDeactivate) : { pass; }
        otherwise(): { resume; }
      }
    }
    SendToTargetM(m_penTarget10, m_eetEvent10, m_penCaused);

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
        CPrintF(TRANS("[%s] Finished Awaiting!\n"), m_strName);
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
        CPrintF(TRANS("[%s] Finished Awaiting!\n"), m_strName);
      }
    }

    m_iPos = IRnd() % 10 + 1;

    if (m_bDebugMessages) {
      CPrintF(TRANS("  Type=Random\n"));
      CPrintF(TRANS("  Sending event to Target %d\n"), m_iPos);
    }

    SendToSpecifiedTarget();
    Stuff();
    return;
  }
};