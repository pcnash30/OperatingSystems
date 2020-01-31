/*test machine: CSEL-KH1260-01
* date: 10/9/19
* name: Peyton Nash, Taylor O'Neill
* x500: nashx282, oneil569
*/

The purpose of the program is implement the functionality of the "make" command into a
  in-house coded executable.

The program is compiled by running "make" in the location of the source file. The
  "make" will generate an executable called "mymake"

The program is able to take in any well formed makefile and do 1 of 4 things:
  1) Run as a normal "make" command when executed in the form of
    "./mymake [makefile]"

  2) Run the "make" command on a specific target within the makefile
    "./mymake [makefile] [target]"

  3) Print out the amount of dependencies and recipes for each target in a given makefile
    "./mymake -p [makefile]"

  4) Print out the contents of each recipe in the correct order of execution of
      a provided makefile
    "./mymake -r [makefile]"

Assumptions:
  1) Any assumption made in section 5 of the project desription document
  2) There is at least 1 target in the provided makefile
  3) The makefile exists in the directory of the executable
  4) For the -p flag, we include a blank lines between targets, and assume to
     not lose points due to the blanks

For project contributions, all work on the project was done side-by-side in the lab.

Extra credit is attempted.
