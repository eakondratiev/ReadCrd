/*
 * ReadCrd - reading Windows CRD files.
 *
 * MIT License
 *
 * Copyright (c) 2021 E. Kondratiev (https://github.com/eakondratiev/)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include "ReadCrd.h"

/**
 * Main programm.
 */
int main (int argc, char** argv) {

    char *filename;
    enum SourceFileEncoding enc;
    int i;

    enc = ASCII;

    /* excepcted argments: */
    /* ReadCrd filename */
    /* ReadCrd -e Windows-1251 filename */
    if (argc != 2 && argc != 4) {

        Usage (argv[0]);
        return 0;
    }

    if (argc == 4 &&
        strcmp (argv[1], "-e") == 0) {

        if (strcmp (argv[2], "Windows-1251") == 0) {

            enc = Windows1251;

        }
    }

    filename = argv[argc - 1]; /* last argument */
	printf ("\nWindows Card File %s", filename);

    switch (ReadFile (filename, enc)) {
    case -1:
        printf ("\nError reading file.\n\n");
        break;
    case -2:
        printf ("\nNot the Windows Card file.\n\n");
        break;
    case 0:
        printf ("\n\n");
        break;
    }

	return 0;
}

/**
 * Prints the application Usage.
 */
void Usage (char *filePath) {

    char* fileName;
    char* slashPtr;

    /* Get the program name. */
    /* On Linux and Windows the filePath can contain othe folders, */
    /* so we get the program name as characters after the last slash. */
    slashPtr = strrchr(filePath, '/'); /* the last slash on Linux */

    if (slashPtr) {
        fileName = slashPtr + 1;
    }
    else {

        slashPtr = strrchr(filePath, '\\'); /* the last slash on Windows */

        if (slashPtr) {
            fileName = slashPtr + 1;
        }
        else {
            fileName = filePath;
        }
    }

    printf ("\nUsage:\n\n%s [OPTION] file.\n\n"
        "Opens the Windows CRD (contact card) file\n\n"
        "  OPTION:\n"
        "  -e <encoding> decode from this encoding to UTF-8.\n"
        "                currently supports only Windows-1251.\n\n"
        "  EXAMPLE:\n"
        "  %s -e Windows-1251 file.CDR\n\n"
        "  NOTE:\n"
        "  In the Windows terminal use chcp command to set the code page.\n"
        "  For example: \"chcp 1251\" to set the Windwso-1251 code page.\n\n"
        ,
        fileName, fileName);

}

/**
 * Reads the binary file.
 */
int ReadFile (char* filename, enum SourceFileEncoding encoding) {

    FILE *f;
    struct FileSignature cfs;
    struct RRGFileHeader rrgHeader;
    struct MGCFileHeader mgcHeader;
    short totalCards = 0;
    enum CardFileType fileType;
    int retCode = 0;
    int *cardOffsets;
    int i;

    f = fopen(filename, "rb"); /* read, binary */

    if (!f) {
        return -1; // Not opened
    }

    fread (&cfs, sizeof (struct FileSignature), 1, f);

    // Parsing
    if (cfs.letter[0] == 'R' &&
        cfs.letter[1] == 'R' &&
        cfs.letter[2] == 'G') {

        // RRG
        fread (&rrgHeader, sizeof (struct RRGFileHeader), 1, f);
        totalCards = *(short *)rrgHeader.totalCards;
        fileType = RRG;

        printf ("\n\nRRG card file, total cards: %i\n", totalCards);

    }
    else if (cfs.letter[0] == 'M' &&
             cfs.letter[1] == 'G' &&
             cfs.letter[2] == 'C') {

        // MGC
        fread (&mgcHeader, sizeof (struct MGCFileHeader), 1, f);
        totalCards = *(short *)mgcHeader.totalCards;
        fileType = MGC;

        printf ("\n\nMGC card file, total cards: %i\n", totalCards);

    }
    else {
        printf ("\n\nThe signature is not supported: %c%c%c",
            cfs.letter[0], cfs.letter[1], cfs.letter[2]);
        retCode = -2; // Not RRG and not MGC
    }

    if (totalCards > 0) {
        // Allocate memory for card offsets in the file
        cardOffsets = calloc (totalCards, sizeof(int));

        for (i = 0; i < totalCards; i++) {
            ReadCardIndex (f, encoding, cardOffsets, i);
            ReadCard (f, encoding, fileType, cardOffsets [i]);
        }

        free (cardOffsets);
    }

    fclose (f);
    return retCode;
}

