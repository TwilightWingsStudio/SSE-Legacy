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
#include <Engine/Base/Stream.h>
#include <Engine/Base/CTString.inl>
#include <Engine/Graphics/Color.h>

#include <Engine/Templates/StaticArray.cpp>
#include <Engine/Templates/DynamicArray.cpp>
#include <Engine/Templates/DynamicContainer.cpp>

#include <Engine/Templates/Stock_CModelData.h>

CModelData::CModelData(const CModelData &c) { ASSERT(FALSE); };
CModelData &CModelData::operator=(const CModelData &c){ ASSERT(FALSE); return *this; };

/*
 * Default constructor sets invalid data
 */
CModelData::CModelData()
{
  INDEX i;
  md_bPreparedForRendering = FALSE;

  md_VerticesCt = 0;															// number of vertices in model
  md_FramesCt = 0;																// number of all frames used by this model
  md_MipCt = 0;																		// number of mip-models

  md_bIsEdited = FALSE; // not edited by default

  // invalidate mip-model info data
  for (i = 0; i < MAX_MODELMIPS; i++) {
    md_MipInfos[i].mmpi_PolygonsCt = 0;
  }

  md_Flags = 0;		                                // model flags (flat, reflection mapping)
  md_ShadowQuality = 0;
  md_Stretch = FLOAT3D(1, 1, 1);                 		// stretch vector (static one, dynamic one is in model object)
  md_bCollideAsCube = FALSE;                      // collide as sphere
  md_colDiffuse = C_WHITE | CT_OPAQUE;
  md_colReflections = C_WHITE | CT_OPAQUE;
  md_colSpecular = C_WHITE | CT_OPAQUE;
  md_colBump = C_WHITE | CT_OPAQUE;
}

/*
 * Default destructor asserts invalid data and clears valid ones
 */
CModelData::~CModelData()
{
  Clear();
}

/*
 * Clear all model data arrays
 */
void CModelData::Clear(void)
{
  INDEX i;
  md_bPreparedForRendering = FALSE;

  CAnimData::Clear();

  md_FrameVertices16.Clear();
  md_FrameVertices8.Clear();
  md_FrameInfos.Clear();
  md_MainMipVertices.Clear();
  md_TransformedVertices.Clear();
  md_VertexMipMask.Clear();
  md_aampAttachedPosition.Clear();
  md_acbCollisionBox.Clear();

  for (i = 0; i < md_MipCt; i++) md_MipInfos[i].Clear();
  for (i = 0; i < MAX_COLOR_NAMES; i++) md_ColorNames[i].Clear();
  for (i = 0; i < MAX_TEXTUREPATCHES; i++) md_mpPatches[i].mp_toTexture.SetData_t(CTString(""));

  md_VerticesCt = 0;
  md_FramesCt = 0;
  md_MipCt = 0;
}

// get amount of memory used by this object
SLONG CModelData::GetUsedMemory(void)
{
  SLONG slUsed = sizeof(*this) + CAnimData::GetUsedMemory() - sizeof(CAnimData);

  slUsed += md_FrameVertices16.Count()*sizeof(struct ModelFrameVertex16);
  slUsed += md_FrameVertices8.Count()*sizeof(struct ModelFrameVertex8);
  slUsed += md_FrameInfos.Count()*sizeof(struct ModelFrameInfo);
  slUsed += md_MainMipVertices.Count()*sizeof(FLOAT3D);
  slUsed += md_TransformedVertices.Count()*sizeof(struct TransformedVertexData);
  slUsed += md_VertexMipMask.Count()*sizeof(ULONG);
  slUsed += md_acbCollisionBox.Count()*sizeof(CModelCollisionBox);
  slUsed += md_aampAttachedPosition.Count()*sizeof(CAttachedModelPosition);

  for (INDEX i = 0; i < md_MipCt; i++)
  {
    slUsed += md_MipInfos[i].mmpi_aPolygonsPerPatch.Count()*sizeof(struct PolygonsPerPatch);
    slUsed += md_MipInfos[i].mmpi_Polygons.Count()*sizeof(struct ModelPolygon);
    slUsed += md_MipInfos[i].mmpi_TextureVertices.Count()*sizeof(struct ModelTextureVertex);
    slUsed += md_MipInfos[i].mmpi_MappingSurfaces.Count()*sizeof(struct MappingSurface);
  }

  return slUsed;
}

