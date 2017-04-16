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

296
%{
#include "StdH.h"
#include <EntitiesMP/AnimationHub.h>
%}

uses "EntitiesMP/ModelHolder2";
uses "EntitiesMP/MovableModel";
uses "EntitiesMP/Light";

enum EAnimType
{
  0 AT_MODEL        "Model Animation [0]",
  1 AT_TEXTURE      "Texure Animation [1]",
  2 AT_LIGHTDIFFUSE "Light Anim. [2]",
  3 AT_AMBIENTLIGHT "Amb. Light Anim. [3]"
};

enum EDACValueType
{
  0 EDACVT_INDEX       "Index [0]",
  1 EDACVT_FLOATINT    "Float (int part) [1]",
  2 EDACVT_FLOATFRACT  "Float (fract part) [2]"
};

enum EDACValueClampType
{
  0 EDACVCT_NONE       "None [0]",
  1 EDACVCT_POSITIVE   "Positive (>= 0) [1]",
  2 EDACVCT_NEGATIVE   "Negative (<= 0) [2]",
};

event ENextDACIndex
{
  INDEX iValue,
};

event ENextDACFractFloat
{
  FLOAT fValue,
};

event ENextDACMinus {};

event ENextDACNone {};

class CDigitalAnimationChanger : CRationalEntity {
name      "DigitalAnimationChanger";
thumbnail "Thumbnails\\DigitalAnimationChanger.tbn";
features  "HasName", "HasTarget", "IsTargetable";

properties:
  1 CTString m_strName          "Name" 'N'      = "Digital Anim. Changer",
  2 CTString m_strDescription = "",

  3 CEntityPointer m_penTarget  "Target" 'T' COLOR(C_GREEN|0xFF),
  
  5 BOOL m_bDebugMessages       "Debug Messages" = FALSE,
  8 CEntityPointer m_penNext    "Next Digit" COLOR(C_RED|0xFF),
  
 10 enum EAnimType m_eatAnimation "Anim. Type" = AT_TEXTURE,
 
 12 BOOL m_bAbsoluteValue    "Absolute Value" = FALSE,
 
 13 enum EDACValueClampType m_eValueClampType  "Clamp Value" = EDACVCT_NONE,
 
 15 INDEX m_iValue           "Value Index" = 0,
 16 FLOAT m_fValue           "Value Float" = 0.0F,
 18 enum EDACValueType m_eValueType "Value Type" = EDACVT_INDEX,
  
 30 ANIMATION m_iAnim0       "Animation 0" = 0,
 31 ANIMATION m_iAnim1       "Animation 1" = 0,
 32 ANIMATION m_iAnim2       "Animation 2" = 0,
 33 ANIMATION m_iAnim3       "Animation 3" = 0,
 34 ANIMATION m_iAnim4       "Animation 4" = 0,
 35 ANIMATION m_iAnim5       "Animation 5" = 0,
 36 ANIMATION m_iAnim6       "Animation 6" = 0,
 37 ANIMATION m_iAnim7       "Animation 7" = 0,
 38 ANIMATION m_iAnim8       "Animation 8" = 0,
 39 ANIMATION m_iAnim9       "Animation 9" = 0,

 50 ANIMATION m_iAnimNone    "Animation None" = 0,
 //51 ANIMATION m_iAnimDot     "Animation Dot" = 0,
 52 ANIMATION m_iAnimMinus   "Animation Minus" = 0,

components:
  1 model   MODEL_CHANGER     "Models\\Editor\\DigitalAnimationChanger.mdl",
  2 texture TEXTURE_CHANGER   "Models\\Editor\\DigitalAnimationChanger.tex"
  
functions:

