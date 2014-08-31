// Kroni's Classes: objects for reading and writing ini-file data
// (c) 1997 Wolfgang Kronberg
// file: krprof.cpp


#include "krprof.hpp"

#define INCL_WINSHELLDATA                        // Prf* API functions
#include <os2.h>

#include <ithread.hpp>                           // To get the anchor block handle



_KrProfileBuf::_KrProfileBuf (_KrProfileBase & aParent)
  : streambuf ()
{
  parent = &aParent;
  buffer = new _KrProfileBufCons (*parent);
  theStream = new iostream (buffer);
  theStream->flags (ios::dec);
  theStream->clear();
  unbuffered(0);
  allocate ();
};


_KrProfileBuf::~_KrProfileBuf ()
{
  delete theStream;
  delete buffer;
};


void _KrProfileBuf::renewBuffer ()
{
  delete theStream;
  delete buffer;
  buffer = new _KrProfileBufCons (*parent);
  theStream = new iostream (buffer);
  theStream->flags (ios::dec);
  theStream->clear();
};


int _KrProfileBuf::overflow (int c)
{
  if (out_waiting())
     {
     theStream->write (pbase(), out_waiting());
     theStream->flush();
     }
  setp (pbase(),epptr());
  if (c!=EOF)
     {
       sputc (c);
     };
  return EOF+1;                                  // No error -> don't return EOF.
};


int _KrProfileBuf::doallocate ()
{
  const int buffsize = 100;                      // size of each of the buffers
  char * buff;
  buff = new char [2*buffsize];                  // create get & put buffer
  setg (buff+buffsize,buff+buffsize,buff+buffsize);
                                                 // empty get area & putbackarea
  setp (buff+buffsize, buff+2*buffsize);         // second half available for put area
  return 0;
};


int _KrProfileBuf::underflow ()
{
  if (in_avail())
     return *gptr();

  int c;
  int i = buffer->in_avail();
  if (!i)                                        // No input waiting -> try to get one char
     {
     c = theStream->get();
     if (c==EOF)                                 // No more input possible
        return c;
     *(pbase()-1) = (char)c;                     // Put the char at the end of the possible get area
     setg (pbase()-1, pbase()-1, pbase());       // Set get area as one char big
     }
  else
     {
     if (i>pbase()-base())                       // Possible input larger than available get area?
        i = pbase()-base();
     theStream->read (pbase()-i, i);             // Get as much as possible
     setg (pbase()-i, pbase()-i, pbase());       // Gotten area = new get area, putback area = empty
     }
  return *gptr();
};


int _KrProfileBuf::sync()
{
  return overflow();
};


_KrProfileBufCons * _KrProfileBuf::buf()
{
  return buffer;
};


iostream * _KrProfileBuf::stream()
{
  return theStream;
};



_KrProfileBufCons::_KrProfileBufCons (_KrProfileBase & aParent)
  : strstreambuf ()
{
  parent = &aParent;
};


int _KrProfileBufCons::overflow (int c)
{
  int t = strstreambuf::overflow(c);
  parent->writeData (false);
  return t;
};


int _KrProfileBufCons::underflow ()
{
  if (!pcount())                                 // only try to read input from ultimate producer
                                                 //   if string is empty
     parent->readData ();

  return strstreambuf::underflow();
};


int _KrProfileBufCons::sync()
{
  return overflow();
};



KrProfile::KrProfile (const IString & aApp, profile aProfile, Boolean iBuffered)
  : _KrProfileBase (aApp,iBuffered)
  , iostream (buf)
  , hini (0)
{
  flags (ios::unitbuf|ios::dec);
  profileName = "";
  switch (aProfile)
     {
     case system:
        hini = HINI_SYSTEMPROFILE;
        break;
     case user:
        hini = HINI_USERPROFILE;
        break;
     case both:
        hini = HINI_PROFILE;
        break;
     };
  isGood = true;
};


KrProfile::KrProfile (const IString & aApp, const IString & aProfile, Boolean iBuffered)
  : _KrProfileBase (aApp,iBuffered)
  , iostream (buf)
  , hini (0)
{
  flags (ios::unitbuf|ios::dec);
  hini = PrfOpenProfile (IThread::current().anchorBlock(), aProfile);
  isGood = (hini!=NULLHANDLE);
};


KrProfile::~KrProfile ()
{
  if (profileName != "" && isGood)
     PrfCloseProfile (hini);
};


Boolean KrProfile::good ()
{
  return isGood;
};


IString & KrProfile::key ()
{
  return keyName;
};


void KrProfile::setKey (const IString & aKey)
{
  char buffer [100];
  while (!eof())                                 // Make the buffer empty
    read (buffer,50);
  keyName = aKey;
  clearBuffer ();
  clear();
};


void KrProfile::clearBuffer ()
{
  buf->renewBuffer();
};



_KrProfileBase::_KrProfileBase (const IString & aApp, Boolean iBuffered)
{
  buf = new _KrProfileBuf(*this);
  buffered = iBuffered;
  appName = aApp;
  keyName = "Default";
};


_KrProfileBase::~_KrProfileBase ()
{
  delete buf;
};


void KrProfile::writeData (Boolean callFromBuff)
{
  if (!(buffered&&callFromBuff))
     {
     char * c = buf->buf()->str();
     unsigned long size = buf->buf()->pcount();
     PrfWriteProfileData (hini, appName, keyName, c, size);
     buf->buf()->freeze(0);
     }
};


void KrProfile::readData ()
{
  char * c;
  unsigned long size;
  PrfQueryProfileSize (hini, appName, keyName, &size);
  c = new char [size];
  PrfQueryProfileData (hini, appName, keyName, c, &size);
  buf->stream()->write (c, size);
  buf->stream()->flush();
  delete c;
};


long KrProfile::size ()
{
  unsigned long size;
  PrfQueryProfileSize (hini, appName, keyName, &size);
  return size;
};


void KrProfile::removeKey ()
{
  PrfWriteProfileData (hini, appName, keyName, NULL, 0);
};


void KrProfile::removeApp ()
{
  PrfWriteProfileData (hini, appName, NULL, NULL, 0);
};


Boolean KrProfile::exists ()
{
  unsigned long ul;
  PrfQueryProfileSize (hini, appName, NULL, &ul);
  return (ul!=0);
};


Boolean KrProfile::existsKey ()
{
  unsigned long ul;
  PrfQueryProfileSize (hini, appName, keyName, &ul);
  return (ul!=0);
};


KrProfile & clear (KrProfile & aProfile)
{
  aProfile.clearBuffer ();
  return aProfile;
};


KrProfile & flush (KrProfile & aProfile)
{
  aProfile.flush ();
  return aProfile;
};


KrProfile & remove (KrProfile & aProfile)
{
  aProfile.removeKey ();
  return aProfile;
};


KrProfile & operator << (KrProfile & aProfile, KrProfile & (*f)(KrProfile &))
{
  return f(aProfile);
};

