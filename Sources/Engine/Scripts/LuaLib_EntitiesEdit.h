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


#ifndef SE_INCL_LUA_LIB_ENTITIES_H
#define SE_INCL_LUA_LIB_ENTITIES_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/API/IScriptAPI.h>
#include <luajit/src/lua.hpp>

class CLuaLibEntitiesEdit : public IScriptAPI
{
  public:
    static const luaL_Reg methods[];

  private:
    static int l_SetEntityParent(lua_State* L);

    // Vital.
    static int l_SetEntityHealth(lua_State* L);
    static int l_SetEntityArmor(lua_State* L);
    static int l_SetEntityShields(lua_State* L);

    // Progression.
    static int l_SetEntityLevel(lua_State* L);
    static int l_SetEntityExperience(lua_State* L);

    // Currencies.
    static int l_SetEntityBalance(lua_State* L);
    static int l_SetEntityMoney(lua_State* L);
    static int l_SetEntitySupplies(lua_State* L);

    static int l_SetEntityPropByName(lua_State* L);

    // Position & Orientation.
    static int l_SetEntityRotation(lua_State* L);
    static int l_TeleportEntityToPosition(lua_State* L);
    static int l_TeleportEntityToPlacement(lua_State* L);

    static int l_TeleportEntityToEntity(lua_State* L);
    static int l_GenerateSyncSafeInt(lua_State* L);
    static int l_GenerateSyncSafeFloat(lua_State* L);

    // Events.
    static int l_SendEEventToEntity(lua_State* L);
    static int l_SendEEventExToEntity(lua_State* L);
    
};

#endif  /* include-once check. */