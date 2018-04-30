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

enum ECondition {
   0 EC_SAME         "0 ==",
   2 EC_DIFFERENT    "1 !=",
   1 EC_LARGER       "2 >",
   3 EC_SMALLER      "3 <",
   4 EC_SMALLER_SAME "4 <=",
   5 EC_LARGER_SAME  "5 >=",

  // Bitwise conditions.   
   6 EC_BITMASKINC   "6 & (WIP)",
   7 EC_BITMASKEXC   "7 !& (WIP)",
   8 EC_BITINC       "8 & (1 << X) (WIP)",
   9 EC_BITEX        "9 !& (1 << X) (WIP)",

};

enum EConType {
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

   9 enum ECondition m_eCondition "Operation" = EC_SAME,
  10 BOOL m_bActive        "Active"           = TRUE,
  11 BOOL m_bDebugMessages         "Debug Messages"   = FALSE,
  12 BOOL m_bAbs1          "Property 1 As Absolute" = FALSE,
  13 BOOL m_bAbs2          "Property 2 As Absolute" = FALSE,
  14 BOOL m_bCode          "Output as Script" = FALSE,

  15 enum EConType  m_eCT1 "Target 1 Property Type" = ECT_ENTITY,
  16 enum EConType  m_eCT2 "Target 2 Property Type" = ECT_ENTITY,

