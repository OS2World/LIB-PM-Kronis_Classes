 
                  ***  K R O N I ' S   C L A S S E S  ***

              --  A class library for lazy PM programmers  --

Software requirements:  OS/2 Warp  and  Visual Age C++.
Hardware Requirements:  none  (anything which runs Warp and VAC++ is good 
                               enough).

******************************************************************************

Please read the file  doc/legal  BEFORE YOU GO ON!

******************************************************************************

Have you ever...

- wanted to simply say things like cout << s; cin >> s; iniFile << s;
  to output text on the screen, although it is a PM program you're writing,
  to input stuff from dialog windows, and to handle OS/2 ini files?

- wanted to draw your graphics on the printer as easy as on the screen, while
  letting the user of your program interactively change the printer and its
  settings?

- hated your computer for the slow redrawing of some part of the screen you
  just moved an overlapping window away from?

- wanted to draw your graphics in the coordinate system that *you* choose and
  not in some integer coordinate system which has varying sizes depending on
  window size and output device?

- tried to write a function main() which does all the action without caring
  a bit about repainting of windows and such strange stuff?

- longed to do all this but still use the IBM open class library, since apart
  from the above lacks it isn't *that* bad?

- wanted to use a library that is well documented and comes with an
  installation program that does all the stuff, so you don't need a
  semester of research to be able to understand the library?

- had the desire to hack in a program *fast* that will nevertheless be not too
  bad to look at, and that will take full advantage of the OS/2 PM?

Then you certainly need to have a look at Kroni's Classes, since they take
care of all these issues and more!

In addition to the actual programs you are now able to write quickly, the 
source code of the library itself may also be worth a glance.

******************************************************************************

To use Kroni's classes, do the following:

1.  Unpack the archive (you've probably already done this) and ensure that
    there are at least 10MB of disk space free.

2.  Change into the directory (using cd) which install.cmd resides in. Don't
    leave out this step, or the next one won't succeed.

3.  Call install.cmd.

4.  Edit the line 

      SET BOOKSHELF=...

    in your config.sys to contain a dot (".") or the doc directory. If you
    fail to do so, you will not see all links when viewing the online
    documentation. This setting will only work after a shutdown & reboot.

5.  Call programs in the demo directory.

6.  Have a look at the sources in the demo directory

7a. If you don't like what you see, delete the whole software including
    this readme.1st ;) *now*.

7b. If you like what you see, get a postcard *now*, write at least your name
    and the version number of Kroni's Classes onto it, and mail it to:

      Wolfgang Kronberg
      Richard-Linde-Weg 16a
      21033 Hamburg
      Germany

    The postcard could e.g. show your home city (if it's not Hamburg ;) ).
    You might also want to add other details like your address or your
    email address. I will not give any information from the postcards
    away to anybody.

    In addition (but only in addition!) to the postcard, you may of course
    send me anything else, like e.g. comments, poems, Twix, money, gold,
    diamonds, your own software, ... :-)  But that is only for those people
    who have a nice idea and want me to participate. You *need* only to send
    the postcard with the name and the version number.

8.  If you are still reading this because you did not delete it at 6a., you
    may now have a look at the other directories and/or compile your own
    programs, possibly starting from a .cpp file in the demo directory.

9.  Read doc/*.inf to complete your ideas on what you can do with Kroni's
    Classes, and how you can do it.

10. Enjoy!

******************************************************************************

If you have any questions or comments, please mail them to me:

  vlk@amt.comlink.de.

However, don't count on a friendly reply if you didn't send me a postcard in
the first place. :)  Also, please keep all mails to this address short.
I have to pay for the email to this address.

******************************************************************************
