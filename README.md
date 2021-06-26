# mp2tp README

mp2tp project provides a library to demultiplex a 
MPEG-2 Transport Stream ([ISO/IEC 13818-1])(https://www.iso.org/standard/74427.html#:~:text=ISO%2FIEC%2013818-1%3A2018%20specifies%20the%20system%20layer%20of%20the,the%20synchronization%20of%20multiple%20compressed%20streams%20on%20decoding%3B) file.

The test application, `mp2tp`, will read a MPEG-2 TS file from the 
command-line, and print out TS packets contained in the file to console or a file then exit.

The library (`libmp2t`) and the test application (`mp2tp`) can be build and run on both Windows and 
Linux systems.

## Output Format
The output format from `mp2tp` loosely follows what is described in the 
[standard](https://www.iso.org/standard/74427.html#:~:text=ISO%2FIEC%2013818-1%3A2018%20specifies%20the%20system%20layer%20of%20the,the%20synchronization%20of%20multiple%20compressed%20streams%20on%20decoding%3B), chapter 2.3, "Method of describing bit stream syntax".  It was a
delibrate decision because we want to avoid external dependency on third party libraries.
