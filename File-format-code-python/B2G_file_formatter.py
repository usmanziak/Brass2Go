import librosa


def get_note_sectors(filename):
    """Uses librosa to find the the sample of each new note
    Returns: List of sample numbers                     """

    y, sr = librosa.load(filename, sr=44100)
    tempo, note_frames = librosa.beat.beat_track(y=y, sr=sr)
    print("Estimated Tempo : {:.2f}".format(tempo))
    note_sectors = [i * 2 for i in note_frames]
    return note_sectors


def add_note_flags(filename, sector_list):
    """Sets the 5 least-significant bits of each sector's first byte in
    filename to specify:
                        - if a button needs to be sampled (BIT 0)
                        - if the note is a rest (ignore note)
                        - which buttons need to be pressed (only if BIT 0 == 1)
                        """

    sound_file = open(filename, "rb+")
    for sector in range(1, 10):
        sound_file.seek(sector*512)
        original_byte = int.from_bytes(sound_file.read(1), "big")

        if sector in sector_list:  # IF buttons must be pressed

            new_byte = original_byte & 0xFE | 1  # Set LSB to 1
            # TODO: set button bits and rest bit
        else:
            new_byte = original_byte & 0xFE | 0  # Set LSB to 0

        sound_file.seek(sector*512)
        sound_file.write(new_byte.to_bytes(1, "big"))
    sound_file.close()
