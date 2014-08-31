// Kroni's Classes: Demonstration of many abilities in one guided tour
// (c) 1997 Wolfgang Kronberg
// file: demo.cpp


// ************************************************************************* //
//                   Demonstration of Kroni's Classes                        //
//                                                                           //
//                               Guided tour                                 //
// ************************************************************************* //

#define IC_TRACE_ALL                             // Activate full tracing even in non-critical situations

#include "krcto.hpp"                             // Support to re-route cout to a window
#include "krbwin.hpp"                            // Support for bitmap-buffered graphics
#include "krprint.hpp"                           // Support for printing
#include "krgobj.hpp"                            // Support for special graphics objects
                                                 //   includes krcstran.hpp for coordinate systems
#include "krasyncm.hpp"                          // Support for asynchronous main function
#include "krwc.hpp"                              // Support for windowed communication
#include "krtrace.hpp"                           // Support for exception handling

#include <isplitcv.hpp>                          // ISplitCanvas
#include <icheckbx.hpp>                          // ICheckBox
#include <ientryfd.hpp>                          // IEntryField
#include <imsgbox.hpp>                           // IMessageBox


// First, we must derive a class from the virtual class KrAsyncMain in a standard way:

class MyMain : public KrAsyncMain
{
public:
  MyMain (IWindow * mainWindow) : KrAsyncMain (mainWindow) {};
                                                 // We must forward the original object's only constructor
  virtual void main (IEvent & event);            // This will be our actual new main function
};


// All global window objects should be declared static and before all functions.
//   This is the comon UICL way to define a frame window and its childs:

static IFrameWindow mainWindow ("Demonstration of Kroni's Classes", 0x1000);
                                                 // Our main window
static ISplitCanvas split (IC_FRAME_CLIENT_ID, &mainWindow, &mainWindow);
                                                 // A canvas to hold the various sub-windows we need for
                                                 //   this demonstration
static KrCommonTextOutput text (0x1001, &split, &split);
                                                 // This window will hold all text output, e.g. for use
                                                 //   with common cout.
static IDrawingCanvas iDraw (0x1002, &split, &split);
                                                 // This window will hold general graphics output. We will
                                                 //   later use it to hold a KrMemoryBitmap object.
static KrDrawingCanvas krDraw (0x1003, &split, &split);
                                                 // Another window, just like the previous one, but we want
                                                 //   to put graphics objects in there associated with an
                                                 //   KrCoordSystemTranslator object.

// The following will demonstrate how to prepare any kind of object for user input.
//   This is a little bit sophisticated; skip it on first read  and
//   go on at the definition of MyMain::main ().

// This is the object we want to input: a combination of string and boolean,
//   the boolean indicating whether the string has any meaning.

class BoolString
{
public:
  BoolString (const IString & s = "", Boolean b = false) {string = s; boolean = b;};
  IString string;
  Boolean boolean;
};

// All user defined input is done by a class derived from KrUserDataField:

class BoolStringInput : public KrUserDataField
{
public:

  BoolStringInput (BoolString & b);              // The constructor (there's no work to do for the
                                                 //   base class)
  ~BoolStringInput ();                           // The same goes for the destructor.

  virtual Boolean transform (Boolean doIt);      // These two functions *must* be overwritten.
  virtual int initialize (IMultiCellCanvas & c, int start);

private:

  BoolString * theVariable;                      // Pointer to the variable we want to input

  IMultiCellCanvas * canvas;                     // Some windows we'll need to accept user input
  ICheckBox * checkBox;
  IEntryField * entryField;
};

// Now let's define BoolStringInput's member functions.

BoolStringInput::BoolStringInput (BoolString & b)
{
  theVariable = &b;
  canvas = 0; checkBox = 0; entryField = 0;      // To indicate that those are not constructed yet.
};

BoolStringInput::~BoolStringInput ()
{
  if (canvas) delete canvas;
  if (checkBox) delete checkBox;
  if (entryField) delete entryField;
};