/* Get the description of this object. */
CTString CModelData::GetDescription(void)
{
  CTString str;

  ModelMipInfo &mmi0 = md_MipInfos[0];
  str.PrintF("%d mips, %d anims, %d frames, %d vtx, %d svx, %d tri",
    md_MipCt, ad_NumberOfAnims, md_FramesCt,
    mmi0.mmpi_ctMipVx, mmi0.mmpi_ctSrfVx, mmi0.mmpi_ctTriangles);

  return str;
}

/*
 * Routine converts mpv_ptvTransformedVertex and mpv_ptvTextureVertex from Indices to ptrs
 */
void CModelData::IndicesToPtrs()
{
  INDEX i, j;

  for (i = 0; i < md_MipCt; i++)
  {
    FOREACHINSTATICARRAY(md_MipInfos[i].mmpi_Polygons, ModelPolygon, it1)
    {
      FOREACHINSTATICARRAY(it1.Current().mp_PolygonVertices, ModelPolygonVertex, it2)
      {
        struct ModelPolygonVertex * pMPV = &it2.Current();

        j = (INDEX)it2.Current().mpv_ptvTransformedVertex;
        it2.Current().mpv_ptvTransformedVertex = &md_TransformedVertices[j];

        j = (INDEX)it2.Current().mpv_ptvTextureVertex;
        it2.Current().mpv_ptvTextureVertex = &md_MipInfos[i].mmpi_TextureVertices[j];
      }
    }
  }
}

/*
 * Routine converts mpv_ptvTransformedVertex and mpv_ptvTextureVertex from ptrs to Indices
 */
void CModelData::PtrsToIndices()
{
  INDEX i, j;

  for (i = 0; i < md_MipCt; i++)
  {
    FOREACHINSTATICARRAY(md_MipInfos[i].mmpi_Polygons, ModelPolygon, it1)
    {
      FOREACHINSTATICARRAY(it1.Current().mp_PolygonVertices, ModelPolygonVertex, it2)
      {
        for (j = 0; j < md_TransformedVertices.Count(); j++) {
          if (it2.Current().mpv_ptvTransformedVertex == &md_TransformedVertices[j])
            break;
        }

        it2.Current().mpv_ptvTransformedVertex = (struct TransformedVertexData *) j;

        for (j = 0; j < md_MipInfos[i].mmpi_TextureVertices.Count(); j++) {
          if (it2.Current().mpv_ptvTextureVertex == &md_MipInfos[i].mmpi_TextureVertices[j])
            break;
        }

        it2.Current().mpv_ptvTextureVertex = (ModelTextureVertex *)j;
      }
    }
  }
}

// reference counting (override from CAnimData)
void CModelData::RemReference_internal(void)
{
  // if this model is part of edit model object
  if (md_bIsEdited) {
    // just unreference it
    MarkUnused();
    // if this model is part of model stock
  }
  else {
    // release it
    _pModelStock->Release(this);
  }
}

ULONG CModelData::GetFlags(void)
{
  return md_Flags;
};

void CModelData::SpreadMipSwitchFactors(INDEX iFirst, float fStartingFactor)
{
  // set switch steep to cover all mips until max default range reached
  FLOAT fSteep;

  // if we allready skipped max range or we don't have any more mips
  if ((fStartingFactor > MAX_SWITCH_FACTOR) || ((md_MipCt - iFirst) <= 0)) {
    fSteep = 1.2f; // define next switch factor offset

  // else divide factor from starting factor to max switch factor with number of mip models left
  } else {
    fSteep = (MAX_SWITCH_FACTOR - fStartingFactor) / (md_MipCt - iFirst);
  }

  // spread mip switch factors for rougher mip models
  for (INDEX i = iFirst; i < md_MipCt; i++)
  {
    md_MipSwitchFactors[i] = fStartingFactor + (i - iFirst + 1)*fSteep;
  }
}

