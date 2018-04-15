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

229
%{
  #include "StdH.h"
  #include "Entities/Player.h"
  #include "Entities/PlayerWeapons.h"
%}

enum ECoderModeType {
  0 ECMT_IDECODER  "Decoder Index [0]",
  1 ECMT_FDECODER  "Decoder Float [1]",
  2 ECMT_ETENCODER "Encoder Event [2] Type",
  
  //3 ECMT_TENCODER  "3 Encoder Target",
};

class CCoder: CEntity {
name      "Coder";
thumbnail "Thumbnails\\Coder.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'      = "Coder",
   3 CTString m_strDescription = "",
   4 BOOL m_bDebugMessages "Debug Messages" = FALSE,
   5 enum ECoderModeType m_etType "Type" = ECMT_IDECODER,
   
  10 INDEX m_iIOIndex "IO Index" = 0,
  11 FLOAT m_fIOFloat "IO Float" = 0.0F,
  
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
  
  50 INDEX m_iIndex01    "Index For Target 01" = 0,
  51 INDEX m_iIndex02    "Index For Target 02" = 0,
  52 INDEX m_iIndex03    "Index For Target 03" = 0,
  53 INDEX m_iIndex04    "Index For Target 04" = 0,
  54 INDEX m_iIndex05    "Index For Target 05" = 0,
  55 INDEX m_iIndex06    "Index For Target 06" = 0,
  56 INDEX m_iIndex07    "Index For Target 07" = 0,
  57 INDEX m_iIndex08    "Index For Target 08" = 0,
  58 INDEX m_iIndex09    "Index For Target 09" = 0,
  59 INDEX m_iIndex10    "Index For Target 10" = 0,
  
  60 FLOAT m_fFloat01    "Float For Target 01" = 0.0F,
  61 FLOAT m_fFloat02    "Float For Target 02" = 0.0F,
  62 FLOAT m_fFloat03    "Float For Target 03" = 0.0F,
  63 FLOAT m_fFloat04    "Float For Target 04" = 0.0F,
  64 FLOAT m_fFloat05    "Float For Target 05" = 0.0F,
  65 FLOAT m_fFloat06    "Float For Target 06" = 0.0F,
  66 FLOAT m_fFloat07    "Float For Target 07" = 0.0F,
  67 FLOAT m_fFloat08    "Float For Target 08" = 0.0F,
  68 FLOAT m_fFloat09    "Float For Target 09" = 0.0F,
  69 FLOAT m_fFloat10    "Float For Target 10" = 0.0F,

 100 enum EventEType m_eetDefaultType "Default Event Type" = EET_TRIGGER,
 101 CEntityPointer m_penDefaultTarget "Default Target",
 102 INDEX m_iDefaultIndex "Default Index" = 0,
 103 INDEX m_fDefaultFloat "Default Float" = 0.0F,
 
 150 CEntityPointer m_penCaused,
  
components:
  1 model   MODEL_CODER   "Models\\Editor\\Coder.mdl",
  2 texture TEXTURE_CODER "Models\\Editor\\Coder.tex",

functions:
  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const
  {
    if (m_etType == ECMT_IDECODER) {
      ((CTString&)m_strDescription).PrintF("IDecoder");
    } else if (m_etType == ECMT_FDECODER) {
      ((CTString&)m_strDescription).PrintF("FDecoder");
    } else if (m_etType == ECMT_ETENCODER) {
      ((CTString&)m_strDescription).PrintF("ETEncoder");
    } else {
      ((CTString&)m_strDescription).PrintF("TEncoder");
    }

    return m_strDescription;
  }

  void SendToSpecifiedTarget(INDEX i)
  {
    switch (i)
    {
      case  1: SendToTarget(m_penTarget01, m_eetEvent01, m_penCaused); break;
      case  2: SendToTarget(m_penTarget02, m_eetEvent02, m_penCaused); break;
      case  3: SendToTarget(m_penTarget03, m_eetEvent03, m_penCaused); break;
      case  4: SendToTarget(m_penTarget04, m_eetEvent04, m_penCaused); break;
      case  5: SendToTarget(m_penTarget05, m_eetEvent05, m_penCaused); break;
      case  6: SendToTarget(m_penTarget06, m_eetEvent06, m_penCaused); break;
      case  7: SendToTarget(m_penTarget07, m_eetEvent07, m_penCaused); break;
      case  8: SendToTarget(m_penTarget08, m_eetEvent08, m_penCaused); break;
      case  9: SendToTarget(m_penTarget09, m_eetEvent09, m_penCaused); break;
      case 10: SendToTarget(m_penTarget10, m_eetEvent10, m_penCaused); break;
      
      default: SendToTarget(m_penDefaultTarget, m_eetDefaultType, NULL); break;
    }
  }

