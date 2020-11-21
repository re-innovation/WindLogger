EESchema Schematic File Version 2
LIBS:WindLoggerv3_PCB-rescue
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:1wire
LIBS:arduino_shieldsNCL
LIBS:atmel-1
LIBS:atmel-2005
LIBS:philips
LIBS:nxp
LIBS:matts_components
LIBS:linear2
LIBS:WindLoggerv3_PCB-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 6 6
Title ""
Date "25 dec 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_8 P15
U 1 1 5464FB9E
P 5700 3150
F 0 "P15" V 5650 3150 60  0000 C CNN
F 1 "SIM900 GSM" V 5750 3150 60  0000 C CNN
F 2 "" H 5700 3150 60  0000 C CNN
F 3 "" H 5700 3150 60  0000 C CNN
	1    5700 3150
	1    0    0    -1  
$EndComp
Text Notes 5500 3750 0    60   ~ 0
1: +\n2: -\n3: DR\n4: DT\n5: Rx\n6: Tx\n7: P\n8: RST\n\n
Text HLabel 4500 2800 0    60   Input ~ 0
+
Text HLabel 4500 2900 0    60   Input ~ 0
GND
Text HLabel 4650 3300 0    60   Input ~ 0
TxGSM
Text HLabel 4650 3200 0    60   Input ~ 0
RxGSM
Text HLabel 4650 3400 0    60   Input ~ 0
P_GSM
Wire Wire Line
	4500 2800 5350 2800
Wire Wire Line
	4650 3200 5350 3200
Wire Wire Line
	4650 3300 5350 3300
Wire Wire Line
	4650 3400 5350 3400
Wire Wire Line
	4500 2900 5350 2900
$Comp
L CP1 C14
U 1 1 5465032C
P 5000 2550
F 0 "C14" H 5050 2650 50  0000 L CNN
F 1 "10uf" H 5050 2450 50  0000 L CNN
F 2 "" H 5000 2550 60  0000 C CNN
F 3 "" H 5000 2550 60  0000 C CNN
	1    5000 2550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5200 2550 5250 2550
Wire Wire Line
	5250 2550 5250 2900
Connection ~ 5250 2900
Wire Wire Line
	4800 2550 4750 2550
Wire Wire Line
	4750 2550 4750 2800
Connection ~ 4750 2800
$EndSCHEMATC
