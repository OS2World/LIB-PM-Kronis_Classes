// Kroni's Classes: objects for  user communication using window
// (c) 1997 Wolfgang Kronberg
// file: krwc.cpp


#include "krwc.hpp"
#include "krprof.hpp"

#include <os2.h>                                 // WM_* messages

#include <limits.h>                              // To test for valid integer numbers

#include <istattxt.hpp>                          // IStaticText
#include <ientryfd.hpp>                          // IEntryField
#include <ipushbut.hpp>                          // IPushButton
#include <icmdhdr.hpp>                           // ICommandHandler
#include <iradiobt.hpp>                          // IRadioButton
#include <icheckbx.hpp>                          // ICheckBox



int const & key (_KrChoiceDataEntry const & t)
{
  return t.id;
};


int const & key (_KrBitfieldDataEntry const & t)
{
  return t.id;
};


int const & key (_KrWinCommDataField const & t)
{
  return t.id;
};


#define _KrWcOk 0x1ffe                           // ID for OK button in the KrWinComm dialog
#define _KrWcCancel 0x1ffd                       // ID for cancel button in the KrWinComm dialog


class _KrWcHandler : public ICommandHandler
{

public:

  _KrWcHandler (IWindow * win, KrWinComm * wc) : ICommandHandler () {window = win; winComm = wc;};
                                                 // win: Dialog Window
  virtual Boolean command (ICommandEvent & event);

private:

  IWindow * window;
  KrWinComm * winComm;

};


Boolean _KrWcHandler::command (ICommandEvent & event)
{
  KrWinCommData::Cursor cursor (winComm->data);
  winComm -> errorOccurred = false;
  winComm -> okPressed = false;

  switch (event.commandId ())
     {
     case _KrWcOk:
        if (winComm->data.setToFirst(cursor))
           {
           do
              {
              if (winComm->data.elementAt(cursor).translator (
                    true, winComm->data.elementAt(cursor).data, winComm->data.elementAt(cursor).window
                  )
                 )
                 winComm -> errorOccurred = true;
              }
           while (winComm->data.setToNext(cursor));
           };

        window -> postEvent(WM_CLOSE,0,0);
        winComm -> okPressed = true;
        window -> postEvent(WM_CLOSE,0,0);
        return true;

     case _KrWcCancel:
        window -> postEvent(WM_CLOSE,0,0);
        return true;

     default:
        return false;
     }
};



KrChoice::KrChoice ()
{
  active = 0;
};


KrChoice::KrChoice (const KrChoiceData & aData)
{
  data = aData;
  active = 0;
};


int KrChoice::add (int id, const IString & newChoice)
{
  if (data.containsElementWithKey(id))
     return 0;
  else
     {
     data.addOrReplaceElementWithKey (_KrChoiceDataEntry(id,newChoice));
     return id;
     };
};


int KrChoice::add (const IString & newChoice)
{
  int id = nextFree ();
  data.addOrReplaceElementWithKey (_KrChoiceDataEntry(id,newChoice));
  return id;
};


void KrChoice::set (int id)
{
  active = id;
};


int KrChoice:: get ()
{
  return active;
};


int KrChoice::numberOfEntries ()
{
  return data.numberOfElements ();
};


int KrChoice::getFirstKey ()
{
  return data.key(data.firstElement());
};


int KrChoice::getNextKey (int id)
{
  KrChoiceData::Cursor cursor (data);
  data.locateElementWithKey (id, cursor);
  if (!data.isLast(cursor))
     data.setToNext (cursor);
  return data.key(data.elementAt(cursor));
};


IString & KrChoice::getText (int id)
{
  return data.elementWithKey(id).text;
};


int KrChoice::nextFree ()
{
  if (data.isEmpty())
     return 1;
  else
     return data.key (data.lastElement()) + 1;
};



KrBitfield::KrBitfield (const KrBitfieldData & aData)
{
  data = aData;
};


