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
#include <Engine/Base/Stream.h>
#include <Engine/Ska/StringTable.h>
#include <Engine/Math/Functions.h>
#include <Engine/Math/Geometry.h>

#define ANIMSET_VERSION  14
#define ANIMSET_ID       "ANIM"

// if rotations are compresed does loader also fills array of uncompresed rotations
static BOOL bAllRotations = FALSE;

void RememberUnCompresedRotatations(BOOL bRemember)
{
  bAllRotations = bRemember;
}


// conpres normal
static void CompressAxis(const FLOAT3D &vNormal, UWORD &ubH, UWORD &ubP)
{
  ANGLE h, p;

  const FLOAT &x = vNormal(1);
  const FLOAT &y = vNormal(2);
  const FLOAT &z = vNormal(3);

  // calculate pitch
  p = ASin(y);

  // if y is near +1 or -1
  if (y > 0.99999 || y < -0.99999) {
    h = 0; // heading is irrelevant
  } else { // otherwise
    h = ATan2(-x, -z); // calculate heading
  }

  h = (h / 360.0f) + 0.5f;
  p = (p / 360.0f) + 0.5f;
  ASSERT(h >= 0 && h <= 1);
  ASSERT(p >= 0 && p <= 1);
  ubH = UWORD(h * 65535);
  ubP = UWORD(p * 65535);
}

// write to stream
void CAnimSet::Write_t(CTStream *ostrFile)
{
  ostrFile->WriteID_t(CChunkID(ANIMSET_ID)); // write id
  (*ostrFile) << (INDEX)ANIMSET_VERSION; // write version

  INDEX ctan = as_Anims.Count(); // count anims
  (*ostrFile) << ctan; // write anim count

  for (int ian = 0; ian < ctan; ian++)
  {
    Animation &an = as_Anims[ian];
    CTString pstrNameID = ska_GetStringFromTable(an.an_iID);
    
    (*ostrFile) << an.an_fnSourceFile; // write anim source file
    (*ostrFile) << pstrNameID; // write anim id
    (*ostrFile) << an.an_fSecPerFrame; // write secperframe
    (*ostrFile) << an.an_iFrames; // write num of frames
    (*ostrFile) << an.an_fTreshold; // write treshold
    (*ostrFile) << an.an_bCompresed; // write if compresion is used
    (*ostrFile) << an.an_bCustomSpeed; // write bool if animstion uses custom speed

    INDEX ctbe = an.an_abeBones.Count();
    INDEX ctme = an.an_ameMorphs.Count();

    (*ostrFile) << ctbe; // write bone envelopes count

    // for each bone envelope
    for (int ibe = 0; ibe < ctbe; ibe++)
    {
      BoneEnvelope &be = an.an_abeBones[ibe];
      CTString pstrNameID = ska_GetStringFromTable(be.be_iBoneID);
      
      (*ostrFile) << pstrNameID; // write bone envelope ID
      ostrFile->Write_t(&be.be_mDefaultPos[0], sizeof(FLOAT)* 12); // write default pos(matrix12)
      
      INDEX ctp = be.be_apPos.Count(); // count positions
      (*ostrFile) << ctp; // write position count

      // for each position
      for (INDEX ip = 0; ip < ctp; ip++) {
        ostrFile->Write_t(&be.be_apPos[ip], sizeof(AnimPos)); // write position
      }
      
      INDEX ctRotations = be.be_arRot.Count(); // count rotations
      (*ostrFile) << ctRotations; // write rotations count

      // for each rotation
      for (INDEX ir = 0; ir<ctRotations; ir++)
      {
        AnimRot &arRot = be.be_arRot[ir];
        ostrFile->Write_t(&arRot, sizeof(AnimRot)); // write rotation
      }

      INDEX ctOptRotations = be.be_arRotOpt.Count();

      if (ctOptRotations > 0) {
        // OPTIMISED ROTATIONS ARE NOT SAVED !!!
        // use RememberUnCompresedRotatations();
        ASSERT(ctRotations >= ctOptRotations);
      }
      
      (*ostrFile) << be.be_OffSetLen; // write offsetlen
    }

    (*ostrFile) << ctme; // write morph envelopes count

    // write morph envelopes
    for (int ime = 0; ime < ctme; ime++)
    {
      MorphEnvelope &me = an.an_ameMorphs[ime];
      CTString pstrNameID = ska_GetStringFromTable(me.me_iMorphMapID);
      (*ostrFile) << pstrNameID; // write morph map ID
      
      INDEX ctmf = me.me_aFactors.Count();
      (*ostrFile) << ctmf; // write morph factors count
      ostrFile->Write_t(&me.me_aFactors[0], sizeof(FLOAT)*ctmf); // write morph factors array
    }
  }
}

