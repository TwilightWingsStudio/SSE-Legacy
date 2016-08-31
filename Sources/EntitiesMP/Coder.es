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
  #include "EntitiesMP/Player.h"
  #include "EntitiesMP/PlayerWeapons.h"
%}

enum ECoderModeType {
  0 ECMT_IDECODER "0 Decoder Index",
  1 ECMT_FDECODER "1 Decoder Float",
//  2 ECMT_ENCODER  "2 Encoder",
};

class CCoder: CEntity {
name      "Coder";
thumbnail "Thumbnails\\Coder.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'      = "Player Info",
   3 CTString m_strDescription = "",
   4 BOOL m_bDebugMessages "Debug Messages" = FALSE,
   5 enum ECoderModeType m_etType "Type" = ECMT_IDECODER,
   
  10 INDEX m_iIOIndex "IO Index" = 0,
  11 INDEX m_iIOFloat "IO Float" = 0.0F,
  
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
  
  40 enum EventEType m_eetEvent01    "Target 01 Event Type" 'G' = EET_TRIGGER,  // type of event to send
  41 enum EventEType m_eetEvent02    "Target 02 Event Type" 'H' = EET_TRIGGER,
  42 enum EventEType m_eetEvent03    "Target 03 Event Type" 'J' = EET_TRIGGER,
  43 enum EventEType m_eetEvent04    "Target 04 Event Type" 'K' = EET_TRIGGER,
  44 enum EventEType m_eetEvent05    "Target 05 Event Type" 'L' = EET_TRIGGER,
  45 enum EventEType m_eetEvent06    "Target 06 Event Type" = EET_TRIGGER,
  46 enum EventEType m_eetEvent07    "Target 07 Event Type" = EET_TRIGGER,
  47 enum EventEType m_eetEvent08    "Target 08 Event Type" = EET_TRIGGER,
  48 enum EventEType m_eetEvent09    "Target 09 Event Type" = EET_TRIGGER,
  49 enum EventEType m_eetEvent10    "Target 10 Event Type" = EET_TRIGGER,
  
  50 INDEX m_iIndex01    "Target 01 Index" = 0,
  51 INDEX m_iIndex02    "Target 02 Index" = 0,
  52 INDEX m_iIndex03    "Target 03 Index" = 0,
  53 INDEX m_iIndex04    "Target 04 Index" = 0,
  54 INDEX m_iIndex05    "Target 05 Index" = 0,
  55 INDEX m_iIndex06    "Target 06 Index" = 0,
  56 INDEX m_iIndex07    "Target 07 Index" = 0,
  57 INDEX m_iIndex08    "Target 08 Index" = 0,
  58 INDEX m_iIndex09    "Target 09 Index" = 0,
  59 INDEX m_iIndex10    "Target 10 Index" = 0,
  
  50 FLOAT m_fFloat01    "Target 01 Float" = 0.0F,
  51 FLOAT m_fFloat02    "Target 02 Float" = 0.0F,
  52 FLOAT m_fFloat03    "Target 03 Float" = 0.0F,
  53 FLOAT m_fFloat04    "Target 04 Float" = 0.0F,
  54 FLOAT m_fFloat05    "Target 05 Float" = 0.0F,
  55 FLOAT m_fFloat06    "Target 06 Float" = 0.0F,
  56 FLOAT m_fFloat07    "Target 07 Float" = 0.0F,
  57 FLOAT m_fFloat08    "Target 08 Float" = 0.0F,
  58 FLOAT m_fFloat09    "Target 09 Float" = 0.0F,
  59 FLOAT m_fFloat10    "Target 10 Float" = 0.0F,

 100 enum EventEType m_eetDefaultType "Default Event Type" = EET_TRIGGER,
 101 CEntityPointer m_penDefaultTarget "Default Target",
 102 INDEX m_iDefaultIndex "Dafault Index" = 0,
 102 INDEX m_fDefaultFloat "Dafault Float" = 0.0F,
 
 150 CEntityPointer m_penCaused,
  
components:
  1 model   MODEL_CODER   "Models\\Editor\\Coder.mdl",
  2 texture TEXTURE_CODER "Models\\Editor\\Coder.tex",

functions:
  const CTString &GetDescription(void) const {
    if (m_etType == ECMT_IDECODER) {
      ((CTString&)m_strDescription).PrintF("IDecoder");
    } else if (m_etType == ECMT_FDECODER) {
      ((CTString&)m_strDescription).PrintF("FDecoder");
    } else {
      ((CTString&)m_strDescription).PrintF("Encoder");
    }

    return m_strDescription;
  }

  void SendToSpecifiedTarget(INDEX i) {
    switch (i) {
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

  void DoIDecoder(const CEntityEvent &ee)
  {
    INDEX i = -1;

    if (m_iIOIndex == m_iIndex01) {
      i = 1;
    } else if (m_iIOIndex == m_iIndex02) {
      i = 2;
    } else if (m_iIOIndex == m_iIndex03) {
      i = 3;
    } else if (m_iIOIndex == m_iIndex04) {
      i = 4;
    } else if (m_iIOIndex == m_iIndex05) {
      i = 5;
    } else if (m_iIOIndex == m_iIndex06) {
      i = 6;
    } else if (m_iIOIndex == m_iIndex07) {
      i = 7;
    } else if (m_iIOIndex == m_iIndex08) {
      i = 8;
    } else if (m_iIOIndex == m_iIndex09) {
      i = 9;
    } else if (m_iIOIndex == m_iIndex10) {
      i = 10;
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

    if (m_iIOFloat == m_fFloat01) {
      i = 1;
    } else if (m_iIOFloat == m_fFloat02) {
      i = 2;
    } else if (m_iIOFloat == m_fFloat03) {
      i = 3;
    } else if (m_iIOFloat == m_fFloat04) {
      i = 4;
    } else if (m_iIOFloat == m_fFloat05) {
      i = 5;
    } else if (m_iIOFloat == m_fFloat06) {
      i = 6;
    } else if (m_iIOFloat == m_fFloat07) {
      i = 7;
    } else if (m_iIOFloat == m_fFloat08) {
      i = 8;
    } else if (m_iIOFloat == m_fFloat09) {
      i = 9;
    } else if (m_iIOFloat == m_fFloat10) {
      i = 10;
    }

    if (m_bDebugMessages) {
      if (i > 0 && i < 11) {
        CPrintF("[%s] (FDecoder) : Sending event to Target %d\n", GetName(), i);
      } else {
        CPrintF("[%s] (FDecoder) : Sending event to default target.\n", GetName());
      }
    }

    // Send event to target depending on input value.
    SendToSpecifiedTarget(i);
  }
  
  void DoEncoder(const CEntityEvent &ee) {
    m_iIOIndex = m_iDefaultIndex;
    m_iIOFloat = m_fDefaultFloat;
  }

  BOOL HandleEvent(const CEntityEvent &ee)
  {
    // Decoders can be started only by trigger event.
    if (ee.ee_slEvent == EVENTCODE_ETrigger && (m_etType == ECMT_IDECODER || m_etType == ECMT_FDECODER)) {
      ETrigger eTrigger = ((ETrigger &) ee);
      m_penCaused = eTrigger.penCaused;

      if (m_etType == ECMT_IDECODER) {
        DoIDecoder(ee);
      } else {
        DoFDecoder(ee);
      }
    } else {
      m_penCaused = NULL;
    }

    return CEntity::HandleEvent(ee);
  }
  
procedures:
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

