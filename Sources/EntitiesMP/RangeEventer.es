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

class CRangeEventer : CRationalEntity {
name      "RangeEventer";
thumbnail "Thumbnails\\RangeEventer.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'      = "Range Eventer",
   3 CTString m_strDescription = "",
   
   4 BOOL m_bActive              "Active" 'A' = TRUE,
   5 BOOL m_bDebugMessages    "Debug Messages" = FALSE,

  10 RANGE m_fSendRange            "Send Range" 'R' = 1.0f,
  11 enum EventEType m_eetRange    "Event Type" = EET_IGNORE,
  20 CTString m_strRangeClass      "Only Class" = "",
  21 CTString m_strRangeName       "Only Name"  = "",
 
components:
  1 model   MODEL_RANGEEVENTER   "Models\\Editor\\RangeEventer.mdl",
  2 texture TEXTURE_RANGEEVENTER "Models\\Editor\\RangeEventer.tex",

functions:
  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const {
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Called every time when entity receiving ETrigger entity event.
  // --------------------------------------------------------------------------------------
  void DoSend(const ETrigger &eTrigger)
  {
    if (m_eetRange == EET_IGNORE)
    {
      if (m_bDebugMessages) {
        CPrintF(TRANS("[RE][%s] : Sending skipped! Because selected 'Don't Send Event'!\n"), m_strName);
      }

      return;
    }
    
    BOOL bNoClass = m_strRangeClass == "" ? TRUE : FALSE;
    BOOL bNoName = m_strRangeName == "" ? TRUE : FALSE;
    
    INDEX ctTriggered = 0;
    
    if (m_bDebugMessages) {
      CPrintF(TRANS("[RE][%s] : Sending event in range %f\n"), m_strName, m_fSendRange);
    }
    
    if (m_fSendRange < 0.0F) {
      m_fSendRange = Abs(m_fSendRange);
    }

    // for each entity in container
    FOREACHINDYNAMICCONTAINER(GetWorld()->wo_cenEntities, CEntity, iten)
    {
      CEntity *pen = iten;

      if (pen == NULL) {
        continue;
      }
      
      if (pen == this) {
        continue;
      }
      
      CPlacement3D plEntity = pen->GetPlacement();
      plEntity.AbsoluteToRelative(this->GetPlacement());

      if (plEntity.pl_PositionVector.Length() <= m_fSendRange && (bNoClass || IsDerivedFromClass(pen, m_strRangeClass)) && (bNoName || m_strRangeName == pen->GetName()))
      {
        SendToTarget(pen, m_eetRange, eTrigger.penCaused);
        
        if (m_bDebugMessages) {
          ctTriggered++;
        }
      }
    }
    
    if (m_bDebugMessages)
    {
      if (ctTriggered > 0) {
        CPrintF("[RE][%s] : Entities received event = %d\n", m_strName, ctTriggered);
      } else {
        CPrintF("[RE][%s] : No any entity received event.\n", m_strName, ctTriggered);
      }
    }
  }

procedures:
  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    if (m_fSendRange < 0.0F) {
      m_fSendRange = Abs(m_fSendRange);
    }
    
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_RANGEEVENTER);
    SetModelMainTexture(TEXTURE_RANGEEVENTER);
  
    autowait(0.1f);
  
    wait()
    {
      on (EBegin) : { 
        resume;
      }

      on(ETrigger eTrigger) :
      {
        if (m_bActive) {
          DoSend(eTrigger);
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