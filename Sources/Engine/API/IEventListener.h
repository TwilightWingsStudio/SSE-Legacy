/* Copyright (c) 2017 by ZCaliptium

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

#ifndef SE_INCL_IEVENTLISTENER_H
#define SE_INCL_IEVENTLISTENER_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

enum EEventType
{
  EET_INTEGER_EVENT,
  EET_FLOAT_EVENT,
  EET_GUI_EVENT,
};

enum EGuiEventType
{
  EGET_TRIGGERED,
  EGET_PRECHANGE,
  EGET_CHANGED,
};

class IGuiComponent;

// Structure which represents an event.
struct SEvent
{
  // Simple event which holds s32 value.
  struct SIntegerEvent
  {
    INDEX Value;
  };
  
  // Simple event which holds floating point value.
  struct SFloatEvent
  {
    FLOAT Value;
  };
  
  // Universal GUI event.
  struct SGuiEvent
  {
    IGuiComponent *Caller;
    IGuiComponent *Target;
    EGuiEventType EventType;
    INDEX IntValue;
  };
  
  // Event type stored here.
  EEventType EventType;
  
  // Data Block.
  union
  {
    struct SIntegerEvent IntegerEvent;
    struct SFloatEvent FloatEvent;
    struct SGuiEvent GuiEvent;
  };
};

// Interface for of an object which can receive events.
class IEventListener
{
  public:
    // Destructor.
    virtual ~IEventListener() {}
    
    // Returns TRUE if event was processed.
    virtual BOOL OnEvent(const SEvent& event) = 0;
};

#endif