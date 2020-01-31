/*test machine: CSEL-KH1260-16
* date: 11/13/19
* name: Peyton Nash, Taylor O'Neill
* x500: nashx282, oneil569
*/

The purpose of this program is to mimic a lettercount of words within files of a given directory and produce a textfile with the final results.

The program is compiled by running "make" in the location of the source files, "main.c, producer.c, consumer.c". The
"make" will generate an executable called "wcs". Ensure source files and header files are also located in the same directory.

The program is able to take in any .txt file and traverse through it and keep a count of how many words start with each letter. The program then produces a FinalResults.txt with the results. The program uses multiple threads and follows a producer/consumer model.

Assumptions:
  1) Any assumption made in section 8 of the project desription document
  2) Sample path is less than 100 characters


For project contributions, all work on the project was done side-by-side in the lab.

Extra credit is attempted.
