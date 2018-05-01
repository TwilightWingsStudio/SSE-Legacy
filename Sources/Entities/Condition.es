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

3974
%{
  #include "StdH.h"
%}

enum EConditionOp
{
   0 EC_NOP          "00 No Operation",

  // Basic Operations
   1 EC_SAME         "01 ==",
   2 EC_DIFFERENT    "02 !=",
   3 EC_LARGER       "03 >",
   4 EC_SMALLER      "04 <",
   5 EC_SMALLER_SAME "05 <=",
   6 EC_LARGER_SAME  "06 >=",

  // Bitwise conditions.   
   7 EC_BITMASKINC   "07  & (WIP)",
   8 EC_BITMASKEXC   "08  !& (WIP)",
   9 EC_BITINC       "09 & (1 << X) (WIP)",
  10 EC_BITEX        "10 !& (1 << X) (WIP)",
};

enum EConType
{
   0 ECT_ENTITY    "00 Property By Name",
   1 ECT_POSX      "01 Pos(X)",
   2 ECT_POSY      "02 Pos(Y)",
   3 ECT_POSZ      "03 Pos(Z)",
   4 ECT_SPEEDX    "04 Speed(X)",
   5 ECT_SPEEDY    "05 Speed(Y)",
   6 ECT_SPEEDZ    "06 Speed(Z)",
   7 ECT_SPEEDALL  "07 Speed(Total)",
   8 ECT_ROTH      "08 Rotation(H)",
   9 ECT_ROTP      "09 Rotation(P)",
  10 ECT_ROTB      "10 Rotation(B)",
  11 ECT_SPEEDXREL "11 Relative Speed(X)",
  12 ECT_SPEEDYREL "12 Relative Speed(Y)",
  13 ECT_SPEEDZREL "13 Relative Speed(Z)",
  14 ECT_HEALTH    "14 Health",
  15 ECT_TYPE      "15 Entity Class (unchangeable)",
  16 ECT_PROPBYID  "16 Property By ID (changer only)"
};

%{
static const char *PropertyTypeToShortName(EConType eType)
{
  switch (eType)
  {
    case ECT_POSX: {
      return "PosX"; 
    } break;

    case ECT_POSY: {
      return "PosY";
    } break;

    case ECT_POSZ: {
      return "PosZ";
    } break;

    case ECT_SPEEDX: {
      return "SpeedX";
    } break;

    case ECT_SPEEDY: {
      return "SpeedY";
    } break;

    case ECT_SPEEDZ: {
      return "SpeedZ";
    } break;

    case ECT_SPEEDALL: {
      return "SpeedTotal";
    } break;

    case ECT_ROTH: {
      return "RotH";
    } break;

    case ECT_ROTB: {
      return "RotP";
    } break;

    case ECT_ROTP: {
      return "RotB";
    } break;

    case ECT_SPEEDXREL: {
      return "SpeedXRel";
    } break;

    case ECT_SPEEDYREL: {
      return "SpeedYRel";
    } break;

    case ECT_SPEEDZREL: {
      return "SpeedZRel";
    } break;

    case ECT_HEALTH: {
      return "Health";
    } break;

    case ECT_TYPE: {
      return "EntityClass";
    } break;
  }
  
  return "<Error>";
}

static inline BOOL IsIndexEPT(CEntityProperty::PropertyType eptProperty)
{
  switch (eptProperty)
  {
    case CEntityProperty::EPT_INDEX: return TRUE;
    case CEntityProperty::EPT_ENUM: return TRUE;
    case CEntityProperty::EPT_FLAGS: return TRUE;
    case CEntityProperty::EPT_ANIMATION: return TRUE;
    case CEntityProperty::EPT_ILLUMINATIONTYPE: return TRUE;
    case CEntityProperty::EPT_COLOR: return TRUE;
  }
  
  return FALSE;
}
%}

class CCondition: CEntity {
name      "Condition";
thumbnail "Thumbnails\\Condition.tbn";
features  "HasName","IsTargetable";

properties:

