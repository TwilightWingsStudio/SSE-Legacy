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

2104
%{
  #include "StdH.h"
  #include "EntitiesMP/WorldSettingsController.h"
%}

uses "EntitiesMP/ModelDestruction";
uses "EntitiesMP/AnimationChanger";
uses "EntitiesMP/BloodSpray";
uses "EntitiesMP/ModelHolder2";
uses "EntitiesMP/Player";

enum EBlockAction {
   0 EBA_STOP         "0 Stop",
   1 EBA_SLIDE        "1 Slide",
   2 EBA_CLIMBORSLIDE "2 Climb or Slide",
   3 EBA_BOUNCE       "3 Bounce",
   4 EBA_PUSH         "4 Push",
   5 EBA_STOPEXACT    "5 Stop exact",
};

%{
  // info structure
  static EntityInfo eiMModel = {
    EIBT_FLESH, 1000.0F,
    0, 0, 0,     // source (eyes)
    0.0F, 0, 0.0F,     // target (body)
  };

  #define MIPRATIO 0.003125F //(2*tan(90/2))/640

  // used to render certain entities only for certain players (like picked items, etc.)
  extern ULONG _ulPlayerRenderingMask;
%}

class CMovableModel : CMovableModelEntity {
name      "Movable Model";
thumbnail "Thumbnails\\MovableModel.tbn";
features "HasName", "HasDescription", "IsTargetable";

properties:
   // Paths to common assets.
   1 CTFileName m_fnModel      "Model" 'M'      = CTFILENAME("Models\\Editor\\Axis.mdl")   features(EPROPF_READONLY),
   2 CTFileName m_fnTexture    "Texture" 'T'    = CTFILENAME("Models\\Editor\\Vector.tex") features(EPROPF_READONLY),
  
  // Paths to additional textures.
  22 CTFileName m_fnReflection "Reflection"     = CTString("") features(EPROPF_READONLY),
  23 CTFileName m_fnSpecular   "Specular"       = CTString("") features(EPROPF_READONLY),
  24 CTFileName m_fnBump       "Bump"           = CTString("") features(EPROPF_READONLY),
 
   3 FLOAT m_fStretchAll       "StretchAll" 'S' = 1.0F,
   4 FLOAT m_fStretchX         "StretchX"   'X' = 1.0F,
   5 FLOAT m_fStretchY         "StretchY"   'Y' = 1.0F,
   6 FLOAT m_fStretchZ         "StretchZ"   'Z' = 1.0F,
 
   7 CTString m_strName            "Name" 'N'          = "Movable Model",
  12 CTString m_strDescription                         = "",
   8 BOOL m_bColliding             "Colliding" 'L'     = TRUE,    // set if model is not immatierial
   9 ANIMATION m_iModelAnimation   "Model animation"   = 0,
  10 ANIMATION m_iTextureAnimation "Texture animation" = 0,
  11 enum ShadowType m_stClusterShadows "Shadows" 'W'  = ST_CLUSTER,   // set if model uses cluster shadows
  13 BOOL m_bBackground            "Background" 'B'    = FALSE,   // set if model is rendered in background
  21 BOOL m_bTargetable                                = TRUE, // set if model should be targetable
 
  // Parameters for custom shading of a model (overrides automatic shading calculation).
  14 enum CustomShadingType m_cstCustomShading "Custom shading" 'H' = CST_NONE,
  15 ANGLE3D m_aShadingDirection               "Light direction" 'D' = ANGLE3D( AngleDeg(45.0F), AngleDeg(45.0F), AngleDeg(45.0F)),
  16 COLOR m_colLight                          "Light color" 'O' = C_WHITE,
  17 COLOR m_colAmbient                        "Ambient color" 'A' = C_BLACK,
  18 CTFileName m_fnmLightAnimation            "Light animation file" = CTString(""),
  19 ANIMATION m_iLightAnimation               "Light animation" = 0,
  20 CAnimObject m_aoLightAnimation,
  25 BOOL m_bAttachments                       "Attachments" = TRUE, // set if model should auto load attachments
  26 BOOL m_bActive                            "Active" = TRUE,
  31 FLOAT m_fMipAdd                           "Mip Add" = 0.0F,
  32 FLOAT m_fMipMul                           "Mip Mul" = 1.0F,
  //33 FLOAT m_fMipFadeDist "Mip Fade Dist" = 0.0F,
  //34 FLOAT m_fMipFadeLen  "Mip Fade Len" = 0.0F,
  33 FLOAT m_fMipFadeDist = 0.0F,
  34 FLOAT m_fMipFadeLen  = 0.0F,
  35 RANGE m_rMipFadeDistMetric "Mip Fade Dist (Metric)" = -1.0F,
  36 FLOAT m_fMipFadeLenMetric  "Mip Fade Len (Metric)" = -1.0F,
  
