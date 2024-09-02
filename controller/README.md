## The controller circuit

Here is the controller circuit for remoting the 5480A.

### The schematic

![image](/images/controller-host.png)

### The parts

* 2x 74LS164
* 2x 74LS273
* 2k 1/4W
* 6.2k 1/4W
* 0.1uF 
* LM336Z-2.5
* Ribbon cable
* 20x male pins and shrouds for the 5480A inputs (size?) 

RPi I/O pins are 3.3V. Output pins from an RPi can directly drive TTL inputs, apparently safely. Clamp used for output from the adapter to the RPi.

