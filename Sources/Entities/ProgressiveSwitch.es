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

2576
%{
  #include "StdH.h"
%}

event EStartInteraction {
  CEntityPointer penPlayer,
};

event EStopInteraction {
  CEntityPointer penPlayer,
};

event EInteractionTick {
  CEntityPointer penPlayer,
};

event EInteractionComplete {
  CEntityPointer penInteractable,
};

enum EPSwitchType {
  0 EPST_ONEPLAYER  "0 One Player",
  1 ESWT_MULTIPLE   "1 Multiple Players",
};

enum EPSwitchBehavior {
  0 EPSB_BREAK     "0 Break",
  1 EPSB_SAVE      "1 Pause",
  2 EPSB_DEGRADE   "2 Degrade",
};

class CProgressiveSwitch : CRationalEntity {
name      "ProgressiveSwitch";
thumbnail "Thumbnails\\ProgressiveSwitch.tbn";
features  "HasName", "IsTargetable";

properties:
  1 CTString m_strName        "Name" 'N'        = "Progressive Switch",

  3 BOOL m_bActive            "Active" 'A' = TRUE,
  4 BOOL m_bDebugMessages "Debug Messages" = FALSE,

  5 enum EPSwitchType m_epstType  "Type" = EPST_ONEPLAYER,
 10 FLOAT m_fUseRange     "Use Range" = 2.0F,
 11 CTString m_strMessage "Message" 'M' = "",

 15 INDEX m_iMaxInteractors /*"Max Interactors"*/ = 1,
 
 16 enum EPSwitchBehavior m_epsbBehavior "On Stop Behavior" = EPSB_BREAK,
 
 20 CEntityPointer m_penStartTarget    "On Start Target" COLOR(C_dBLUE|0xFF),
 21 CEntityPointer m_penJoinTarget     "On Join Target" COLOR(C_dBLUE|0xFF),
 22 CEntityPointer m_penStopTarget     "On Stop Target" COLOR(C_dYELLOW|0xFF),
 23 CEntityPointer m_penBreakTarget    "On Break Target" COLOR(C_dRED|0xFF),
 24 CEntityPointer m_penCompleteTarget "On Complete Target" COLOR(C_dGREEN|0xFF),
 
 30 enum EventEType m_eetStart      "On Start Event type"    = EET_TRIGGER,
 31 enum EventEType m_eetJoin       "On Join Event type"    = EET_TRIGGER,
 32 enum EventEType m_eetStop       "On Stop Event type"    = EET_TRIGGER,
 33 enum EventEType m_eetBreak      "On Break Event type"    = EET_TRIGGER,
 34 enum EventEType m_eetComplete   "On Complete Event type" = EET_TRIGGER, // When the interaction successfully ended. penCaused will be last tick.
 
 // internal -> do not use
 41 CEntityPointer m_penLastTicker,   // who triggered it last time

 44 FLOAT m_fProgress "IO Progress" = 0.0F,
 45 FLOAT m_fMaxProgress "Max Progress" = 100.0F,
 46 FLOAT m_fTickProgress "Per Tick Progress" = 1.0F,
 
components:

  1 model   MODEL_PROGRESSIVESWITCH    "Models\\Editor\\ProgressiveSwitch.mdl",
  2 texture TEXTURE_PROGRESSIVESWITCH  "Models\\Editor\\ProgressiveSwitch.tex"

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
  // [SSE] Interaction API
  // Return true if this entity can act act like interaction provider.
  // --------------------------------------------------------------------------------------
  virtual BOOL IsInteractionProvider()
  {
    return TRUE;
  }
  
  // --------------------------------------------------------------------------------------
  // [SSE] Interaction API
  // Returns minimal distance to able interact with object.
  // --------------------------------------------------------------------------------------
  virtual FLOAT GetInteractionDistance(void) const
  {
    return m_fUseRange;
  }

  // --------------------------------------------------------------------------------------
  // [SSE] Interaction API
  // Returns hint showed when you point the object.
  // --------------------------------------------------------------------------------------
  virtual const CTString& GetInteractionHint(void) const
  {
    return m_strMessage;
  }

  // --------------------------------------------------------------------------------------
  // Returns TRUE if this door reacts on this entity.
  // --------------------------------------------------------------------------------------
  BOOL CanReactOnEntity(CEntity *pen)
  {
    if (pen == NULL) {
      return FALSE;
    }

    // never react on non-live or dead entities
    if (!(pen->GetFlags()&ENF_ALIVE)) {
      return FALSE;
    }

    return TRUE;
  }

  // --------------------------------------------------------------------------------------
  // Returns bytes of memory used by this object.
  // --------------------------------------------------------------------------------------
  SLONG GetUsedMemory(void)
  {
    // initial
    SLONG slUsedMemory = sizeof(CProgressiveSwitch);

    // add some more
    slUsedMemory += m_strMessage.Length();
    return slUsedMemory;
  }
  
  // --------------------------------------------------------------------------------------
  // Processes activation.
  // --------------------------------------------------------------------------------------
  void Activate(void)
  {
    m_bActive = TRUE;
    
    if (m_bDebugMessages) {
      CPrintF("[PSw] %s : Activated!\n", m_strName);
    }
  }
  
  // --------------------------------------------------------------------------------------
  // Processes deactivation.
  // --------------------------------------------------------------------------------------
  void Deactivate(void)
  {
    m_bActive = FALSE;
    
    if (m_bDebugMessages) {
      CPrintF("[PSw] %s : Deactivated!\n", m_strName);
    }
  }
  
