import cv2
import tcp_server
import threading
import http_server
import argparse

WIDTH = 0
HEIGHT = 1

respulutions_list = [
    (320, 240),
    (352, 240),
    (352, 288),
    (400, 240),
    (480, 576),
    (640, 240),
    (320, 480),
    (640, 360),
    (640, 480),
    (800, 480),
    (800, 600),
    (848, 480),
    (960, 540),
    (1024, 600),
    (1024, 768),
    (1152, 864),
    (1200, 600),
    (1280, 720),
    (1280, 768),
    (1280, 1024),
    (1440, 900),
    (1400, 1050),
    (1536, 960),
    (1536, 1024),
    (1600, 900),
    (1600, 1024),
    (1600, 1200),
    (1680, 1050),
    (1920, 1080),
    (1920, 1200),
    (2048, 1080),
    (2048, 1152),
    (2048, 1536),
    (2560, 1440),
    (2560, 1600),
    (2560, 2048),
    (3072, 1620),
    (3200, 1800),
    (3200, 2048),
    (3200, 2400),
    (3440, 1440),
    (3840, 2400),
    (3840, 2160),
    (4096, 2160),
    (4128, 2322),
    (4128, 3096),
    (5120, 2160),
    (5120, 2700),
    (5120, 2880),
    (5120, 3840),
    (5120, 4096),
    (6144, 3240),
    (6400, 4096),
    (6400, 4800),
    (7168, 3780),
    (7680, 4320),
    (7680, 4800),
    (8192, 4320)
]

def get_cameras_list():
    # checks the first 10 indexes.
    index = 0
    arr = []
    for i in range(256):
        cap = cv2.VideoCapture(i)
        if cap.read()[0]:
            arr.append(index)
            cap.release()
    return arr

def check_resolution_support(camera):
    supported_resolutions = []

    for resolution in respulutions_list:
        set_width_result =  camera.set(cv2.CAP_PROP_FRAME_WIDTH, resolution[WIDTH])
        set_height_result = camera.set(cv2.CAP_PROP_FRAME_HEIGHT, resolution[HEIGHT])

        width = int(camera.get(cv2.CAP_PROP_FRAME_WIDTH))
        height = int(camera.get(cv2.CAP_PROP_FRAME_HEIGHT))

        if resolution[WIDTH] == width and resolution[HEIGHT] == height:
            supported_resolutions.append(resolution)

    return supported_resolutions

def detectFaceOpenCVHaar(faceCascade, frame, inHeight=300, inWidth=0):
    frameOpenCVHaar = frame.copy()
    frameHeight = frameOpenCVHaar.shape[0]
    frameWidth = frameOpenCVHaar.shape[1]
    if not inWidth:
        inWidth = int((frameWidth / frameHeight) * inHeight)

    scaleHeight = frameHeight / inHeight
    scaleWidth = frameWidth / inWidth

    frameOpenCVHaarSmall = cv2.resize(frameOpenCVHaar, (inWidth, inHeight))
    frameGray = cv2.cvtColor(frameOpenCVHaarSmall, cv2.COLOR_BGR2GRAY)

    faces = faceCascade.detectMultiScale(frameGray)
    bboxes = []
    for (x, y, w, h) in faces:
        x1 = x
        y1 = y
        x2 = x + w
        y2 = y + h
        cvRect = [int(x1 * scaleWidth), int(y1 * scaleHeight),
                  int(x2 * scaleWidth), int(y2 * scaleHeight)]
        bboxes.append(cvRect)
        cv2.rectangle(frameOpenCVHaar, (cvRect[0], cvRect[1]), (cvRect[2], cvRect[3]), (0, 255, 0),
                      int(round(frameHeight / 150)), 4)
    return frameOpenCVHaar, bboxes


if __name__ == '__main__':

    # define a video capture object
    vid = cv2.VideoCapture(0)

    width = vid.get(cv2.CAP_PROP_FRAME_WIDTH)
    height = vid.get(cv2.CAP_PROP_FRAME_HEIGHT)

    #get_cameras_list()
    modes = check_resolution_support(vid)

    faceCascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')

    parser = argparse.ArgumentParser(description='HDCAM daemon parameters')
    parser.add_argument('--settings', metavar='N', type=bool, default=False,help='Run in settings mode')
    args = parser.parse_args()

    if args.settings == True:
        pass

    vid.set(cv2.CAP_PROP_FRAME_WIDTH, width)
    vid.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

    args = parser.parse_args()

    server = tcp_server.tcp_server(target_port = 1234)
    http_server = http_server.http_server_imp(port=8081)

    http_server.set_current_mode(width, height)
    http_server.set_modes(modes)

    threading.Thread(target=server.run).start()
    threading.Thread(target=http_server.run).start()

    while (True):

        # Capture the video frame
        # by frame

        res, requested_width, requested_height = http_server.get_requested_mode()

        if res:
            vid.set(cv2.CAP_PROP_FRAME_WIDTH, requested_width)
            vid.set(cv2.CAP_PROP_FRAME_HEIGHT, requested_height)

        ret, frame = vid.read()

        frame, boxes = detectFaceOpenCVHaar(faceCascade, frame)

        http_server.set_face_count(len(boxes))

        # Display the resulting frame
        cv2.imshow('frame', frame)

        server.set_frame(frame)

        # the 'q' button is set as the
        # quitting button you may use any
        # desired button of your choice
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # After the loop release the cap object
    vid.release()
    # Destroy all the windows
    cv2.destroyAllWindows()