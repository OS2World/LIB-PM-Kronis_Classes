// Kroni's Classes: objects for buffered twodimensional graphics
// (c) 1997 Wolfgang Kronberg
// file: krbwin.cpp


#include "krprint.hpp"
#include "krtrace.hpp"

#define INCL_DOSERRORS                           // Dos error constants
#define INCL_SPL                                 // SplEnumQueue & Co.
#define INCL_SPLERRORS                           // Spl error constants
#define INCL_SPLDOSPRINT                         // PRQ* data structures
#define INCL_DEV                                 // DevEscape & Co.
#include <os2.h>

#include <ithread.hpp>                           // IThread
#include <igrafctx.hpp>                          // IGraphicContext
#include <ipushbut.hpp>                          // IPushButton
#include <iframe.hpp>                            // IFrameWindow
#include <imcelcv.hpp>                           // IMultiCellCanvas
#include <ilistbox.hpp>                          // IListBox
#include <icmdhdr.hpp>                           // ICommandHandler



class _KrPrHandler : public ICommandHandler
{

public:

  _KrPrHandler (IWindow * win, Boolean * res, KrPrinter * prt, IListBox * lb) : ICommandHandler ()
    {window = win; result = res; (*result) = false; printer = prt; listBox = lb;};
                                                 // win: Dialog Window; res: Pointer to result code
                                                 //   (true = OK, false = cancel)
  virtual Boolean command (ICommandEvent & event);

private:

  IWindow * window;
  Boolean * result;
  KrPrinter * printer;
  IListBox * listBox;

};


#define pd_ok       0x1002                       // Identifier for the buttons of the print dialog
#define pd_cancel   0x1003
#define pd_settings 0x1004


Boolean _KrPrHandler::command (ICommandEvent & event)
{
  ULONG devrc;
  HAB hab = IThread::current().anchorBlock();
  PSZ pszPrinter;
  PDRIVDATA pOldDrivData;
  PDRIVDATA pNewDrivData;
  DEVOPENSTRUC dops;
  LONG buflen;
  _KrPrintQueueInfo * queueInfo;
  static IString temp;

  switch (event.commandId ())
     {
     case pd_ok:
        (*result) = true;
        printer->selectedQueue = listBox->selection();
        window->postEvent(WM_CLOSE,0,0);
        return true;

     case pd_cancel:
        window->postEvent(WM_CLOSE,0,0);
        return true;

     case pd_settings:
        queueInfo = &(printer->printQueues[listBox->selection()]);
        temp = IString::remove (queueInfo->driver,queueInfo->driver.indexOf('.'));
        dops.pszDriverName = temp;
        pszPrinter = queueInfo->queueName;
        dops.pdriv = (DRIVDATA*) (queueInfo->driverData);

        buflen = DevPostDeviceModes (hab,
                                    NULL,
                                    dops.pszDriverName,
                                    dops.pdriv->szDeviceName,
                                    pszPrinter,
                                    DPDM_POSTJOBPROP
                                    );

        if (buflen<=0)
           _KRSYSTHROW (IDeviceError("DevPostDeviceModes() failed."));

        if (DosAllocMem((PPVOID)&pNewDrivData,buflen,fALLOC))
           _KRSYSTHROW (IOutOfMemory("Could not allocate memory for printer data."));

        pOldDrivData = dops.pdriv;
        dops.pdriv = pNewDrivData;
        long i;
        i= pOldDrivData->cb;
        if (i>buflen)
           i = buflen;
        memcpy( (PSZ)pNewDrivData, (PSZ)pOldDrivData, i);
        DosFreeMem (pOldDrivData);

        devrc = DevPostDeviceModes (hab,
                                   dops.pdriv,
                                   dops.pszDriverName,
                                   dops.pdriv->szDeviceName,
                                   pszPrinter,
                                   DPDM_POSTJOBPROP
                                   );
        queueInfo->driverData = (_KrDrivData*)(pNewDrivData);
        return true;

     default:
        return false;
     }
};



KrPrinter::KrPrinter ()
{
  initPrintQueues ();
  selectedQueue = -1;

  int i;
  for (i=0; i<numberOfQueues; i++)
     if (printQueues[i].defaultQueue)
        selectedQueue = i;

  graphicsList = NULL;
};

