// Copyright 1994 Brad Pitzel
// 
// Feel free to use/distribute/modify as long as credit/copyrights for
// myself 
// and contributors are included.

// File : RawImageLoader.c[1.0]
// Name : gamelib1.0
// Status : proposed
// Changed: Sun Jun 12 22:19:27 1994
// 

#include "riloader.h"

char
 RawImageLoader::Vmagic[] = {
    0x6d,
    0x68,
    0x77,
    0x61,
    0x6e,
    0x68
};


// ///////////////////////////////////////////////////////////////////////
// to ensure no endian ordering problems, this will convert
// the 2 byte numbers from the header into integers.
int
 RawImageLoader::bti(const char p[2]) const
{
    unsigned char pt[2];

    pt[1] = (unsigned char) p[1];
    pt[0] = (unsigned char) p[0];
    return (pt[0] << 8) | pt[1];
}

// ///////////////////////////////////////////////////////////////////////
// call this after loadHeader() for testing/info purposes
void RawImageLoader::displayHeader()
{
    /*
     * cout << "Magic # :" << hex; for(unsigned int i=0;
     * i<sizeof(header.magic); i++) { cout << "0x" << (int)header.magic[i] 
     * << " "; } cout << dec << endl;
     * 
     * cout << "Version :" << bti(header.version) << endl; cout << "Width
     * :" << bti(header.width) << endl; cout << "Height :" <<
     * bti(header.height) << endl; cout << "# colors:" <<
     * bti(header.palSize) << endl; cout << "H dpi :" << bti(header.Hdpi)
     * << endl; cout << "V dpi :" << bti(header.Vdpi) << endl; cout <<
     * "Gamma :" << bti(header.gamma) << endl << endl; 
     */
}

// ///////////////////////////////////////////////////////////////////////
int RawImageLoader::loadHeader(const char *file)
{
    char *buf;

    buf = Vcfile->readoffset(file, 0, sizeof(header), NULL);

    if (buf == NULL) {
	memset(headerBuf, 0, sizeof(header));
	return EXIT_FAILURE;
    }

    memcpy(headerBuf, buf, sizeof(header));
    free(buf);

    // check magic number
    if (strncmp(header.magic, Vmagic, sizeof(header.magic))) {
	cerr << "RawImageLoader:: unknown magic number for file " << file
	    << endl;
	return EXIT_FAILURE;
    }
    // check version
    if (bti(header.version) != 4) {
	cerr << "RawImageLoader:: unknown version "
	    << bti(header.version) << endl;
	return EXIT_FAILURE;
    }
    // decompose header
    Vwidth = bti(header.width);
    Vheight = bti(header.height);
    VimageByteSize = Vwidth * Vheight;
    VnumColors = bti(header.palSize);
    VpaletteByteSize = VnumColors * 3;

    if (VnumColors < 2 || VnumColors > 256) {
	cerr << "RawImageLoader:: image not 8bit paletted (colors="
	    << VnumColors << ")" << endl;
	return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// ///////////////////////////////////////////////////////////////////////
int RawImageLoader::loadFile(const char *file, char flags)
{
    if (loadHeader(file) == EXIT_FAILURE)
	return EXIT_FAILURE;

    // figure out where palette/image info is in file.
    long paletteOffset = sizeof(RawHeader);
    long imageOffset = paletteOffset + VpaletteByteSize;

    if (flags & LOAD_PALETTE) {
	Vpalette = new unsigned char[VpaletteByteSize];

	if (Vpalette == NULL) {
	    cerr << "RawImageLoader:: Cannot allocate memory for palette"
		<< endl;
	    return EXIT_FAILURE;
	}

	Vcfile->readoffset(file, paletteOffset, (long) VpaletteByteSize,
			   Vpalette);
    }

    if (flags & LOAD_IMAGE) {
	Vimage = new unsigned char[VimageByteSize];


	if (Vimage == NULL) {
	    cerr << "RawImageLoader:: Cannot allocate memory for image" <<
		endl;
	    return EXIT_FAILURE;
	}

	Vcfile->readoffset(file, imageOffset, VimageByteSize, Vimage);
    }

    return EXIT_SUCCESS;
}
