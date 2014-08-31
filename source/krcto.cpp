// Kroni's Classes: Output window for common text output
// (c) 1997 Wolfgang Kronberg
// file: krcto.cpp


#include "krcto.hpp"

#include <istring.hpp>                           // IString



KrCommonTextOutput::KrCommonTextOutput (unsigned long id, IWindow* parent, IWindow* owner,
                                        const IRectangle& initial, const Style& style )
  : IMultiLineEdit (id, parent, owner, initial, style)
{
  ostrbuf = new _KrStreambuf (buffer, sizeof(buffer),this);
  outputStream = new ostream (ostrbuf);
  outputStream->setf(ios::unitbuf);
};


ostream & KrCommonTextOutput::stream ()
{
  return (*outputStream);
};



_KrStreambuf::_KrStreambuf (char* c, int len, KrCommonTextOutput* top)
  : streambuf (c,len)
{
  textOutputArea = top;
};


int _KrStreambuf::overflow (int c)
{
  if (out_waiting () > 0)
     {
       IString s (pbase(), out_waiting());
       int i;
       IString t;
       while (i = s.indexOf ('\n') )
          {
          if (i>1)
             t = s.subString (1,i-1);
          else
             t = "";
          textOutputArea->add (t,t.length());
          textOutputArea->addLineAsLast (IString(""));
          textOutputArea->setCursorLinePosition(textOutputArea->numberOfLines());
          if (i<s.length())
             s = s.subString (i+1,s.length()-i);
          else
             s = "";
          };
       if (s.length())
          textOutputArea->add (s,s.length());
     };
  setp (pbase(),epptr());
  if (c!=EOF)
     {
       sputc (c);
     };
  return EOF+1;                                  // No error -> don't return EOF.
};


