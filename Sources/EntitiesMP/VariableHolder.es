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

21845
%{
  #include "StdH.h"
%}

class CVariableHolder : CEntity {
name      "Variable Holder";
thumbnail "Thumbnails\\VariableHolder.tbn";
features  "HasName", "IsTargetable";

properties:
   1 CTString m_strName          "Name" 'N' = "Variable Holder",
     
   2 INDEX m_i1  "Index 01" = 0,
   3 INDEX m_i2  "Index 02" = 0,
   4 INDEX m_i3  "Index 03" = 0,
   5 INDEX m_i4  "Index 04" = 0,
   6 INDEX m_i5  "Index 05" = 0,
     
  30 FLOAT m_f1  "Float 01" = 0.0F,
  31 FLOAT m_f2  "Float 02" = 0.0F,
  32 FLOAT m_f3  "Float 03" = 0.0F,
  33 FLOAT m_f4  "Float 04" = 0.0F,
  34 FLOAT m_f5  "Float 05" = 0.0F,
     
  60 CTString m_str1  "String 01" = "",
  61 CTString m_str2  "String 02" = "",
  62 CTString m_str3  "String 03" = "",
  63 CTString m_str4  "String 04" = "",
  64 CTString m_str5  "String 05" = "",
     
  90 CEntityPointer m_pen1  "Target 01",
  91 CEntityPointer m_pen2  "Target 02",
  92 CEntityPointer m_pen3  "Target 03",
  93 CEntityPointer m_pen4  "Target 04",
  94 CEntityPointer m_pen5  "Target 05",
     
 120 BOOL m_b1  "Bool 01" = FALSE,
 121 BOOL m_b2  "Bool 02" = FALSE,
 122 BOOL m_b3  "Bool 03" = FALSE,
 123 BOOL m_b4  "Bool 04" = FALSE,
 124 BOOL m_b5  "Bool 05" = FALSE,
     
 135 COLOR m_col1  "Color 01" = COLOR(C_WHITE|0xFF),
 136 COLOR m_col2  "Color 02" = COLOR(C_WHITE|0xFF),

components:
  1 model     MODEL_VARHOLDER   "Models\\Editor\\VariableHolder.mdl",
  2 texture TEXTURE_VARHOLDER   "Models\\Editor\\VariableHolder.tex",

functions:

procedures:
  Main()
  {
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_VARHOLDER);
    SetModelMainTexture(TEXTURE_VARHOLDER);
    return;
  }
};