  // --------------------------------------------------------------------------------------
  // Get anim data for given animation property - returns NULL for none.
  // --------------------------------------------------------------------------------------
  CAnimData *GetAnimData(SLONG slPropertyOffset) 
  {
    CEntity *penTarget = m_penTarget;

    if (penTarget == NULL) {
      return NULL;
    }
    
    if (IsOfClass(penTarget, "AnimationHub")) {
      penTarget = ((CAnimationHub*)penTarget)->m_penTarget0;
    }
    
    if (penTarget == NULL) {
      return NULL;
    }
    
    BOOL bIsAnimVar = FALSE;
    
    if (slPropertyOffset >= offsetof(CDigitalAnimationChanger, m_iAnim0) && slPropertyOffset <= offsetof(CDigitalAnimationChanger, m_iAnim9))
    {
      bIsAnimVar = TRUE;
    }
      
    if (slPropertyOffset >= offsetof(CDigitalAnimationChanger, m_iAnimNone) && slPropertyOffset <= offsetof(CDigitalAnimationChanger, m_iAnimMinus))
    {
      bIsAnimVar = TRUE;
    }
    
    if (!bIsAnimVar) {
      return NULL;
    }
    
    // If ModelHolder2
    if (IsOfClass(penTarget, "ModelHolder2")) {
      CModelHolder2 *penModel = (CModelHolder2*)&*penTarget;

      if (m_eatAnimation == AT_MODEL) {
        return penModel->GetModelObject()->GetData();
      } else if (m_eatAnimation == AT_TEXTURE) {
        return penModel->GetModelObject()->mo_toTexture.GetData();
      } else if (m_eatAnimation == AT_LIGHTDIFFUSE) {
        return penModel->m_aoLightAnimation.GetData();
      }

    // If MovableModel
    } else if (IsOfClass(penTarget, "Movable Model")) {
      CMovableModel *penModel = (CMovableModel*)&*penTarget;
      
      if (m_eatAnimation == AT_MODEL) {
        return penModel->GetModelObject()->GetData();
      } else if (m_eatAnimation == AT_TEXTURE) {
        return penModel->GetModelObject()->mo_toTexture.GetData();
      } else if (m_eatAnimation == AT_LIGHTDIFFUSE) {
        return penModel->m_aoLightAnimation.GetData();
      }
    
    // if light
    } else if (IsOfClass(penTarget, "Light")) {
      CLight *penLight = (CLight*)&*penTarget;
    
      if (m_eatAnimation == AT_LIGHTDIFFUSE) {
        return penLight->m_aoLightAnimation.GetData();
      } else if (m_eatAnimation = AT_AMBIENTLIGHT) {
        return penLight->m_aoAmbientLightAnimation.GetData();
      }
    }
    
    return CEntity::GetAnimData(slPropertyOffset);
  }
  
  // --------------------------------------------------------------------------------------
  // Sends event to target to make it change anim. 
  // --------------------------------------------------------------------------------------
  void SendChangeAnim(INDEX iAnim)
  {
    EChangeAnim eChange;
    eChange.iModelAnim        = m_eatAnimation == AT_MODEL ? iAnim : 0;
    eChange.iTextureAnim      = m_eatAnimation == AT_TEXTURE ? iAnim : 0;
    eChange.iLightAnim        = m_eatAnimation == AT_LIGHTDIFFUSE ? iAnim : 0;
    eChange.iAmbientLightAnim = m_eatAnimation == AT_AMBIENTLIGHT ? iAnim : 0;
    eChange.bModelLoop        = 0;
    eChange.bTextureLoop      = 0;
    eChange.bLightLoop        = 0;
    eChange.colAmbient        = C_WHITE|255;
    eChange.colDiffuse        = C_WHITE|255;
    m_penTarget->SendEvent(eChange);
  }
  
  void PerformIndex()
  {
    const INDEX* aiAnimations = &m_iAnim0;

    if (m_bDebugMessages) {
      CPrintF("[DAC] '%s' : Displaying index value: %d\n", m_strName, m_iValue);
    }
    
    INDEX iValue = m_iValue;
    
    if (m_eValueClampType == EDACVCT_POSITIVE) {
      iValue = ClampDn(iValue, INDEX(0));
    } else if (m_eValueClampType == EDACVCT_NEGATIVE) {
      iValue = ClampUp(iValue, INDEX(0));
    }
    
    if (m_bAbsoluteValue) {
      iValue = Abs(iValue);
      
      if (m_bDebugMessages) {
        CPrintF("  Absolute Value: %d", iValue);
      }
    }
    
    INDEX iDigitVal = iValue % 10;

    // If target model exist then change its animation.
    if (m_penTarget) {
      SendChangeAnim(aiAnimations[Abs(iDigitVal)]);
    }

    if (m_penNext)
    {
      INDEX iNextValue = (iValue - iDigitVal) / 10;

      // If it is the last number then say to next DAC to show minus.
      if (iValue < 0 && iNextValue == 0) {
        m_penNext->SendEvent(ENextDACMinus());
      } else {

        ENextDACIndex eNextDAC;
        eNextDAC.iValue = iNextValue;

        m_penNext->SendEvent(eNextDAC);
      }
    }
  }
  
