// Kroni's Classes: Objekte fÅr Zeichnen im mathematischen Koordinatensystem
// (c) 1997 Wolfgang Kronberg
// file: krcstran.cpp


#include "krcstran.hpp"
#include "krtrace.hpp"

#define INCL_WINWINDOWMGR                        // WinWindowFromDC ()
#include <os2.h>

#include <math.h>                                // log ()

#include <igrafctx.hpp>                          // IGraphicContext
#include <ithread.hpp>                           // IThread



KrRectangle::KrRectangle (KrPoint p1, KrPoint p2)
{
if (p1.x < p2.x)
   {
   dLeft = p1.x;
   dRight = p2.x;
   }
else
   {
   dLeft = p2.x;
   dRight = p1.x;
   }

if (p1.y < p2.y)
   {
   dBottom = p1.y;
   dTop = p2.y;
   }
else
   {
   dBottom = p2.y;
   dTop = p1.y;
   }
};


void KrRectangle::setLeft (double d)
{
if (d<dRight)
   dLeft = d;
else
   {
   dLeft = dRight;
   dRight = d;
   }
};


void KrRectangle::setRight (double d)
{
if (d>dLeft)
   dRight = d;
else
   {
   dRight = dLeft;
   dLeft = d;
   }
};


void KrRectangle::setTop (double d)
{
if (d>dBottom)
   dTop = d;
else
   {
   dTop = dBottom;
   dBottom = d;
   }
};


void KrRectangle::setBottom (double d)
{
if (d<dTop)
   dBottom = d;
else
   {
   dBottom = dTop;
   dTop = d;
   }
};



KrCoordSystemTranslator::KrCoordSystemTranslator (const KrPoint & p1, const KrPoint & p2)
{
  if ((p1.x==p2.x) || (p1.y==p2.y))
     _KRSYSTHROW (IInvalidParameter("Neither width nor height may be zero"));

  if (p1.x<p2.x)
     {
     cMin.x = p1.x;
     cMax.x = p2.x;
     }
  else
     {
     cMin.x = p2.x;
     cMax.x = p1.x;
     };

  if (p1.y<p2.y)
     {
     cMin.y = p1.y;
     cMax.y = p2.y;
     }
  else
     {
     cMin.y = p2.y;
     cMax.y = p1.y;
     };

  pMin = IPoint (0,0);
  pMax = IPoint (100,100);
  pAspectRatio = 1;
  align = KrAlignment ();

  xLog = yLog = FALSE;

  fMin = cMin; fMax = cMax;
  frameList = new _KrFrameList ();

  setAspectRatio ();
  setDeviceAspectRatio ();

  setStretches ();
};


void KrCoordSystemTranslator::setStretches ()
{

  double deltaPH, deltaC, orgC, a;

  deltaPH = (pMax.x() - pMin.x()) / (hMax.x - hMin.x);

  if (xLog)
     {
     if (cMin.x<=0)
        _KRSYSTHROW (IInvalidRequest("Can't use nonpositive logarithmic scale on x axis"));
     orgC = log (cMin.x);
     deltaC = log (cMax.x) - orgC;
     }
  else
     {
     orgC = cMin.x;
     deltaC = cMax.x - orgC;
     };

  xStretch = (fMax.x - fMin.x) * deltaPH / deltaC;
  xOrg = pMin.x() + (fMin.x - hMin.x) * deltaPH - orgC * xStretch;

  deltaPH = (pMax.y() - pMin.y()) / (hMax.y - hMin.y);

  if (yLog)
     {
     if (cMin.y<=0)
        _KRSYSTHROW (IInvalidRequest("Can't use nonpositive logarithmic scale on y axis"));
     orgC = log (cMin.y);
     deltaC = log (cMax.y) - orgC;
     }
  else
     {
     orgC = cMin.y;
     deltaC = cMax.y - orgC;
     };

  yStretch = (fMax.y - fMin.y) * deltaPH / deltaC;
  yOrg = pMin.y() + (fMin.y - hMin.y) * deltaPH - orgC * yStretch;

};


void KrCoordSystemTranslator::setDeviceAspectRatio (double ar)
{
  if (ar<0)
     _KRSYSTHROW (IInvalidParameter("Aspect ratio must not be negative."));

  if (ar)
     {
     pAspectRatio = ar;
     iFixedPAspectRatio = TRUE;
     }
  else
     iFixedPAspectRatio = FALSE;
};


