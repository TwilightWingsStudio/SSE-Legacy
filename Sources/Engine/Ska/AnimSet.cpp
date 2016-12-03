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

#include "StdH.h"
#include <Engine/Ska/AnimSet.h>
#include <Engine/Templates/StaticArray.h>
#include <Engine/Base/CTString.h>
#include <Engine/Ska/StringTable.h>
#include <Engine/Templates/StaticArray.cpp>
#include <Engine/templates/DynamicContainer.cpp>
#include <Engine/Base/Stream.h>
#include <Engine/Base/Console.h>
#include <Engine/Math/Functions.h>
#include <Engine/Math/Geometry.h>
#include <Engine/Base/Timer.h>

// table for removed frames
static CStaticArray<BOOL> aiRemFrameTable;
// precalculated angles for rotations
static CStaticArray<ANGLE3D> aangAngles;

CAnimSet::CAnimSet() {}

CAnimSet::~CAnimSet() {}

// try to remove 2. keyframe in rotation
BOOL RemoveRotFrame(AnimRot &ar1,AnimRot &ar2,AnimRot &ar3,FLOAT fTreshold)
{
  ANGLE3D ang1,ang2,ang2i,ang3;
  FLOATmatrix3D m2i;
  // calculate slerp factor for ar2'
  FLOAT fSlerpFactor = (FLOAT)(ar2.ar_iFrameNum - ar1.ar_iFrameNum)/(FLOAT)(ar3.ar_iFrameNum - ar1.ar_iFrameNum);
  // calculate ar2'
  FLOATquat3D q2i = Slerp<FLOAT>(fSlerpFactor,ar1.ar_qRot,ar3.ar_qRot);
  // read precalculated values
  ang1 = aangAngles[ar1.ar_iFrameNum];
  ang2 = aangAngles[ar2.ar_iFrameNum];
  ang3 = aangAngles[ar3.ar_iFrameNum];
  q2i.ToMatrix(m2i);
  DecomposeRotationMatrixNoSnap(ang2i,m2i);

  for(INDEX i=1;i<4;i++)
  {
    if( ((ang2(i) < ang3(i)) && (ang2(i) < ang1(i))) || ((ang2(i) > ang3(i)) && (ang2(i) > ang1(i))) )
    {
      // this is extrem
      if(Abs(ang2(i)) > 0.1f) return FALSE;
    }
    FLOAT fErr = Abs(ang2(i)-ang2i(i)) / Abs(ang3(i) - ang1(i));
    if(Abs(ang2(i)-ang2i(i)) < 0.1f) continue;
    if(fErr>fTreshold) return FALSE;
  }
  return TRUE;
}

// try to remove 2. keyrame in translation
BOOL RemovePosFrame(AnimPos &ap1,AnimPos &ap2,AnimPos &ap3,FLOAT fTreshold)
{
  FLOAT fLerpFactor = (FLOAT)(ap2.ap_iFrameNum - ap1.ap_iFrameNum)/(FLOAT)(ap3.ap_iFrameNum - ap1.ap_iFrameNum);
  FLOAT3D v2i = Lerp(ap1.ap_vPos,ap3.ap_vPos,fLerpFactor);

  FLOAT3D v1 = ap1.ap_vPos;
  FLOAT3D v2 = ap2.ap_vPos;
  FLOAT3D v3 = ap3.ap_vPos;

  for(INDEX i=1;i<4;i++)
  {
    if( ((v2(i) < v3(i)) && (v2(i) < v1(i))) || ((v2(i) > v3(i)) && (v2(i) > v1(i))) )
    {
      // extrem
      if(Abs(v2(i)) > 0.001f) return FALSE;
    }
    FLOAT fErr = Abs(v2(i)-v2i(i)) / Abs(v3(i) - v1(i));
    if(Abs(v2(i)-v2i(i)) < 0.001f) continue;
    if(fErr>fTreshold) return FALSE;
  }
  return TRUE;
}

// find next keyframe that havent been marked as removed
INDEX FindNextFrame(INDEX ifnToFind)
{
  INDEX ctfn = aiRemFrameTable.Count();
  if(ifnToFind >= ctfn) return -1;
  if(aiRemFrameTable[ifnToFind] == FALSE) return ifnToFind;
  for(INDEX ifn=ifnToFind;ifn<ctfn;ifn++)
  {
    if(aiRemFrameTable[ifn] == FALSE) return ifn;
  }
  return -1;
}

// optimize all animations
void CAnimSet::Optimize()
{
  INDEX ctan=as_Anims.Count();
  for(INDEX ian=0;ian<ctan;ian++)
  {
    Animation &an = as_Anims[ian];
    //CalculateExtraSpins(an);
    OptimizeAnimation(an,an.an_fTreshold);
  }
}

