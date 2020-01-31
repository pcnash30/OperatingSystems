/*test machine: CSEL-KH1260-16
* date: 10/23/19
* name: Peyton Nash, Taylor O'Neill
* x500: nashx282, oneil569
*/

The purpose of this program is to mimic a lettercount of words within files of a given directory and produce a textfile with the final results.

The program is compiled by running "make" in the location of the source file, "main.c". The
"make" will generate an executable called "mapreduce". Ensure phases 1-4 c files and header files are also located in the same directory.

The program is able to take in any well formed directory (only .txt files, with one word per line, or folders) and traverse through the .txt files and keep a count of how many words start with each letter. The program then produces a FinalResults.txt with the results. The program uses multiple processes and pipes while executing.

Assumptions:
  1) Any assumption made in section 8 of the project desription document
  2) Sample path is less than 100 characters


For project contributions, all work on the project was done side-by-side in the lab.

Extra credit is not attempted.
