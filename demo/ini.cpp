// Kroni's Classes: Demonstration: OS/2 ini file access
// (c) 1997 Wolfgang Kronberg
// file: ini.cpp

// ************************************************************************* //
// *                 Demonstration of Kroni's Classes                      * //
// *                                                                       * //
// *                         ini file access                               * //
// *                                                                       * //
// *  Demonstrated classes:                                                * //
// *    KrAsyncMain                                                        * //
// *    KrCommonTextOutput                                                 * //
// *    KrMenuBar                                                          * //
// *    KrProfile                                                          * //
// ************************************************************************* //


#include "krcto.hpp"                             // Support to re-route cout to a window
#include "krasyncm.hpp"                          // Support for asynchronous main function
#include "krmenu.hpp"                            // Support for "correct" menu bar
#include "krprof.hpp"                            // Support for ini file access

#include <iframe.hpp>                            // IFrameWindow


// First of all, all global window objects should be declared static and before all functions.
//   This is the comon UICL way to define a frame window and its childs:

static IFrameWindow mainWindow ("Demonstration of Kroni's Classes: Ini files", 0x1000);
                                                 // Our main window
static KrCommonTextOutput text (IC_FRAME_CLIENT_ID, &mainWindow, &mainWindow);
                                                 // This window will hold all text output
static KrMenuBar menu (&mainWindow);             // Our menu bar


// We need to define a constant for each menu entry we are aplanning to create.

const static int menuWrite = KrAsyncMain::cmdUser + 1;
const static int menuWriteData = KrAsyncMain::cmdUser + 2;
const static int menuWritePicard = KrAsyncMain::cmdUser + 3;
const static int menuDelete = KrAsyncMain::cmdUser + 4;
const static int menuRead = KrAsyncMain::cmdUser + 5;
const static int menuExit = KrAsyncMain::cmdUser + 6;


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

  static KrProfile profile ("KrClassesDemoIni"); // For reading and writing anything which belongs to this
                                                 //   application from/to the OS/2 user ini file.
                                                 //   Be sure to use a name no-one else will choose!
  IString s;

  switch (event.parameter1())                    // Use this to define the different parts of your program
     {
     case cmdStartUp:                            // This part will be run when the program first comes up.

        cout = text.stream();                    // Assign the KrTextOutput object's stream to cout
        cout.setf (ios::unitbuf);                // Unfortunately, the above line does not copy this property

        break;                                   // Our effective main program ends here.

     case menuWriteData :                        // Just use menu item IDs in this place
        profile.setKey ("Data");
        profile << "I'm an android.";            // Writes this text to the "Data" section of the
                                                 //   application's area in the ini file.
        cout << "Written to profile, key ""Data"":\n  'I'm an android.'\n";
        break;

     case menuWritePicard :
        profile.setKey ("Picard");
        profile << "Captain";
        cout << "Written to profile, key ""Picard"":\n  'Captain'\n";
        break;

     case menuDelete :
        profile.removeApp();
        profile << clear;                        // Or the previous data will still be in the buffer
        cout << "All application data deleted.\n";
        break;

     case menuRead :
        profile.setKey ("Data");
        profile >> s;                            // Careful: like in ordinary stream handling, this will
                                                 //   just read one word and not the complete data!
        cout << "Data section, first word: " << s << "\n";
        profile.setKey ("Picard");
        profile >> s;
        cout << "Picard section, first word: " << s << "\n";
        break;

     case menuExit :
        mainWindow.close ();
        break;

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

  // We now build up our menu

  menu.addText (menuWrite, "Write...");          // Our first menu entry...
  menu.addSubmenu (menuWrite);                   // ...will be a pull-down menu
  menu.addText (menuWriteData, "Data", menuWrite);
                                                 // These entries belong to the submenu
  menu.addText (menuWritePicard, "Picard", menuWrite);
  menu.addSeparator (0, menuWrite);
  menu.addText (menuDelete, "Delete", menuWrite);
  menu.addText (menuRead, "Read");               // This does not belong to the submenu
  menu.addText (menuExit, "Quit");

  // These should always be the very last statements in the main() function:

  MyMain myMain (&mainWindow);                   // initializes the use of the function MyMain::main
  IApplication::current().run();
  return 0;
};

