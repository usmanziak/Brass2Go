#ifndef B2G_ERROR_H
#define B2G_ERROR_H

typedef enum {
    OPEN_BLOCK,
    RIFF_HEADER,
    WAVE_HEADER,
    FMT_HEADER,
    CODEC,
    BIT_DEPTH,
    DATA_HEADER,
    CHANNELS
} Error;

Error global_error;

void error(Error e) {
    // tell someone something went wrong
    global_error = e;
    __nop();
}


#endif