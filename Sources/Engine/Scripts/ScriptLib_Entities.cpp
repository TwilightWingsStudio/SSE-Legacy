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

#define SCRIPTLIBPREFIX entities

#pragma warning (disable: 4005)

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
#define SCRIPTFUNCNAME "IsEntityValid"
static int l_entities_IsEntityValid(lua_State* L)
{  
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);

  lua_pushinteger(L, penEntity != NULL);

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is live entity.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsLiveEntity"
static int l_entities_IsLiveEntity(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsLiveEntity());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is alive
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsEntityAlive"
static int l_entities_IsEntityAlive(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsAlive());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is alive
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsEntityDead"
static int l_entities_IsEntityDead(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsDead());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is rational entity.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsRationalEntity"
static int l_entities_IsRationalEntity(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsRationalEntity());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is player entity.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsMovableEntity"
static int l_entities_IsMovableEntity(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsMovableEntity());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is player entity.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsPlayerEntity"
static int l_entities_IsPlayerEntity(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsPlayerEntity());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is interaction provider.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsInteractionProvider"
static int l_entities_IsInteractionProvider(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsInteractionProvider());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is interaction relay.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsInteractionRelay"
static int l_entities_IsInteractionRelay(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsInteractionRelay());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity position on X axis.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityPosX"
static int l_entities_GetEntityPosX(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushnumber(L, static_cast<CLiveEntity*>(penEntity)->GetPlacement().pl_PositionVector(1));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity position on Y axis.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityPosY"
static int l_entities_GetEntityPosY(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushnumber(L, static_cast<CLiveEntity*>(penEntity)->GetPlacement().pl_PositionVector(2));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity position on Z axis.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityPosZ"
static int l_entities_GetEntityPosZ(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushnumber(L, static_cast<CLiveEntity*>(penEntity)->GetPlacement().pl_PositionVector(3));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity heading rotation.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityRotH"
static int l_entities_GetEntityRotH(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushnumber(L, static_cast<CLiveEntity*>(penEntity)->GetPlacement().pl_OrientationAngle(1));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity pitch rotation.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityRotP"
static int l_entities_GetEntityRotP(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushnumber(L, static_cast<CLiveEntity*>(penEntity)->GetPlacement().pl_OrientationAngle(2));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity banking rotation.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityRotB"
static int l_entities_GetEntityRotB(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushnumber(L, static_cast<CLiveEntity*>(penEntity)->GetPlacement().pl_OrientationAngle(3));

  return 1;
}

// --------------------------------------------------------------------------------------
// Returns distance between two specified entities.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "DistanceBetweenEntities"
static int l_entities_DistanceBetweenEntities(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  ULONG ulSecondEntityID = luaL_checkinteger (L, 2);

  DEFENTBYID(penEntity, ulEntityID);
  DEFENTBYID(penSecondEntity, ulSecondEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYVALIDENTITY(penSecondEntity);

  lua_pushnumber(L, (penEntity->GetPlacement().pl_PositionVector - penSecondEntity->GetPlacement().pl_PositionVector).Length());

  return 1;
}

// --------------------------------------------------------------------------------------
// Returns distance between two specified points.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "DistanceBetweenPoints"
static int l_entities_DistanceBetweenPoints(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 6);

  FLOAT3D vFirstPoint(0.0F, 0.0F, 0.0F);
  vFirstPoint(1) = luaL_checknumber (L, 1);
  vFirstPoint(2) = luaL_checknumber (L, 2);
  vFirstPoint(3) = luaL_checknumber (L, 3);
  
  FLOAT3D vSecondPoint(0.0F, 0.0F, 0.0F);
  vSecondPoint(1) = luaL_checknumber (L, 4);
  vSecondPoint(2) = luaL_checknumber (L, 5);
  vSecondPoint(3) = luaL_checknumber (L, 6); 

  lua_pushnumber(L, (vFirstPoint - vSecondPoint).Length());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity health.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityHealth"
static int l_entities_GetEntityHealth(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushnumber(L, static_cast<CLiveEntity*>(penEntity)->GetHealth());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity armor.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityArmor"
static int l_entities_GetEntityArmor(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushnumber(L, static_cast<CLiveEntity*>(penEntity)->GetArmor());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity shields.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityShields"
static int l_entities_GetEntityShields(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushnumber(L, static_cast<CLiveEntity*>(penEntity)->GetShields());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity level.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityLevel"
static int l_entities_GetEntityLevel(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushinteger(L, static_cast<CLiveEntity*>(penEntity)->GetLevel());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity property value by its name.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityPropByName"
static int l_entities_GetEntityPropByName(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  CTString strPropertyName = lua_tostring(L, 2);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  
  CEntityProperty *pTargetProperty = penEntity->PropertyForName(strPropertyName);
  
  ONLYVALIDPROPERTY(pTargetProperty);
  
  CEntityProperty::PropertyType eptTargetProperty = pTargetProperty->ep_eptType;
  SLONG offset = pTargetProperty->ep_slOffset; 
  
  if (IsIndexEPT(eptTargetProperty)) {
    INDEX iValue = *((INDEX *)(((UBYTE *)penEntity) + offset)); 
    lua_pushinteger(L, iValue);

  } else if (eptTargetProperty == CEntityProperty::EPT_FLOAT || eptTargetProperty == CEntityProperty::EPT_ANGLE || eptTargetProperty == CEntityProperty::EPT_RANGE) {
    FLOAT fValue = *((FLOAT *)(((UBYTE *)penEntity) + offset)); 
    lua_pushnumber(L, fValue);
    
  } else if (eptTargetProperty == CEntityProperty::EPT_ENTITYPTR) {
    CEntityPointer penPointer = *((CEntityPointer *)(((UBYTE *)penEntity) + offset));
    
    if (penPointer) {
      lua_pushinteger(L, penPointer->en_ulID);
    } else {
      lua_pushinteger(L, -1);
    }
    
  } else if (eptTargetProperty == CEntityProperty::EPT_STRING || eptTargetProperty == CEntityProperty::EPT_FILENAME || eptTargetProperty == CEntityProperty::EPT_STRINGTRANS) {
    CTString strValue = *((CTString *)(((UBYTE *)penEntity) + offset));
    lua_pushstring(L, strValue);

  } else {
    lua_pushinteger(L, -1);
  }

  return 1;
}

static const struct luaL_Reg entitiesLib [] = {
  //////// Common entity checks ////////
  {"IsEntityValid", l_entities_IsEntityValid},
  {"IsLiveEntity", l_entities_IsLiveEntity},
  {"IsRationalEntity", l_entities_IsRationalEntity},
  {"IsMovableEntity", l_entities_IsMovableEntity},
  {"IsPlayerEntity", l_entities_IsPlayerEntity},

  {"IsInteractionProvider", l_entities_IsInteractionProvider},
  {"IsInteractionRelay", l_entities_IsInteractionRelay},

  {"IsEntityAlive", l_entities_IsEntityAlive},
  {"IsEntityDead", l_entities_IsEntityDead},

  //////// Getters for CLiveEntity ////////
  {"GetEntityArmor", l_entities_GetEntityArmor},
  {"GetEntityHealth", l_entities_GetEntityHealth},
  {"GetEntityShields", l_entities_GetEntityShields},
  {"GetEntityLevel", l_entities_GetEntityLevel},

  //////// Working with properties ////////
  {"GetEntityPropByName", l_entities_GetEntityPropByName},
  // <multitype> GetEntityPropByID(EID, PROPID)
  
  //////// Getters for position and Orientation ////////
  {"GetEntityPosX", l_entities_GetEntityPosX},
  {"GetEntityPosY", l_entities_GetEntityPosY},
  {"GetEntityPosZ", l_entities_GetEntityPosZ},
  
  {"GetEntityRotH", l_entities_GetEntityRotH},
  {"GetEntityRotP", l_entities_GetEntityRotP},
  {"GetEntityRotB", l_entities_GetEntityRotB},
  
  //////// Utils ////////
  {"DistanceBetweenEntities", l_entities_DistanceBetweenEntities},
  {"DistanceBetweenPoints", l_entities_DistanceBetweenPoints},
  
  //////// Raycasting ////////
  /*
  enum CCastRay::TestType {
    TT_NONE,            // do not test at all
    TT_SIMPLE,          // do approximate testing
    TT_COLLISIONBOX,    // do testing by collision box
    TT_FULL,            // do full testing
    TT_FULLSEETHROUGH,  // do full testing without entities marked as see through
  };
  */

  // EID CastRayFromEntityToEntity(EID, EID, CCastRay::TestType)
  // EID CastRayFromPosToPos(X1, Y1, Z1, X2, Y2, Z2, CCastRay::TestType)
  // EID CastRayFromPosToEntity(EID, X, Y, Z, CCastRay::TestType)
  // EID CastRayFromEntityToPos(EID, X, Y, Z, CCastRay::TestType)
  
  {NULL, NULL} /* end of array */
};

void luaopen_luaentitieslib(lua_State *L)
{
  lua_getglobal(L, "_G");
  luaL_register(L, NULL, entitiesLib);
  lua_pop(L, 1);
}