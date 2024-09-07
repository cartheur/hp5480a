## The controller circuit

Here is the controller circuit for remoting the 5480A.

### The schematic

![image](/images/controller-host.png)

### The parts

* 2x 74LS164 -  `SIPO`.
* 2x 74LS273 - it has a positive-edge trigger and can operate at up to 30 MHz clock frequency.
* 2k 1/4W
* 6.2k 1/4W
* 0.1uF 
* LM336Z-2.5
* Rather than the ribbon cable, which seems really difficult to find in this older, hand-friendly form-factor. Pins were also a difficult proposition but since the indication of AWG16 was provided, smooth and solder-capable pins there were the exact size (barring a dollop of machine oil), these are: 

* Jumper wires from Adafruit
    - Mouser p/n: 485-4447 and 485-4482
    - ![image](/controller/images/jumper-wires.png)
* Pins 
    - Mouser p/n:  571-1060-16-0122-LP
    - ![image](/controller/images/16z-pin.png)
* LM336Z-2.5 - Clamp used for output from the adapter to the RPi
    - ![image](/controller/images/lm336z-kon.png)
    - ![image](/controller/images/LM336-pins.png)
    - ![image](/controller/images/lm336-hookup.png)
* Connected into this configuration
    - ![image](/controller/images/lm366-power-setup.png)
* Leveraged in the circuit between the 5480A and the Pi
    - ![image](/controller/images/lm366-circuit-place.png)
    - To conform to RPi I/O pins are 3.3V. Output pins from an RPi can safely directly drive TTL inputs
* `HIGH-LOW` is detected on falling or rising edge, just putting 1v doesn't produce and edge so to speak and may be inconsistent.
    - The `GPIO` inputs are floating on boot but then usually Pulled Down by default when everything is loaded up, so technically applying 1v will produce a rising edge and trigger the `HIGH` Input value.
* If the `GPIO` is already pulled up or has an voltage of ~3.3v, falling from 3.3v to 1v produces a falling edge and produces a `LOW`. So you can see how 1v can be `HIGH` and `LOW` because its based on the Rising or Falling Edges.
    - This is useful for high frequency communications where the signal will rapidly change between the thresholds you mention but only edges will determine the actual value. This is why its called a Digital Input.
    - In logic `LOW` = `0` and `HIGH` = `1` ... what's in between? Floating uncertainty with random noise. As mentioned, these are digital inputs not analogue, you can't tell if the input is 1V or 1.5v, only `1` or `0`.
* 74LS164 - An 8-bit parallel-out serial shift register
    - ![image](/images/74164.jpg)
* 74LS263 - An octal D-type flip-flop with clear input and single-rail outputs
    - ![image](/images/74273.jpg)

Where the board looks like this, when assembled:

![image](/images/controller.jpg)

Where it is important to understand what these four connectors are purposed

* ![image](/controller/images/logic-conn-01.png)
* ![image](/controller/images/logic-pin-table-01.png)
* ![image](/controller/images/logic-pin-table-02.png)
* ![image](/controller/images/logic-pin-table-03.png)

-----

## What is the outcome

Asembling this system using Adafruit jumper wires and sets of jumper pins from Mouser...

-----

## Taking the experiment a set further

There is this bit of interesting-ness to the application

* [here](/data-input/README.md)