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
#include "Skeleton.h"

#include <Engine/Ska/StringTable.h>
#include <Engine/Base/Stream.h>
#include <Engine/Templates/DynamicContainer.cpp>

#define SKELETON_VERSION  6
#define SKELETON_ID       "SKEL"

// write to stream
void CSkeleton::Write_t(CTStream *ostrFile)
{
  INDEX ctslods = skl_aSkeletonLODs.Count();
  
  ostrFile->WriteID_t(CChunkID(SKELETON_ID)); // write id
  (*ostrFile) << (INDEX)SKELETON_VERSION; // write version
  (*ostrFile) << ctslods; // write lods count

  // for each lod in skeleton
  for (INDEX islod = 0; islod < ctslods; islod++)
  {
    SkeletonLOD &slod = skl_aSkeletonLODs[islod];
    INDEX ctBones = slod.slod_aBones.Count();

    (*ostrFile) << slod.slod_fnSourceFile; // write source file name
    (*ostrFile) << slod.slod_fMaxDistance; // write MaxDistance
    (*ostrFile) << ctBones; // write bone count

    // write skeleton bones
    for (INDEX iBone = 0; iBone < ctBones; iBone++)
    {
      CTString strNameID = ska_GetStringFromTable(slod.slod_aBones[iBone].sb_iID);
      CTString strParentID = ska_GetStringFromTable(slod.slod_aBones[iBone].sb_iParentID);
      SkeletonBone &sb = slod.slod_aBones[iBone];
      
      (*ostrFile) << strNameID; // write ID
      (*ostrFile) << strParentID; // write Parent ID
      //(*ostrFile)<<slod.slod_aBones[ib].sb_iParentIndex;
      
      ostrFile->Write_t(&sb.sb_mAbsPlacement, sizeof(FLOAT)* 12); // write AbsPlacement matrix
      ostrFile->Write_t(&sb.sb_qvRelPlacement, sizeof(QVect)); // write RelPlacement Qvect stuct
      (*ostrFile) << sb.sb_fOffSetLen; // write offset len
      (*ostrFile) << sb.sb_fBoneLength; // write bone length
    }
  }
}

// read from stream
void CSkeleton::Read_t(CTStream *istrFile)
{
  INDEX iFileVersion;
  INDEX ctslods;

  // read chunk id
  istrFile->ExpectID_t(CChunkID(SKELETON_ID));
  
  (*istrFile) >> iFileVersion;

  // check file version
  if (iFileVersion != SKELETON_VERSION) {
    ThrowF_t(TRANS("File '%s'.\nInvalid skeleton file version.\nExpected Ver \"%d\" but found \"%d\"\n"),
      (const char*)istrFile->GetDescription(), SKELETON_VERSION, iFileVersion);
  }

  (*istrFile) >> ctslods; // read skeleton lod count

  if (ctslods > 0) {
    skl_aSkeletonLODs.Expand(ctslods);
  }

  // for each skeleton lod
  for (INDEX islod = 0; islod < ctslods; islod++)
  {
    SkeletonLOD &slod = skl_aSkeletonLODs[islod];

    INDEX ctBones;
    (*istrFile) >> slod.slod_fnSourceFile; // read source file name
    (*istrFile) >> slod.slod_fMaxDistance; // read MaxDistance
    (*istrFile) >> ctBones; // read bone count
    
    slod.slod_aBones.New(ctBones); // create bone array

    // read skeleton bones
    for (INDEX iBone = 0; iBone < ctBones; iBone++)
    {
      CTString strNameID;
      CTString strParentID;
      SkeletonBone &sb = slod.slod_aBones[iBone];
      
      (*istrFile) >> strNameID; // read ID
      (*istrFile) >> strParentID; // read Parent ID

      //(*istrFile)>>slod.slod_aBones[ib].sb_iParentIndex ;
      sb.sb_iID = ska_GetIDFromStringTable(strNameID);
      sb.sb_iParentID = ska_GetIDFromStringTable(strParentID);

      istrFile->Read_t(&sb.sb_mAbsPlacement, sizeof(FLOAT)* 12); // read AbsPlacement matrix
      istrFile->Read_t(&sb.sb_qvRelPlacement, sizeof(QVect)); // read RelPlacement Qvect stuct
      (*istrFile) >> sb.sb_fOffSetLen; // read offset len
      (*istrFile) >> sb.sb_fBoneLength; // read bone length
    }
  }
}