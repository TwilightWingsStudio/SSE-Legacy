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

#include <Engine/Entities/Entity.h>
#include <Engine/Entities/EntityProperties.h>

#include <luajit/src/lua.hpp>
#include <luajit/src/lualib.h>
#include <luajit/src/lauxlib.h>

#include <Engine/Scripts/LuaLib_EntitiesEdit.h>
#include <Engine/Scripts/ScriptEngine.h>
#include <Engine/Scripts/ScriptEngine_internal.h>

#define SCRIPTLIBPREFIX entitiesed

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
// Sets entity health to given value.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntityParent"
int CLuaLibEntitiesEdit::l_SetEntityParent(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  INDEX iParentID = luaL_checkinteger (L, 2);

  DEFENTBYID(penEntity, ulEntityID);
  DEFENTBYID(penSecondEntity, iParentID);
  
  ONLYVALIDENTITY(penEntity);

  penEntity->SetParent(penSecondEntity);

  return 0;
}

// --------------------------------------------------------------------------------------
// Sets entity health to given value.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntityHealth"
int CLuaLibEntitiesEdit::l_SetEntityHealth(lua_State* L)
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
// Sets entity armor to given value.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntityArmor"
int CLuaLibEntitiesEdit::l_SetEntityArmor(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  FLOAT fValue = luaL_checknumber (L, 2);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);

  static_cast<CLiveEntity*>(penEntity)->SetArmor(fValue);

  return 0;
}

// --------------------------------------------------------------------------------------
// Sets entity shields to given value.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntityShields"
int CLuaLibEntitiesEdit::l_SetEntityShields(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  FLOAT fValue = luaL_checknumber (L, 2);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);

  static_cast<CLiveEntity*>(penEntity)->SetShields(fValue);

  return 0;
}

// --------------------------------------------------------------------------------------
// Sets entity level to given value.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntityLevel"
int CLuaLibEntitiesEdit::l_SetEntityLevel(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  INDEX iLevel = ClampDn(luaL_checkinteger (L, 2), 0);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);

  static_cast<CLiveEntity*>(penEntity)->SetLevel(iLevel);

  return 0;
}

// --------------------------------------------------------------------------------------
// Sets entity experience to given value.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntityExperience"
int CLuaLibEntitiesEdit::l_SetEntityExperience(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  INDEX iLevel = ClampDn(luaL_checkinteger (L, 2), 0);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);

  static_cast<CLiveEntity*>(penEntity)->SetExperience(iLevel);

  return 0;
}

// --------------------------------------------------------------------------------------
// Sets entity specified currency balance to given value.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntityBalance"
int CLuaLibEntitiesEdit::l_SetEntityBalance(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 3);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  
  INDEX iCurrencyID = ClampDn(luaL_checkinteger (L, 2), 0);
  INDEX iValue = luaL_checkinteger (L, 3);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);

  static_cast<CLiveEntity*>(penEntity)->SetBalance(iCurrencyID, iValue);

  return 0;
}

// --------------------------------------------------------------------------------------
// Sets entity money to given value.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntityMoney"
int CLuaLibEntitiesEdit::l_SetEntityMoney(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  INDEX iMoney = ClampDn(luaL_checkinteger (L, 2), 0);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);

  static_cast<CLiveEntity*>(penEntity)->SetMoney(iMoney);

  return 0;
}

// --------------------------------------------------------------------------------------
// Sets entity supplies to given value.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntitySupplies"
int CLuaLibEntitiesEdit::l_SetEntitySupplies(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 2);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  INDEX iSupplies = ClampDn(luaL_checkinteger (L, 2), 0);

  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYLIVEENTITY(penEntity);

  static_cast<CLiveEntity*>(penEntity)->SetSupplies(iSupplies);

  return 0;
}

