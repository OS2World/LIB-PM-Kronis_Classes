// Kroni's Classes: Objects for standard OS/2 menues
// (c) 1997 Wolfgang Kronberg
// file: krmenu.hpp

// **********************************************************************************************************
//
// defines these classes:
//
//   KrMenuBar               Define a main() function which is independent of the message queue
//
// **********************************************************************************************************


#ifndef __KRMENU_HPP__
#define __KRMENU_HPP__


#include <imenubar.hpp>


class KrMenuBar : public IMenuBar
{

public:

  KrMenuBar (IFrameWindow *owner, const Style & style = defaultStyle());
                                                 // This constructor is changed to load the correct font
  KrMenuBar (const IResourceId & menuResId, IFrameWindow *owner) : IMenuBar (menuResId, owner) {};
                                                 // This constructor is just forwarded
};


#endif

