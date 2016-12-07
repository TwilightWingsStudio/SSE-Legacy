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

#include <Engine/Models/ModelData.h>
#include <Engine/Models/Normals.h>
#include <Engine/Base/Stream.h>
#include <Engine/Base/CTString.inl>

#include <Engine/Templates/StaticArray.cpp>
#include <Engine/Templates/DynamicArray.cpp>

#include <Engine/Templates/Stock_CModelData.h>

extern UBYTE aubGouraudConv[16384];

// model LOD biasing control
extern FLOAT mdl_fLODMul;
extern FLOAT mdl_fLODAdd;
extern INDEX mdl_iLODDisappear; // 0=never, 1=ignore bias, 2=with bias
extern INDEX mdl_bFineQuality;  // 0=force to 8-bit, 1=optimal

// if any surface in model that we are currently reading has any transparency
extern BOOL _bHasAlpha;

//--------------------------------------------------------------------------------------------
//------------------------------------------ WRITE
void CModelData::Write_t(CTStream *pFile)  // throw char *
{
  INDEX i;

  PtrsToIndices();
  // Save main ID
  pFile->WriteID_t(CChunkID("MDAT"));

  // Save version number
  pFile->WriteID_t(CChunkID(MODEL_VERSION));

  // Save flags
  pFile->Write_t(&md_Flags, sizeof(ULONG));

  // Save vertices and frames ct
  pFile->WriteFullChunk_t(CChunkID("IVTX"), &md_VerticesCt, sizeof(INDEX));
  pFile->WriteFullChunk_t(CChunkID("IFRM"), &md_FramesCt, sizeof(INDEX));

  // write array of 8-bit or 16-bit compressed vertices
  if (md_Flags & MF_COMPRESSED_16BIT) {
    pFile->WriteFullChunk_t(CChunkID("AV17"), &md_FrameVertices16[0], md_VerticesCt * md_FramesCt *
      sizeof(struct ModelFrameVertex16));
  }
  else {
    pFile->WriteFullChunk_t(CChunkID("AFVX"), &md_FrameVertices8[0], md_VerticesCt * md_FramesCt *
      sizeof(struct ModelFrameVertex8));
  }

  // Save frame info array
  pFile->WriteFullChunk_t(CChunkID("AFIN"), &md_FrameInfos[0], md_FramesCt * sizeof(struct ModelFrameInfo));

  // Save frame main mip vertices array
  pFile->WriteFullChunk_t(CChunkID("AMMV"), &md_MainMipVertices[0], md_VerticesCt * sizeof(FLOAT3D));

  // Save vertex mip-mask array
  pFile->WriteFullChunk_t(CChunkID("AVMK"), &md_VertexMipMask[0], md_VerticesCt * sizeof(ULONG));

  // Save mip levels counter
  pFile->WriteFullChunk_t(CChunkID("IMIP"), &md_MipCt, sizeof(INDEX));

  // Save mip factors array
  pFile->WriteFullChunk_t(CChunkID("FMIP"), &md_MipSwitchFactors[0], MAX_MODELMIPS * sizeof(float));

  // Save all model mip infos
  for (i = 0; i < md_MipCt; i++) md_MipInfos[i].Write_t(pFile);

  // Save patches
  pFile->WriteID_t(CChunkID("PTC2"));
  for (INDEX iPatch = 0; iPatch < MAX_TEXTUREPATCHES; iPatch++) md_mpPatches[iPatch].Write_t(pFile);

  // Save texture width and height in MEX-es
  pFile->WriteFullChunk_t(CChunkID("STXW"), &md_Width, sizeof(MEX));
  pFile->WriteFullChunk_t(CChunkID("STXH"), &md_Height, sizeof(MEX));

  // Save value for shading type
  pFile->Write_t(&md_ShadowQuality, sizeof(SLONG));

  // Save static stretch value
  pFile->Write_t(&md_Stretch, sizeof(FLOAT3D));

  // Save model offset
  pFile->Write_t(&md_vCenter, sizeof(FLOAT3D));

  // Save count of collision boxes
  INDEX ctCollisionBoxes = md_acbCollisionBox.Count();
  pFile->Write_t(&ctCollisionBoxes, sizeof(INDEX));
  md_acbCollisionBox.Lock();

  // save all collision boxes
  for (INDEX iCollisionBox = 0; iCollisionBox < ctCollisionBoxes; iCollisionBox++) {
    md_acbCollisionBox[iCollisionBox].Write_t(pFile); // save current collision box
  }

  md_acbCollisionBox.Unlock();

  // save boolean defining collision type for this model
  pFile->WriteID_t(CChunkID("COLI"));
  *pFile << md_bCollideAsCube;

  // Save count of attached positions
  INDEX ctAttachedPositions = md_aampAttachedPosition.Count();
  *pFile << ctAttachedPositions;

  FOREACHINDYNAMICARRAY(md_aampAttachedPosition, CAttachedModelPosition, itamp) {
    itamp->Write_t(pFile);
  }

  // Save color names (get count of valid names, write count and then write existing names)
  INDEX iValidColorsCt = 0;
  for (i = 0; i < MAX_COLOR_NAMES; i++)
  {
    if (md_ColorNames[i] != "") {
      iValidColorsCt++;
    }
  }

  pFile->WriteFullChunk_t(CChunkID("ICLN"), &iValidColorsCt, sizeof(INDEX));

  for (i = 0; i < MAX_COLOR_NAMES; i++)
  {
    if (md_ColorNames[i] != "") {
      *pFile << i;
      *pFile << md_ColorNames[i];
    }
  }

  // Save AnimData
  CAnimData::Write_t(pFile);
  IndicesToPtrs();

  *pFile << md_colDiffuse;
  *pFile << md_colReflections;
  *pFile << md_colSpecular;
  *pFile << md_colBump;
}