  // Random values variables.
  50 BOOL m_bRandomStretch   "Apply RND stretch"   = FALSE, // apply random stretch
  52 FLOAT m_fStretchRndX    "Stretch RND X (%)"   = 0.2F, // random stretch width
  51 FLOAT m_fStretchRndY    "Stretch RND Y (%)"   = 0.2F, // random stretch height
  53 FLOAT m_fStretchRndZ    "Stretch RND Z (%)"   = 0.2F, // random stretch depth
  54 FLOAT m_fStretchRndAll  "Stretch RND All (%)" = 0.0F, // random stretch all
  55 FLOAT3D m_fStretchRandom = FLOAT3D(1.0F, 1.0F, 1.0F),
 
  // Destruction values.
  60 CEntityPointer m_penDestruction ,    // model destruction entity
  61 FLOAT3D m_vDamage    = FLOAT3D(0.0F, 0.0F, 0.0F),    // current damage impact
  62 FLOAT m_tmLastDamage = -1000.0F,
  63 CEntityPointer m_penDestroyTarget "Destruction Target" COLOR(C_WHITE|0xFF), // targeted when destroyed
  64 CEntityPointer m_penLastDamager,
  65 FLOAT m_tmSpraySpawned = 0.0F,   // time when damage has been applied
  66 FLOAT m_fSprayDamage   = 0.0F,   // total ammount of damage
  67 CEntityPointer m_penSpray,       // the blood spray
  68 FLOAT m_fMaxDamageAmmount  = 0.0F, // max ammount of damage recived in in last xxx ticks
 
  70 FLOAT m_fClassificationStretch  "Classification stretch" = 1.0F, // classification box multiplier
  80 COLOR m_colBurning = COLOR(C_WHITE|CT_OPAQUE), // color applied when burning
 
  90 enum DamageType m_dmtLastDamageType=DMT_CHAINSAW,
  91 FLOAT m_fChainSawCutDamage    "Chain saw cut dammage" 'C' = 300.0F,
  93 INDEX m_iFirstRandomAnimation "First random animation" 'R' = -1,
 100 FLOAT m_fMaxTessellationLevel "Max tessellation level" = 0.0F,
 
 // Translations and angular speeds.
 102 FLOAT m_fTransX       "X Translation"    = 0.0F,
 103 FLOAT m_fTransY       "Y Translation"    = 0.0F,
 104 FLOAT m_fTransZ       "Z Translation"    = 0.0F,
 105 FLOAT m_fRotH         "Heading Rotation" = 0.0F,
 106 FLOAT m_fRotP         "Pitch Rotation"   = 0.0F,
 107 FLOAT m_fRotB         "Banking Rotation" = 0.0F,
 111 BOOL m_bRelative      "Relative Translation" = TRUE,
 
 112 FLOAT m_fbTransX = 0.0F,
 113 FLOAT m_fbTransY = 0.0F,
 114 FLOAT m_fbTransZ = 0.0F,
 115 FLOAT m_fbRotH   = 0.0F,
 116 FLOAT m_fbRotP   = 0.0F,
 117 FLOAT m_fbRotB   = 0.0F,
 
 120 BOOL m_bOriented  "Oriented by Gravity" = TRUE,
 121 BOOL m_bTranslated "Translated by Gravity" = TRUE,

 122 enum EBlockAction m_eBA  "On Block:" = EBA_STOP,
 123 FLOAT m_fAcceleration "Acceleration" = 1000,
 124 FLOAT m_fDeceleration "Deceleration" = 1000,
 125 FLOAT m_fHealth       "Health" = -1,
 126 CEntityPointer m_penDeathTarget  "Death Target",
 127 BOOL m_bDivide    "Heading=Heading/Z Translation" = FALSE,
 128 INDEX m_ulVisibleMask = 0,
 129 CEntityPointer m_penPlayer,
 130 CEntityPointer m_penFace "View Target",
 131 FLOAT m_fRealHealth = -1,
 132 FLOAT m_fMaxRotation "Max Rotation Speed" = 1000.0F,
 133 FLOAT m_fMaxHealth "Max Health" = -1.0F,
 134 BOOL m_bPPhys "Plane physic test" = FALSE,
 135 BOOL m_bVTHeadingOnly "VT Heading only" = FALSE,
 
 141 FLOAT m_fAddedTransX = 0.0F,
 142 FLOAT m_fAddedTransY = 0.0F,
 143 FLOAT m_fAddedTransZ = 0.0F,
 
 150 CEntityPointer m_penSwitch "Switch",  // for switch relaying

  {
    CTFileName m_fnOldModel;  // used for remembering last selected model (not saved at all)
  }

components:
  1 class   CLASS_BLOOD_SPRAY     "Classes\\BloodSpray.ecl",

functions:

  // --------------------------------------------------------------------------------------
  // [SSE] Interaction API
  // Return true if this entity can act act like interaction relay.
  // --------------------------------------------------------------------------------------
  virtual BOOL IsInteractionRelay()
  {
    return TRUE;
  }

  // --------------------------------------------------------------------------------------
  // [SSE] Interaction API
  // Return pointer to interaction provider if needed.
  // --------------------------------------------------------------------------------------
  virtual CEntity *GetInteractionProvider()
  {
    return m_penSwitch;
  }

