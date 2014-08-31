// Kroni's Classes: Objects for standard OS/2 menues
// (c) 1997 Wolfgang Kronberg
// file: krmenu.cpp


#define INCL_WINSHELLDATA
#include <os2.h>

#include "krmenu.hpp"

#include <ifont.hpp>
#include <istring.hpp>


KrMenuBar::KrMenuBar (IFrameWindow *owner, const Style & style)
  : IMenuBar (owner, style)
{
  char buffer [100];

  PrfQueryProfileString (                        // Get the system font for menus
    HINI_USERPROFILE,                            // Search os2.ini
    "PM_SystemFonts",                            // Get this application...
    "Menus",                                     // ...with this key
    0,                                           // There's no default since this must always exist
    buffer, sizeof(buffer)                       // The buffer for the output of the function
  );

  IString tempStr (buffer);
  int pointPos = tempStr.indexOf ('.');          // Extract point position from "size.name"
  IString name = tempStr.subString(pointPos+1);  // Extract name
  tempStr = tempStr.subString(1,pointPos);       // Extract point size

  setFont (IFont (name, tempStr.asInt()) );      // Now set the font.

};