int KrBitfield::add (int id, const IString & newEntry, Boolean checked)
{
  if (data.containsElementWithKey(id))
     return 0;
  else
     {
     data.addOrReplaceElementWithKey (_KrBitfieldDataEntry(id,newEntry,checked));
     return id;
     };
};


int KrBitfield::add (const IString & newEntry, Boolean checked)
{
  int id = nextFree ();
  data.addOrReplaceElementWithKey (_KrBitfieldDataEntry(id,newEntry,checked));
  return id;
};


int KrBitfield::numberOfEntries ()
{
  return data.numberOfElements ();
};


Boolean KrBitfield::toggle (int id, Boolean check)
{
  return data.elementWithKey(id).chosen = check;
};


Boolean KrBitfield::toggle (int id)
{
  return data.elementWithKey(id).chosen = ! isChecked (id);
};


Boolean KrBitfield::isChecked (int id)
{
  return data.elementWithKey(id).chosen;
};


int KrBitfield::getFirstKey ()
{
  return data.key(data.firstElement());
};


int KrBitfield::getNextKey (int id)
{
  KrBitfieldData::Cursor cursor (data);
  data.locateElementWithKey (id, cursor);
  if (!data.isLast(cursor))
     data.setToNext (cursor);
  return data.key(data.elementAt(cursor));
};


IString & KrBitfield::getText (int id)
{
  return data.elementWithKey(id).text;
};


int KrBitfield::nextFree ()
{
  if (data.isEmpty())
     return 1;
  else
     return data.key (data.lastElement()) + 1;
};



KrWinComm::KrWinComm (const IString & aTitle)
{
  currentLine = 2;
  currentLabel = "";
  buttons = 0;
  callWinInit = true;
  titleName = aTitle;
  exitList = 0;
  font = IFont ("WarpSans",9);
};


KrWinComm::~KrWinComm ()
{
  data.removeAll();

  typeExitList * el, * el2;
  el = exitList;
  while (el) {
     el2 = el->next;
     delete el->entry;
     delete el;
     el = el2;
     };
  exitList = 0;
};


void KrWinComm::winInit ()
{
  callWinInit = false;
  frame = new IFrameWindow (titleName,0x1000);
  title = new ITitle (frame, titleName);
  canvas = new IMultiCellCanvas (IC_FRAME_CLIENT_ID,frame,frame);
  KrProfile profile ("PM_Colors");
  profile.setKey ("DialogBackground");
  IString r,g,b;
  profile >> r >> g >> b;
  canvas -> setBackgroundColor (IColor(r.asInt(),g.asInt(),b.asInt()));
  canvas->setFont (font);
  canvas->setColumnWidth (1,0,true);
  canvas->setColumnWidth (6,0,true);
  canvas->setRowHeight (1,0,true);
}


void KrWinComm::setNextPrompt (const IString & title)
{
  currentLabel = title;
};


void KrWinComm::addToNextPrompt (const IString & title)
{
  currentLabel += title;
};


void KrWinComm::setTitle (const IString & aTitle)
{
  titleName = aTitle;
  if (!callWinInit)
     title->setText (aTitle);
};


void KrWinComm::setFont (const IFont & aFont)
{
  font = aFont;
  if (!callWinInit)
     canvas->setFont (font);
};


int KrWinComm::nextFree ()
{
  if (data.isEmpty())
     return 1;
  else
     return data.key (data.lastElement()) + 1;
};


void KrWinComm::addEntryField (IString & s)
{
  if (callWinInit)
     winInit ();

  IEntryField * entryField = new IEntryField (0x1000 + 0x40 * currentLine, canvas, canvas);
  entryField -> setAutoDeleteObject();
  entryField -> setText(s).enableTabStop();

  data.addOrReplaceElementWithKey (
    _KrWinCommDataField (
      nextFree(), IString (currentLabel), &s, entryField, translateString
    )
  );

  IStaticText * text = new IStaticText (0x1ff0 - currentLine, canvas, canvas);
  text -> setAutoDeleteObject ();
  text -> setText (currentLabel);
  canvas -> addToCell (text, 3, currentLine);
  canvas -> addToCell (entryField, 4, currentLine);
  canvas -> setRowHeight (++currentLine, 2, true);

  currentLabel = "";
  currentLine++;
};


