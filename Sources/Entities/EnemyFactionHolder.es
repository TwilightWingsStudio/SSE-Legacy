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

2029
%{
  #include "StdH.h"
%}

enum EFHIndex {
   0 EFHI_00 "Faction [00]",
   1 EFHI_01 "Faction [01]",
   2 EFHI_02 "Faction [02]",
   3 EFHI_03 "Faction [03]",
   4 EFHI_04 "Faction [04]",
   5 EFHI_05 "Faction [05]",
   6 EFHI_06 "Faction [06]",
   7 EFHI_07 "Faction [07]",
   8 EFHI_08 "Faction [08]",
   9 EFHI_09 "Faction [09]",
 
  10 EFHI_10 "Faction [10]",
  11 EFHI_11 "Faction [11]",
  12 EFHI_12 "Faction [12]",
  13 EFHI_13 "Faction [13]",
  14 EFHI_14 "Faction [14]",
  15 EFHI_15 "Faction [15]",
  16 EFHI_16 "Faction [16]",
  17 EFHI_17 "Faction [17]",
  18 EFHI_18 "Faction [18]",
  19 EFHI_19 "Faction [19]",

  20 EFHI_20 "Faction [20]",
  21 EFHI_21 "Faction [21]",
  22 EFHI_22 "Faction [22]",
  23 EFHI_23 "Faction [23]",
  24 EFHI_24 "Faction [24]",
  25 EFHI_25 "Faction [25]",
  26 EFHI_26 "Faction [26]",
  27 EFHI_27 "Faction [27]",
  28 EFHI_28 "Faction [28]",
  29 EFHI_29 "Faction [29]",

  30 EFHI_30 "Faction [30]",
  31 EFHI_31 "Faction [31]",
};

enum EFactionRelation {
  0 FRT_ENEMY   "Enemy [0]",
  1 FRT_NEUTRAL "Neutral [1]",
  2 FRT_ALLY    "Ally [2]",
};

class CEnemyFactionHolder : CEntity {
name      "CEnemyFactionHolder";
thumbnail "Thumbnails\\EnemyFactionHolder.tbn";
features  "HasName", "IsTargetable";

properties:

  1 CTString m_strName          "Name" 'N' = "EnemyFactionHolder",
  3 CTString m_strDescription = "",
  
  5 enum EFHIndex m_iFactionIndex "Faction Index" = EFHI_00,

 10 INDEX m_ulAlliesMask  "Allies Mask" = 0,
 11 INDEX m_ulEnemiesMask "Enemies Mask" = 0,

 15 BOOL m_bDamageFromAllies   "Damage From Allies"  = FALSE,
 17 BOOL m_bDamageFromMembers  "Damage From Members" = FALSE,
 19 BOOL m_bDamageFromPlayers  "Damage From Players" = TRUE,
 
 30 enum EFactionRelation m_eRelationToPlayers "Relation To Player(s)" = FRT_NEUTRAL,
 
 40 BOOL m_bCode          "Output as Script" = FALSE,

components:

  1 model   MODEL_FACTION_HOLDER     "Models\\Editor\\EnemyFactionHolder.mdl",
  2 texture TEXTURE_FACTION_HOLDER   "Models\\Editor\\EnemyFactionHolder.tex"

functions:

  // --------------------------------------------------------------------------------------
  // Returns short entity description to show it in SED.
  // --------------------------------------------------------------------------------------
  const CTString &GetDescription(void) const
  {
    ((CTString&)m_strDescription).PrintF("Faction Index = %d", m_iFactionIndex);

    return m_strDescription;
  }

  // --------------------------------------------------------------------------------------
  // Returns TRUE if current faction index is valid.
  // --------------------------------------------------------------------------------------
  BOOL IsIndexValid()
  {
    if (m_iFactionIndex < 0 || m_iFactionIndex > 31) {
      return FALSE;
    }

    return TRUE;
  }
  
  // --------------------------------------------------------------------------------------
  // Returns relation to specified faction.
  // --------------------------------------------------------------------------------------
  EFactionRelation GetRelationToFaction(INDEX iFactionIndex)
  {
    ULONG ulFaction = (1<<INDEX(iFactionIndex));

    BOOL bAlly = (m_ulAlliesMask&ulFaction);
    BOOL bEnemy = (m_ulEnemiesMask&ulFaction);

    //CPrintF("Ally: %d Enemy %d\n", bAlly, bEnemy);

    // If ally and non-enemy.
    if (bAlly && !bEnemy) {
      return FRT_ALLY;
    }

    // If enemy and non-ally.
    if (bEnemy && !bAlly) {
      return FRT_ENEMY;
    }

    // If enemy and ally at the same time.
    return FRT_NEUTRAL;
  }

  // --------------------------------------------------------------------------------------
  // Returns bytes of memory used by this object.
  // --------------------------------------------------------------------------------------
  SLONG GetUsedMemory(void)
  {
    // Initial.
    SLONG slUsedMemory = sizeof(CEnemyFactionHolder) - sizeof(CEntity) + CEntity::GetUsedMemory();
    
    // Add some more.
    slUsedMemory += m_strName.Length();
    slUsedMemory += m_strDescription.Length();

    return slUsedMemory;
  }
  
  // --------------------------------------------------------------------------------------
  // Shows dialog wnd with relations list.
  // --------------------------------------------------------------------------------------
  void ShowInfoDialog()
  {
    CTString strCode;
    strCode.PrintF("Faction Index = [%0d]\n\n", m_iFactionIndex);
    
    for (INDEX i = 0; i < 32; i++)
    {
      EFactionRelation eRelation = GetRelationToFaction(i);
      
      CTString strTemp;
      strTemp.PrintF("[%02d] = ", i);
      
      strCode += strTemp;
      
      if (eRelation == FRT_ENEMY) {
        strCode += "Enemy\n";
      } else if (eRelation == FRT_NEUTRAL) {
        strCode += "Neutral\n";
      } else {
        strCode += "Friend\n";
      }
    }

    WarningMessage(strCode);
  }

procedures:

  // --------------------------------------------------------------------------------------
  // The entry point.
  // --------------------------------------------------------------------------------------
  Main()
  {
    // Clamp to limits.
    if (m_iFactionIndex < EFHI_00) { m_iFactionIndex = EFHI_00; }
    if (m_iFactionIndex > EFHI_31) { m_iFactionIndex = EFHI_31; }
    
    // If should display info dialog.
    if (m_bCode)
    {
      m_bCode = FALSE;
      
      ShowInfoDialog();
    }
  
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_FACTION_HOLDER);
    SetModelMainTexture(TEXTURE_FACTION_HOLDER);

    return;
  }
};

