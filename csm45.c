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
 * The blocks above will be output for each orientation.
 * All data in normal orientation is output first, 
 * Then vertically mirrored, horizontally mirrored, and
 * horizontally+vertically mirrored.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* Output decimal value of byte based on the pixels belonging
 * to the character. 
 */
 
int dumpCharDecimal(char * data,    /* String of pixels */
					int    order,   /* Order to output pixels/bits (0)/left to right, (1)/Right to left */
					int    padme )  /* Comma padding after pixels value? 0/No, 1/Yes. */
{
	int loop     = 0;
	int byte     = 0;
	int findbits = 0;
	char thispixel;

	struct Bitty 
	{
		char id;
		int value[4];
	};

	struct Bitty bits[7] = /* pixel bit values left to right */
	{
		{'.', {    0,    0,    0,    0 } },
		{'0', {    0,    0,    0,    0 } },
		{'1', { 0x40, 0x10, 0x04, 0x01 } },
		{'2', { 0x80, 0x20, 0x08, 0x02 } },
		{'3', { 0xC0, 0x30, 0x0C, 0x03 } },
		{'4', { 0xC0, 0x30, 0x0C, 0x03 } },
		{ 0,  {    0,    0,    0,    0 } }
	};


	if (!data || (order < 0) || (order > 1) )
	{
		fprintf(stderr, "dumpCharDecimal() bad args\n");
		return -1;
	}

	if ( order )  /* If 1 then reverse order */
		data += 3 ; /* start at the end */

	while ( loop < 4 )
	{
		thispixel = *data;

		findbits = 0;

		while ( bits[findbits].id )
		{
			if ( bits[findbits].id == thispixel )
				byte |= bits[findbits].value[loop] ;
		
			findbits++;
		}

		if ( order )
			data--;
		else
			data++;

		loop++;
	}

	fprintf(stdout, "%d", byte);

	if (padme)
		fputc(',',stdout);

	return 0;
}



/* Output '1' or '0' based on the pixels belonging
 * to the character. 
 */
 
int dumpCharBits(char * data,    /* String of pixels */
			   int    order,   /* Order to output pixels/bits (0)/left to right, (1)/Right to left */
			   int    padme )  /* Blank padding between pixels? 0/No, 1/Yes. */
{
	int loop = 0;

	if (!data || (order < 0) || (order > 1) )
	{
		fprintf(stderr, "dumpCharBits() bad args\n");
		return -1;
	}

	if ( order )  /* If 1 then reverse order */
		data += 3 ; /* start at the end */

	while ( loop < 4 )
	{
		switch (*data)
		{
			case '1': 
				fprintf(stdout,"01");
				break;
			
			case '2': 
				fprintf(stdout,"10");
				break;
			
			case '3': 
			case '4':
				fprintf(stdout,"11");
				break;
			
			default:
				fprintf(stdout,"00");
		}

		if (padme)
			fputc(' ',stdout);

		if ( order )
			data--;
		else
			data++;

		loop++;
	}

	return 0;
}



/* A comment line for assembly has the text map of the pixels 
 * followed by the character bitmap on the same line.
 */
 
int commentLine(char * l,
				int    order )
{
	fprintf(stdout,"; ");

	/* Output the text map */
	
	if ( !order ) /* Left to Right */
		fprintf(stdout, "%c %c %c %c   ",
				*(l),*(l+1),*(l+2),*(l+3) );
	else
		fprintf(stdout, "%c %c %c %c   ",
				*(l+3),*(l+2),*(l+1),*(l) );
	
	/*  Output the bits patterns -  '1' is padding between bits */

	dumpCharBits(l, order, 1); 
	
	fputc('\n', stdout);

	return 0;
}


/* Dump the bytes in this group as decimals with copmmad between 
*/

int dumpBASIC(char data[][9], 
			  int   start, 
			  int   end, 
			  int   order )
{
	int    loop = start;
	char * line = NULL;
	int    padme = 1;

	fprintf(stdout,"\n%d DATA ", start+1000 );

	while ( loop != end )
	{
		line = data[loop];
		
		if ( start < end )
		{
			if ( ( end - loop ) <= 1 )
				padme = 0;
		}
		else
		{
			if ( ( loop - end ) <= 1 )
				padme = 0;
		}

		dumpCharDecimal( line, order, padme );

		if ( start < end )
			loop++;
		else
			loop--;
	}

	fputc('\n', stdout);

	return 0;
}



/* Iterate through the pixel rows and output the bits for 
 * a character's pixels.
 */
 
int characterBitmaps(char   data[][9],
					  int    start,
					  int    end,
					  int    order)
{
	int    loop = start;
	char * line      = NULL;

	
	while ( loop != end )
	{
		line = data[loop];

		fprintf(stdout,"\t.by %%"); /* tab indent, and .by data type declaration and % for binary */

		dumpCharBits(line, order, 0); /* NO padding between bits */

		fputc('\n',stdout);
		
		if ( start < end )
			loop++;
		else
			loop--;
	}
	
	return 0;
}



