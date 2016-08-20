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

enum EFRelationToPlayers {
  0 FRT_ENEMY   "Enemy",
  1 FRT_NEUTRAL "Neutral",
  2 FRT_ALLY    "Ally",
};

class CEnemyFactionHolder : CEntity {
name      "EnemyFactionHolder";
thumbnail "Thumbnails\\EnemyFactionHolder.tbn";
features  "HasName", "IsTargetable";

properties:

  1 CTString m_strName          "Name" 'N' = "EnemyFactionHolder",
  3 CTString m_strDescription = "",
  
  5 INDEX m_iFactionIndex "Faction Index" = 0,

 10 INDEX m_ulAlliesMask  "Allies Mask" = 0,
 11 INDEX m_ulEnemiesMask "Enemies Mask" = 0,

 15 BOOL m_bDamageFromAllies   "Damage From Allies"  = FALSE,
 //16 BOOL m_bDamageToAllies   "Damage To Allies"    = FALSE,
 17 BOOL m_bDamageFromMembers  "Damage From Members" = FALSE,
 //18 BOOL m_bDamageToMembers  "Damage To Members"   = FALSE,
 19 BOOL m_bDamageFromPlayers  "Damage From Players" = TRUE,
 
 30 enum EFRelationToPlayers m_efrtRelationToPlayers "Relation To Player(s)" = FRT_NEUTRAL,


components:

  1 model   MODEL_FACTION_HOLDER     "Models\\Editor\\EnemyFactionHolder.mdl",
  2 texture TEXTURE_FACTION_HOLDER   "Models\\Editor\\EnemyFactionHolder.tex"


functions:

  const CTString &GetDescription(void) const
  {
    ((CTString&)m_strDescription).PrintF("Faction Index = %d", m_iFactionIndex);
    return m_strDescription;
  }

  BOOL IsIndexValid() {
    if (m_iFactionIndex < 0 || m_iFactionIndex > 31) {
      return FALSE;
    }

    return TRUE;
  }
  
  EFRelationToPlayers GetRelationToFaction(INDEX iFactionIndex) {
    ULONG ulFaction = (1<<iFactionIndex);

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

  // returns bytes of memory used by this object
  SLONG GetUsedMemory(void)
  {
    // initial
    SLONG slUsedMemory = sizeof(CEnemyFactionHolder) - sizeof(CEntity) + CEntity::GetUsedMemory();
    // add some more
    slUsedMemory += m_strName.Length();
    slUsedMemory += m_strDescription.Length();
    return slUsedMemory;
  }


procedures:


  Main()
  {
    if (m_iFactionIndex < 0) { m_iFactionIndex = 0; }
    if (m_iFactionIndex > 31) { m_iFactionIndex = 31; }
  
    InitAsEditorModel();
    SetPhysicsFlags(EPF_MODEL_IMMATERIAL);
    SetCollisionFlags(ECF_IMMATERIAL);

    // set appearance
    SetModel(MODEL_FACTION_HOLDER);
    SetModelMainTexture(TEXTURE_FACTION_HOLDER);

    return;
  }
};

