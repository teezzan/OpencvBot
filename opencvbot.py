# -*- coding: utf-8 -*-
"""
Created on Fri Apr 27 12:26:58 2018

@author: TEEE
"""

import numpy as np
import cv2
import urllib.request as ur
font=cv2.FONT_HERSHEY_PLAIN
kernel = np.ones((3,3),np.uint8)

def senseBlu(img):
       hsv=cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
       lower_blue = np.array([110,50,50])
       upper_blue = np.array([130,255,255])
       mask=cv2.inRange(hsv,lower_blue,upper_blue)
#style1
#       opening = cv2.morphologyEx(mask, cv2.MORPH_OPEN, kernel)
       mask1 = cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
       mask1 = cv2.erode(mask1,kernel,iterations = 1)
       mask1 = cv2.morphologyEx(mask1, cv2.MORPH_CLOSE, kernel)
       mask1 = cv2.medianBlur(mask1,5)
#style2       
       closing=cv2.morphologyEx(mask, cv2.MORPH_CLOSE, kernel)
       mask2 = cv2.morphologyEx(closing, cv2.MORPH_OPEN, kernel)
       mask2 = cv2.medianBlur(mask2,9)
#       bitwiseor
       mask=cv2.bitwise_or(mask1,mask2,mask=mask1)
       return mask
def corContour(contour):
    a=[]
    if (contour==0):
        return contour
    else:
        for i in range(len(contour)):
            cnt=contour[i]
            if(cv2.contourArea(cnt)>=1000):
                a.append(cnt)
        return a
def shwcont(contour,frame1,name='frame'):
    if (len(contour)==0):
        cv2.imshow(name ,frame1)
        
#        cv2.imshow('mask',mask)
    else:
        
        if (len(contour)>0):
            
            cv2.drawContours(frame1,contour,-1,(0,0,255),6) 
            for i in range(len(contour)):
                cnt = contour[i]
                area = cv2.contourArea(cnt)
                M = cv2.moments(cnt)
                if((M['m00']!=0)):
                    cx = int(M['m10']/M['m00'])
                    cy = int(M['m01']/M['m00'])
                cv2.putText(frame1,str(area),(cx,cy), font, 1,(255,255,255),2,cv2.LINE_AA)
                cv2.putText(frame1,('No of contours:'+str(len(contour))),(400,50), font, 1.5,(255,5,255),2,cv2.LINE_AA)
                cv2.imshow(name ,frame1)
def area(contour):
    ar=[]
    if (len(contour)==0):
        return ar
        
    else: 
        for i in range(len(contour)):
            cnt = contour[i]
            area = cv2.contourArea(cnt)
            M = cv2.moments(cnt)
            if((M['m00']!=0)):
                cx = int(M['m10']/M['m00'])
                cy = int(M['m01']/M['m00'])
            ar.append([area,cx,cy])
        return ar
cap=cv2.VideoCapture(0)
while True:   
    ret,frame=cap.read()
    frame=cv2.flip(frame,2)
    frame1=frame
    #spliting the image 
    acA=0
    acB=0
    acC=0
    bA=False
    bB=False
    bC=False
    y,x=frame.shape[:2]
    a=int(x/4)
    b=int((3/4)*x)
#    cv2.line(frame,(a,0),(a,y),(255,200,3),3)
#    cv2.line(frame,(b,0),(b,y),(255,200,3),3)
    
    left=frame[:,0:a,:]
    right=frame[:,b:x,:]
    main=frame[:,a:b,:]
    
    
    
    
    left1=senseBlu(left)
    main1=senseBlu(main)
    right1=senseBlu(right)
    frame=senseBlu(frame)
    
    _,contour1,_=cv2.findContours(frame,cv2.RETR_LIST,cv2.CHAIN_APPROX_SIMPLE)
    _,conleft,_=cv2.findContours(left1,cv2.RETR_LIST,cv2.CHAIN_APPROX_SIMPLE)
    _,conright,_=cv2.findContours(right1,cv2.RETR_LIST,cv2.CHAIN_APPROX_SIMPLE)
    _,conmain,_=cv2.findContours(main1,cv2.RETR_LIST,cv2.CHAIN_APPROX_SIMPLE)
    contour=corContour(contour1)
    conleft=corContour(conleft)
    conright=corContour(conright)
    conmain=corContour(conmain)
	
    shwcont(conleft,left,name='conleft')
    shwcont(conright,right,name='conright')
    shwcont(conmain,main,name='conmain')
#    shwcont(contour,frame1)
    
    
# the logic  
    #check the main frame
    a=area(conmain)
    b=area(conleft)
    c=area(conright)
    if (len(a)!=0):
        acA=a[0][0]
        bA=True
       # print('main :',acA,'square')
    if (len(b)!=0):
        acB=b[0][0]
        bB=True
#        print('left :',acB,'square')
    if (len(c)!=0):
        acC=c[0][0]
        bC=True
#        print('right :',acC,'square')
        
    print('left:',bB,'main:',bA,'right:',bC)
    if((bA & bB)|bB):
        print('move left')
       # ur.urlopen('http://192.168.43.43/left')
        
    if((bA & bC)|bC):
        print('move right')
        #ur.urlopen('http://192.168.43.43/right')
    are=not(bB|bC)
    if (bA & are):
        print ('move forward')
        #ur.urlopen('http://192.168.43.43/forward')

    
    k=cv2.waitKey(27)
    if k==27:
        break

cv2.destroyAllWindows()
cap.release()
    