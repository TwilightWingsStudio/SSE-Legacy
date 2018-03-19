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

209
%{
  #include "StdH.h"
%}

enum ESwitchType {
  0 ESWT_ONCE    "Once",
  1 ESWT_ONOFF   "On/Off",
};

class CSimpleSwitch : CRationalEntity {
name      "SimpleSwitch";
thumbnail "Thumbnails\\SimpleSwitch.tbn";
features  "HasName", "HasTarget", "IsTargetable";


properties:
  1 CTString m_strName        "Name" 'N'        = "Simple Switch",

  3 BOOL m_bActive            "Active" 'A' = TRUE,
  4 BOOL m_bDebugMessages "Debug Messages" = FALSE,

 10 CEntityPointer m_penTarget      "ON-OFF Target" 'T' COLOR(C_dBLUE|0xFF),                      // send event to entity
 11 enum EventEType m_eetEvent      "ON  Event type" 'U' = EET_TRIGGER,  // type of event to send
 12 enum EventEType m_eetOffEvent   "OFF Event type" 'I' = EET_TRIGGER, // type of event to send
 13 CEntityPointer m_penOffTarget   "OFF Target" COLOR(C_dBLUE|0xFF),  // off target, if not null recives off event

 18 enum ESwitchType m_swtType   "Type" 'Y' = ESWT_ONOFF,
 19 CTString m_strMessage       "Message" 'M' = "",

 // internal -> do not use
 20 BOOL m_bSwitchON = FALSE,
 21 CEntityPointer m_penCaused,   // who triggered it last time

 // [SSE]
 30 FLOAT m_fUseRange "Use Range" = 2.0F,
 
components:

  1 model   MODEL_SIMPLESWITCH    "Models\\Editor\\SimpleSwitch.mdl",
  2 texture TEXTURE_SIMPLESWITCH  "Models\\Editor\\SimpleSwitch.tex"

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
    SLONG slUsedMemory = sizeof(CSimpleSwitch);

    // add some more
    slUsedMemory += m_strMessage.Length();
    return slUsedMemory;
  }

  // --------------------------------------------------------------------------------------
  // Turn the switch on.
  // --------------------------------------------------------------------------------------
  void SwitchON()
  {
    // If already on then do nothing!
    if (m_bSwitchON) {
      return;
    }

    m_bSwitchON = TRUE;

    // If we use targeted event then follow different way.
    if (m_eetEvent == EET_TARGETED) {
      SendTargetedEvent(m_penTarget, m_penCaused, this); // [SSE] Entities - Targeted Event
    } else {
      SendToTarget(m_penTarget, m_eetEvent, m_penCaused); // send event to target
    }
  };
  
  // --------------------------------------------------------------------------------------
  // Turn the switch off.
  // --------------------------------------------------------------------------------------
  void SwitchOFF()
  {
    // If already off then do nothing!
    if (!m_bSwitchON) {
      return;
    }

    m_bSwitchON = FALSE;
    
    CEntity *penOffTarget = m_penOffTarget != NULL ? m_penOffTarget : m_penTarget;

    // If we use targeted event then follow different way.
    if (m_eetOffEvent == EET_TARGETED) {
      SendTargetedEvent(penOffTarget, m_penCaused, this); // [SSE] Entities - Targeted Event
    } else {
      SendToTarget(penOffTarget, m_eetOffEvent, m_penCaused); // send event to target
    }
  };

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
    SetModel(MODEL_SIMPLESWITCH);
    SetModelMainTexture(TEXTURE_SIMPLESWITCH);
    
    wait()
    {
      // trigger event -> change switch
      on (ETrigger eTrigger) :
      {
        if (m_bActive && CanReactOnEntity(eTrigger.penCaused))
        {
          m_penCaused = eTrigger.penCaused;
          
          if (m_swtType == ESWT_ONCE) {
            m_bActive = FALSE;
          }

          // if switch is ON make it OFF
          if (m_bSwitchON) {
            SwitchOFF();

            if (m_bDebugMessages) {
              CPrintF("%s : Switched OFF!\n", m_strName);
            }

          } else { // else if switch is OFF make it ON
            SwitchON();

            if (m_bDebugMessages) {
              CPrintF("%s : Switched ON!\n", m_strName);
            }
          }
        }

        resume;
      }

      // start -> switch ON
      on (EStart) : {
        SwitchON();
        resume;
      }

      // stop -> switch OFF
      on (EStop) : {
        SwitchOFF();
        resume;
      }

      on (EActivate) :
      {
        m_bActive = TRUE;
        
        if (m_bDebugMessages) {
          CPrintF("%s : Activated!\n", m_strName);
        }
        
        resume;
      }
      
      on (EDeactivate) :
      {
        m_bActive = FALSE;
        
        if (m_bDebugMessages) {
          CPrintF("%s : Deactivated!\n", m_strName);
        }
        
        resume;
      }
    }

    return;
  };
};