/* Iterate through the pixel rows of a character 
 * block and count the number of times the '3' and 
 * '4' pixels occur.
 * If both are non-zero, report a warning. 
 */
 
int testConflict34(char   data[][9],
					int    start,
					int    end)
{
	int    loop   = start;
	char * line   = NULL;
	int    count3 = 0;
	int    count4 = 0;

//fprintf(stderr,"testConflict34(%d,%d)\n",start,end);
	
	while ( loop != end )
	{
		line = data[loop];
//fprintf(stderr,"testConflict34() '%s'\n",line);
		if ( strchr(line, '3' ) )
			count3++;
		
		if ( strchr(line, '4' ) )
			count4++;
//fprintf(stderr,"testConflict34() '3' = %d\n",(int)strchr(line, '3' ));
//fprintf(stderr,"testConflict34() '4' = %d\n",(int)strchr(line, '4' ));

		if ( start < end )
			loop++;
		else
			loop--;
	}

	if ( count3 && count4 )
		fprintf(stderr,
				"Warning: Block %d (character %d) has '3' and '4'.\n", 
				( start < end ) ? start : end,
				( start < end ) ? start/8 : end/8 );

	return ( count3 + count4 );
}



/* Calculate the next character section boundary */

int calcSection( int sectionStart,
				 int  start,
				 int  end )
{
	int sectionEnd  = 0;

// fprintf(stderr,"calcSection(%d, %d, %d) entry\n",loop,start,end);

	if (start < end )
	{
		sectionEnd = sectionStart + 8;
		if (sectionEnd > end)
			sectionEnd = end;
	}
	else
	{
		sectionEnd = sectionStart - 8;
		if (sectionEnd < end)
			sectionEnd = end;
	}

// fprintf(stderr,"calcSection(%d, %d, %d) exit sectionEnd=%d\n",loop,start,end,sectionEnd);

	return( sectionEnd );
}



/* Dump data lines in order from top to bottom */

int dumpLines( char data[][9],
			   int  start,
			   int  end,
			   int  order ) /* (0) Left To Right  v  (1) Right To Left */
{
	int    loop        = start;
	int    sectionStart = start;
	int    sectionEnd   = 0;
	char * line         = NULL;


	sectionEnd = calcSection( loop, start, end );

	while (loop != end )
	{
		testConflict34( data, sectionStart, sectionEnd);

		/* First Section... Picture of coded pixels and bits in comments. */
		
		fprintf(stdout,"\n; 0 1 2 3   BITS\n"); 	/* Comment Title */

		while (loop != sectionEnd )
		{
			line = data[loop];

			commentLine(line,order);

			if ( start < end )
				loop++;
			else
				loop--;
		}

		/* Next Section.  Character bits. */

		fprintf(stdout,"\nCSET_%d\n",sectionStart/8);

		characterBitmaps(data, sectionStart, sectionEnd, order);

		fputc('\n',stdout);

		dumpBASIC(data, sectionStart, sectionEnd, order);

		sectionStart = loop;

		sectionEnd = calcSection( sectionStart, start, end );
	}

	return 0;
}




int main( int argc, char ** argv )
{
	char line[257];     /* The stdio input buffer. Please don't break me */
	char data[1024][9];  /* Please don't break me either */
	int  numLines = 0 ; /* Number of lines of data read from file.*/
	int  len      = 0 ; /* length of the current line being read.*/
	char * c;
	char * s;

	while ( (c = (char *)fgets( line, 256, stdin ) ) && (numLines < 1024) )
	{
		line[256] = '\0';  /* Force end of string just in case.  I'm lazy. */

		s = c;

		while ( *s )  /* strip/discard invalid chars */
		{
			if ( strchr( ".01234", *s ) )
				*c++ = *s++;
			else
				s++;
		}

		*c = '\0';                    /* Force end of string here. */

		if ( !strlen(line) ) /* no data to process. skip it */
		{
			fprintf(stderr, "No data read at line %d. Skipping...\n",numLines);
			continue;
		}

		strncat(line, "....", 4); /* Force padding in line to at least length 4 */

		line[4] = '\0'; /* force end of string */

		strcpy( data[numLines++], line ); /* Copy line to sprite data array. */ 
	}

	if ( !numLines )
	{
		fprintf(stderr, "No Lines Read\n" );
		exit(0);
	}

	dumpLines(data,0,numLines,0); /* data, start, end++, left to right pixel order) */

//	dumpLines(data,0,numLines,1); /* data, start, end++, right to left pixel order) */

//	dumpLines(data,numLines-1,-1,0); /* data, start, end--, left to right pixel order) */

//	dumpLines(data,numLines-1,-1,1); /* data, start, end--, right to left pixel order) */

	return 0;
}



