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

#include <Engine/Base/Stream.h>
#include <Engine/Base/Console.h>
#include <Engine/Math/Projection.h>
#include <Engine/Graphics/DrawPort.h>
#include <Engine/Templates/StaticArray.h>
#include <Engine/Templates/StaticArray.cpp>
#include <Engine/Templates/Stock_CShader.h>

INDEX AreVerticesDiferent(INDEX iCurentIndex, INDEX iLastIndex);

struct VertexLocator
{
  INDEX vl_iIndex;
  INDEX vl_iSubIndex;
};

struct SortArray
{
  INDEX sa_iNewIndex;
  INDEX sa_iSurfaceIndex;
  CStaticArray<struct VertexLocator> sa_aWeightMapList;
  CStaticArray<struct VertexLocator> sa_aMorphMapList;
};

CStaticArray <struct SortArray> _aSortArray;
CStaticArray <INDEX> _aiOptimizedIndex;
CStaticArray <INDEX> _aiSortedIndex;

MeshLOD *pMeshLOD;// curent mesh lod (for quick sort)
MeshLOD mshOptimized;

CMesh::CMesh()
{
}

CMesh::~CMesh()
{
}

// release old shader and obtain new shader for mesh surface (expand ShaderParams if needed)
void ChangeSurfaceShader_t(MeshSurface &msrf,CTString fnNewShader)
{
  CShader *pShaderNew = _pShaderStock->Obtain_t(fnNewShader);
  ASSERT(pShaderNew!=NULL);
  if(msrf.msrf_pShader!=NULL) _pShaderStock->Release(msrf.msrf_pShader);
  msrf.msrf_pShader = pShaderNew;
  // get new shader description
  ShaderDesc shDesc;
  msrf.msrf_pShader->GetShaderDesc(shDesc);
  // if needed expand size of arrays for new shader
  // reset new values!!!!
  INDEX ctOldTextureIDs = msrf.msrf_ShadingParams.sp_aiTextureIDs.Count();
  INDEX ctNewTextureIDs = shDesc.sd_astrTextureNames.Count();
  INDEX ctOldUVMaps     = msrf.msrf_ShadingParams.sp_aiTexCoordsIndex.Count();
  INDEX ctNewUVMaps     = shDesc.sd_astrTexCoordNames.Count();
  INDEX ctOldColors     = msrf.msrf_ShadingParams.sp_acolColors.Count();
  INDEX ctNewColors     = shDesc.sd_astrColorNames.Count();
  INDEX ctOldFloats     = msrf.msrf_ShadingParams.sp_afFloats.Count();
  INDEX ctNewFloats     = shDesc.sd_astrFloatNames.Count();
  if(ctOldTextureIDs<ctNewTextureIDs) {
    // expand texture IDs array
    msrf.msrf_ShadingParams.sp_aiTextureIDs.Expand(ctNewTextureIDs);
    // set new texture IDs to 0
    for(INDEX itx=ctOldTextureIDs;itx<ctNewTextureIDs;itx++) {
      msrf.msrf_ShadingParams.sp_aiTextureIDs[itx] = -1;
    }
  }
  // expand array of uvmaps if needed
  if(ctOldUVMaps<ctNewUVMaps) {
    // expand uvmaps IDs array
    msrf.msrf_ShadingParams.sp_aiTexCoordsIndex.Expand(ctNewUVMaps);
    // set new uvmaps indices to 0
    for(INDEX itxc=ctOldUVMaps;itxc<ctNewUVMaps;itxc++) {
      msrf.msrf_ShadingParams.sp_aiTexCoordsIndex[itxc] = 0;
    }
  }
  // expand array of colors if needed
  if(ctOldColors<ctNewColors) {
    // expand color array
    msrf.msrf_ShadingParams.sp_acolColors.Expand(ctNewColors);
    // set new colors indices white
    for(INDEX icol=ctOldUVMaps;icol<ctNewColors;icol++) {
      msrf.msrf_ShadingParams.sp_acolColors[icol] = 0xFFFFFFFF;
    }
  }
  // expand array of floats if needed
  if(ctOldFloats<ctNewFloats) {
    // expand float array
    msrf.msrf_ShadingParams.sp_afFloats.Expand(ctNewFloats);
    // set new floats to 0
    for(INDEX ifl=ctOldFloats;ifl<ctNewFloats;ifl++) {
      msrf.msrf_ShadingParams.sp_afFloats[ifl] = 1;
    }
  }
}

