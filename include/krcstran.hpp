// Kroni's Classes: Objekte fÅr Zeichnen im mathematischen Koordinatensystem
// (c) 1997 Wolfgang Kronberg
// file: krcstran.hpp

// **********************************************************************************************************
//
// defines these classes:
//
//   KrPoint                 Define a point in two dimensional space with floating point coordinates
//   KrRectangle             Define a rectangle with floating point coordinates
//   KrAlignment             Define how to align objects relative to available space
//   KrCoordSystemTranslator Define a coordinate system and map it to any output device drawing area
//   KrGraphicList           Enhances IGraphicList to be compatible with KrCoordSystemTranslator
//   KrDrawingCanvas         Enhances IDrawingCanvas to be compatible with KrCoordSystemTranslator
//
// defines those global symbols for private use:
//
//   _KrFrameList
//
// **********************************************************************************************************


#ifndef __KRCSTRAN_HPP__
#define __KRCSTRAN_HPP__


#include <idrawcv.hpp>                           // IDrawingCanvas & Co.
#include <iglist.hpp>                            // IGList



class KrPoint                                    // Point with mathematical (floating point) coordinates
{
public:
  KrPoint (double _x, double _y) {x=_x;y=_y;};
  KrPoint () {x=y=0;};
  double x;
  double y;
};


class KrRectangle                                // Rectangle with mathematical (floating point) coordinates
{

public:

  KrRectangle (KrPoint p1, KrPoint p2);
  KrRectangle () {dLeft=dRight=dTop=dBottom=0;};

  double width () {return dRight-dLeft;};
  double height () {return dTop-dBottom;};

  double left () const {return dLeft;};
  double right () const {return dRight;};
  double top () const {return dTop;};
  double bottom () const {return dBottom;};

  void setLeft (double d);
  void setRight (double d);
  void setTop (double d);
  void setBottom (double d);

private:

  double dLeft, dRight, dTop, dBottom;

};


class _KrFrameList
{
public:
  _KrFrameList () {next=0;};
  KrPoint fMin;
  KrPoint fMax;
  _KrFrameList * next;
};


class KrAlignment
{

public:

  KrAlignment (int h = KrAlignment::center, int v = KrAlignment::center);
  void setHorizontal (int h);
  void setVertical (int v);
  int horizontal ();
  int vertical ();

  typedef enum { center = 1, right, left, top, bottom } align;

private:

  int horiz, vert;

};


class KrCoordSystemTranslator : public IBase
{

public:

  KrCoordSystemTranslator (const KrPoint & p1 = KrPoint(1,1), const KrPoint & p2 = KrPoint(0,0));
                                                 // Defines a coordinate system with corners p1 and p2
                                                 //   If p2 is left out: between p1 and (0,0)
                                                 //   If no parameter is given: between (0,0) and (1,1)
                                                 //   Initial physical draw area: (0,1000),(0,1000)
                                                 // Note: numerically unstable choices will result
                                                 //   in undefined behaviour

  void assign (const IGraphicContext & printer); // Get physical draw area

  IPoint & translate (const KrPoint p) const;    // Translate logical units to physical units
  IRectangle & translate (const KrRectangle p) const;

  void setAspectRatio (double ar=0);             // ar=0 -> automatic adjustment to full size
                                                 //  Meaning of aspect ratio value ar:
                                                 //  take a square coordinate  system with
                                                 //  y-range 0..1 and x-range 0..ar.
  double aspectRatio ();                         // Returns current value of aspect ratio
  Boolean isFixedAspectRatio ();                 // True -> setAspectRatio has been used with ar != 0

  void setDeviceAspectRatio (double ar=0);       // Overrules automatic detection of output device
                                                 //   aspect ratio, unless ar=0
  double deviceAspectRatio ();                   // Gets the aspect ratio of the device
  Boolean isFixedDeviceAspectRatio ();           // Did we overrule automatic detetion of device aspect
                                                 //   ratio?

  void addFrame (const KrPoint & p, double scale);
                                                 // Add a frame with left low corner p and relative size
                                                 //   "scale". All subsequent drawing will be relative
                                                 //   to the frame instead of relative to the whole area.
                                                 //   This includes further calls of addFrame itself.
                                                 //   Logarithmic scales will be ignored when placing
                                                 //   the frame, but within the frame the scale will be
                                                 //   logarithmic again.
  void removeFrame ();                           // Restore the status before the previous addFrame()

  void setAlignment (const KrAlignment & alignment);
                                                 // If fixed aspect ratio is used, the coordinate system will
                                                 //   cover only part of the device/window. This function
                                                 //   fixes how the system will be aligned on the device.
  KrAlignment & alignment ();                    // Gets the current alignment

private:

  void setStretches ();                          // Set xStretch, yStretch, xOrg, yOrg

  KrPoint cMin, cMax;                            // Corners (left,low) and (right,high) of coordinate system
  IPoint pMin, pMax;                             // Corners (minimal values) and (maximal values) of
                                                 //   physical device area to paint on
  KrPoint hMin, hMax;                            // Corners of hidden (for aRatio expanded) coordinate system
  KrPoint fMin, fMax;                            // Corners of frame
  double xStretch, yStretch, xOrg, yOrg;         // Cached temporary variables for performance improvement
  Boolean xLog, yLog;                            // Indicates logarithmic coordinate systems

  double aRatio;                                 // Current aspect ratio
  double logicalARatio;                          // Same as aRatio if aspect ratio is fixed; 0 otherwise.
  Boolean iFixedAspectRatio;                     // Is aspect ratio fixed?
  Boolean iFixedPAspectRatio;                    // Is device aspect ratio fixed?
  double pAspectRatio;                           // Physical aspect ratio

  _KrFrameList * frameList;                      // Lists all current and previous frames

  KrAlignment align;                             // Alignment of coordinate system on device in case of
                                                 //   fixed aspect ratio
};


class KrGraphicList : public IGList              // This is only technical; I hope to get rid of it sometime
{

public:

  KrGraphicList (KrCoordSystemTranslator & trans);

  virtual KrGraphicList & drawOn (IGraphicContext & graphicContext);

private:

  KrCoordSystemTranslator * translator;

};


class KrDrawingCanvas : public IDrawingCanvas    // This is only technical; I hope to get rid of it sometime
{

public:

  KrDrawingCanvas (unsigned long windowIdentifier, IWindow *parent, IWindow *owner,
                   const IRectangle & initial = IRectangle(), const Style style = defaultStyle ());
                                                 // Constructor, syntax identical to IDrawingCanvas
  ~KrDrawingCanvas ();                           // Destructor

private:

  IHandle::Value hps;                            // Handle to the presentation space we will draw on
  IPresSpaceHandle * ip;                         // UICL object representating hps
  IGraphicContext * dc;                          // UICL object representating ip
  IGraphicContext * oldGraphicContext;           // graphics context used by IDrawingCanvas

};


#endif

