// Kroni's Classes: objects for exception handling and tracing
// (c) 1997 Wolfgang Kronberg
// file: krtrace.cpp

#define INCL_DOSFILEMGR
#include <os2.h>

#include "krtrace.hpp"

#include <itrace.hpp>
#include <iapp.hpp>
#include <terminat.h>
#include <imsgbox.hpp>


IString KrTrace::fileName = "";
ofstream * KrTrace::lFile = 0;
KrTrace::traceTarget KrTrace::target = KrTrace::queue;
KrTrace::vvf KrTrace::oldFunction = 0;
_KrException * KrTrace::lastException = 0;
KrTrace::userInfo KrTrace::info = noMsg;


void KrTrace::traceTo (const IString & s)
{
  if (target==logFile && fileName==s)
     return;                                     // Nothing to do!

  try {
     if (lFile) delete lFile;
     lFile = new ofstream (s, ios::out|ios::app);
     }
  catch (...) {
     _KRSYSTHROW (IAccessError("Could not open the file " + s + " for writing."));
     }

  HFILE handle = 1;
  APIRET rc = DosDupHandle (lFile->rdbuf()->fd(), &handle);
  if (rc) {
     _KRSYSTHROW (IAccessError("Can't log to the file " + s + " ."));
     }

  ITrace::enableTrace ();
  ITrace::writeToStandardOutput ();

  target = file;
  fileName = s;
};


void KrTrace::traceTo (traceTarget aTarget)
{

  switch (aTarget) {

     case stdout:
        if (file)
           lFile->close();
        ITrace::enableTrace ();
        ITrace::writeToStandardOutput ();
        break;

     case stderr:
        if (file)
           lFile->close();
        ITrace::enableTrace ();
        ITrace::writeToStandardError ();
        break;

     case file:
        if (fileName=="")
           traceTo ();
        else
           traceTo (fileName);
        return;

     case queue:
        if (file)
           lFile->close();
        ITrace::enableTrace ();
        ITrace::writeToQueue ();
        break;

     case none:
        ITrace::disableTrace ();
        break;
     }

  target = aTarget;

};


void KrTrace::traceTo ()
{
  IString filename = IApplication::current().argv(0);
                                                 // Complete filename of the program
  int i = filename.lastIndexOf ('\\');           // Beyond i is the filename without the path
  int j = filename.lastIndexOf ('.');            // i is the position of the last '.' in the filename

  if (j>i)                                       // the filename itself does contain a '.'
     filename = filename.subString (1,j-1);

  filename += ".log";

  if (filename==".log")                          // Just in case that ICurrentApplication.setArgs
     filename = "errors.log";                    //   has not been called

  traceTo (filename);
};


void KrTrace::tellUser (userInfo aInfo)
{
  if (!oldFunction) {
     oldFunction = set_terminate (KrTrace::_terminate);
     };
  info = aInfo;
};


void KrTrace::_throw (const IException & ex, const char* fileName, const char* functionName,
                unsigned long lineNumber)
{
  if (lastException) delete lastException;
  lastException = new _KrException (ex,ex.name());
  lastException->addLocation (IExceptionLocation (fileName, functionName, lineNumber));
  lastException->logExceptionData();
  throw (*lastException);
};


void KrTrace::_throw (const IException & ex, const char* fileName, const char* functionName,
                unsigned long lineNumber, const IString & group)
{
  if (lastException) delete lastException;
  lastException = new _KrException (ex,ex.name());
  lastException->addLocation (IExceptionLocation (fileName, functionName, lineNumber));
  lastException->setErrorCodeGroup(group);
  lastException->logExceptionData();
  throw (*lastException);
};


void KrTrace::_terminate ()
{
  IMessageBox box (0);
  const IExceptionLocation * loc;
  box.setTitle ("Internal Error!");

  IString s = "Sorry, an internal error occurred. I can't continue.\n";

  switch (info)
     {
     case noMsg:
        abort ();
        break;
     case full:
        s+= "Detailed error information:\n";
        if (lastException->textCount())
           s += "  " + IString (lastException->text(0)) + "\n";
        s+= "  Error code: " + IString (lastException->errorCodeGroup()) + " #"
           + IString(lastException->errorId()) +"\n";
        s+= "  Exception class: " + IString(lastException->name()) + "\n";
        loc = lastException->locationAtIndex(0);
        if (loc)
          s+= "  Error location:\n    function: " + IString (loc->functionName()) + "\n    file: "
            + loc->fileName() + ", line" + IString (loc->lineNumber()) + "\n";
     case logFile:
        if (target==file)
           s += "For further information, have a look at the file\n   " + fileName + ".\n";
     case simple:
        s += "Please send a problem report to the author.";
        break;
     };

  box.show (s, IMessageBox::catastrophic);
};



_KrException::_KrException (const IException & ex, const IString & aTypeName)
   : IException (ex)
{
  typeName = aTypeName;
};


const char * _KrException::name () const
{
  return typeName;
};


IException & _KrException::setErrorCodeGroup (ErrorCodeGroup aErrorGroup)
{
  errorGroup = aErrorGroup;                      // this one will be guaranteed to survive.
  return IException::setErrorCodeGroup (errorGroup);
};