// quck sort func for comparing vertices
static int qsort_CompareArray(const void *pVx1, const void *pVx2)
{
  INDEX *n1 = ((INDEX*)pVx1);
  INDEX *n2 = ((INDEX*)pVx2);
  return AreVerticesDiferent(*n1,*n2);
}
// clear array of sort vertices
void ClearSortArray(INDEX ctOldVertices)
{
  for(int iv=0;iv<ctOldVertices;iv++)
  {
    _aSortArray[iv].sa_aWeightMapList.Clear();
    _aSortArray[iv].sa_aMorphMapList.Clear();
  }
  _aiOptimizedIndex.Clear();
  _aiSortedIndex.Clear();
  _aSortArray.Clear();
}
// optimize mesh
void CMesh::Optimize(void)
{
  INDEX ctmshlods = msh_aMeshLODs.Count();
  for(int imshlod=0;imshlod<ctmshlods;imshlod++)
  {
    // optimize each lod in mesh
    OptimizeLod(msh_aMeshLODs[imshlod]);
  }
}
// optimize lod of mesh
void CMesh::OptimizeLod(MeshLOD &mLod)
{
  INDEX ctVertices  = mLod.mlod_aVertices.Count();
  INDEX ctSurfaces  = mLod.mlod_aSurfaces.Count();
  INDEX ctUVMaps    = mLod.mlod_aUVMaps.Count();
  INDEX ctWeightMaps = mLod.mlod_aWeightMaps.Count();
  INDEX ctMorphMaps = mLod.mlod_aMorphMaps.Count();

  if(ctVertices<=0) return;
  
  // create array for sorting
  _aSortArray.New(ctVertices);
  _aiSortedIndex.New(ctVertices);
  _aiOptimizedIndex.New(ctVertices);
  // put original vertex indices in SortArray
  for(int iv=0;iv<ctVertices;iv++)
  {
    _aiSortedIndex[iv] = iv;
  }
  // loop each surface and expand SurfaceList in SortArray
  int is=0;
  for(;is<ctSurfaces;is++)
  {
    INDEX ctts=mLod.mlod_aSurfaces[is].msrf_aTriangles.Count();
    for(int its=0;its<ctts;its++)
    {
      MeshTriangle &mtTriangle = mLod.mlod_aSurfaces[is].msrf_aTriangles[its];

      _aSortArray[mtTriangle.iVertex[0]].sa_iSurfaceIndex = is;
      _aSortArray[mtTriangle.iVertex[1]].sa_iSurfaceIndex = is;
      _aSortArray[mtTriangle.iVertex[2]].sa_iSurfaceIndex = is;
    }
  }
  // loop each weightmap and expand sa_aWeightMapList in SortArray
  for(INDEX iw=0;iw<ctWeightMaps;iw++)
  {
    // loop each wertex weight array in weight map array
    INDEX ctwm = mLod.mlod_aWeightMaps[iw].mwm_aVertexWeight.Count();
    for(INDEX iwm=0;iwm<ctwm;iwm++)
    {
      MeshVertexWeight &mwwWeight = mLod.mlod_aWeightMaps[iw].mwm_aVertexWeight[iwm];
      // get curent list num of weightmaps  
      INDEX ctWeightMapList = _aSortArray[mwwWeight.mww_iVertex].sa_aWeightMapList.Count();
      // expand array of sufrace lists for 1
      _aSortArray[mwwWeight.mww_iVertex].sa_aWeightMapList.Expand(ctWeightMapList+1);
      // set vl_iIndex to index of surface
      // set vl_iSubIndex to index in triangle set
      VertexLocator &vxLoc = _aSortArray[mwwWeight.mww_iVertex].sa_aWeightMapList[ctWeightMapList];
      vxLoc.vl_iIndex = iw;
      vxLoc.vl_iSubIndex = iwm;
    }
  }
  // loop each morphmap and expand sa_aMorphMapList in SortArray
  for(INDEX im=0;im<ctMorphMaps;im++)
  {
    // loop each morph map in array
    INDEX ctmm = mLod.mlod_aMorphMaps[im].mmp_aMorphMap.Count();
    for(INDEX imm=0;imm<ctmm;imm++)
    {
      MeshVertexMorph &mwmMorph = mLod.mlod_aMorphMaps[im].mmp_aMorphMap[imm];
      // get curent list num of morphmaps  
      INDEX ctMorphMapList = _aSortArray[mwmMorph.mwm_iVxIndex].sa_aMorphMapList.Count();
      // expand array of sufrace lists for 1
      _aSortArray[mwmMorph.mwm_iVxIndex].sa_aMorphMapList.Expand(ctMorphMapList+1);
      // set vl_iIndex to index of surface
      // set vl_iSubIndex to index in triangle set
      VertexLocator &vxLoc = _aSortArray[mwmMorph.mwm_iVxIndex].sa_aMorphMapList[ctMorphMapList];
      vxLoc.vl_iIndex = im;
      vxLoc.vl_iSubIndex = imm;
    }
  }
  // set global pMeshLOD pointer used by quicksort
  pMeshLOD = &mLod;
  // sort array
  qsort(&_aiSortedIndex[0],ctVertices,sizeof(&_aiSortedIndex[0]),qsort_CompareArray);
  
  // compare vertices
  INDEX iDiferentVertices = 1;
  INDEX iLastIndex = _aiSortedIndex[0];
  _aSortArray[iLastIndex].sa_iNewIndex = 0;
  _aiOptimizedIndex[0] = iLastIndex;
  
  for(INDEX isa=1;isa<ctVertices;isa++)
  {
    INDEX iCurentIndex = _aiSortedIndex[isa];
    // check if vertices are diferent
    if(AreVerticesDiferent(iLastIndex,iCurentIndex))
    {
      // add Curent index to Optimized index array
      _aiOptimizedIndex[iDiferentVertices] = iCurentIndex;
      iDiferentVertices++;
      iLastIndex = iCurentIndex;
    }
    _aSortArray[iCurentIndex].sa_iNewIndex = iDiferentVertices-1;
  }

  // create new mesh
  INDEX ctNewVertices = iDiferentVertices;
  mshOptimized.mlod_aVertices.New(ctNewVertices);
  mshOptimized.mlod_aNormals.New(ctNewVertices);
  mshOptimized.mlod_aUVMaps.New(ctUVMaps);
  for(INDEX iuvm=0;iuvm<ctUVMaps;iuvm++)
  {
    mshOptimized.mlod_aUVMaps[iuvm].muv_aTexCoords.New(ctNewVertices);
  }

  // add new vertices and normals to mshOptimized
  for(INDEX iNewVx=0;iNewVx<ctNewVertices;iNewVx++)
  {
    mshOptimized.mlod_aVertices[iNewVx] = mLod.mlod_aVertices[_aiOptimizedIndex[iNewVx]];
    mshOptimized.mlod_aNormals[iNewVx] = mLod.mlod_aNormals[_aiOptimizedIndex[iNewVx]];
    for(INDEX iuvm=0;iuvm<ctUVMaps;iuvm++)
    {
      //???
      mshOptimized.mlod_aUVMaps[iuvm].muv_iID = mLod.mlod_aUVMaps[iuvm].muv_iID;
      mshOptimized.mlod_aUVMaps[iuvm].muv_aTexCoords[iNewVx] = mLod.mlod_aUVMaps[iuvm].muv_aTexCoords[_aiOptimizedIndex[iNewVx]];
    }
  }
  // remap surface triangles
  for(is=0;is<ctSurfaces;is++)
  {
    MeshSurface &msrf = mLod.mlod_aSurfaces[is];
    INDEX iMinIndex = ctNewVertices+1;
    INDEX iMaxIndex = -1;
    INDEX ctts=msrf.msrf_aTriangles.Count();
    // for each triangle in this surface
    INDEX its=0;
    for(;its<ctts;its++)
    {
      MeshTriangle &mtTriangle = msrf.msrf_aTriangles[its];
      // for each vertex in triangle
      for(INDEX iv=0;iv<3;iv++)
      {
        mtTriangle.iVertex[iv] = _aSortArray[mtTriangle.iVertex[iv]].sa_iNewIndex;
        // find first index in this surface
        if(mtTriangle.iVertex[iv]<iMinIndex) iMinIndex = mtTriangle.iVertex[iv];
        // find last index in this surface
        if(mtTriangle.iVertex[iv]>iMaxIndex) iMaxIndex = mtTriangle.iVertex[iv];
      }
    }
    // remember first index in vertices array
    msrf.msrf_iFirstVertex = iMinIndex;
    // remember vertices count
    msrf.msrf_ctVertices = iMaxIndex-iMinIndex+1;

    // for each triangle in surface
    for(its=0;its<ctts;its++)
    {
      MeshTriangle &mtTriangle = msrf.msrf_aTriangles[its];
      // for each vertex in triangle
      for(INDEX iv=0;iv<3;iv++)
      {
        // substract vertex index in triangle with first vertex in surface
        mtTriangle.iVertex[iv] -= msrf.msrf_iFirstVertex;
        ASSERT(mtTriangle.iVertex[iv]<msrf.msrf_ctVertices);
      }
    }
  }

  // remap weightmaps
  mshOptimized.mlod_aWeightMaps.New(ctWeightMaps);
  // expand wertex veights array for each vertex
  INDEX ivx=0;
  for(;ivx<ctNewVertices;ivx++)
  {
    INDEX ioptVx = _aiOptimizedIndex[ivx];
    for(INDEX iwl=0;iwl<_aSortArray[ioptVx].sa_aWeightMapList.Count();iwl++)
    {
      VertexLocator &wml = _aSortArray[ioptVx].sa_aWeightMapList[iwl];
      INDEX wmIndex = wml.vl_iIndex;
      INDEX wwIndex = wml.vl_iSubIndex;
      INDEX ctww = mshOptimized.mlod_aWeightMaps[wmIndex].mwm_aVertexWeight.Count();
      MeshWeightMap &mwm = mshOptimized.mlod_aWeightMaps[wmIndex];
      MeshVertexWeight &mww = mLod.mlod_aWeightMaps[wmIndex].mwm_aVertexWeight[wwIndex];

      mwm.mwm_iID = mLod.mlod_aWeightMaps[wmIndex].mwm_iID;
      mwm.mwm_aVertexWeight.Expand(ctww+1);
      mwm.mwm_aVertexWeight[ctww].mww_fWeight = mww.mww_fWeight;
      mwm.mwm_aVertexWeight[ctww].mww_iVertex = ivx;
    }
  }

  // remap morphmaps
  mshOptimized.mlod_aMorphMaps.New(ctMorphMaps);
  // expand morph maps array for each vertex
  for(ivx=0;ivx<ctNewVertices;ivx++)
  {
    INDEX ioptVx = _aiOptimizedIndex[ivx];
    for(INDEX iml=0;iml<_aSortArray[ioptVx].sa_aMorphMapList.Count();iml++)
    {
      VertexLocator &mml = _aSortArray[ioptVx].sa_aMorphMapList[iml];
      INDEX mmIndex = mml.vl_iIndex;
      INDEX mwmIndex = mml.vl_iSubIndex;
      INDEX ctmwm = mshOptimized.mlod_aMorphMaps[mmIndex].mmp_aMorphMap.Count();
      MeshMorphMap &mmm = mshOptimized.mlod_aMorphMaps[mmIndex];
      MeshVertexMorph &mwm = mLod.mlod_aMorphMaps[mmIndex].mmp_aMorphMap[mwmIndex];

      mmm.mmp_iID = mLod.mlod_aMorphMaps[mmIndex].mmp_iID;
      mmm.mmp_bRelative = mLod.mlod_aMorphMaps[mmIndex].mmp_bRelative;

      mmm.mmp_aMorphMap.Expand(ctmwm+1);
      mmm.mmp_aMorphMap[ctmwm].mwm_iVxIndex = ivx;
      mmm.mmp_aMorphMap[ctmwm].mwm_x = mwm.mwm_x;
      mmm.mmp_aMorphMap[ctmwm].mwm_y = mwm.mwm_y;
      mmm.mmp_aMorphMap[ctmwm].mwm_z = mwm.mwm_z;
      mmm.mmp_aMorphMap[ctmwm].mwm_nx = mwm.mwm_nx;
      mmm.mmp_aMorphMap[ctmwm].mwm_ny = mwm.mwm_ny;
      mmm.mmp_aMorphMap[ctmwm].mwm_nz = mwm.mwm_nz;
    }
  }

  mLod.mlod_aVertices.CopyArray(mshOptimized.mlod_aVertices);
  mLod.mlod_aNormals.CopyArray(mshOptimized.mlod_aNormals);
  mLod.mlod_aMorphMaps.CopyArray(mshOptimized.mlod_aMorphMaps);
  mLod.mlod_aWeightMaps.CopyArray(mshOptimized.mlod_aWeightMaps);
  mLod.mlod_aUVMaps.CopyArray(mshOptimized.mlod_aUVMaps);

  // clear memory
  ClearSortArray(ctVertices);
  mshOptimized.mlod_aVertices.Clear();
  mshOptimized.mlod_aNormals.Clear();
  mshOptimized.mlod_aWeightMaps.Clear();
  mshOptimized.mlod_aMorphMaps.Clear();
  mshOptimized.mlod_aUVMaps.Clear();
}

