// The CRD file format:
// https://jeffpar.github.io/kbarchive/kb/099/Q99340/

// All numbers in this document, including those in the descriptive text,
// should be interpreted as hexadecimal numerals.
// In the text section of a card's data,
// <CR> is always followed by <LF>.

// Byte #  Description
// -------------------
// 0 - 2   Signature bytes--always "RRG" (52 52 47).
struct FileSignature {

    char letter[3]; // 3 chars

};

// After the signature 3 bytes:
// 3 - 6   Last object's ID.
// 7 - 8   Number of cards in file.
// Total 9 bytes
struct RRGFileHeader {

    char lastObjectId[4]; // 32 bits, 4 bytes
    char totalCards[2];   // 16 bits, 2 bytes
    
};

// After the signature 3 bytes:
// 3 - 4 Number of cards in file (maximum 65535)
struct MGCFileHeader {

    char totalCards[2]; // 16 bits, 2 bytes

};

// Byte #  Description
// -------------------
// 0 - 5   Null bytes, reserved for future use (should all be 00).
// 6 - 9   Absolute position of card data in file.
// A       Flag byte (00).
// B - 32  Index line text. [it's HEX, so x32 - xB = x27 = 39 dec and plus 1 = 40 bytes]
// 33      Null byte; indicates end of index entry.
// Total 34 bytes (x34 = 52 dec)
struct CardIndex {
    char reserved[6];         // all bytes are 00
    char absolutePosition[4]; // int
    char flag;                // 00
    char indexLineText[41];   // last byte is 00
};

// MGC card
struct MGCCard {

    char width[2];
    char height[2];
    char x[2]; // x-coordinate
    char y[2]; // y-coordinate
    // byte 10 
};

// Represents the card file type
enum CardFileType {

    RRG = 0,
    MGC = 1

};

// Represents the source file encoding.
enum SourceFileEncoding {
    ASCII = 0,
    Windows1251 = 1
};

/*
 * Function prototypes
 */
void Usage (char *filePath);
int ReadFile (char *filename, enum SourceFileEncoding encoding);
int ReadCardIndex (FILE *f, enum SourceFileEncoding encoding, int *cardOffsets, short cardIndex);
int ReadCard (FILE *f, enum SourceFileEncoding encoding, enum CardFileType fileType, int cardOffset);
char* getText (char* text, enum SourceFileEncoding encoding);
char* win1251toUtf8 (char *src);
