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

#include <luajit/src/lua.hpp>
#include <luajit/src/lualib.h>
#include <luajit/src/lauxlib.h>

#include <Engine/Scripts/ScriptEngine_internal.h>

static int l_sebase_print(lua_State* L)
{
  INDEX ctArgs = lua_gettop(L);
  
  CTString strResult;

  for (int i = 1; i <= ctArgs; i++) {
    strResult.PrintF("%s%s", strResult, lua_tostring(L, i));
  }
  
  //CPrintF("print() redefinition!\n");
  CPrintF("[LUA][OUT] %s\n", strResult);

  return 0;
}

static const struct luaL_Reg sebaseLib [] = {
  {"print", l_sebase_print},
  {NULL, NULL} /* end of array */
};

void luaopen_luasebaselib(lua_State *L)
{
  lua_getglobal(L, "_G");
  luaL_register(L, NULL, sebaseLib);
  lua_pop(L, 1);
}
