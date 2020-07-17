#coding=utf-8
import numpy as np
import cv2
import face_recognition
import time
import thread
import requests
import json
#调用笔记本内置摄像头，所以参数为0，如果有其他的摄像头可以调整参数为1，2
cap=cv2.VideoCapture(0)


r = requests.get(url='http://www.niubenn.xyz:9000/init')

end = len(r.content)
print len(r.content)   #打印解码后的返回数据
count = (r.content)[0:4]
a = np.frombuffer(count, np.int32)
print "count : ", a
index = a[0]*512 + 4
print "index:", index
feature = (r.content)[4:index]
known_face_encodings = np.frombuffer(feature, dtype=np.float32)
print known_face_encodings
print len(known_face_encodings)

known_face_encodings = known_face_encodings.reshape(-1,128)
print known_face_encodings
print len(known_face_encodings)

info = (r.content)[index:end]
print info

known_face = json.loads(info)


def detect_faceinfo(rgb_small_frame, face_locations):
    l2len = 1.0
    face_index = -1
    res_index = -1
    l2lens = []
    face_names = []
    tmp = 'Unknow'
    
    face_encodings = face_recognition.face_encodings(rgb_small_frame, face_locations)
    for face_encoding in face_encodings:
        print "-------------------------------feature----------------------------------"
        print face_encoding
        print len(face_encoding.tostring())
        print "------------------------------------------------------------------------"
        for index in range(len(known_face_encodings)):
            l2 = np.linalg.norm(face_encoding - known_face_encodings[index])
            if (l2 < l2len):
                l2len = l2
                face_index = index
                tmp = known_face[index]['ename']
            print known_face[index]['ename'], ' L2', l2
        print ' '
        if (l2len < 0.46):
            face_names.append(tmp)
            l2lens.append(l2len)
            if res_index == -1:
                res_index = face_index
        else:
            face_names.append('Unknow')
            l2lens.append(1.0)
        
    for name, l2len in zip(face_names, l2lens):
        print 'Result: [name:', name, '] [dis: ', l2len, ']'
    print 'face_names:', face_names
    print 'res_index:', res_index
    print '------------------------------------'
    print ''
    return  face_names, res_index
    
def registered_face():

    while True:
        sucess, img = cap.read()
        img = cv2.flip(img,1)
        small_frame = cv2.resize(img, (0, 0), fx=0.25, fy=0.25)
        rgb_small_frame = small_frame[:, :, ::-1]
        face_locations = face_recognition.face_locations(rgb_small_frame)
        if len(face_locations):
            sp = img.shape
            for (top, right, bottom, left) in face_locations:
                top *= 4
                right *= 4
                bottom *= 4
                left *= 4
                cv2.rectangle(img, (left, top), (right, bottom), green, 2)
            register_str = 'Captured face, press\' y\' to register'
            #cv2.putText(img, register_str, (sp[1] / 2 - 230, sp[0] - 50),cv2.FONT_HERSHEY_PLAIN, 1.6, red, 2)
            cv2.putText(img, register_str, (sp[1] / 2 -130, sp[0] - 50),cv2.FONT_HERSHEY_PLAIN, 1, red, 2)
            
        img = cv2.resize(img, (0, 0), fx=1.4, fy=1.4)
        cv2.imshow("face",img)
        k = cv2.waitKey(1)
        if k & 0xFF == ord('q'):
            #通过esc键退出摄像
            break
        elif k & 0xFF == ord('y'):
            face_encodings = face_recognition.face_encodings(rgb_small_frame, face_locations)
            if(len(face_encodings) > 0):
                sucess, img = cap.read()
                a = face_encodings[0]
                a = a.astype(np.float32)
                b = a.tostring()
                r = requests.post(url='http://www.niubenn.xyz:9000/registered_face', data=b)
                code_str = "Code :" + r.content
                info_str = "Please press any key to continue"
                img = cv2.flip(img,1)
                img = cv2.filter2D(img, -1 ,knrnel3)
                cv2.putText(img, code_str, (sp[1] / 2 -80, sp[0] - 50),cv2.FONT_HERSHEY_PLAIN, 1.6, red, 2)
                cv2.putText(img, info_str, (sp[1] / 2 -130, sp[0] - 30),cv2.FONT_HERSHEY_PLAIN, 1, red, 2)
                img = cv2.resize(img, (0, 0), fx=1.4, fy=1.4)
                cv2.imshow("face",img)
                k = cv2.waitKey(1000000)
                return


    