double KrCoordSystemTranslator::deviceAspectRatio ()
{
  return pAspectRatio;
};


Boolean KrCoordSystemTranslator::isFixedDeviceAspectRatio ()
{
  return iFixedPAspectRatio;
};


void KrCoordSystemTranslator::assign (const IGraphicContext & graphicContext)
{
  HPS hps = graphicContext.handle();             // Get the presentation space handle
  HDC hdc = GpiQueryDevice (hps);                // Get the device context associated to the graphic context
  HWND hwnd = WinWindowFromDC (hdc);             // Handle to the window associated with the device context

  if (hwnd)                                      // Does such a window exist?
     {
     RECTL rect;                                 // Yes: use its size
     WinQueryWindowRect (hwnd, &rect);
     pMax = IPoint (rect.xRight-rect.xLeft,rect.yTop-rect.yBottom);
     pMin = IPoint (0,0);                        // Currently, we don't support anything else
     }
  else                                           // No: use the general page size
     {
     ISize size = graphicContext.pageSize ();
     pMax = IPoint (size.width(), size.height());
     pMin = IPoint (0,0);                        // Currently, we don't support anything else
     };

  if (!iFixedPAspectRatio)                       // If we don't overrule automatic aspect ratio detection
     {
     LONG data[2];                               // We need two words of data for DevQueryCaps
     BOOL rc;                                    // Return code of DevQueryCaps

     rc = DevQueryCaps (
            hdc,                                 // Always needed for this kind of API call
            CAPS_HORIZONTAL_RESOLUTION,          // The first element of interest
            2,                                   // We need CAPS_HORIZONTAL_RESOLUTION and the next element
            data                                 // Data array
          );
     if (!rc)
        _KRSYSTHROW (IDeviceError("DevQueryCaps failed"));

     pAspectRatio = double(data[0])/double(data[1]);
                                                 // Horizontal / vertical resolution
     }

  setAspectRatio (logicalARatio);
  setStretches();
};


IPoint & KrCoordSystemTranslator::translate (const KrPoint p) const
{
  IPair::Coord x,y;

  if (xLog)
     x = log(p.x)*xStretch+xOrg;
  else
     x = p.x*xStretch+xOrg;

  if (yLog)
     y = log(p.y)*yStretch+yOrg;
  else
     y = p.y*yStretch+yOrg;

  return *(new IPoint (x,y));
};


IRectangle & KrCoordSystemTranslator::translate (const KrRectangle p) const
{
  KrPoint lu = KrPoint (p.left(),p.bottom());
  KrPoint rt = KrPoint (p.right(),p.top());
  return *(new IRectangle (translate(lu),translate(rt)));
};


void KrCoordSystemTranslator::setAspectRatio (double ar)
{
  if (ar<0)
     _KRSYSTHROW (IInvalidParameter("Aspect ratio must not be negative."));

  hMin = cMin; hMax = cMax;
  double tAr = (cMax.x-cMin.x)/(cMax.y-cMin.y) * (pMax.y()-pMin.y())/(pMax.x()-pMin.x()) / pAspectRatio;

  if (ar==0)
     {
     aRatio = tAr;
     logicalARatio = 0;
     iFixedAspectRatio = FALSE;
     }
  else
     {
     aRatio = ar;
     logicalARatio = ar;
     iFixedAspectRatio = TRUE;
     if (ar>tAr)
        {
        double cLen = cMax.x-cMin.x;
        double hLen = cLen * ar/tAr;
        switch (align.horizontal())
           {
           case KrAlignment::left:
              hMax.x = hMin.x + hLen;
              break;
           case KrAlignment::right:
              hMin.x = hMax.x - hLen;
              break;
           case KrAlignment::center:
              double mid = cMin.x + cLen/2;
              hMin.x = mid - hLen/2;
              hMax.x = mid + hLen/2;
              break;
           };
        }
     else
        {
        double cLen = cMax.y-cMin.y;
        double hLen = cLen * tAr/ar;
        switch (align.vertical())
           {
           case KrAlignment::bottom:
              hMax.y = hMin.y + hLen;
              break;
           case KrAlignment::top:
              hMin.y = hMax.y - hLen;
              break;
           case KrAlignment::center:
              double mid = cMin.y + cLen/2;
              hMin.y = mid - hLen/2;
              hMax.y = mid + hLen/2;
              break;
           };
        };
     };
};