  // --------------------------------------------------------------------------------------
  // Fill in entity statistics - for AI purposes only.
  // --------------------------------------------------------------------------------------
  BOOL FillEntityStatistics(EntityStats *pes)
  {
    pes->es_strName = m_fnModel.FileName() + ", " + m_fnTexture.FileName();
    pes->es_ctCount = 1;
    pes->es_ctAmmount = 1;
   
    pes->es_fValue = 0;
    pes->es_iScore = 0;
    
    return TRUE;
  }

  // --------------------------------------------------------------------------------------
  // Classification box multiplier.
  // --------------------------------------------------------------------------------------
  FLOAT3D GetClassificationBoxStretch(void) {
    return FLOAT3D( m_fClassificationStretch, m_fClassificationStretch, m_fClassificationStretch);
  }

  // --------------------------------------------------------------------------------------
  // Maximum allowed tessellation level for this model (for Truform/N-Patches support)
  // --------------------------------------------------------------------------------------
  FLOAT GetMaxTessellationLevel(void) {
    return m_fMaxTessellationLevel;
  }

  /* Receive damage */
  void ReceiveDamage(CEntity *penInflictor, enum DamageType dmtType,
    FLOAT fDamageAmmount, const FLOAT3D &vHitPoint, const FLOAT3D &vDirection) 
  {
    FLOAT fNewDamage = fDamageAmmount;

    // if not destroyable
    if (m_fHealth == -1 || !m_bActive) {
      // do nothing
      return;
    }

    if ( dmtType == DMT_BURNING) {
      UBYTE ubR, ubG, ubB, ubA;
      ColorToRGBA(m_colBurning, ubR, ubG, ubB, ubA);
      ubR = ClampDn(ubR-4, 32);
      m_colBurning = RGBAToColor(ubR, ubR, ubR, ubA);
    }
    
    // remember who damaged you
    m_penLastDamager = penInflictor;
    
    CMovableModelEntity::ReceiveDamage(penInflictor, dmtType, fDamageAmmount, vHitPoint, vDirection);
  };

  // --------------------------------------------------------------------------------------
  // Entity info.
  // --------------------------------------------------------------------------------------
  void *GetEntityInfo(void) {
    return CEntity::GetEntityInfo();
  };

  class CModelDestruction *GetDestruction(void)
  {
    ASSERT(m_penDestruction == NULL || IsOfClass(m_penDestruction, "ModelDestruction"));
    return (CModelDestruction*)&*m_penDestruction;
  }

  // --------------------------------------------------------------------------------------
  // Get anim data for given animation property - return NULL for none.
  // --------------------------------------------------------------------------------------
  CAnimData *GetAnimData(SLONG slPropertyOffset) 
  {
    if (slPropertyOffset == offsetof(CMovableModel, m_iModelAnimation)) {
      return GetModelObject()->GetData();
    } else if (slPropertyOffset == offsetof(CMovableModel, m_iTextureAnimation)) {
      return GetModelObject()->mo_toTexture.GetData();
    } else if (slPropertyOffset == offsetof(CMovableModel, m_iLightAnimation)) {
      return m_aoLightAnimation.GetData();
    } else {
      return CEntity::GetAnimData(slPropertyOffset);
    }
  };

  // --------------------------------------------------------------------------------------
  // Calculate rotation matrix that points in direction of a target entity.
  // --------------------------------------------------------------------------------------
  void CalcTargetedRotation(const FLOAT3D &vMyPos, CEntity *penViewTarget,
    FLOAT3D vPosRatio, FLOATmatrix3D &mRotTarget, BOOL bLerping) const
  {
    FLOAT3D vAbsPos;
    penViewTarget->GetEntityPointRatio(vPosRatio, vAbsPos, bLerping);

    FLOAT3D vDir;
    vDir = vAbsPos-vMyPos;
    vDir.Normalize();

    ANGLE3D aDir;
    DirectionVectorToAnglesNoSnap(vDir, aDir);
    MakeRotationMatrixFast(mRotTarget, aDir);
  }

  // --------------------------------------------------------------------------------------
  // Adjust model mip factor if needed.
  // --------------------------------------------------------------------------------------
  void AdjustMipFactor(FLOAT &fMipFactor)
  {
    if (m_ulVisibleMask != 0 && m_ulVisibleMask != _ulPlayerRenderingMask) {
      if (m_penPlayer) {
        CPrintF(TRANS("Don't render for %s\n"), m_penPlayer->GetName());
        /*CModelObject *pmo = GetModelObject();
        if (pmo==NULL) {
          return;
        }
        pmo->mo_colBlendColor = (pmo->mo_colBlendColor&~255)|UBYTE(255*0);*/
        fMipFactor = UpperLimit(0.0F);
        return;
      }
    }
    /*}else{
      CModelObject *pmo = GetModelObject();
      if (pmo==NULL) {
        return;
      }
      pmo->mo_colBlendColor = (pmo->mo_colBlendColor&~255)|UBYTE(255*256);
    }*/

    // if should fade last mip
    if (m_fMipFadeDist > 0) {
      CModelObject *pmo = GetModelObject();
      if (pmo == NULL) {
        return;
      }
      // adjust for stretch
      FLOAT fMipForFade = fMipFactor;
      // TODO: comment the next 3 lines for mip factors conversion
      /*if (pmo->mo_Stretch != FLOAT3D(1,1,1)) {
        fMipForFade -= Log2( Max(pmo->mo_Stretch(1),Max(pmo->mo_Stretch(2),pmo->mo_Stretch(3))));
      }*/

      // if not visible
      if (fMipForFade > m_fMipFadeDist) {
        // set mip factor so that model is never rendered
        fMipFactor = UpperLimit(0.0F);
        return;
      }

      // adjust fading
      FLOAT fFade = (m_fMipFadeDist - fMipForFade);
      if (m_fMipFadeLen > 0) {
        fFade /= m_fMipFadeLen;
      } else {
        if (fFade > 0) {
          fFade = 1.0F;
        }
      }
      
      fFade = Clamp(fFade, 0.0F, 1.0F);
      // make it invisible
      pmo->mo_colBlendColor = (pmo->mo_colBlendColor&~255)|UBYTE(255*fFade);
    }

    fMipFactor = fMipFactor*m_fMipMul+m_fMipAdd;
  }

