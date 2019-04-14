#ifndef WAVE_FILE
#define WAVE_FILE

//#include <stdint.h>
#include <xc.h>

#define WAVE_FORMAT_PCM 0x0001
#define SOFTWARE_TAG "Group 11"

#define RIFF_CC "RIFF"
#define WAVE_CC "WAVE"
#define FMT_CC  "fmt "
#define DATA_CC "data"
#define LIST_CC "LIST"
#define INFO_CC "INFO"

#define ISFT_CC "ISFT"
#define INAM_CC "INAM"
#define IART_CC "IART"

#define CUE_CC  "cue "
#define SCOR_CC "scor"
#define TIME_CC "time"
#define TONE_CC "tone"
#define READINTO(s) readBytes((char*)&s, sizeof(s));
#define BUFFER_SIZE 32

unsigned short channels;
unsigned long  sampRate;
unsigned short bitsPerSamp;

unsigned long byteCounter = 0;

char sdata[2];

//SampleFrame sdata;

short lbuffer[BUFFER_SIZE];
short rbuffer[BUFFER_SIZE];
unsigned short buffer_read_index = 0;
unsigned short buffer_write_index = 1;

short blockIndex = 0;
unsigned long dataLength;
char readMessage = 0xFF;


long address = 0;
bool isPlaying = true;
bool samplePending = false;


void openFile(long a);

typedef char FourCC[4];

typedef struct {
    FourCC ckID;
    unsigned long     ckSize;
} RiffTag;

// Nc = channels
// F  = samples per second
// M  = bytes per sample
typedef struct {
    FourCC   WAVEID;       // WAVE_CC
    RiffTag  fmtHeader;    // 
    unsigned short formatTag;    // WAVE_FORMAT_PCM
    unsigned short channels;     // Nc
    unsigned long sampRate;     // F
    unsigned long bytesPerSec;  // F*M*Nc
    unsigned short blockAlign;   // M*Nc
    unsigned short bitsPerSamp;  // rounds up to 8*M (for 1 channel only)
} FmtChunk;

typedef struct {
  unsigned int identifier;
        unsigned long position;
  FourCC   refChunk;     // DATA_CC
  unsigned long chunkStart;   // 0
  unsigned long blockStart;   // 0
        unsigned long sampleOffset;
} CuePoint;

typedef struct {
    RiffTag  Header;
    unsigned long points;
    CuePoint cues[];
} CueChunk;

typedef struct {
    RiffTag  header;
    char   instrument;
    unsigned char  nTimes;
    unsigned short nTones;
} ScorHeader;

typedef struct {
    unsigned char timeID;  
          unsigned char timeSig;
          short tempo;    // sign bit represents a gradual change in tempo
} TimeFrame;

typedef struct {
    unsigned short toneID;
          unsigned long position;
          unsigned long duration;
          unsigned char  combination;
          char   pitch;
} ToneFrame;


typedef struct {
    short left;
    short right;
} Stereo16;

//typedef short Mono16;

typedef union {
    short mono16;
    Stereo16 stereo16;
    char bytes[4];
} SampleFrame;

//int fourCCeq(FourCC a, FourCC b) {
//    for (char i=0; i<4; i++) {
//        if (a[i] != b[i]) return 0;
//    }
//    return 1;
//}

#endif

/*
RIFF
    WAVE
        fmt
        LIST
            INFO
                ISFT
                INAM
                IART
        scor
            time
            tone
        data

*/