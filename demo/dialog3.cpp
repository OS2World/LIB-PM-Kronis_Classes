// Kroni's Classes: Demonstration: Input dialog with user defined classes
// (c) 1997 Wolfgang Kronberg
// file: dialog3.cpp

// ************************************************************************* //
// *                 Demonstration of Kroni's Classes                      * //
// *                                                                       * //
// *              Input Dialog with user defined classes                   * //
// *                                                                       * //
// *  Demonstrated classes:                                                * //
// *    KrAsyncMain                                                        * //
// *    KrCommonTextOutput                                                 * //
// *    KrWinComm                                                          * //
// *    KrBitfield                                                         * //
// *    KrUserDataField                                                    * //
// *    KrTrace                                                            * //
// ************************************************************************* //


#include "krcto.hpp"                             // Support to re-route cout to a window
#include "krasyncm.hpp"                          // Support for asynchronous main function
#include "krwc.hpp"                              // Support for dialog input (windowed communication)
#include "krtrace.hpp"                           // Support for throwing & handling exceptions

#include <iframe.hpp>                            // IFrameWindow
#include <icheckbx.hpp>                          // ICheckBox


// First of all, all global window objects should be declared static and before all functions.
//   This is the comon UICL way to define a frame window and its childs:

static IFrameWindow mainWindow ("Demonstration of Kroni's Classes: User Defined Dialog Classes", 0x1000);
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


// Now let's get to work. We will define an input method for Boolean variables.

// First of all, we must derive a new class from KrUserDataField, and it must have a constructor
//   which accepts a pointer to a Boolean.

class BooleanInput : public KrUserDataField
{

public:

  BooleanInput (Boolean *aBool);                 // The aforementioned constructor
  ~BooleanInput ();                              // To fight memory leaks

  virtual Boolean transform (Boolean doIt);      // These two virtual functions must always be defined.
  virtual int initialize (IMultiCellCanvas & c, int start);

private:

  Boolean * bool;                                // A pointer to the boolean variable to be input
  ICheckBox * checkBox;                          // We'll use this check box for user input

};

BooleanInput::BooleanInput (Boolean * aBool)     // This needs only to do basic initialization
{
  bool = aBool;                                  // Remember the pointer to the variable to be input
  checkBox = 0;                                  // Simply to indicate that this has not yet been defined
};

BooleanInput::~BooleanInput ()
{
  if (checkBox) delete checkBox;
};

Boolean BooleanInput::transform (Boolean doIt)
{
  if (!doIt)                                     // doIt = false: only check for valid input
     return true;                                // No check neccessary: user can't type in invalid input

  if (checkBox)                                  // This should always be the case.
     {                                           // Replace (checkBox) by (!checkBox) to see the exception!
     (*bool) = checkBox->isSelected ();          // Write the check box state into the variable
     }
  else
     KRNAMEDTHROW (IException("CheckBox undefined!"),"Kroni's Classes Demo");
                                                 // Throw an exception. Do the above replacement to see it.
  return true;                                   // The input is valid.
};

int BooleanInput::initialize (IMultiCellCanvas & c, int start)
{
  if (checkBox) delete checkBox;                 // Clean up if neccessary

  checkBox = new ICheckBox (0x1000+0x40*start, &c, &c);
                                                 // Create the actual check box for the user input.
                                                 // You may use this window ID and the next 0x3f here.
  checkBox -> select (*bool);                    // Give the check box the right starting value
  checkBox -> setText ("Yes/No");                // Give some name to the check box to ease clicking

  c.addToCell (checkBox,4,start);                // Column 4 is reserved for the actual entry fields.
                                                 // "start" is the first row you may need.
  return ++start;                                // We return the first un-occupied row.
};


// Now we must overload the >> operator to accept Boolean variables. This is absolutely similar
//   for all types of variables.

KrWinComm & operator >> (KrWinComm & aWinComm, Boolean & aBool)
{
  BooleanInput * bi;
  bi = new BooleanInput (&aBool);                // Create a new object which will survive the exit from
                                                 //   this operator function
  return (aWinComm >> bi);                       // Forward the stuff to this predifined operator
};

// Now we are done defining a new type for input using KrWinComm!


// Next define the asynchronous main function. Doing calculations here does not block message processing.

void MyMain::main (IEvent & event)               // Do in this effective main function whatever you want!
{

  switch (event.parameter1())                    // Use this to define the different parts of your program
     {
     case cmdStartUp:                            // This part will be run when the program first comes up.

        cout = text.stream();                    // Assign the KrTextOutput object's stream to cout
        cout.setf (ios::unitbuf);                // Unfortunately, the above line does not copy this property

        static Boolean b = true;                 // The variable we want to input

        wcin << "I like Star Trek!" >> b >> display;

        cout << "Your choice:\n";
        cout << " You " << (b?"do":"don't") << " like Star Trek!\n";

        break;                                   // Our effective main program ends here.
     };
};


int main (int argc, char **argv, char **envv)    // This should include all initial window definitions;
{                                                //   the other stuff should be in MyMain::main().
  IApplication::current().setArgs(argc,argv);    // This is "good style" when using the UICL.
                                                 //   Do always include this line.
  KrTrace::tellUser (KrTrace::full);             // Give the user full information in a popup window
                                                 //   if any exception occurs!

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