// optimize animation
void CAnimSet::OptimizeAnimation(Animation &an, FLOAT fTreshold)
{
  INDEX ctfn = an.an_iFrames;
  INDEX ctbe = an.an_abeBones.Count();
  aiRemFrameTable.Clear();
  aiRemFrameTable.New(ctfn);
  aangAngles.Clear();
  aangAngles.New(ctfn);

  for(INDEX ibe=0;ibe<ctbe;ibe++)
  {
    BoneEnvelope &be = an.an_abeBones[ibe];
    // calculate length on bone in default pos
    be.be_OffSetLen = (FLOAT3D(be.be_mDefaultPos[3],be.be_mDefaultPos[7],be.be_mDefaultPos[11])).Length();
    // create a table for removed frames
    memset(&aiRemFrameTable[0],0,sizeof(BOOL)*ctfn);
    memset(&aangAngles[0],0,sizeof(ANGLE3D)*ctfn);
    // fill array of decomposed matrices
    FLOATmatrix3D mat;
    for(INDEX im=0;im<ctfn;im++)
    {
      be.be_arRot[im].ar_qRot.ToMatrix(mat);
      DecomposeRotationMatrixNoSnap(aangAngles[im],mat);
    }
    // try to remove rotations, steping by 2
    INDEX iloop=0;
    for(;iloop<ctfn;iloop++)
    {
      INDEX ctRemoved=0;
      // for each frame in bone envelope
      for(INDEX ifn=0;ifn<ctfn;ifn+=2)
      {
        INDEX iInd1 = FindNextFrame(ifn);
        INDEX iInd2 = FindNextFrame(iInd1+1);
        INDEX iInd3 = FindNextFrame(iInd2+1);
        // !!!! try only ind3
        if((iInd1 < 0)||(iInd2 < 0)||(iInd3 < 0)) break;

        AnimRot *parCurent = &be.be_arRot[iInd1];
        AnimRot *parNext   = &be.be_arRot[iInd2];
        AnimRot *parLast   = &be.be_arRot[iInd3];
        if(RemoveRotFrame(*parCurent,*parNext,*parLast,fTreshold))
        {
          aiRemFrameTable[iInd2] = TRUE;
          ctRemoved++;
        }
      }
      if(ctRemoved==0)
      {
        // exit if no keyframe has been removed
        break;
      }
    }
    // create temp array for rotations that are not removed
    CStaticStackArray<struct AnimRot> arRot;
    // for each removed frame
    for(INDEX ifnr=0;ifnr<ctfn;ifnr++)
    {
      // if frame is not in table for removed frames add it to temp arRot array
      if(!aiRemFrameTable[ifnr])
      {
        AnimRot &ar = arRot.Push();
        ar = be.be_arRot[ifnr];
      }
    }
    // count frames that are left
    INDEX ctfl = arRot.Count();
    // create new array for bone envelope
    be.be_arRot.Clear();
    be.be_arRot.New(ctfl);
    // copy array of rotaions
    INDEX fl=0;
    for(;fl<ctfl;fl++)
    {
      be.be_arRot[fl] = arRot[fl];
    }
    arRot.Clear();
    
    // do same thing for positions
    // clear table for removed frames
    memset(&aiRemFrameTable[0],0,sizeof(BOOL)*ctfn);
    // try to remove translations steping by 2
    for(iloop=0;iloop<ctfn;iloop++)
    {
      INDEX ctRemoved=0;
      for(INDEX ifn=0;ifn<ctfn;ifn+=2)
      {
        INDEX iInd1 = FindNextFrame(ifn);
        INDEX iInd2 = FindNextFrame(iInd1+1);
        INDEX iInd3 = FindNextFrame(iInd2+1);
        // !!!! try only ind3
        if((iInd1 < 0)||(iInd2 < 0)||(iInd3 < 0)) break;

        AnimPos *papCurent = &be.be_apPos[iInd1];
        AnimPos *papNext   = &be.be_apPos[iInd2];
        AnimPos *papLast   = &be.be_apPos[iInd3];
        if(RemovePosFrame(*papCurent,*papNext,*papLast,fTreshold))
        {
          aiRemFrameTable[iInd2] = TRUE;
          ctRemoved++;
        }
      }
      if(ctRemoved==0)
      {
        // exit if no keyframe has been removed
        break;
      }
    }
    CStaticStackArray<struct AnimPos> apPos;
    // count removed frames
    for(INDEX ifr=0;ifr<ctfn;ifr++)
    {
      if(!aiRemFrameTable[ifr])
      {
        AnimPos &ap = apPos.Push();
        ap = be.be_apPos[ifr];
      }
    }
    // count frames that are left
    ctfl = apPos.Count();
    // create new array for bone envelope
    be.be_apPos.Clear();
    be.be_apPos.New(ctfl);
    // copy array of translations
    for(fl=0;fl<ctfl;fl++)
    {
      be.be_apPos[fl] = apPos[fl];
    }
    apPos.Clear();
  }
  aiRemFrameTable.Clear();

  // if morph envelope has all factors 0 remove it
  CStaticStackArray<struct MorphEnvelope> aMorphs;

  INDEX ctme = an.an_ameMorphs.Count();
  for(INDEX ime=0;ime<ctme;ime++)
  {
    MorphEnvelope &me = an.an_ameMorphs[ime];
    // morph factors count
    INDEX ctwm=me.me_aFactors.Count();
    // index of wertex morph
    INDEX iwm=0;
    BOOL bMorphIsZero = TRUE;
    while(bMorphIsZero)
    {
      if(iwm>=ctwm) break;
      FLOAT &fMorphFactor = me.me_aFactors[iwm];
      // check if morph factor is 0
      bMorphIsZero = fMorphFactor == 0;
      iwm++;
    }
    // dont remove this morph envelope
    if(!bMorphIsZero)
    {
      // copy this morphmap to temp array of morph envelopes
      MorphEnvelope &meNew = aMorphs.Push();
      meNew = me;
    }
  }
  INDEX ctmeNew = aMorphs.Count();
  // crate new array for morph envelopes
  an.an_ameMorphs.Clear();
  an.an_ameMorphs.New(ctmeNew);
  // copy morph back to animations array of morph envelopes
  for(INDEX imeNew=0;imeNew<ctmeNew;imeNew++)
  {
    an.an_ameMorphs[imeNew] = aMorphs[imeNew];
  }
}

