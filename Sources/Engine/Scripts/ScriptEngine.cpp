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
#include <Engine/Base/FileName.h>
#include <Engine/Base/Stream.h>

#include <Engine/Scripts/ScriptEngine.h>

#include <Engine/Entities/Entity.h>

CScriptEngine *_pScriptEngine = NULL;

#include <luajit/src/lua.hpp>
#include <luajit/src/lualib.h>
#include <luajit/src/lauxlib.h>

#pragma comment(lib, "lua51.lib")

static int l_my_print(lua_State* L)
{
  int ctArgs = lua_gettop(L);
  
  CTString strResult;

  for (int i = 1; i <= ctArgs; i++) {
    strResult.PrintF("%s%s", strResult, lua_tostring(L, i));
  }
  
  //CPrintF("print() redefinition!\n");
  CPrintF("[LUA] %s\n", strResult);

  return 0;
}

static const struct luaL_Reg printlib [] = {
  {"print", l_my_print},
  {NULL, NULL} /* end of array */
};


static void luaopen_luamylib(lua_State *L)
{
  lua_getglobal(L, "_G");
  luaL_register(L, NULL, printlib);
  lua_pop(L, 1);
}

static lua_State *CreateSafeState()
{
  lua_State *L;

  L = luaL_newstate();

  if (L)
  {
      lua_pushcfunction(L, luaopen_base);
      lua_pushstring(L, "");
      lua_call(L, 1, 0);
      lua_pushcfunction(L, luaopen_package);
      lua_pushstring(L, LUA_LOADLIBNAME);
      lua_call(L, 1, 0);
      luaopen_luamylib(L);
  }

  return L;
}

extern void LUAJitTest(void *pArgs)
{
  lua_State *state = CreateSafeState();

  if (!state) {
    CPrintF("[LUA] Error! Failed to initialize Lua VM!\n");
    return;
  }

  int result;
  
  CPrintF("[LUA] Running script sukablyad.lua...\n");
  result = luaL_loadfile(state, "sukablyad.lua");
  if (result != 0)
  {
    CPrintF("[LUA] Error: %s\n", lua_tostring(state, -1));
    lua_pop(state, 1);
    return;
  }

  result = lua_pcall(state, 0, 0, 0);

  if (result != 0)
  {
    CPrintF("[LUA] Error: %s\n", lua_tostring(state, -1));
    lua_pop(state, 1);
    return;
  }

  lua_settop(state, 0); // Cleanup the stack.
}

void CScriptEngine::ExecEntityScript(CEntity* penOwner, const CTFileName &fnmScript, CEntity* penCaused, INDEX aiPins[5])
{
  lua_State *state = CreateSafeState();

  if (!state) {
    CPrintF("[LUA] Error: Failed to initialize Lua VM!\n");
    return;
  }

  int result;
  
  CTFileName fnmFullPath;
  INDEX iFile = ExpandFilePath(EFP_READ, fnmScript, fnmFullPath);
  
  CPrintF("[LUA] Running script '%s'...\n", fnmScript);

  if (iFile != EFP_FILE) {
    CPrintF("[LUA] Error: Unable to run script! Path points to not a file!\n");
    return;
  }
  
  result = luaL_loadfile(state, (const char*)fnmFullPath);
  if (result != 0)
  {
    CPrintF("[LUA] Error: %s\n", lua_tostring(state, -1));
    lua_pop(state, 1);
    return;
  }
  
  lua_pushinteger(state, penOwner->en_ulID);
  lua_setglobal(state, "_entityID");

  lua_pushinteger(state, penCaused ? penCaused->en_ulID : -1);
  lua_setglobal(state, "_penCausedID");
  
  result = lua_pcall(state, 0, 0, 0);

  if (result != 0)
  {
    CPrintF("[LUA] Error: %s\n", lua_tostring(state, -1));
    lua_pop(state, 1);
    return;
  }
  
  lua_close(state);
}