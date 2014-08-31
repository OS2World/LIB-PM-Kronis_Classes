// Kroni's Classes: objects for buffered twodimensional graphics
// (c) 1997 Wolfgang Kronberg
// file: krbwin.cpp


#include "krbwin.hpp"

#define INCL_GPIBITMAPS                          // For GpiQueryDeviceBitmapFormats & Co.
#include <os2.h>

#include <ithread.hpp>                           // IThread
#include <igrafctx.hpp>                          // IGraphicContext
#include <igline.hpp>                            // IGLine



KrMemoryBitmap::KrMemoryBitmap (int aWidth, int aHeight)
  : IGBitmap (IBitmapHandle(createHbm(aWidth, aHeight)))
{
  hdcMem = hdcMemS;
  hpsMem = hpsMemS;
  hbmMem = hbmMemS;
  width = widthS;
  height = heightS;
  graphicsList = 0;
  trans = 0;
};


HBITMAP KrMemoryBitmap::createHbm (SHORT aWidth, SHORT aHeight)
{

  const LONG deviceFormats = 1;                  // Display has only one device format
  const LONG alDataSize = 2*deviceFormats;
  LONG alData [alDataSize];                      // Pair planes / bitCount for this device
  BITMAPINFOHEADER2 bmp;                         // Structure for bitmap on screen
  HAB hab = IThread::current().anchorBlock();    // Anchor-Block handle
  const LONG pszDataSize = 4;                    // So many fields of the data structure are used
  PSZ pszData[pszDataSize] = { "Display", NULL, NULL, NULL };
                                                 // Use "Display" as the device
  SIZEL sizePage = { aWidth, aHeight };          // Page size of the presentation space

  widthS = aWidth;
  heightS = aHeight;

  hdcMemS = DevOpenDC (
    hab,                                         // Always used for this kind of API call
    OD_MEMORY,                                   // Memory Device Context
    "*",                                         // OS/2 ignores this
    pszDataSize, (PDEVOPENDATA) pszData,         // We open indeed the display
    NULLHANDLE                                   // Device Context will be compatible to display
  );

  hpsMemS = GpiCreatePS (
    hab, hdcMemS, &sizePage,
    PU_PELS |                                    // Unit size of the presentation space is one pixel
    GPIA_ASSOC | GPIT_MICRO                      // Needed to create a Micro-PS
  );

  GpiQueryDeviceBitmapFormats(hpsMemS, alDataSize, alData);

  bmp.cbFix = (ULONG) sizeof(BITMAPINFOHEADER2); // Size of the structure
  bmp.cx = widthS;                               // Size of the bitmap
  bmp.cy = heightS;
  bmp.cPlanes = alData[0];                       // Number of bit planes
  bmp.cBitCount = alData[1];                     // Number of bits per pixel within each plane
  bmp.ulCompression = BCA_UNCOMP;                // No compression
  bmp.cbImage = (((widthS * (1 << bmp.cPlanes) * (1 << bmp.cBitCount)) + 31) / 32) * heightS;
                                                 // Size of shown bitmap data in bytes
                                                 //   Organization of bitmaps is padded to DWORD per row
  bmp.cxResolution = 70;                         // One might want to change this
  bmp.cyResolution = 70;
  bmp.cclrUsed = 0;                              // Don't restrict the colors to use by the bitmap
  bmp.cclrImportant = 0;                         // All colors are important
  bmp.usUnits = BRU_METRIC;                      // Measure in pixels per meter
  bmp.usReserved = 0;                            // This is reserved and must be zero
  bmp.usRecording = BRA_BOTTOMUP;                // First logical row is the lowest on screen
  bmp.usRendering = BRH_NOTHALFTONED;            // We don't do any rendering
  bmp.cSize1 = 0;                                // Belongs to usRendering
  bmp.cSize2 = 0;                                // Belongs to usRendering
  bmp.ulColorEncoding = BCE_RGB;                 // This is the default
  bmp.ulIdentifier = 0;                          // Application data

  hbmMemS = GpiCreateBitmap (                    // Actually create the bitmap
    hpsMemS, &bmp,
    FALSE,                                       // We don't want to initialize it
    NULL,NULL                                    // There's no initialization data
  );
  return hbmMemS;
};