// check if this kind of objects is auto-freed
BOOL CModelData::IsAutoFreed(void)
{
  return FALSE;
}

void CModelData::ClearAnimations(void)
{
  CAnimData::Clear();

  md_FrameVertices16.Clear();
  md_FrameVertices8.Clear();
  md_FrameInfos.Clear();
  md_FramesCt = 0;
}

//--------------------------------------------------------------------------------------------
// riches texture dimensions
void CModelData::GetTextureDimensions(MEX &mexWidth, MEX &mexHeight)
{
  mexWidth = md_Width;
  mexHeight = md_Height;
}

//--------------------------------------------------------------------------------------------
// calculates bounding box of all frames
void CModelData::GetAllFramesBBox(FLOATaabbox3D &MaxBB)
{
  for (INDEX i = 0; i < md_FramesCt; i++) {
    MaxBB |= md_FrameInfos[i].mfi_Box;
  }
}

FLOAT3D CModelData::GetCollisionBoxMin(INDEX iCollisionBox)
{
  md_acbCollisionBox.Lock();
  INDEX iCollisionBoxClamped = Clamp(iCollisionBox, 0L, md_acbCollisionBox.Count() - 1L);
  FLOAT3D vMin = md_acbCollisionBox[iCollisionBoxClamped].mcb_vCollisionBoxMin;
  md_acbCollisionBox.Unlock();

  return vMin;
};

FLOAT3D CModelData::GetCollisionBoxMax(INDEX iCollisionBox = 0)
{
  md_acbCollisionBox.Lock();
  INDEX iCollisionBoxClamped = Clamp(iCollisionBox, 0L, md_acbCollisionBox.Count() - 1L);
  FLOAT3D vMax = md_acbCollisionBox[iCollisionBoxClamped].mcb_vCollisionBoxMax;
  md_acbCollisionBox.Unlock();

  return vMax;
};

// returns HEIGHT_EQ_WIDTH, LENGHT_EQ_WIDTH or LENGHT_EQ_HEIGHT
INDEX CModelData::GetCollisionBoxDimensionEquality(INDEX iCollisionBox = 0)
{
  md_acbCollisionBox.Lock();
  iCollisionBox = Clamp(iCollisionBox, 0L, md_acbCollisionBox.Count() - 1L);
  INDEX iDimEq = md_acbCollisionBox[iCollisionBox].mcb_iCollisionBoxDimensionEquality;
  md_acbCollisionBox.Unlock();

  return iDimEq;
};