  void PerformIntegralPart()
  {
    const INDEX* aiAnimations = &m_iAnim0;
    
    if (m_bDebugMessages) {
      CPrintF("[DAC] '%s' : Displaying float value: %f\n", m_strName, m_fValue);
    }
    
    FLOAT fValue = m_fValue;
    
    if (m_eValueClampType == EDACVCT_POSITIVE) {
      fValue = ClampDn(fValue, 0.0F);
    } else if (m_eValueClampType == EDACVCT_NEGATIVE) {
      fValue = ClampUp(fValue, 0.0F);
    }
    
    if (m_bAbsoluteValue) {
      fValue = Abs(fValue);
      
      if (m_bDebugMessages) {
        CPrintF("  Absolute Value: %f", fValue);
      }
    }

    FLOAT fIntegralPart;
    
    modff(fValue, &fIntegralPart);
    
    INDEX iPart = (INDEX)fIntegralPart;
    
    if (m_bDebugMessages) {
      CPrintF("  Integral part: %d\n", iPart);
    }
    
    INDEX iDigitVal = iPart % 10;
    
    // If target model exist then change its animation.
    if (m_penTarget) {
      SendChangeAnim(aiAnimations[Abs(iDigitVal)]);
    }
    
    if (m_penNext)
    {
      INDEX iNextValue = (iPart - iDigitVal) / 10;

      // If it is the last number then say to next DAC to show minus.
      if (iPart < 0 && iNextValue == 0) {
        m_penNext->SendEvent(ENextDACMinus());
      } else {

        ENextDACIndex eNextDAC;
        eNextDAC.iValue = iNextValue;

        m_penNext->SendEvent(eNextDAC);
      }
    }
  }
  
  void PerformFractionalPart()
  {
    const INDEX* aiAnimations = &m_iAnim0;
    
    if (m_bDebugMessages) {
      CPrintF("[DAC] '%s' : Displaying float value: %f\n", m_strName, m_fValue);
    }
    
    FLOAT fFractionalPart, fIntegralPart;
    
    // Gets fractional part from floating point variable.
    fFractionalPart = modff(m_fValue, &fIntegralPart);
    fFractionalPart = Abs(fFractionalPart); 
    
    if (m_bDebugMessages) {
      CPrintF("  Fractional part: %f\n", fFractionalPart);
    }
    
    fFractionalPart *= 10.0F;
    fFractionalPart = modff(fFractionalPart, &fIntegralPart);
    
    INDEX iPart = (INDEX)fIntegralPart;
    
    INDEX iDigitVal = iPart % 10;
    
    // If target model exist then change its animation.
    if (m_penTarget) {
      SendChangeAnim(aiAnimations[Abs(iDigitVal)]);
    }
    
    if (m_penNext)
    {
      INDEX iNextValue = (iPart - iDigitVal) / 10;

      ENextDACFractFloat eNextDAC;
      eNextDAC.fValue = fFractionalPart;

      m_penNext->SendEvent(eNextDAC);
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
    SetModel(MODEL_CHANGER);
    SetModelMainTexture(TEXTURE_CHANGER);

    // check target type
    if (m_penTarget != NULL && 
      !IsOfClass(m_penTarget, "AnimationHub") &&
      !IsOfClass(m_penTarget, "ModelHolder2") &&
      !IsOfClass(m_penTarget, "Movable Model") &&
      !IsOfClass(m_penTarget, "Light")) {
      WarningMessage("Target must be AnimationHub, ModelHolder2, MovableModel or Light!");
      m_penTarget = NULL;
    }

    if (m_penTarget == NULL) {
      return;
    }
    
    // Main Loop
    wait()
    {
      on (EBegin) : {
        resume;
      }
      
      on (ENextDACIndex eNextDAC) :
      {
        m_iValue = eNextDAC.iValue;
        
        if (m_iValue != 0) {
          PerformIndex();
        } else {
          SendChangeAnim(m_iAnimNone);
          
          if (m_penNext) {
            m_penNext->SendEvent(ENextDACNone());
          }
        }
      }
      
      on (ENextDACFractFloat eNextDAC) :
      {
        const INDEX* aiAnimations = &m_iAnim0;

        FLOAT fFractionalPart, fIntegralPart;
        
        fFractionalPart = eNextDAC.fValue * 10.0F;
        fFractionalPart = modff(fFractionalPart, &fIntegralPart);
        
        INDEX iPart = (INDEX)fIntegralPart;
        INDEX iDigitVal = iPart % 10;
        
        // If target model exist then change its animation.
        if (m_penTarget) {
          SendChangeAnim(aiAnimations[Abs(iDigitVal)]);
        }
        
        if (m_penNext)
        {
          INDEX iNextValue = (iPart - iDigitVal) / 10;

          ENextDACFractFloat eNextDAC;
          eNextDAC.fValue = fFractionalPart;

          m_penNext->SendEvent(eNextDAC);
        }
      }

      on (ENextDACMinus) :
      {
        SendChangeAnim(m_iAnimMinus);
        
        if (m_penNext) {
          m_penNext->SendEvent(ENextDACNone());
        }
      }

      on (ENextDACNone) :
      {
        SendChangeAnim(m_iAnimNone);
        
        if (m_penNext) {
          m_penNext->SendEvent(ENextDACNone());
        }
      }

      on (EStart) :
      {
        // Index variable.
        if (m_eValueType == EDACVT_INDEX) {
          PerformIndex();

        // Float integral part.
        } else if (m_eValueType == EDACVT_FLOATINT) {
          PerformIntegralPart();

        // Float fractional part.
        } else {
          PerformFractionalPart();
        }
        
        resume;
      }
    }

    return;
  }
};