  // --------------------------------------------------------------------------------------
  // Adjust model shading parameters if needed.
  // --------------------------------------------------------------------------------------
  BOOL AdjustShadingParameters(FLOAT3D &vLightDirection, COLOR &colLight, COLOR &colAmbient)
  {
    switch( m_cstCustomShading)
    {
      case CST_FULL_CUSTOMIZED: {
        // if there is color animation
        if (m_aoLightAnimation.GetData() != NULL) {
          // get lerping info
          SLONG colFrame0, colFrame1;
          FLOAT fRatio;

          m_aoLightAnimation.GetFrame(colFrame0, colFrame1, fRatio);
          UBYTE ubAnimR0, ubAnimG0, ubAnimB0;
          UBYTE ubAnimR1, ubAnimG1, ubAnimB1;
          ColorToRGB(colFrame0, ubAnimR0, ubAnimG0, ubAnimB0);
          ColorToRGB(colFrame1, ubAnimR1, ubAnimG1, ubAnimB1);

          // calculate current animation color
          FLOAT fAnimR = NormByteToFloat( Lerp( ubAnimR0, ubAnimR1, fRatio));
          FLOAT fAnimG = NormByteToFloat( Lerp( ubAnimG0, ubAnimG1, fRatio));
          FLOAT fAnimB = NormByteToFloat( Lerp( ubAnimB0, ubAnimB1, fRatio));
          
          // decompose constant colors
          UBYTE ubLightR,   ubLightG,   ubLightB;
          UBYTE ubAmbientR, ubAmbientG, ubAmbientB;
          ColorToRGB(m_colLight,   ubLightR,   ubLightG,   ubLightB);
          ColorToRGB(m_colAmbient, ubAmbientR, ubAmbientG, ubAmbientB);
          colLight   = RGBToColor(ubLightR  *fAnimR, ubLightG  *fAnimG, ubLightB  *fAnimB);
          colAmbient = RGBToColor(ubAmbientR*fAnimR, ubAmbientG*fAnimG, ubAmbientB*fAnimB);

        // if there is no color animation
        } else {
          colLight   = m_colLight;
          colAmbient = m_colAmbient;
        }

        // obtain world settings controller
        CWorldSettingsController *pwsc = GetWSC(this);

        if (pwsc != NULL && pwsc->m_bApplyShadingToModels) {
          // apply animating shading
          COLOR colShade = GetWorld()->wo_atbTextureBlendings[9].tb_colMultiply;
          colLight = MulColors(colLight, colShade);
          colAmbient = MulColors(colAmbient, colShade);
        }

        AnglesToDirectionVector(m_aShadingDirection, vLightDirection);
        vLightDirection = -vLightDirection;
        break;
      }
      case CST_CONSTANT_SHADING: {
        // combine colors with clamp
        UBYTE lR,lG,lB,aR,aG,aB,rR,rG,rB;
        ColorToRGB( colLight,   lR, lG, lB);
        ColorToRGB( colAmbient, aR, aG, aB);
        colLight = 0;
        rR = (UBYTE) Clamp( (ULONG)lR+aR, (ULONG)0, (ULONG)255);
        rG = (UBYTE) Clamp( (ULONG)lG+aG, (ULONG)0, (ULONG)255);
        rB = (UBYTE) Clamp( (ULONG)lB+aB, (ULONG)0, (ULONG)255);
        colAmbient = RGBToColor( rR, rG, rB);
        break;
      }
      case CST_NONE: {
        // do nothing
        break;
      }
    }

    if (m_colBurning != COLOR(C_WHITE|CT_OPAQUE)) {
      colAmbient = MulColors(colAmbient, m_colBurning);
      colLight = MulColors(colLight, m_colBurning);
      return TRUE;
    }

    return m_stClusterShadows != ST_NONE;
  };

  // --------------------------------------------------------------------------------------
  // Apply mirror and stretch to the entity.
  // --------------------------------------------------------------------------------------
  void MirrorAndStretch(FLOAT fStretch, BOOL bMirrorX)
  {
    m_fStretchAll *= fStretch;
    if (bMirrorX) {
      m_fStretchX = -m_fStretchX;
    }
  }

