// Kroni's Classes: Demonstration: Input dialog
// (c) 1997 Wolfgang Kronberg
// file: dialog.cpp

// ************************************************************************* //
// *                 Demonstration of Kroni's Classes                      * //
// *                                                                       * //
// *                           Input Dialog                                * //
// *                                                                       * //
// *  Demonstrated classes:                                                * //
// *    KrAsyncMain                                                        * //
// *    KrCommonTextOutput                                                 * //
// *    KrWinComm                                                          * //
// ************************************************************************* //


#include "krcto.hpp"                             // Support to re-route cout to a window
#include "krasyncm.hpp"                          // Support for asynchronous main function
#include "krwc.hpp"                              // Support for dialog input (windowed communication)

#include <iframe.hpp>                            // IFrameWindow


// First of all, all global window objects should be declared static and before all functions.
//   This is the comon UICL way to define a frame window and its childs:

static IFrameWindow mainWindow ("Demonstration of Kroni's Classes: Dialog sample", 0x1000);
                                                 // Our main window
static KrCommonTextOutput text (IC_FRAME_CLIENT_ID, &mainWindow, &mainWindow);
                                                 // This window will hold all text output, e.g. for use
                                                 //   with common cout.


// The object which take user input does not need to be defined here, but it's easier that way.

static KrWinComm wcin;                           // We could also give the dialog a title: ...wcin(title).


// We must always derive a class from the virtual class KrAsyncMain in a standard way:

class MyMain : public KrAsyncMain
{
public:
  MyMain (IWindow * mainWindow) : KrAsyncMain (mainWindow) {};
                                                 // We must forward the original object's only constructor
  virtual void main (IEvent & event);            // This will be our actual new main function
};


// Next define the asynchronous main function. Doing calculations here does not block message processing.

void MyMain::main (IEvent & event)               // Do in this effective main function whatever you want!
{

  IString s;                                     // Two values we want the user to input.
  double d;

  switch (event.parameter1())                    // Use this to define the different parts of your program
     {
     case cmdStartUp:                            // This part will be run when the program first comes up.

        cout = text.stream();                    // Assign the KrTextOutput object's stream to cout
        cout.setf (ios::unitbuf);                // Unfortunately, the above line does not copy this property

        s = "Old value of the string";           // Since the user will be offered to use the old values,
        d = 42;                                  //   those should be set to a sensible value.

        wcin << "Please enter a string here:" >> s;
                                                 // Ask for a string with this text explaining what to do...
        wcin << "...and a number here:" >> d     // ...and for a number.
             << display;                         // The display manipulator will display all the
                                                 //   queries we've made (in one dialog window)

        if (wcin.ok())                           // The result of the last dialog
           cout << "You pressed OK.\n";
        else
           cout << "You aborted the dialog.\n";

        cout << "  s = " << s << "\n  d = " << d;

        break;                                   // Our effective main program ends here.
     };
};


int main (int argc, char **argv, char **envv)    // This should include all initial window definitions;
{                                                //   the other stuff should be in MyMain::main().
  IApplication::current().setArgs(argc,argv);    // This is "good style" when using the UICL.
                                                 //   Do always include this line.

  // First, we must make sure that our window is shown on the screen.
  //   This is ordinary UICL code.

  mainWindow.setClient (&text);
  mainWindow.sizeTo (ISize(500,300));
  mainWindow.setFocus ();
  mainWindow.show ();

  // These should always be the very last statements in the main() function:

  MyMain myMain (&mainWindow);                   // initializes the use of the function MyMain::main
  IApplication::current().run();
  return 0;
};

