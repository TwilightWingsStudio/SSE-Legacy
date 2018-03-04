/* Copyright (c) 2017 by ZCaliptium

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

#ifndef SE_INCL_SCRIPTENGINE_H
#define SE_INCL_SCRIPTENGINE_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#define SCRIPT_THIS_ENTITYID "_entityID"
#define SCRIPT_PENCAUSED_ENTITYID "_penCausedID"
#define SCRIPT_PENTARGET_ENTITYID "_penTargetID"

class ENGINE_API CScriptEngine
{
  public:
    void ExecEntityScript(CEntity* penOwner, const CTFileName &fnmScript, CEntity* penCaused, CEntity* penTarget, INDEX aiSlots[5], BOOL bDebugMessages);
};

ENGINE_API extern void (*_pSendEEvent)(CEntity* penTarget, INDEX iType, CEntity* penCaused);
ENGINE_API extern CScriptEngine *_pScriptEngine;

#endif  /* include-once check. */