  // --------------------------------------------------------------------------------------
  // Stretch model.
  // --------------------------------------------------------------------------------------
  void StretchModel(void) {
    // stretch factors must not have extreme values
    if (Abs(m_fStretchX)  < 0.01F) { m_fStretchX   = 0.01F;  }
    if (Abs(m_fStretchY)  < 0.01F) { m_fStretchY   = 0.01F;  }
    if (Abs(m_fStretchZ)  < 0.01F) { m_fStretchZ   = 0.01F;  }
    if (m_fStretchAll < 0.01F)     { m_fStretchAll = 0.01F;  }

    if (Abs(m_fStretchX) > 1000.0F) { m_fStretchX   = 1000.0F*Sgn(m_fStretchX); }
    if (Abs(m_fStretchY) > 1000.0F) { m_fStretchY   = 1000.0F*Sgn(m_fStretchY); }
    if (Abs(m_fStretchZ) > 1000.0F) { m_fStretchZ   = 1000.0F*Sgn(m_fStretchZ); }
    if (m_fStretchAll > 1000.0F)    { m_fStretchAll = 1000.0F; }

    if (m_bRandomStretch) {
      m_bRandomStretch = FALSE;
      // stretch
      m_fStretchRndX   = Clamp( m_fStretchRndX  , 0.0F, 1.0F);
      m_fStretchRndY   = Clamp( m_fStretchRndY  , 0.0F, 1.0F);
      m_fStretchRndZ   = Clamp( m_fStretchRndZ  , 0.0F, 1.0F);
      m_fStretchRndAll = Clamp( m_fStretchRndAll, 0.0F, 1.0F);

      m_fStretchRandom(1) = (FRnd()*m_fStretchRndX*2 - m_fStretchRndX) + 1;
      m_fStretchRandom(2) = (FRnd()*m_fStretchRndY*2 - m_fStretchRndY) + 1;
      m_fStretchRandom(3) = (FRnd()*m_fStretchRndZ*2 - m_fStretchRndZ) + 1;

      FLOAT fRNDAll = (FRnd()*m_fStretchRndAll*2 - m_fStretchRndAll) + 1;
      m_fStretchRandom(1) *= fRNDAll;
      m_fStretchRandom(2) *= fRNDAll;
      m_fStretchRandom(3) *= fRNDAll;
    }

    GetModelObject()->StretchModel( FLOAT3D(
      m_fStretchAll*m_fStretchX*m_fStretchRandom(1),
      m_fStretchAll*m_fStretchY*m_fStretchRandom(2),
      m_fStretchAll*m_fStretchZ*m_fStretchRandom(3)) );
    ModelChangeNotify();
  };

  // --------------------------------------------------------------------------------------
  // Init model holder.
  // --------------------------------------------------------------------------------------
  void InitModelHolder(void) {

    // must not crash when model is removed
    if (m_fnModel=="") {
      m_fnModel=CTFILENAME("Models\\Editor\\Axis.mdl");
    }

    if ( m_fnReflection == CTString("Models\\Editor\\Vector.tex")) {
      m_fnReflection = CTString("");
    }
    if ( m_fnSpecular == CTString("Models\\Editor\\Vector.tex")) {
      m_fnSpecular = CTString("");
    }
    if ( m_fnBump == CTString("Models\\Editor\\Vector.tex")) {
      m_fnBump = CTString("");
    }

    if (m_bActive) {
      InitAsModel();
    } else {
      InitAsEditorModel();
    }

    // set appearance
    SetModel(m_fnModel);
    INDEX iAnim = m_iModelAnimation;
    FLOAT tmOffsetPhase = 0.0F;

    if (m_iFirstRandomAnimation >= 0) {
      INDEX ctAnims = GetModelObject()->GetAnimsCt() - m_iFirstRandomAnimation;
      iAnim = m_iFirstRandomAnimation + Clamp(INDEX(FRnd()*ctAnims), INDEX(0), ctAnims);
      tmOffsetPhase = FRnd() * 10.0F;
    }

    GetModelObject()->PlayAnim(iAnim, AOF_LOOPING);
    GetModelObject()->OffsetPhase(tmOffsetPhase);

    // if initialized for the first time
    if (m_fnOldModel == "") {
      // just remember the model filename
      m_fnOldModel = m_fnModel;
    // if re-initialized
    } else {
      // if the model filename has changed
      if (m_fnOldModel != m_fnModel) {
        m_fnOldModel = m_fnModel;
        GetModelObject()->AutoSetTextures();
        m_fnTexture = GetModelObject()->mo_toTexture.GetName();
        m_fnReflection = GetModelObject()->mo_toReflection.GetName();
        m_fnSpecular = GetModelObject()->mo_toSpecular.GetName();
        m_fnBump = GetModelObject()->mo_toBump.GetName();
      }
    }
    
    if ( m_bAttachments) {
      GetModelObject()->AutoSetAttachments();
    } else {
      GetModelObject()->RemoveAllAttachmentModels();
    }

    try {
      GetModelObject()->mo_toTexture.SetData_t(m_fnTexture);
      GetModelObject()->mo_toTexture.PlayAnim(m_iTextureAnimation, AOF_LOOPING);
      GetModelObject()->mo_toReflection.SetData_t(m_fnReflection);
      GetModelObject()->mo_toSpecular.SetData_t(m_fnSpecular);
      GetModelObject()->mo_toBump.SetData_t(m_fnBump);
    } catch (char *strError) {
      WarningMessage(strError);
    }

    // set model stretch
    StretchModel();
    ModelChangeNotify();

    switch(m_stClusterShadows) {
      case ST_NONE: {
        SetFlags(GetFlags()&~ENF_CLUSTERSHADOWS);
        //SetFlags(GetFlags()&~ENF_POLYGONALSHADOWS);
        break;
      }
      case ST_CLUSTER: {
        SetFlags(GetFlags()|ENF_CLUSTERSHADOWS);
        //SetFlags(GetFlags()&~ENF_POLYGONALSHADOWS);
        break;
      }
      case ST_POLYGONAL: {
        //SetFlags(GetFlags()|ENF_POLYGONALSHADOWS);
        SetFlags(GetFlags()&~ENF_CLUSTERSHADOWS);
        break;
      }
    }

    if (m_bBackground) {
      SetFlags(GetFlags()|ENF_BACKGROUND);
    } else {
      SetFlags(GetFlags()&~ENF_BACKGROUND);
    }

    try {
      m_aoLightAnimation.SetData_t(m_fnmLightAnimation);
    } catch (char *strError) {
      WarningMessage(TRANS("Cannot load '%s': %s"), (CTString&)m_fnmLightAnimation, strError);
      m_fnmLightAnimation = "";
    }

    if (m_aoLightAnimation.GetData() != NULL) {
      m_aoLightAnimation.PlayAnim(m_iLightAnimation, AOF_LOOPING);
    }

    if (m_penDestruction == NULL) {
      m_strDescription.PrintF("%s,%s undestroyable", (CTString&)m_fnModel.FileName(), (CTString&)m_fnTexture.FileName());
    } else {
      m_strDescription.PrintF("%s,%s -> %s", (CTString&)m_fnModel.FileName(), (CTString&)m_fnTexture.FileName(),
      m_penDestruction->GetName());
    }

    return;
  }

