// Kroni's Classes: objects for printing graphics on printers
// (c) 1997 Wolfgang Kronberg
// file: krprint.hpp

// **********************************************************************************************************
//
// defines these classes:
//
//   KrPrinter               Act like a KrDrawingCanvas, but draw on printer and not on screen
//
// defines those global symbols for private use:
//
//   _KrPrintQueueInfo
//   _KrDrivData
//   __KrDrivData
//
// **********************************************************************************************************


#ifndef __KRPRINT_HPP__
#define __KRPRINT_HPP__


#include <iglist.hpp>                            // IGList & Co.
#include <ihandle.hpp>                           // IHandle
#include <istring.hpp>                           // IString



typedef struct __KrDrivData {
long   cb;
long   lVersion;
char   szDeviceName[32];
char   abGeneralData[1];
} _KrDrivData;


class _KrPrintQueueInfo
{
public:
  IString queueName;                             // Name of the logical queue
  IString queueProcName;                         // Name of the queue processor
  IString queueDescription;                      // Title of the queue
  IString driver;                                // Name of the physical device driver
  _KrDrivData * driverData;                      // Tells the driver which options are set
  Boolean defaultQueue;                          // Is this the default queue?
};


class KrPrinter : public IBase
{

public:

  KrPrinter ();                                  // Constructor

  Boolean print (IString title);                 // Do the printing; call the job "title"
  IGList * setList (IGList & list);              // Sets a new graphicList; returns a pointer to the old one.
  void getArea (IPoint & p1, IPoint & p2);       // Gets the area drawing takes place on
                                                 //   (p1=left,low; p2=right,high)
  Boolean printDialog ();                        // Lets the user choose a printer and set options for it.
                                                 //   returns false if the user aborted the dialog,
                                                 //   otherwise true.

private:

  _KrPrintQueueInfo * printQueues;               // Information about all accessable print queues
  int selectedQueue;                             // This is the queue we will print on

  void initPrintQueues ();                       // Initialization of printQueues field
  IHandle::Value openPrinterDc (IString title);  // Opens a device context for the current printer
                                                 //   for job "title"

  IGList * graphicsList;                         // List of graphics elements to print

  int numberOfQueues;                            // Number of all accessable print queues

  IHandle::Value hdc;                            // Printer device context
  IHandle::Value hps;                            // Printer presentation space

  friend class _KrPrHandler;                     // Needs access to printQueues and selectedQueue
};


#endif