  // --------------------------------------------------------------------------------------
  // Processes stop interaction logic while used by one player.
  // --------------------------------------------------------------------------------------
  void StopSingleLogic(CEntity *penPlayer)
  {
    // Instant break.
    if (m_epsbBehavior == EPSB_BREAK) {
      if (m_bDebugMessages) {
        CPrintF("[PSw] %s : Handled OnBreak()\n", m_strName);
      }
      
      if (m_bDebugMessages) {
        if (m_penBreakTarget != NULL && m_eetBreak != EET_IGNORE) {
          CPrintF("[PSw] %s : Sending event to [%s]\n", m_strName, m_penBreakTarget->GetName());
        }
      }

      SendToTarget(m_penBreakTarget, m_eetBreak, penPlayer);

      m_fProgress = 0.0F;

    // Save the progress to continue later.
    } else if (m_epsbBehavior == EPSB_SAVE || m_epsbBehavior == EPSB_DEGRADE) {
      if (m_bDebugMessages) {
        CPrintF("[PSw] %s : Handled OnStop()\n", m_strName);
      }
      
      if (m_bDebugMessages) {
        if (m_penStopTarget != NULL && m_eetStop != EET_IGNORE) {
          CPrintF("[PSw] %s : Sending event to [%s]\n", m_strName, m_penStopTarget->GetName());
        }
      }

      SendToTarget(m_penStopTarget, m_eetStop, penPlayer);
    }
  }

procedures:

  // --------------------------------------------------------------------------------------
  // Working state.
  // --------------------------------------------------------------------------------------
  Working(EVoid)
  {
    // Repeat always.
    while (TRUE)
    {
      wait(_pTimer->TickQuantum)
      {
        on (EBegin) : { 
          resume;
        }
        
        on (EStartInteraction eStartInteraction) :
        {
          if (m_bDebugMessages) {
            CPrintF("[PSw] %s : Handled JoinInteraction()\n", m_strName);
            
            if (m_penJoinTarget != NULL && m_eetJoin != EET_IGNORE) {
              CPrintF("[PSw] %s : Sending event to [%s]\n", m_strName, m_penJoinTarget->GetName());
            }
          }

          SendToTarget(m_penJoinTarget, m_eetJoin, eStartInteraction.penPlayer);

          resume;
        }
        
        on (EStopInteraction eStopInteraction) :
        {
          if (m_iMaxInteractors == 1) {
            StopSingleLogic(eStopInteraction.penPlayer);
            jump Idle();

          } else {
            if (m_bDebugMessages) {
              CPrintF("[PSw] %s : Handled StopInteraction!\n", m_strName);
            }
          }

          resume;
        }
        
        on (EInteractionTick eInteractionTick) :
        {
          if (m_bDebugMessages) {
            CPrintF("[PSw] %s : Handled OnTick()\n", m_strName);
          }
          
          m_fProgress += m_fTickProgress;
          
          m_penLastTicker = eInteractionTick.penPlayer; // Remember the last ticker.

          if (m_fProgress < m_fMaxProgress) {
            resume;
          }

          INDEX ctMaxPlayers = GetMaxPlayers();
          CEntity *penPlayer = NULL;

          // Cycle through all players...
          for (INDEX i = 0; i < ctMaxPlayers; i++)
          {
            penPlayer = GetPlayerEntity(i);
            
            // If player is invalid then skip him.
            if (penPlayer == NULL) {
              continue;
            }
            
            EInteractionComplete eInteractionComplete;
            eInteractionComplete.penInteractable = this;
            penPlayer->SendEvent(eInteractionComplete);
          }

          m_fProgress = 0;
          
          if (m_bDebugMessages) {
            CPrintF("[PSw] %s : Handled OnComplete()\n", m_strName);

            if (m_penCompleteTarget != NULL && m_eetComplete != EET_IGNORE) {
              CPrintF("[PSw] %s : Sending event to [%s]\n", m_strName, m_penCompleteTarget->GetName());
            }
          }
          
          SendToTarget(m_penCompleteTarget, m_eetComplete, eInteractionTick.penPlayer);
          
          jump Idle();
        }

        on(ETimer) : { stop; }
      }
    }
  }

  // --------------------------------------------------------------------------------------
  // Idle state.
  // --------------------------------------------------------------------------------------
  Idle(EVoid)
  {
    wait()
    {
      on (EBegin) : { resume; }

      on (EStartInteraction eStartInteraction) :
      { 
        if (m_bDebugMessages)
        {
          if (eStartInteraction.penPlayer == NULL) {
            CPrintF("[PSw] %s : Handled OnStart() from unknown!\n", m_strName);
          }
          
          CPrintF("[PSw] %s : Handled OnStart()\n", m_strName);
        
          if (m_penStartTarget != NULL && m_eetStart != EET_IGNORE) {
            CPrintF("[PSw] %s : Sending event to [%s]\n", m_strName, m_penStartTarget->GetName());
          }
        }
        
        SendToTarget(m_penStartTarget, m_eetStart, eStartInteraction.penPlayer);

        jump Working();
      }
      
      on (EActivate) :
      {
        Activate();
        resume;
      }
      
      on (EDeactivate) :
      {
        Deactivate();
        resume;
      }
    }
  }

  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    m_iMaxInteractors = ClampDn(m_iMaxInteractors, INDEX(1));
    
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_PROGRESSIVESWITCH);
    SetModelMainTexture(TEXTURE_PROGRESSIVESWITCH);
    
    autocall Idle() EEnd;
    
    return;
  };
};
