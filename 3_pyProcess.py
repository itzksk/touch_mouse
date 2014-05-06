#!/usr/bin/python
''' Python process to read from shared memory
    created by <cProcess.c>.
    KEY1=12345
'''
from pymouse import PyMouse
import sysv_ipc as ipc
import re,os,sys
import time
KEY1 = 12345 #shared memory
KEY2 = 6789  #semaphore
MAX_SAMPLES = 40
COUNT=45
DEBOUNCE = 30

shm = ipc.SharedMemory(KEY1) #implicitly attaches the shm
sem = ipc.Semaphore(KEY2)
m = PyMouse()
size = m.screen_size()

prev_x = 0
prev_y = 0
prev_xx=0
prev_yy=0
speed = 1
count=0

right_click_debounce = 0
left_click_debounce = 0
wheel_up_debounce = 0
wheel_down_debounce = 0

xx = range(MAX_SAMPLES)
yy = range(MAX_SAMPLES)
xx_index=0
yy_index=0
release = False
for i in range(len(xx)):
	xx[i]=0
	yy[i]=0
	
while 1:
	time.sleep(.0019)
	sem.acquire()
	read = shm.read()
	sem.release()
	cursor=re.findall('\d+',read)
#	print "cursor=",cursor
	if len(cursor) == 4 or len(cursor) == 2 or len(cursor) == 3:
		x=int(cursor[0])
		y=int(cursor[1])
#		print str(x) + ' ' + str(y)
		if x>=1000 or y>=1000 : #it means if not pressed
			for i in range(len(xx)): #flush xx and yy
				xx[i]=0
				yy[i]=0
#			prev_xx=0
#			prev_yy=0
			continue	
		#if pressed		
		xx[xx_index]=x;
		yy[yy_index]=y;
		print "x=",x,",","y=",y
		xx_index=xx_index+1
		yy_index=yy_index+1
		if xx_index==MAX_SAMPLES:xx_index=0
		if yy_index==MAX_SAMPLES:yy_index=0

		XX=sum(xx)/MAX_SAMPLES
		YY=sum(yy)/MAX_SAMPLES
		print "XX=",XX,",","YY=",YY
		if x > 950 and y > 850:
			right_click_debounce = right_click_debounce+1
			if right_click_debounce == DEBOUNCE:
		#		if XX > 900 and YY > 850:
					 os.system("xdotool click 3")
	#				 print'XX=',XX,",","YY=",YY
				         right_click_debounce = 0
		elif x < 290 and y > 800:
			left_click_debounce = left_click_debounce+1
			if left_click_debounce == DEBOUNCE:
				os.system("xdotool click 1")
				left_click_debounce = 0
		elif x > 900 and y < 180:
			wheel_up_debounce = wheel_up_debounce+1
			if wheel_up_debounce == DEBOUNCE:
				os.system("xdotool click 4")
				wheel_up_debounce = 0
		elif x > 900 and y > 350 and y < 450:
			wheel_down_debounce = wheel_down_debounce+1
			if wheel_down_debounce == DEBOUNCE:
				os.system("xdotool click 5")
				wheel_down_debounce = 0  
		
		else:
			right_click_debounce = 0
			left_click_debounce = 0
			wheel_up_debounce = 0
			wheel_down_debounce = 0
			X=XX-prev_xx
			Y=YY-prev_yy
			if abs(X) <10 and abs(Y)<10:
				count=count+1
				if count==COUNT:
					speed=1
				current_pos = m.position()
				#print "##########X=",X,",","Y=",Y
				m.move(current_pos[0]+(X*speed*2),current_pos[1]+(Y*2*speed))
			prev_xx=XX
			prev_yy=YY
	else:
		speed = 0
		count = 0
		prev_xx=0
		prev_yy=0