// --------------------------------------------------------------------------------------
// Gets entity property value by its name.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntityPropByName"
int CLuaLibEntitiesEdit::l_SetEntityPropByName(lua_State* L)
{
  ONLYREQARGCT(lua_gettop(L), 3);
  
  ULONG ulEntityID = luaL_checkinteger(L, 1);
  CTString strPropertyName = lua_tostring(L, 2);
  
  DEFENTBYID(penEntity, ulEntityID);
  
  ONLYVALIDENTITY(penEntity);
  
  CEntityProperty *pTargetProperty = penEntity->PropertyForName(strPropertyName);
  
  ONLYVALIDPROPERTY(pTargetProperty);
  
  if (pTargetProperty->ep_ulFlags & EPROPF_READONLY)
  {
    return luaL_error(L, "%s() target property is read-only and cannot be changed!", SCRIPTFUNCNAME);
  }
  
  CEntityProperty::PropertyType eptTargetProperty = pTargetProperty->ep_eptType;
  SLONG offset = pTargetProperty->ep_slOffset; 
  
  if (IsIndexEPT(eptTargetProperty)) {
    // If not number and not boolean
    if (!lua_isnumber(L, 3) && !lua_isboolean(L, 3)) {
      return luaL_error(L, "%s() value is not number or boolean!", SCRIPTFUNCNAME);
    }
    
    INDEX *iValue = (INDEX *)(((UBYTE *)penEntity) + offset); 
    INDEX iNewValue = luaL_checkinteger(L, 3);
    
    *iValue = iNewValue;

  } else if (eptTargetProperty == CEntityProperty::EPT_FLOAT || eptTargetProperty == CEntityProperty::EPT_ANGLE || eptTargetProperty == CEntityProperty::EPT_RANGE) {
    if (!lua_isnumber(L, 3)) {
      return luaL_error(L, "%s() value is not number!", SCRIPTFUNCNAME);
    }

    FLOAT *fValue = (FLOAT *)(((UBYTE *)penEntity) + offset); 
    FLOAT fNewValue = luaL_checknumber (L, 3);
    
    *fValue = fNewValue;

  } else if (eptTargetProperty == CEntityProperty::EPT_ENTITYPTR) {
    if (!lua_isnumber(L, 3)) {
      return luaL_error(L, "%s() value is not number!", SCRIPTFUNCNAME);
    }
    
    CEntityPointer *penPointer = (CEntityPointer *)(((UBYTE *)penEntity) + offset);
    INDEX iNewValue = luaL_checkinteger(L, 3);
    DEFENTBYID(penNewEntity, iNewValue);
    
    *penPointer = penNewEntity;

  } else if (eptTargetProperty == CEntityProperty::EPT_STRING || eptTargetProperty == CEntityProperty::EPT_FILENAME || eptTargetProperty == CEntityProperty::EPT_STRINGTRANS) {
    CTString *strValue = (CTString *)(((UBYTE *)penEntity) + offset);
    
    if (!lua_isstring(L, 3)) {
      return luaL_error(L, "%s() value is not string!", SCRIPTFUNCNAME);
    }
    
    *strValue = lua_tostring(L, 3);
  }

  return 0;
}

// --------------------------------------------------------------------------------------
// Sets the entity rotation.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SetEntityRotation"
int CLuaLibEntitiesEdit::l_SetEntityRotation(lua_State* L)
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
#define SCRIPTFUNCNAME "TeleportEntityToPosition"
int CLuaLibEntitiesEdit::l_TeleportEntityToPosition(lua_State* L)
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
// Teleports entity to given placement.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "TeleportEntityToPlacement"
int CLuaLibEntitiesEdit::l_TeleportEntityToPlacement(lua_State* L)
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

// --------------------------------------------------------------------------------------
// Teleports entity to placement of given entity.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "TeleportEntityToEntity"
int CLuaLibEntitiesEdit::l_TeleportEntityToEntity(lua_State* L)
{
  const INDEX REQUIRED_ARGS = 2;

  INDEX ctArgs = lua_gettop(L);
  
  ONLYREQARGCT(ctArgs, REQUIRED_ARGS);

  ULONG ulEntityID = luaL_checkinteger (L, 1);
  ULONG ulSecondEntityID = luaL_checkinteger (L, 2);
  
  DEFENTBYID(penEntity, ulEntityID);
  DEFENTBYID(penSecondEntity, ulSecondEntityID);
  
  ONLYVALIDENTITY(penEntity);
  ONLYVALIDENTITY(penSecondEntity);

  penEntity->Teleport(penSecondEntity->GetPlacement());

  return 0;
}

// --------------------------------------------------------------------------------------
// Generates sync-safe random integer.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GenerateSyncSafeInt"
int CLuaLibEntitiesEdit::l_GenerateSyncSafeInt(lua_State* L)
{
  lua_getglobal(L, SCRIPT_THIS_ENTITYID);
  
  if (!lua_isnumber(L, -1)) {
    return luaL_error(L, "%s() entityID is not integer!", SCRIPTFUNCNAME);
  }
  
  ULONG ulEntityID = lua_tointeger(L, -1);
  
  DEFENTBYID(penEntity, ulEntityID);
  ONLYVALIDENTITY(penEntity);
  
  lua_pushinteger(L, penEntity->IRnd());
  
  return 1;
}

// --------------------------------------------------------------------------------------
// Generates sync-safe random float.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "GenerateSyncSafeFloat"
int CLuaLibEntitiesEdit::l_GenerateSyncSafeFloat(lua_State* L)
{
  lua_getglobal(L, SCRIPT_THIS_ENTITYID);
  
  if (!lua_isnumber(L, -1)) {
    return luaL_error(L, "%s() entityID is not integer!", SCRIPTFUNCNAME);
  }
  
  ULONG ulEntityID = lua_tointeger(L, -1);
  
  DEFENTBYID(penEntity, ulEntityID);
  ONLYVALIDENTITY(penEntity);
  
  lua_pushnumber(L, penEntity->FRnd());
  
  return 1;
}

