import sys
import os
import librosa

button_dict = {"0": 0, "1": 1, "2": 2, "3": 3,
               "3": 4, "12": 3, "13": 5, "23": 6, "123": 7}


def get_note_sectors(filename):
    """Uses librosa to find the the sample of each new note
    Returns: List of sample numbers                     """
    # TODO: Downsample audio signal
    y, sr = librosa.load(filename, sr=44100)
    tempo, note_frames = librosa.beat.beat_track(y=y, sr=sr)
    note_sectors = [i * 2 for i in note_frames]
    # each sector is 256 samples
    # We may have to shift the samples so they are triggered a little later to
    # account for human reaction time e.g. [i * 2 + 10 for i in note_frames]
    return note_sectors


def add_note_flags(filename, sector_list, note_list):
    """Sets the 5 least-significant bits of each sector's first byte in
    filename to specify:
                        - if a button needs to be sampled (BIT 0)
                        - if the note is a rest (ignore note)
                        - which buttons need to be pressed (only if BIT 0 == 1)
                        """

    sound_file = open(filename, "rb+")
    for sector in range(1, os.path.getsize(filename) // 512):
        sound_file.seek(sector*512)
        original_byte = int.from_bytes(sound_file.read(1), "big")

        if sector in sector_list:  # IF buttons must be pressed
            i = sector_list.index(sector)
            button_bits = note_list[i] << 1
            new_byte = original_byte & 0xFE | 1  # Set LSB to 1
            # TODO: set button bits
            new_byte = new_byte & 0xF1 | button_bits  # Set bits 1-3 to button
        else:
            new_byte = original_byte & 0xFE | 0  # Set LSB to 0

        sound_file.seek(sector*512)
        sound_file.write(new_byte.to_bytes(1, "big"))
    sound_file.close()


def main():
    if len(sys.argv) != 3:
        print("""Incorrect number of command-line arguments
The specific format should be
SCRIPT.py 'INPUT_AUDIO_FILE.wav' 'INPUT_NOTE_FILE.txt'\n""")
        quit()
    filename_audio_in = sys.argv[1]
    filename_notes_in = sys.argv[2]
    notes_file = open(filename_notes_in)
    note_list = notes_file.read().replace(",", " ").split()

    # Replace commas with whitespace for .split() purposes
    try:
        sector_list = get_note_sectors(filename_audio_in)
        note_list = [button_dict.get(i) for i in note_list]
        add_note_flags(filename_audio_in, sector_list, note_list)
        print("Successfully formatted file")
    except OSError:
        print("An error occured while formatting the file \nQuitting...")


main()
