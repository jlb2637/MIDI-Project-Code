# MIDI-Project-Code
The Code to run on an Arduino Micro for my custom MIDI controller project

A summary of the project:
I have a grid of 30 buttons (5x6) and 3 potentiometers(2 knobs, 1 slider) fitted to a 3d printed enclosure able to send MIDI signals over a serial connection to a digital audio workstation(DAW) on a pc. I'm running all of my digital data through an arduino micro to be sent to a PC via serial cable(I want to cut this out in future versions with a more custom solution). As you could imagine, a small arduino does not have 30 digital pins so! I designed a custom parallel-in serial-out shift register PCB so I am only using a single digital I/O pin on the arduino. Currently progress is slow as college takes up quite a bit of time! 

# Early May (2022) Update
The semester is over and I have time to work!
The main button grid plays MIDI notes and the black buttons change the grid to play a different set of notes!

I believe the KiCad files to be on  an old SSD, so for now here are some old photos of the screen(lol) of the schematic and layout:

Here is the Schematic:
![IMG_4496](https://github.com/jlb2637/MIDI-Project-Code/assets/47393028/3dc260b8-ef8c-4245-81af-16638e1edcb8)
I'm using<blank> shift registers
  
Here is the Layout:
![IMG_4497](https://github.com/jlb2637/MIDI-Project-Code/assets/47393028/18a8ef66-7f48-44cf-8812-6dc06b6a7507)
  

Bugs:
  -Changing main button grids will keep playing all previous notes (is this a bug even?)
  -notes are skipped because the black buttons are not longer used to play notes
