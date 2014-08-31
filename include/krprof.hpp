// Kroni's Classes: objects for reading and writing ini-file data
// (c) 1997 Wolfgang Kronberg
// file: krprof.hpp

// **********************************************************************************************************
//
// defines these classes:
//
//   KrProfile               Acts like a stream for in- and output of ini-file data
//
// defines those global symbols for private use:
//
//   _KrProfileBase
//   _KrProfileBuf
//
// **********************************************************************************************************


#ifndef __KRPROF_HPP__
#define __KRPROF_HPP__


#include <istring.hpp>
#include <strstrea.h>
#include <ihandle.hpp>
#include <iomanip.h>


class _KrProfileBase;                            // forward declarator for _KrProfileBuf

class _KrProfileBufCons : public strstreambuf    // The ultimate Profile string consumer
{

public:

  _KrProfileBufCons (_KrProfileBase & aParent);

  virtual int sync();
  virtual int overflow (int c=EOF);
  virtual int underflow ();


private:

  _KrProfileBase * parent;

};


class _KrProfileBuf : public streambuf           // The buffer class attached to KrProfile
{

public:

  _KrProfileBuf (_KrProfileBase & aParent);
  ~_KrProfileBuf ();

  void renewBuffer ();                           // Discard the _KrProfileBufCons and get a new one

  virtual int sync();
  virtual int overflow (int c=EOF);
  virtual int underflow ();
  virtual int doallocate ();

  _KrProfileBufCons * buf();                     // Returns the ultimate consumer buffer
  iostream * stream();                           // Returns the ultimate consumer

private:

  iostream * theStream;
  _KrProfileBufCons * buffer;                    // The ultimate consumer buffer
  _KrProfileBase * parent;

};



class _KrProfileBase
{

public:

  _KrProfileBase (const IString & aApp, Boolean iBuffered);
  ~_KrProfileBase ();

  virtual void writeData (Boolean callFromBuff) = 0;
  virtual void readData () = 0;


protected:

  _KrProfileBuf * buf;
  Boolean buffered;
  IString keyName, appName;

};


class KrProfile : public _KrProfileBase, public iostream
{

public:

  enum profile { system, user, both };           // Which profile is to be used?

  KrProfile (const IString & aApp, profile aProfile = user, Boolean iBuffered = false);
                                                 // Constructor: aApp = Name of application
                                                 //   aProfile: sort of ini file to be used
                                                 //   iBuffered: true = data must be explicitely written
  KrProfile (const IString & aApp, const IString & aProfile, Boolean iBuffered = false);
                                                 // aProfile: name of ini file to be used, this must be
                                                 //   neither the user nor the system ini file!

  ~KrProfile ();

  Boolean good ();                               // Open profile successful?

  void clearBuffer ();                           // Clears the associated strstreambuf object
  void removeKey ();                             // Removes the current key from the ini file
  void removeApp ();                             // Removes the current application from the ini file
  long size ();                                  // Returnes the size of the current key data

  IString & key ();                              // Returns the current key
  void setKey (const IString & aKey = "Default");
                                                 // Sets the new key

  Boolean exists ();                             // Exists the current application in the profile?
  Boolean existsKey ();                          // Exists the key in the profile?

  virtual void writeData (Boolean callFromBuff); // Writes to the ini file if one of iBuffered and
                                                 //   callFromBuff is false
  virtual void readData ();                      // Reads from the ini file

private:

  IHandle hini;
  IString profileName;
  Boolean isGood;
};


// Manipulators for KrProfile

KrProfile & clear (KrProfile & aProfile);        // Calls KrProfile::clearBuffer()
KrProfile & flush (KrProfile & aProfile);        // Writes to the ini file *now*
KrProfile & remove (KrProfile & aProfile);       // Removes the current key from the ini file
KrProfile & operator << (KrProfile & aProfile, KrProfile & (*f)(KrProfile &));

#endif

