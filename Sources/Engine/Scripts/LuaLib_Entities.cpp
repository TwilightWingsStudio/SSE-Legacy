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

#include "StdH.h"

#include <Engine/Base/Console.h>
#include <Engine/Base/Shell.h>

#include <Engine/Network/Network.h>
#include <Engine/Network/SessionState.h>

#include <Engine/Entities/Entity.h>
#include <Engine/Entities/EntityClass.h>
#include <Engine/Entities/EntityProperties.h>
#include <Engine/Entities/InternalClasses.h>

#include <luajit/src/lua.hpp>
#include <luajit/src/lualib.h>
#include <luajit/src/lauxlib.h>

#include <Engine/Scripts/LuaLib_Entities.h>
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
int CLuaLibEntities::l_IsEntityValid(lua_State* L)
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
int CLuaLibEntities::l_IsLiveEntity(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsLiveEntity());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is rational entity.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsRationalEntity"
int CLuaLibEntities::l_IsRationalEntity(lua_State* L)
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
int CLuaLibEntities::l_IsMovableEntity(lua_State* L)
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
int CLuaLibEntities::l_IsPlayerEntity(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsPlayerEntity());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is marker entity.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsMarkerEntity"
int CLuaLibEntities::l_IsMarkerEntity(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsMarker());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is interaction provider.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsInteractionProvider"
int CLuaLibEntities::l_IsInteractionProvider(lua_State* L)
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
int CLuaLibEntities::l_IsInteractionRelay(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsInteractionRelay());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is alive
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsEntityAlive"
int CLuaLibEntities::l_IsEntityAlive(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsAlive());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is dead
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsEntityDead"
int CLuaLibEntities::l_IsEntityDead(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsDead());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is active.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsEntityActive"
int CLuaLibEntities::l_IsEntityActive(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->IsActive());

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is instance of class.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsEntityOfClass"
int CLuaLibEntities::l_IsEntityOfClass(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  CTString strClassName = lua_tostring(L, 2);

  lua_pushinteger(L, IsOfClass(penEntity, strClassName));

  return 1;
}