// read from stream
void CAnimSet::Read_t(CTStream *istrFile)
{
  INDEX iFileVersion;
  
  istrFile->ExpectID_t(CChunkID(ANIMSET_ID)); // read chunk id
  (*istrFile) >> iFileVersion; // read file version

  // check file version
  if (iFileVersion != ANIMSET_VERSION)
  {
    ThrowF_t(TRANS("File '%s'.\nInvalid animset file version. Expected Ver \"%d\" but found \"%d\"\n"),
      (const char*)istrFile->GetDescription(), ANIMSET_VERSION, iFileVersion);
  }

  INDEX ctan;
  (*istrFile) >> ctan; // read anims count
  as_Anims.New(ctan); // create anims array

  for (int ian = 0; ian < ctan; ian++)
  {
    Animation &an = as_Anims[ian];
    CTString pstrNameID;
    
    (*istrFile) >> an.an_fnSourceFile; // read anim source file
    (*istrFile >> pstrNameID); // read Anim ID
    an.an_iID = ska_GetIDFromStringTable(pstrNameID);
    
    (*istrFile) >> an.an_fSecPerFrame; // read secperframe
    (*istrFile) >> an.an_iFrames; // read num of frames
    (*istrFile) >> an.an_fTreshold; // read treshold
    (*istrFile) >> an.an_bCompresed; // read if compresion is used
    (*istrFile) >> an.an_bCustomSpeed; // read bool if animstion uses custom speed

    INDEX ctbe;
    INDEX ctme;

    // read bone envelopes count
    (*istrFile) >> ctbe;

    // create bone envelopes array
    an.an_abeBones.New(ctbe);

    // read bone envelopes
    for (int ibe = 0; ibe < ctbe; ibe++)
    {
      BoneEnvelope &be = an.an_abeBones[ibe];
      CTString pstrNameID;

      (*istrFile) >> pstrNameID; // read bone envelope ID
      be.be_iBoneID = ska_GetIDFromStringTable(pstrNameID); 
      istrFile->Read_t(&be.be_mDefaultPos[0], sizeof(FLOAT)* 12); // read default pos(matrix12)

      INDEX ctp;
      (*istrFile) >> ctp; // read pos array
      be.be_apPos.New(ctp);

      // for each position
      for (INDEX ip = 0; ip < ctp; ip++) {
        istrFile->Read_t(&be.be_apPos[ip], sizeof(AnimPos));
      }

      INDEX ctr;
      (*istrFile) >> ctr; // read rot array count

      if (!an.an_bCompresed) {
        be.be_arRot.New(ctr); // create array for uncompresed rotations
      } else {
        // if flag is set to remember uncompresed rotations
        if (bAllRotations) {
          be.be_arRot.New(ctr); // create array for uncompresed rotations
        }
        
        be.be_arRotOpt.New(ctr); // create array for compresed rotations
      }

      // for each rotation
      for (INDEX ir = 0; ir < ctr; ir++)
      {
        AnimRot arRot;// = be.be_arRot[ir];
        istrFile->Read_t(&arRot, sizeof(AnimRot));

        if (!an.an_bCompresed) {
          be.be_arRot[ir] = arRot;
        } else {
          if (bAllRotations) {
            be.be_arRot[ir] = arRot; // fill uncompresed rotations
          }

          // optimize quaternions
          FLOAT3D vAxis;
          ANGLE aAngle;
          UWORD ubH, ubP;
          FLOATquat3D &qRot = arRot.ar_qRot;
          AnimRotOpt &aroRot = be.be_arRotOpt[ir];
          qRot.ToAxisAngle(vAxis, aAngle);
          CompressAxis(vAxis, ubH, ubP);

          // compress angle
          aroRot.aro_aAngle = aAngle * ANG_COMPRESIONMUL;
          aroRot.aro_iFrameNum = arRot.ar_iFrameNum;
          aroRot.aro_ubH = ubH;
          aroRot.aro_ubP = ubP;
          be.be_arRotOpt[ir] = aroRot;
        }
      }
      
      (*istrFile) >> be.be_OffSetLen; // read offsetlen
    }
    
    (*istrFile) >> ctme; // read morph envelopes
    an.an_ameMorphs.New(ctme); // create morph envelopes array

    // read morph envelopes
    for (int ime = 0; ime < ctme; ime++)
    {
      MorphEnvelope &me = an.an_ameMorphs[ime];
      CTString pstrNameID;
      INDEX ctmf;

      (*istrFile) >> pstrNameID; // read morph envelope ID
      me.me_iMorphMapID = ska_GetIDFromStringTable(pstrNameID);
      
      (*istrFile) >> ctmf; // read morph factors count
      me.me_aFactors.New(ctmf); // create morph factors array
      istrFile->Read_t(&me.me_aFactors[0], sizeof(FLOAT)*ctmf); // read morph factors
    }
  }
}