  17 BOOL m_bCaused1       "Target 1=penCaused" = FALSE,
  18 BOOL m_bCaused2       "Target 2=penCaused" = FALSE,

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
  void DoCompare(CEntity *penCaused, CEntity *penTargetArg)
  {
    // If non-active.
    if (!m_bActive) {
      return;
    }

    // If enabled then set penCaused as condition target 1.
    if (m_bCaused1 && penCaused) {
      m_penIfCondition1 = penCaused;
    }

    // If enabled then set penCaused as condition target 2.
    if (m_bCaused2 && penCaused) {
      m_penIfCondition2 = penCaused;
    }

    if (m_penIfCondition1 == NULL) {
      if (m_bDebugMessages) {
        CPrintF(TRANS("[C][%s] : First Condition Target Not Set!\n"), m_strName);
      }

      return;
    }

    // Check condition target 2 existance and compare data types.
    if (m_penIfCondition2) {
      if (m_bDebugMessages && m_eCT1 == ECT_ENTITY && m_eCT2 == ECT_ENTITY && m_penIfCondition1->PropertyForName(m_strProperty1) && m_penIfCondition2->PropertyForName(m_strProperty2) && m_penIfCondition1->PropertyForName(m_strProperty1)->ep_eptType!=m_penIfCondition2->PropertyForName(m_strProperty2)->ep_eptType) {
        CPrintF(TRANS("[C][%s] : Different Data Types!\n"), m_strName);
      }
    } else {
      if (m_bDebugMessages) {
        CPrintF(TRANS("[C][%s] : Second Condition Target Not Set!\n"), m_strName);
      }
    }

    if (m_eCT1 == ECT_ENTITY && m_bDebugMessages && m_penIfCondition1->PropertyForName(m_strProperty1) == NULL) {
      CPrintF(TRANS("[C][%s] : Condition 1 : %s Not Found\n"), m_strName, m_strProperty1);
      return;
    }

    BOOL bResult = 0, be = 0;

    if (m_eCT1 == ECT_TYPE) {
      if (IsDerivedFromClass(m_penIfCondition1, m_strClass)) {
        bResult = TRUE;
      }
    } else {
      FLOAT fValue;
      FLOAT fValue2;
      CEntityPointer penPointer;
      CEntityPointer penPointer2;
      CTString strValue;
      CTString strValue2;
      BOOL bf1, bf2, bp1, bp2, bs1, bs2;

      if (m_eCT1 == ECT_ENTITY) {
        CEntityProperty* pProperty = m_penIfCondition1->PropertyForName(m_strProperty1);

        if (pProperty != NULL) {
          SLONG offset1 = pProperty->ep_slOffset;
          CEntityProperty::PropertyType eptType = pProperty->ep_eptType;

          if (eptType == CEntityProperty::EPT_FLOAT || eptType == CEntityProperty::EPT_INDEX || eptType == CEntityProperty::EPT_RANGE || eptType == CEntityProperty::EPT_BOOL || eptType == CEntityProperty::EPT_ANGLE) {
            fValue = *((FLOAT *)(((UBYTE *)(CEntity*)&*m_penIfCondition1) + offset1));
            bf1 = TRUE;
          } else if (eptType== CEntityProperty::EPT_ENTITYPTR) {
            penPointer = *((CEntityPointer *)(((UBYTE *)(CEntity*)&*m_penIfCondition1) + offset1));
            bp1 = TRUE;
          } else if (eptType == CEntityProperty::EPT_STRING || eptType == CEntityProperty::EPT_FILENAME) {
            strValue = *((CTString *)(((UBYTE *)(CEntity*)&*m_penIfCondition1) + offset1));
            bs1 = TRUE;
          }
        }
      } else if (m_eCT1 == ECT_POSX) {
        fValue = m_penIfCondition1->GetPlacement().pl_PositionVector(1);
        bf1 = TRUE;
      } else if (m_eCT1 == ECT_POSY) {
        fValue = m_penIfCondition1->GetPlacement().pl_PositionVector(2);
        bf1 = TRUE;
      } else if (m_eCT1 == ECT_POSZ) {
        fValue = m_penIfCondition1->GetPlacement().pl_PositionVector(3);
        bf1 = TRUE;
      } else if (m_eCT1 == ECT_ROTH) {
        fValue = m_penIfCondition1->GetPlacement().pl_OrientationAngle(1);
        bf1 = TRUE;
      } else if (m_eCT1 == ECT_ROTP) {
        fValue = m_penIfCondition1->GetPlacement().pl_OrientationAngle(2);
        bf1 = TRUE;
      } else if (m_eCT1 == ECT_ROTB) {
        fValue = m_penIfCondition1->GetPlacement().pl_OrientationAngle(3);
        bf1 = TRUE;
      } else if (m_penIfCondition1->GetFlags()&ENF_ALIVE && m_eCT1 == ECT_HEALTH) {
        fValue = ((CLiveEntity&)*m_penIfCondition1).en_fHealth;
        bf1 = TRUE;
      } else if (IsDerivedFromClass(m_penIfCondition1,"MovableEntity")) {
        if (m_eCT1 == ECT_SPEEDX) {
          fValue = ((CMovableEntity&)*m_penIfCondition1).en_vCurrentTranslationAbsolute(1);
          bf1 = TRUE;
        } else if (m_eCT1 == ECT_SPEEDY) {
          fValue = ((CMovableEntity&)*m_penIfCondition1).en_vCurrentTranslationAbsolute(2);
          bf1 = TRUE;
        } else if (m_eCT1 == ECT_SPEEDZ) {
          fValue = ((CMovableEntity&)*m_penIfCondition1).en_vCurrentTranslationAbsolute(3);
          bf1 = TRUE;
        } else if (m_eCT1 == ECT_SPEEDALL) {
          fValue = ((CMovableEntity&)*m_penIfCondition1).en_vCurrentTranslationAbsolute.Length();
          bf1 = TRUE;
        } else if (m_eCT1 == ECT_SPEEDXREL) {
          CPlacement3D plSpeed = CPlacement3D(((CMovableEntity&)*m_penIfCondition1).en_vCurrentTranslationAbsolute,ANGLE3D(0,0,0));
          CPlacement3D plRot = CPlacement3D(FLOAT3D(0, 0, 0),m_penIfCondition1->GetPlacement().pl_OrientationAngle);
          plSpeed.AbsoluteToRelative(plRot);
          fValue = plSpeed.pl_PositionVector(1);
          bf1 = TRUE;
        } else if (m_eCT1 == ECT_SPEEDYREL) {
          CPlacement3D plSpeed = CPlacement3D(((CMovableEntity&)*m_penIfCondition1).en_vCurrentTranslationAbsolute,ANGLE3D(0,0,0));
          CPlacement3D plRot = CPlacement3D(FLOAT3D(0, 0, 0),m_penIfCondition1->GetPlacement().pl_OrientationAngle);
          plSpeed.AbsoluteToRelative(plRot);
          fValue = plSpeed.pl_PositionVector(2);
          bf1 = TRUE;
        } else if (m_eCT1 == ECT_SPEEDZREL) {
          CPlacement3D plSpeed = CPlacement3D(((CMovableEntity&)*m_penIfCondition1).en_vCurrentTranslationAbsolute,ANGLE3D(0,0,0));
          CPlacement3D plRot = CPlacement3D(FLOAT3D(0, 0, 0),m_penIfCondition1->GetPlacement().pl_OrientationAngle);
          plSpeed.AbsoluteToRelative(plRot);
          fValue = plSpeed.pl_PositionVector(3);
          bf1 = TRUE;
        }

      } else {
        be = TRUE;
        if (m_bDebugMessages) {
          CPrintF(TRANS("[C][%s][1] : Don't use speeds on not moving entities or health on entities without health\n"), m_strName);
        }
      }

      if (m_penIfCondition2) {
        if (m_eCT2 == ECT_ENTITY) {
          CEntityProperty* pProperty2 = m_penIfCondition2->PropertyForName(m_strProperty2);

          if (pProperty2 != NULL) {
            SLONG offset2 = pProperty2->ep_slOffset;
            CEntityProperty::PropertyType eptType = pProperty2->ep_eptType;

            if (eptType == CEntityProperty::EPT_FLOAT || eptType == CEntityProperty::EPT_INDEX || eptType == CEntityProperty::EPT_RANGE || eptType == CEntityProperty::EPT_BOOL || eptType == CEntityProperty::EPT_ANGLE) {
              fValue2 = *((FLOAT *)(((UBYTE *)(CEntity*)&*m_penIfCondition2)+offset2));
              bf2 = TRUE;
            } else if (eptType == CEntityProperty::EPT_ENTITYPTR) {
              penPointer2 = *((CEntityPointer *)(((UBYTE *)(CEntity*)&*m_penIfCondition2)+offset2));
              bp2 = TRUE;
            } else if (eptType == CEntityProperty::EPT_STRING || eptType == CEntityProperty::EPT_FILENAME) {
              strValue2 = *((CTString *)(((UBYTE *)(CEntity*)&*m_penIfCondition2)+offset2));
              bs2 = TRUE;
            }
          }
        } else if (m_eCT2 == ECT_POSX) {
          fValue2 = m_penIfCondition2->GetPlacement().pl_PositionVector(1);
          bf2 = TRUE;
        } else if (m_eCT2 == ECT_POSY) {
          fValue2 = m_penIfCondition2->GetPlacement().pl_PositionVector(2);
          bf2 = TRUE;
        } else if (m_eCT2 == ECT_POSZ) {
          fValue2 = m_penIfCondition2->GetPlacement().pl_PositionVector(3);
          bf2 = TRUE;
        } else if (m_eCT2 == ECT_ROTH) {
          fValue2 = m_penIfCondition2->GetPlacement().pl_OrientationAngle(1);
          bf2 = TRUE;
        } else if (m_eCT2 == ECT_ROTP) {
          fValue2 = m_penIfCondition2->GetPlacement().pl_OrientationAngle(2);
          bf2 = TRUE;
        } else if (m_eCT2 == ECT_ROTB) {
          fValue2 = m_penIfCondition2->GetPlacement().pl_OrientationAngle(3);
          bf2 = TRUE;
        } else if (m_penIfCondition2->GetFlags()&ENF_ALIVE&&m_eCT2 == ECT_HEALTH) {
          fValue2 = ((CLiveEntity&)*m_penIfCondition2).en_fHealth;
          bf2 = TRUE;
        } else if (IsDerivedFromClass(m_penIfCondition1,"MovableEntity")) {
          if (m_eCT2 == ECT_SPEEDX) {
            fValue2 = ((CMovableEntity&)*m_penIfCondition2).en_vCurrentTranslationAbsolute(1);
            bf2 = TRUE;
          } else if (m_eCT2 == ECT_SPEEDY) {
            fValue2 = ((CMovableEntity&)*m_penIfCondition2).en_vCurrentTranslationAbsolute(2);
            bf2 = TRUE;
          } else if (m_eCT2 == ECT_SPEEDZ) {
            fValue2 = ((CMovableEntity&)*m_penIfCondition2).en_vCurrentTranslationAbsolute(3);
            bf2 = TRUE;
          } else if (m_eCT2 == ECT_SPEEDALL) {
            fValue2 = ((CMovableEntity&)*m_penIfCondition2).en_vCurrentTranslationAbsolute.Length();
            bf2 = TRUE;
          } else if (m_eCT2 == ECT_SPEEDXREL) {
            CPlacement3D plSpeed = CPlacement3D(((CMovableEntity&)*m_penIfCondition2).en_vCurrentTranslationAbsolute, ANGLE3D(0, 0, 0));
            CPlacement3D plRot = CPlacement3D(FLOAT3D(0, 0, 0), m_penIfCondition2->GetPlacement().pl_OrientationAngle);
            plSpeed.AbsoluteToRelative(plRot);
            fValue2 = plSpeed.pl_PositionVector(1);
            bf2 = TRUE;
          } else if (m_eCT2 == ECT_SPEEDYREL) {
            CPlacement3D plSpeed = CPlacement3D(((CMovableEntity&)*m_penIfCondition2).en_vCurrentTranslationAbsolute, ANGLE3D(0, 0, 0));
            CPlacement3D plRot = CPlacement3D(FLOAT3D(0, 0, 0), m_penIfCondition2->GetPlacement().pl_OrientationAngle);
            plSpeed.AbsoluteToRelative(plRot);
            fValue2 = plSpeed.pl_PositionVector(2);
            bf2 = TRUE;
          } else if (m_eCT2 == ECT_SPEEDZREL) {
            CPlacement3D plSpeed = CPlacement3D(((CMovableEntity&)*m_penIfCondition2).en_vCurrentTranslationAbsolute, ANGLE3D(0, 0, 0));
            CPlacement3D plRot = CPlacement3D(FLOAT3D(0, 0, 0), m_penIfCondition2->GetPlacement().pl_OrientationAngle);
            plSpeed.AbsoluteToRelative(plRot);
            fValue2 = plSpeed.pl_PositionVector(3);
            bf2 = TRUE;
          }
        } else {
          be = TRUE;
          if (m_bDebugMessages) {
            CPrintF(TRANS("[C][%s][2] : Don't use speeds on not moving entities or health on entities without health\n"), m_strName);
          }
        }
      } else {
        be = TRUE;
        CPrintF(TRANS("[C][%s] : Set the second condition target goddammit\n"), GetName());
      }

      // FLOAT
      if (bf1 && bf2) {
        if (m_bDebugMessages)
        {
          CPrintF("[C][%s] : Comparing two floating point properties...\n", m_strName);
        }
        
        FLOAT rAbs1 = fValue;
        FLOAT rAbs2 = fValue2;

        // Take absolute values if it enabled.
        if (m_bAbs1) {rAbs1 = abs(rAbs1);}
        if (m_bAbs2) {rAbs2 = abs(rAbs2);}

        if (m_eCondition == EC_SAME) {
          if (m_bDebugMessages) {
            CPrintF("  Condition = [==] (same)\n");
          }

          if (rAbs1 == rAbs2) {
            bResult = TRUE;
          }
        } else if (m_eCondition == EC_DIFFERENT) {
          if (m_bDebugMessages) {
            CPrintF("  Condition = [!=] (different)\n");
          }
          
          if (rAbs1 != rAbs2) {
            bResult = TRUE;
          }
        } else if (m_eCondition == EC_LARGER) {
          if (m_bDebugMessages) {
            CPrintF("  Condition = [>] (larger)\n");
          }

          if (rAbs1 > rAbs2) {
            bResult = TRUE;
          }
        } else if (m_eCondition == EC_LARGER_SAME) {
          if (m_bDebugMessages) {
            CPrintF("  Condition = [>=] (larger-same)\n");
          }
          
          if (rAbs1 >= rAbs2) {
            bResult = TRUE;
          }
        } else if (m_eCondition == EC_SMALLER) {
          if (m_bDebugMessages) {
            CPrintF("  Condition = [<] (smaller)\n");
          }

          if (rAbs1 < rAbs2) {
            bResult = TRUE;
          }
        } else if (m_eCondition == EC_SMALLER_SAME) {
          if (m_bDebugMessages) {
            CPrintF("  Condition = [<=] (smaller-same)\n");
          }

          if (rAbs1 <= rAbs2) {
            bResult = TRUE;
          }
        }
      
      // ONE FLOAT
      } else if (bf1) {
        if (fValue > 0) {
          bResult = TRUE;
        }

      // POINTER
      } else if (bp1 && bp2) {
        if (m_bDebugMessages)
        {
          CPrintF("[%s] : Comparing two pointers...\n", m_strName);

          if (penPointer) {
            CPrintF("  [%s].[%s] = [#%d][%s]\n", ((CEntity&)*m_penIfCondition1).GetName(), m_strProperty1, penPointer->en_ulID, penPointer->GetName());
          } else {
            CPrintF("  [%s].[%s] = NULL\n", ((CEntity&)*m_penIfCondition1).GetName(), m_strProperty1);
          }
          
          if (penPointer2) {
            CPrintF("  [%s].[%s] = [#%d][%s]\n", ((CEntity&)*m_penIfCondition2).GetName(), m_strProperty2, penPointer2->en_ulID, penPointer2->GetName());
          } else {
            CPrintF("  [%s].[%s] = NULL\n", ((CEntity&)*m_penIfCondition2).GetName(), m_strProperty2);
          }
        }
        
        if (m_eCondition == EC_SAME)
        {
          if (m_bDebugMessages) {
            CPrintF("  Condition = [==] (same)\n");
          }

          if (penPointer == penPointer2) {
            bResult = TRUE;
          }

        } else if (m_eCondition == EC_DIFFERENT) {
          if (m_bDebugMessages) {
            CPrintF("  Condition = [!=] (different)\n");
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
            CPrintF("[%s] : Comparing two strings...\n", m_strName);
          }
          
          // ===
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
            CPrintF("[%s] : Checking if string is not empty...\n", m_strName);
          }
            
            bResult = TRUE;
          }
        }

      // Other
      } else {
        be = TRUE;
        if (m_bDebugMessages) {
          CPrintF(TRANS("[%s] : Unsupported Data Type\n"), m_strName);
        }
      }
    }

    if (be) {
      SendToTarget(m_penError, EET_TRIGGER, penCaused);
    }

    // trigger proper target
    if (bResult) {
      if (m_penIfTarget) {
        if (m_bDebugMessages) {
          CPrintF(TRANS("[C][%s] : Triggering [If Target]: %s\n"), m_strName, m_penIfTarget->GetName());
        }

        SendToTarget(m_penIfTarget, EET_TRIGGER, penCaused);
      } else if (m_bDebugMessages) {
        CPrintF(TRANS("[C][%s] : Result=TRUE, but no [If Target] to trigger\n"), m_strName);
      }
    } else {
      if (m_penElseTarget) {
        if (m_bDebugMessages) {
          CPrintF(TRANS("[C][%s] : Triggering [Else Target]: %s\n"), m_strName, m_penElseTarget->GetName());
        }

        SendToTarget(m_penElseTarget, EET_TRIGGER, penCaused);
      } else if (m_bDebugMessages) {
        CPrintF(TRANS("[C][%s] : Result=FALSE, but no [Else Target] to trigger\n"), m_strName);
      }
    }
  }

  // --------------------------------------------------------------------------------------
  // The entity event handler.
  // --------------------------------------------------------------------------------------
  BOOL HandleEvent(const CEntityEvent &ee)
  {
    // Trigger Event
    if (ee.ee_slEvent == EVENTCODE_ETrigger) {
      DoCompare(((ETrigger &) ee).penCaused, NULL);
    }
    
    // Targeted Event
    if (ee.ee_slEvent == EVENTCODE_ETargeted) {
      const ETargeted &eTargeted = ((ETargeted &) ee);
      
      DoCompare(eTargeted.penCaused, eTargeted.penTarget);
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
          switch (m_eCT1)
          {
            case ECT_POSX:      strCode += "PosX"; break;
            case ECT_POSY:      strCode += "PosY"; break;
            case ECT_POSZ:      strCode += "PosZ"; break;
            case ECT_SPEEDX:    strCode += "SpeedX"; break;
            case ECT_SPEEDY:    strCode += "SpeedY"; break;
            case ECT_SPEEDZ:    strCode += "SpeedZ"; break;
            case ECT_SPEEDALL:  strCode += "SpeedTotal"; break;
            case ECT_ROTH:      strCode += "RotH"; break;
            case ECT_ROTB:      strCode += "RotP"; break;
            case ECT_ROTP:      strCode += "RotB"; break;
            case ECT_SPEEDXREL: strCode += "SpeedXRel"; break;
            case ECT_SPEEDYREL: strCode += "SpeedYRel"; break;
            case ECT_SPEEDZREL: strCode += "SpeedZRel"; break;
            case ECT_HEALTH:    strCode += "Health"; break;
            case ECT_TYPE:      strCode += "EntityClass"; break;
          
            default: break;
          };
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
          switch (m_eCT2)
          {
            case ECT_POSX:      strCode += "PosX"; break;
            case ECT_POSY:      strCode += "PosY"; break;
            case ECT_POSZ:      strCode += "PosZ"; break;
            case ECT_SPEEDX:    strCode += "SpeedX"; break;
            case ECT_SPEEDY:    strCode += "SpeedY"; break;
            case ECT_SPEEDZ:    strCode += "SpeedZ"; break;
            case ECT_SPEEDALL:  strCode += "SpeedTotal"; break;
            case ECT_ROTH:      strCode += "RotH"; break;
            case ECT_ROTB:      strCode += "RotP"; break;
            case ECT_ROTP:      strCode += "RotB"; break;
            case ECT_SPEEDXREL: strCode += "SpeedXRel"; break;
            case ECT_SPEEDYREL: strCode += "SpeedYRel"; break;
            case ECT_SPEEDZREL: strCode += "SpeedZRel"; break;
            case ECT_HEALTH:    strCode += "Health"; break;
            case ECT_TYPE:      strCode += "EntityClass"; break;
          
            default: break;
          };
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