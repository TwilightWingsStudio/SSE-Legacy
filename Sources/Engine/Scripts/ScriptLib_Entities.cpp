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

#define DEFENTBYID(varname, entityid) \
  CEntity* varname = _pNetwork->ga_World.EntityFromID(entityid);

#define ONLYREQARGCT(gotarg, reqarg, name) \
  if (gotarg != reqarg) \
    return luaL_error(L, "%s() got %d/%d arguments!", name, gotarg, reqarg)

#define ONLYVALIDENTITY(entity, name) \
  if (!entity) \
    return luaL_error(L, "%s() got invalid entity!", name)

#define ONLYLIVEENTITY(entity, name) \
  if (!entity->IsLiveEntity()) \
    return luaL_error(L, "%s() got not live entity!", name)
    
#define ONLYPLAYERENTITY(entity, name) \
  if (!entity->IsPlayerEntity()) \
    return luaL_error(L, "%s() got not player entity!", name)
    
#define ONLYVALIDPROPERTY(property, name) \
  if (!property) \
    return luaL_error(L, "%s() got invalid property!", name)
    
static inline BOOL IsIndexEPT(CEntityProperty::PropertyType eptProperty)
{
  switch (eptProperty)
  {
    case CEntityProperty::EPT_BOOL: return TRUE;
    case CEntityProperty::EPT_INDEX: return TRUE;
    case CEntityProperty::EPT_ENUM: return TRUE;
    case CEntityProperty::EPT_FLAGS: return TRUE;
    case CEntityProperty::EPT_ANIMATION: return TRUE;
    case CEntityProperty::EPT_ILLUMINATIONTYPE: return TRUE;
    case CEntityProperty::EPT_COLOR: return TRUE;
  }
  
  return FALSE;
}

// --------------------------------------------------------------------------------------
// Checks if entity valid.
// --------------------------------------------------------------------------------------
static int l_entities_IsEntityValid(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1, "IsEntityValid");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);

  lua_pushinteger(L, penEntity != NULL);

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is live entity.
// --------------------------------------------------------------------------------------
static int l_entities_IsLiveEntity(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1, "IsLiveEntity");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity, "IsLiveEntity");

  lua_pushinteger(L, penEntity->IsLiveEntity());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is alive
// --------------------------------------------------------------------------------------
static int l_entities_IsEntityAlive(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1, "IsEntityAlive");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity, "IsEntityAlive");

  lua_pushinteger(L, penEntity->IsAlive());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is alive
// --------------------------------------------------------------------------------------
static int l_entities_IsEntityDead(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1, "IsEntityDead");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity, "IsEntityDead");

  lua_pushinteger(L, penEntity->IsDead());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is rational entity.
// --------------------------------------------------------------------------------------
static int l_entities_IsRationalEntity(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1, "IsRationalEntity");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity, "IsRationalEntity");

  lua_pushinteger(L, penEntity->IsRationalEntity());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is player entity.
// --------------------------------------------------------------------------------------
static int l_entities_IsMovableEntity(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1, "IsMovableEntity");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity, "IsMovableEntity");

  lua_pushinteger(L, penEntity->IsMovableEntity());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is player entity.
// --------------------------------------------------------------------------------------
static int l_entities_IsPlayerEntity(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1, "IsPlayerEntity");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity, "IsPlayerEntity");

  lua_pushinteger(L, penEntity->IsPlayerEntity());

  return 1;
}

// --------------------------------------------------------------------------------------
// Returns distance between two specified entities.
// --------------------------------------------------------------------------------------
static int l_entities_DistanceBetweenEntities(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2, "DistanceBetweenEntities");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  ULONG ulSecondEntityID = luaL_checkinteger (L, 2);

  DEFENTBYID(penEntity, ulEntityID);
  DEFENTBYID(penSecondEntity, ulSecondEntityID);
  
  ONLYVALIDENTITY(penEntity, "DistanceBetweenEntities");
  ONLYVALIDENTITY(penSecondEntity, "DistanceBetweenEntities");

  lua_pushnumber(L, (penEntity->GetPlacement().pl_PositionVector - penSecondEntity->GetPlacement().pl_PositionVector).Length());

  return 1;
}

// --------------------------------------------------------------------------------------
// Sets entity health to given value.
// --------------------------------------------------------------------------------------
static int l_entities_SetEntityHealth(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2, "SetEntityHealth");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  FLOAT fValue = luaL_checknumber (L, 2);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity, "SetEntityHealth");
  ONLYLIVEENTITY(penEntity, "SetEntityHealth");

  static_cast<CLiveEntity*>(penEntity)->SetHealth(fValue);

  return 0;
}

// --------------------------------------------------------------------------------------
// Gets entity health.
// --------------------------------------------------------------------------------------
static int l_entities_GetEntityHealth(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1, "GetEntityHealth");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity, "GetEntityHealth");
  ONLYLIVEENTITY(penEntity, "GetEntityHealth");
  
  lua_pushnumber(L, static_cast<CLiveEntity*>(penEntity)->GetHealth());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity armor.
