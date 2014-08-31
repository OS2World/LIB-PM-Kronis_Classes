// Kroni's Classes: Object for Asynchronous main function
// (c) 1997 Wolfgang Kronberg
// file: krasyncm.cpp


#include "krasyncm.hpp"
#include "krtrace.hpp"

#define INCL_DOSSEMAPHORES                       // DosCreateEventSem & Co
#include <os2.h>

#include <iwindow.hpp>                           // IWindow



KrAsyncMain::KrAsyncMain (IWindow * mainWindow)
  : ICommandHandler ()
  , event (mainWindow,0,IEventParameter1(0),IEventParameter2(0))
                                                 // Dummy value, to be overwritten later
  , funcRef (this)
{
  window = mainWindow;
  handleEventsFor (mainWindow);
  threadRunning = false;
  listStart = 0; listEnd = 0;

  HEV semHandle;
  DosCreateEventSem (0,&semHandle,0,0);          // Create a private Semaphore, initially reset
  sem = semHandle;

  thread.setPriority (IApplication::idleTime,0);
  thread.start (funcRef,true);
  window -> postEvent (IWindow::command, IEventParameter1(cmdStartUp));
};


Boolean KrAsyncMain::command (ICommandEvent & event)
{
  _KrEventList * p;

  if (threadRunning)                             // Can't execute this event directly
     {
     if (event.parameter1() == cmdFuncFinished)  // Thread has finished
        {
        if (listEnd)                             // Execute next waiting event
           {
           KrAsyncMain::event = *(listStart->event);
           if (listStart==listEnd)               // This is the last element
              {
              listEnd = 0;
              delete listStart->event;
              delete listStart;
              listStart= 0;
              }
           else                                  // There are still several events waiting
              {
              p = listStart->next;
              delete listStart->event;
              delete listStart;
              listStart = p;
              };
           DosPostEventSem (sem);
           }
        else                                     // No events left, stop processing
           {
           threadRunning = false;                // In case this has not happened already anyway
           }
        }
     else                                        // Thread still running, put event into waiting list
        {
        if (listEnd)                             // There are already elements in the list
           {
           listEnd->next = new _KrEventList;
           listEnd = listEnd->next;
           }
        else                                     // The list is empty
           {
           listStart = new _KrEventList;
           listEnd = listStart;
           };
        listEnd->event = new IEvent (event);
        };
     }
  else                                           // Execute event directly
     {
     if (event.parameter1() == cmdFuncFinished)  // Thread has finished, this can't happen
        _KRSYSTHROW (IInvalidRequest("More threads ended than begun"));
     else                                        // Everything's OK, run the event directly
        {
        KrAsyncMain::event = event;
        threadRunning = true;
        DosPostEventSem (sem);
        };
     };
  return true;
};


void KrAsyncMain::run ()
{
  unsigned long dummy;                           // needed for DosResetEventSem (number of posts to
                                                 //   semaphore)
  while (true)
     {
     DosWaitEventSem (sem,SEM_INDEFINITE_WAIT);
     DosResetEventSem (sem,&dummy);
     main (event);
     window -> postEvent (IWindow::command, IEventParameter1(cmdFuncFinished));
     }
};


