# Atari-csm45
Convert text characters representing pixels into mode 4/5 character set data

---

Yet more bastardization introduced to the sprflip utility, this time accepting pixel data and outputting as character set data that can be used for ANTIC mode 4 or 5 text modes.

/* csm45 -- (Character Set Mode 4/5 )
 *
 * The program re-express text representing color character 
 * set data optionally mirrored horizontally and vertically.
 * This will help expedite creating characters that rotate or
 * that need to appear in left-handed and right handed version.
 * Also, this is useful just to create the assembly language
 * and BASIC data based on only a text picture of the data.
 *
 * Output provides bitmap data for character sets in 
 * assembler and BASIC formats.
 *
 * Character data input is expressed as
 * - (0) or (.) period for empty space/background
 * - (1) for COLPF0
 * - (2) for COLPF1
 * - (3) for COLPF2
 * - (4) for COLPF3 (COLPF2 inverse)
 *
 * At this time character data is only 4 pixels per line.
 * Any pixel value other than the characters described
 * will be discarded.
 * More than 4 usable bytes will be discarded.
 *
 * Up to 1024 lines of data may be provided.
 * Groups of 8 lines will be separated into a new comment block 
 * and data output section.
 * The program will ignore further data.
 *
 * A short line of data will be padded with zeros to the right.
 *
 * An outright empty line (no convertible characters) will be ignored.
 *
 * If both '3' and '4' are in the same character a warning 
 * will be displayed, but output will convert both 
 * pixel values to bits '11'.
 *
 * (In the future, this could be expanded to allow for
 * multiple maps for multiple characters on the same  
 * horizontal line.  So, the program will cut the 
 * characters out of a larger pixel map.) 
 *
 * Given input data:   
 *  2 1 0 3 
 *  1 0 3 2
 *  0 3 2 1
 *  3 2 1 0
 *  2 . 1 4  ('4' and '3' in the same set of 8 bytes will create a warning message)
 *  . 1 3 2
 *  1 3
 *  . . 0 0
 *
 * The  assembly output will resemble:
 * ; 0 1 2 3   Bits              
 * ; 2 1 . 3   10 01 00 11 
 * ; 1 . 3 2   01 00 11 10
 * ; . 3 2 1   00 11 10 01
 * ; 3 2 1 .   11 10 01 00
 * ; 2 . 1 4   10 00 01 11
 * ; . 1 3 2   00 01 11 10
 * ; 1 3 . .   01 11 00 00
 * ; . . . .   00 00 00 00 
 * ;
 * ; CSET_
 * ;  .by %10010011
 * ;  .by %01001110
 * ;  .by %00111001
 * ;  .by %11100100
 * ;  .by %10000111
 * ;  .by %00011110
 * ;  .by %01110000
 * ;  .by %00000000
 *
 *
 * The BASIC output will resemble:
 * 1000 DATA 147,78,57,228,135,30,112,0
 *
 * Any additional data will start a new comment block 
 * in assembly, then an assembly data block, or a 
 * DATA line for BASIC.
 *
 */

The block of comments and data described above will be output for each orientation. 
Use command line option -d to choose output.

* -d n == Normal output.
* -d h == Horizontally mirrored.
* -d v == vertically mirrored.

---