// --------------------------------------------------------------------------------------
// Checks if entity is instance of child class.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "IsEntityDerivedFromClass"
int CLuaLibEntities::l_IsEntityDerivedFromClass(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  CTString strClassName = lua_tostring(L, 2);

  lua_pushinteger(L, IsDerivedFromClass(penEntity, strClassName));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets the parent entity.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityParent"
int CLuaLibEntities::l_GetEntityParent(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->GetParent() ? penEntity->GetParent()->en_ulID : INDEX(-1));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets the child count of entity.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityChildCount"
int CLuaLibEntities::l_GetEntityChildCount(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushinteger(L, penEntity->GetChildCount());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets the entity name.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityName"
int CLuaLibEntities::l_GetEntityName(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushstring(L, penEntity->GetName());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets the entity class name.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityClassName"
int CLuaLibEntities::l_GetEntityClassName(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushstring(L, penEntity->GetClass()->ec_pdecDLLClass->dec_strName);

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity position on X axis.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityPosX"
int CLuaLibEntities::l_GetEntityPosX(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  
  lua_pushnumber(L, penEntity->GetPlacement().pl_PositionVector(1));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity position on Y axis.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityPosY"
int CLuaLibEntities::l_GetEntityPosY(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  
  lua_pushnumber(L, penEntity->GetPlacement().pl_PositionVector(2));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity position on Z axis.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityPosZ"
int CLuaLibEntities::l_GetEntityPosZ(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  
  lua_pushnumber(L, penEntity->GetPlacement().pl_PositionVector(3));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity heading rotation.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityRotH"
int CLuaLibEntities::l_GetEntityRotH(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);

  lua_pushnumber(L, penEntity->GetPlacement().pl_OrientationAngle(1));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity pitch rotation.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityRotP"
int CLuaLibEntities::l_GetEntityRotP(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  
  lua_pushnumber(L, penEntity->GetPlacement().pl_OrientationAngle(2));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity banking rotation.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityRotB"
int CLuaLibEntities::l_GetEntityRotB(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  
  lua_pushnumber(L, penEntity->GetPlacement().pl_OrientationAngle(3));

  return 1;
}

// --------------------------------------------------------------------------------------
// Returns distance between two specified entities.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "DistanceBetweenEntities"
int CLuaLibEntities::l_DistanceBetweenEntities(lua_State* L)
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
int CLuaLibEntities::l_DistanceBetweenPoints(lua_State* L)
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
int CLuaLibEntities::l_GetEntityHealth(lua_State* L)
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
int CLuaLibEntities::l_GetEntityArmor(lua_State* L)
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
int CLuaLibEntities::l_GetEntityShields(lua_State* L)
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
int CLuaLibEntities::l_GetEntityLevel(lua_State* L)
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
// Gets entity experience.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityExperience"
int CLuaLibEntities::l_GetEntityExperience(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushinteger(L, static_cast<CLiveEntity*>(penEntity)->GetExperience());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity balance for specified currency.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityBalance"
int CLuaLibEntities::l_GetEntityBalance(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);

  INDEX iCurrencyID = luaL_checkinteger (L, 2);
  
  lua_pushinteger(L, static_cast<CLiveEntity*>(penEntity)->GetBalance(iCurrencyID));

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity money.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityMoney"
int CLuaLibEntities::l_GetEntityMoney(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushinteger(L, static_cast<CLiveEntity*>(penEntity)->GetMoney());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity supplies.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntitySupplies"
int CLuaLibEntities::l_GetEntitySupplies(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);
  
  lua_pushinteger(L, static_cast<CLiveEntity*>(penEntity)->GetSupplies());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets entity property value by its name.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetEntityPropByName"
int CLuaLibEntities::l_GetEntityPropByName(lua_State* L)
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

// --------------------------------------------------------------------------------------
// Gets player entity id by its PLID.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetPlayerEntityByPLID"
int CLuaLibEntities::l_GetPlayerEntityByPLID(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  INDEX iPlayerID = luaL_checkinteger (L, 1);
  
  if (iPlayerID < 0 || (iPlayerID-1) > CEntity::GetMaxPlayers()) {
    return luaL_error(L, "%s() wrongly specified PLID!", SCRIPTFUNCNAME);
  }  
  
  CEntity *pen = CEntity::GetPlayerEntity(iPlayerID);
  
  lua_pushinteger(L, pen ? pen->en_ulID : -1);

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets active players count.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetPlayersCount"
int CLuaLibEntities::l_GetPlayersCount(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 0);
  
  lua_pushinteger(L, _pNetwork->ga_sesSessionState.GetPlayersCount());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets max players count.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetMaxPlayers"
int CLuaLibEntities::l_GetMaxPlayers(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 0);
  
  lua_pushinteger(L, CEntity::GetMaxPlayers());

  return 1;
}

// --------------------------------------------------------------------------------------
// Gets PLID from entity.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GetPlayerID"
int CLuaLibEntities::l_GetPlayerID(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 1);
  
  INDEX ulEntityID = luaL_checkinteger (L, 1);

  DEFENTBYID(penEntity, ulEntityID);

  ONLYVALIDENTITY(penEntity);
  ONLYPLAYERENTITY(penEntity);

  lua_pushinteger(L, static_cast<CPlayerEntity*>(penEntity)->GetMyPlayerIndex());

  return 1;
}

const luaL_Reg CLuaLibEntities::methods[] = {
  //////// Common entity checks ////////
  {"IsEntityValid", CLuaLibEntities::l_IsEntityValid},
  {"IsLiveEntity", CLuaLibEntities::l_IsLiveEntity},
  {"IsRationalEntity", CLuaLibEntities::l_IsRationalEntity},
  {"IsMovableEntity", CLuaLibEntities::l_IsMovableEntity},
  {"IsPlayerEntity", CLuaLibEntities::l_IsPlayerEntity},

  {"IsMarkerEntity", CLuaLibEntities::l_IsMarkerEntity},
  {"IsInteractionProvider", CLuaLibEntities::l_IsInteractionProvider},
  {"IsInteractionRelay", CLuaLibEntities::l_IsInteractionRelay},

  {"IsEntityAlive", CLuaLibEntities::l_IsEntityAlive},
  {"IsEntityDead", CLuaLibEntities::l_IsEntityDead},
  
  {"IsEntityActive", CLuaLibEntities::l_IsEntityActive},
  
  //////// Exact Class Checks ////////
  {"IsEntityOfClass", CLuaLibEntities::l_IsEntityOfClass},
  {"IsEntityDerivedFromClass", CLuaLibEntities::l_IsEntityDerivedFromClass},

  //////// CEntity Utils ////////
  {"GetEntityParent", CLuaLibEntities::l_GetEntityParent},
  {"GetEntityChildCount", CLuaLibEntities::l_GetEntityChildCount},
  //{"GetEntityChild", CLuaLibEntities::l_GetEntityChild},
  {"GetEntityName", CLuaLibEntities::l_GetEntityName},
  {"GetEntityClassName", CLuaLibEntities::l_GetEntityClassName},
  //{"GetEntityInteractionProvider", CLuaLibEntities:l_GetEntityInteractionProvider},

  //////// Getters for CLiveEntity ////////
  // Vital.
  {"GetEntityArmor", CLuaLibEntities::l_GetEntityArmor},
  {"GetEntityHealth", CLuaLibEntities::l_GetEntityHealth},
  {"GetEntityShields", CLuaLibEntities::l_GetEntityShields},
  
  // Progression.
  {"GetEntityLevel", CLuaLibEntities::l_GetEntityLevel},
  {"GetEntityExperience", CLuaLibEntities::l_GetEntityExperience},
  
  // Currencies.
  {"GetEntityBalance", CLuaLibEntities::l_GetEntityBalance},
  {"GetEntityMoney", CLuaLibEntities::l_GetEntityMoney},
  {"GetEntitySupplies", CLuaLibEntities::l_GetEntitySupplies},

  //////// Working with properties ////////
  {"GetEntityPropByName", CLuaLibEntities::l_GetEntityPropByName},
  // <multitype> GetEntityPropByID(EID, PROPID)
  
  //////// Getters for position and Orientation ////////
  {"GetEntityPosX", CLuaLibEntities::l_GetEntityPosX},
  {"GetEntityPosY", CLuaLibEntities::l_GetEntityPosY},
  {"GetEntityPosZ", CLuaLibEntities::l_GetEntityPosZ},
  
  {"GetEntityRotH", CLuaLibEntities::l_GetEntityRotH},
  {"GetEntityRotP", CLuaLibEntities::l_GetEntityRotP},
  {"GetEntityRotB", CLuaLibEntities::l_GetEntityRotB},
  
  //////// Utils ////////
  {"DistanceBetweenEntities", CLuaLibEntities::l_DistanceBetweenEntities},
  {"DistanceBetweenPoints", CLuaLibEntities::l_DistanceBetweenPoints},
  // FLOAT DistanceBetweenEntityAndPos(EntityID, X, Y, Z)
  
  //////// Players ////////
  {"GetPlayerEntityByPLID", CLuaLibEntities::l_GetPlayerEntityByPLID},
  {"GetPlayerID", CLuaLibEntities::l_GetPlayerID},
  // {"GetRndActivePlayerID", l_entities_GetRndActivePlayerID},
  {"GetPlayersCount", CLuaLibEntities::l_GetPlayersCount},
  {"GetMaxPlayers", CLuaLibEntities::l_GetMaxPlayers},
  
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
  
  {0, 0} /* end of array */
};

void luaopen_luaentitieslib(lua_State *L)
{
  lua_getglobal(L, "_G");
  luaL_register(L, NULL, CLuaLibEntities::methods);
  lua_pop(L, 1);
}