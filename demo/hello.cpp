// Kroni's Classes: Demonstration: "Hello World" program
// (c) 1997 Wolfgang Kronberg
// file: hello.cpp

// ************************************************************************* //
// *                 Demonstration of Kroni's Classes                      * //
// *                                                                       * //
// *                          "Hello, World"                               * //
// *                                                                       * //
// *  Demonstrated classes:                                                * //
// *    KrAsyncMain                                                        * //
// *    KrCommonTextOutput                                                 * //
// ************************************************************************* //


#include "krcto.hpp"                             // Support to re-route cout to a window
#include "krasyncm.hpp"                          // Support for asynchronous main function

#include <iframe.hpp>                            // IFrameWindow


// First of all, all global window objects should be declared static and before all functions.
//   This is the comon UICL way to define a frame window and its childs:

static IFrameWindow mainWindow ("Demonstration of Kroni's Classes: Hello, World", 0x1000);
                                                 // Our main window
static KrCommonTextOutput text (IC_FRAME_CLIENT_ID, &mainWindow, &mainWindow);
                                                 // This window will hold all text output, e.g. for use
                                                 //   with common cout.

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

        cout = text.stream();                    // Assign the KrTextOutput object's stream to cout
        cout.setf (ios::unitbuf);                // Unfortunately, the above line does not copy this property

        cout << "Hello, World!";

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