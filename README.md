mp2tp README
============

MPEG-2 Transport Stream (ISO/IEC 13818-1) parser and library.

Cross-platform command-line application and library that will run on Windows 
and Linux.  The test application, `mp2tp`, will read a MPEG-2 TS file from the 
command-line, and print out the first 10,000 TS packets to console then exit.

## Output Format
The output format from `mp2tp` loosely follows what is described in the 
standard, chapter 2.3, "Method of describing bit stream syntax".  It was a
delibrate decision because we want not to be depended on any external libraries.