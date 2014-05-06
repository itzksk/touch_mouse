#!/usr/bin/python
''' Python process to read from shared memory
    created by <cProcess.c>.
    KEY=12345
'''
from pymouse import PyMouse
import sysv_ipc as ipc
import re
KEY1 = 12345 #shared memory
KEY2 = 6789  #semaphore
MAX_SAMPLES = 10
shm = ipc.SharedMemory(KEY1) #implicitly attaches the shm
sem = ipc.Semaphore(KEY2)
m = PyMouse()
size = m.screen_size()
offset = 0
prev_x = 0
prev_y = 0
count = 0
speed = 1
while True:
	#semlock
	sem.acquire()
	read = shm.read()
	#semunlock
	sem.release()
	#print read
	a = re.findall('\d+',read)
#	print a,len(a)
	
	#seperate x and y
	xx = range(len(a)/2)
	yy = range(len(a)/2)
	for i in range(len(a)/2 ):
		xx[i] = int(a[i*2])
		yy[i] = int(a[2*i+1])
		#print 'i=',i
#	print 'xx=',xx
#	print 'yy=',yy
	XX = sum(xx)/MAX_SAMPLES
	YY = sum(yy)/MAX_SAMPLES
#	print 'XX=',XX,'  YY=',YY	
	X = XX - prev_x
	Y = YY - prev_y
	print 'X=',X,'Y=',Y
	if abs(X)<10 and abs(Y)<10 :
		count = count+1
		if count==25:
			speed=1
		current_pos = m.position()
		m.move(current_pos[0]+(X*speed*2),current_pos[1]+(Y*speed*2))
	prev_x = XX
	prev_y = YY
#	if it == 10:	break
#	else : it = it+1