SHORT KrMemoryBitmap::widthS, KrMemoryBitmap::heightS;
HDC KrMemoryBitmap::hdcMemS;
HPS KrMemoryBitmap::hpsMemS;
HBITMAP KrMemoryBitmap::hbmMemS;


void KrMemoryBitmap::drawOnBitmap (IGList * list)
{
  GpiSetBitmap (hpsMem, hbmMem);                 // Drawing in hpsMem will now fill the Bitmap

  IPresSpaceHandle ihpsMem (hpsMem);             // We declare the UICL versions of the handles
  IGraphicContext gc (ihpsMem);

  if (list) list->drawOn (gc);                   // We draw the list itself

  GpiSetBitmap (hpsMem, NULLHANDLE);             // The bitmap must be freed again
};


IGList * KrMemoryBitmap::setList (IGList & list)
{
  IGList * rc = graphicsList;                    // Save the old value...
  graphicsList = &list;                          // ...and set the new one

  erase ();                                      // Invalidate anything old
  drawOnBitmap (graphicsList);                   // Draw the new graphics immediately

  return rc;                                     // Return the old list
};


IGList * KrMemoryBitmap::list ()
{
  return graphicsList;
};


void KrMemoryBitmap::addList (IGList & list)
{
  graphicsList->addAsLast (list);                // Add the new list
  drawOnBitmap (&list);                          // Draw the new graphics immediately
};


void KrMemoryBitmap::erase (const IColor & color)
{
  GpiSetBitmap (hpsMem, hbmMem);                 // Drawing in hpsMem will now fill the Bitmap

  // Some secret initializations must be done before calling GpiSetColor.
  //   I don't know them, but the UICL does, so let's draw some dummy using it.

  IPresSpaceHandle ihpsMem (hpsMem);             // We declare the UICL versions of the handles
  IGraphicContext gc (ihpsMem);

  IGList tempList;
  IGLine tempLine (IPoint (0,0), IPoint(1,1));
  tempList.addAsFirst (tempLine);
  tempList.drawOn (gc);                          // Just to draw anything

  // Now, back to work.

  POINTL p = { width, height };
  POINTL o = { 0,0 };
  GpiMove (hpsMem, &o);
  GpiSetColor (hpsMem, color.asRGBLong());
  GpiBox (hpsMem, DRO_FILL, &p, 0, 0);

  GpiSetBitmap (hpsMem, NULLHANDLE);             // The bitmap must be freed again
};


KrCoordSystemTranslator * KrMemoryBitmap::setCoordSystem (KrCoordSystemTranslator * aTrans,
   const KrPoint & aP1, const KrPoint & aP2)
{
  KrCoordSystemTranslator * oldTrans = trans;
  trans = aTrans;
  p1 = aP1;
  p2 = aP2;
  return oldTrans;
};


KrMemoryBitmap & KrMemoryBitmap::drawOn (IGraphicContext & graphicContext)
{
  return drawOn (graphicContext, normal);        // This is this function's definition
};


KrMemoryBitmap & KrMemoryBitmap::drawOn( IGraphicContext & graphicContext, const IPoint& targetBottomLeft,
             const IPoint& targetTopRight, const IPoint& sourceBottomLeft, const IPoint& sourceTopRight,
             long rasterOperation, CompressMode compressMode)
{
  IGBitmap::drawOn (graphicContext, targetBottomLeft, targetTopRight, sourceBottomLeft, sourceTopRight,
     rasterOperation, compressMode);             // Just forward this call to IGBitmap. It does not make
                                                 //   sense to use this with a coordinate system translator.
  return (*this);
};


KrMemoryBitmap & KrMemoryBitmap::drawOn( IGraphicContext& graphicContext, long rasterOperation)
{
  if (trans)
     {
     IPoint dest1 = trans -> translate (p1);
     IPoint dest2 = trans -> translate (p2);
     IPoint source1 (0,0);
     IPoint source2 (width,height);
     drawOn (graphicContext, dest1, dest2, source1, source2, rasterOperation);
     }
  else
     IGBitmap::drawOn (graphicContext, rasterOperation);
  return (*this);
};

