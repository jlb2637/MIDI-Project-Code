# MIDI-Project-Code
The Code to run on an Arduino Micro for my custom MIDI controller project

A summary of the project:
I have a grid of 30 buttons (5x6) and 3 potentiometers(2 knobs, 1 slider) fitted to a 3d printed enclosure able to send MIDI signals over a serial connection to a digital audio workstation(DAW) on a pc. I'm running all of my digital data through an arduino micro to be sent to a PC via serial cable(I want to cut this out in future versions with a more custom solution). As you could imagine, a small arduino does not have 30 digital pins so! I designed a custom parallel-in serial-out shift register PCB so I am only using a single digital I/O pin on the arduino. Currently progress is slow as college takes up quite a bit of time! 

# The Basics
The main button grid plays MIDI notes and the black buttons change the grid to play a different set of notes!

I believe the KiCad files to be on  an old SSD, so for now here are some old photos of the screen(lol) of the schematic and layout:

Here is the Schematic:
![IMG_4496](https://github.com/jlb2637/MIDI-Project-Code/assets/47393028/3c6a9140-a08c-40da-90bb-71652ff9517d)
I'm using TI SN74HC165 Parallel in Serial out shift registers with 10k Ohm resistors on each input, as well as 0.1uf polarized decoupling capacitors between the VDD and GND of each register to filter out any potential voltage spikes.
  
Here is the Layout:
![IMG_4497](https://github.com/jlb2637/MIDI-Project-Code/assets/47393028/f9208187-e540-4cf6-9d01-83b1f0a04513)
As I did not have access to any SMD soldering equipment, I needed to use THT components for everything, hence why the resistors take up so much space!
  
I added a fun little startup LED sequence as shown in the video below:
https://github.com/jlb2637/MIDI-Project-Code/assets/47393028/1f2d1970-3f24-4edf-a286-1d6c569cf97a

And here is (almost) the full device hooked up to a DAW (Reaper in this case) being used to play a virtual instrument!
https://github.com/jlb2637/MIDI-Project-Code/assets/47393028/b4a03d49-669e-4b8d-8d72-53b0125e0704

At this point in time, all of the components have been wired up and installed, I have not had the time to implement everything in software just yet. I'm really leaning towards creating a new version that integrates all of the electronics onto a single custom PCB rather than finishing up the software on this design. 
