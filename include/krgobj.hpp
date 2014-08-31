// Kroni's Classes: Verschiedene Grafikobjekte
// (c) 1997 Wolfgang Kronberg
// file: krgobj.hpp

// **********************************************************************************************************
//
// defines these classes:
//
//   KrGLine                 Draw a line using an arbitrary coordinate system
//   KrGString               Draw a string using an arbitrary alignment and an arbitrary coordinate system
//   KrGBox                  Draw a rectangular Box using an arbitrary color and an arbitrary coord. sys.
//   KrFrame                 Map coordinate system into rectangle part of itself
//   KrAntiFrame             Cancels previous KrFrame
//
// defines no global symbols for private use.
//
// **********************************************************************************************************


#ifndef __KRGOBJ_HPP__
#define __KRGOBJ_HPP__


#include "krcstran.hpp"

#include <igline.hpp>                            // IGLine
#include <igrect.hpp>                            // IGRectangle
#include <igstring.hpp>                          // IGString


class KrGLine : public IGLine                    // Object to draw lines
{

public:

  KrGLine (KrCoordSystemTranslator & translator, const KrPoint & startingPoint, const KrPoint & endPoint);
                                                 // Translator may be changed after contruction, but not
                                                 //   its address
  virtual KrGLine & drawOn (IGraphicContext & graphicContext);
                                                 // This will do the actual drawing

private:

  KrPoint start, end;                            // Startpoint and endpoint of line
  KrCoordSystemTranslator * trans;               // Pointer to the associated translator

};



class KrGString : public IGString                // Object to display text at an arbitrary point
{

public:

  KrGString (KrCoordSystemTranslator & translator, const IString & text, const KrPoint & positionPoint);
                                                 // Translator may be changed after contruction, but not
                                                 //   its address
  virtual KrGString & drawOn (IGraphicContext & graphicContext);
                                                 // This will do the actual drawing
  void setAlignment (const KrAlignment & al = KrAlignment());
                                                 // Set alignment of the text relative to positionPoint
  KrAlignment & alignment ();                    // Get the current alignment

private:

  KrPoint stringPosition;                        // Position of text on the screen
  KrCoordSystemTranslator * trans;               // Pointer to the associated translator
  KrAlignment align;                             // Alignment data structure

};



class KrGBox : public IGRectangle                // Object to draw solid boxes
{

public:

  KrGBox (KrCoordSystemTranslator & translator, const KrRectangle & aRectangle, const IColor & aColor);
                                                 // Translator may be changed after contruction, but not
                                                 //   its address
  virtual KrGBox & drawOn (IGraphicContext & graphicContext);
                                                 // This will do the actual drawing

private:

  KrRectangle rectangle;                         // Area of box
  IColor color;                                  // Color of the box
  KrCoordSystemTranslator * trans;               // Pointer to the associated translator

};



class KrFrame : public IGraphic                  // Object to define a frame
{

public:

  KrFrame (KrCoordSystemTranslator & translator, const KrPoint & p, double scale);
                                                 // Construct frame with low/left corner at p which has
                                                 //   edges "scale" times as long as the original window
  virtual KrFrame & drawOn (IGraphicContext & graphicContext);
                                                 // This will be called from a graphics list and activate
                                                 //   the frame
  KrPoint & basePoint ();                        // Returns low/left corner of frame
  double scale ();                               // Returns scale relative to original window

private:

  KrPoint bPoint;                                // Data field for low/left corner of frame
  double dScale;                                 // Data field for scale relative to original window
  KrCoordSystemTranslator * trans;               // Translator to be called to realize the frame

};



class KrAntiFrame : public IGraphic              // Object to remove the previous frame
{

public:

  KrAntiFrame (KrCoordSystemTranslator & translator);
                                                 // Removes the most recently activated frame
  virtual KrAntiFrame & drawOn (IGraphicContext & graphicContext);
                                                 // This will be called from a graphics list and remove
                                                 //   the frame
private:

  KrCoordSystemTranslator * trans;               // Translator to be called to remove the frame

};


#endif