// --------------------------------------------------------------------------------------
// Sends event of given EventEType to target with custom penCaused argument.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SendEEventToEntity"
int CLuaLibEntitiesEdit::l_SendEEventToEntity(lua_State* L)
{
  const INDEX REQUIRED_ARGS = 3;
  
  INDEX ctArgs = lua_gettop(L);
  
  ONLYREQARGCT(ctArgs, REQUIRED_ARGS);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  INDEX iTypeID = luaL_checkinteger (L, 2);
  ULONG ulSecondEntityID = luaL_checkinteger (L, 3);
  
  if (_pSendEEvent == NULL) {
    return luaL_error(L, "%s() _pSendEEvent is NULL! How it possible?", SCRIPTFUNCNAME);
  }
  
  if (iTypeID < 0 || iTypeID > 13) {
    return luaL_error(L, "%s() Got invalid EventEType!", SCRIPTFUNCNAME);
  }
  
  DEFENTBYID(penEntity, ulEntityID);
  ONLYVALIDENTITY(penEntity);

  DEFENTBYID(penSecondEntity, ulSecondEntityID);

  (*_pSendEEvent)(penEntity, iTypeID, penSecondEntity);
  
  return 1;
}

// --------------------------------------------------------------------------------------
// Sends event of given EventEType to target with custom penCaused argument.
// --------------------------------------------------------------------------------------
#define SCRIPTFUNCNAME "SendEEventExToEntity"
int CLuaLibEntitiesEdit::l_SendEEventExToEntity(lua_State* L)
{
  const INDEX REQUIRED_ARGS = 4;
  
  INDEX ctArgs = lua_gettop(L);
  
  ONLYREQARGCT(ctArgs, REQUIRED_ARGS);
  
  ULONG ulEntityID = luaL_checkinteger (L, 1);
  INDEX iTypeID = luaL_checkinteger (L, 2);
  ULONG ulSecondEntityID = luaL_checkinteger (L, 3);
  ULONG ulThirdEntityID = luaL_checkinteger (L, 4);
  
  if (_pSendEEventEx == NULL) {
    return luaL_error(L, "%s() _pSendEEventEx is NULL! How it possible?", SCRIPTFUNCNAME);
  }
  
  if (iTypeID < 0 || iTypeID > 13) {
    return luaL_error(L, "%s() Got invalid EventEType!", SCRIPTFUNCNAME);
  }
  
  DEFENTBYID(penEntity, ulEntityID);
  ONLYVALIDENTITY(penEntity);

  DEFENTBYID(penSecondEntity, ulSecondEntityID);
  DEFENTBYID(penThirdEntity,  ulThirdEntityID);

  (*_pSendEEventEx)(penEntity, iTypeID, penSecondEntity, penThirdEntity);

  return 1;
}

const luaL_Reg CLuaLibEntitiesEdit::methods[] = {
  {"SetEntityParent", CLuaLibEntitiesEdit::l_SetEntityParent},

  //////// Setters for CLiveEntity ////////
  // Vital.
  {"SetEntityHealth", CLuaLibEntitiesEdit::l_SetEntityHealth},
  {"SetEntityArmor", CLuaLibEntitiesEdit::l_SetEntityArmor},
  {"SetEntityShields", CLuaLibEntitiesEdit::l_SetEntityShields},

  // Progression.
  {"SetEntityLevel", CLuaLibEntitiesEdit::l_SetEntityLevel},
  {"SetEntityExperience", CLuaLibEntitiesEdit::l_SetEntityExperience},

  // Currencies.
  {"SetEntityBalance", CLuaLibEntitiesEdit::l_SetEntityBalance},
  {"SetEntityMoney", CLuaLibEntitiesEdit::l_SetEntityMoney},
  {"SetEntitySupplies", CLuaLibEntitiesEdit::l_SetEntitySupplies},
  
  //////// Working with properties ////////
  {"SetEntityPropByName", CLuaLibEntitiesEdit::l_SetEntityPropByName},
  // void SetEntityPropByID(EID, PROPID, Value)
  
  //////// Setters for position and Orientation ////////
  {"SetEntityRotation", CLuaLibEntitiesEdit::l_SetEntityRotation},
  {"TeleportEntityToPosition", CLuaLibEntitiesEdit::l_TeleportEntityToPosition},
  {"TeleportEntityToPlacement", CLuaLibEntitiesEdit::l_TeleportEntityToPlacement},
  {"TeleportEntityToEntity", CLuaLibEntitiesEdit::l_TeleportEntityToEntity},
  
  // TeleportEntityToEntityWithOffset
  
  {"GenerateSyncSafeInt", CLuaLibEntitiesEdit::l_GenerateSyncSafeInt},
  {"GenerateSyncSafeFloat", CLuaLibEntitiesEdit::l_GenerateSyncSafeFloat},
  
  // Weapon and Ammo

  // SendEvent
  {"SendEEventToEntity", CLuaLibEntitiesEdit::l_SendEEventToEntity},
  {"SendEEventExToEntity", CLuaLibEntitiesEdit::l_SendEEventExToEntity},
  
  {0, 0} /* end of array */
};

void luaopen_luaentitiesedlib(lua_State *L)
{
  lua_getglobal(L, "_G");
  luaL_register(L, NULL, CLuaLibEntitiesEdit::methods);
  lua_pop(L, 1);
}