Boolean BoolStringInput::transform (Boolean doIt)
                                                 // Here, we transform the data in the windows which
                                                 //   the user typed his input in to the variable itself.
{
  if (!doIt)                                     // No action should take place
     return true;                                // This means that the user input is OK (he couldn't
                                                 //   possibly do anything wrong).

  if (canvas && checkBox && entryField)          // If you don't shrew it up yourself, this should
     {                                           //   always be the case.
     theVariable -> boolean = checkBox -> isSelected ();
                                                 // This gives the user input straight to the variable.
     theVariable -> string = entryField -> text ();
     }
  else                                           // This can't happen, but we're careful...
     {
     KRNAMEDTHROW("Windows undefined!","Kroni's Classes Demo");
     };
  return true;                                   // User input is ok (s.a.).
};

int BoolStringInput::initialize (IMultiCellCanvas & c, int start)
                                                 // This function draws the windows for user input.
{
  if (canvas) delete canvas;                     // Clean up if neccessary
  if (checkBox) delete checkBox;
  if (entryField) delete entryField;

  canvas = new IMultiCellCanvas (
                 0x1000 + 0x40 * start,          // This window ID + the next 0x3f are save to use.
                 &c, &c                          // c must own the top level of windows you're building.
               );

  checkBox = new ICheckBox (0x1000, canvas, canvas);
                                                 // You are free to choose any window IDs for windows
                                                 //   on lower levels.
  entryField = new IEntryField (0x1001, canvas, canvas);

  checkBox -> select (theVariable->boolean);     // Initialize the check box with the boolean variable
                                                 //   it is meant to represent on input
  entryField -> setText (theVariable->string);   // dto. for the string variable

  // The cleanest way to show windows in the dialog is to put them into a canvas (if they are more
  //   than one) and to put this canvas into c at cell (4,start).

  canvas->addToCell (checkBox,1,1);
  canvas->addToCell (entryField,2,1);
  c.addToCell (canvas,4,start);

  return ++start;                                // Return the first unoccupied line in c.
};


// Next define the asynchronous main function. Doing calculations here does not block message processing.

