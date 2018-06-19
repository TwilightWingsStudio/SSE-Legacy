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

class CLuaLibEntities : public IScriptAPI
{
  public:
    static const luaL_Reg methods[];

  private:
    // Common checks.
    static int l_IsEntityValid(lua_State* L);
    static int l_IsLiveEntity(lua_State* L);
    static int l_IsRationalEntity(lua_State* L);
    static int l_IsMovableEntity(lua_State* L);
    static int l_IsPlayerEntity(lua_State* L);

    static int l_IsMarkerEntity(lua_State* L);
    static int l_IsInteractionProvider(lua_State* L);
    static int l_IsInteractionRelay(lua_State* L);

    static int l_IsEntityAlive(lua_State* L);
    static int l_IsEntityDead(lua_State* L);

    static int l_IsEntityActive(lua_State* L);

    static int l_IsEntityOfClass(lua_State* L);
    static int l_IsEntityDerivedFromClass(lua_State* L);

    static int l_GetEntityParent(lua_State* L);
    static int l_GetEntityChildCount(lua_State* L);
    static int l_GetEntityName(lua_State* L);
    static int l_GetEntityClassName(lua_State* L);

    // Position.
    static int l_GetEntityPosX(lua_State* L);
    static int l_GetEntityPosY(lua_State* L);
    static int l_GetEntityPosZ(lua_State* L);

    // Orientation.
    static int l_GetEntityRotH(lua_State* L);
    static int l_GetEntityRotP(lua_State* L);
    static int l_GetEntityRotB(lua_State* L);

    static int l_DistanceBetweenEntities(lua_State* L);
    static int l_DistanceBetweenPoints(lua_State* L);

    // Vital.
    static int l_GetEntityHealth(lua_State* L);
    static int l_GetEntityArmor(lua_State* L);
    static int l_GetEntityShields(lua_State* L);

    // Progression.
    static int l_GetEntityLevel(lua_State* L);
    static int l_GetEntityExperience(lua_State* L);

    static int l_GetEntityBalance(lua_State* L);
    static int l_GetEntityMoney(lua_State* L);
    static int l_GetEntitySupplies(lua_State* L);

    // Properties.
    static int l_GetEntityPropByName(lua_State* L);

    // Players.
    static int l_GetPlayerEntityByPLID(lua_State* L);
    static int l_GetPlayersCount(lua_State* L);
    static int l_GetMaxPlayers(lua_State* L);
    static int l_GetPlayerID(lua_State* L);
};

#endif  /* include-once check. */