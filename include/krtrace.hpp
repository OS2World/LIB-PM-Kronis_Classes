// Kroni's Classes: objects for exception handling and tracing
// (c) 1997 Wolfgang Kronberg
// file: krtrace.hpp

// **********************************************************************************************************
//
// defines these classes:
//
//   KrTrace                 Analogous to ITrace, this class provides support for error reporting
//
// defines these macros:
//
//   KRTHROW (ex)            Like ITHROW, but also works with exceptions declared as temporaries
//   KRNAMEDTHROW (ex,name)  Like KRTHROW, but sets the exception group name to "name"
//
// defines those global symbols for private use:
//
//   _KrException
//   _KRSYSTHROW (ex)
//
// **********************************************************************************************************


#ifndef __KRTRACE_HPP__
#define __KRTRACE_HPP__


#include <fstream.h>

#include <iexcbase.hpp>
#include <ivbase.hpp>
#include <istring.hpp>


class _KrException : public IException
{

public:

  _KrException (const IException & ex, const IString & aTypeName);
                                                 // Standard Constructor

  virtual const char * name () const;
  IException & setErrorCodeGroup (ErrorCodeGroup aErrorGroup);

private:

  IString typeName;
  IString errorGroup;

};


class KrTrace : public IBase
{

public:

  typedef void (*vvf)();                         // Defines vvf as the type of a function void -> void

  enum traceTarget {stdout, stderr, file, queue, none};
  enum userInfo {noMsg, simple, logFile, full};  // What kind of MessageBox should inform the user
                                                 //   about any kind of unexpected exception?

  static void traceTo (traceTarget aTarget);     // To which destination should be traced?
  static void traceTo (const IString & s);       // Trace to the file with this name
  static void traceTo ();                        // Trace to progname.log
  static void tellUser (userInfo aInfo);         // How detailed should the user be informed?

  static void _throw (
    const IException & ex, const char* fileName, const char* functionName,
    unsigned long lineNumber);

  static void _throw (
    const IException & ex, const char* fileName, const char* functionName,
    unsigned long lineNumber, const IString & group);


private:

  KrTrace () {};                                 // Private default constructor, so that no objects
                                                 //   of this class can be constructed

  static void _terminate ();                     // Replaces the "common" terminate() function

  static _KrException * lastException;
  static vvf oldFunction;
  static traceTarget target;
  static userInfo info;
  static IString fileName;
  static ofstream * lFile;

};


#define KRTHROW(exc)\
          KrTrace::_throw (exc,__FILE__,__FUNCTION__,__LINE__)
                                                 // Use this to simply throw an exception

#define KRNAMEDTHROW(exc,name)\
          KrTrace::_throw (exc,__FILE__,__FUNCTION__,__LINE__,name)
                                                 // Use this to throw exceptions of your own group

#define _KRSYSTHROW(exc)\
          KRNAMEDTHROW (exc,"Kroni's Classes")   // Used to throw exceptions within Kroni's Classes

#endif

