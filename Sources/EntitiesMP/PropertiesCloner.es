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

1337
%{
  #include "StdH.h"
%}

enum EPropsClonerDirection {
  0  EPCD_T1TOT2  "0 Target 1 -> Target 2",
  1  EPCO_T2TOT1  "1 Target 2 -> Target 1",
};

class CPropertiesCloner: CRationalEntity {
name      "PropertiesCloner";
thumbnail "Thumbnails\\PropertiesCloner.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName "Name" 'N'      = "Properties Cloner",
   3 CTString m_strDescription = "",
   
   4 BOOL m_bActive              "Active" 'A' = TRUE,
   5 BOOL m_bDebugMessages "Debug Messages" = FALSE,

   6 enum EPropsClonerDirection m_editDirection "Direction" = EPCD_T1TOT2,

   9 CEntityPointer m_penTarget1      "Target 1" 'T' COLOR(C_CYAN|0xFF),
  10 CEntityPointer m_penTarget2      "Target 2" 'Y' COLOR(C_MAGENTA|0xFF),
  
  17 BOOL m_bPenCausedAsTarget1 "Target 1=penCaused" = FALSE,
  18 BOOL m_bPenCausedAsTarget2 "Target 2=penCaused" = FALSE,

components:
  1 model   MODEL_PROPERTIESCLONER   "Models\\Editor\\PropertiesCloner.mdl",
  2 texture TEXTURE_PROPERTIESCLONER "Models\\Editor\\PropertiesCloner.tex",

functions:
  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const {
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Processes all business logic here. Called on trigger event.
  // --------------------------------------------------------------------------------------
  void DoClone(const ETrigger &eTrigger)
  {
    CEntity *penTarget1 = m_penTarget1;
    CEntity *penTarget2 = m_penTarget2;

    if (m_bPenCausedAsTarget1) {
      penTarget1 = eTrigger.penCaused;
    }

    if (m_bPenCausedAsTarget2) {
      penTarget2 = eTrigger.penCaused;
    }

    // If non-existing target then cancel.
    if (penTarget1 == NULL || penTarget2 == NULL)
    {
      if (m_bDebugMessages) {
        CPrintF("%s : One of targets is NULL!\n", m_strName);
      }

      return;
    }

    // If same target then cancel.
    if (penTarget1 == penTarget2)
    {
      if (m_bDebugMessages) {
        CPrintF("%s : Targets are same entity!\n", m_strName);
      }
      
      return;
    }

    // If different class then cancel.
    if (penTarget1->en_pecClass != penTarget2->en_pecClass)
    {
      if (m_bDebugMessages) {
        CPrintF("%s : Entities aren't of same class!\n", m_strName);
      }
      
      return;
    }

    if (m_editDirection == EPCD_T1TOT2) {
      penTarget2->CopyEntityProperties(*penTarget1, 0);

      if (m_bDebugMessages)
      {
        CPrintF("%s : Copied properties:\n", m_strName);
        CPrintF("  Source: %s\n", penTarget1->GetName());
        CPrintF("  Target: %s\n", penTarget2->GetName());
      }
    } else {
      penTarget1->CopyEntityProperties(*penTarget2, 0);

      if (m_bDebugMessages)
      {
        CPrintF("%s : Copied properties:\n", m_strName);
        CPrintF("  Source: %s\n", penTarget2->GetName());
        CPrintF("  Target: %s\n", penTarget1->GetName());
      }
    }
    
    if (m_bDebugMessages)
    {
      CPrintF("  ----------------------------------------\n");
      for(CDLLEntityClass *pdecDLLClass = penTarget1->en_pecClass->ec_pdecDLLClass;
        pdecDLLClass != NULL;
        pdecDLLClass = pdecDLLClass->dec_pdecBase) {
        
        if (CTString(pdecDLLClass->dec_strName).Length() == 0) {
          continue;
        }

        CPrintF("  [%s] - %d properties.\n", pdecDLLClass->dec_strName,  pdecDLLClass->dec_ctProperties);
      }
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

    // set appearance
    SetModel(MODEL_PROPERTIESCLONER);
    SetModelMainTexture(TEXTURE_PROPERTIESCLONER);
  
    autowait(0.1f);
  
    wait()
    {
      on (EBegin) : { 
        resume;
      }

      on(ETrigger eTrigger) :
      {
        if (m_bActive) {
          DoClone(eTrigger);
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

