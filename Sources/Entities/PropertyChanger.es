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

3334
%{
  #include "StdH.h"
  #include "Condition.h"
%}

enum EOperation {
  0  EO_SET        "=",
  1  EO_ADD        "+",
  2  EO_SUBSTRACT  "-",
  3  EO_MULTIPLY   "*",
  4  EO_DIVIDE     "/",
  5  EO_REMAINDER  "% (MOD / FMOD)", // TODO: Write code for it later.
  6  EO_BITAND     "& (AND index only)",
  7  EO_BITOR      "| (OR index only)",
  8  EO_XOR        "^ (XOR index only)",
  9  EO_INCLUDEBIT "|= 1 << X (index only)",
 10  EO_EXCLUDEBIT "&= ~(1 << X) (index only)",
};

%{
static CTString GetNameForEPT(CEntityProperty::PropertyType eptType)
{
  switch (eptType)
  {
    case CEntityProperty::EPT_PARENT:           return "CEntityPointer";
    case CEntityProperty::EPT_SPAWNFLAGS:       return "SPAWNFLAGS";
    case CEntityProperty::EPT_ENUM:             return "ENUM";
    case CEntityProperty::EPT_BOOL:             return "BOOL";
    case CEntityProperty::EPT_FLOAT:            return "FLOAT";
    case CEntityProperty::EPT_COLOR:            return "COLOR";
    case CEntityProperty::EPT_STRING:           return "CTString";
    case CEntityProperty::EPT_RANGE:            return "RANGE";
    case CEntityProperty::EPT_ENTITYPTR:        return "CEntityPointer";
    case CEntityProperty::EPT_FILENAME:         return "CTFileName";
    case CEntityProperty::EPT_INDEX:            return "INDEX";
    case CEntityProperty::EPT_ANIMATION:        return "ANIMATION";
    case CEntityProperty::EPT_ILLUMINATIONTYPE: return "ILLUMINATION";
    case CEntityProperty::EPT_FLOATAABBOX3D:    return "FLOATAABBox3D";
    case CEntityProperty::EPT_ANGLE:            return "ANGLE";
    case CEntityProperty::EPT_FLOAT3D:          return "FLOAT3D";
    case CEntityProperty::EPT_ANGLE3D:          return "ANGLE3D";
    case CEntityProperty::EPT_FLOATplane3D:     return "FLOATplane3D";
    case CEntityProperty::EPT_MODELOBJECT:      return "CModelObject";
    case CEntityProperty::EPT_PLACEMENT3D:      return "CPlacement3D";
    case CEntityProperty::EPT_ANIMOBJECT:       return "CAnimObject";
    case CEntityProperty::EPT_FILENAMENODEP:    return "CTFileName";
    case CEntityProperty::EPT_SOUNDOBJECT:      return "CSoundObject";
    case CEntityProperty::EPT_STRINGTRANS:      return "CTString";
    case CEntityProperty::EPT_FLOATQUAT3D:      return "FLOAtquat3D";
    case CEntityProperty::EPT_FLOATMATRIX3D:    return "FLOATmatrix3D";
    case CEntityProperty::EPT_FLAGS:            return "FLAGS";
    case CEntityProperty::EPT_MODELINSTANCE:    return "CModelInstance";

    default: return "UNKNOWN";
  }
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

class CPropertyChanger: CEntity {
name      "PropertyChanger";
thumbnail "Thumbnails\\PropertyChanger.tbn";
features  "HasName", "IsTargetable", "HasTarget";

properties:
 
   1 CTString m_strName        "Name" 'N'        = "Property Changer",
   3 CTString m_strDescription                   = "Property Changer",

   5 BOOL m_bActive            "Active"          = TRUE,
   6 BOOL m_bDebugMessages     "Debug Messages"  = FALSE,
   
   // Different data types start here.
  10 CTString   m_strValue     "Value String"    = "",
  11 INDEX      m_iValue       "Value Integer"   = 0,
  12 FLOAT      m_fValue       "Value Float"     = 0.0F,
  13 CTFileName m_fnValue      "Value File Name" = CTFILENAME(""),
  14 RANGE      m_rValue       "Value Range"     = 1.0F,
  15 CEntityPointer m_penValue "Value Target" COLOR(C_RED|0xFF),
  16 BOOL      m_bValue        "Value Bool"      = FALSE,
  17 COLOR     m_cValue        "Value Color"     = COLOR(C_WHITE|0xFF),
  18 ANIMATION m_aValue        "Value Animation" = 0,
  19 ANGLE     m_anValue       "Value Angle"     = 0,
  20 ANGLE3D   m_an3dValue     "Value Angle 3D"  = ANGLE3D(0, 0, 0),

  40 enum EOperation m_eOperation  "Operation"      = EO_SET,

  // Source/Target settings start here.
  41 CEntityPointer  m_penSource   "Source Target" COLOR(C_CYAN|0xFF),
  42 CEntityPointer  m_penTarget   "Target" COLOR(C_RED|0xFF),

  43 CTString  m_strSourceProperty "Source Property Name" = "",
  44 CTString  m_strTargetProperty "Target Property Name" = "",
  45 enum EConType  m_eSourcePT    "Source Property Type" = ECT_ENTITY,
  46 enum EConType  m_eTargetPT    "Target Property Type" = ECT_ENTITY,

  50 BOOL m_bPenCausedAsSource     "Source Target=penCaused" = FALSE,
  51 BOOL m_bPenCausedAsTarget     "Target=penCaused" = FALSE,
  52 BOOL m_bPenCausedAsTargetValue "Value Target=penCaused" = FALSE,

components:
  1 model   MODEL_MARKER     "Models\\Editor\\PropertyChanger.mdl",
  2 texture TEXTURE_MARKER   "Models\\Editor\\PropertyChanger.tex",

functions:
  // --------------------------------------------------------------------------------------
  // Returns short description to show it in the SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const
  {
    ((CTString&)m_strDescription).PrintF("-><none>");

    if (m_penTarget != NULL) {
      ((CTString&)m_strDescription).PrintF("->%s", m_penTarget->GetName());
    }
    
    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Returns a string.
  // --------------------------------------------------------------------------------------
  void GetBinaryStringFromIndex(CTString &strValue, INDEX iValue)
  {
    for(int i = 0; i <= 31; i++)
    {
      if ((iValue&(1 << i)) >= 1) {
        strValue = "1" + strValue;
      } else {
        strValue = "0" + strValue;
      }
    }
  }
  
  FLOAT GeNonEPFloatSource()
  {
    // Position and orientation BEGIN.
    if (m_eSourcePT == ECT_POSX) {
      return m_penSource->GetPlacement().pl_PositionVector(1);

    } else if (m_eSourcePT == ECT_POSY) {
      return m_penSource->GetPlacement().pl_PositionVector(2);

    } else if (m_eSourcePT == ECT_POSZ) {
      return m_penSource->GetPlacement().pl_PositionVector(3);

    } else if (m_eSourcePT == ECT_ROTH) {
      return m_penSource->GetPlacement().pl_OrientationAngle(1);

    } else if (m_eSourcePT == ECT_ROTP) {
      return m_penSource->GetPlacement().pl_OrientationAngle(2);

    } else if (m_eSourcePT == ECT_ROTB) {
      return m_penSource->GetPlacement().pl_OrientationAngle(3);
    }
    // Position and Orientation END.

    BOOL bIsMovableEntity = IsDerivedFromClass(m_penSource, "MovableEntity");
    BOOL bError = FALSE;

    if (m_eSourcePT == ECT_SPEEDX) {
      if (bIsMovableEntity) {
        return ((CMovableEntity&)*m_penSource).en_vCurrentTranslationAbsolute(1);
      } else {
        bError = TRUE;
      }
    } else if (m_eSourcePT == ECT_SPEEDY) {
      if (bIsMovableEntity) {
        return ((CMovableEntity&)*m_penSource).en_vCurrentTranslationAbsolute(2);
      } else {
        bError = TRUE;
      }
    } else if (m_eSourcePT == ECT_SPEEDZ) {
      if (bIsMovableEntity) {
        return ((CMovableEntity&)*m_penSource).en_vCurrentTranslationAbsolute(3);
      } else {
        bError = TRUE;
      }
    } else if (m_eSourcePT == ECT_SPEEDALL) {
      if (bIsMovableEntity) {
        return ((CMovableEntity&)*m_penSource).en_vCurrentTranslationAbsolute.Length();
      } else {
        bError = TRUE;
      }
    } else if (m_eSourcePT == ECT_SPEEDXREL) {
      if (bIsMovableEntity) {
        CPlacement3D plSpeed = CPlacement3D(((CMovableEntity&)*m_penSource).en_vCurrentTranslationAbsolute, ANGLE3D(0, 0, 0));
        CPlacement3D plRot = CPlacement3D(FLOAT3D(0, 0, 0), m_penSource->GetPlacement().pl_OrientationAngle);
        plSpeed.AbsoluteToRelative(plRot);
        return plSpeed.pl_PositionVector(1);
      } else {
        bError = TRUE;
      }
    } else if (m_eSourcePT == ECT_SPEEDYREL) {
      if (bIsMovableEntity) {
        CPlacement3D plSpeed = CPlacement3D(((CMovableEntity&)*m_penSource).en_vCurrentTranslationAbsolute, ANGLE3D(0, 0, 0));
        CPlacement3D plRot = CPlacement3D(FLOAT3D(0, 0, 0), m_penSource->GetPlacement().pl_OrientationAngle);
        plSpeed.AbsoluteToRelative(plRot);
        return plSpeed.pl_PositionVector(2);
      } else {
        bError = TRUE;
      }
    } else if (m_eSourcePT == ECT_SPEEDZREL) {
      if (bIsMovableEntity) {
        CPlacement3D plSpeed = CPlacement3D(((CMovableEntity&)*m_penSource).en_vCurrentTranslationAbsolute, ANGLE3D(0, 0, 0));
        CPlacement3D plRot = CPlacement3D(FLOAT3D(0, 0, 0), m_penSource->GetPlacement().pl_OrientationAngle);
        plSpeed.AbsoluteToRelative(plRot);
        return plSpeed.pl_PositionVector(3);
      } else {
        bError = TRUE;
      }
    }

    if (bError) {
      if (m_bDebugMessages) {
        CPrintF(TRANS("%s : Don't try to receive speeds from non-moving entities!\n"), m_strName);
      }

      return 0.0F;
    }

    if (m_eSourcePT == ECT_HEALTH) {
      return ((CLiveEntity&)*m_penSource).GetHealth();
    } else {
      CPrintF("%s : Shit happened with FSource! Tell to DEV!\n", GetName());
      return 0.0F;
    }
  }

  // --------------------------------------------------------------------------------------
  // This code executes on Trigger Event.
  // --------------------------------------------------------------------------------------
  void HandleETrigger(const CEntityEvent &ee)
  {
    // If non-active.
    if (!m_bActive) {
      return;
    }

    ETrigger eTrigger = ((ETrigger &) ee);

    //
    if (m_bPenCausedAsSource && eTrigger.penCaused) {
      m_penSource = eTrigger.penCaused;
    }

    // Target=penCaused
    if (m_bPenCausedAsTarget && eTrigger.penCaused) {
      m_penTarget = eTrigger.penCaused;
    }

    // If invalid target.
    if (m_penTarget == NULL) {
      return;
    }

    // Get the target property.
    CEntityProperty *pTargetProperty = NULL;
    BOOL bTargetIsProperty = m_eTargetPT == ECT_ENTITY || m_eTargetPT == ECT_PROPBYID;

    if (bTargetIsProperty && m_strTargetProperty.Length() == 0)
    {
      if (m_bDebugMessages) {
        CPrintF(TRANS("[PC] %s : [Target Property Name] is empty!\n"), m_strName);
      }

      return;
    }

    if (m_eTargetPT == ECT_PROPBYID)
    {
      INDEX iTargetPropID = atoi(m_strTargetProperty);
      
      if (iTargetPropID <= 0)
      {
        if (m_bDebugMessages) {
          CPrintF(TRANS("[PC] %s : '%s' is not a valid number for property ID!\n"), m_strName, m_strTargetProperty);
        }

        return;
      } else {
        if (m_bDebugMessages) {
          CPrintF(TRANS("[PC] %s : Target property ID = %d\n"), m_strName, iTargetPropID);
        }
      }
      
      for (CDLLEntityClass *pdecDLLClass = m_penTarget->en_pecClass->ec_pdecDLLClass; pdecDLLClass != NULL; pdecDLLClass = pdecDLLClass->dec_pdecBase)
      {
        for (INDEX iProperty = 0; iProperty < pdecDLLClass->dec_ctProperties; iProperty++)
        {
          // if it has that same identifier
          if (pdecDLLClass->dec_aepProperties[iProperty].ep_ulID == iTargetPropID) {
            pTargetProperty = &pdecDLLClass->dec_aepProperties[iProperty];
            break;
          }
        }
        
        if (pTargetProperty != NULL) {
          break;
        }
      }
      
    } else {
      pTargetProperty = m_penTarget->PropertyForName(m_strTargetProperty);
    }
    

    // If invalid property.
    if (pTargetProperty == NULL && m_eTargetPT == ECT_ENTITY) { 
      if (m_bDebugMessages) {
        CPrintF(TRANS("[PC] %s : Error! Property with name '%s' not found!\n"), m_strName, m_strTargetProperty);
      }

      return;

    } else if (pTargetProperty == NULL && m_eTargetPT == ECT_PROPBYID) {
      if (m_bDebugMessages) {
        CPrintF(TRANS("[PC] %s : Error! Property with ID '%s' not found!\n"), m_strName, m_strTargetProperty);
      }
      
      return;
    }
    
    if (pTargetProperty->ep_ulFlags & EPROPF_READONLY) {
      if (m_bDebugMessages) {
        CPrintF(TRANS("[PC] %s : Error! Target Entity Property is a read-only property!\n"), m_strName);
      }

      return;
    }

    if (m_bDebugMessages && pTargetProperty)
    {
      CPrintF(TRANS("[PC] %s : Target Entity Property Type is %s\n"), m_strName, GetNameForEPT(pTargetProperty->ep_eptType));
    }
      
    CEntityProperty *pSourceProperty = NULL;
    BOOL bSourceIsProperty = m_eSourcePT == ECT_ENTITY || m_eSourcePT == ECT_PROPBYID;

    // If we have source entity and source procperty type is entity property. Then receive its property!
    if (m_penSource != NULL)
    {
      if (m_eSourcePT == ECT_PROPBYID) {
        INDEX iSourcePropID = atoi(m_strSourceProperty);
        
        if (iSourcePropID <= 0)
        {
          if (m_bDebugMessages) {
            CPrintF(TRANS("[PC] %s : '%s' is not a valid number for source property ID!\n"), m_strName, m_strSourceProperty);
          }

          return;
        } else {
          if (m_bDebugMessages) {
            CPrintF(TRANS("[PC] %s : Source property ID = %d\n"), m_strName, iSourcePropID);
          }
        }
        
        for (CDLLEntityClass *pdecDLLClass = m_penSource->en_pecClass->ec_pdecDLLClass; pdecDLLClass != NULL; pdecDLLClass = pdecDLLClass->dec_pdecBase)
        {
          for (INDEX iProperty = 0; iProperty < pdecDLLClass->dec_ctProperties; iProperty++)
          {
            // if it has that same identifier
            if (pdecDLLClass->dec_aepProperties[iProperty].ep_ulID == iSourcePropID) {
              pSourceProperty = &pdecDLLClass->dec_aepProperties[iProperty];
              break;
            }
          }
          
          if (pSourceProperty != NULL) {
            break;
          }
        }
      } else if (m_eSourcePT == ECT_ENTITY) {
        pSourceProperty = m_penSource->PropertyForName(m_strSourceProperty);
      }
    }
    
    if (m_bDebugMessages && pSourceProperty)
    {
      CPrintF(TRANS("[PC] %s : Source Entity Property Type is %s\n"), m_strName, GetNameForEPT(pSourceProperty->ep_eptType));
    }

    if (bTargetIsProperty) {
      SLONG offset = pTargetProperty->ep_slOffset; 
      CEntityProperty::PropertyType eptType = pTargetProperty->ep_eptType;

      // FLOAT
      if (eptType == CEntityProperty::EPT_FLOAT) {
        FLOAT *fValue = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penTarget) + offset)); 
        FLOAT fOld = *fValue;
        FLOAT *fNew = &m_fValue;

        // If we have source target.
        if (m_penSource != NULL) {
          // If property is entity property.
          if (bSourceIsProperty) {
            // If entity property exists.
            if (pSourceProperty != NULL) {
              CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
              
              // Here is support for both FLOAT and INDEX.
              if (eptSourceType == CEntityProperty::EPT_FLOAT) {
                SLONG offset1 = pSourceProperty->ep_slOffset; 
                fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
              } else if (eptSourceType == CEntityProperty::EPT_INDEX || eptSourceType == CEntityProperty::EPT_ENUM) {
                SLONG offset1 = pSourceProperty->ep_slOffset;
                *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
              }
            }
          // If source type isn't classname.
          } else if (m_eSourcePT != ECT_TYPE) {
            FLOAT fSrc = GeNonEPFloatSource();
            *fNew = *((FLOAT*)&fSrc);
          }
        }
        
        BOOL bErrorOccured = FALSE;

        if (m_eOperation == EO_SET) {
          *fValue = *fNew;

        } else if (m_eOperation == EO_ADD) {
          *fValue += *fNew;

        } else if (m_eOperation == EO_SUBSTRACT) {
          *fValue -= *fNew;

        } else if (m_eOperation == EO_MULTIPLY) {
          *fValue *= *fNew;

        } else if (m_eOperation == EO_DIVIDE && (*fNew != 0.0F)) {
          *fValue /= *fNew;
          
        } else if (m_eOperation == EO_REMAINDER && (*fNew != 0.0F)) {
          *fValue =  fmod(*fValue, *fNew);

        } else {
          bErrorOccured = TRUE;
        }
        
        if (m_bDebugMessages && !bErrorOccured)
        {
          CPrintF(TRANS("[PC] %s : Target Entity Property: [%s].[%s] is currently (FLOAT)%f\n"), m_strName, ((CEntity&)*m_penTarget).GetName(), m_strTargetProperty, fOld);

          
          if (m_penSource != NULL && pSourceProperty != NULL) {
            CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
            
            if (eptSourceType == CEntityProperty::EPT_FLOAT || eptSourceType == CEntityProperty::EPT_INDEX || eptSourceType == CEntityProperty::EPT_ENUM) {
              CPrintF(TRANS("[PC] %s : Source Entity Property: [%s].[%s] is currently (FLOAT)%f\n"), m_strName, ((CEntity&)*m_penSource).GetName(), m_strSourceProperty, *fNew);
            } else {
              CPrintF(TRANS("[PC] %s : Source Entity Property: [%s].[%s] have incompatable type!\n"), m_strName, ((CEntity&)*m_penSource).GetName(), m_strSourceProperty);
              CPrintF(TRANS("[PC] %s : Using [Value Float] Instead: (FLOAT)%F\n"), m_strName, m_fValue);
            }

          } else {
            CPrintF(TRANS("[PC] %s : Source Value: (FLOAT)%F\n"), m_strName, m_fValue);
          }

          if (m_eOperation == EO_SET) {
            CPrintF(TRANS("  Expression : [%s] = (FLOAT)%f\n"), m_strTargetProperty, *fNew);
            CPrintF(TRANS("  Result = %f\n"), *fValue);

          } else if (m_eOperation == EO_ADD) {
            CPrintF(TRANS("  Expression : [%s] = (FLOAT)%f + (FLOAT)%f\n"), m_strTargetProperty, fOld, *fNew);
            CPrintF(TRANS("  Result = %f\n"), *fValue);

          } else if (m_eOperation == EO_SUBSTRACT) {
            CPrintF(TRANS("  Expression : [%s] = (FLOAT)%f - (FLOAT)%f\n"), m_strTargetProperty, fOld, *fNew);
            CPrintF(TRANS("  Result = %f\n"), *fValue);

          } else if (m_eOperation == EO_MULTIPLY) {
            CPrintF(TRANS("  Expression : [%s] = (FLOAT)%f * (FLOAT)%f\n"), m_strTargetProperty, fOld, *fNew);
            CPrintF(TRANS("  Result = %f\n"), *fValue);

          } else if (m_eOperation == EO_DIVIDE) {
            CPrintF(TRANS("  Expression : [%s] = (FLOAT)%f / (FLOAT)%f\n"), m_strTargetProperty, fOld, *fNew);
            CPrintF(TRANS("  Result = %f\n"), *fValue);

          } else if (m_eOperation == EO_REMAINDER) {
            CPrintF(TRANS("  Expression : [%s] = (FLOAT)%f FMOD(%%) (FLOAT)%f\n"), m_strTargetProperty, fOld, *fNew);
            CPrintF(TRANS("  Result = %f\n"), *fValue);
          }
        }
          
      // CEntityPointer
      } else if (eptType == CEntityProperty::EPT_ENTITYPTR) {
        CEntityPointer *penPointer = ((CEntityPointer *)(((UBYTE *)(CEntity*)&*m_penTarget) + offset)); 
        CEntityPointer penOld = *penPointer;
        CEntityPointer *penNew = &m_penValue;
        
        if (m_penSource != NULL && bSourceIsProperty && pSourceProperty != NULL && pSourceProperty->ep_eptType == CEntityProperty::EPT_ENTITYPTR) {
          SLONG offset1 = pSourceProperty->ep_slOffset; 
          penNew = ((CEntityPointer *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
        }

        if (m_bPenCausedAsTargetValue && eTrigger.penCaused) {
          penNew = &eTrigger.penCaused;
        }

        *penPointer = *penNew;

        if (m_bDebugMessages) {
          CTString strName1 = "NULL";
          CTString strName2 = "NULL";

          if (penOld) {
            strName1 = ((CEntity&)*penOld).GetName();
          }

          if (*penNew) {
            strName2 = ((CEntity&)**penNew).GetName();
          }

          CPrintF(TRANS("%s : changing Pointer %s from %s to %s.\n"), m_strName, m_strTargetProperty, strName1, strName2);
        }

      // INDEX
      } else if (IsIndexEPT(eptType)) {
        INDEX *iValue = ((INDEX *)(((UBYTE *)(CEntity*)&*m_penTarget) + offset)); 
        INDEX iOld = *iValue;
        INDEX *iNew = &m_iValue;

        // If we have source target.
        if (m_penSource != NULL)
        {
          // If property is entity property.
          if (bSourceIsProperty) {
            // If entity property exists.
            if (pSourceProperty != NULL) {
              CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
              
              // Here is support for both INDEX and FLOAT.
              if (IsIndexEPT(eptSourceType)) {
                SLONG offset1 = pSourceProperty->ep_slOffset; 
                iNew = ((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 

              } else if (eptSourceType == CEntityProperty::EPT_FLOAT) {
                SLONG offset1 = pSourceProperty->ep_slOffset;
                *iNew = *((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
              }
            }

          // If source type isn't classname.
          } else if (m_eSourcePT != ECT_TYPE) {
            FLOAT fSrc = GeNonEPFloatSource();
            *iNew = *((FLOAT *)&fSrc);
          }
        }

        BOOL bErrorOccured = FALSE;

        if (m_eOperation == EO_SET) {
          *iValue = *iNew;

        } else if (m_eOperation == EO_ADD) {
          *iValue += *iNew;

        } else if (m_eOperation == EO_SUBSTRACT) {
          *iValue -= *iNew;

        } else if (m_eOperation == EO_MULTIPLY) {
          *iValue *= *iNew;

        } else if (m_eOperation == EO_DIVIDE && *iNew != 0) {
          *iValue /= *iNew;
          
        } else if (m_eOperation == EO_REMAINDER && *iNew != 0) {
          *iValue %= *iNew;
          
        } else if (m_eOperation == EO_BITAND) {
          *iValue &= *iNew;

        } else if (m_eOperation == EO_BITOR) {
          *iValue |= *iNew;
          
        } else if (m_eOperation == EO_XOR) {
          *iValue ^= *iNew;

        // BITWISE | and ~
        } else if (m_eOperation == EO_INCLUDEBIT || m_eOperation == EO_EXCLUDEBIT) {
          if ((*iNew) >= 0 && (*iNew) < 32) {
            if (m_eOperation == EO_INCLUDEBIT) {
              *iValue |= 1 << (*iNew);
            } else {
              *iValue &= ~(1 << (*iNew));
            }
          } else {
            if (m_bDebugMessages) {
              CPrintF(TRANS("[PC] %s : Error! Out of bounds while using bitwise operation.\n  Bounds: [0..31]\nValue: %d\n"), m_strName, m_iValue);
              bErrorOccured = TRUE;
            }
          }

        } else {
          if (m_bDebugMessages) {
            CPrintF(TRANS("[PC] %s : Error! DO NOT DIVIDE THROUGH 0!\n"), m_strName);
            bErrorOccured = TRUE;
          }
        }

        // If debug messages enabled and no errors then printout some text into console.
        if (m_bDebugMessages && !bErrorOccured)
        {
          CPrintF(TRANS("[PC] %s : Target Entity Property: [%s].[%s] is currently (%s)%d\n"), m_strName, ((CEntity&)*m_penTarget).GetName(), m_strTargetProperty, GetNameForEPT(eptType), iOld);

          if (m_penSource != NULL && pSourceProperty != NULL) {
            CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;

            if (IsIndexEPT(eptSourceType)) {
              CPrintF(TRANS("[PC] %s : Source Entity Property: [%s].[%s] is currently (%s)%d\n"), m_strName, ((CEntity&)*m_penSource).GetName(), m_strSourceProperty, GetNameForEPT(eptSourceType), *iNew);
            
            } else if (eptSourceType == CEntityProperty::EPT_FLOAT) {
              CPrintF(TRANS("[PC] %s : Source Entity Property: [%s].[%s] is currently (INDEX)%d\n"), m_strName, ((CEntity&)*m_penSource).GetName(), m_strSourceProperty, *iNew);

            } else  {
              CPrintF(TRANS("[PC] %s : Source Entity Property: [%s].[%s] have incompatable type!\n"), m_strName, ((CEntity&)*m_penSource).GetName(), m_strSourceProperty);
              CPrintF(TRANS("[PC] %s : Using [Source Value] Instead: (INDEX)%d\n"), m_strName, m_iValue);
            }
          } else {
            CPrintF(TRANS("  Source Value = (INDEX)%d\n"), m_iValue);
          }

          if (m_eOperation == EO_SET) {
            CPrintF(TRANS("  Expression : [%s] = (INDEX)%d\n"), m_strTargetProperty, *iNew);
            CPrintF(TRANS("  Result = %d\n"), *iValue);

          } else if (m_eOperation == EO_ADD) {
            CPrintF(TRANS("  Expression : [%s] = (%s)%d + (INDEX)%d\n"), m_strTargetProperty, GetNameForEPT(eptType), iOld, *iNew);
            CPrintF(TRANS("  Result = %d\n"), *iValue);

          } else if (m_eOperation == EO_SUBSTRACT) {
            CPrintF(TRANS("  Expression : [%s] = (%s)%d - (INDEX)%d\n"), m_strTargetProperty, GetNameForEPT(eptType), iOld, *iNew);
            CPrintF(TRANS("  Result = %d\n"), *iValue);

          } else if (m_eOperation == EO_MULTIPLY) {
            CPrintF(TRANS("  Expression : [%s] = (%s)%d * (INDEX)%d\n"), m_strTargetProperty, GetNameForEPT(eptType), iOld, *iNew);
            CPrintF(TRANS("  Result = %d\n"), *iValue);

          } else if (m_eOperation == EO_DIVIDE) {
            CPrintF(TRANS("  Expression : [%s] = (%s)%d / (INDEX)%d\n"), m_strTargetProperty, GetNameForEPT(eptType), iOld, *iNew);
            CPrintF(TRANS("  Result = %d\n"), *iValue);

          } else if (m_eOperation == EO_REMAINDER) {
            CPrintF(TRANS("  Expression : [%s] = (%s)%d %% (INDEX)%d\n"), m_strTargetProperty, GetNameForEPT(eptType), iOld, *iNew);
            CPrintF(TRANS("  Result = %d\n"), *iValue);

          } else if (m_eOperation == EO_INCLUDEBIT || m_eOperation == EO_EXCLUDEBIT || m_eOperation == EO_BITAND || m_eOperation == EO_BITOR || m_eOperation == EO_XOR) {

            CTString strBinaryOld("");
            CTString strBinaryNew("");
            CTString strBinaryResult("");

            GetBinaryStringFromIndex(strBinaryOld, iOld);
            GetBinaryStringFromIndex(strBinaryResult, *iValue);
            
            INDEX iNewNew = *iNew;

            if (m_eOperation == EO_INCLUDEBIT) {
              CPrintF(TRANS("  Expression : [%s] = (%s)%d | (1 << %d)\n"), m_strTargetProperty, GetNameForEPT(eptType), iOld, *iNew);
              iNewNew = 1 << *iNew;

            } else if (m_eOperation == EO_EXCLUDEBIT) {
              CPrintF(TRANS("  Expression : [%s] = (%s)%d &~ (1 << %d)\n"), m_strTargetProperty, GetNameForEPT(eptType), iOld, *iNew);
              iNewNew = 1 << *iNew;

            } else if (m_eOperation == EO_BITOR) {
              CPrintF(TRANS("  Expression : [%s] = (%s)%d | (INDEX)%d\n"), m_strTargetProperty, GetNameForEPT(eptType), iOld, *iNew);

            } else if (m_eOperation == EO_BITAND) {
              CPrintF(TRANS("  Expression : [%s] = (%s)%d & (INDEX)%d\n"), m_strTargetProperty, GetNameForEPT(eptType), iOld, *iNew);

            } else {
              CPrintF(TRANS("  Expression : [%s] = (%s)%d XOR (INDEX)%d\n"), m_strTargetProperty, GetNameForEPT(eptType), iOld, *iNew);
            }

            GetBinaryStringFromIndex(strBinaryNew, iNewNew);

            CPrintF("  OLD = |%s| = %d\n", strBinaryOld, iOld);
            CPrintF("  MOD = |%s| = %d\n", strBinaryNew, *iNew);
            CPrintF("        |--------------------------------|\n");
            CPrintF("  OUT = |%s| = %d\n", strBinaryResult, *iValue);
          }
        }

      // BOOL
      } else if (eptType == CEntityProperty::EPT_BOOL) {
        BOOL *bValue = ((BOOL *)(((UBYTE *)(CEntity*)&*m_penTarget) + offset)); 
        BOOL *bNew = &m_bValue;

        if (m_penSource != NULL && pSourceProperty != NULL && pSourceProperty->ep_eptType == CEntityProperty::EPT_BOOL) {
          SLONG offset1 = pSourceProperty->ep_slOffset; 
          bNew = ((BOOL *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
        }

        if (m_eOperation == EO_MULTIPLY) {
          *bValue = !bValue;
        } else {
          *bValue = *bNew;
        }

      // CTString
      } else if (eptType == CEntityProperty::EPT_STRING) {
        CTString *strValue = ((CTString *)(((UBYTE *)(CEntity*)&*m_penTarget) + offset)); 
        CTString *strNew = &m_strValue;
        
        if (m_penSource != NULL)
        {
          if (bSourceIsProperty && pSourceProperty != NULL && pSourceProperty->ep_eptType == CEntityProperty::EPT_STRING) {
            SLONG offset1 = pSourceProperty->ep_slOffset; 
            strNew = ((CTString *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
          } else if (m_eSourcePT == ECT_TYPE) {
            CTString strVal = m_penSource->GetClass()->ec_pdecDLLClass->dec_strName;
            strNew = &strVal;
          }
        }

        if (m_eOperation == EO_ADD) {
          *strValue += *strNew;
        } else {
          *strValue = *strNew;
        }

      // RANGE
      } else if (eptType == CEntityProperty::EPT_RANGE) {
        RANGE *rValue = ((RANGE *)(((UBYTE *)(CEntity*)&*m_penTarget) + offset)); 
        RANGE *rNew = &m_rValue;

        if (m_penSource != NULL && pSourceProperty != NULL && pSourceProperty->ep_eptType == CEntityProperty::EPT_RANGE) {
          SLONG offset1 = pSourceProperty->ep_slOffset; 
          rNew = ((RANGE *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
        }

        if (m_eOperation== EO_SET) {
          *rValue = *rNew;
        } else if (m_eOperation == EO_ADD) {
          *rValue += *rNew;
        } else if (m_eOperation == EO_SUBSTRACT) {
          *rValue -= *rNew;
        } else if (m_eOperation == EO_MULTIPLY) {
          *rValue *= *rNew;
        } else if (m_eOperation == EO_DIVIDE && m_rValue != 0) {
          *rValue /= *rNew;
        } else {
          if (m_bDebugMessages) {
            CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
          }
        }

      // Filename
      } else if (eptType == CEntityProperty::EPT_FILENAME) {
        CTFileName *fnValue = ((CTFileName *)(((UBYTE *)(CEntity*)&*m_penTarget)+offset)); 
        CTFileName *fnNew = &m_fnValue;

        if (m_penSource != NULL && pSourceProperty != NULL && pSourceProperty->ep_eptType == CEntityProperty::EPT_FILENAME) {
          SLONG offset1 = pSourceProperty->ep_slOffset; 
          fnNew = ((CTFileName *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
        }

        *fnValue = *fnNew;

      // ANGLE
      } else if (eptType == CEntityProperty::EPT_ANGLE) {
        ANGLE *anValue = ((ANGLE *)(((UBYTE *)(CEntity*)&*m_penTarget) + offset)); 
        ANGLE *anNew = &m_anValue;

        if (m_penSource != NULL && pSourceProperty != NULL && pSourceProperty->ep_eptType == CEntityProperty::EPT_ANGLE) {
          SLONG offset1 = pSourceProperty->ep_slOffset; 
          anNew = ((ANGLE *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
        }

        if (m_eOperation == EO_SET) {
          *anValue = *anNew;
        } else if (m_eOperation == EO_ADD) {
          *anValue += *anNew;
        } else if (m_eOperation == EO_SUBSTRACT) {
          *anValue -= *anNew;
        } else if (m_eOperation == EO_MULTIPLY) {
          *anValue *= *anNew;
        } else if (m_eOperation == EO_DIVIDE && m_anValue!=0) {
          *anValue /= *anNew;
        } else {
          if (m_bDebugMessages) {
            CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
          }
        }

      // ANGLE3D
      } else if (eptType == CEntityProperty::EPT_ANGLE3D) {
        ANGLE3D *an3dValue = ((ANGLE3D *)(((UBYTE *)(CEntity*)&*m_penTarget)+offset)); 
        ANGLE3D *anNew = &m_an3dValue;

        if (m_penSource != NULL && pSourceProperty != NULL && pSourceProperty->ep_eptType == CEntityProperty::EPT_ANGLE3D) {
          SLONG offset1 = pSourceProperty->ep_slOffset; 
          anNew = ((ANGLE3D *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
        }

        if (m_eOperation == EO_ADD) {
          *an3dValue += *anNew;
        } else if (m_eOperation == EO_SUBSTRACT) {
          *an3dValue -= *anNew;
        } else if (m_eOperation == EO_MULTIPLY) {
          *an3dValue *= *anNew;
        } else {
          *an3dValue = *anNew;
        }
      } else {
        if (m_bDebugMessages) {
          CPrintF("[PC] %s : Unknown target property type!\n", m_strName);
        }
      }

    } else if (m_eTargetPT == ECT_POSX) {
      FLOAT *fValue = &((CEntity&)*m_penTarget).en_plPlacement.pl_PositionVector(1); 
      FLOAT fOld = *fValue;
      FLOAT *fNew = &m_fValue;

      // If we have source target.
      if (m_penSource != NULL) {
        // If property is entity property.
        if (bSourceIsProperty) {
          // If entity property exists.
          if (pSourceProperty != NULL) {
            CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
            
            // Here is support for both FLOAT and INDEX.
            if (eptSourceType == CEntityProperty::EPT_FLOAT) {
              SLONG offset1 = pSourceProperty->ep_slOffset; 
              fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
            } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
              SLONG offset1 = pSourceProperty->ep_slOffset;
              *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
            }
          }
        // If source type isn't classname.
        } else if (m_eSourcePT != ECT_TYPE) {
          FLOAT fSrc = GeNonEPFloatSource();
          fNew = &fSrc;
        }
      }

      if (m_bDebugMessages) {
        CPrintF("%s : Modifing target PosX with value=%f.\n", GetName(), *fNew);
      }

      if (m_eOperation == EO_SET) {
        *fValue = *fNew;
      } else if (m_eOperation == EO_ADD) {
        *fValue += *fNew;
      } else if (m_eOperation == EO_SUBSTRACT) {
        *fValue -= *fNew;
      } else if (m_eOperation == EO_MULTIPLY) {
        *fValue *= *fNew;
      } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
        *fValue /= *fNew;
      } else {
        if (m_bDebugMessages) {
          CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
        }
      }

    } else if (m_eTargetPT == ECT_POSY) {
      FLOAT *fValue = &((CEntity&)*m_penTarget).en_plPlacement.pl_PositionVector(2); 
      FLOAT fOld = *fValue;
      FLOAT *fNew = &m_fValue;

      // If we have source target.
      if (m_penSource != NULL) {
        // If property is entity property.
        if (bSourceIsProperty) {
          // If entity property exists.
          if (pSourceProperty != NULL) {
            CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
            
            // Here is support for both FLOAT and INDEX.
            if (eptSourceType == CEntityProperty::EPT_FLOAT) {
              SLONG offset1 = pSourceProperty->ep_slOffset; 
              fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
            } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
              SLONG offset1 = pSourceProperty->ep_slOffset;
              *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
            }
          }
        // If source type isn't classname.
        } else if (m_eSourcePT != ECT_TYPE) {
          FLOAT fSrc = GeNonEPFloatSource();
          fNew = &fSrc;
        }
      }

      if (m_bDebugMessages) {
        CPrintF("%s : Modifing target PosY with value=%f.\n", GetName(), *fNew);
      }

      if (m_eOperation == EO_SET) {
        *fValue = *fNew;
      } else if (m_eOperation == EO_ADD) {
        *fValue += *fNew;
      } else if (m_eOperation == EO_SUBSTRACT) {
        *fValue -= *fNew;
      } else if (m_eOperation == EO_MULTIPLY) {
        *fValue *= *fNew;
      } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
        *fValue /= *fNew;
      } else {
        if (m_bDebugMessages) {
          CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
        }
      }

    } else if (m_eTargetPT == ECT_POSZ) {
      FLOAT *fValue = &((CEntity&)*m_penTarget).en_plPlacement.pl_PositionVector(3); 
      FLOAT fOld = *fValue;
      FLOAT *fNew = &m_fValue;

      // If we have source target.
      if (m_penSource != NULL) {
        // If property is entity property.
        if (bSourceIsProperty) {
          // If entity property exists.
          if (pSourceProperty != NULL) {
            CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
            
            // Here is support for both FLOAT and INDEX.
            if (eptSourceType == CEntityProperty::EPT_FLOAT) {
              SLONG offset1 = pSourceProperty->ep_slOffset; 
              fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
            } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
              SLONG offset1 = pSourceProperty->ep_slOffset;
              *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
            }
          }
        // If source type isn't classname.
        } else if (m_eSourcePT != ECT_TYPE) {
          FLOAT fSrc = GeNonEPFloatSource();
          fNew = &fSrc;
        }
      }

      if (m_bDebugMessages) {
        CPrintF("%s : Modifing target PosZ with value=%f.\n", GetName(), *fNew);
      }

      if (m_eOperation == EO_SET) {
        *fValue = *fNew;
      } else if (m_eOperation == EO_ADD) {
        *fValue += *fNew;
      } else if (m_eOperation == EO_SUBSTRACT) {
        *fValue -= *fNew;
      } else if (m_eOperation == EO_MULTIPLY) {
        *fValue *= *fNew;
      } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
        *fValue /= *fNew;
      } else {
        if (m_bDebugMessages) {
          CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
        }
      }

    } else if (m_eTargetPT == ECT_ROTH) {
      FLOAT *fValue = &((CEntity&)*m_penTarget).en_plPlacement.pl_OrientationAngle(1); 
      FLOAT fOld = *fValue;
      FLOAT *fNew = &m_fValue;

      // If we have source target.
      if (m_penSource != NULL) {
        // If property is entity property.
        if (bSourceIsProperty) {
          // If entity property exists.
          if (pSourceProperty != NULL) {
            CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
            
            // Here is support for both FLOAT and INDEX.
            if (eptSourceType == CEntityProperty::EPT_FLOAT) {
              SLONG offset1 = pSourceProperty->ep_slOffset; 
              fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
            } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
              SLONG offset1 = pSourceProperty->ep_slOffset;
              *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
            }
          }
        // If source type isn't classname.
        } else if (m_eSourcePT != ECT_TYPE) {
          FLOAT fSrc = GeNonEPFloatSource();
          fNew = &fSrc;
        }
      }

      if (m_bDebugMessages) {
        CPrintF("%s : Modifing target RotH with value=%f.\n", GetName(), *fNew);
      }

      if (m_eOperation == EO_SET) {
        *fValue = *fNew;
      } else if (m_eOperation == EO_ADD) {
        *fValue += *fNew;
      } else if (m_eOperation == EO_SUBSTRACT) {
        *fValue -= *fNew;
      } else if (m_eOperation == EO_MULTIPLY) {
        *fValue *= *fNew;
      } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
        *fValue /= *fNew;
      } else {
        if (m_bDebugMessages) {
          CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
        }
      }

    } else if (m_eTargetPT == ECT_ROTP) {
      FLOAT *fValue = &((CEntity&)*m_penTarget).en_plPlacement.pl_OrientationAngle(2); 
      FLOAT fOld = *fValue;
      FLOAT *fNew = &m_fValue;

      // If we have source target.
      if (m_penSource != NULL) {
        // If property is entity property.
        if (bSourceIsProperty) {
          // If entity property exists.
          if (pSourceProperty != NULL) {
            CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
            
            // Here is support for both FLOAT and INDEX.
            if (eptSourceType == CEntityProperty::EPT_FLOAT) {
              SLONG offset1 = pSourceProperty->ep_slOffset; 
              fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
            } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
              SLONG offset1 = pSourceProperty->ep_slOffset;
              *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
            }
          }
        // If source type isn't classname.
        } else if (m_eSourcePT != ECT_TYPE) {
          FLOAT fSrc = GeNonEPFloatSource();
          fNew = &fSrc;
        }
      }

      if (m_bDebugMessages) {
        CPrintF("%s : Modifing target RotP with value=%f.\n", GetName(), *fNew);
      }

      if (m_eOperation == EO_SET) {
        *fValue = *fNew;
      } else if (m_eOperation == EO_ADD) {
        *fValue += *fNew;
      } else if (m_eOperation == EO_SUBSTRACT) {
        *fValue -= *fNew;
      } else if (m_eOperation == EO_MULTIPLY) {
        *fValue *= *fNew;
      } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
        *fValue /= *fNew;
      } else {
        if (m_bDebugMessages) {
          CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
        }
      }

    } else if (m_eTargetPT == ECT_ROTB) {
      FLOAT *fValue = &((CEntity&)*m_penTarget).en_plPlacement.pl_OrientationAngle(3); 
      FLOAT fOld = *fValue;
      FLOAT *fNew = &m_fValue;

      // If we have source target.
      if (m_penSource != NULL) {
        // If property is entity property.
        if (bSourceIsProperty) {
          // If entity property exists.
          if (pSourceProperty != NULL) {
            CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
            
            // Here is support for both FLOAT and INDEX.
            if (eptSourceType == CEntityProperty::EPT_FLOAT) {
              SLONG offset1 = pSourceProperty->ep_slOffset; 
              fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
            } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
              SLONG offset1 = pSourceProperty->ep_slOffset;
              *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
            }
          }
        // If source type isn't classname.
        } else if (m_eSourcePT != ECT_TYPE) {
          FLOAT fSrc = GeNonEPFloatSource();
          fNew = &fSrc;
        }
      }

      if (m_bDebugMessages) {
        CPrintF("%s : Modifing target RotB with value=%f.\n", GetName(), *fNew);
      }

      if (m_eOperation == EO_SET) {
        *fValue = *fNew;
      } else if (m_eOperation == EO_ADD) {
        *fValue += *fNew;
      } else if (m_eOperation == EO_SUBSTRACT) {
        *fValue -= *fNew;
      } else if (m_eOperation == EO_MULTIPLY) {
        *fValue *= *fNew;
      } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
        *fValue /= *fNew;
      } else {
        if (m_bDebugMessages) {
          CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
        }
      }

    } else if (m_penTarget->GetFlags()&ENF_ALIVE && m_eTargetPT == ECT_HEALTH) {
      FLOAT fValue = ((CLiveEntity&)*m_penTarget).en_fHealth;
      FLOAT fOld = fValue;
      FLOAT *fNew = &m_fValue;

      // If we have source target.
      if (m_penSource != NULL) {
        // If property is entity property.
        if (bSourceIsProperty) {
          // If entity property exists.
          if (pSourceProperty != NULL) {
            CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
            
            // Here is support for both FLOAT and INDEX.
            if (eptSourceType == CEntityProperty::EPT_FLOAT) {
              SLONG offset1 = pSourceProperty->ep_slOffset; 
              fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
            } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
              SLONG offset1 = pSourceProperty->ep_slOffset;
              *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
            }
          }
        // If source type isn't classname.
        } else if (m_eSourcePT != ECT_TYPE) {
          FLOAT fSrc = GeNonEPFloatSource();
          fNew = &fSrc;
        }
      }

      if (m_bDebugMessages) {
        CPrintF("%s : Modifing target Health with value=%f.\n", GetName(), *fNew);
      }

      if (m_eOperation == EO_SET) {
        ((CLiveEntity&)*m_penTarget).SetHealth(*fNew);
      } else if (m_eOperation == EO_ADD) {
        ((CLiveEntity&)*m_penTarget).SetHealth(fValue + *fNew);
      } else if (m_eOperation == EO_SUBSTRACT) {
        ((CLiveEntity&)*m_penTarget).SetHealth(fValue - *fNew);
      } else if (m_eOperation == EO_MULTIPLY) {
        ((CLiveEntity&)*m_penTarget).SetHealth(fValue * *fNew);
      } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
        ((CLiveEntity&)*m_penTarget).SetHealth(fValue / *fNew);
      } else {
        if (m_bDebugMessages) {
          CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
        }
      }
    } else if (IsDerivedFromClass(m_penTarget, "MovableEntity")) {
      if (m_eTargetPT == ECT_SPEEDX) {
        FLOAT *fValue = &((CMovableEntity&)*m_penTarget).en_vCurrentTranslationAbsolute(1); 
        FLOAT fOld = *fValue;
        FLOAT *fNew = &m_fValue;

        // If we have source target.
        if (m_penSource != NULL) {
          // If property is entity property.
          if (bSourceIsProperty) {
            // If entity property exists.
            if (pSourceProperty != NULL) {
              CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
              
              // Here is support for both FLOAT and INDEX.
              if (eptSourceType == CEntityProperty::EPT_FLOAT) {
                SLONG offset1 = pSourceProperty->ep_slOffset; 
                fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
              } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
                SLONG offset1 = pSourceProperty->ep_slOffset;
                *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
              }
            }
          // If source type isn't classname.
          } else if (m_eSourcePT != ECT_TYPE) {
            FLOAT fSrc = GeNonEPFloatSource();
            fNew = &fSrc;
          }
        }

        if (m_bDebugMessages) {
          CPrintF("%s : Modifing target SpeedX with value=%f.\n", GetName(), *fNew);
        }

        if (m_eOperation == EO_SET) {
          *fValue = *fNew;
        } else if (m_eOperation == EO_ADD) {
          *fValue += *fNew;
        } else if (m_eOperation == EO_SUBSTRACT) {
          *fValue -= *fNew;
        } else if (m_eOperation == EO_MULTIPLY) {
          *fValue *= *fNew;
        } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
          *fValue /= *fNew;
        } else {
          if (m_bDebugMessages) {
            CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
          }
        }

        ((CMovableEntity&)*m_penTarget).AddToMovers();

      } else if (m_eTargetPT == ECT_SPEEDY) {
        FLOAT *fValue = &((CMovableEntity&)*m_penTarget).en_vCurrentTranslationAbsolute(2); 
        FLOAT fOld = *fValue;
        FLOAT *fNew = &m_fValue;

        // If we have source target.
        if (m_penSource != NULL) {
          // If property is entity property.
          if (bSourceIsProperty) {
            // If entity property exists.
            if (pSourceProperty != NULL) {
              CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
              
              // Here is support for both FLOAT and INDEX.
              if (eptSourceType == CEntityProperty::EPT_FLOAT) {
                SLONG offset1 = pSourceProperty->ep_slOffset; 
                fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
              } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
                SLONG offset1 = pSourceProperty->ep_slOffset;
                *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
              }
            }
          // If source type isn't classname.
          } else if (m_eSourcePT != ECT_TYPE) {
            FLOAT fSrc = GeNonEPFloatSource();
            fNew = &fSrc;
          }
        }

        if (m_bDebugMessages) {
          CPrintF("%s : Modifing target SpeedY with value=%f.\n", GetName(), *fNew);
        }

        if (m_eOperation == EO_SET) {
          *fValue = *fNew;
        } else if (m_eOperation == EO_ADD) {
          *fValue += *fNew;
        } else if (m_eOperation == EO_SUBSTRACT) {
          *fValue -= *fNew;
        } else if (m_eOperation == EO_MULTIPLY) {
          *fValue *= *fNew;
        } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
          *fValue /= *fNew;
        } else {
          if (m_bDebugMessages) {
            CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
          }
        }

        ((CMovableEntity&)*m_penTarget).AddToMovers();

      } else if (m_eTargetPT == ECT_SPEEDZ) {
        FLOAT *fValue = &((CMovableEntity&)*m_penTarget).en_vCurrentTranslationAbsolute(3); 
        FLOAT fOld = *fValue;
        FLOAT *fNew = &m_fValue;

        // If we have source target.
        if (m_penSource != NULL) {
          // If property is entity property.
          if (bSourceIsProperty) {
            // If entity property exists.
            if (pSourceProperty != NULL) {
              CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
              
              // Here is support for both FLOAT and INDEX.
              if (eptSourceType == CEntityProperty::EPT_FLOAT) {
                SLONG offset1 = pSourceProperty->ep_slOffset; 
                fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
              } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
                SLONG offset1 = pSourceProperty->ep_slOffset;
                *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
              }
            }
          // If source type isn't classname.
          } else if (m_eSourcePT != ECT_TYPE) {
            FLOAT fSrc = GeNonEPFloatSource();
            fNew = &fSrc;
          }
        }

        if (m_bDebugMessages) {
          CPrintF("%s : Modifing target SpeedZ with value=%f.\n", GetName(), *fNew);
        }

        if (m_eOperation == EO_SET) {
          *fValue = *fNew;
        } else if (m_eOperation == EO_ADD) {
          *fValue += *fNew;
        } else if (m_eOperation == EO_SUBSTRACT) {
          *fValue -= *fNew;
        } else if (m_eOperation == EO_MULTIPLY) {
          *fValue *= *fNew;
        } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
          *fValue /= *fNew;
        } else {
          if (m_bDebugMessages) {
            CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
          }
        }

        ((CMovableEntity&)*m_penTarget).AddToMovers();

      } else if (m_eTargetPT == ECT_SPEEDXREL) {
        FLOAT *fValue = &((CMovableEntity&)*m_penTarget).en_vDesiredTranslationRelative(1); 
        FLOAT fOld = *fValue;
        FLOAT *fNew = &m_fValue;

        // If we have source target.
        if (m_penSource != NULL) {
          // If property is entity property.
          if (bSourceIsProperty) {
            // If entity property exists.
            if (pSourceProperty != NULL) {
              CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
              
              // Here is support for both FLOAT and INDEX.
              if (eptSourceType == CEntityProperty::EPT_FLOAT) {
                SLONG offset1 = pSourceProperty->ep_slOffset; 
                fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
              } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
                SLONG offset1 = pSourceProperty->ep_slOffset;
                *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
              }
            }
          // If source type isn't classname.
          } else if (m_eSourcePT != ECT_TYPE) {
            FLOAT fSrc = GeNonEPFloatSource();
            fNew = &fSrc;
          }
        }

        if (m_bDebugMessages) {
          CPrintF("%s : Modifing target SpeedXRel with value=%f.\n", GetName(), *fNew);
        }

        if (m_eOperation == EO_SET) {
          *fValue = *fNew;
        } else if (m_eOperation == EO_ADD) {
          *fValue += *fNew;
        } else if (m_eOperation == EO_SUBSTRACT) {
          *fValue -= *fNew;
        } else if (m_eOperation == EO_MULTIPLY) {
          *fValue *= *fNew;
        } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
          *fValue /= *fNew;
        } else {
          if (m_bDebugMessages) {
            CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
          }
        }

        ((CMovableEntity&)*m_penTarget).AddToMovers();

      } else if (m_eTargetPT == ECT_SPEEDYREL) {
        FLOAT *fValue = &((CMovableEntity&)*m_penTarget).en_vDesiredTranslationRelative(2); 
        FLOAT fOld = *fValue;
        FLOAT *fNew = &m_fValue;

        // If we have source target.
        if (m_penSource != NULL) {
          // If property is entity property.
          if (bSourceIsProperty) {
            // If entity property exists.
            if (pSourceProperty != NULL) {
              CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
              
              // Here is support for both FLOAT and INDEX.
              if (eptSourceType == CEntityProperty::EPT_FLOAT) {
                SLONG offset1 = pSourceProperty->ep_slOffset; 
                fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
              } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
                SLONG offset1 = pSourceProperty->ep_slOffset;
                *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
              }
            }
          // If source type isn't classname.
          } else if (m_eSourcePT != ECT_TYPE) {
            FLOAT fSrc = GeNonEPFloatSource();
            fNew = &fSrc;
          }
        }

        if (m_bDebugMessages) {
          CPrintF("%s : Modifing target SpeedYRel with value=%f.\n", GetName(), *fNew);
        }

        if (m_eOperation == EO_SET) {
          *fValue = *fNew;
        } else if (m_eOperation == EO_ADD) {
          *fValue += *fNew;
        } else if (m_eOperation == EO_SUBSTRACT) {
          *fValue -= *fNew;
        } else if (m_eOperation == EO_MULTIPLY) {
          *fValue *= *fNew;
        } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
          *fValue /= *fNew;
        } else {
          if (m_bDebugMessages) {
            CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
          }
        }

        ((CMovableEntity&)*m_penTarget).AddToMovers();

      } else if (m_eTargetPT == ECT_SPEEDZREL) {
        FLOAT *fValue = &((CMovableEntity&)*m_penTarget).en_vDesiredTranslationRelative(3); 
        FLOAT fOld = *fValue;
        FLOAT *fNew = &m_fValue;

        // If we have source target.
        if (m_penSource != NULL) {
          // If property is entity property.
          if (bSourceIsProperty) {
            // If entity property exists.
            if (pSourceProperty != NULL) {
              CEntityProperty::PropertyType eptSourceType = pSourceProperty->ep_eptType;
              
              // Here is support for both FLOAT and INDEX.
              if (eptSourceType == CEntityProperty::EPT_FLOAT) {
                SLONG offset1 = pSourceProperty->ep_slOffset; 
                fNew = ((FLOAT *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1)); 
              } else if (eptSourceType == CEntityProperty::EPT_INDEX) {
                SLONG offset1 = pSourceProperty->ep_slOffset;
                *fNew = *((INDEX *)(((UBYTE *)(CEntity*)&*m_penSource) + offset1));
              }
            }
          // If source type isn't classname.
          } else if (m_eSourcePT != ECT_TYPE) {
            FLOAT fSrc = GeNonEPFloatSource();
            fNew = &fSrc;
          }
        }

        if (m_bDebugMessages) {
          CPrintF("%s : Modifing target SpeedZRel with value=%f.\n", GetName(), *fNew);
        }
    
        if (m_eOperation == EO_SET) {
          *fValue = *fNew;
        } else if (m_eOperation == EO_ADD) {
          *fValue += *fNew;
        } else if (m_eOperation == EO_SUBSTRACT) {
          *fValue -= *fNew;
        } else if (m_eOperation == EO_MULTIPLY) {
          *fValue *= *fNew;
        } else if (m_eOperation == EO_DIVIDE && m_fValue != 0) {
          *fValue /= *fNew;
        } else {
          if (m_bDebugMessages) {
            CPrintF(TRANS("%s : DO NOT DIVIDE THROUGH 0!\n"), m_strName);
          }
        }

        ((CMovableEntity&)*m_penTarget).AddToMovers();
      }
    }
  }

  // --------------------------------------------------------------------------------------
  // Event handler.
  // --------------------------------------------------------------------------------------
  BOOL HandleEvent(const CEntityEvent &ee) { 

    if (ee.ee_slEvent == EVENTCODE_ETrigger) {
      HandleETrigger(ee);
    }

    if (ee.ee_slEvent == EVENTCODE_EActivate) {
      m_bActive = TRUE;
    }

    if (ee.ee_slEvent == EVENTCODE_EDeactivate) {
      m_bActive = FALSE;
    }
    return CEntity::HandleEvent(ee); 
  }

procedures:
  // --------------------------------------------------------------------------------------
  // Entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    if (m_eTargetPT == ECT_TYPE) {
      m_eTargetPT = ECT_ENTITY;
      WarningMessage("[Target Property Type] can't be ""Entity Class""!\nIt is impossible to change entity class!");
    } 
    
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_MARKER);
    SetModelMainTexture(TEXTURE_MARKER);

    return;
  }
};