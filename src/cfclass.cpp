/*
 * Compound file class Copyright John Remyn 1994
 * boogyman@xs4all.hacktic.nl 
 */

/*
 * Modified By Brad Pitzel, Oct/94 - allow offset reads - removed the 12
 * char filename limit - prefixed class variables with a V, so you'll know 
 * from where they came. - Removed upper/lower case translation on
 * filenames (mostly in the ccf.c compiler program, not here. - Removed
 * version checking, and storage type (for compression) since I figure
 * compression should be file content dependent, or could be handled by
 * the filesystem. 
 */


// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <ctype.h>
// #include <glob.h>
// #include <iostream>

// using namespace std;

#include "sasteroids.h"


// #include "cfclass.h"

#define SIGLEN 12

// //////////////////////////////////////////////////////////////////////////
char *cfc::cf_signature = "CF3.0       ";

#define NROFSIGS (sizeof( cf_sigs )/sizeof( char * ))

char *cfc::errstr[9] = {
    "no error",			// 0
    "file error",		// 1
    "memory allocation error",	// 2
    "no compound file open",	// 3
    "memory deallocation error",	// 4
    "no more files in compound file",	// 5
    "unknown compound file signature",	// 6
    "cannot close compound file",	// 7
    "file not found in compound"	// 8
};

// //////////////////////////////////////////////////////////////////////////
int
 cfc::readdata(int handle, void *target, int size)
{
    return::read(handle, (char *) target, size);
}


// //////////////////////////////////////////////////////////////////////////
int cfc::getNumByName(const char *filename)
{
    // looks for the VcurFileNum mathing this file, filename==null returns 
    // VcurFileNum
    // returns -1 and sets errno on not found
    // returns -1 if nr beyond file
    int nr;

    if (filename == NULL) {
	nr = VcurFileNum;
    } else {
	int i = 0;

	Cf_Filelist *p = Vfile_list;

	while (strcmp(filename, (char *) (p->name)) != 0 && i < VnumFiles) {
	    i++;
	    p++;
	}
	nr = i;
    }

    if (nr >= VnumFiles) {
	Verrno = 8;
	return -1;
    } else
	return (nr);
}


// //////////////////////////////////////////////////////////////////////////
cfc::cfc(void)
{				// constructor
    Vhandle = 0;
    Vfile_list = NULL;
    VcurFileNum = 0;
    VnumFiles = 0;
    Verrno = 0;
}


// //////////////////////////////////////////////////////////////////////////
cfc::~cfc(void)
{				// destructor
    closecf();
}


// //////////////////////////////////////////////////////////////////////////
char *cfc::errorstr(void)
{
    // Verrno = 0;
    return (errstr[Verrno]);
}

// //////////////////////////////////////////////////////////////////////////
// Read in one entry for a stored file from the .cf file header
int cfc::readFileEntry(int handle, Cf_Filelist * entry)
{
    int name_len = 0;

    entry->name = NULL;
    entry->truesize = 0;
    entry->offset = 0;

    // get the file name length
    readdata(handle, &name_len, sizeof(name_len));

    // sanity check on name length
    if (name_len < 1 || name_len > 256) {
	Verrno = 1;
	return -1;
    }

    entry->name = new char[name_len + 1];

    if (entry->name == NULL) {
	Verrno = 2;		// memory alloc error
	return -1;
    }

    readdata(handle, entry->name, name_len);

    entry->name[name_len] = '\0';

    readdata(handle, &(entry->truesize), sizeof(entry->truesize));

    return 0;
}

// //////////////////////////////////////////////////////////////////////////
// Write one entry for a stored file to the .cf file header
int cfc::writeFileEntry(int handle, Cf_Filelist * entry)
{
    int name_len = strlen(entry->name);

    // write the file name length
    write(handle, &name_len, sizeof(name_len));

    // write the file name
    write(handle, entry->name, strlen(entry->name));

    // write the file size
    write(handle, &(entry->truesize), sizeof(entry->truesize));

    return 0;
}

// //////////////////////////////////////////////////////////////////////////
int cfc::open(const char *cfnam)
{
    // open file
    // create list
    // VcurFileNum = 0
    // return Verrno

    int i;
    char ver[SIGLEN + 1];
    Cf_Filelist *lf;
    long fpointer;

    // open the compound file
    if ((Vhandle =::open(cfnam, O_RDONLY, 0)) == -1) {
	Verrno = 1;
	return (1);
    }
    // check signature (version #)
    readdata(Vhandle, (char *) ver, SIGLEN);
    ver[SIGLEN] = '\0';

    if (strcmp(ver, cf_signature) != 0) {
	closecf();
	Verrno = 6;
	return (Verrno);
    }
    // read the number of files in the cf file
    VnumFiles = 0;
    readdata(Vhandle, (void *) &VnumFiles, sizeof(unsigned short int));

    Vfile_list = (Cf_Filelist *) malloc(VnumFiles * sizeof(Cf_Filelist));

    if (Vfile_list == NULL) {
	Verrno = 2;
	return (Verrno);
    }
    // head in the .cf filelist part of the header, 
    // which has one entry for each stored file.
    lf = Vfile_list;

    for (i = 0; i < VnumFiles; i++) {
	readFileEntry(Vhandle, lf);
	lf++;
    }

    // now, calculate the offsets into the .cf where each stored file
    // begins. First, we get the offset after the header, where the
    // first file is stored:
    fpointer = lseek(Vhandle, 0, SEEK_CUR);

    lf = Vfile_list;
    for (i = 0; i < VnumFiles; i++) {
	lf->offset = fpointer;
	fpointer += lf->truesize;
	lf++;
    }

    VcurFileNum = 0;

    return (Verrno);
}