INDEX AreVerticesDiferent(INDEX iCurentIndex, INDEX iLastIndex)
{
#define CHECK(x,y) if(((x)-(y))!=0) return ((x)-(y))
#define CHECKF(x,y) if(((x)-(y))!=0) return Sgn((x)-(y))
  
  // check surfaces
  CHECK(_aSortArray[iCurentIndex].sa_iSurfaceIndex,_aSortArray[iLastIndex].sa_iSurfaceIndex);
  // check vertices
  CHECKF(pMeshLOD->mlod_aVertices[iCurentIndex].y,pMeshLOD->mlod_aVertices[iLastIndex].y);
  CHECKF(pMeshLOD->mlod_aVertices[iCurentIndex].x,pMeshLOD->mlod_aVertices[iLastIndex].x);
  CHECKF(pMeshLOD->mlod_aVertices[iCurentIndex].z,pMeshLOD->mlod_aVertices[iLastIndex].z);
  // check normals
  CHECKF(pMeshLOD->mlod_aNormals[iCurentIndex].ny,pMeshLOD->mlod_aNormals[iLastIndex].ny);
  CHECKF(pMeshLOD->mlod_aNormals[iCurentIndex].nx,pMeshLOD->mlod_aNormals[iLastIndex].nx);
  CHECKF(pMeshLOD->mlod_aNormals[iCurentIndex].nz,pMeshLOD->mlod_aNormals[iLastIndex].nz);
  // check uvmaps
  INDEX ctUVMaps = pMeshLOD->mlod_aUVMaps.Count();
  for(INDEX iuvm=0;iuvm<ctUVMaps;iuvm++)
  {
    CHECKF(pMeshLOD->mlod_aUVMaps[iuvm].muv_aTexCoords[iCurentIndex].u,pMeshLOD->mlod_aUVMaps[iuvm].muv_aTexCoords[iLastIndex].u);
    CHECKF(pMeshLOD->mlod_aUVMaps[iuvm].muv_aTexCoords[iCurentIndex].v,pMeshLOD->mlod_aUVMaps[iuvm].muv_aTexCoords[iLastIndex].v);
  }
  // count weight and morph maps
  INDEX ctwmCurent  = _aSortArray[iCurentIndex].sa_aWeightMapList.Count();
  INDEX ctwmLast    = _aSortArray[iLastIndex].sa_aWeightMapList.Count();
  INDEX ctmmCurent  = _aSortArray[iCurentIndex].sa_aMorphMapList.Count();
  INDEX ctmmLast    = _aSortArray[iLastIndex].sa_aMorphMapList.Count();
  // check if vertices have same weight and morph maps count
  CHECK(ctwmCurent,ctwmLast);
  CHECK(ctmmCurent,ctmmLast);
  // check if vertices have same weight map factors
  for(INDEX iwm=0;iwm<ctwmCurent;iwm++)
  {
    // get weight map indices
    INDEX iwmCurent = _aSortArray[iCurentIndex].sa_aWeightMapList[iwm].vl_iIndex;
    INDEX iwmLast   = _aSortArray[iLastIndex].sa_aWeightMapList[iwm].vl_iIndex;
    // get wertex weight indices
    INDEX iwwCurent = _aSortArray[iCurentIndex].sa_aWeightMapList[iwm].vl_iSubIndex;
    INDEX iwwLast   = _aSortArray[iLastIndex].sa_aWeightMapList[iwm].vl_iSubIndex;
    // if weight map factors are diferent
    CHECKF(pMeshLOD->mlod_aWeightMaps[iwmCurent].mwm_aVertexWeight[iwwCurent].mww_fWeight,pMeshLOD->mlod_aWeightMaps[iwmLast].mwm_aVertexWeight[iwwLast].mww_fWeight);
  }

  // check if vertices have same morph map factors
  for(INDEX imm=0;imm<ctmmCurent;imm++)
  {
    // get morph map indices
    INDEX immCurent = _aSortArray[iCurentIndex].sa_aMorphMapList[imm].vl_iIndex;
    INDEX immLast   = _aSortArray[iLastIndex].sa_aMorphMapList[imm].vl_iIndex;
    // get mesh vertex morph indices
    INDEX imwmCurent  = _aSortArray[iCurentIndex].sa_aMorphMapList[imm].vl_iSubIndex;
    INDEX imwmLast    = _aSortArray[iLastIndex].sa_aMorphMapList[imm].vl_iSubIndex;
    
    // if mesh morph map params are diferent return
    CHECKF(pMeshLOD->mlod_aMorphMaps[immCurent].mmp_aMorphMap[imwmCurent].mwm_x,
      pMeshLOD->mlod_aMorphMaps[immLast].mmp_aMorphMap[imwmLast].mwm_x);
    CHECKF(pMeshLOD->mlod_aMorphMaps[immCurent].mmp_aMorphMap[imwmCurent].mwm_y,
      pMeshLOD->mlod_aMorphMaps[immLast].mmp_aMorphMap[imwmLast].mwm_y);
    CHECKF(pMeshLOD->mlod_aMorphMaps[immCurent].mmp_aMorphMap[imwmCurent].mwm_z,
      pMeshLOD->mlod_aMorphMaps[immLast].mmp_aMorphMap[imwmLast].mwm_z);
    CHECKF(pMeshLOD->mlod_aMorphMaps[immCurent].mmp_aMorphMap[imwmCurent].mwm_nx,
      pMeshLOD->mlod_aMorphMaps[immLast].mmp_aMorphMap[imwmLast].mwm_nx);
    CHECKF(pMeshLOD->mlod_aMorphMaps[immCurent].mmp_aMorphMap[imwmCurent].mwm_ny,
      pMeshLOD->mlod_aMorphMaps[immLast].mmp_aMorphMap[imwmLast].mwm_ny);
    CHECKF(pMeshLOD->mlod_aMorphMaps[immCurent].mmp_aMorphMap[imwmCurent].mwm_nz,
      pMeshLOD->mlod_aMorphMaps[immLast].mmp_aMorphMap[imwmLast].mwm_nz);
  }
  return 0;
}
// normalize weights in mlod
void CMesh::NormalizeWeightsInLod(MeshLOD &mlod)
{
  CStaticArray<float> aWeightFactors;
  int ctvtx = mlod.mlod_aVertices.Count();
  int ctwm = mlod.mlod_aWeightMaps.Count();
  // create array for weights
  aWeightFactors.New(ctvtx);
  memset(&aWeightFactors[0],0,sizeof(aWeightFactors[0])*ctvtx);
  int iwm=0;
  for(;iwm<ctwm;iwm++)
  {
    MeshWeightMap &mwm = mlod.mlod_aWeightMaps[iwm];
    for(int iww=0;iww<mwm.mwm_aVertexWeight.Count();iww++)
    {
      MeshVertexWeight &mwh = mwm.mwm_aVertexWeight[iww];
       aWeightFactors[mwh.mww_iVertex] += mwh.mww_fWeight;
    }
  }

  for(iwm=0;iwm<ctwm;iwm++)
  {
    MeshWeightMap &mwm = mlod.mlod_aWeightMaps[iwm];
    for(int iww=0;iww<mwm.mwm_aVertexWeight.Count();iww++)
    {
      MeshVertexWeight &mwh = mwm.mwm_aVertexWeight[iww];
      mwh.mww_fWeight /= aWeightFactors[mwh.mww_iVertex];
    }
  }
  // clear weight array
  aWeightFactors.Clear();
}
// normalize weights in mesh
void CMesh::NormalizeWeights()
{
  INDEX ctmlods = msh_aMeshLODs.Count();
  for(INDEX imlod=0;imlod<ctmlods;imlod++)
  {
    // normalize each lod
    NormalizeWeightsInLod(msh_aMeshLODs[imlod]);
  }
}
// add new mesh lod to mesh
void CMesh::AddMeshLod(MeshLOD &mlod)
{
  INDEX ctmlods = msh_aMeshLODs.Count();
  msh_aMeshLODs.Expand(ctmlods+1);
  msh_aMeshLODs[ctmlods] = mlod;
}
// remove mesh lod from mesh
void CMesh::RemoveMeshLod(MeshLOD *pmlodRemove)
{
  INDEX ctmlod = msh_aMeshLODs.Count();
  // create temp space for skeleton lods
  CStaticArray<struct MeshLOD> aTempMLODs;
  aTempMLODs.New(ctmlod-1);
  INDEX iIndexSrc=0;

  // for each skeleton lod in skeleton
  for(INDEX imlod=0;imlod<ctmlod;imlod++)
  {
    MeshLOD *pmlod = &msh_aMeshLODs[imlod];
    // copy all skeleton lods except the selected one
    if(pmlod != pmlodRemove)
    {
      aTempMLODs[iIndexSrc] = *pmlod;
      iIndexSrc++;
    }
  }
  // copy temp array of skeleton lods back in skeleton
  msh_aMeshLODs.CopyArray(aTempMLODs);
  // clear temp skleletons lods array
  aTempMLODs.Clear();
}

