# Brass2Go

Brass2Go is an Engineering design project for the second-year course ENEL 300, Electrical and Computer Engineering Professional Skills, at the University of Calgary.

Brass2Go is a portable device that allows brass musicians to practice their instruments and compositions on-the-go. The device, powered by a PIC16F1778 microcontroller, plays an audio (.wav) file from a microSD card and the user plays the finger sequence along with the song. If the user plays the note incorrectly (i.e. the wrong note and/or at the wrong time) a red LED will light up. Contrastly, a green LED will light up if the note is played correctly. Once the file is finished playing, an LCD screen will display the number of notes played incorrectly, and the percent of notes played correctly (e.g. 5/26 wrong -- 80.7% correct) which helps the user improve at a particular song of their choice. During playback, the user can pause and play as well.


The audio file must have the button/valve press data encoded, which can be done with the Python 3 script [formatter.py](https://github.com/usmanziak/Brass2Go/blob/master/audio/formatter.py).

For correct operation, the following must be satisfied due to system constraints with the PIC16 microcontroller:

* The audio file must be a 16-bit PCM .wav file with a sample rate of 44100 Hz.

* A MIDI (.mid) and its corresponding audio (.wav) file must be supplied to formatter.py

* The formatted .wav file must be added to the SD card without the use of a file system. (Use HxD to load the file onto the SD card)

* All [software prerequisites](#software-prerequisites) must be correctly installed.
  - MPLAB X
  - XC8 Compiler
  - Python 3
  - Mido (pip package)
  - HxD
  - MuseScore (optional)
  - Audacity (optional)


## Hardware Used

The hardware used for this device is:
* PIC16F1778 Microcontroller
* 2-line LCD Display (Midas MC21605G12W‐VNMLWS)
* SPI SD Card Reader (AT25SF321)
* ENEL 343, Circuits II, Audio Amplifier and Filter (TLV4112 Op Amp)
* Red and Green 20mA LEDs
* Switches (MJTP Series)
* 3.5 mm Headphone Adapter
* Resistors (10kΩ for switches, 22Ω for LEDs,  10kΩ potentiometer and 960Ω for volume knob)

The following is the circuit schematic of the final design:

![](schematic/b2g-schematic.png)


The following is an image of the breadboard prototype:

<img src="https://i.imgur.com/lbkdtMU.jpg" alt="Breadboard Prototype" width="500"/>




## Software Prerequisites
The following software must be installed on your Windows, MacOS, or Linux machine:
- MPLAB X
- XC8 Compiler
- Python 3
- Mido (pip package)
- HxD
- MuseScore (optional)
- Audacity (optional)

### MPLAB X and XC8 Compiler
  Click [here](https://www.microchip.com/mplab/mplab-x-ide) to install MPLAB X and the XC8 Compiler on your device. Once installed, you can then compile and program the PIC16F1778 microcontroller with the  Brass2Go.X project. This only needs to be done once.  

### Python 3
  Click [here](https://www.python.org/downloads/) to download the latest release of Python 3. While installing, ensure you select the "Add Python 3.x to PATH" option in the install wizard.

  <img src="https://files.realpython.com/media/win-install-dialog.40e3ded144b0.png" alt="Add to PATH" width="500"/>

  To check if Python 3 is installed correctly, run ```python --version``` in your machine's command prompt / terminal, and ensure it outputs the following:
```
            python --version
            Python 3.x
```

  If you are running a MacOS or Linux machine, or you already have Python 2 installed, postfix all ```python``` commands to ```python3```. E.g. ```python3 --version```

### Mido
  Mido is a python  package that is used by formatter.py to parse MIDI files. Most installations of Python 3 include pip (the Python package installer), but in the event ```pip``` is not a recognized command, click [here](https://pip.pypa.io/en/stable/installing/) to install it. To install mido, run the following command.
```
pip install mido
```


### HxD
   Click [here](https://mh-nexus.de/en/downloads.php) to install HxD onto your machine. HxD is used to load the formatted .wav file to the SD card.

### MuseScore and Audacity

   Click [here](https://musescore.org/en/download) to download MuseScore and [here](https://www.audacityteam.org/download/) to download Audacity. These programs are used to generate the required audio files for formatter.py. If you already have MIDI and .wav files for your composition, these programs are not needed.

   MuseScore is a convenient open-source platform to convert sheet music and MusicXML files to MIDI and audio (.wav) files. Ensure you are using the correct key, and instrument type in MuseScore when you are generating the MIDI and audio files.

   Audacity is an open-source platform to edit and mix audio files. Use Audacity to ensure that the audio (.wav) files that you are using are:

  * Mono
  * 16-bit PCM
  * Sample Rate (Project Rate) = 44100 Hz





## Formatting and Loading Files
  Assuming all prerequisite software is installed on your machine, and that you have correctly created your MIDI (.mid) and audio (.wav) files, follow the following steps to add it to the SD card. As an example, we will format and upload  ```chromatic.mid ``` and ```chromatic.wav``` (located in the ```~\Desktop\Brass2Go\audio``` folder) on a Windows 10 machine.

  1. Add the .mid and .wav files to the ```\Brass2Go\audio``` folder, and navigate to that directory using the command ```cd 'PATH TO Brass2Go\audio' ```

  2. Run the Python 3 script using the command ```python formatter.py audio_file.wav MIDI_file.mid```

   <img src="https://i.imgur.com/DdmZWhn.jpg" alt="Step 2" width="400"/>

  3. Open HxD **'As Administrator'** and open the 'Tools' menu, and press 'Open Disk' (Windows Keyboard Shortcut ```Ctrl-Shift-D```), and open the SD card. Make sure you de-select the 'Open as Readonly' option.

   <img src="https://i.imgur.com/J78QUwx.jpg" alt="Step 2" width="400"/>

  4. Open the file that was formatted by the Python script (```formatted-chromatic.wav ``` in this example). Copy all the data to the clipboard by using the shortcuts ```Ctrl-A``` then ```Ctrl-C```.

  <img src="https://i.imgur.com/IoFoLSR.jpg" alt="Step 2" width="400"/>

  5. Copy all the data into the SD card by using the shortcut ```Ctrl-B```, then save it to the card by using the shortcut ```Ctrl-S```.

  <img src="https://i.imgur.com/2psOK2N.jpg" alt="Step 2" width="400"/>


  The file is now successfully on the SD card. Place the card into the card reader on the device. Then, power on the device, and it should function correctly.

## Authors
* **Luke Garland** - [Github](https://github.com/lukegarland) - [Email](mailto:luke.garland1@ucalgary.ca)

* **Toshiro Taperek** - [Github](https://github.com/robotoshi)

* **Usman Zia** - [Github](https://github.com/usmanziak) - [Email](mailto:muhammadusman.zia@ucalgary.ca)

See also the list of [contributors](https://github.com/usmanziak/Brass2Go/graphs/contributors) who participated and the project's [commit history](https://github.com/usmanziak/Brass2Go/commits/master).

Note: To view full product documentation please access the [Product Documentation](https://github.com/usmanziak/Brass2Go/blob/master/BRASS2GO-Documentation-updated.pdf)

Moreover, check out the final product demonstration slides which includes Product Background and Project Management techniques used throughout product development. [Demonstration](https://github.com/usmanziak/Brass2Go/blob/master/Brass2Go_Final_Presentation.pptx)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