// --------------------------------------------------------------------------------------
static int l_entities_GetEntityArmor(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1, "GetEntityArmor");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity, "GetEntityArmor");
  ONLYLIVEENTITY(penEntity, "GetEntityArmor");
  
  lua_pushnumber(L, static_cast<CLiveEntity*>(penEntity)->GetArmor());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity property value by its name.
// --------------------------------------------------------------------------------------
static int l_entities_GetEntityPropByName(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2, "GetEntityPropByName");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  CTString strPropertyName = lua_tostring(L, 2);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity, "GetEntityPropByName");
  
  CEntityProperty *pTargetProperty = penEntity->PropertyForName(strPropertyName);
  
  ONLYVALIDPROPERTY(pTargetProperty, "GetEntityPropByName");
  
  CEntityProperty::PropertyType eptTargetProperty = pTargetProperty->ep_eptType;
  SLONG offset = pTargetProperty->ep_slOffset; 
  
  if (IsIndexEPT(eptTargetProperty)) {
    INDEX iValue = *((INDEX *)(((UBYTE *)penEntity) + offset)); 
    lua_pushinteger(L, iValue);

  } else if (eptTargetProperty == CEntityProperty::EPT_FLOAT || eptTargetProperty == CEntityProperty::EPT_ANGLE || eptTargetProperty == CEntityProperty::EPT_RANGE) {
    FLOAT fValue = *((FLOAT *)(((UBYTE *)penEntity) + offset)); 
    lua_pushnumber(L, fValue);

  } else {
    lua_pushinteger(L, -1);
  }

  return 1;
}

// --------------------------------------------------------------------------------------
// Teleports entity to given coords.
// --------------------------------------------------------------------------------------
static int l_entities_TeleportEntityToXYZ(lua_State* L)
{
  const INDEX REQUIRED_ARGS = 4;

  INDEX ctArgs = lua_gettop(L);
  
  ONLYREQARGCT(ctArgs, REQUIRED_ARGS, "TeleportEntityToXYZ");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  FLOAT3D vNewPos(0.0F, 0.0F, 0.0F);
  vNewPos(1) = luaL_checknumber (L, 2);
  vNewPos(2) = luaL_checknumber (L, 3);
  vNewPos(3) = luaL_checknumber (L, 4);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity, "TeleportEntityToXYZ");

  penEntity->Teleport(CPlacement3D(vNewPos, penEntity->GetPlacement().pl_OrientationAngle));

  return 0;
}

// --------------------------------------------------------------------------------------
// Teleports entity to given coords.
// --------------------------------------------------------------------------------------
static int l_entities_TeleportEntityToEntity(lua_State* L)
{
  const INDEX REQUIRED_ARGS = 2;

  INDEX ctArgs = lua_gettop(L);
  
  ONLYREQARGCT(ctArgs, REQUIRED_ARGS, "TeleportEntityToEntity");
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  ULONG ulSecondEntityID = luaL_checkinteger (L, 2);
  
  DEFENTBYID(penEntity, ulEntityID);
  DEFENTBYID(penSecondEntity, ulSecondEntityID);
  
  ONLYVALIDENTITY(penEntity, "TeleportEntityToEntity");

  penEntity->Teleport(penSecondEntity->GetPlacement());

  return 0;
}

static const struct luaL_Reg entitiesLib [] = {
  // Common entity checks.
  {"IsEntityValid", l_entities_IsEntityValid},
  {"IsLiveEntity", l_entities_IsLiveEntity},
  {"IsRationalEntity", l_entities_IsRationalEntity},
  {"IsMovableEntity", l_entities_IsMovableEntity},
  {"IsPlayerEntity", l_entities_IsPlayerEntity},

  {"IsEntityAlive", l_entities_IsEntityAlive},
  {"IsEntityDead", l_entities_IsEntityDead},

  // Getters for CLiveEntity stuff.
  {"GetEntityArmor", l_entities_GetEntityArmor},
  {"GetEntityHealth", l_entities_GetEntityHealth},

  // Setters for CLiveEntity stuff.
  {"SetEntityHealth", l_entities_SetEntityHealth},

  // Working with properties.
  {"GetEntityPropByName", l_entities_GetEntityPropByName},
  
  // Utils.
  {"DistanceBetweenEntities", l_entities_DistanceBetweenEntities},
  {"TeleportEntityToXYZ", l_entities_TeleportEntityToXYZ},
  {"TeleportEntityToEntity", l_entities_TeleportEntityToEntity},
  {NULL, NULL} /* end of array */
};

void luaopen_luaentitieslib(lua_State *L)
{
  lua_getglobal(L, "_G");
  luaL_register(L, NULL, entitiesLib);
  lua_pop(L, 1);
}