// clear mesh
void CMesh::Clear(void)
{
  // for each LOD
  INDEX ctmlod = msh_aMeshLODs.Count();
  for (INDEX imlod=0; imlod<ctmlod; imlod++)
  {
    // for each surface, clear the triangles list
    MeshLOD &mlod = msh_aMeshLODs[imlod];
    INDEX ctsrf = mlod.mlod_aSurfaces.Count();
    for (INDEX isrf=0;isrf<ctsrf;isrf++)
    {
      MeshSurface &msrf = mlod.mlod_aSurfaces[isrf];
      msrf.msrf_aTriangles.Clear();
      // release shader form stock
      if(msrf.msrf_pShader!=NULL) _pShaderStock->Release(msrf.msrf_pShader);
      msrf.msrf_pShader = NULL;
    }
    // clear the surfaces array
    mlod.mlod_aSurfaces.Clear();
    // for each uvmap, clear the texcord list
    INDEX ctuvm = mlod.mlod_aUVMaps.Count();
    for (INDEX iuvm=0;iuvm<ctuvm;iuvm++)
    {
      mlod.mlod_aUVMaps[iuvm].muv_aTexCoords.Clear();
    }
    // clear the uvmaps array
    mlod.mlod_aUVMaps.Clear();
    // clear the vertices array
    mlod.mlod_aVertices.Clear();
    // clear the normals array
    mlod.mlod_aNormals.Clear();
  }
  // in the end, clear all LODs
  msh_aMeshLODs.Clear();
}

