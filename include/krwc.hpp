// Kroni's Classes: objects for user communication using windows
// (c) 1997 Wolfgang Kronberg
// file: krwc.hpp

// **********************************************************************************************************
//
// defines these classes:
//
//   KrBitfieldData          Data structure with options each of which may be set or unset
//   KrChoiceData            Data structure with options of which exactly one is set
//   KrBitfield              For easy handling of KrBitfieldData
//   KrChoice                For easy handling of KrChoiceData
//   KrUserDataField         Virtual class to allow >>-like input for any data structure
//   KrWinComm               Allow >>-like input by presenting the user a dialog window
//
// defines overloads of these functions:
//
//   <<                      Add prompt lines to KrWinComm dialog window
//   >>                      Input data from KrWinComm dialog window
//   display                 Actually display the KrWinComm dialog window
//
// defines those global symbols for private use:
//
//   _KrChoiceDataEntry
//   _KrBitfieldDataEntry
//   _KrWinCommDataField
//   key ()
//
// **********************************************************************************************************


#ifndef __KRWC_HPP__
#define __KRWC_HPP__


#include <iksset.h>                              // Key sorted set

#include <istring.hpp>                           // IString
#include <iframe.hpp>                            // IFrameWindow
#include <imcelcv.hpp>                           // IMultiCellCanvas
#include <ititle.hpp>                            // ITitle
#include <ifont.hpp>                             // IFont



class _KrChoiceDataEntry                         // Represents one radio button
{
public:
  _KrChoiceDataEntry (int aId, IString aText = IString())
    {id = aId; text = aText;};
  int id;
  IString text;
};


class _KrBitfieldDataEntry                       // Represents one check box
{
public:
  _KrBitfieldDataEntry (int aId, IString aText = IString(), Boolean aChosen = false)
    {id = aId; text = aText; chosen = aChosen;};
  int id;
  IString text;
  Boolean chosen;
};


int const & key (_KrChoiceDataEntry const & t);  // These are required for IKeySortedSet
int const & key (_KrBitfieldDataEntry const & t);

typedef IKeySortedSet <_KrChoiceDataEntry,int> KrChoiceData;
                                                 // Defines sets of KrChoiceData elements
typedef IKeySortedSet <_KrBitfieldDataEntry,int> KrBitfieldData;
                                                 // Defines sets of KrBitfieldData elements


class KrChoice : public IBase                    // Represents a collection of radio buttons
{

public:

  KrChoice ();                                   // Default constructor
  KrChoice (const KrChoiceData & aData);         // Constructor from data field

  int add (int id, const IString & newChoice);   // Adds a new choice, returns its ID or zero if id invalid
  int add (const IString & newChoice);           // Adds a new choice, returns its ID
  void set (int id);                             // Set choice #id active
  int get ();                                    // Get active ID

  int numberOfEntries ();                        // How many times was "add" called?

  int getFirstKey ();                            // Gets the ID of the first radio button
  int getNextKey (int id);                       // Gets the ID of the next radio button
  IString & getText (int id);                    // Gets the next beneath radio button # "id"

private:

  int nextFree ();                               // Gets a free ID
  int active;                                    // Active choice
  KrChoiceData data;                             // Contains all added choices

};


class KrBitfield : public IBase                  // Represents a collection of check boxes
{

public:

  KrBitfield () {};                              // Default constructor
  KrBitfield (const KrBitfieldData & aData);     // Constructor from data field

  int add (int id, const IString & newEntry, Boolean checked = false);
                                                 // Adds a new entry, returns its ID or zero if id invalid
  int add (const IString & newEntry, Boolean checked = false);
                                                 // Adds a new entry, returns its ID
  Boolean toggle (int id, Boolean check);        // Check or uncheck entry #id, return new check status
  Boolean toggle (int id);                       // Reverse check status of entry #id, return it
  Boolean isChecked (int id);                    // Is entry checked?

  int numberOfEntries ();                        // How many times was "add" called?

  int getFirstKey ();                            // Gets the ID of the first check box
  int getNextKey (int id);                       // Gets the ID of the next check box
  IString & getText (int id);                    // Gets the next beneath check box # "id"

private:

  int nextFree ();                               // Gets a free ID
  KrBitfieldData data;                           // Contains all added entries

};


class KrUserDataField : public IBase
{
public:
  virtual Boolean transform (Boolean doIt) = 0;  // Transforms the user input to the actual data structure
                                                 //   which is to be input, if doIt = true. Otherwise, only
                                                 //   says if that transformation *would* be successful.
  virtual int initialize (IMultiCellCanvas & c, int start) = 0;
                                                 // Performs all actions neccessary to initialize the
                                                 //   user input window. This is called by
                                                 //   KrWindowedCommunication::setEntryField().
                                                 //   The input window should be placed in the canvas c,
                                                 //   starting at row "start". The first unused row should
                                                 //   be returned. Use columns 2-5 for framed groups and
                                                 //   columns 3-4 otherwise.
};


