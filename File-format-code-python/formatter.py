import sys
import os
from mido import MidiFile
import mido

button_dict = {"0": 0, "1": 1, "2": 2,
               "3": 4, "12": 3, "13": 5, "23": 6, "123": 7}
note_button_dict = {52: 7, 53: 5, 54: 6, 55: 3, 56: 1, 57: 2,
58: 0, 59: 7,60: 5, 61: 6, 62: 3, 63: 1, 64: 2, 65: 0, 66: 6,
67: 3, 68: 1, 69: 2, 70: 0, 71: 3, 72: 1, 73: 2,74: 0, 75: 1,
76: 2, 77: 0, 78: 6, 79: 3,80: 1, 81: 2, 82: 0, 83: 6, 84: 1,
85: 2, 86: 0, 87: 1, 88: 2, 89: 0, 90: 6, 91: 3, 92: 1, 93: 2,
94: 0
}


def get_note_sectors(filename):
    """Uses mido to parse MIDI file and return a list in the format of
        [(note, sector_where_note_occurs),...]"""
    mid = MidiFile(filename)
    note_time_list = []
    running_time = 0
    for msg in mid:
        running_time += msg.time

        if not msg.is_meta:
            if msg.type == 'note_on':

                if msg.velocity != 0:
                    note_time_list.append((msg.note, running_time))
    note_sector_list = [(note, int(note_time*44100/256 + 1)) for note, note_time in note_time_list]
    print(note_time_list)
    return note_sector_list


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
SCRIPT.py 'INPUT_AUDIO_FILE.wav' 'INPUT_MIDI_FILE.mid'\n""")
        quit()
    filename_audio_in = sys.argv[1]
    filename_midi_in = sys.argv[2]

    note_sector_list = get_note_sectors(filename_midi_in)

    try:
        note_list = [note_button_dict.get(notes) for notes, sectors in note_sector_list]
        sector_list = [sectors for notes, sectors in note_sector_list]
        print(note_list)
        add_note_flags(filename_audio_in, sector_list, note_list)
        print("Successfully formatted file")
    except OSError:
        print("An error occured while formatting the file \nQuitting...")


main()