void KrCoordSystemTranslator::setAlignment (const KrAlignment & alignment)
{
  align = alignment;
  if (iFixedAspectRatio)
     {
     setAspectRatio (logicalARatio);
     setStretches ();
     };
};


KrAlignment & KrCoordSystemTranslator::alignment ()
{
  return align;
};


double KrCoordSystemTranslator::aspectRatio ()
{
  return aRatio;
};


Boolean KrCoordSystemTranslator::isFixedAspectRatio ()
{
  return iFixedAspectRatio;
};


void KrCoordSystemTranslator::addFrame (const KrPoint & p, double scale)
{
  _KrFrameList * list = frameList;
  while (list->next)
     list = list->next;
  list->fMin = fMin;
  list->fMax = fMax;
  list->next = new _KrFrameList ();

  KrPoint p2;
  p2.x = p.x + scale*(cMax.x-cMin.x);
  p2.y = p.y + scale*(cMax.y-cMin.y);

  double z = (list->fMax.x-list->fMin.x)/(cMax.x-cMin.x);
  fMin.x = (p.x-cMin.x)*z+list->fMin.x;
  fMax.x = (p2.x-cMin.x)*z+list->fMin.x;
  z = (list->fMax.y-list->fMin.y)/(cMax.y-cMin.y);
  fMin.y = (p.y-cMin.y)*z+list->fMin.y;
  fMax.y = (p2.y-cMin.y)*z+list->fMin.y;

  setStretches ();
};


void KrCoordSystemTranslator::removeFrame ()
{
  if (frameList->next)
     {
     _KrFrameList * l1 = frameList;
     _KrFrameList * l2 = frameList->next;
     while (l2->next)
        {
        l1 = l2;
        l2 = l2->next;
        };
     delete l2;
     l1->next = NULL;
     fMin = l1->fMin;
     fMax = l1->fMax;
     }
  else
     _KRSYSTHROW (IInvalidRequest("No more frame present."));
};



KrGraphicList::KrGraphicList (KrCoordSystemTranslator & trans)
  : IGList ()
{
  translator = &trans;
};


KrGraphicList & KrGraphicList::drawOn (IGraphicContext & graphicContext)
{
  translator->assign (graphicContext);

  IGList::drawOn (graphicContext);
  return (*this);
};



KrAlignment::KrAlignment (int h, int v)
{
  setHorizontal (h);
  setVertical (v);
};


void KrAlignment::setHorizontal (int h)
{
  switch (h)
     {
     case right:
     case left:
     case center:
        horiz = h;
        break;
     default:
        _KRSYSTHROW (IInvalidParameter("This is no alignment type."));
        break;
     };
};


void KrAlignment::setVertical (int v)
{
  switch (v)
     {
     case top:
     case bottom:
     case center:
        vert = v;
        break;
     default:
        _KRSYSTHROW (IInvalidParameter("This is no alignment type."));
        break;
     };
};


int KrAlignment::horizontal ()
{
  return horiz;
};


int KrAlignment::vertical ()
{
  return vert;
};



KrDrawingCanvas::KrDrawingCanvas (unsigned long windowIdentifier, IWindow *parent, IWindow *owner,
                   const IRectangle & initial, const Style style)
  : IDrawingCanvas (windowIdentifier, parent, owner, initial, style)
{
  SIZEL size = {0,0};
  HDC hdc = WinOpenWindowDC (handle());
  HPS hps = GpiCreatePS (IThread::current().anchorBlock(), hdc, &size, PU_PELS | GPIT_MICRO | GPIA_ASSOC);
  ip = new IPresSpaceHandle (hps);
  dc = new IGraphicContext (*ip);
  oldGraphicContext = graphicContext ();
  setGraphicContext (dc);
};


KrDrawingCanvas::~KrDrawingCanvas ()
{
  setGraphicContext (oldGraphicContext);
  GpiDestroyPS (*ip);
  delete dc;
  delete ip;
};