// add animation to animset
void CAnimSet::AddAnimation(Animation *pan)
{
  INDEX ctan = as_Anims.Count();
  as_Anims.Expand(ctan+1);
  Animation &an = as_Anims[ctan];
  an = *pan;
}

// remove animation from animset
void CAnimSet::RemoveAnimation(Animation *pan)
{
  INDEX ctan = as_Anims.Count();
  ASSERT(ctan>0);
  ASSERT(pan!=NULL);
  
  // copy all animations to temp array
  CStaticArray<struct Animation> animsTemp;
  animsTemp.New(ctan-1);
  INDEX ianNew=0;
  for(INDEX ian=0;ian<ctan;ian++)
  {
    Animation *panTemp = &as_Anims[ian];
    if(panTemp != pan)
    {
      // copy anims
      animsTemp[ianNew] = *panTemp;
      ianNew++;
    }
  }
  as_Anims = animsTemp;  
}

// clear animset
void CAnimSet::Clear(void)
{
  INDEX ctAnims = as_Anims.Count();
  for(INDEX iAnims=0;iAnims<ctAnims;iAnims++)
  {
    Animation &an = as_Anims[iAnims];
    INDEX ctBoneEnv = an.an_abeBones.Count();
    INDEX ctMorphEnv = an.an_ameMorphs.Count();
    for(INDEX iBoneEnv=0;iBoneEnv<ctBoneEnv;iBoneEnv++)
    {
      BoneEnvelope &be = an.an_abeBones[iBoneEnv];
      //be.be_aqvPlacement.Clear();
      be.be_apPos.Clear();
      be.be_arRot.Clear();
    }
    for(INDEX iMorphEnv=0;iMorphEnv<ctMorphEnv;iMorphEnv++)
    {
      MorphEnvelope &me = an.an_ameMorphs[iMorphEnv];
      me.me_aFactors.Clear();
    }
    an.an_abeBones.Clear();
    an.an_ameMorphs.Clear();
  }
  as_Anims.Clear();
}

// Count used memory
SLONG CAnimSet::GetUsedMemory(void)
{
  SLONG slMemoryUsed = sizeof(*this);
  INDEX ctAnims = as_Anims.Count();
  for(INDEX ias=0;ias<ctAnims;ias++) {
    Animation &an = as_Anims[ias];
    slMemoryUsed+=sizeof(an);
    // for each bone envelope
    INDEX ctbe = an.an_abeBones.Count();
    for(INDEX ibe=0;ibe<ctbe;ibe++) {
      BoneEnvelope &be = an.an_abeBones[ibe];
      slMemoryUsed+=sizeof(be);
      slMemoryUsed+=be.be_apPos.Count() * sizeof(AnimPos);
      slMemoryUsed+=be.be_arRot.Count() * sizeof(AnimRot);
      slMemoryUsed+=be.be_arRotOpt.Count() * sizeof(AnimRotOpt);
    }
    // for each morph envelope
    INDEX ctme = an.an_ameMorphs.Count();
    for(INDEX ime=0;ime<ctme;ime++) {
      MorphEnvelope &me = an.an_ameMorphs[ime];
      slMemoryUsed+=sizeof(me);
      slMemoryUsed+=sizeof(FLOAT) * me.me_aFactors.Count() + 12;
    }
  }
  return slMemoryUsed;
}