void KrWinComm::addEntryField (double & d)
{
  if (callWinInit)
     winInit ();

  IEntryField * entryField = new IEntryField (0x1000 + 0x40 * currentLine, canvas, canvas);
  entryField -> setAutoDeleteObject ();
  entryField -> setText (IString(d)).enableTabStop();

  data.addOrReplaceElementWithKey (
    _KrWinCommDataField (
      nextFree(), IString (currentLabel), &d, entryField, translateDouble
    )
  );

  IStaticText * text = new IStaticText (0x1ff0 - currentLine, canvas, canvas);
  text -> setAutoDeleteObject ();
  text -> setText (currentLabel);
  canvas -> addToCell (text, 3, currentLine);
  canvas -> addToCell (entryField, 4, currentLine);
  canvas -> setRowHeight (++currentLine, 2, true);

  currentLabel = "";
  currentLine++;
};


void KrWinComm::addEntryField (unsigned long & l)
{
  if (callWinInit)
     winInit ();

  IEntryField * entryField = new IEntryField (0x1000 + 0x40 * currentLine, canvas, canvas);
  entryField -> setAutoDeleteObject ();
  entryField -> setText (IString(l)).enableTabStop();

  data.addOrReplaceElementWithKey (
    _KrWinCommDataField (
      nextFree(), IString (currentLabel), &l, entryField, translateUnsignedLong
    )
  );

  IStaticText * text = new IStaticText (0x1ff0 - currentLine, canvas, canvas);
  text -> setAutoDeleteObject ();
  text -> setText (currentLabel);
  canvas -> addToCell (text, 3, currentLine);
  canvas -> addToCell (entryField, 4, currentLine);
  canvas -> setRowHeight (++currentLine, 2, true);

  currentLabel = "";
  currentLine++;
};


void KrWinComm::addEntryField (signed long & l)
{
  if (callWinInit)
     winInit ();

  IEntryField * entryField = new IEntryField (0x1000 + 0x40 * currentLine, canvas, canvas);
  entryField -> setAutoDeleteObject ();
  entryField -> setText (IString(l)).enableTabStop();

  data.addOrReplaceElementWithKey (
    _KrWinCommDataField (
      nextFree(), IString (currentLabel), &l, entryField, translateSignedLong
    )
  );

  IStaticText * text = new IStaticText (0x1ff0 - currentLine, canvas, canvas);
  text -> setAutoDeleteObject ();
  text -> setText (currentLabel);
  canvas -> addToCell (text, 3, currentLine);
  canvas -> addToCell (entryField, 4, currentLine);
  canvas -> setRowHeight (++currentLine, 2, true);

  currentLabel = "";
  currentLine++;
};


void KrWinComm::addEntryField (KrBitfield & b)
{
  if (callWinInit)
     winInit ();

  IMultiCellCanvas * bitfieldCanvas = new IMultiCellCanvas
     (0x1000 + 0x40 * currentLine, canvas, canvas);
  bitfieldCanvas -> setAutoDeleteObject ();

  data.addOrReplaceElementWithKey (
    _KrWinCommDataField (
      nextFree(), IString (currentLabel), &b, bitfieldCanvas, translateBitfield
    )
  );

  int i;
  int n = b.numberOfEntries ();
  int key = b.getFirstKey ();
  for (i=0; i<n; i++)
     {
     ICheckBox * checkBox;

     checkBox = new ICheckBox (0x1000+i, bitfieldCanvas, bitfieldCanvas);
     checkBox -> setAutoDeleteObject ();
     if (b.isChecked (key))
        checkBox -> select ();
     checkBox -> setText (b.getText (key)).enableTabStop();
     bitfieldCanvas -> addToCell (checkBox,1,i+1);

     key = b.getNextKey (key);
     }

  IStaticText * text = new IStaticText (0x1ff0 - currentLine, canvas, canvas);
  text -> setAutoDeleteObject ();
  text -> setText (currentLabel);
  canvas -> addToCell (text, 3, currentLine);
  canvas -> addToCell (bitfieldCanvas, 4, currentLine);
  canvas -> setRowHeight (++currentLine, 2, true);

  currentLabel = "";
  currentLine++;

};


