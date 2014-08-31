// Kroni's Classes: objects for buffered twodimensional graphics
// (c) 1997 Wolfgang Kronberg
// file: krbwin.hpp

// **********************************************************************************************************
//
// defines these classes:
//
//   KrMemoryBitmap          Define a memory bitmap to do buffered painting on the screen
//
// defines no global symbols for private use.
//
// **********************************************************************************************************


#ifndef __KRBWIN_HPP__
#define __KRBWIN_HPP__


#include "krcstran.hpp"

#include <igbitmap.hpp>                          // IGBitmap



class KrMemoryBitmap : public IGBitmap
{

public:

  KrMemoryBitmap (int aWidth, int aHeight);      // Constructor for bitmap of size _width x _height

  IGList * setList (IGList & list);              // Sets a new graphicList.
                                                 //   Returns a pointer to the old one.
  IGList * list ();                              // Returns a pointer to the current graphicList
  void addList (IGList & list);                  // Adds a new graphicList; draws all new graphics elements.

  void erase (const IColor & color = IGUIColor (IGUIColor::windowBgnd) );
                                                 // Fills the bitmap with the given color
  KrCoordSystemTranslator * setCoordSystem (KrCoordSystemTranslator * aTrans,
                                            const KrPoint & aP1, const KrPoint & aP2);
                                                 // Maps the Bitmap in the rectangle between aP1 and aP2
                                                 //   in the coordinate system aTrans. Returns the old
                                                 //   coordinate system translator.

  virtual KrMemoryBitmap & drawOn (IGraphicContext & graphicContext);
  virtual KrMemoryBitmap & drawOn( IGraphicContext & graphicContext, const IPoint& targetBottomLeft,
             const IPoint& targetTopRight, const IPoint& sourceBottomLeft, const IPoint& sourceTopRight,
             long rasterOperation = normal, CompressMode compressMode = ignore);
  virtual KrMemoryBitmap & drawOn( IGraphicContext& graphicContext, long rasterOperation);
                                                 // Overloads of all IBitmap::drawOn functions

private:

  KrCoordSystemTranslator * trans;               // For the coordinate system the bitmap is drawn into

  IHandle::Value createHbm (short aWidth, short aHeight);
                                                 // Create the Bitmap in the memory.
                                                 //   Creates also device context and the presentation space
                                                 //   as side effects (intended)

  void drawOnBitmap (IGList * list);             // Draw the contents of list on the bitmap

  IGList *graphicsList;                          // List of graphics elements drawn on the bitmap

  KrPoint p1, p2;                                // Corners of the Bitmap in "trans" coordinates.

  short width, height;                           // Size of the bitmap
  IHandle::Value hdcMem;                         // Memory Device Context handle
  IHandle::Value hpsMem;                         // Memory Presentation Space handle
  IHandle::Value hbmMem;                         // Handle to the bitmap itself

  static short widthS, heightS;                  // Temporary values required for initilization
  static IHandle::Value hdcMemS;
  static IHandle::Value hpsMemS;
  static IHandle::Value hbmMemS;

};


#endif