void CModelData::LinkDataForSurfaces(BOOL bFirstMip)
{
  INDEX iMipStart = 1;

  if (bFirstMip) {
    iMipStart = 0;
  }

  // for each mip model
  for (INDEX iMip = iMipStart; iMip < md_MipCt; iMip++)
  {
    ModelMipInfo *pMMI = &md_MipInfos[iMip];

    // --------------------- Set index of transformed vertex to texture vertex
    {for (INDEX iPolygon = 0; iPolygon < pMMI->mmpi_Polygons.Count(); iPolygon++)
    {
      for (INDEX iVertex = 0; iVertex < pMMI->mmpi_Polygons[iPolygon].mp_PolygonVertices.Count(); iVertex++)
      {
        ModelPolygonVertex *pmpvPolygonVertex = &pMMI->mmpi_Polygons[iPolygon].mp_PolygonVertices[iVertex];
        INDEX iTransformed = md_TransformedVertices.Index(pmpvPolygonVertex->mpv_ptvTransformedVertex);
        pmpvPolygonVertex->mpv_ptvTextureVertex->mtv_iTransformedVertex = iTransformed;
      }
    }}

    // --------------------- Linking polygons for surface
    // array telling how many polygons are in each surface
    CStaticArray<INDEX> actPolygonsInSurface;
    INDEX ctSurfaces = pMMI->mmpi_MappingSurfaces.Count();
    actPolygonsInSurface.New(ctSurfaces);

    // for each surface
    {for (INDEX iSurface = 0; iSurface < ctSurfaces; iSurface++) {
      actPolygonsInSurface[iSurface] = 0; // reset count of polygons
    }}

    // for each polygon
    {for (INDEX iPolygon = 0; iPolygon < pMMI->mmpi_Polygons.Count(); iPolygon++) {
      actPolygonsInSurface[pMMI->mmpi_Polygons[iPolygon].mp_Surface]++; // increment count of polygons in its surface
    }}

    // for each surface
    {for (INDEX iSurface = 0; iSurface < ctSurfaces; iSurface++)
    {
      // allocate array for polygon indices
      pMMI->mmpi_MappingSurfaces[iSurface].ms_aiPolygons.Clear();
      pMMI->mmpi_MappingSurfaces[iSurface].ms_aiPolygons.New(actPolygonsInSurface[iSurface]);

      if (actPolygonsInSurface[iSurface] != 0) {
        // last place in array will contain counter of added polygons
        pMMI->mmpi_MappingSurfaces[iSurface].ms_aiPolygons[actPolygonsInSurface[iSurface] - 1] = 0;
      }
    }}

    // for each polygon
    for (INDEX iPolygon = 0; iPolygon < pMMI->mmpi_Polygons.Count(); iPolygon++)
    {
      // get surface, polygons in surface and last remembered index of polygon in surface
      INDEX iSurface = pMMI->mmpi_Polygons[iPolygon].mp_Surface;
      INDEX ctPolygonsInSurface = actPolygonsInSurface[iSurface];

      if (ctPolygonsInSurface != 0)
      {
        INDEX iLastSet = pMMI->mmpi_MappingSurfaces[iSurface].ms_aiPolygons[ctPolygonsInSurface - 1];

        pMMI->mmpi_MappingSurfaces[iSurface].ms_aiPolygons[ctPolygonsInSurface - 1] = iLastSet + 1; // remember last set polygon index
        pMMI->mmpi_MappingSurfaces[iSurface].ms_aiPolygons[iLastSet] = iPolygon; // remember index of polygon
      }
    }

    // --------------------- Linking texture vertices for surface
    // for each surface
    {for (INDEX iSurface = 0; iSurface < ctSurfaces; iSurface++)
    {
      CDynamicContainer<ModelTextureVertex> cmtvInSurface;

      // for each polygon in surface
      FOREACHINSTATICARRAY(pMMI->mmpi_MappingSurfaces[iSurface].ms_aiPolygons, INDEX, itimpo)
      {
        ModelPolygon &mpPolygon = pMMI->mmpi_Polygons[itimpo.Current()];

        // for each vertex in polygon
        for (INDEX iVertex = 0; iVertex < mpPolygon.mp_PolygonVertices.Count(); iVertex++)
        {
          // get texture vertex
          ModelTextureVertex *ptv = mpPolygon.mp_PolygonVertices[iVertex].mpv_ptvTextureVertex;

          // if it is not added yet then add it
          if (!cmtvInSurface.IsMember(ptv)) {
            cmtvInSurface.Add(ptv);
          }
        }
      }

      // add needed number of texture vertices
      pMMI->mmpi_MappingSurfaces[iSurface].ms_aiTextureVertices.Clear();
      pMMI->mmpi_MappingSurfaces[iSurface].ms_aiTextureVertices.New(cmtvInSurface.Count());
      INDEX cttv = 0;

      // for each texture vertex in container
      FOREACHINDYNAMICCONTAINER(cmtvInSurface, ModelTextureVertex, itmtv) {
        INDEX idxtv = pMMI->mmpi_TextureVertices.Index(itmtv);
        pMMI->mmpi_MappingSurfaces[iSurface].ms_aiTextureVertices[cttv] = idxtv;
        cttv++;
      }
    }}
  }
}
