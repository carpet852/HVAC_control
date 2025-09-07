# HVAC control

## Objective
Use an Arduino board to control an AC unit while I am away from home.  
The AC remote control is too basic and does not have a timer/scheduler function.

## Arduino code
The code is based on a Adafruit tutorial [^1].  
There are 2 sketches:
1. ir_remote_decoder.ino => to record the IR sequences directly from the remote control.
2. ir_remote_on_off.ino => to control the AC unit using the recorded IR sequences.

Paste the ON sequence in the function below:
```
// send ON command to Carrier A/C (mode=cool, airflow=low, temp)
void SendOn(int temp) {
  switch (temp) {
  ...
  }
```

Paste the OFF sequence in the function below:
```
// send OFF command to Carrier A/C
void SendOff() {
...
}
```

## Hardware
Refer to the [schematic](https://github.com/carpet852/HVAC_control/blob/main/Hardware/HVAC%20Arduino%20schematics.pdf).
- Arduino UNO
- prototype shield that can be bought on [Aliexpress](https://www.aliexpress.com/item/32219226047.html)
- IR receiver 38kHz: Vishay TSOP4838 (Mouser/Digikey)
- IR diode 940nm: Vishay VSLB3940 (Mouser/Digikey)
- transistor 2N2222 (Mouser/Digikey)

## References
[^1]: https://learn.adafruit.com/ir-sensor/overview
