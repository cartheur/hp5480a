## hp5480a

Code and desicription for the Hewlett-Packard 5480A Display Section and Statistical Measurement Modules.

### The code in the _src_ folder

Inside are the following:

	ctl5480/	: Remote-control program for the 5480.
			  Interactive terminal/line-at-a-time program to be run from a unix shell.
			  C language. Make will produce a binary 'ctl5480'.

	util/		: Some utility routines needed by the ctl5480. compile/make this directory before ctl5480.

	HP5480Schematic.pdf	: Functional schematic of the system. This is mostly a re-presentation of HP's schematics.
				  This was done largely on a 'need' basis, there are some things left out: IC lists, physical locations, some boards of the 2-input analog plug-in.
				  HP's schematics are presented on a physical-module basis, they are redrawn to better present functional-module association.
				  The 4-input analog plug-in is presented, this uses some of the boards of the 2-input plug-in (ADC and 1 or 2 others) but not all.

	HP5480Modules.pdf	: Schematics of functional modules referenced in HP5480Schematic.pdf

	HP5480Timing.pdf	: Timing diagrams for the main timing system and some details for the Prepare state.

	HP5480Adapter.pdf	: Schematic of adapter to interface between the 5480 and controller-host (RPi or other suitable).

My redrawing of the HP schematics may or may not be useful to you. I did that while deriving some functional sense of the design in the absence of the Th-of-Op manual. If you're going between the HP schematics and my schematics you may find the renaming of symbol names annoying, but that, again, was to bring some functional clarity to things. I eventually arrived at breaking the internal system control down to a hierarchy of:
	1. function selection
	2. function execution control
	3. sweep control
	4. sample processing
	5. process micro-cycles
.. and named things to better reflect that.

clt5480 has a limited help: "?" will get you a list of commands/arguments. It is focussed on exercising the accumulator and core memory as that's what I needed to do. I did the clocked-serial style interface just to minimize the number of IO pins to the controller-host.

In using ctl5480, or implementing some alternative remote-control, the major point to be aware of is that to do many/most things from remote control the timing system of the 5480 must be shut down. Otherwise, the memory continues to be scanned, and the address registers modified 'behind your back', as you try to do anything with memory via remote.
The timing system is shut down from remote by asserting nSVQ_MAIN and nSVQ_SUB (HP5480Schematic.pdf.4) (nMBSL asserted holds the timing system in reset).
I simplified this a tad in my unit with the internal mod (in red) so one only has to assert nSVQ_SUB, but the mod is not a requisite (actually, I just ran out of male pins for the rear-panel connectors).

Regarding the rear-panel connectors, I had a bunch of the correct male pins for those connectors but not the base & shroud. There were some on ebay but 50$+ (at that time).
A possibility was using short stubs of common #14 solid copper wire. #14 is slightly larger diameter than the proper pins but will fit. The downside was sharp edges of cut #14 might scratch the gold plating in the female pins and whether the larger diameter is unduly bending the spring metal in the F pins.