void MyMain::main (IEvent & event)               // Do in this effective main function whatever you want!
{                                                //   However, please note that this function - unlike the
                                                 //   usual main() - will be left before the program exits.
                                                 //   Make sure to define all variables as static or use
                                                 //   new / delete for objects which must survive the exit
                                                 //   of the function.

  switch (event.parameter1())                    // Use this to define the different parts of your program
     {
     case cmdStartUp:                            // This part will be run when the program first comes up.
                                                 //   You may define more commands (e.g. as menu entries or
                                                 //   as subprocedures to be called from here) and add them
                                                 //   to this switch instruction.

  // We will use a common UICL message box to guide the user through the demo as he runs it.

        IMessageBox msgBox (0);                  // Use desktop as parent to make the box non-modal
        msgBox.setTitle ("Kroni's Classes Demo - Next Action");
        IMessageBox::Style msgBoxStyle =
          IMessageBox::okButton | IMessageBox::informationIcon | IMessageBox::moveable;

        msgBox.show ("Welcome to this demo of Kroni's Classes!\n\n"
                     "From left to right, you see:\n"
                     "- a KrCommonTextOutput object\n"
                     "      (used to diplay text on the screen)\n"
                     "- an IDrawingCanvas object\n"
                     "      (the common UICL graphics surface)\n"
                     "- a KrDrawingCanvas object\n"
                     "      (the IDrawingCanvas equivalent in Kroni's Classes)\n"
                     "\nFeel free to rearrange the windows however you like!\n"
                     "To stop this demo at any time, just close its main window.", msgBoxStyle);

  // Here the action takes place! Let's have a look at what Kroni's Classes can do.

  // Here's how to re-route text output to a GUI window:

        cout = text.stream();                    // Assign the KrTextOutput object's stream to cout
        cout.setf (ios::unitbuf);                // Unfortunately, the above line does not copy this property

        msgBox.show ("You are able to output text\n"
                     "using common C++ stream operators like <<.", msgBoxStyle);

  // Now we can use cout as we would in text mode programs:

        cout << "Here's a little test of\nKroni's Class Library!\n";

  // Of course, re-routing of cout is not necessary. You can also use the stream directly:

        text.stream() << "\nYou may both re-route cout or access the GUI stream directly.\n";

  // We draw a cross using common IOCL methods...

        static IGLine line1 (IPoint(0,0), IPoint(300,300));
        static IGLine line2 (IPoint(0,300), IPoint(300,0));
        static IGList crossList;
        crossList.addAsFirst (line1);
        crossList.addAsFirst (line2);

  // ...and display it on both drawing areas:

        msgBox.show ("Kroni's Classes peacefully coexist\n"
                     "with the IBM Open Class Library they're based on.\n\n"
                     "Do demonstrate this, we draw a cross\n"
                     "using the common UICL classes in the middle window.\n"
                     "Try resizing the window to see how the UICL reacts.",
                     msgBoxStyle);

        iDraw.setGraphicList (&crossList);
        iDraw.refresh();

        msgBox.show ("The drawing canvas from Kroni's Classes (right window)\n"
                     "is completely compatible and shows the same behaviour.",
                     msgBoxStyle);

        krDraw.setGraphicList (&crossList);
        krDraw.refresh();

  // We now put the cross into a memory bitmap:

        static KrMemoryBitmap mbm (300,300);
        mbm.setList (crossList);

  // This memory bitmap can be shown using common IOCL methods:

        static IGList mbmList;
        mbmList.addAsFirst (mbm);

        msgBox.show ("If you draw complex graphics on the screen,\n"
                     "redrawing can take a lot of time.\n"
                     "One solution is to do the complex drawing on a memory bitmap.\n"
                     "When the screen is redrawn, it is then sufficient to do a simple\n"
                     "copy operation from the memory to the screen.\n\n"
                     "The demo will now substitute the cross in the middle window\n"
                     "with a memory bitmap into which the very same cross is drawn.\n"
                     "You won't notice any difference.",
                     msgBoxStyle);

        iDraw.setGraphicList (&mbmList);
        iDraw.refresh();

        msgBox.show ("Again, we do the same for the right window.\n"
                     "Again, there won't be any difference.",
                     msgBoxStyle);

        krDraw.setGraphicList (&mbmList);
        krDraw.refresh();

  // Now let's do some graphics with user-friendly scaling methods.

  // This defines a coordinate system with a range from -1 to 1 on both axes.

        static KrCoordSystemTranslator trans (KrPoint(0,-1),KrPoint(1,1));

  // Within this coordinate system, we draw some graphics

        static KrGLine mLine1 (trans, KrPoint (0,-1), KrPoint (1,0));
        static KrGLine mLine2 (trans, KrPoint (0,0), KrPoint (1,-1));
        static KrGString mText (trans, "Kroni's Classes", KrPoint(0.5,0.5));

  // To collect graphics using a coordinate system, we need a KrGraphicList object instead of IGraphicList.

        static KrGraphicList mList (trans);

  // We use the common IOCL methods to get our graphics on the screen:

        mList.addAsFirst (mLine1);
        mList.addAsFirst (mLine2);
        mList.addAsFirst (mText);

        msgBox.show ("With Kroni's Classes, it is possible to draw pictures\n"
                     "using an arbitrary coordinate system instead of pixel units.\n\n"
                     "Try resizing the right window, into which an example is drawn,\n"
                     "and note how the picture is resized automatically\n"
                     "to always fit the new window size.",
                     msgBoxStyle);

        krDraw.setGraphicList (&mList);
        krDraw.refresh();

  // Now we want a unit on the y-axis to appear exactely as long as a unit on the x-axis.
  //   That means, we want to have an aspect ratio of 1.

        msgBox.show ("It is, however, also possible to preserve the aspect ratio.\n"
                     "In this case, the picture will still be as large as possible\n"
                     "*under the condition* that the aspect ratio is preserved.\n\n"
                     "Please note that 'as large as possible' means that the whole\n"
                     "coordinate system must fit in. In the following example, the cross\n"
                     "occupies only the lower half of the coordinate system.\n\n"
                     "If the cross in the right window does not cover the space\n"
                     "of an exakt square, your graphics driver gives wrong\n"
                     "information about the aspect ratio.\n"
                     "This error, however, should not be too large.",
                     msgBoxStyle);

        trans.setAspectRatio (1);
        krDraw.refresh();

  // All of this also works with memory bitmaps.

        msgBox.show ("Of course, you can also draw into memory bitmaps.\n"
                     "using the coordinate system method.\n"
                     "However, the bitmap is generally not automatically resized.\n\n"
                     "Have a look at the middle window to find out\n"
                     "what that means.",
                     msgBoxStyle);

        mbm.setList (mList);
        iDraw.refresh ();

        msgBox.show ("If you *really* want to resize a memory bitmap\n"
                     "automatically according to the window size, however,\n"
                     "there's a way to do it: assign a coordinate system to it.\n\n"
                     "Note, however, that a resized bitmap is often ugly.\n"
                     "Note also, that drawing in a coordinate system requires a\n"
                     "KrDrawingCanvas, so that the bitmap in this example must be\n"
                     "removed from the middle window; only the right can show it.\n\n"
                     "When writing your own programs, do yourself a favor by\n"
                     "using only KrDrawingCanvas and never IDrawingCanvas,\n"
                     "even if you currently don't use coordinate systems.",
                     msgBoxStyle);

        static IGList emptyList;                 // An empty list for the IDrawingCanvas
        iDraw.setGraphicList (&emptyList);
        iDraw.refresh();

        static KrCoordSystemTranslator trans2 (KrPoint(0,0),KrPoint(1,1));
        static KrGraphicList m2List (trans2);    // We use a new coordinate system
                                                 //   without fixed aspect ratio for fitting the bitmap
        m2List.addAsFirst (mbm);
        mbm.setCoordSystem (&trans2, KrPoint (0,0), KrPoint (1,1));
                                                 // The bitmap should cover the whole coordinate system
        krDraw.setGraphicList (&m2List);
        krDraw.refresh ();

  // Let's have a look at what frames do.

        static KrGraphicList fList (trans);

        msgBox.show ("The mighty frame concept lets you place arbitrary\n"
                     "complex pictures easily into subsections of the screen.\n\n"
                     "This is especially useful for printing.\n",
                     msgBoxStyle);

  // We add a frame in the upper left area, enter our picture there, and remove the frame.

        KrFrame frame1 (trans,KrPoint(0.05,0.05),0.4);
        KrAntiFrame aFrame (trans);
        fList.addAsLast (frame1);
        fList.addAsLast (mList);
        fList.addAsLast (aFrame);

  // Now we add a frame in the lower right area, enter the same picture there, and remove the frame.

        KrFrame frame2 (trans,KrPoint(0.55,-0.95),0.4);
        fList.addAsLast (frame2);
        fList.addAsLast (mList);
        fList.addAsLast (aFrame);

  // We draw the whole picture in the KrDrawingCanvas.

        krDraw.setGraphicList (&fList);
        krDraw.refresh ();

  // Removing all frames is neccessary to leave trans unframed for the next redraw.
  //   It is possible (but not desired for redraw) to have frames in frames.


  // We will not only look at the above object, but print it. First, create a KrPrinter object.

        static KrPrinter printer;

  // You can select any IGList (that means, also KrGraphicsList) into the KrPrinter object.

        printer.setList (fList);

        msgBox.show ("Kroni's Classes let you print everything what you can draw.\n"
                     "It also provides the neccessary dialogs for user interaction.\n\n"
                     "If you press OK in the next dialog, the picture from the\n"
                     "right window will be printed! Press cancel instead of OK\n"
                     "in the next dialog if you don't want this to happen,\n"
                     "but be sure to try the settings button before that!",
                     msgBoxStyle);

  // You might want to give the user the chance to chhoses the printer and set its options.

        if (printer.printDialog ())

  // Then all you have to do ist call the print command. The string parameter is diplayed in the spooler.

            printer.print ("KrClasses Demo");

  // Now let's have a look at the windowed communication classes.

        msgBox.show ("The last part of this demo shows a very easy way\n"
                     "to query for user input using the common >> operator.\n\n"
                     "The KrWinComm class will collect the input variables\n"
                     "and put them all into a dialog window.\n\n"
                     "The user can change the data and press either OK or\n"
                     "Cancel. In the latter case, the variables remain unchanged.",
                     msgBoxStyle);

        static KrWinComm kwc ("TestDialog");     // This defines the dialog object itself and sets its title

        static IString s = "Test Text";          // We define some arbitrary variables.
        static double d = 38.75;                 //   Since their values are shown to the user, they
        static unsigned long l = 1024;           //   must be initialized.
        static signed long ls = -2048;

        static KrBitfield bf;                    // This is how to give the user options (check boxes)
        int bf1 = bf.add ("Option One");
        int bf2 = bf.add ("Option Two");

        static KrChoice ch;                      // This is how to offer the user a choice (radio buttons)
        int ch1 = ch.add ("Choice One");
        int ch2 = ch.add ("Choice Two");
        ch.set (ch1);                            // Some choice should be set by default.

        static BoolString bs ("Optional string", true);
                                                 // All kind of self-defined variables can be used...
        static BoolStringInput bsi (bs);         // ...but they must be wrapped into a corresponding object
                                                 //   to define the appearence on the screen.

        kwc << "Enter a string here:" >> s       // The input is done using the common >> operator.
                                                 //   In addition, the << operator can be used to
                                                 //   put comments for the input variables into the dialog.
            << "Enter a floating point number here:" >> d
            << "Enter an unsigned integer here:" >> l
            << "Enter a signed integer here:" >> ls
            << "Select whatever options you like:" >> bf
            << "Choose one of these:" >> ch
            << "Enter a string or not:" >> bsi
            << display;                          // The "display" manipulator actually displays and
                                                 //   evaluates the dialog. It may be used with both the
                                                 //   >> and the << operator.

        cout << "\nResults of your Input:"       // This is to test the success.
             << "\nString: " << s
             << "\nFloating Point: " << d
             << "\nUnsigned int: " << l
             << "\nSigned int: " << ls
             << "\nOption 1: " << bf.isChecked(bf1)
             << "\nOption 2: " << bf.isChecked(bf2)
             << "\nChoice: " << ch.get ()
             << "\nOptional String: " << bs.boolean << " / " << bs.string;


        msgBox.show ("Thank you for trying Kroni's Classes!\n\n"
                     "To leave the program, close the main window.",
                     msgBoxStyle);

        break;                                   // Our effective main program ends here.
     };
};


int main (int argc, char **argv, char **envv)    // This should include all initial window definitions;
{                                                //   the other stuff should be in MyMain::main().
  IApplication::current().setArgs(argc,argv);    // This is "good style" when using the UICL.
                                                 //   Do always include this line.
  KrTrace::traceTo ();                           // Create log file
  KrTrace::tellUser (KrTrace::full);             // Give the user full information in a popup window
                                                 //   if any exception occurs!

  // First, we must make sure that our window is shown on the screen.
  //   This is ordinary UICL code.

  mainWindow.setClient (&split);
  mainWindow.sizeTo (ISize(800,500));
  mainWindow.setFocus ();
  mainWindow.show ();

  // These should always be the very last statements in the main() function:

  MyMain myMain (&mainWindow);                   // initializes the use of the function MyMain::main
  IApplication::current().run();
  return 0;
};

