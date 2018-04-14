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

10000
%{
  #include "StdH.h"
%}

class CTargetHub : CEntity {
name      "CTargetHub";
thumbnail "Thumbnails\\TargetHub.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName     "Name" 'N' = "Target Hub",
  
  10 CEntityPointer m_penTarget01 "Target 01",
  11 CEntityPointer m_penTarget02 "Target 02",
  12 CEntityPointer m_penTarget03 "Target 03",
  13 CEntityPointer m_penTarget04 "Target 04",
  14 CEntityPointer m_penTarget05 "Target 05",
  15 CEntityPointer m_penTarget06 "Target 06",
  16 CEntityPointer m_penTarget07 "Target 07",
  17 CEntityPointer m_penTarget08 "Target 08",
  18 CEntityPointer m_penTarget09 "Target 09",
  19 CEntityPointer m_penTarget10 "Target 10",

components:
  1 model     MODEL_HUB  "Models\\Editor\\TargetHub.mdl",
  2 texture TEXTURE_HUB  "Models\\Editor\\TargetHub.tex",

functions:

  BOOL HandleEvent(const CEntityEvent &ee)
  {
    CEntity *penCaused = NULL;
    CEntity *penTarget = NULL;

    switch (ee.ee_slEvent)
    {
      // receive penCaused
      case EVENTCODE_EStart:
        penCaused = ((EStart &)ee).penCaused;
        break;
      case EVENTCODE_ETrigger:
        penCaused = ((ETrigger &)ee).penCaused;
        break;
      case EVENTCODE_ETargeted: {
        ETargeted &eTargeted = ((ETargeted &)ee);
        penCaused = eTargeted.penCaused;
        penTarget = eTargeted.penTarget;
      } break;
      // don't have event arguments
      case EVENTCODE_EStop:
      case EVENTCODE_EActivate:
      case EVENTCODE_EDeactivate:
      case EVENTCODE_EEnvironmentStart:
      case EVENTCODE_EEnvironmentStop:
      case EVENTCODE_EStopBlindness:
      case EVENTCODE_EStopDeafness:
      case EVENTCODE_ETeleportMovingBrush:
        break;
      // don't receive any other events
      default: {
        return CEntity::HandleEvent(ee);
      }
    }

    const CEntityPointer *apenTargets = &m_penTarget01;

    for (INDEX iTarget = 0; iTarget < 10; iTarget++) {
      SendToTargetEx(apenTargets[iTarget], EventCodeToEventType(ee.ee_slEvent), penCaused, penTarget);
    }

    return CEntity::HandleEvent(ee);
  }

procedures:
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_HUB);
    SetModelMainTexture(TEXTURE_HUB);
    return;
  }
};

