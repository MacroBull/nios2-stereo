#! /usr/bin/env python3

IF_LEFT = "left.pgm"
IF_RIGHT = "right.pgm"
IF_TOF = "tof.pgm"
IF_DISP = "disparity.pgm"

PARAMS=[100,100]

PGM_MAGIC="P5"
FLAG0="START\n"
FLAG1="BOOM\n"
ACK="ACK\n"
NAK="NAK\n"

import sys

if len(sys.argv)>1 and sys.argv[1] == '-d':
	pass
else:
	import os
	f = os.popen("cd ../Stereo && /opt/Altera/QuartusPrime-15.1/nios2eds/nios2_command_shell.sh make -j6 download-elf")
	for l in f:
		print(l, end="")

import PIL.Image as Image
import time


left = Image.open(IF_LEFT)
right = Image.open(IF_RIGHT)
tof = Image.open(IF_TOF)


def validate():
	dim = left.size
	if (dim != right.size) or (dim != tof.size):
		print("Dimention mimatch!")
	
def getResp(ser):
	try:
		resp = ser.readline().decode('utf-8')#.rstrip()
	except UnicodeDecodeError:
		resp = ""
		
	print("getResp:<-" + resp.rstrip())
	return resp

def crc32(data, crc = 0): #The internal crc32 polynomial sucks
	crc = 0xffffffff & ~crc
	for d in data:
		crc = crc ^ d
		for j in range(8):
			crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1))
	return 0xffffffff & ~crc

def watiFor(ser, myFlag = ACK, target = ACK):
	resp = ""
	while resp!=target: 
		ser.write(myFlag.encode("utf-8"))
		print("watiFor:->" + myFlag.rstrip())
		resp = getResp(ser)

def transfer():
	from serial import Serial
	from macrobull.misc import serialChecker
	
	ser = Serial(serialChecker(), 115200, timeout=0.1)
	watiFor(ser, FLAG0)
	ts0 = time.time()	
	print("Sending parameters...")
	ser.write("{}\n".format(",".join([str(p) for p in PARAMS])).encode('utf-8'))
	watiFor(ser, FLAG1)
	print("Sending images...")
	checksum = "x"
	for fn in [IF_LEFT, IF_RIGHT, IF_TOF]:
		resp = ""
		while resp!= checksum:
			f = open(fn, 'rb')
			data = f.read()
			f.close()
			ser.write(data)
			for i in range(3): data = data[data.index(b'\n')+1:]
			checksum = "{0:08x}".format(crc32(data))
			resp = getResp(ser).rstrip()
			if resp!= checksum:
				print("CRC mismatch, expected:{}, response:{}".format(checksum, resp))
				ser.write(NAK.encode("utf-8"))
		ser.write(ACK.encode("utf-8"))
		
	print("Sending finished.")
	ts1 = time.time()	
	watiFor(ser, ACK, FLAG1);
	ts2 = time.time()	
	print("Running time:{}".format(ts2 - ts1))
	print("Receiving images...")
	
	resp = ""
	while resp!= ACK:
		buf = bytearray()
		l = ""
		while l!=(PGM_MAGIC+"\n").encode('utf-8'):
			l = ser.readline()
		buf += l#MAGIC
		l = ser.readline();buf += l#dimention
		width, height = [int(n) for n in l.decode('utf-8').rstrip().split()]
		print("Image size:{}x{}".format(width, height))
		l = ser.readline();buf += l#white
		
		data = bytearray()
		size = width*height
		while size>0:
			buf1 = ser.read(size)
			data += buf1
			size -= len(buf1)
		
		buf += data
		f=open(IF_DISP, 'wb')
		f.write(buf);
		f.close()
		
		checksum = "{0:08x}".format(crc32(data))
		checksum += "\n"
		ser.write(checksum.encode('utf-8'))
		resp = getResp(ser)
	
	print("Receiving finished.")
	ts3 = time.time()
	print("Time on io:{}, processing:{}".format(ts1+ts3-ts0-ts2, ts2 - ts1))
	

#left.show(title="Left")
#right.show(title="Right")
#tof.show(title="TOF")

while 1:
	transfer()
	

