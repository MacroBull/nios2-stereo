#! /usr/bin/env python3

IF_LEFT = "left.pgm"
IF_RIGHT = "right.pgm"
IF_TOF = "tof.pgm"
IF_DISP = "disparity.pgm"

PARAMS=[100, 100, int(round(146.316*3646.308))]

PGM_MAGIC="P5"
FLAG0="START"
FLAG1="BOOM"
ACK="ACK"
NAK="NAK"


import sys, time

if len(sys.argv)>1 and sys.argv[1] == '-d':
	pass
else:
	import os
	f = os.popen("cd ../Stereo && /opt/Altera/QuartusPrime-15.1/nios2eds/nios2_command_shell.sh make -j6 download-elf")
	for l in f:
		print(l.rstrip())


def validate():
	dim = tof.size
	if (dim != left.size) or (dim != right.size):
		print("Dimention mimatch!")
		
def display(fn, **args):
	import PIL.Image as Image
	Image.open(fn).show(**args)
		
		
def request(ser, r, e=ACK, poll = False):
	r = r.rstrip()
	e = e.rstrip()
	resp = ""
	while (resp != e) ^ (e == ""):
		if r:
			print("request	tx->:" + r)
			ser.write((r+"\n").encode("utf-8"))
		if not poll:
			while not ser.inWaiting(): time.sleep(ser.timeout)
		try:
			resp = ser.readline().decode('utf-8').rstrip()
		except UnicodeDecodeError:
			resp = ""
		print("request	rx<-:" + resp)
	return resp

def listen(ser, e="", r="", timeout = 0):
	r = r.rstrip()
	e = e.rstrip()
	req = ""
	while (req != e) ^ (e==""):
		tStart = time.time()
		while not ser.inWaiting(): 
			time.sleep(ser.timeout)
			if timeout and time.time()-tStart>timeout:
				print("listen timeout")
				return ''
		try:
			req = ser.readline().decode('utf-8').rstrip()
		except UnicodeDecodeError:
			req = ""
		print("listen	rx<-:" + req)
	if r:
		ser.write((r+"\n").encode("utf-8"))
		print("listen	tx->:" + r)
	return req

def crc32(data, crc = 0): #The internal crc32 polynomial sucks
	crc = 0xffffffff & ~crc
	for d in data:
		crc = crc ^ d
		for j in range(8):
			crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1))
	return 0xffffffff & ~crc

def transfer():
	from serial import Serial
	from macrobull.misc import serialChecker
	
	ser = Serial(serialChecker(), 115200, timeout=0.1)
	
	request(ser, FLAG0, ACK, poll = True)
	ts0 = time.time()	
	print("Sending parameters...")
	ser.write("{}\n".format(len(PARAMS)).encode('utf-8'))
	ser.write("{}\n".format(" ".join([str(p) for p in PARAMS])).encode('utf-8'))
	request(ser, FLAG1)
	print("Sending images...")
	
	checksum = "x"
	for fn in [IF_LEFT, IF_RIGHT, IF_TOF]:
		checksum_r = ""
		while checksum_r!= checksum:
			print("Sending {}...".format(fn))
			f = open(fn, 'rb')
			data = f.read()
			f.close()
			
			ser.write(data)
			#ser.flush()
			
			for i in range(3): data = data[data.index(b'\n')+1:]
			checksum = "{0:08x}".format(crc32(data))
			
			checksum_r = listen(ser, timeout = len(data)/11520)
			
			if not checksum_r: 
				while not ser.inWaiting(): 
					ser.write(b'\n')
					time.sleep(ser.timeout)
				checksum_r = ser.readline().decode('utf-8').rstrip()
			if checksum_r!= checksum:
				print("CRC mismatch, expected:{}, response:{}".format(checksum, checksum_r))
				ser.write((NAK+"\n").encode("utf-8"))
				
		ser.write((ACK+"\n").encode("utf-8"))
		
	print("Sending finished.")
	ts1 = time.time()	
	listen(ser, FLAG1, ACK)
	ts2 = time.time()	
	print("Processing time:{:.3f}s".format(ts2 - ts1))
	print("Receiving images...")
	
	resp = ""
	while resp!= ACK:
		buf = bytearray()
		l = ""
		while l!=(PGM_MAGIC+"\n").encode('utf-8'): l = ser.readline() #skip blanks if exist
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
		resp = request(ser, checksum)
	
	print("Receiving finished.")
	ts3 = time.time()
	print("Time on io:{:.3f}s, processing:{:.3f}s".format(ts1+ts3-ts0-ts2, ts2 - ts1))
	

while 1:
	transfer()
	

