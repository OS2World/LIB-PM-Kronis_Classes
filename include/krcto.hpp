// Kroni's Classes: Output window for common text output
// (c) 1997 Wolfgang Kronberg
// file: krcto.hpp

// **********************************************************************************************************
//
// defines these classes:
//
//   KrCommonTextOutput      Define a window to keep stream output to screen
//
// defines those global symbols for private use:
//
//   _KrStreambuf
//
// **********************************************************************************************************


#ifndef __KRCTO_HPP__
#define __KRCTO_HPP__


#include <iostream.h>                            // ostream & Co.

#include <imle.hpp>                              // IMultiLineEdit & Co.



class KrCommonTextOutput;                        // Forward for kr_streambuf

class _KrStreambuf : public streambuf
{

public:

  _KrStreambuf (char* c, int len, KrCommonTextOutput* top);

  virtual int overflow (int c=EOF);

private:

  KrCommonTextOutput* textOutputArea;

};


class KrCommonTextOutput : public IMultiLineEdit
{

public:

  KrCommonTextOutput (unsigned long id, IWindow* parant, IWindow* owner,
                      const IRectangle& initial = IRectangle (),
                      const Style& style = defaultStyle () | readOnly);

  ostream & stream ();

private:

  ostream * outputStream;
  _KrStreambuf* ostrbuf;
  char buffer [200];

};

#endif