last_detect_time = time.time()
last_clock_time = time.time()
face_names = []
res_index = -1
green = (0, 255, 0)
red = (0, 0, 255)
Clock = False
Register = False
isclock = False
knrnel3 = np.array([[1.0/9**2]*9 for i in range(9)])

while True:
    #从摄像头读取图片
    sucess, img = cap.read()
    img = cv2.flip(img,1)
    small_frame = cv2.resize(img, (0, 0), fx=0.25, fy=0.25)
    rgb_small_frame = small_frame[:, :, ::-1]
    #转为灰度图片
    #gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    
    #faceRects = classifier.detectMultiScale( gray, scaleFactor=1.2, minNeighbors=3, minSize=(32, 32)) 
    face_locations = face_recognition.face_locations(rgb_small_frame)
    if len(face_locations): # 大于0则检测到人脸
        
        now_time = time.time()
        if now_time - last_detect_time > 1.5:
            last_detect_time = now_time
            face_names, res_index = detect_faceinfo(rgb_small_frame, face_locations)

        if Register:
            registered_face()
            sucess, img = cap.read()
            img = cv2.flip(img,1)
            Register = not Register


        elif Clock:
            if res_index != -1 and now_time - last_clock_time > 1:
                sp = img.shape
                img = cv2.filter2D(img, -1 ,knrnel3)
                time_array = time.localtime(int(time.time()))
                time_str = time.strftime("%Y-%m-%d %H:%M:%S", time_array)
                #time_str = time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime())
                cv2.putText(img, face_names[0], (sp[1] / 2 - 70, sp[0] / 2 - 10),cv2.FONT_HERSHEY_PLAIN, 2.0, green, 2)
                cv2.putText(img, time_str, (sp[1] / 2 - 180, sp[0] / 2 + 50),cv2.FONT_HERSHEY_PLAIN, 2.0, red, 2)
                imgtemp = cv2.resize(img, (0, 0), fx=1.4, fy=1.4)
                cv2.imshow("face",imgtemp)
                
                r = requests.post(url='http://www.niubenn.xyz:9000/clock', data = {'id': known_face[res_index]['id']})
                res_str = r.content
                cv2.putText(img, res_str, (sp[1] / 2 - 80, sp[0] / 2 + 110),cv2.FONT_HERSHEY_PLAIN, 2.0, red, 2)
                imgtemp = cv2.resize(img, (0, 0), fx=1.4, fy=1.4)
                cv2.imshow("face",imgtemp)
                cv2.waitKey(1000)
                last_clock_time = time.time()
        else:
            for (top, right, bottom, left), name in zip(face_locations, face_names):
                top *= 4
                right *= 4
                bottom *= 4
                left *= 4
                cv2.rectangle(img, (left, top), (right, bottom), green, 2)
                cv2.putText(img, name, (left + 10, top - 10), cv2.FONT_HERSHEY_PLAIN, 2.0, green, 2)
    img = cv2.resize(img, (0, 0), fx=1.4, fy=1.4)
    cv2.imshow("face",img)
        
    #保持画面的持续。
    k = cv2.waitKey(1)
    if k & 0xFF == ord('q'):
        #通过esc键退出摄像
        cv2.destroyAllWindows()
        break
    elif k & 0xFF == ord('s'):
        #通过s键保存图片，并退出。
        cv2.imwrite("/home/niuben/work/face/image2.jpg",img)
        cv2.destroyAllWindows()
        #break
    elif k & 0xFF == ord('r'):
        Register = True
    elif k & 0xFF == ord('c'):
        Clock = not Clock
#关闭摄像头

cap.release()