# Robockey
Robot that autonomously locates an IR- emitting puck and score it into the opponents goal against other robot
This repository contains a number of programs to be flashed on to a Maevarm 2 (M2) microcontroller.

## What programs are in this repository?
### Clean
This is a set of clean files for new projects that is used as the base file-structure and framework for new projects.

### localizeMe
This is a Matlab program to test that the robot can determine it's location on the field relative to a constellation of IR-diodes mounted on the ceiling.

### qualifiersCode
This is a C program that controls the robot and allows it to show rudimentary autonomous navigation by driving to the opponents goal.

### robotcode
This is the directory for the final C program that is used on the robot for competition.

## Flashing programs onto the M2
Instructions vary across different operating systems:

* [Windows](http://medesign.seas.upenn.edu/index.php/Guides/MaEvArM-starting)
* [OS X](http://medesign.seas.upenn.edu/index.php/Guides/MaEvArM-starting-mac)
* [Linux](http://medesign.seas.upenn.edu/index.php/Guides/MaEvArM-starting-linux)

**Note:** The files and directories of each project in this repository are structured to be flashed to the M2 from a Linux machine.
## Further Information
For more information, please see the Maevarm documentation on the [MEAM.Design](http://medesign.seas.upenn.edu/index.php/Guides/MaEvArM) page.
