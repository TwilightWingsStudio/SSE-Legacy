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

243
%{
  #include "StdH.h"
%}

class CParticleCloudsHolder : CMovableModelEntity {
name      "ParticleCloudsHolder";
thumbnail "Thumbnails\\ParticleCloudsHolder.tbn";
features "IsImportant";

properties:

components:

  1 model   MODEL_PARTICLECLOUDSHOLDER   "Models\\Editor\\ParticleCloudsHolder.mdl",
  2 texture TEXTURE_PARTICLECLOUDSHOLDER "Models\\Editor\\ParticleCloudsHolder.tex"
  
functions:
  // --------------------------------------------------------------------------------------
  // Renders particles.
  // --------------------------------------------------------------------------------------
  void RenderParticles(void)
  {
    
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

    // Set appearance
    SetModel(MODEL_PARTICLECLOUDSHOLDER);
    SetModelMainTexture(TEXTURE_PARTICLECLOUDSHOLDER);

    return;
  }
};