   1 CTString m_strName        "Name" 'N'         = "Condition",
   2 CTString m_strDescription                    = "Condition",

   3 CTString m_strProperty1      "Target 1 Property Name"    = "",
   4 CTString m_strProperty2      "Target 2 Property Name"    = "",

   5 CEntityPointer m_penIfCondition1  "Target 1" COLOR(C_RED|0xFF),
   6 CEntityPointer m_penIfCondition2  "Target 2" COLOR(C_RED|0xFF),

   7 CEntityPointer m_penIfTarget      "If Target"             COLOR(C_RED|0xFF),
   8 CEntityPointer m_penElseTarget    "Else Target"           COLOR(C_RED|0xFF),

   9 enum EConditionOp m_eCondition "Operation" = EC_SAME,
  10 BOOL m_bActive        "Active"           = TRUE,
  11 BOOL m_bDebugMessages         "Debug Messages"   = FALSE,
  12 BOOL m_bAbs1          "Property 1 As Absolute" = FALSE,
  13 BOOL m_bAbs2          "Property 2 As Absolute" = FALSE,
  14 BOOL m_bCode          "Output as Script" = FALSE,

  15 enum EConType  m_eCT1 "Target 1 Property Type" = ECT_ENTITY,
  16 enum EConType  m_eCT2 "Target 2 Property Type" = ECT_ENTITY,

  17 enum TETargetType m_teTargetType1   "Target 1 Type"    = TETT_TARGET,
  18 enum TETargetType m_teTargetType2   "Target 2 Type"    = TETT_TARGET,

  19 CEntityPointer m_penError "Error Target",
  20 CTString m_strClass       "Class Name"  = "",

components:
  1 model   MODEL_CONDITION     "Models\\Editor\\Condition.mdl",
  2 texture TEXTURE_CONDITION   "Models\\Editor\\Condition.tex"

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
  const CTString &GetDescription(void) const{
    return m_strDescription;
  } 

