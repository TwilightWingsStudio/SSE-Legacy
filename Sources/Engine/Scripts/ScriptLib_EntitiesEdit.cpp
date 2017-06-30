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

#include <Engine/Base/Console.h>
#include <Engine/Base/Shell.h>

#include <Engine/Network/Network.h>

#include <Engine/Entities/Entity.h>
#include <Engine/Entities/EntityProperties.h>

#include <luajit/src/lua.hpp>
#include <luajit/src/lualib.h>
#include <luajit/src/lauxlib.h>

#include <Engine/Scripts/ScriptEngine_internal.h>

#define SCRIPTLIBPREFIX entitiesed

#pragma warning (disable: 4005)

// --------------------------------------------------------------------------------------
// Sets entity health to given value.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntityHealth"
static int l_entitiesed_SetEntityHealth(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  FLOAT fValue = luaL_checknumber (L, 2);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);

  static_cast<CLiveEntity*>(penEntity)->SetHealth(fValue);

  return 0;
}

// --------------------------------------------------------------------------------------
// Teleports entity to given coords.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "TeleportEntityToXYZ"
static int l_entitiesed_TeleportEntityToXYZ(lua_State* L)
{
  const INDEX REQUIRED_ARGS = 4;

  INDEX ctArgs = lua_gettop(L);
  
  ONLYREQARGCT(ctArgs, REQUIRED_ARGS);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  FLOAT3D vNewPos(0.0F, 0.0F, 0.0F);
  vNewPos(1) = luaL_checknumber (L, 2);
  vNewPos(2) = luaL_checknumber (L, 3);
  vNewPos(3) = luaL_checknumber (L, 4);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  penEntity->Teleport(CPlacement3D(vNewPos, penEntity->GetPlacement().pl_OrientationAngle));

  return 0;
}

// --------------------------------------------------------------------------------------
// Sets the entity rotation.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntityRotation"
static int l_entitiesed_SetEntityRotation(lua_State* L)
{
  const INDEX REQUIRED_ARGS = 4;

  INDEX ctArgs = lua_gettop(L);
  
  ONLYREQARGCT(ctArgs, REQUIRED_ARGS);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  ANGLE3D vNewRot(0.0F, 0.0F, 0.0F);
  vNewRot(1) = luaL_checknumber (L, 2);
  vNewRot(2) = luaL_checknumber (L, 3);
  vNewRot(3) = luaL_checknumber (L, 4);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  CPlacement3D plNew(penEntity->GetPlacement().pl_PositionVector, vNewRot);
  penEntity->SetPlacement(plNew);

  return 0;
}

// --------------------------------------------------------------------------------------
// Teleports entity to given coords.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "TeleportEntityToEntity"
static int l_entitiesed_TeleportEntityToEntity(lua_State* L)
{
  const INDEX REQUIRED_ARGS = 2;

  INDEX ctArgs = lua_gettop(L);
  
  ONLYREQARGCT(ctArgs, REQUIRED_ARGS);

  ULONG ulEntityID = luaL_checkinteger (L, 1);
  ULONG ulSecondEntityID = luaL_checkinteger (L, 2);
  
  DEFENTBYID(penEntity, ulEntityID);
  DEFENTBYID(penSecondEntity, ulSecondEntityID);
  
  ONLYVALIDENTITY(penEntity);

  penEntity->Teleport(penSecondEntity->GetPlacement());

  return 0;
}

// --------------------------------------------------------------------------------------
// Teleports entity to given placement.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "TeleportEntityToPlacement"
static int l_entitiesed_TeleportEntityToPlacement(lua_State* L)
{
  const INDEX REQUIRED_ARGS = 7;

  INDEX ctArgs = lua_gettop(L);
  
  ONLYREQARGCT(ctArgs, REQUIRED_ARGS);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  FLOAT3D vNewPos(0.0F, 0.0F, 0.0F);
  vNewPos(1) = luaL_checknumber (L, 2);
  vNewPos(2) = luaL_checknumber (L, 3);
  vNewPos(3) = luaL_checknumber (L, 4);

  ANGLE3D vNewRot(0.0F, 0.0F, 0.0F);
  vNewRot(1) = luaL_checknumber (L, 5);
  vNewRot(2) = luaL_checknumber (L, 6);
  vNewRot(3) = luaL_checknumber (L, 7);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  CPlacement3D plNew(vNewPos, vNewRot);
  penEntity->SetPlacement(plNew);

  return 0;
}

static const struct luaL_Reg entitiesedLib [] = {
  //////// Setters for CLiveEntity ////////
  {"SetEntityHealth", l_entitiesed_SetEntityHealth},
  // void SetEntityArmor(EID, FLOAT)
  // void SetEntityShields(EID, FLOAT)
  
  // void SetEntityPropByName(EID, String, Value)
  // void SetEntityPropByID(EID, PROPID, Value)
  
  {"SetEntityRotation", l_entitiesed_SetEntityRotation},
  {"TeleportEntityToXYZ", l_entitiesed_TeleportEntityToXYZ},
  {"TeleportEntityToPlacement", l_entitiesed_TeleportEntityToPlacement},
  {"TeleportEntityToEntity", l_entitiesed_TeleportEntityToEntity},
  
  // IRnd
  // FRnd
  
  // Weapon and Ammo

  // SendEvent
  
  {NULL, NULL} /* end of array */
};

void luaopen_luaentitiesedlib(lua_State *L)
{
  lua_getglobal(L, "_G");
  luaL_register(L, NULL, entitiesedLib);
  lua_pop(L, 1);
}