void KrWinComm::addEntryField (KrChoice & c)
{
  if (callWinInit)
     winInit ();

  IMultiCellCanvas * choiceCanvas = new IMultiCellCanvas
     (0x1000 + 0x40 * currentLine, canvas, canvas);
  choiceCanvas -> setAutoDeleteObject ();

  data.addOrReplaceElementWithKey (
    _KrWinCommDataField (
      nextFree(), IString (currentLabel), &c, choiceCanvas, translateChoice
    )
  );

  int i;
  int n = c.numberOfEntries ();
  int key = c.getFirstKey ();
  for (i=0; i<n; i++)
     {
     IRadioButton * radio;

     radio = new IRadioButton (0x1000+i, choiceCanvas, choiceCanvas);
     radio -> setAutoDeleteObject ();
     if (key == c.get())
        radio -> select ();
     radio -> setText (c.getText (key));
     choiceCanvas -> addToCell (radio,1,i+1);

     key = c.getNextKey (key);
     }

  IStaticText * text = new IStaticText (0x1ff0 - currentLine, canvas, canvas);
  text -> setAutoDeleteObject ();
  text -> setText (currentLabel);
  canvas -> addToCell (text, 3, currentLine);
  canvas -> addToCell (choiceCanvas, 4, currentLine);
  canvas -> setRowHeight (++currentLine, 2, true);

  currentLabel = "";
  currentLine++;

};


void KrWinComm::addEntryField (KrUserDataField * f)
{
  if (callWinInit)
     winInit ();

  IStaticText * text = new IStaticText (0x1ff0 - currentLine, canvas, canvas);
  text -> setAutoDeleteObject ();
  text -> setText (currentLabel);
  currentLabel = "";
  canvas -> addToCell (text, 3, currentLine);

  currentLine = f->initialize (*canvas, currentLine);

  data.addOrReplaceElementWithKey (
    _KrWinCommDataField (
      nextFree(),
      IString (currentLabel),
      f,
      0,                                         // We don't need the KrUserDataField's window here
      translateUserData
    )
  );

  canvas -> setRowHeight (++currentLine, 2, true);

  typeExitList * el;
  el = exitList;
  if (el) {
     while (el->next) el = el->next;
     el->next = new typeExitList;
     el = el->next;
     }
  else el = new typeExitList;
  el->entry = f;
  el->next = 0;
};


void KrWinComm::addEntryField (KrUserDataField & f)
{
  if (callWinInit)
     winInit ();

  IStaticText * text = new IStaticText (0x1ff0 - currentLine, canvas, canvas);
  text -> setAutoDeleteObject ();
  text -> setText (currentLabel);
  currentLabel = "";
  canvas -> addToCell (text, 3, currentLine);

  currentLine = f.initialize (*canvas, currentLine);

  data.addOrReplaceElementWithKey (
    _KrWinCommDataField (
      nextFree(),
      IString (currentLabel),
      &f,
      0,                                         // We don't need the KrUserDataField's window here
      translateUserData
    )
  );

  canvas -> setRowHeight (++currentLine, 2, true);
};