  // --------------------------------------------------------------------------------------
  // Called every time when entity receives ETrigger event and it's active.
  // --------------------------------------------------------------------------------------
  BOOL DoCompare(CEntity *penCaused, CEntity *penTargetArg)
  {
    // If non-active.
    if (!m_bActive) {
      return TRUE;
    }
    
    if (m_bDebugMessages)
    {
      CPrintF("[C][%s] : Received event (-> %d, -> %d)\n", m_strName, penCaused ? penCaused->en_ulID : 0, penTargetArg ? penTargetArg->en_ulID : 0);
      
      if (penCaused) {
        CPrintF("  penCaused.Name = '%s'\n", penCaused->GetName().Undecorated());
      }
      
      if (penTargetArg) {
        CPrintF("  penTarget.Name = '%s'\n", penTargetArg->GetName().Undecorated());
      }
    }

    // Extended Target 1 types.
    switch (m_teTargetType1)
    {
      case TETT_PENCAUSED: {
        if (penCaused) {
          m_penIfCondition1 = penCaused;
        }
      } break;

      case TETT_PENTARGET: {
        if (penTargetArg) {
          m_penIfCondition1 = penTargetArg;
        }
      } break;

      default: break;
    }
    
    // Extended Target 2 types.
    switch (m_teTargetType2)
    {
      case TETT_PENCAUSED: {
        if (penCaused) {
          m_penIfCondition2 = penCaused;
        }
      } break;

      case TETT_PENTARGET: {
        if (penTargetArg) {
          m_penIfCondition2 = penTargetArg;
        }
      } break;

      default: break;
    }

    // Check both targets. If no atleast one target then process the error.
    if (m_penIfCondition1 == NULL && m_penIfCondition2 == NULL)
    {
      if (m_bDebugMessages) {
        CPrintF(TRANS("  Error! Both targets are NULL!\n"));
      }

      return FALSE;
    }

    // Initialize some variables.
    CEntityProperty *pProperty1 = NULL;
    CEntityProperty *pProperty2 = NULL;

    if (m_eCT1 == ECT_PROPBYID || m_eCT2 == ECT_PROPBYID)
    {
      if (m_bDebugMessages) {
        CPrintF(TRANS("  Error! Property By ID is currently not supported by Condition!\n"));
      }

      return FALSE;
    }

    // Get the first property.
    if (m_penIfCondition1 && m_eCT1 == ECT_ENTITY)
    {
      if (m_strProperty1 == "") {
        if (m_bDebugMessages) {
          CPrintF(TRANS("  Error! [Target 1 Property Name] is empty!\n"));
        }

        return FALSE;
      }
      
      
      pProperty1 = m_penIfCondition1->PropertyForName(m_strProperty1);
    }

    // Get the second property.
    if (m_penIfCondition2 && m_eCT2 == ECT_ENTITY)
    {
      if (m_strProperty2 == "") {
        if (m_bDebugMessages) {
          CPrintF(TRANS("  Error! [Target 2 Property Name] is empty!\n"));
        }

        return FALSE;
      }
      
      pProperty2 = m_penIfCondition2->PropertyForName(m_strProperty2);
    }

    // Output targets status.
    if (m_bDebugMessages)
    {
      if (m_penIfCondition1 == NULL) {
        CPrintF("  [Target 1] == NULL!\n");
      } else if (m_penIfCondition2 == NULL) {
        CPrintF("  [Target 2] == NULL!\n");
      } else {
        CPrintF("  Both targets are present!\n");
      }
    }

    // If [Target 1] valid but entity property not found.
    if (m_penIfCondition1 && m_eCT1 == ECT_ENTITY && pProperty1 == NULL)
    {
      if (m_bDebugMessages) {
        CPrintF(TRANS("  Error! [Target 1].[%s] not found\n"), m_strProperty1);
      }
      
      return FALSE;
    }

    // If [Target 2] valid but entity property not found.
    if (m_penIfCondition2 && m_eCT2 == ECT_ENTITY && pProperty2 == NULL)
    {
      if (m_bDebugMessages) {
        CPrintF(TRANS("  Error! [Target 2].[%s] not found\n"), m_strProperty2);
      }
      
      return FALSE;
    }

    BOOL bResult = FALSE, bError = FALSE;

    // Classname check.
    if (m_eCT1 == ECT_TYPE) {
      if (IsDerivedFromClass(m_penIfCondition1, m_strClass)) {
        bResult = TRUE;
      }

    } else {
      FLOAT fValue, fValue2;
      INDEX iValue, iValue2;
      CEntityPointer penPointer;
      CEntityPointer penPointer2;
      CTString strValue;
      CTString strValue2;
      BOOL bf1 = FALSE, bf2 = FALSE, bp1 = FALSE, bp2 = FALSE, bs1 = FALSE, bs2 = FALSE;
      BOOL bi1 = FALSE, bi2 = FALSE;

      // If entity property.
      if (m_eCT1 == ECT_ENTITY)
      {
        SLONG offset1 = pProperty1->ep_slOffset;
        CEntityProperty::PropertyType eptType = pProperty1->ep_eptType;
        
        if (IsIndexEPT(eptType)) {
          iValue = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penIfCondition1) + offset1));
          bi1 = TRUE;
        } else if (eptType == CEntityProperty::EPT_FLOAT || eptType == CEntityProperty::EPT_RANGE || eptType == CEntityProperty::EPT_ANGLE) {
          fValue = *((FLOAT *)(((UBYTE *)(CEntity*)&*m_penIfCondition1) + offset1));
          bf1 = TRUE;
        } else if (eptType== CEntityProperty::EPT_ENTITYPTR) {
          penPointer = *((CEntityPointer *)(((UBYTE *)(CEntity*)&*m_penIfCondition1) + offset1));
          bp1 = TRUE;
        } else if (eptType == CEntityProperty::EPT_STRING || eptType == CEntityProperty::EPT_FILENAME) {
          strValue = *((CTString *)(((UBYTE *)(CEntity*)&*m_penIfCondition1) + offset1));
          bs1 = TRUE;
        }