  INDEX GetIValueByID(INDEX i)
  {
    if (i < 0 || i > 9) {
      return m_iDefaultIndex;
    }
    
    const INDEX* aiValues = &m_iIndex01;

    return aiValues[i];
  }

  FLOAT GetFValueByID(INDEX i)
  {
    if (i < 0 || i > 9) {
      return m_fDefaultFloat;
    }

    const FLOAT* afValues = &m_fFloat01;

    return afValues[i];
  }
  
  INDEX GetIDByEventType(EventEType eet)
  {
    if (eet == m_eetEvent01) {
      return 0;
    } else if (eet == m_eetEvent02) {
      return 1;
    } else if (eet == m_eetEvent03) {
      return 2;
    } else if (eet == m_eetEvent04) {
      return 3;
    } else if (eet == m_eetEvent05) {
      return 4;
    } else if (eet == m_eetEvent06) {
      return 5;
    } else if (eet == m_eetEvent07) {
      return 6;
    } else if (eet == m_eetEvent08) {
      return 7;
    } else if (eet == m_eetEvent09) {
      return 8;
    } else if (eet == m_eetEvent10) {
      return 9;
    }

    return -1;
  }

  void DoIDecoder(const CEntityEvent &ee)
  {
    INDEX i = -1;

    const INDEX* aiValues = &m_iIndex01;

    for (INDEX b = 0; b < 10; b++) {
      //CPrintF("%d - %d\n", (b+1), aiValues[b]);
      if (m_iIOIndex == aiValues[b]) {
        i = b + 1;
        break;
      }
    }

    if (m_bDebugMessages) {
      if (i > 0 && i < 11) {
        CPrintF("[%s] (IDecoder) : Sending event to Target %d\n", GetName(), i);
      } else {
        CPrintF("[%s] (IDecoder) : Sending event to default target.\n", GetName());
      }
    }

    // Send event to target depending on input value.
    SendToSpecifiedTarget(i);
  }

  void DoFDecoder(const CEntityEvent &ee)
  {
    INDEX i = -1;

    const FLOAT* afValues = &m_fFloat01;

    for (INDEX b = 0; b < 10; b++) {
      //CPrintF("%d - %f\n", (b+1), afValues[b]);
      if (m_fIOFloat == afValues[b]) {
        i = b + 1;
        break;
      }
    }

    if (m_bDebugMessages) {
      if (i > 0 && i < 11) {
        CPrintF("[%s] (FDecoder) : Sending event to Target %d.\n", GetName(), i);
      } else {
        CPrintF("[%s] (FDecoder) : Sending event to default target.\n", GetName());
      }
    }

    // Send event to target depending on input value.
    SendToSpecifiedTarget(i);
  }
  
  void DoETEncoder(const CEntityEvent &ee)
  {
    INDEX i = -1;

    EventEType ett = EventCodeToEventType(ee.ee_slEvent);

    // Here is exists shit like EPass/EBegin etc. it's way to filter not allowed events.
    if (ett == EET_IGNORE) {
      return;
    }

    i = GetIDByEventType(ett);

    m_iIOIndex = GetIValueByID(i);
    m_fIOFloat = GetFValueByID(i);

    if (m_bDebugMessages) {
      if (i > 0 && i < 11) {
        CPrintF("[%s] (ETEncoder) : Ouputing I=Index[%d]=%d F=Float[%d]=%f taken from In %d.\n", GetName(), i, m_iIOIndex, i, m_fIOFloat, i + 1);
      } else {
        CPrintF("[%s] (ETEncoder) : Ouputing I=%d F=%f taken from default In.\n", GetName(), m_iIOIndex, m_fIOFloat);
      }
    }
  }

  // --------------------------------------------------------------------------------------
  // The entity event handler.
  // --------------------------------------------------------------------------------------
  BOOL HandleEvent(const CEntityEvent &ee)
  {
    // Decoders can be started only by trigger event.
    if ((ee.ee_slEvent == EVENTCODE_ETrigger || ee.ee_slEvent == EVENTCODE_ETargeted)
     && (m_etType == ECMT_IDECODER || m_etType == ECMT_FDECODER)) {
      ETrigger eTrigger = ((ETrigger &) ee);
      m_penCaused = eTrigger.penCaused;

      if (m_etType == ECMT_IDECODER) {
        DoIDecoder(ee);
      } else {
        DoFDecoder(ee);
      }
    } else if (m_etType == ECMT_ETENCODER) {
      DoETEncoder(ee);
    }

    // Clean up the target after event processing.
    m_penCaused = NULL;

    return CEntity::HandleEvent(ee);
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
    SetModel(MODEL_CODER);
    SetModelMainTexture(TEXTURE_CODER);
  
    return;
  }
};

