// Kroni's Classes: Demonstration: Input dialog with special classes
// (c) 1997 Wolfgang Kronberg
// file: dialog2.cpp

// ************************************************************************* //
// *                 Demonstration of Kroni's Classes                      * //
// *                                                                       * //
// *                 Input Dialog with special classes                     * //
// *                                                                       * //
// *  Demonstrated classes:                                                * //
// *    KrAsyncMain                                                        * //
// *    KrCommonTextOutput                                                 * //
// *    KrWinComm                                                          * //
// *    KrBitfield                                                         * //
// *    KrChoice                                                           * //
// ************************************************************************* //


#include "krcto.hpp"                             // Support to re-route cout to a window
#include "krasyncm.hpp"                          // Support for asynchronous main function
#include "krwc.hpp"                              // Support for dialog input (windowed communication)

#include <iframe.hpp>                            // IFrameWindow


// First of all, all global window objects should be declared static and before all functions.
//   This is the comon UICL way to define a frame window and its childs:

static IFrameWindow mainWindow ("Demonstration of Kroni's Classes: Special Dialog Classes", 0x1000);
                                                 // Our main window
static KrCommonTextOutput text (IC_FRAME_CLIENT_ID, &mainWindow, &mainWindow);
                                                 // This window will hold all text output

// The object which take user input does not need to be defined here, but it's easier that way.

static KrWinComm wcin;


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

  switch (event.parameter1())                    // Use this to define the different parts of your program
     {
     case cmdStartUp:                            // This part will be run when the program first comes up.

        cout = text.stream();                    // Assign the KrTextOutput object's stream to cout
        cout.setf (ios::unitbuf);                // Unfortunately, the above line does not copy this property

        static KrBitfield bf;                    // Let the user click several options on/off independently
        int bf1 = bf.add ("Space");              // The int variables are used to identify the item later.
        int bf2 = bf.add ("Lots of Money");      // You may also define them a priori, but this is easier.
                                                 // All options are off by default, but his can be changed.

        static KrChoice ch;                      // Let the user choose exactly one of several choices.
        int ch1 = ch.add ("Betazoid");
        int ch2 = ch.add ("Klingon");
        ch.set (ch1);                            // One choice should be set as the default.

        wcin << "'Enterprise' has something to do with..." >> bf;
        wcin << "Deanna Troi is..." >> ch
             >> display;                         // The display manipulator will work with both
                                                 //   the >> and the << operator.

        cout << "Your choices:\n";
        cout << " 'Enterprise' has " << (bf.isChecked(bf1)?"something":"nothing") << " to do with Space\n";
        cout << " 'Enterprise' has " << (bf.isChecked(bf2)?"something":"nothing")
             << " to do with Lots of Money\n";
        cout << " Deanna Troi is " << (ch.get()==ch1?"Betazoid":"Klingon") << "\n";

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