Boolean KrWinComm::run ()
{
  if (callWinInit)
     winInit ();

  if (!buttons)
     {
     buttons = new IMultiCellCanvas (0x1fff, canvas, canvas);
     buttons -> setAutoDeleteObject ();
     IPushButton * ok = new IPushButton (
                              _KrWcOk, buttons, buttons, IRectangle(),
                              IPushButton::defaultStyle() | IPushButton::defaultButton
                            );
     IPushButton * cancel = new IPushButton (_KrWcCancel, buttons, buttons);
     ok -> setAutoDeleteObject ();
     cancel -> setAutoDeleteObject ();
     ok -> setText ("Ok");
     cancel -> setText ("Cancel");
     buttons -> addToCell (ok,2,2);
     buttons -> addToCell (cancel,4,2);
     buttons -> setColumnWidth (1,2,true);
     buttons -> setColumnWidth (3,2,true);
     buttons -> setColumnWidth (5,2,true);
     buttons -> setRowHeight (1,2,true);
     buttons -> setRowHeight (3,2,true);
     canvas -> addToCell (buttons,3,currentLine,2,1);
     canvas -> setRowHeight (++currentLine, 2, true);
     currentLine++;
     };

  ISize size = canvas -> minimumSize (true);

  IRectangle sizeFrame = frame -> frameRectFor (IRectangle(IPoint(0,0),IPoint(size.width(),size.height())));
  size.setWidth (sizeFrame.width());
  size.setHeight (sizeFrame.height());
  size.setWidth (size.width()+8);                // 4 pixels extra width per flexible cell
  int numFlex = 0;
  int i;
  for (i=1; i<currentLine; i++)
     if (canvas -> isRowExpandable (i))
        numFlex++;
  numFlex *= 4;
  size.setHeight (size.height()+numFlex);
  frame->sizeTo (size);

  _KrWcHandler * handler;
  handler = new _KrWcHandler (frame,this);
  handler->handleEventsFor (frame);

  frame -> setFocus ();
  frame -> show ();
  IApplication::current().run();

  delete canvas;
  delete title;
  delete frame;
  delete handler;
  currentLine = 2;
  buttons = 0;
  data.removeAll();
  callWinInit = true;

  typeExitList * el, * el2;
  el = exitList;
  while (el) {
     el2 = el->next;
     delete el->entry;
     delete el;
     el = el2;
     };
  exitList = 0;

  return okPressed;
};


Boolean KrWinComm::ok ()
{
  return okPressed;
};


Boolean KrWinComm::translateString (Boolean doIt, void * data, void * window)
{
  IString s = ((IEntryField*)(window)) -> text ();
  if (doIt)
     (*(IString*)(data)) = s;
  return true;                                   // No error possible
};


Boolean KrWinComm::translateDouble (Boolean doIt, void * data, void * window)
{
  IString s = ((IEntryField*)(window)) -> text ();
  if (s.occurrencesOf (".") > 1)                 // More than one decimal point?
     return false;
  IString t = s;
  if (t.includes ('.'))
     t.remove (t.indexOf ('.'),1);               // t is s without any point
  t.stripLeading (' ');
  t.stripLeading ("-");
  t.stripLeading ("+");
  t.stripLeading (' ');                          // The detection is not perfect, but should be enough.
  if ( ! t.isDigits() )
     return false;                               // t is not a valid number

  if (doIt)
     (*(double*)(data)) = s.asDouble();
  return true;                                   // No more error possible
};


Boolean KrWinComm::translateSignedLong (Boolean doIt, void * data, void * window)
{
  IString s = ((IEntryField*)(window)) -> text ();
  IString t = s;
  t.stripLeading (' ');
  t.stripLeading ("-");
  t.stripLeading ("+");
  t.stripLeading (' ');                          // The detection is not perfect, but should be enough.
  if ( ! t.isDigits() )
     return false;                               // t is not a valid integer number
  double d = s.asDouble();
  if ((d<LONG_MIN) || (d>LONG_MAX))
     return false;                               // Number too big

  if (doIt)
     (*(signed long*)(data)) = d;
  return true;                                   // No more error possible
};


