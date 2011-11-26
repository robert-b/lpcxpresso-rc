Cool project for R/C, telemetry and engine control

History

14.11.2011
	- use timer1 to capture pulsewidth and print via semihosting
	
16.11.2011
	- read Graupner HoTT SUM signal, connect SUM signal to P1[18] and P1[19]
	- generate trigger signal for oscilloscope at P1[20]
	- generate threshold signal for channel 1 to show some action by switching the LED (P0[22])
	
17.11.2011
	- new capture method to get correct pulse length
	- added doxygen documentation

23.11.2011
	- process up to 8 servo output channels

27.11.2011
	- added RPM module