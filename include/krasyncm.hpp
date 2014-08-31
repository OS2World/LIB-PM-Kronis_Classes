// Kroni's Classes: Object for asynchronous main function
// (c) 1997 Wolfgang Kronberg
// file: krasyncm.hpp

// **********************************************************************************************************
//
// defines these classes:
//
//   KrAsyncMain             Define a main() function which is independent of the message queue
//
// defines those global symbols for private use:
//
//   _KrEventList
//
// **********************************************************************************************************


#ifndef __KRASYNCM_HPP__
#define __KRASYNCM_HPP__


#include <icmdhdr.hpp>                           // ICommandHandler & Co.
#include <ithread.hpp>                           // IThreadFn & Co.
#include <istring.hpp>                           // IString


class _KrEventList
{
public:
  IEvent * event;
  _KrEventList * next;
};


class KrAsyncMain : virtual public ICommandHandler, virtual public IThreadFn
{

public:

  KrAsyncMain (IWindow * mainWindow);            // Construct this from your main frame window

  typedef enum {cmdStartUp = 1, cmdFuncFinished, cmdUser} commands;
                                                 // All commands reserved for the object's internal use.
                                                 //   Use only cmdUser and higher for your own commands
  virtual void main (IEvent & event) = 0;        // Overwrite this to define your actual new main function

  virtual IString asString() const {return "KrAsyncMain: no Info available";};
  virtual IString asDebugInfo() const {return "KrAsyncMain";};

private:

  virtual Boolean command (ICommandEvent & event);
                                                 // This function starts the thread when needed
  IThread thread;
  virtual void run ();                           // Internal thread function
  IEvent event;                                  // The event passed to main ()
  ISemaphoreHandle sem;                          // handles stop/resume of asynchronous thread

  IWindow * window;
  Boolean threadRunning;                         // Is the thread already running?
  _KrEventList *listStart, *listEnd;             // List of unprocessed messages
  IReference <IThreadFn> funcRef;                // Reference for this object as a thread function

};


#endif