typedef Boolean (* KrWinCommTranslator) (Boolean doIt, void * data, void * window);
                                                 // Translates window -> data.
                                                 //   return value (0/1) = success (y/n)
                                                 //   doIt = false: don't translate, just test user data
                                                 //   in "window": is it valid for translation?

class _KrWinCommDataField                         // Internal data structure for KrWinComm
{
public:
  _KrWinCommDataField (int i, const IString & l, void * d, void * w,
      KrWinCommTranslator t)
    {id = i; label = l; data = d; window = w; translator = t;};
  IString label;
  void * data;
  void * window;
  KrWinCommTranslator translator;
  int id;
};


int const & key (_KrWinCommDataField const & t); // Required for IKeySortedSet
typedef IKeySortedSet <_KrWinCommDataField,int> KrWinCommData;
                                                 // Collection of KrWinCommDataField objects

class KrWinComm : public IBase                   // Display and evaluate a dialog
{

public:

  KrWinComm (const IString & aTitle = "Dialog"); // Constructor. Sets the Window Title.
  ~KrWinComm ();                                 // Destructor

  void setNextPrompt (const IString & title);    // Sets an introducting line for the next input
  void addToNextPrompt (const IString & title);  // Appends to the introducting line for the next input
  void setTitle (const IString & aTitle);        // Sets the dialog title
  void setFont (const IFont & aFont);            // Sets the dialog font

  void addEntryField (IString & s);              // Inputs data from the user. These are all input
  void addEntryField (double & d);               //   as IString and converted internally.
  void addEntryField (unsigned long & l);
  void addEntryField (signed long & l);

  void addEntryField (KrBitfield & b);           // These are input as checkboxes / radio buttons.
  void addEntryField (KrChoice & c);             //   NOTE: do not use b's or c's "add" functions
                                                 //   after calling these methods (or you'll most
                                                 //   likely get a sys3175 after closing the dialog)!

  void addEntryField (KrUserDataField & f);      // This lets the user define any other type of variable
                                                 //   to input.

  void addEntryField (KrUserDataField * f);      // This can be used to input even predefined types that
                                                 //   are none of the above and that do not inherit
                                                 //   from KrUserDataField

  Boolean ok ();                                 // Did the user press OK?
  Boolean run ();                                // Display the dialog and wait for user input
                                                 //   returnes: did the user press OK?
private:

  typedef struct typeExitList {
    KrUserDataField * entry;
    typeExitList * next;
  } _tagTypeExitList;

  int nextFree ();

  void winInit ();                               // Does the initialization for window elements
  Boolean callWinInit;                           // must winInit still be called?

  static Boolean translateString (Boolean doIt, void * data, void * window);
  static Boolean translateDouble (Boolean doIt, void * data, void * window);
  static Boolean translateUnsignedLong (Boolean doIt, void * data, void * window);
  static Boolean translateSignedLong (Boolean doIt, void * data, void * window);
  static Boolean translateBitfield (Boolean doIt, void * data, void * window);
  static Boolean translateChoice (Boolean doIt, void * data, void * window);
  static Boolean translateUserData (Boolean doIt, void * data, void * window);

  typeExitList * exitList;
  IString currentLabel;
  int currentLine;
  IMultiCellCanvas * canvas;
  IFrameWindow * frame;
  ITitle * title;
  IString titleName;
  IMultiCellCanvas * buttons;
  Boolean okPressed;
  Boolean errorOccurred;
  KrWinCommData data;
  IFont font;

  friend class _KrWcHandler;                     // Defined elsewhere; handles dialog message queue
};


KrWinComm & operator << (KrWinComm & aWinComm, const IString & aString);
                                                 // Operator access for KrWinComm::addToNextPrompt ()

KrWinComm & operator >> (KrWinComm & aWinComm, IString & aString);
                                                 // Operator access for KrWinComm::addEntryField (...)
KrWinComm & operator >> (KrWinComm & aWinComm, double & aDouble);
KrWinComm & operator >> (KrWinComm & aWinComm, unsigned long & aUnsigned);
KrWinComm & operator >> (KrWinComm & aWinComm, signed long & aSigned);
KrWinComm & operator >> (KrWinComm & aWinComm, KrBitfield & aBitfield);
KrWinComm & operator >> (KrWinComm & aWinComm, KrChoice & aChoice);
KrWinComm & operator >> (KrWinComm & aWinComm, KrUserDataField & aUserData);
KrWinComm & operator >> (KrWinComm & aWinComm, KrUserDataField * aUserData);

KrWinComm & display (KrWinComm & aWinComm);      // Operator access for KrWinComm.run ()

KrWinComm & operator >> (KrWinComm & aWinComm, KrWinComm & (*f) (KrWinComm&) );
                                                 // These two are needed for display ()
KrWinComm & operator << (KrWinComm & aWinComm, KrWinComm & (*f) (KrWinComm&) );


#endif