// //////////////////////////////////////////////////////////////////////////
void cfc::closecf(void)
{
    if (Vhandle) {
	if ((Verrno =::close(Vhandle)) == 0) {
	    Vhandle = 0;
	} else
	    Verrno = 7;
    }

    if (Vfile_list) {
	for (int i = 0; i < VnumFiles; i++) {
	    if (Vfile_list[i].name != NULL)
		delete[]Vfile_list[i].name;
	}

	free(Vfile_list);
	Vfile_list = NULL;
    }
    VcurFileNum = VnumFiles = 0;
}


// //////////////////////////////////////////////////////////////////////////
int cfc::close(void)
{
    // close file
    // free list
    // reset variables
    // return Verrno
    if (Vhandle)
	closecf();
    else
	Verrno = 3;
    return (Verrno);
}

// //////////////////////////////////////////////////////////////////////////
int cfc::error(void)
{
    return Verrno;
}

// //////////////////////////////////////////////////////////////////////////
// read in a stored file from start.
char *cfc::read(const char *filename, long n, char *target)
{
    return cfc::readoffset(filename, 0, n, (unsigned char *) target);
}

// //////////////////////////////////////////////////////////////////////////
// read in a file from a certain offset within that file.  
char *cfc::readoffset(const char *filename, long offset, unsigned long n,
		      unsigned char *target)
{
    // memory is always allocated on far heap
    // check if open
    // if filename == null if VcurFileNum behind list error = no more
    // files
    // else find filename, VcurFileNum =
    // find file ( VcurFileNum );
    // if n==0 n = true filesize
    // if n>true filesize n = true filesize
    // check if target==NULL
    // yes: allocate n bytes
    // if the read routine returns an error memory is freed and
    // VcurFileNum is inc'd
    // so this file will be skipped( hopefully );
    // VcurFileNum = filelist index + 1
    // on error set Verrno, return NULL, free memory
    // return pointer to target (allocated memory)

    Cf_Filelist *p;
    long readlen;
    int mallocedhere = 0;

    if (!Vhandle) {
	Verrno = 3;
    }

    if (filename == NULL)
	filename = currentFilename();

    if (setFileNumb(filename) == -1)
	return (NULL);

    p = Vfile_list + VcurFileNum;

    if (n == 0 || n > (p->truesize - offset)) {
	n = readlen = p->truesize - offset;
    } else {
	readlen = n;
    }

    // see if we are supposed to allocate the mem ourselves.
    if (target == NULL) {
	target = (unsigned char *) malloc(n);
	if (!target) {
	    Verrno = 2;
	    cout << "target " << endl;
	}
	mallocedhere = 1;
    }

    lseek(Vhandle, p->offset + offset, SEEK_SET);

    if (readdata(Vhandle, target, readlen) < readlen) {
	Verrno = 1;
	if (mallocedhere)
	    free(target);
	VcurFileNum++;
	return (NULL);
    }

    VcurFileNum++;
    return ((char *) target);

}



// //////////////////////////////////////////////////////////////////////////
long cfc::size(const char *filename)
{
    if (filename != NULL) {
	if (setFileNumb(filename) == -1) {
	    return (-1);
	}
    }

    if (VcurFileNum >= VnumFiles) {
	Verrno = 5;
	return (-1);
    }

    return ((Vfile_list + VcurFileNum)->truesize);
}

// //////////////////////////////////////////////////////////////////////////
long cfc::offset(const char *filename)
{

    if (filename != NULL) {
	if (setFileNumb(filename) == -1) {
	    return (-1);
	}
    }

    if (VcurFileNum >= VnumFiles) {
	Verrno = 5;
	return (-1);
    }

    return ((Vfile_list + VcurFileNum)->offset);
}


// //////////////////////////////////////////////////////////////////////////
char *cfc::currentFilename(void)
{
    // check if open
    // if VcurFileNum behind filelist Verrno = no more files
    // get filename
    // return filename *, NULL if error

    if (!Vhandle) {
	Verrno = 3;
	return (NULL);
    }

    if (VcurFileNum >= VnumFiles) {
	Verrno = 5;
	return (NULL);
    }

    return ((Vfile_list + VcurFileNum)->name);
}


// //////////////////////////////////////////////////////////////////////////
int cfc::nroffiles(void)
{
    // check if open
    // return VnumFiles
    // returns -1 on error

    if (!Vhandle) {
	Verrno = 3;
	return (-1);
    }

    return (VnumFiles);
}


// //////////////////////////////////////////////////////////////////////////
int cfc::setFileNumb(const char *filename)
{
    // check if open
    // find filename
    // not found return error
    // set new VcurFileNum
    int i;

    if (!Vhandle) {
	Verrno = 3;
	return (-1);
    }

    i = getNumByName(filename);

    if (i == -1) {
	return (-1);		// Verrno is already set by getNumByName
    }

    return (VcurFileNum = i);
}