      // If non-property.
      } else {
        fValue = GetFloatNonEP(m_penIfCondition1, m_eCT1, bError);
        bf1 = !bError;
      }

      if (m_penIfCondition2) {
        // If entity property.
        if (m_eCT2 == ECT_ENTITY) {
          SLONG offset2 = pProperty2->ep_slOffset;
          CEntityProperty::PropertyType eptType = pProperty2->ep_eptType;

          if (IsIndexEPT(eptType)) {
            iValue2 = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penIfCondition1) + offset2));
            bi2 = TRUE;
          } else if (eptType == CEntityProperty::EPT_FLOAT || eptType == CEntityProperty::EPT_RANGE || eptType == CEntityProperty::EPT_ANGLE) {
            fValue2 = *((FLOAT *)(((UBYTE *)(CEntity*)&*m_penIfCondition2) + offset2));
            bf2 = TRUE;
          } else if (eptType == CEntityProperty::EPT_ENTITYPTR) {
            penPointer2 = *((CEntityPointer *)(((UBYTE *)(CEntity*)&*m_penIfCondition2) + offset2));
            bp2 = TRUE;
          } else if (eptType == CEntityProperty::EPT_STRING || eptType == CEntityProperty::EPT_FILENAME) {
            strValue2 = *((CTString *)(((UBYTE *)(CEntity*)&*m_penIfCondition2) + offset2));
            bs2 = TRUE;
          }

        // If non-property.
        } else {
          fValue = GetFloatNonEP(m_penIfCondition2, m_eCT2, bError);
          bf2 = !bError;
        }
      }

      // INDEX FLOAT
      if (bi1 && bf2)
      {
        fValue = (FLOAT)iValue; // Convert INDEX to FLOAT.
        bf1 = TRUE;
        bi1 = FALSE;

        if (m_bDebugMessages) {
          CPrintF("  INDEX(%d) -> FLOAT(%.2f)\n", iValue, fValue);
        }
      }

      // FLOAT INDEX
      if (bf1 && bi2)
      {
        fValue2 = (FLOAT)iValue2; // Convert INDEX to FLOAT.
        bf2 = TRUE;
        bi2 = FALSE;

        if (m_bDebugMessages) {
          CPrintF("  INDEX(%d) -> FLOAT(%.2f)\n", iValue2, fValue2);
        }
      }

      // INDEX INDEX
      if (bi1 && bi2) {
        if (m_bDebugMessages) {
          CPrintF("  Comparing two integer properties...\n");
        }

        if (m_bAbs1) {
          iValue = Abs(iValue);
        }
        
        if (m_bAbs2) {
          iValue2 = Abs(iValue2);
        }

        // Act according to condition type.
        switch (m_eCondition)
        {
          // [==]
          case EC_SAME: {
            if (m_bDebugMessages) {
              CPrintF("  Expression = (INDEX(%d) == INDEX(%d))\n", iValue, iValue2);
            }

            if (iValue == iValue2) {
              bResult = TRUE;
            }
          } break;

          // [!=]
          case EC_DIFFERENT: {
            if (m_bDebugMessages) {
              CPrintF("  Expression = (INDEX(%d) != INDEX(%d))\n", iValue, iValue2);
            }

            if (iValue != iValue2) {
              bResult = TRUE;
            }
          } break;

          // [>]
          case EC_LARGER: {
            if (m_bDebugMessages) {
              CPrintF("  Expression = (INDEX(%d) > INDEX(%d))\n", iValue, iValue2);
            }

            if (iValue > iValue2) {
              bResult = TRUE;
            }
          } break;
          
          // [>=]
          case EC_LARGER_SAME: {
            if (m_bDebugMessages) {
              CPrintF("  Expression = (INDEX(%d) >= INDEX(%d))\n", iValue, iValue2);
            }

            if (iValue >= iValue2) {
              bResult = TRUE;
            }
          } break;
          
          // [<]
          case EC_SMALLER: {
            if (m_bDebugMessages) {
              CPrintF("  Expression = (INDEX(%d) < INDEX(%d))\n", iValue, iValue2);
            }
            
            if (iValue < iValue2) {
              bResult = TRUE;
            }
          } break;
          
          // [<=]
          case EC_SMALLER_SAME: {
            if (m_bDebugMessages) {
              CPrintF("  Expression = (INDEX(%d) <= INDEX(%d))\n", iValue, iValue2);
            }

            if (iValue <= iValue2) {
              bResult = TRUE;
            }
          } break;
        }

      // ONE INDEX
      } else if (bi1) {
        if (m_eCondition == EC_NOP) {
          
          if (m_bDebugMessages) {
            CPrintF("  Expression = (INDEX(%d))\n", iValue);
          }

          if (iValue != 0) {
            bResult = TRUE;
          }
        } else {
          if (m_bDebugMessages) {
            CPrintF("  Error! Missing second operand!\n");
          }
          
          bError = TRUE;
        }

      // FLOAT FLOAT
      } else if (bf1 && bf2) {
        if (m_bDebugMessages) {
          CPrintF("  Comparing two floating point properties...\n");
        }
        
        FLOAT rAbs1 = fValue;
        FLOAT rAbs2 = fValue2;

        // Take absolute values if it enabled.
        if (m_bAbs1) {rAbs1 = abs(rAbs1);}
        if (m_bAbs2) {rAbs2 = abs(rAbs2);}
        
        // Act according to condition type.
        switch (m_eCondition)
        {
          // [==]
          case EC_SAME: {
            if (m_bDebugMessages) {
              CPrintF("  Expression = (FLOAT(%.4f) == FLOAT(%.4f))\n", rAbs1, rAbs2);
            }

            if (rAbs1 == rAbs2) {
              bResult = TRUE;
            }
          } break;

          // [!=]
          case EC_DIFFERENT: {
            if (m_bDebugMessages) {
              CPrintF("  Expression = (FLOAT(%.4f) != FLOAT(%.4f))\n", rAbs1, rAbs2);
            }
            
            if (rAbs1 != rAbs2) {
              bResult = TRUE;
            }
          } break;

          // [>]
          case EC_LARGER: {
            if (m_bDebugMessages) {
              CPrintF("  Expression = (FLOAT(%.4f) > FLOAT(%.4f))\n", rAbs1, rAbs2);
            }

            if (rAbs1 > rAbs2) {
              bResult = TRUE;
            }
          } break;
          
          // [>=]
          case EC_LARGER_SAME: {
            if (m_bDebugMessages) {
              CPrintF("  Expression = (FLOAT(%.4f) >= FLOAT(%.4f))\n", rAbs1, rAbs2);
            }
            
            if (rAbs1 >= rAbs2) {
              bResult = TRUE;
            }
          } break;
          
          // [<]
          case EC_SMALLER: {
            if (m_bDebugMessages) {
              CPrintF("  Expression = (FLOAT(%.4f) < FLOAT(%.4f))\n", rAbs1, rAbs2);
            }

            if (rAbs1 < rAbs2) {
              bResult = TRUE;
            }
          } break;
          
          // [<=]
          case EC_SMALLER_SAME: {
            if (m_bDebugMessages) {
              CPrintF("  Expression = (FLOAT(%.4f) <= FLOAT(%.4f))\n", rAbs1, rAbs2);
            }

            if (rAbs1 <= rAbs2) {
              bResult = TRUE;
            }
          } break;
        }

      // ONE FLOAT
      } else if (bf1) {

        if (m_eCondition == EC_NOP) {
          if (m_bDebugMessages) {
            CPrintF("  Expression = (FLOAT(%.4f))\n", fValue);
          }

          if (fValue != 0.0F) {
            bResult = TRUE;
          }
        } else {
          if (m_bDebugMessages) {
            CPrintF("  Error! Missing second operand!\n");
          }
          
          bError = TRUE;
        }

      // POINTER POINTER
      } else if (bp1 && bp2) {
        if (m_bDebugMessages)
        {
          CPrintF("  Comparing two pointers...\n");

          if (penPointer) {
            CPrintF("  [%s].[%s] = (-> %d)[%s]\n", ((CEntity&)*m_penIfCondition1).GetName(), m_strProperty1, penPointer->en_ulID, penPointer->GetName());
          } else {
            CPrintF("  [%s].[%s] = NULL\n", ((CEntity&)*m_penIfCondition1).GetName(), m_strProperty1);
          }
          
          if (penPointer2) {
            CPrintF("  [%s].[%s] = (-> %d)[%s]\n", ((CEntity&)*m_penIfCondition2).GetName(), m_strProperty2, penPointer2->en_ulID, penPointer2->GetName());
          } else {
            CPrintF("  [%s].[%s] = NULL\n", ((CEntity&)*m_penIfCondition2).GetName(), m_strProperty2);
          }
        }
        
        // [==]
        if (m_eCondition == EC_SAME)
        {
          if (m_bDebugMessages) {
            CPrintF("  Expression = ((-> %d) == (-> %d))\n", penPointer ? penPointer->en_ulID : 0, penPointer2 ? penPointer2->en_ulID : 0);
          }

          if (penPointer == penPointer2) {
            bResult = TRUE;
          }

        // [!=]
        } else if (m_eCondition == EC_DIFFERENT) {
          if (m_bDebugMessages) {
            CPrintF("  Expression = ((-> %d) 1= (-> %d))\n", penPointer ? penPointer->en_ulID : 0, penPointer2 ? penPointer2->en_ulID : 0);
          }

          if (penPointer != penPointer2) {
            bResult = TRUE;
          }
        }

      // String
      } else if (bs1) {
        // If we have second string then compare them between.
        if (bs2) {
          if (m_bDebugMessages)
          {
            CPrintF("  Comparing two strings...\n");
          }
          
          // ==
          if (m_eCondition == EC_SAME) {
            if (m_bDebugMessages) {
              CPrintF("  Condition = [==] (same)\n");
            }
            
            if (strValue == strValue2) {
              bResult = TRUE;
            }

          // !=
          } else if (m_eCondition == EC_DIFFERENT) {
            if (m_bDebugMessages) {
              CPrintF("  Condition = [!=] (different)\n");
            }
            
            if (strValue != strValue2) {
              bResult = TRUE;
            }
          }
          
        // If we have only one string then check if it isn't empty.
        } else {
          if (strValue != "") {
          if (m_bDebugMessages)
          {
            CPrintF("  Checking if string is not empty...\n");
          }
            
            bResult = TRUE;
          }
        }

      // Other
      } else {
        bError = TRUE;
        if (m_bDebugMessages) {
          CPrintF(TRANS("  Unsupported Data Type\n"));
        }
      }
    }

    if (bError) {
      return FALSE;
    }

    // trigger proper target
    if (bResult) {
      if (m_penIfTarget) {
        if (m_bDebugMessages) {
          CPrintF(TRANS("  Triggering [If Target]: %s\n"), m_penIfTarget->GetName());
        }

        SendToTarget(m_penIfTarget, EET_TRIGGER, penCaused);
      } else if (m_bDebugMessages) {
        CPrintF(TRANS("  Result = TRUE, but no [If Target] to trigger\n"));
      }

    } else {
      if (m_penElseTarget) {
        if (m_bDebugMessages) {
          CPrintF(TRANS("  Triggering [Else Target]: %s\n"), m_penElseTarget->GetName());
        }

        SendToTarget(m_penElseTarget, EET_TRIGGER, penCaused);
      } else if (m_bDebugMessages) {
        CPrintF(TRANS("  Result = FALSE, but no [Else Target] to send event.\n"));
      }
    }
    
    return TRUE;
  }

  // --------------------------------------------------------------------------------------
  // Returns non-property float from given entity.
  // --------------------------------------------------------------------------------------
  FLOAT GetFloatNonEP(CEntity *penSource, EConType eType, BOOL &bError)
  {
    // Position & Orientation
    switch (eType)
    {
      case ECT_POSX: {
        return penSource->GetPlacement().pl_PositionVector(1);
      } break;
      
      case ECT_POSY: {
        return penSource->GetPlacement().pl_PositionVector(2);
      } break;
      
      case ECT_POSZ: {
        return penSource->GetPlacement().pl_PositionVector(3);
      } break;
    
      case ECT_ROTH: {
        return penSource->GetPlacement().pl_OrientationAngle(1);
      } break;

      case ECT_ROTP: {
        return penSource->GetPlacement().pl_OrientationAngle(2);
      } break;

      case ECT_ROTB: {
        return penSource->GetPlacement().pl_OrientationAngle(3);
      } break;
    }

    // Health
    if (eType == ECT_HEALTH)
    {
      if (penSource->IsLiveEntity()) {
        return ((CLiveEntity&)*penSource).GetHealth();
      } else {
        if (m_bDebugMessages) {
          CPrintF(TRANS("  Don't use health on entities without health!\n"));
        }

        bError = TRUE;
        return 0.0F;
      }
    }

    // Absolute & Relative Speeds
    if (penSource->IsMovableEntity()) {
      CMovableEntity *penMovableSource = static_cast<CMovableEntity *>(penSource);
      
      switch (eType)
      {
        case ECT_SPEEDX: {
          return penMovableSource->en_vCurrentTranslationAbsolute(1);
        } break;
        
        case ECT_SPEEDY: {
          return penMovableSource->en_vCurrentTranslationAbsolute(2);
        } break;
        
        case ECT_SPEEDZ: {
          return penMovableSource->en_vCurrentTranslationAbsolute(3);
        } break;
        
        case ECT_SPEEDALL: {
          return penMovableSource->en_vCurrentTranslationAbsolute.Length();
        } break;
        
        case ECT_SPEEDXREL: {
          CPlacement3D plSpeed = CPlacement3D(((CMovableEntity&)*m_penIfCondition1).en_vCurrentTranslationAbsolute,ANGLE3D(0, 0, 0));
          CPlacement3D plRot = CPlacement3D(FLOAT3D(0, 0, 0),m_penIfCondition1->GetPlacement().pl_OrientationAngle);
          plSpeed.AbsoluteToRelative(plRot);
          return plSpeed.pl_PositionVector(1);
        } break;
        
        case ECT_SPEEDYREL: {
          CPlacement3D plSpeed = CPlacement3D(((CMovableEntity&)*m_penIfCondition1).en_vCurrentTranslationAbsolute,ANGLE3D(0, 0, 0));
          CPlacement3D plRot = CPlacement3D(FLOAT3D(0, 0, 0),m_penIfCondition1->GetPlacement().pl_OrientationAngle);
          plSpeed.AbsoluteToRelative(plRot);
          return plSpeed.pl_PositionVector(2);
        } break;
        
        case ECT_SPEEDZREL: {
          CPlacement3D plSpeed = CPlacement3D(((CMovableEntity&)*m_penIfCondition1).en_vCurrentTranslationAbsolute,ANGLE3D(0, 0, 0));
          CPlacement3D plRot = CPlacement3D(FLOAT3D(0, 0, 0),m_penIfCondition1->GetPlacement().pl_OrientationAngle);
          plSpeed.AbsoluteToRelative(plRot);
          return plSpeed.pl_PositionVector(3);
        } break;
      }
    }
    
    if (m_bDebugMessages) {
      CPrintF(TRANS("  Invalid property type!\n"));
    }

    bError = TRUE;
    return 0.0F;
  }

  // --------------------------------------------------------------------------------------
  // The entity event handler.
  // --------------------------------------------------------------------------------------
  BOOL HandleEvent(const CEntityEvent &ee)
  {
    // Trigger Event
    if (ee.ee_slEvent == EVENTCODE_ETrigger)
    {
      const ETrigger &eTrigger = ((ETrigger &) ee);

      // If error occured then send event to error target.
      if (!DoCompare(eTrigger.penCaused, NULL)) {
        SendToTarget(m_penError, EET_TRIGGER, eTrigger.penCaused);
      }
    }
    
    // Targeted Event
    if (ee.ee_slEvent == EVENTCODE_ETargeted)
    {
      const ETargeted &eTargeted = ((ETargeted &) ee);
      
      // If error occured then send event to error target.
      if (!DoCompare(eTargeted.penCaused, eTargeted.penTarget)) {
        SendToTarget(m_penError, EET_TRIGGER, eTargeted.penCaused);
      }
    }

    if (ee.ee_slEvent== EVENTCODE_EActivate) {
      m_bActive = TRUE;
    }

    if (ee.ee_slEvent == EVENTCODE_EDeactivate) {
      m_bActive = FALSE;
    }

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

    if (m_teTargetType1 == TETT_THIS) {
      m_teTargetType1 = TETT_TARGET;
    }

    if (m_teTargetType2 == TETT_THIS) {
      m_teTargetType2 = TETT_TARGET;
    }
    
    if (m_eCT1 == ECT_PROPBYID) {
      m_eCT1 = ECT_ENTITY;
    }
    
    if (m_eCT2 == ECT_PROPBYID) {
      m_eCT2 = ECT_ENTITY;
    }

    if (m_bCode) {
      m_bCode = FALSE;
      CTString strCode = "if ( ";

      if (m_penIfCondition1) {
        strCode += ((CEntity&)*m_penIfCondition1).GetName();

        if (m_eCT1 == ECT_ENTITY) {
          if (m_penIfCondition1->PropertyForName(m_strProperty1) != NULL) {
            strCode += ".";
            strCode += m_strProperty1;
          } else {
            strCode += ".[Property not found]";
          }
        } else {
          strCode += ".";

          // Select first operand's name.
          strCode += PropertyTypeToShortName(m_eCT1);
        }
      } else {
        strCode += "NULL";
      }

      strCode += " ";

      if (m_eCondition == EC_SAME) {
        strCode += "== ";
      } else if (m_eCondition == EC_LARGER) {
        strCode += "> ";
      } else if (m_eCondition == EC_LARGER_SAME) {
        strCode += ">= ";
      } else if (m_eCondition == EC_SMALLER) {
        strCode += "< ";
      } else if (m_eCondition == EC_SMALLER_SAME) {
        strCode += "<= ";
      } else if (m_eCondition == EC_DIFFERENT) {
        strCode += "!= ";
      }

      if (m_penIfCondition2) {
        strCode += ((CEntity&)*m_penIfCondition2).GetName();

        if (m_eCT2 == ECT_ENTITY) {
          if (m_penIfCondition2->PropertyForName(m_strProperty2) != NULL) {
            strCode += ".";
            strCode += m_strProperty2;
          } else {
            strCode += ".[Property not found]";
          }
        } else {
          strCode += ".";

          // Select second operand's name.
          strCode += PropertyTypeToShortName(m_eCT2);
        }
      } else {
        strCode += "NULL";
      }

      strCode += " ) {\n";

      if (m_penIfTarget) {
        strCode += "  Trigger(";
        strCode += ((CEntity&)*m_penIfTarget).GetName();
        strCode += ");\n}";
      } else {
        strCode += "}";
      }

      if (m_penElseTarget) {
        strCode += " else {\n";
        strCode += "  Trigger(";
        strCode += ((CEntity&)*m_penElseTarget).GetName();
        strCode += ");\n}";
      }

      WarningMessage(strCode);
    }

    // set appearance
    SetModel(MODEL_CONDITION);
    SetModelMainTexture(TEXTURE_CONDITION);

    return;
  }
};