// Count used memory
SLONG CMesh::GetUsedMemory(void)
{
  SLONG slMemoryUsed = sizeof(*this);
  INDEX ctmlods = msh_aMeshLODs.Count();
  for(INDEX imlod=0;imlod<ctmlods;imlod++) {
    MeshLOD &mlod = msh_aMeshLODs[imlod];
    slMemoryUsed+=sizeof(mlod);
    slMemoryUsed+=mlod.mlod_aVertices.Count() * sizeof(MeshVertex);
    slMemoryUsed+=mlod.mlod_aNormals.Count() * sizeof(MeshNormal);

    // for each uvmap
    INDEX ctuvmaps = mlod.mlod_aUVMaps.Count();
    for(INDEX iuvm=0;iuvm<ctuvmaps;iuvm++) {
      MeshUVMap &uvmap = mlod.mlod_aUVMaps[iuvm];
      slMemoryUsed+=sizeof(uvmap);
      slMemoryUsed+=uvmap.muv_aTexCoords.Count() * sizeof(MeshTexCoord);
    }

    // for each surface
    INDEX ctmsrf = mlod.mlod_aSurfaces.Count();
    for(INDEX imsrf=0;imsrf<ctmsrf;imsrf++) {
      MeshSurface &msrf = mlod.mlod_aSurfaces[imsrf];
      slMemoryUsed+=sizeof(msrf);
      slMemoryUsed+=msrf.msrf_aTriangles.Count() * sizeof(MeshTriangle);
      slMemoryUsed+=sizeof(ShaderParams);
      slMemoryUsed+=sizeof(INDEX) * msrf.msrf_ShadingParams.sp_aiTextureIDs.Count();
      slMemoryUsed+=sizeof(INDEX) * msrf.msrf_ShadingParams.sp_aiTexCoordsIndex.Count();
      slMemoryUsed+=sizeof(COLOR) * msrf.msrf_ShadingParams.sp_acolColors.Count();
      slMemoryUsed+=sizeof(FLOAT) * msrf.msrf_ShadingParams.sp_afFloats.Count();
    }
    // for each weight map
    INDEX ctwm = mlod.mlod_aWeightMaps.Count();
    for(INDEX iwm=0;iwm<ctwm;iwm++) {
      MeshWeightMap &mwm = mlod.mlod_aWeightMaps[iwm];
      slMemoryUsed+=sizeof(mwm);
      slMemoryUsed+=mwm.mwm_aVertexWeight.Count() * sizeof(MeshVertexWeight);
    }
    // for each morphmap
    INDEX ctmm = mlod.mlod_aMorphMaps.Count();
    for(INDEX imm=0;imm<ctmm;imm++) {
      MeshMorphMap &mmm = mlod.mlod_aMorphMaps[imm];
      slMemoryUsed+=sizeof(mmm);
      slMemoryUsed+=mmm.mmp_aMorphMap.Count() * sizeof(MeshVertexMorph);
    }
  }
  return slMemoryUsed;
}
