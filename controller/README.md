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

RPi I/O pins are 3.3V. Output pins from an RPi can directly drive TTL inputs, apparently safely. 


* `High` `Low` is detected on falling or rising edge, just putting 1v doesn't produce and edge so to speak and may be inconsistent.
* The `GPIO` inputs are floating on boot but then usually Pulled Down by default when everything is loaded up, so technically applying 1v will produce a rising edge and trigger the `HIGH` Input value.
* If the `GPIO` is already pulled up or has an voltage of ~3.3v, falling from 3.3v to 1v produces a falling edge and produces a `LOW`. So you can see how 1v can be HIGH and LOW because its based on the Rising or Falling Edges.
* This is useful for high frequency communications where the signal will rapidly change between the thresholds you mention but only edges will determine the actual value. This is why its called a Digital Input.
* In logic `LOW` = `0` and `HIGH` = `1` ... what's in between? Floating uncertainty with random noise. As mentioned, these are digital inputs not analogue, you can't tell if the input is 1V or 1.5v, only `1` or `0`.

Clamp used for output from the adapter to the RPi.

