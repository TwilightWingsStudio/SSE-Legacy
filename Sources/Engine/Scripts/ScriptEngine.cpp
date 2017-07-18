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

#include <Engine/Network/Network.h>

#include <Engine/Entities/Entity.h>

#include <luajit/src/lua.hpp>
#include <luajit/src/lualib.h>
#include <luajit/src/lauxlib.h>

#include <Engine/Scripts/ScriptEngine.h>
#include <Engine/Scripts/ScriptEngine_internal.h>

#pragma comment(lib, "lua51.lib")

CScriptEngine *_pScriptEngine = NULL;

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

      luaopen_luasebaselib(L);
      luaopen_luaentitieslib(L);
      luaopen_luaentitiesedlib(L);
  }

  return L;
}

extern void LUAJitTest(void *pArgs)
{
  lua_State *state = CreateSafeState();

  if (!state) {
    CPrintF("[LUA][ERR] Failed to initialize Lua VM!\n");
    return;
  }

  int result;
  
  CPrintF("[LUA][INF] Running script sukablyad.lua...\n");
  result = luaL_loadfile(state, "sukablyad.lua");
  if (result != 0)
  {
    CPrintF("[LUA][ERR] %s\n", lua_tostring(state, -1));
    lua_pop(state, 1);
    return;
  }

  result = lua_pcall(state, 0, 0, 0);

  if (result != 0)
  {
    CPrintF("[LUA][ERR] %s\n", lua_tostring(state, -1));
    lua_pop(state, 1);
    return;
  }

  lua_settop(state, 0); // Cleanup the stack.
}

void CScriptEngine::ExecEntityScript(CEntity* penOwner, const CTFileName &fnmScript, CEntity* penCaused, INDEX aiSlots[5], BOOL bDebugMessages)
{
  lua_State *state = CreateSafeState();

  if (!state) {
    CPrintF("[LUA][ERR] Failed to initialize Lua VM!\n");
    return;
  }

  int result;
  
  CTFileName fnmFullPath;
  INDEX iFile = ExpandFilePath(EFP_READ, fnmScript, fnmFullPath);
  
  if (bDebugMessages) {
    CPrintF("[LUA][INF] Running script '%s'...\n", fnmScript);
  }

  if (iFile != EFP_FILE) {
    CPrintF("[LUA][ERR] Unable to run script! Path points to not a file!\n");
    return;
  }
  
  result = luaL_loadfile(state, (const char*)fnmFullPath);
  
  // If error occured during the script file loading then print it!
  if (result != 0)
  {
    CPrintF("[LUA][ERR] %s\n", lua_tostring(state, -1));
    lua_pop(state, 1);
    return;
  }
  
  // Prepare globals.
  {
    lua_pushinteger(state, penOwner->en_ulID);
    lua_setglobal(state, SCRIPT_THIS_ENTITYID);

    lua_pushinteger(state, penCaused ? penCaused->en_ulID : -1);
    lua_setglobal(state, SCRIPT_PENCAUSED_ENTITYID);
    
    // Pins
    lua_pushinteger(state, aiSlots[0]);
    lua_setglobal(state, "_slot1");
    
    lua_pushinteger(state, aiSlots[1]);
    lua_setglobal(state, "_slot2");
    
    lua_pushinteger(state, aiSlots[2]);
    lua_setglobal(state, "_slot3");
    
    lua_pushinteger(state, aiSlots[3]);
    lua_setglobal(state, "_slot4");
    
    lua_pushinteger(state, aiSlots[4]);
    lua_setglobal(state, "_slot5");
  }
  
  CTimerValue tv0 = _pTimer->GetHighPrecisionTimer();
  
  // Execute the script.
  result = lua_pcall(state, 0, 0, 0);

  // If error occured during the script execution then print it!
  if (result != 0)
  {
    CTString strError(lua_tostring(state, -1));
    strError.RemovePrefix(_fnmApplicationPath);
    CPrintF("[LUA][ERR] %s\n", strError);
    lua_pop(state, 1);
    return;
  }
  
  lua_close(state);
  
  CTimerValue tv1 = _pTimer->GetHighPrecisionTimer();
  
  if (bDebugMessages) {
    CPrintF("[LUA][INF] Script executed in %.4f s\n", (tv1-tv0).GetSeconds());
  }
}