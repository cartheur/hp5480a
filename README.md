## The Hewlett-Packard 5480a Signal Analyzer

The Hewlett-Packard 5480A Digital Signal Analyser was introduced in April 1968. Also known as a signal averager or enhancer, the 5480A Signal Analyzer recovers repetitive signals buried in noise by averaging out that noise. The Model 5480A Signal Analyzer applies statistical principles for real time analysis of data and signal-to-noise improvement.

* OPTION 01 (VARIANCE OPTION)
	- DESCRIPTION
	This Option, available for both the 5485A and the 5487A Input Plug-Ins, enables use of the 5480A VARIANCE OUTPUT signal to check validity of the averaged signal. The Option may be ordered installed and tested at the factory, or it may be installed in the field. Field installation consists of replacing the Switching Logic B Assembly (05485-60010) with Switching Logic B Assembly (05485-60020); no wiring changesare required.

Accurate, detailed information is made available through signal averaging; an example of the results of this technique applied to nerve response is shown in Figure 1.

![fig.1]()

The render of this data on an hp 141S/8553L/8552A Spectrum analyzer is shown in Figure 2.

![fig.2]()

The render of this data on the hp 5480A is shown in Figure 3.

![fig.3]()

Many other applications exist for the 5480A in the fields of medicine, bio-medicine, chemistry, physics, electronics, astronomy, vibration, and turbulance. The plug-in design of the 5480A provides a more versatile instrument and guards against obsolescence. The 5480A Mainframe contains a 1024 word, 24-bit magnetic core memory with related circuits and a CRT display while the two plug-ins chosen (5486A, and either the 5485A, 5487A, or 5488A) depend upon the specific application.

In the case of the work at-hand: Discovery of the phenomenon active in a Model 'D' oscillator.

The 5480A also does multi-channel scaling, and time and frequency histograms. With some external pulse-amplitude discriminators, it could be a pulse-height analyzer. It appears to be an attempt by HP to address several small markets for specialised instruments by producing one multi-function instrument. The basic specifications of the 5480A are 9/7/5-bit A-D resolution, up to 100 kHz A-D conversion rate, and 1024*24-bit memory. The display resolution is 1000 Horizonal * 1024 Vertical.

The 5480A was announced in HP's Spring 1968 _New Instruments_ catalog, and is last listed in the 1973 catalog. The 1970 _Electronics for Measurement, Analysis, and Computation_, pp.42-3, lists the price for the corresponding 5480 configuration

| Photo | Part number | Description | Price |
|-------|-------------|:-------------:|------:|
|![5480](/images/5480A.jpg)| 5480A |  Memory Display     | $6950 |
|![5485](/images/5485A.jpg)| 5485A |  Two-Channel Input  | $1500 |
|![5486](/images/5486A.jpg)| 5486A |  Control Unit       | $1500 |
|![5488](/images/5488A.jpg)| 5488A |  Correlator Input   | $2500 |

 This would be a total cost of $12,450, which inflates to $99,576.26 as of February 2024. Its listed weight with plug-in modules is 34.5 kg. 

## What is in here?

Code and desicription for the Hewlett-Packard 5480A Display Section and Statistical Measurement Modules.

### Building the controller code for Raspberry Pi

Prequisite:

* Clone the [wiringPi](https://github.com/WiringPi/WiringPi) repository, follow the instructions to install on Linux

* Step into the `/src/util` folder and run `make`
* Step into the `/src/crl5480` folder and run `make`
* The binary is build and ready to use.

`clt5480` has a limited help: `?` will get you a list of commands/arguments. It is focussed on exercising the accumulator and core memory as that's what I needed to do. I did the clocked-serial style interface just to minimize the number of IO pins to the controller-host.

### Things to be aware of when using the application

* In using `ctl5480`, or implementing some alternative remote-control, the major point to be aware of is that to do many/most things from remote control the timing system of the 5480 must be shut down; otherwise, _the memory continues to be scanned, and the address registers modified 'behind your back', as you try to do anything with memory via remote_.
* The timing system is shut down from remote by asserting `nSVQ_MAIN` and `nSVQ_SUB` (HP5480Schematic.pdf.4) (`nMBSL` asserted holds the timing system in reset). I simplified this a tad in my unit with the internal mod (in red) so one only has to assert `nSVQ_SUB`, but the mod is not a requisite (actually, I just ran out of male pins for the rear-panel connectors).

### 5480A Block Diagram

![block](/images/block-diagram.png)

My redrawing of the HP schematics may or may not be useful to you. I did that while deriving some functional sense of the design in the absence of the Operating or Service manuals; although a source on microfiche was discovered and scans are in the [manuals](/manuals/README.md) folder. If you're going between the HP schematics and my schematics you may find the renaming of symbol names annoying, but that, again, was to bring some functional clarity to things. I eventually arrived at breaking the internal system control down to a hierarchy of:

1. Function selection
2. Function execution control
3. Sweep control
4. Sample processing
5. Process micro-cycles

.. and named things to better reflect that.

### The controller schematic

In order to perform remote-control of this unit, a system needs to be constructed of the following.

![schematic](/images/controller-host.png)

Regarding the rear-panel connectors, I had a bunch of the correct male pins for those connectors but not the base & shroud. There were some on ebay but 50$+ (at that time). A possibility was using short stubs of common #14 solid copper wire. #14 is slightly larger diameter than the proper pins but will fit. The downside was sharp edges of cut #14 might scratch the gold plating in the female pins and whether the larger diameter is unduly bending the spring metal in the F pins.

The specific details of how this circuit is connected and the meaning of the nomenclature is noted [here](/controller/schematic/README.md).

### Organization of this repository

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

### Errata

* This code was received from the author [here](http://madrona.ca/e/HP5480A/index.html).
* Read the [unit](/unit/DSA-Unit-Log.html) log.