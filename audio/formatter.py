import sys
import os
import shutil
from mido import MidiFile
import mido

button_dict = {"0": 0, "1": 1, "2": 2,
               "3": 4, "12": 3, "13": 5, "23": 6, "123": 7}
note_button_dict = {
                    52: 7, 53: 5, 54: 6, 55: 3, 56: 1, 57: 2,
                    58: 0, 59: 7, 60: 5, 61: 6, 62: 3, 63: 1, 64: 2, 65: 0,
                    66: 6, 67: 3, 68: 1, 69: 2, 70: 0, 71: 3, 72: 1, 73: 2,
                    74: 0, 75: 1, 76: 2, 77: 0, 78: 6, 79: 3, 80: 1, 81: 2,
                    82: 0, 83: 6, 84: 1, 85: 2, 86: 0, 87: 1, 88: 2, 89: 0,
                    90: 6, 91: 3, 92: 1, 93: 2, 94: 0}


def get_note_sectors(filename):
    """Uses mido to parse MIDI file and return a list in the format of
        [(note, sector_where_note_occurs),...]"""
    mid = MidiFile(filename)
    note_time_list = []
    running_time = 0

    trumpets_found = 0 # number of trumpets found
    trumpet_channel = None

    for msg in mid:
        running_time += msg.time
        if not msg.is_meta:

            if msg.type == 'program_change' and msg.program == 56:
                # for first trumpet found, find it's channel
                if trumpets_found == 0:
                    trumpets_found = 1
                    trumpet_channel = msg.channel
                else: # Multiple trumpet case
                    print("Error: Muliple Trumpets in MIDI file\nQuitting...")
                    quit()

            if ((msg.type == 'note_on' and msg.velocity != 0 and msg.channel == trumpet_channel) or msg.type == 'note_off'):
                    note_time_list.append((msg.note, running_time))

    # Convert times to SD card sector (44100 samples per second,
    # 256 samples per sector and 2 bytes per sample)
    note_sector_list = [(note, int(note_time*44100/256 + 5)) for note, note_time in note_time_list]

    return note_sector_list


def add_note_flags(filename, sector_list, note_list):
    """Sets the 5 least-significant bits of each sector's first byte in
    filename to specify:
                        - if a button needs to be sampled (BIT 0)
                        - which buttons need to be pressed (only if BIT 0 == 1) which flags
                        bits 1-3
                        """

    shutil.copyfile(filename, "formatted-" + filename)
    sound_file = open( "formatted-" + filename, "rb+")
    for sector in range(1, os.path.getsize(filename) // 512):
        sound_file.seek(sector*512)
        original_byte = int.from_bytes(sound_file.read(1), "big")

        if sector in sector_list:  # IF buttons must be pressed
            i = sector_list.index(sector)
            button_bits = note_list[i] << 1
            new_byte = original_byte & 0xFE | 1  # Set LSB to 1
            new_byte = new_byte & 0xF1 | button_bits  # Set bits 1-3 to button
        else:
            new_byte = original_byte & 0xFE | 0  # Set LSB to 0

        sound_file.seek(sector*512)  # seek to correct sector
        sound_file.write(new_byte.to_bytes(1, "big"))  # write to .wav file
    sound_file.close()


def main():
    if len(sys.argv) == 1:
        filename_audio_in = input("Enter the audio (*.wav) file:")
        filename_midi_in = input("Enter the midi (*.mid) file:")
    elif len(sys.argv) == 3:
        filename_audio_in = sys.argv[1]
        filename_midi_in = sys.argv[2]
    else:
        print("""Incorrect number of command-line arguments
Enter no arguments, or supply 2 as shown below:
SCRIPT.py 'INPUT_AUDIO_FILE.wav' 'INPUT_MIDI_FILE.mid'\n""")
        quit()

    # get list of notes and the correcponding sector where the note
    # needs to be played
    note_sector_list = get_note_sectors(filename_midi_in)
    try:
        # seperate notes from notes and sector list, and convert to bit representation
        note_list = [note_button_dict.get(notes) for notes, sectors in note_sector_list]

        sector_list = [sectors for notes, sectors in note_sector_list]

        # encode file with button press data
        add_note_flags(filename_audio_in, sector_list, note_list)
        print("Successfully formatted file to ", "formatted-"+filename_audio_in )
    except OSError:
        print("An error occured with while formatting the file \nQuitting...")
        quit()


main()
