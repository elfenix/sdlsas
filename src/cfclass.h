#ifndef _CFCLASS_H
#define _CFCLASS_H

/* Compound file v3.0
 * (C) John Remyn 1992-1994
 * booyman@xs4all.hacktic.nl
 *
 * Version 3.0 is a hack job done by me, Brad Pitzel on John's v2.0
 * It's not backward compatible with v2.0 or v1.0.. oh well..
 * See cfclass.c header comments for what I changed..
 *
 * cf.read:
 * If target == NULL, the required memory is allocated by the read routine. The
 * maximum amount of memory allocated is n bytes.
 * If n==0 and target is NULL the required amount of memory is allocated.
 * if filename or nr is 0, the file indicated by readnr is read.
 * Every time a file is read, readnr is incremented.
 *
 *
 * error handling:
 *  if an error occurs the Verrno integer will be != 0. a subsequent call to
 *  errorstr() returns a string with an error message and resets Verrno.
 */

// class' function definitions in are in cfclass.cpp

////////////////////////////////////////////////////////////////////////////
struct cf_filelist {
    char     		*name;
    unsigned long 	truesize;
    long 		offset;
};                                  // element in list of files

typedef struct cf_filelist	Cf_Filelist;

////////////////////////////////////////////////////////////////////////////
class cfc {
    protected:
    	int	Vhandle;
    	Cf_Filelist *Vfile_list;
    	int	VcurFileNum;
    	int	Verrno;
    	int	VnumFiles;                   // nr of files in compound file

    	void	closecf( void );        // closes compound file
    	int	getNumByName( const char *filename );
    	int	readdata( int handle, void *target, int size );

	// these return 0 on success, -1 on error.
	int 	readFileEntry( int handle, Cf_Filelist *entry );
	int 	writeFileEntry( int handle, Cf_Filelist *entry );

    	static	char *cf_signature;
    	static	char *errstr[9];

    public:
    	cfc( void );
    	// constructor
    	
    	~cfc( void ); 
    	// destructor
    	
    	int open( const char *cfilename );        
    	// opens a compound file
    	
    	char *read( const char *filename = NULL, long n = 0, char *target = NULL );
    	char *readoffset( const char *filename, long offset, unsigned long n=0, unsigned char *target=NULL );
    	// reads a file, maxsize n, allocates memory if required
    	// increases VcurFileNum by 1
    	
    	long size( const char *filename = NULL);
    	// returns the size of <filename>, or the file currently pointed to
    	// by VcurFileNum
    	
    	long offset( const char *filename = NULL);
    	// returns the offset of <filename>, or the file currently pointed to
    	// by VcurFileNum

    	int close( void );
    	// closes the comound file
    	
    	int error( void );
        // returns a value not equal to 0 if an error occured
        
    	int nroffiles( void );
    	// returns the number of files in the compound file
    	
    	int setFileNumb( const char *filename );
    	// sets VcurFileNum to <filename>

    	char *currentFilename( void );      
    	// returns name of file currently pointed to

	int  currentFileNum( void ) { return VcurFileNum; }

	// check if current file # is at end-of-list
	int atend(void ) { return VcurFileNum>=VnumFiles; }

	// increment counter to next file
	void next(void) { if (!atend()) VcurFileNum++; }

    	char *errorstr( void );           
    	// returns pointer to string with error desc.
};

#endif