/**
 * Reads a card index data (top of the card).
 */
int ReadCardIndex (FILE* f, enum SourceFileEncoding encoding, int* cardOffsets, short cardIndex) {

    size_t indexSize = sizeof (struct CardIndex);
    struct CardIndex ci;
    char* text;

    fread (&ci, indexSize, 1, f);

    if (*(int *)ci.absolutePosition == 0 ||
        ci.flag != 0) {

        /* Empty card or the flag is not zero */
        cardOffsets[cardIndex] = -1;
        return -1;
    }

    cardOffsets[cardIndex] = *(int *)ci.absolutePosition;
    text = getText (ci.indexLineText, encoding);

    /* Left space padding to align the card number. */
    char* spaces = "   ";
    int padLength;

    if (cardIndex < 9) {
        padLength = 2; /* printed number 1...9 */
    }
    else if (cardIndex < 99) {
        padLength = 1; /* printed number 10...99 */
    }
    else {
        padLength = 0;
    }

    /* format specification: %[flags][width][.precision][size]type */
    printf ("\n%.*sCard %i", padLength, spaces, cardIndex + 1);
    printf ("\n--------");
    printf ("\n%s\n", text);

    if (text != ci.indexLineText) {
        free (text); /* it was allocated in getText */
    }

    return 0;
}

/**
 * Reads the card data by the file offset ant type.
 */
int ReadCard (FILE* f, enum SourceFileEncoding encoding, enum CardFileType fileType, int cardOffset) {

    unsigned short bitmapLength;
    unsigned short textLength;
    fpos_t prevPosition;
    char* text = NULL;
    char* utf8text;

    if (cardOffset < 0) {
        return -1;
    }

    /* Save the current read position */
    fgetpos (f, &prevPosition);

    /* Set the file position of the stream: */
    /* move to the place from the file beginning. */
    fseek (f, cardOffset, SEEK_SET);

    // format source: http://cardwin.sourceforge.net/
    switch (fileType) {
        case RRG:

            // read the object flag
            fread (&bitmapLength, sizeof(unsigned short), 1, f);

            if (bitmapLength == 0) {
                // No object
                fread (&textLength, sizeof(unsigned short), 1, f);

                // read the text
                textLength += 3; // by specification
                text = calloc (textLength + 1, sizeof (char));
                text [textLength] = '\0'; // terminate the string
                fread (text, sizeof(char), textLength, f);

            }
            else {
                /* bytes
                 * 2 - 5 UID
                 * 6 - x OLE object
                 *  x+1 -  x+2  DIB character width
                 *  x+3 -  x+4  DIB character height
                 *  x+5 -  x+6  X coordinate U-L
                 *  x+7 -  x+8  Y coordinate U-L
                 *  x+9 - x+10  X coordinate L-R
                 * x+11 - x+12  Y coordinate L-R
                 * x+13 - x+14  embedded=0,linked=1, static=2
                 * x+15 - x+16  length of text entry
                 * x+17 - y     text
                 * WHERE:
                 * x = 6 + size in bytes of OLE object.
                 * y = x + 16 + text entry length
                 * https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-oleds/5a95d153-a771-4408-8041-ae87c45b6987
                 */
                unsigned int uid; // 4 bytes
                unsigned int OLEversion;
                unsigned int formatID;
                unsigned int classNameLength;

                fread (&uid, sizeof(unsigned int), 1, f);
                fread (&OLEversion, sizeof(unsigned int), 1, f); // will be ignored
                fread (&formatID, sizeof(unsigned int), 1, f); // 0x00000003 is CF_METAFILEPICT
                fread (&classNameLength, sizeof(unsigned int), 1, f);

                printf ("\n*** OLE object, not supported,\n"
                          "    UID: %i, format ID: %i, class name len: %i.\n",
                    uid, formatID, classNameLength);
            }

            break;
        case MGC:

            // read the bitmap length
            fread (&bitmapLength, sizeof(unsigned short), 1, f);

            // read the lext length
            if (bitmapLength > 0) {
                fseek (f, 10 + bitmapLength, SEEK_CUR);
            }
            fread (&textLength, sizeof(unsigned short), 1, f);

            // read the text
            text = calloc (textLength + 1, sizeof (char));
            text [textLength] = '\0'; // terminate the string
            fread (text, sizeof(char), textLength, f);

            break;

        default:
            printf ("\nWrong file type.\n");
    }

    if (text) {
        // the text was allocated and read
        // optionally convert to UTF-8
        utf8text = getText (text, encoding); // either text or newly allocated string

        printf ("\n%s\n", utf8text);

        if (text != utf8text) {
            // free converter text
            free (utf8text);
        }
        // free read text
        free (text);
    }

    /* Restore reading position */
    fsetpos (f, &prevPosition);

    return 0;
}

