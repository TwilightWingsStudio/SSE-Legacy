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
#include "Mesh.h"

#define MESH_VERSION  12
#define MESH_ID       "MESH"

#include <Engine/Base/Stream.h>
#include <Engine/Ska/StringTable.h>
#include <Engine/Templates/Stock_CShader.h>

// write to stream
void CMesh::Write_t(CTStream *ostrFile)
{
  INDEX ctmlods = msh_aMeshLODs.Count();

  ostrFile->WriteID_t(CChunkID(MESH_ID)); // write chunk id
  (*ostrFile) << (INDEX)MESH_VERSION; // write version
  (*ostrFile) << ctmlods; // write mlod count

  // for each lod in mesh
  for (INDEX imlod = 0; imlod < ctmlods; imlod++)
  {
    MeshLOD &mLod = msh_aMeshLODs[imlod];

    INDEX ctVx = mLod.mlod_aVertices.Count();   // vertex count
    INDEX ctUV = mLod.mlod_aUVMaps.Count();     // uvmaps count
    INDEX ctSf = mLod.mlod_aSurfaces.Count();   // surfaces count
    INDEX ctWM = mLod.mlod_aWeightMaps.Count(); // weight maps count
    INDEX ctMM = mLod.mlod_aMorphMaps.Count();  // morph maps count

    (*ostrFile) << mLod.mlod_fnSourceFile; // write source file name
    (*ostrFile) << mLod.mlod_fMaxDistance; // write max distance
    (*ostrFile) << mLod.mlod_ulFlags; // write flags

    // Write vertices and normals.
    (*ostrFile) << ctVx; // write wertex count
    ostrFile->Write_t(&mLod.mlod_aVertices[0], sizeof(MeshVertex)*ctVx); // write wertices
    ostrFile->Write_t(&mLod.mlod_aNormals[0], sizeof(MeshNormal)*ctVx); // write normals

    (*ostrFile) << ctUV; // write uvmaps count

    // write uvmaps
    for (int iuv = 0; iuv < ctUV; iuv++)
    {
      CTString strNameID = ska_GetStringFromTable(mLod.mlod_aUVMaps[iuv].muv_iID);
      (*ostrFile) << strNameID; // write uvmap ID
      ostrFile->Write_t(&mLod.mlod_aUVMaps[iuv].muv_aTexCoords[0], sizeof(MeshTexCoord)*ctVx); // write uvmaps texcordinates
    }

    ostrFile->Write_t(&ctSf, sizeof(INDEX)); // write surfaces count

    // write surfaces
    for (INDEX isf = 0; isf < ctSf; isf++)
    {
      MeshSurface &msrf = mLod.mlod_aSurfaces[isf];
      INDEX ctTris = msrf.msrf_aTriangles.Count();
      CTString strSurfaceID = ska_GetStringFromTable(msrf.msrf_iSurfaceID);

      (*ostrFile) << strSurfaceID; // write surface ID
      (*ostrFile) << msrf.msrf_iFirstVertex; // write first vertex
      (*ostrFile) << msrf.msrf_ctVertices; // write vertices count
      (*ostrFile) << ctTris; // write tris count
      ostrFile->Write_t(&mLod.mlod_aSurfaces[isf].msrf_aTriangles[0], sizeof(MeshTriangle)*ctTris);  // write triangles

      INDEX bShaderExists = (msrf.msrf_pShader != NULL);
      (*ostrFile) << bShaderExists; // write bool that this surface has a shader

      // if shader exists then write its parameters
      if (bShaderExists)
      {
        // get shader decription
        ShaderDesc shDesc;
        msrf.msrf_pShader->GetShaderDesc(shDesc);
        INDEX cttx = shDesc.sd_astrTextureNames.Count();
        INDEX cttc = shDesc.sd_astrTexCoordNames.Count();
        INDEX ctcol = shDesc.sd_astrColorNames.Count();
        INDEX ctfl = shDesc.sd_astrFloatNames.Count();

        // data count must be at same as size defined in shader or higher
        ASSERT(cttx <= msrf.msrf_ShadingParams.sp_aiTextureIDs.Count());
        ASSERT(cttc <= msrf.msrf_ShadingParams.sp_aiTexCoordsIndex.Count());
        ASSERT(ctcol <= msrf.msrf_ShadingParams.sp_acolColors.Count());
        ASSERT(ctfl <= msrf.msrf_ShadingParams.sp_afFloats.Count());
        ASSERT(msrf.msrf_pShader->GetShaderDesc != NULL);

        (*ostrFile) << cttx; // write texture count 
        (*ostrFile) << cttc; // write texture coords count 
        (*ostrFile) << ctcol; // write color count 
        (*ostrFile) << ctfl; // write float count 

        ASSERT(msrf.msrf_pShader != NULL);

        // get and write shader name
        CTString strShaderName;
        strShaderName = msrf.msrf_pShader->GetName();
        (*ostrFile) << strShaderName;

        // write shader texture IDs
        for (INDEX itx = 0; itx < cttx; itx++)
        {
          INDEX iTexID = msrf.msrf_ShadingParams.sp_aiTextureIDs[itx];
          (*ostrFile) << ska_GetStringFromTable(iTexID);
        }

        // write shader texture coords indices
        for (INDEX itc = 0; itc < cttc; itc++)
        {
          INDEX iTexCoorsIndex = msrf.msrf_ShadingParams.sp_aiTexCoordsIndex[itc];
          (*ostrFile) << iTexCoorsIndex;
        }

        // write shader colors
        for (INDEX icol = 0; icol < ctcol; icol++)
        {
          COLOR colColor = msrf.msrf_ShadingParams.sp_acolColors[icol];
          (*ostrFile) << colColor;
        }

        // write shader floats
        for (INDEX ifl = 0; ifl < ctfl; ifl++)
        {
          FLOAT fFloat = msrf.msrf_ShadingParams.sp_afFloats[ifl];
          (*ostrFile) << fFloat;
        }

        // write shader flags
        ULONG ulFlags = msrf.msrf_ShadingParams.sp_ulFlags;
        (*ostrFile) << ulFlags;
      }
    }

    (*ostrFile) << ctWM; // write weightmaps count

    // for each weightmap in array
    for (INDEX iwm = 0; iwm < ctWM; iwm++)
    {
      INDEX ctWw = mLod.mlod_aWeightMaps[iwm].mwm_aVertexWeight.Count();
      CTString pstrNameID = ska_GetStringFromTable(mLod.mlod_aWeightMaps[iwm].mwm_iID);

      (*ostrFile) << pstrNameID; // write wertex weight map ID
      (*ostrFile) << ctWw; // write wertex weights count

      // write wertex weights
      ostrFile->Write_t(&mLod.mlod_aWeightMaps[iwm].mwm_aVertexWeight[0], sizeof(MeshVertexWeight)*ctWw);
    }

    // write morphmaps count
    (*ostrFile) << ctMM;

    for (INDEX imm = 0; imm < ctMM; imm++)
    {
      INDEX ctms = mLod.mlod_aMorphMaps[imm].mmp_aMorphMap.Count();
      CTString pstrNameID = ska_GetStringFromTable(mLod.mlod_aMorphMaps[imm].mmp_iID);

      (*ostrFile) << pstrNameID; // write ID
      (*ostrFile) << mLod.mlod_aMorphMaps[imm].mmp_bRelative; // write bRelative
      //ostrFile->Write_t(&mLod.mlod_aMorphMaps[imm].mmp_bRelative,sizeof(BOOL)); // Idk why CT removed it.
      ostrFile->Write_t(&ctms, sizeof(INDEX)); // write morph sets count

      // write morph sets
      ostrFile->Write_t(&mLod.mlod_aMorphMaps[imm].mmp_aMorphMap[0], sizeof(MeshVertexMorph)*ctms);
    }
  }
}