void KrPrinter::initPrintQueues ()
{
  SPLERR splerr;
  ULONG  cbBuf;
  ULONG  cTotal;
  ULONG  cReturned;
  ULONG  cbNeeded;
  ULONG  ulLevel;
  ULONG  i;
  PSZ    pszComputerName ;
  PVOID  pBuf = 0;
  PPRQINFO3 prq ;

  ulLevel = 3L;                             // We only want basic infomation
  pszComputerName = (PSZ)NULL ;             // Required
  splerr = SplEnumQueue (                   // First call only to get required buffer size
             pszComputerName, ulLevel, pBuf,
             0,                             // For this call: buffer length = 0
             &cReturned, &cTotal,
             &cbNeeded,                     // The buffer size we need
             NULL
           );

  if ( splerr == ERROR_MORE_DATA || splerr == NERR_BufTooSmall )
     {
     DosAllocMem (&pBuf, cbNeeded, PAG_READ | PAG_WRITE | PAG_COMMIT);
     printQueues = new _KrPrintQueueInfo [cTotal];

     cbBuf = cbNeeded ;
     splerr = SplEnumQueue (pszComputerName, ulLevel, pBuf, cbBuf, &cReturned, &cTotal, &cbNeeded, NULL);
     if (splerr == NO_ERROR)
        {
        numberOfQueues = cReturned;
        prq = PPRQINFO3 (pBuf);
        for (i=0; i < numberOfQueues; i++, prq++)
            {
            printQueues[i].queueName = prq->pszName;
            printQueues[i].queueProcName = prq->pszPrProc;
            if ( prq->fsType & PRQ3_TYPE_APPDEFAULT )
               printQueues[i].defaultQueue = TRUE;
            else
               printQueues[i].defaultQueue = FALSE;
            printQueues[i].queueDescription = prq->pszComment;
            printQueues[i].driver = prq->pszDriverName;
            DosAllocMem (PPVOID(&(printQueues[i].driverData)), prq->pDriverData->cb,
              PAG_READ | PAG_WRITE | PAG_COMMIT
            );
            memcpy (printQueues[i].driverData, prq->pDriverData, prq->pDriverData->cb);
            };
         }
     else                                        // If error occurred
         _KRSYSTHROW (IAssertionFailure("SplEnumQueue() failed"));
     DosFreeMem (pBuf);
     }
  else                                           // If error code unexpected
     _KRSYSTHROW (IAssertionFailure("SplEnumQueue() failed"));

};


HDC KrPrinter::openPrinterDc (IString title)
{
  if (selectedQueue<0)
     _KRSYSTHROW (IAssertionFailure("Selected Queue negative"));

  HAB hab = IThread::current().anchorBlock();    // Anchor-Block handle
  DEVOPENSTRUC dos;                              // Structure for device open

  dos.pszLogAddress = PSZ(printQueues[selectedQueue].queueName);
                                                 // Print to this logical queue
  dos.pszQueueProcName = PSZ(printQueues[selectedQueue].queueProcName);
                                                 // Use this queue processor
  static IString temp = IString::remove
    (printQueues[selectedQueue].driver,printQueues[selectedQueue].driver.indexOf('.'));
  dos.pszDriverName = temp;                      // Physical driver name, part before '.'
  dos.pdriv = (DRIVDATA*) (&(printQueues[selectedQueue].driverData));
                                                 // Special job properties
  dos.pszDataType = PSZ("PM_Q_STD");             // Standard job (not raw format)
  dos.pszComment = PSZ(title);                   // Title of the job
  dos.pszQueueProcParams = PSZ("");              // Parameters for the queue (e.g. # copies). None yet.
  dos.pszSpoolerParams = PSZ("");                // Parameters for the spooler (e.g. form name). None yet.
  dos.pszNetworkParams = PSZ("");                // We could give our user name here (user=...)

  return DevOpenDC (
           hab,                                  // Always needed
           OD_QUEUED,                            // Open a queue device context
           "*",                                  // OS/2 ignores this
           9L,                                   // We need all 9 entries of the data area
           PDEVOPENDATA(&dos),                   // Data area for opening
           NULLHANDLE                            // We are compatible with the screen
         );

};


