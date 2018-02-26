/* Copyright (c) 2018 by ZCaliptium.

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

244
%{
  #include "StdH.h"
%}

class CParticleCloudsMarker : CEntity {
name      "ParticleCloudsMarker";
thumbnail "Thumbnails\\ParticleCloudsMarker.tbn";
features  "HasName", "HasDescription", "IsTargetable", "IsImportant";

properties:

  1 CTString m_strName         "Name" 'N' = "ParticleCloudsMarker",
  2 CTString m_strDescription             = "",
  
components:

  1 model   MODEL_MARKER     "Models\\Editor\\ParticleCloudsMarker.mdl",
  2 texture TEXTURE_MARKER   "Models\\Editor\\ParticleCloudsMarker.tex"
  
functions:

procedures:

  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // Set appearance
    SetModel(MODEL_MARKER);
    SetModelMainTexture(TEXTURE_MARKER);

    return;
  }
};