/**
 * Returns text decoded from the encoding.
 */
char* getText (char* text, enum SourceFileEncoding encoding) {

    switch (encoding) {
    case Windows1251:
        return win1251toUtf8(text);

    default:
        /* As is, ASCII */
        return text;
    }

}

/**
 * Returns the new string with Windows-1251 characters (byte)
 * converted to UTF-8 characters (two bytes).
 */
char* win1251toUtf8 (char* src) {

    const int MAX_SIZE = 512;

    unsigned char c; // ASCII (byte) character
    char* res;    // the string beginning
    char* resPtr; // this will be moved
    int   counter = 0;

    res = calloc (MAX_SIZE, sizeof(char));
    resPtr = res;
    memset (res, 0, sizeof(res));

    /*    W1251 UTF8  two bytes representation
     *    ----- ----  ---------------------------
     * А  c0    d090  -48 (d0) -112 (90) \220
     * Б  c1    d091           -111 (91) \221
     * Я  df    d0af            -81 (af) \257
     * а  e0    d0b0            -80 (b0)
     * п  ef    d0bf            -65 (bf)
     * р  f0    d180  -47 (d1) -128 (80)
     * с  f1    d181           -127 (81)
     * я  ff    d18f           -113 (8f)
     * NOTE: -48 dec = d0 hex
     */

    while (*src != '\0') {

        c = (unsigned char)*src;

        if (0xC0 <= c && c <= 0xEF) {
            /* Convert, one char will be two chars */
            /* А c0 -> {d0, 90} ... п ef -> {d0, bf} */
            resPtr[0] = 0xd0;
            resPtr[1] = 0x90 + c - 0xc0; // UTF-8 Cyr A + c - Win1251 Cyr A
            resPtr += 2;
            counter += 2;
        }
        else if (0xF0 <= c && c <= 0xFF) {
            /* Convert, one char will be two chars */
            /* р f0 -> {d1, 80} ... я ff -> {d1, 8f} */
            resPtr[0] = 0xd1;
            resPtr[1] = 0x80 + c - 0xf0; // UTF-8 Cyr р + c - Win1251 Cyr р
            resPtr += 2;
            counter += 2;
        }
        else if (0x80 <= c && c <= 0xBF) {
            // Uncovered characters: quotes, ё, № etc.
            *resPtr = '*';
            ++resPtr;
            ++counter;
        }
        else {
            // just copy
            *resPtr = c;
            ++resPtr;
            ++counter;
        }

        ++src;

        if (counter > MAX_SIZE - 2) {
            break; // too long
        }

    }

    return res;

}