//------------------------------------------ READ
void CModelData::Read_t(CTStream *pFile) // throw char *
{
  INDEX i;
  _bHasAlpha = FALSE;

  pFile->ExpectID_t(CChunkID("MDAT")); // Read main ID

  // Check version number
  BOOL bHasSavedCenter = FALSE;
  BOOL bHasMultipleCollisionBoxes = FALSE;
  BOOL bHasAttachedPositions = FALSE;
  BOOL bHasPolygonalPatches = FALSE;
  BOOL bHasPolygonsPerSurface = FALSE;
  BOOL bHasSavedFlagsOnStart = FALSE;
  BOOL bHasColorForReflectionAndSpecularity = FALSE;
  BOOL bHasDiffuseColor = FALSE;

  // get version ID
  CChunkID idVersion = pFile->GetID_t();

  // if this is version without stretch center then it doesn't contain multiple collision boxes also
  if (CChunkID(MODEL_VERSION_WITHOUT_STRETCH_CENTER) == idVersion) {

  // if model has stretch center but does not have multiple collision boxes
  } else if (CChunkID(MODEL_VERSION_WITHOUT_MULTIPLE_COLLISION_BOXES) == idVersion) {
    bHasSavedCenter = TRUE;

  } else if (CChunkID(MODEL_VERSION_WITHOUT_ATTACHED_POSITIONS) == idVersion) {
    bHasSavedCenter = TRUE;
    bHasMultipleCollisionBoxes = TRUE;

  } else if (CChunkID(MODEL_VERSION_WITHOUT_POLYGONAL_PATCHES) == idVersion) {
    bHasSavedCenter = TRUE;
    bHasMultipleCollisionBoxes = TRUE;
    bHasAttachedPositions = TRUE;

  } else if (CChunkID(MODEL_VERSION_WITHOUT_POLYGONS_PER_SURFACE) == idVersion) {
    bHasSavedCenter = TRUE;
    bHasMultipleCollisionBoxes = TRUE;
    bHasAttachedPositions = TRUE;
    bHasPolygonalPatches = TRUE;

  } else if (CChunkID(MODEL_VERSION_WITHOUT_16_BIT_COMPRESSION) == idVersion) {
    bHasSavedCenter = TRUE;
    bHasMultipleCollisionBoxes = TRUE;
    bHasAttachedPositions = TRUE;
    bHasPolygonalPatches = TRUE;
    bHasPolygonsPerSurface = TRUE;

    // if has saved flags on start - because 16-bit compression
  } else if (CChunkID(MODEL_VERSION_WITHOUT_REFLECTION_AND_SPECULARITY) == idVersion) {
    bHasSavedCenter = TRUE;
    bHasMultipleCollisionBoxes = TRUE;
    bHasAttachedPositions = TRUE;
    bHasPolygonalPatches = TRUE;
    bHasPolygonsPerSurface = TRUE;
    bHasSavedFlagsOnStart = TRUE;

    // has saved color for reflection and specularity
  } else if (CChunkID(MODEL_VERSION_WITHOUT_DIFFUSE_COLOR) == idVersion) {
    bHasSavedCenter = TRUE;
    bHasMultipleCollisionBoxes = TRUE;
    bHasAttachedPositions = TRUE;
    bHasPolygonalPatches = TRUE;
    bHasPolygonsPerSurface = TRUE;
    bHasSavedFlagsOnStart = TRUE;
    bHasColorForReflectionAndSpecularity = TRUE;

    // has saved diffuse color
  } else if (CChunkID(MODEL_VERSION) == idVersion) {
    bHasSavedCenter = TRUE;
    bHasMultipleCollisionBoxes = TRUE;
    bHasAttachedPositions = TRUE;
    bHasPolygonalPatches = TRUE;
    bHasPolygonsPerSurface = TRUE;
    bHasSavedFlagsOnStart = TRUE;
    bHasColorForReflectionAndSpecularity = TRUE;
    bHasDiffuseColor = TRUE;
  } else {
    throw(TRANS("Invalid model version."));
  }

  if (bHasSavedFlagsOnStart) {
    pFile->Read_t(&md_Flags, sizeof(ULONG));
  }

  // Read vertices and frames ct
  pFile->ReadFullChunk_t(CChunkID("IVTX"), &md_VerticesCt, sizeof(INDEX));
  md_TransformedVertices.New(md_VerticesCt);
  pFile->ReadFullChunk_t(CChunkID("IFRM"), &md_FramesCt, sizeof(INDEX));

  // read array of 8-bit or 16-bit compressed vertices
  if (md_Flags & MF_COMPRESSED_16BIT)
  {
    md_FrameVertices16.New(md_VerticesCt * md_FramesCt);
    CChunkID cidVerticesChunk = pFile->PeekID_t();

    // if we are loading model in old 16-bit compressed format (normals use 1 byte)
    if (cidVerticesChunk == CChunkID("AV16"))
    {
      CChunkID cidDummy = pFile->GetID_t();
      ULONG ulDummy;

      *pFile >> ulDummy; // skip chunk size

      for (INDEX iVtx = 0; iVtx < md_VerticesCt * md_FramesCt; iVtx++)
      {
        pFile->ReadRawChunk_t(&md_FrameVertices16[iVtx], sizeof(struct ModelFrameVertex16_old));

        // convert 8-bit normal from index into normal defined using heading and pitch
        INDEX i8BitNormalIndex = md_FrameVertices16[iVtx].mfv_ubNormH;
        const FLOAT3D &vNormal = avGouraudNormals[i8BitNormalIndex];
        CompressNormal_HQ(vNormal, md_FrameVertices16[iVtx].mfv_ubNormH, md_FrameVertices16[iVtx].mfv_ubNormP);
      }

    // load new 16-bit compressed format (normals use 2 byte) model
    } else if (cidVerticesChunk == CChunkID("AV17")) {
      pFile->ReadFullChunk_t(CChunkID("AV17"), &md_FrameVertices16[0], md_VerticesCt * md_FramesCt *
        sizeof(struct ModelFrameVertex16));
    } else {
      ThrowF_t(TRANS("Expecting chunk ID for model frame vertices but found %s"), cidVerticesChunk);
    }
  }
  else {
    md_FrameVertices8.New(md_VerticesCt * md_FramesCt);
    pFile->ReadFullChunk_t(CChunkID("AFVX"), &md_FrameVertices8[0], md_VerticesCt * md_FramesCt *
      sizeof(struct ModelFrameVertex8));
  }

  // Allocate and Read frame info array
  md_FrameInfos.New(md_FramesCt);
  pFile->ReadFullChunk_t(CChunkID("AFIN"), &md_FrameInfos[0], md_FramesCt * sizeof(struct ModelFrameInfo));

  // Allocate Read frame main mip vertices array
  md_MainMipVertices.New(md_VerticesCt);
  pFile->ReadFullChunk_t(CChunkID("AMMV"), &md_MainMipVertices[0], md_VerticesCt * sizeof(FLOAT3D));

  // Allocate and Read vertex mip-mask array
  md_VertexMipMask.New(md_VerticesCt);
  pFile->ReadFullChunk_t(CChunkID("AVMK"), &md_VertexMipMask[0], md_VerticesCt * sizeof(ULONG));

  // Read mip levels counter
  pFile->ReadFullChunk_t(CChunkID("IMIP"), &md_MipCt, sizeof(INDEX));

  // Read mip factors array
  pFile->ReadFullChunk_t(CChunkID("FMIP"), &md_MipSwitchFactors[0], MAX_MODELMIPS * sizeof(float));

  // Read all model mip infos
  INDEX ctMipsRejected = 0;
  for (i = 0; i < md_MipCt; i++)
  {
    ModelMipInfo mmiDummy; // need one dummy mipmodel info in case of mip level rejection

    // reject mip model in case its even, and not last
    if (!mdl_bFineQuality && (i % 2) == 1 && i != (md_MipCt - 1))
    {
      mmiDummy.Read_t(pFile, bHasPolygonalPatches, bHasPolygonsPerSurface, bHasDiffuseColor);
      mmiDummy.Clear();
      ctMipsRejected++;
    } else {
      // Notice that model's difuse color has been saved in same model format change when surface color has been added
      md_MipInfos[i - ctMipsRejected].Read_t(pFile, bHasPolygonalPatches, bHasPolygonsPerSurface, bHasDiffuseColor);

      // readjust mip scaling factors (if)
      if (i > 0) md_MipSwitchFactors[i - ctMipsRejected - 1] = md_MipSwitchFactors[i - 1];
    }
  }

  // readjust last mip scaling factor
  md_MipSwitchFactors[i - ctMipsRejected - 1] = md_MipSwitchFactors[i - 1];

  md_MipCt -= ctMipsRejected; // reduce mip level count

  CChunkID cidPatchChunkID = pFile->PeekID_t();

  // if patches are saved in old format
  if (cidPatchChunkID == CChunkID("STMK"))
  {
    ULONG ulOldExistingPatches;
    pFile->ReadFullChunk_t(CChunkID("STMK"), &ulOldExistingPatches, sizeof(ULONG));

    for (INDEX iPatch = 0; iPatch < MAX_TEXTUREPATCHES; iPatch++)
    {
      if (((1UL << iPatch) & ulOldExistingPatches) != 0)
      {
        CTFileName fnPatchName;
        *pFile >> fnPatchName;
        try {
          md_mpPatches[iPatch].mp_toTexture.SetData_t(fnPatchName);
        } catch (char *strError) {
          (void)strError;
        }
      }
    }
  // if patches are saved in new format
  } else if (cidPatchChunkID == CChunkID("PTC2")) {
    pFile->ExpectID_t(CChunkID("PTC2"));
    for (INDEX iPatch = 0; iPatch < MAX_TEXTUREPATCHES; iPatch++)
    {
      try {
        md_mpPatches[iPatch].Read_t(pFile);
      } catch (char *strError) {
        (void)strError;
      }
    }
  } else {
    ThrowF_t(TRANS("Expecting chunk containing patch data but found unrecognisable chunk ID."));
  }

  // Read texture width and height in MEX-es
  pFile->ReadFullChunk_t(CChunkID("STXW"), &md_Width, sizeof(MEX));
  pFile->ReadFullChunk_t(CChunkID("STXH"), &md_Height, sizeof(MEX));

  // in old patch format, now patch postiions are loaded
  if (cidPatchChunkID == CChunkID("STMK"))
  {
    pFile->ExpectID_t(CChunkID("POSS"));
    ULONG ulChunkSize;
    *pFile >> ulChunkSize;

    for (INDEX iPatch = 0; iPatch < MAX_TEXTUREPATCHES; iPatch++) {
      *pFile >> md_mpPatches[iPatch].mp_mexPosition; // Read patch position
    }
  }

  if (!bHasSavedFlagsOnStart) {
    pFile->Read_t(&md_Flags, sizeof(ULONG)); // Read flags
  }

  // Read value for shading type
  pFile->Read_t(&md_ShadowQuality, sizeof(SLONG));

  // Read static stretch value
  pFile->Read_t(&md_Stretch, sizeof(FLOAT3D));

  // if this is model with saved center then read it
  if (bHasSavedCenter) {
    pFile->Read_t(&md_vCenter, sizeof(FLOAT3D));
  } else {
    // this model has been saved without center pointso just reset it
    md_vCenter = FLOAT3D(0, 0, 0);
  }

  // convert model to 8-bit if requested and needed
  if (!mdl_bFineQuality && (md_Flags&MF_COMPRESSED_16BIT))
  {
    // prepare 8-bit frame vertices array
    const INDEX ctVtx = md_VerticesCt * md_FramesCt;
    md_FrameVertices8.New(ctVtx);

    // loop thru vertices
    for (INDEX iVtx = 0; iVtx < ctVtx; iVtx++) {
      ModelFrameVertex16 &mfv16 = md_FrameVertices16[iVtx];
      ModelFrameVertex8  &mfv8 = md_FrameVertices8[iVtx];
      // convert vertex coordinate
      mfv8.mfv_SBPoint(1) = mfv16.mfv_SWPoint(1) >> 8;
      mfv8.mfv_SBPoint(2) = mfv16.mfv_SWPoint(2) >> 8;
      mfv8.mfv_SBPoint(3) = mfv16.mfv_SWPoint(3) >> 8;
      // convert normal
      const INDEX iHofs = mfv16.mfv_ubNormH >> 1;
      const INDEX iPofs = mfv16.mfv_ubNormP >> 1;
      mfv8.mfv_NormIndex = aubGouraudConv[iHofs * 128 + iPofs];
    }

    // done with conversion
    md_Stretch *= 256.0f;
    md_FrameVertices16.Clear();
    md_Flags &= ~MF_COMPRESSED_16BIT;
  }

  // create compressed vector center that will be used for setting object handle
  md_vCompressedCenter(1) = -md_vCenter(1) / md_Stretch(1);
  md_vCompressedCenter(2) = -md_vCenter(2) / md_Stretch(2);
  md_vCompressedCenter(3) = -md_vCenter(3) / md_Stretch(3);

  // if model has been saved with multiple collision boxes
  if (bHasMultipleCollisionBoxes)
  {
    INDEX ctCollisionBoxes;

    // get count of collision boxes
    pFile->Read_t(&ctCollisionBoxes, sizeof(INDEX));

    // add needed ammount of members
    md_acbCollisionBox.New(ctCollisionBoxes);
    md_acbCollisionBox.Lock();

    // for all saved collision boxes
    for (INDEX iCollisionBox = 0; iCollisionBox < ctCollisionBoxes; iCollisionBox++)
    {
      // load current collision box from stream (without name)
      md_acbCollisionBox[iCollisionBox].Read_t(pFile);

      // load name manualy
      md_acbCollisionBox[iCollisionBox].ReadName_t(pFile);
    }

    md_acbCollisionBox.Unlock();

    // else add one collision box and load it manually
  } else {
    // add one collision box
    md_acbCollisionBox.New();
    md_acbCollisionBox.Lock();

    // read one collision box manualy (without name)
    md_acbCollisionBox[0].Read_t(pFile);
    md_acbCollisionBox.Unlock();
  }

  // peek chunk ID and see if we should read boolean defining collision type (speheres or cube)
  if (pFile->PeekID_t() == CChunkID("COLI")) {
    pFile->ExpectID_t("COLI");
    *pFile >> md_bCollideAsCube;
  } else {
    md_bCollideAsCube = FALSE;
  }

  // if we should read attached positions
  if (bHasAttachedPositions)
  {
    INDEX ctAttachedPositions;
    *pFile >> ctAttachedPositions; // read count of attached positions
    md_aampAttachedPosition.New(ctAttachedPositions);

    FOREACHINDYNAMICARRAY(md_aampAttachedPosition, CAttachedModelPosition, itamp)
    {
      itamp->Read_t(pFile);

      // clamp vertices to no of model data vertices
      itamp->amp_iCenterVertex = Clamp(itamp->amp_iCenterVertex, (INDEX)0, md_MainMipVertices.Count());
      itamp->amp_iFrontVertex = Clamp(itamp->amp_iFrontVertex, (INDEX)0, md_MainMipVertices.Count());
      itamp->amp_iUpVertex = Clamp(itamp->amp_iUpVertex, (INDEX)0, md_MainMipVertices.Count());
    }
  }

  // Read color names (Read count, read existing names)
  INDEX iValidColorsCt;
  pFile->ReadFullChunk_t(CChunkID("ICLN"), &iValidColorsCt, sizeof(INDEX));

  for (i = 0; i < iValidColorsCt; i++)
  {
    INDEX iExistingColorName;
    *pFile >> iExistingColorName;
    *pFile >> md_ColorNames[iExistingColorName];
  }

  // Read AnimData
  CAnimData::Read_t(pFile);
  IndicesToPtrs();

  // old models don't have saved polygons per surface
  if (!bHasPolygonsPerSurface)
  {
    // so link them manually
    LinkDataForSurfaces(TRUE);

    // for each mip model
    for (INDEX iMip = 0; iMip < md_MipCt; iMip++)
    {
      ModelMipInfo *pMMI = &md_MipInfos[iMip];
      // for each surface
      for (INDEX iSurface = 0; iSurface<pMMI->mmpi_MappingSurfaces.Count(); iSurface++)
      {
        // convert rendering flags into new flags format (per surface)
        if (pMMI->mmpi_MappingSurfaces[iSurface].ms_aiPolygons.Count() > 0)
        {
          ULONG ulFlags = pMMI->mmpi_Polygons[pMMI->mmpi_MappingSurfaces[iSurface].ms_aiPolygons[0]].mp_RenderFlags;
          pMMI->mmpi_MappingSurfaces[iSurface].SetRenderingParameters(ulFlags);
        }
      }
    }
  }

  // turn on diffuse map for all models of old format
  if (!bHasColorForReflectionAndSpecularity)
  {
    for (INDEX iMip = 0; iMip < md_MipCt; iMip++)
    {
      ModelMipInfo *pMMI = &md_MipInfos[iMip];

      for (INDEX iSurface = 0; iSurface < pMMI->mmpi_MappingSurfaces.Count(); iSurface++) {
        pMMI->mmpi_MappingSurfaces[iSurface].ms_ulRenderingFlags |= SRF_DIFFUSE | SRF_NEW_TEXTURE_FORMAT;
      }
    }
  }

  if (bHasDiffuseColor) {
    *pFile >> md_colDiffuse;
  }

  // old models don't have saved colors for reflection and specularity
  if (bHasColorForReflectionAndSpecularity)
  {
    // load colors for reflections, specularity and bump
    *pFile >> md_colReflections;
    *pFile >> md_colSpecular;
    *pFile >> md_colBump;
  }

  md_bHasAlpha = _bHasAlpha;

  // precalculate rendering data
  extern void PrepareModelForRendering(CModelData &md);
  PrepareModelForRendering(*this);
}