Boolean KrWinComm::translateUnsignedLong (Boolean doIt, void * data, void * window)
{
  IString s = ((IEntryField*)(window)) -> text ();
  IString t = s;
  t.stripLeading (' ');
  t.stripLeading ("+");
  t.stripLeading (' ');
  if ( ! t.isDigits() )
     return false;                               // t is not a valid nonnegative integer number
  double d = s.asDouble();
  if (d>ULONG_MAX)
     return false;                               // Number too big

  if (doIt)
     (*(unsigned long*)(data)) = d;
  return true;                                   // No more error possible
};


Boolean KrWinComm::translateBitfield (Boolean doIt, void * data, void * window)
{
  if (!doIt)
     return true;                                // No error possible

  KrBitfield * bitfield = (KrBitfield*)(data);
  IMultiCellCanvas * canvas = (IMultiCellCanvas*)(window);

  int i;
  int n = bitfield -> numberOfEntries ();
  int key = bitfield -> getFirstKey ();
  for (i=0; i<n; i++)
     {
     bitfield -> toggle (key, ((ICheckBox*)(canvas -> windowInCell (1,i+1))) -> isSelected () );
     key = bitfield -> getNextKey (key);
     }

  return true;
};


Boolean KrWinComm::translateChoice (Boolean doIt, void * data, void * window)
{
  if (!doIt)
     return true;                                // No error possible

  KrChoice * choice = (KrChoice*)(data);
  IMultiCellCanvas * canvas = (IMultiCellCanvas*)(window);

  int i;
  int n = choice -> numberOfEntries ();
  int key = choice -> getFirstKey ();
  for (i=0; i<n; i++)
     {
     if ( ((IRadioButton*)(canvas -> windowInCell (1,i+1))) -> isSelected () )
        choice -> set (key);
     key = choice -> getNextKey (key);
     }

  return true;
};


Boolean KrWinComm::translateUserData (Boolean doIt, void * data, void * window)
{
  return ((KrUserDataField*)(data)) -> transform (doIt);
}



KrWinComm & operator << (KrWinComm & aWinComm, const IString & aString)
{
  aWinComm.addToNextPrompt (aString);
  return aWinComm;
};


KrWinComm & operator >> (KrWinComm & aWinComm, IString & aString)
{
  aWinComm.addEntryField (aString);
  return aWinComm;
};


KrWinComm & operator >> (KrWinComm & aWinComm, double & aDouble)
{
  aWinComm.addEntryField (aDouble);
  return aWinComm;
};


KrWinComm & operator >> (KrWinComm & aWinComm, unsigned long & aUnsigned)
{
  aWinComm.addEntryField (aUnsigned);
  return aWinComm;
};


KrWinComm & operator >> (KrWinComm & aWinComm, signed long & aSigned)
{
  aWinComm.addEntryField (aSigned);
  return aWinComm;
};


KrWinComm & operator >> (KrWinComm & aWinComm, KrBitfield & aBitfield)
{
  aWinComm.addEntryField (aBitfield);
  return aWinComm;
};


KrWinComm & operator >> (KrWinComm & aWinComm, KrChoice & aChoice)
{
  aWinComm.addEntryField (aChoice);
  return aWinComm;
};


KrWinComm & operator >> (KrWinComm & aWinComm, KrUserDataField & aUserData)
{
  aWinComm.addEntryField (aUserData);
  return aWinComm;
};


KrWinComm & operator >> (KrWinComm & aWinComm, KrUserDataField * aUserData)
{
  aWinComm.addEntryField (aUserData);
  return aWinComm;
};


KrWinComm & operator >> (KrWinComm & aWinComm, KrWinComm & (*f) (KrWinComm&) )
{
  return f (aWinComm);
};


KrWinComm & operator << (KrWinComm & aWinComm, KrWinComm & (*f) (KrWinComm&) )
{
  return f (aWinComm);
};


KrWinComm & display (KrWinComm & aWinComm)
{
  aWinComm.run();
  return aWinComm;
};

