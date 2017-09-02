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

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define LUAFUNCBEG() LUAFUNCBEG_EX(SCRIPTLIBPREFIX, SCRIPTFUNCNAME)

#define LUAFUNCBEG_EX(scriptlibprefix, myfuncname) \
  extern int l_##scriptlibprefix_##myfuncname(lua_State* L)

#define DEFENTBYID(varname, entityid) \
  CEntity* varname = _pNetwork->ga_World.EntityFromID(entityid);

#define ONLYREQARGCT(gotarg, reqarg) \
  if (gotarg != reqarg) \
    return (gotarg > reqarg ? luaL_error(L, "%s() too many args (%d/%d)", SCRIPTFUNCNAME, gotarg, reqarg) : luaL_error(L, "%s() missing arg (%d/%d)", SCRIPTFUNCNAME, gotarg, reqarg))

#define ONLYVALIDENTITY(entity) \
  if (!entity) \
    return luaL_error(L, "%s() got invalid entity!", SCRIPTFUNCNAME)

#define ONLYLIVEENTITY(entity) \
  if (!entity->IsLiveEntity()) \
    return luaL_error(L, "%s() got not live entity!", SCRIPTFUNCNAME)
    
#define ONLYPLAYERENTITY(entity) \
  if (!entity->IsPlayerEntity()) \
    return luaL_error(L, "%s() got not player entity!", SCRIPTFUNCNAME)
    
#define ONLYVALIDPROPERTY(property) \
  if (!property) \
    return luaL_error(L, "%s() got invalid property!", SCRIPTFUNCNAME)
    
void luaopen_luasebaselib(lua_State *L);
void luaopen_luaentitieslib(lua_State *L);
void luaopen_luaentitiesedlib(lua_State *L);

BOOL luaut_CallVoid(lua_State *L, const char *pName);