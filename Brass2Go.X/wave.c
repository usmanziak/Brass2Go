#include "config.h"
#include "wave.h"
#include "SD.h"
#include "SPI.h"
#include "main.h"

bool fourCCeq(FourCC a, FourCC b) {
    for (char i=0; i<4; i++) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

void readBytes(char* dest, int len) {
    for (int i=0; i<len; i++) {
         SPI_READ(dest[i]);
        ++blockIndex;
        if (blockIndex >= 512) { // end of block condition // format specific
            SD_CloseBlock();
            ++address;
            blockIndex = 0;
        }
    }
    
}

void openFile(long a) {
    SD_OpenStream(a);
    
    {
        RiffTag riff;
        READINTO(riff);
        if (!fourCCeq(riff.ckID, RIFF_CC)) error(RIFF_HEADER);
    }{
        FmtChunk fmt;
        READINTO(fmt);
        if (!fourCCeq(fmt.WAVEID, WAVE_CC)) error(WAVE_HEADER);
        if (!fourCCeq(fmt.fmtHeader.ckID, FMT_CC)) error(FMT_HEADER);
        if (fmt.formatTag != WAVE_FORMAT_PCM) error(CODEC);
        if (fmt.bitsPerSamp != 16) error(BIT_DEPTH);
        
        channels = fmt.channels;
        sampRate = fmt.sampRate;
        
    }{
        RiffTag data;
        READINTO(data);
        if (!fourCCeq(data.ckID, DATA_CC)) error(DATA_HEADER);
        dataLength = data.ckSize;
    }
    
}


