#coding=utf-8
import numpy as np
import cv2
import face_recognition
import time
import thread
#调用笔记本内置摄像头，所以参数为0，如果有其他的摄像头可以调整参数为1，2
cap=cv2.VideoCapture(0)

niuben_image = face_recognition.load_image_file("image2.jpg")
dilireba_image = face_recognition.load_image_file("dilireba.jpg")
edc_image = face_recognition.load_image_file("edc.jpg")
zhoujielun_image = face_recognition.load_image_file("zhoujielun.jpg")
jujingyi_image = face_recognition.load_image_file("jujingyi.jpg")
wanglihong_image = face_recognition.load_image_file("wanglihong.jpg")
wanglihong2_image = face_recognition.load_image_file("wanglihong2.jpg")
jiahaonan_image = face_recognition.load_image_file("jiahaonan.jpg")


niuben_face_encoding = face_recognition.face_encodings(niuben_image)
dilireba_face_encoding = face_recognition.face_encodings(dilireba_image)
edc_face_encoding = face_recognition.face_encodings(edc_image)
zhoujielun_face_encoding = face_recognition.face_encodings(zhoujielun_image)
jujingyi_face_encoding = face_recognition.face_encodings(jujingyi_image)
wanglihong_face_encoding = face_recognition.face_encodings(wanglihong_image)
wanglihong2_face_encoding = face_recognition.face_encodings(wanglihong2_image)
jiahaonan_face_encoding = face_recognition.face_encodings(jiahaonan_image)


known_face_encodings = [
    niuben_face_encoding, 
    dilireba_face_encoding, 
    edc_face_encoding, 
    zhoujielun_face_encoding, 
    jujingyi_face_encoding, 
    wanglihong_face_encoding
]

known_face_names = [
    "NiuBenn", 
    "DiLiReBa", 
    "EDC", 
    "ZhouJieLun",
    "JuJingyi", 
    "WangLiHong"
]


def detect_faceinfo(rgb_small_frame, face_locations):
    l2len = 1.0
    face_index = 0
    res_index = -1
    l2lens = []
    face_names = []
    tmp = 'Unknow'
    
    face_encodings = face_recognition.face_encodings(rgb_small_frame, face_locations)
    for face_encoding in face_encodings:
        for index in range(len(known_face_encodings)):
            l2 = np.linalg.norm(face_encoding - known_face_encodings[index])
            if (l2 < l2len):
                l2len = l2
                tmp = known_face_names[index]
            print known_face_names[index], ' L2', l2
        print ' '
        if (l2len < 0.46):
            face_names.append(tmp)
            l2lens.append(l2len)
            if res_index == -1:
                res_index = face_index
        else:
            face_names.append('Unknow')
            l2lens.append(1.0)
        face_index += 1
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
        catch = face_recognition.face_locations(rgb_small_frame)
        if len(catch):
            sp = img.shape
            for (top, right, bottom, left) in catch:
                top *= 4
                right *= 4
                bottom *= 4
                left *= 4
                cv2.rectangle(img, (left, top), (right, bottom), green, 2)
            register_str = 'Captured face, press\' y\' to register'
            cv2.putText(img, register_str, (sp[1] / 2 - 230, sp[0] - 50),cv2.FONT_HERSHEY_PLAIN, 1.6, red, 2)
            

        cv2.imshow("face",img)
        k = cv2.waitKey(1)
        if k & 0xFF == ord('q'):
            #通过esc键退出摄像
            break

    

last_detect_time = time.time()
last_clock_time = time.time()
face_names = []
res_index = -1
green = (0, 255, 0)
red = (0, 0, 255)
Clock = False
Register = True
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
            if res_index != -1 and now_time - last_clock_time > 2:
                sp = img.shape
                img = cv2.filter2D(img, -1 ,knrnel3)
                time_array = time.localtime(int(time.time()))
                time_str = time.strftime("%Y-%m-%d %H:%M:%S", time_array)
                #time_str = time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime())
                cv2.putText(img, face_names[res_index], (sp[1] / 2 - 70, sp[0] / 2 - 10),cv2.FONT_HERSHEY_PLAIN, 2.0, green, 2)
                cv2.putText(img, time_str, (sp[1] / 2 - 180, sp[0] / 2 + 50),cv2.FONT_HERSHEY_PLAIN, 2.0, red, 2)
                cv2.imshow("face",img)
                
                cv2.waitKey(800)
                cv2.putText(img, 'SUCCESS', (sp[1] / 2 - 70, sp[0] / 2 + 110),cv2.FONT_HERSHEY_PLAIN, 2.0, red, 2)
                cv2.imshow("face",img)
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