//read from stream
void CMesh::Read_t(CTStream *istrFile)
{
  INDEX ctmlods;
  INDEX iFileVersion;

  istrFile->ExpectID_t(CChunkID(MESH_ID)); // read chunk id
  (*istrFile) >> iFileVersion; // check file version

  // if file version is not 11 nor 12
  if (iFileVersion != 11 && iFileVersion != 12) {
    ThrowF_t(TRANS("File '%s'.\nInvalid Mesh file version.\nExpected Ver \"%d\" but found \"%d\"\n"),
      (const char*)istrFile->GetDescription(), MESH_VERSION, iFileVersion);
    return;
  }

  (*istrFile) >> ctmlods; // read mlod count

  // for each lod in mesh
  for (INDEX imlod = 0; imlod < ctmlods; imlod++)
  {
    INDEX ctMeshLODs = msh_aMeshLODs.Count();
    msh_aMeshLODs.Expand(ctMeshLODs + 1); // expand mlod count for one 
    MeshLOD &mLod = msh_aMeshLODs[ctMeshLODs];

    INDEX ctVx;   // vertex count
    INDEX ctUV;   // uvmaps count
    INDEX ctSf;   // surfaces count
    INDEX ctWM;   // weight maps count
    INDEX ctMM;   // morph maps count

    (*istrFile) >> mLod.mlod_fnSourceFile; // read source file name
    (*istrFile) >> mLod.mlod_fMaxDistance; // read max distance
    (*istrFile) >> mLod.mlod_ulFlags; // read flags

    // :)
    if (iFileVersion <= 11) {
      mLod.mlod_ulFlags = 0;
    }

    if (mLod.mlod_ulFlags == 0xCDCDCDCD) {
      mLod.mlod_ulFlags = 0;
    }

    (*istrFile) >> ctVx; // read vertex count

    // create vertex and normal arrays
    mLod.mlod_aVertices.New(ctVx);
    mLod.mlod_aNormals.New(ctVx);

    istrFile->Read_t(&mLod.mlod_aVertices[0], sizeof(MeshVertex)*ctVx); // read wertices
    istrFile->Read_t(&mLod.mlod_aNormals[0], sizeof(MeshNormal)*ctVx); // read normals

    (*istrFile) >> ctUV; // read uvmaps count
    mLod.mlod_aUVMaps.New(ctUV); // create array for uvmaps

    // read uvmaps
    for (int iuv = 0; iuv < ctUV; iuv++)
    {
      CTString strNameID;

      (*istrFile) >> strNameID; // read uvmap ID
      mLod.mlod_aUVMaps[iuv].muv_iID = ska_GetIDFromStringTable(strNameID);

      mLod.mlod_aUVMaps[iuv].muv_aTexCoords.New(ctVx); // create array for uvmaps texcordinates
      istrFile->Read_t(&mLod.mlod_aUVMaps[iuv].muv_aTexCoords[0], sizeof(MeshTexCoord)*ctVx); // read uvmap texcordinates
    }

    (*istrFile) >> ctSf; // read surfaces count
    mLod.mlod_aSurfaces.New(ctSf); // create array for surfaces

    // read surfaces
    for (INDEX isf = 0; isf < ctSf; isf++)
    {
      INDEX ctTris;
      MeshSurface &msrf = mLod.mlod_aSurfaces[isf];
      CTString strSurfaceID;
      INDEX bShaderExists;

      (*istrFile) >> strSurfaceID; // read surface ID
      msrf.msrf_iSurfaceID = ska_GetIDFromStringTable(strSurfaceID);
      (*istrFile) >> msrf.msrf_iFirstVertex; // read first vertex
      (*istrFile) >> msrf.msrf_ctVertices; // read vertices count

      (*istrFile) >> ctTris; // read tris count
      mLod.mlod_aSurfaces[isf].msrf_aTriangles.New(ctTris); // create triangles array
      istrFile->Read_t(&mLod.mlod_aSurfaces[isf].msrf_aTriangles[0], sizeof(MeshTriangle)*ctTris); // read triangles

      (*istrFile) >> bShaderExists; // read bool that this surface has a shader

      // if shader exists read its params
      if (bShaderExists)
      {
        INDEX cttx, cttc, ctcol, ctfl;

        (*istrFile) >> cttx; // read texture count
        (*istrFile) >> cttc; // read texture coords count
        (*istrFile) >> ctcol; // read color count
        (*istrFile) >> ctfl; // read float count

        CShader *pshMeshShader = NULL;
        ShaderParams *pshpShaderParams = NULL;
        CShader shDummyShader;            // dummy shader if shader is not found
        ShaderParams shpDummyShaderParams;// dummy shader params if shader is not found
        CTString strShaderName;

        (*istrFile) >> strShaderName; // read shader name

        // try to load shader
        try {
          msrf.msrf_pShader = _pShaderStock->Obtain_t(strShaderName);
          pshMeshShader = msrf.msrf_pShader;
          pshpShaderParams = &msrf.msrf_ShadingParams;
        }
        catch (char *strErr) {
          CPrintF("%s\n", strErr);
          msrf.msrf_pShader = NULL;
          pshMeshShader = &shDummyShader;
          pshpShaderParams = &shpDummyShaderParams;
        }

        // if mesh shader exisits
        if (msrf.msrf_pShader != NULL)
        {
          // get shader description
          ShaderDesc shDesc;
          msrf.msrf_pShader->GetShaderDesc(shDesc);

          // check if saved params count match shader params count
          if (shDesc.sd_astrTextureNames.Count() != cttx) ThrowF_t("File '%s'\nWrong texture count %d", (const char*)GetName(), cttx);
          if (shDesc.sd_astrTexCoordNames.Count() != cttc) ThrowF_t("File '%s'\nWrong uvmaps count %d", (const char*)GetName(), cttc);
          if (shDesc.sd_astrColorNames.Count() != ctcol) ThrowF_t("File '%s'\nWrong colors count %d", (const char*)GetName(), ctcol);
          if (shDesc.sd_astrFloatNames.Count() != ctfl) ThrowF_t("File '%s'\nWrong floats count %d", (const char*)GetName(), ctfl);
        }

        // create arrays for shader params
        pshpShaderParams->sp_aiTextureIDs.New(cttx);
        pshpShaderParams->sp_aiTexCoordsIndex.New(cttc);
        pshpShaderParams->sp_acolColors.New(ctcol);
        pshpShaderParams->sp_afFloats.New(ctfl);

        // read shader texture IDs
        for (INDEX itx = 0; itx < cttx; itx++) {
          CTString strTexID;
          (*istrFile) >> strTexID;
          INDEX iTexID = ska_GetIDFromStringTable(strTexID);
          pshpShaderParams->sp_aiTextureIDs[itx] = iTexID;
        }

        // read shader texture coords indices
        for (INDEX itc = 0; itc < cttc; itc++) {
          INDEX iTexCoorsIndex;
          (*istrFile) >> iTexCoorsIndex;
          pshpShaderParams->sp_aiTexCoordsIndex[itc] = iTexCoorsIndex;
        }

        // read shader colors
        for (INDEX icol = 0; icol < ctcol; icol++) {
          COLOR colColor;
          (*istrFile) >> colColor;
          pshpShaderParams->sp_acolColors[icol] = colColor;
        }

        // read shader floats
        for (INDEX ifl = 0; ifl<ctfl; ifl++) {
          FLOAT fFloat;
          (*istrFile) >> fFloat;
          pshpShaderParams->sp_afFloats[ifl] = fFloat;
        }

        // there were no flags in shader before ver 12
        if (iFileVersion > 11) {
          ULONG ulFlags;
          (*istrFile) >> ulFlags;
          pshpShaderParams->sp_ulFlags = ulFlags;
        }
        else {
          pshpShaderParams->sp_ulFlags = 0;
        }
      }
      else {
        // this surface does not have shader
        msrf.msrf_pShader = NULL;
      }
    }

    (*istrFile) >> ctWM; // read weightmaps count
    mLod.mlod_aWeightMaps.New(ctWM); // create weightmap array

    // read each weightmap
    for (INDEX iwm = 0; iwm < ctWM; iwm++)
    {
      CTString pstrNameID;
      INDEX ctWw;

      (*istrFile) >> pstrNameID; // read weightmap ID
      mLod.mlod_aWeightMaps[iwm].mwm_iID = ska_GetIDFromStringTable(pstrNameID);

      (*istrFile) >> ctWw; // read wertex weight count
      mLod.mlod_aWeightMaps[iwm].mwm_aVertexWeight.New(ctWw); // create wertex weight array
      istrFile->Read_t(&mLod.mlod_aWeightMaps[iwm].mwm_aVertexWeight[0], sizeof(MeshVertexWeight)*ctWw); // read wertex weights
    }


    (*istrFile) >> ctMM; // read morphmap count
    mLod.mlod_aMorphMaps.New(ctMM); // create morphmaps array

    // read morphmaps
    for (INDEX imm = 0; imm < ctMM; imm++)
    {
      CTString pstrNameID;
      INDEX ctms;

      (*istrFile) >> pstrNameID; // read morphmap ID
      mLod.mlod_aMorphMaps[imm].mmp_iID = ska_GetIDFromStringTable(pstrNameID);
      (*istrFile) >> mLod.mlod_aMorphMaps[imm].mmp_bRelative; // read bRelative

      (*istrFile) >> ctms; // read morph sets count
      mLod.mlod_aMorphMaps[imm].mmp_aMorphMap.New(ctms); // create morps sets array 
      istrFile->Read_t(&mLod.mlod_aMorphMaps[imm].mmp_aMorphMap[0], sizeof(MeshVertexMorph)*ctms); // read morph sets
    }
  }
}