  // --------------------------------------------------------------------------------------
  // Returns bytes of memory used by this object.
  // --------------------------------------------------------------------------------------
  SLONG GetUsedMemory(void)
  {
    // initial
    SLONG slUsedMemory = sizeof(CLight) - sizeof(CRationalEntity) + CRationalEntity::GetUsedMemory();
    // add some more
    slUsedMemory += m_fnModel.Length();
    slUsedMemory += m_fnTexture.Length();
    slUsedMemory += m_fnReflection.Length();
    slUsedMemory += m_fnSpecular.Length();
    slUsedMemory += m_fnBump.Length();
    slUsedMemory += m_strName.Length();
    slUsedMemory += m_strDescription.Length();
    slUsedMemory += m_fnmLightAnimation.Length();
    slUsedMemory += 1* sizeof(CAnimObject); // only 1
    return slUsedMemory;
  }

  // --------------------------------------------------------------------------------------
  // Here is pre-moving calculations.
  // --------------------------------------------------------------------------------------
  void PreMoving()
  {
    // start moving
    if (m_bActive) {
      FLOAT3D fRot = FLOAT3D(m_fRotH, m_fRotP, m_fRotB);
      if (m_bDivide) {
        CPlacement3D plblah = CPlacement3D(en_vCurrentTranslationAbsolute, ANGLE3D(0.0F, 0.0F, 0.0F));
        CPlacement3D plThis = GetPlacement();
        plThis.pl_PositionVector = FLOAT3D(0.0F, 0.0F, 0.0F);
        plblah.AbsoluteToRelative(plThis);
        if (abs(plblah.pl_PositionVector(1)) > 1) {
          fRot /= sqrt(abs(plblah.pl_PositionVector(1)));
        }
      }
      
      if (m_penFace) {
        //direction vector to target
        FLOAT3D vTarget = (m_penFace->GetPlacement().pl_PositionVector-GetPlacement().pl_PositionVector).Normalize();
        FLOAT3D vRot = FLOAT3D(0.0F, 0.0F, 0.0F);

        //convert to absolute angles to target
        DirectionVectorToAngles(vTarget,vRot);

        if (m_bVTHeadingOnly) {
          vRot(2) = 0.0F;
          vRot(3) = 0.0F;
        }

        //angle distance between this and wanted rotation
        //FLOAT3D vRotDist=vRot-GetPlacement().pl_OrientationAngle;
        //scale rotation speed to match distance
        //vRot(1)=vRot(1)*vRotDist(1);
        //vRot(2)=vRot(2)*vRotDist(2);
        //vRot(3)=vRot(3)*vRotDist(3);
        //absolute placement to convert to relative rotation speeds
        CPlacement3D plAbs = CPlacement3D(FLOAT3D(0.0F, 0.0F, 0.0F),vRot);
        plAbs.AbsoluteToRelative(GetPlacement());

        //if not too close, set rotation speed
        if (plAbs.pl_OrientationAngle.Length() > 0.5) {
          SetDesiredRotation(plAbs.pl_OrientationAngle*20);
        } else {
          SetDesiredRotation(FLOAT3D(0.0F, 0.0F, 0.0F));
        }
      } else {
        if (abs(fRot(1))>m_fMaxRotation) {
          fRot(1) *= m_fMaxRotation / abs(fRot(1));
        }
        if (abs(fRot(2))>m_fMaxRotation) {
          fRot(2) *= m_fMaxRotation / abs(fRot(2));
        }
        if (abs(fRot(3))>m_fMaxRotation) {
          fRot(3) *= m_fMaxRotation / abs(fRot(3));
        }

        SetDesiredRotation(fRot);
      }

      SetDesiredTranslation(FLOAT3D(m_fTransX+m_fAddedTransX, m_fTransY+m_fAddedTransY, m_fTransZ+m_fAddedTransZ));
    }else{
      SetDesiredTranslation(FLOAT3D(0.0F, 0.0F, 0.0F));
      SetDesiredRotation(FLOAT3D(0.0F, 0.0F, 0.0F));
    }
    CMovableModelEntity::PreMoving();
  }

procedures:
  Main()
  {
    // initialize the model
    InitModelHolder();
    m_fbTransX = m_fTransX+m_fAddedTransX;
    m_fbTransY = m_fTransY+m_fAddedTransY;
    m_fbTransZ = m_fTransZ+m_fAddedTransZ;
    m_fbRotH = m_fRotH;
    m_fbRotP = m_fRotP;
    m_fbRotB = m_fRotB;
    SetHealth(m_fHealth);
    m_fRealHealth = m_fHealth;
    
    if (m_fMipFadeLenMetric > m_rMipFadeDistMetric) { m_fMipFadeLenMetric = m_rMipFadeDistMetric; }
    
    // convert metric factors to mip factors
    if (m_rMipFadeDistMetric > 0.0F) {
      m_fMipFadeDist = Log2(m_rMipFadeDistMetric*1024.0F*MIPRATIO);
      m_fMipFadeLen  = Log2((m_rMipFadeDistMetric+m_fMipFadeLenMetric)*1024.0F*MIPRATIO) - m_fMipFadeDist;
    } else {
      m_fMipFadeDist = 0.0F;
      m_fMipFadeLen  = 0.0F;
    }
    
    SetPhysicsFlags(EPF_MODEL_WALKING);
    SetFlags(GetFlags()|ENF_ALIVE);

    switch(m_eBA) {
      case EBA_STOP:{
        SetPhysicsFlags(GetPhysicsFlags()|EPF_ONBLOCK_STOP);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_SLIDE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_CLIMBORSLIDE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_BOUNCE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_PUSH);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_STOPEXACT);
        break;
      }
      case EBA_SLIDE:{
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_STOP);
        SetPhysicsFlags(GetPhysicsFlags()|EPF_ONBLOCK_SLIDE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_CLIMBORSLIDE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_BOUNCE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_PUSH);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_STOPEXACT);
        break;
      }
      case EBA_CLIMBORSLIDE:{
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_STOP);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_SLIDE);
        SetPhysicsFlags(GetPhysicsFlags()|EPF_ONBLOCK_CLIMBORSLIDE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_BOUNCE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_PUSH);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_STOPEXACT);
        break;
      }
      case EBA_BOUNCE:{
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_STOP);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_SLIDE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_CLIMBORSLIDE);
        SetPhysicsFlags(GetPhysicsFlags()|EPF_ONBLOCK_BOUNCE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_PUSH);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_STOPEXACT);
        break;
      }
      case EBA_PUSH:{
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_STOP);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_SLIDE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_CLIMBORSLIDE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_BOUNCE);
        SetPhysicsFlags(GetPhysicsFlags()|EPF_ONBLOCK_PUSH);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_STOPEXACT);
        break;
      }
      case EBA_STOPEXACT:{
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_STOP);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_SLIDE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_CLIMBORSLIDE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_BOUNCE);
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ONBLOCK_PUSH);
        SetPhysicsFlags(GetPhysicsFlags()|EPF_ONBLOCK_STOPEXACT);
        break;
      }
    }

    en_fAcceleration = m_fAcceleration;
    en_fDeceleration = m_fDeceleration;

    if (m_bColliding && m_bActive) {
      SetCollisionFlags(ECF_MODEL);
    } else {
      SetCollisionFlags(ECF_IMMATERIAL);
    } 

    autowait(_pTimer->TickQuantum);

    while(TRUE) {
      en_fAcceleration = m_fAcceleration;
      en_fDeceleration = m_fDeceleration;

      if (GetHealth() != m_fHealth) {
        m_fHealth = GetHealth();
        //SetHealth(m_fHealth);
      }

      //CPrintF(TRANS("%f\n"),GetHealth());
      if (m_bOriented) {
        SetPhysicsFlags(GetPhysicsFlags()|EPF_ORIENTEDBYGRAVITY);
      }else{
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ORIENTEDBYGRAVITY);
      }

      if (m_bTranslated) {
        SetPhysicsFlags(GetPhysicsFlags()|EPF_TRANSLATEDBYGRAVITY);
      }else{
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_TRANSLATEDBYGRAVITY);
      }

      if (m_bActive && m_bColliding) {
        SetCollisionFlags(ECF_MODEL);
      } else {
        SetCollisionFlags(ECF_IMMATERIAL);
      }  
    
      if (m_bRelative) {
        SetPhysicsFlags(GetPhysicsFlags()&~EPF_ABSOLUTETRANSLATE);
      }else{
        SetPhysicsFlags(GetPhysicsFlags()|EPF_ABSOLUTETRANSLATE);
      }

      // start moving
      if (m_bActive) {
        FLOAT3D fRot = FLOAT3D(m_fRotH, m_fRotP, m_fRotB);
        if (m_bDivide) {
          CPlacement3D plblah = CPlacement3D(en_vCurrentTranslationAbsolute, ANGLE3D(0.0F, 0.0F, 0.0F));

          if (m_bPPhys == TRUE) { 
            plblah = CPlacement3D(en_vCurrentTranslationAbsolute,ANGLE3D(GetPlacement().pl_PositionVector));
          }

          CPlacement3D plThis = GetPlacement();
          plThis.pl_PositionVector = FLOAT3D(0.0F, 0.0F, 0.0F);
          plblah.AbsoluteToRelative(plThis);

          if (abs(plblah.pl_PositionVector(1)) > 1) {
            fRot /= sqrt(abs(plblah.pl_PositionVector(1)));
          }
          //CPrintF(TRANS("Old H: %f, New H: %f, Speed: %f\n"),m_fRotH,fRot(1),plblah.pl_PositionVector(3));
        }

        if (abs(fRot(1)) > m_fMaxRotation) {
          fRot(1) *= m_fMaxRotation / abs(fRot(1));
        }
        if (abs(fRot(2)) > m_fMaxRotation) {
          fRot(2) *= m_fMaxRotation / abs(fRot(2));
        }
        if (abs(fRot(3)) >m_fMaxRotation) {
          fRot(3) *= m_fMaxRotation / abs(fRot(3));
        }

        SetDesiredRotation(fRot);
        SetDesiredTranslation(FLOAT3D(m_fTransX+m_fAddedTransX,m_fTransY+m_fAddedTransY,m_fTransZ+m_fAddedTransZ));
      } else {
        SetDesiredTranslation(FLOAT3D(0.0F, 0.0F, 0.0F));
        SetDesiredRotation(FLOAT3D(0.0F, 0.0F, 0.0F));
      }
      
      // wait forever
      wait(_pTimer->TickQuantum) {
        // on the beginning
        on(EBegin): {
          resume;
        }
        
        // activate/deactivate shows/hides model
        on (EActivate): {
          SwitchToModel();
          SetFlags(GetFlags()|ENF_ALIVE);
          m_bActive = TRUE;
          
          resume;
        }
        
        on (EDeactivate): {
          SwitchToEditorModel();
          m_bActive = FALSE;
          resume;
        }
        
        // when your parent is destroyed
        on(ERangeModelDestruction): {
          // for each child of this entity
          {FOREACHINLIST(CEntity, en_lnInParent, en_lhChildren, itenChild) {
            // send it destruction event
            itenChild->SendEvent(ERangeModelDestruction());
          }}
          // destroy yourself
          Destroy();
          resume;
        }
        
        // when dead
        on(EDeath): {
          SendToTarget(m_penDeathTarget, EET_TRIGGER, m_penLastDamager);
          ForceFullStop();
          SendEvent(EDeactivate());
          SetHealth(m_fRealHealth);
          SetFlags(GetFlags()&~ENF_ALIVE);
          resume;
        }
        
        // when animation should be changed
        on(EChangeAnim eChange): {
          m_iModelAnimation   = eChange.iModelAnim;
          m_iTextureAnimation = eChange.iTextureAnim;
          m_iLightAnimation   = eChange.iLightAnim;

          if (m_aoLightAnimation.GetData() != NULL) {
            m_aoLightAnimation.PlayAnim(m_iLightAnimation, eChange.bLightLoop ? AOF_LOOPING:0);
          }
          if (GetModelObject()->GetData() != NULL) {
            GetModelObject()->PlayAnim(m_iModelAnimation, eChange.bModelLoop ? AOF_LOOPING:0);
          }
          if (GetModelObject()->mo_toTexture.GetData() != NULL) {
            GetModelObject()->mo_toTexture.PlayAnim(m_iTextureAnimation, eChange.bTextureLoop ? AOF_LOOPING:0);
          }

          resume;
        }
        
        on(EStart eStart):{
          if (IsOfClass(eStart.penCaused, "Player")) {
            m_ulVisibleMask = 1<<((CPlayer&)*eStart.penCaused).GetMyPlayerIndex();
            m_penPlayer = eStart.penCaused;
          }
          resume;
        }
        
        on(EStop eStop):{
          m_ulVisibleMask = 1;
          m_penPlayer = NULL;
          resume;
        }
               
        on (ETimer) : { stop; }
        
        otherwise(): {
          resume;
        }
      };
    } // for while(TRUE)
  }
};
