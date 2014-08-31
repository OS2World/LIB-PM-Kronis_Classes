// Kroni's Classes: Verschiedene Grafikobjekte
// (c) 1997 Wolfgang Kronberg
// file: krgobj.hpp


#include "krgobj.hpp"
#include "krtrace.hpp"

#define INCL_GPIPRIMITIVES                       // GpiSetTextAlignment & Co.
#include <os2.h>

#include <igrafctx.hpp>                          // IGraphicContext



KrGLine::KrGLine (KrCoordSystemTranslator & translator,
                const KrPoint & startingPoint, const KrPoint & endPoint)
  : IGLine (IPoint(0,0),IPoint(1,1))             // Just to initialize the structure somehow
{
  trans = &translator;
  start = startingPoint;
  end = endPoint;
  IGLine::setStartingPoint (trans->translate(start));
  IGLine::setEndingPoint (trans->translate(end));
};


KrGLine & KrGLine::drawOn (IGraphicContext & graphicContext)
{
  IGLine::setStartingPoint (trans->translate(start));
  IGLine::setEndingPoint (trans->translate(end));
  IGLine::drawOn (graphicContext);
  return (*this);
};



KrGString::KrGString (KrCoordSystemTranslator & translator, const IString & text,
     const KrPoint & positionPoint)
  : IGString (text,IPoint(0,0))
{
  trans = &translator;
  stringPosition = positionPoint;
  setAlignment ();
  IGString::moveTo (trans->translate(stringPosition));
};


void KrGString::setAlignment (const KrAlignment & al)
{
  align = al;
};


KrAlignment & KrGString::alignment ()
{
  return align;
};


KrGString & KrGString::drawOn (IGraphicContext & graphicContext)
{

  IGString::moveTo (trans->translate(stringPosition));

  LONG lHoriz = 0, lVert = 0;                    // Gpi variables for alignment

  switch (align.horizontal())
     {
     case KrAlignment::left:
        lHoriz = TA_LEFT;
        break;
     case KrAlignment::right:
        lHoriz = TA_RIGHT;
        break;
     case KrAlignment::center:
        lHoriz = TA_CENTER;
        break;
     };

  switch (align.vertical())
     {
     case KrAlignment::top:
        lVert = TA_TOP;
        break;
     case KrAlignment::bottom:
        lVert = TA_BOTTOM;
        break;
     case KrAlignment::center:
        lVert = TA_HALF;
        break;
     };


  if (!GpiSetTextAlignment (graphicContext.handle(),lHoriz,lVert))
     _KRSYSTHROW (IAssertionFailure("GpiSetTextAlignment failed"));


  IGString::drawOn (graphicContext);
  return (*this);

};



KrGBox::KrGBox (KrCoordSystemTranslator & translator, const KrRectangle & aRectangle, const IColor & aColor)
  : IGRectangle ()
  , color (aColor)
{
  trans = &translator;
  rectangle = aRectangle;
};


KrGBox & KrGBox::drawOn (IGraphicContext & graphicContext)
{
  IGRectangle::setEnclosingRect (trans->translate(rectangle));

  IGraphicBundle gb (graphicContext);            // save current attributes
  graphicContext.setDrawOperation (IGraphicBundle::fill);
  graphicContext.setPenWidth (0);
  graphicContext.setFillPattern (IGraphicBundle::filled);
  graphicContext.setFillColor (color);

  IGRectangle::drawOn (graphicContext);

  graphicContext.setGraphicBundle (gb);

  return (*this);
};


KrFrame::KrFrame (KrCoordSystemTranslator & translator, const KrPoint & p, double scale)
{
  trans = &translator;
  bPoint = p;
  dScale = scale;
};


KrFrame & KrFrame::drawOn (IGraphicContext & graphicContext)
{
  trans -> addFrame (bPoint, dScale);
  return (*this);
};


KrPoint & KrFrame::basePoint ()
{
  return bPoint;
};

double KrFrame::scale ()
{
  return dScale;
};



KrAntiFrame::KrAntiFrame (KrCoordSystemTranslator & translator)
{
  trans = &translator;
};


KrAntiFrame & KrAntiFrame::drawOn (IGraphicContext & graphicContext)
{
  trans -> removeFrame ();
  return (*this);
};

