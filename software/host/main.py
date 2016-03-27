#! /usr/bin/env python3

IF_LEFT = "left.pgm"
IF_RIGHT = "right.pgm"

import PIL.Image as Image

left = Image.open(IF_LEFT)
right = Image.open(IF_RIGHT)

if (left.size != right.size):
	print("Dimention mimatch!")
	

def transfer():
	from serial import Serial
	from macrobull.misc import serialChecker
	
	ser = Serial.open(serialChecker(), baudrate=115200)
	
	
left.show(title="Left")
right.show(title="Right")
#transfer()