Boolean KrPrinter::print (IString title)
{

  hdc = openPrinterDc(title);                    // Open device context for current printer
  if (hdc == DEV_ERROR)
     return FALSE;                               // Error occurred -> we couldn't print it.

  HAB hab = IThread::current().anchorBlock();    // Anchor-Block handle
  SIZEL sizel;
  sizel.cx = 0;                                  // Pagesize -> Null, which gives us the default size
  sizel.cy = 0;                                  //   (which is full size with preserved aspect ratio)

  hps = GpiCreatePS (
          hab,                                   // As always
          hdc,                                   // We assign the just created device context
          &sizel,
          PU_TWIPS | GPIA_ASSOC                  // We want pixels, and we need the hdc (since we set size=0)
        );

  if (hps == GPI_ERROR)
     return FALSE;                               // Error occurred -> we couldn't print it.

  DevEscape (
    hdc,                                         // As always
    DEVESC_STARTDOC,                             // We start a print job
    title.length(), PSZ(title),
    NULL, NULL                                   // These fields are not needed for STARTDOC
  );

  IPresSpaceHandle ips (hps);
  IGraphicContext gc (ips);                      // This is for IOCL compatibility
  if (graphicsList)
     graphicsList -> drawOn (gc);

  DevEscape (
    hdc,                                         // As always
    DEVESC_ENDDOC,                               // We end the job normally
    0, NULL, NULL, NULL                          // None of these is needed for ENDDOC
  );

  GpiDestroyPS (hps);
  DevCloseDC (hdc);                              // Clean up

  return TRUE;                                   // The printing worked!

};


IGList * KrPrinter::setList (IGList & list)
{
  IGList * buffer = graphicsList;
  graphicsList = &list;
  return buffer;
};


void KrPrinter::getArea (IPoint & p1, IPoint & p2)
{
  hdc = openPrinterDc(IString("KrPrinter::getArea temporary"));
                                                 // Open device context for current printer
  if (hdc == DEV_ERROR)
     _KRSYSTHROW (IDeviceError("openPrinterDc failed."));

  p1 = IPoint (0,0);                             // Currently, we don't support anything else

  LONG data[2];                                  // We need 2 LONG words of data area
  BOOL rc;

  rc = DevQueryCaps (
         hdc,                                    // As always
         CAPS_WIDTH,                             // The first element of interest
         2,                                      // We need CAPS_WIDTH and the next element
         data                                    // Data array
       );
  if (!rc)
     _KRSYSTHROW (IDeviceError("DevQueryCaps failed."));

  p2 = IPoint (data[0],data[1]);                 // CAPS_WIDTH and CAPS_HEIGHT

  DevCloseDC (hdc);                              // Clean up.
};


Boolean KrPrinter::printDialog ()
{
  IFrameWindow * frame;
  frame = new IFrameWindow ("Choose a Printer",0x1000);
  IMultiCellCanvas * canvas;
  canvas = new IMultiCellCanvas (IC_FRAME_CLIENT_ID,frame,frame);
  IListBox * listBox;
  listBox = new IListBox (0x1001,canvas,canvas,IRectangle(IPoint(0,0),IPoint(300,100)));
  canvas->addToCell (listBox,2,2,7,1);

  int i;
  for (i=0; i<numberOfQueues; i++)
    listBox->addAsLast (printQueues[i].queueDescription);
  listBox->select (selectedQueue);

  IPushButton * ok;
  ok = new IPushButton (0x1002,canvas,canvas,
             IRectangle(), IPushButton::defaultStyle() | IPushButton::defaultButton);
  ok->setText ("Ok");
  canvas->addToCell (ok,3,4);
  IPushButton * cancel;
  cancel = new IPushButton (0x1003,canvas,canvas);
  cancel->setText ("Cancel");
  canvas->addToCell (cancel,5,4);
  IPushButton * settings;
  settings = new IPushButton (0x1004,canvas,canvas);
  settings->setText ("Settings...");
  canvas->addToCell (settings,7,4);
  canvas->setColumnWidth (1,0,true);
  canvas->setRowHeight (1,0,true);
  canvas->setColumnWidth (9,0,true);
  canvas->setRowHeight (5,0,true);
  Boolean result;
  _KrPrHandler handler (frame,&result,this,listBox);
  handler.handleEventsFor (frame);

  frame->setClient (canvas);
  frame->sizeTo (ISize(400,300));
  frame->setFocus ();
  frame->show ();

  IApplication::current().run();

  delete settings;
  delete cancel;
  delete ok;
  delete listBox;
  delete canvas;
  delete frame;

  return result;
};

