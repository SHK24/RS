from http.server import BaseHTTPRequestHandler
from http.server import HTTPServer
import json
import base64

current_width = 0
current_height = 0
face_count = 0
modes = []
change_video_mode = False
requested_width = 0
requested_height = 0

class HttpGetHandler(BaseHTTPRequestHandler):
    """Обработчик с реализованным методом do_GET."""

    def do_POST(self):

        length = int(self.headers.get('content-length'))
        message = json.loads(self.rfile.read(length))

        response = dict()

        if message['request'] == 'set':
            if message['entity'] == 'video_mode':
                global change_video_mode, requested_width, requested_height
                change_video_mode = True

                width = message["value"].split('x')[0]
                height = message["value"].split('x')[1]

                requested_width = int(width)
                requested_height = int(height)

            response = dict()
            response['result'] = 'OK'
            response['entity'] = 'set_video_mode'

            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps(response).encode('utf-8'))

        if message['request'] == 'get':

            if message['entity'] == 'sceleton_count':
                response['result'] = 'OK'
                response['entity'] = 'sceleton_count'
                response['sceleton_count'] = str(sceleton_count)

            if message['entity'] == 'status':
                response['result'] = 'OK'
                response['entity'] = 'status'
                response['video_mode'] = (current_width, current_height)

            if message['entity'] == 'info':
                pass

            if message['entity'] == 'face_count':
                response['result'] = 'OK'
                response['entity'] = 'face_count'
                response['face_count'] = str(face_count)

            if message['entity'] == 'modes':
                response['result'] = 'OK'
                response['entity'] = 'modes'
                response['modes'] = modes

            self.send_response(200)
            self.send_header("Content-type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps(response).encode('utf-8'))


class http_server_imp:

    def __init__(self, port=8000):
        self.port = port

    def set_sceleton_count(self, value):
        global sceleton_count
        sceleton_count = value

    def set_face_count(self, value):
        global face_count
        face_count = value

    def set_modes(self, value):
        global modes
        modes = value

    def set_current_mode(self, width, height):
        global current_width, current_height
        current_width = width
        current_height = height

    def get_requested_mode(self):
        global change_video_mode, requested_width, requested_height

        res = change_video_mode
        change_video_mode = False


        return res, requested_width, requested_height

    def run(self, server_class=HTTPServer, handler_class=HttpGetHandler):
      server_address = ('', self.port)
      httpd = server_class(server_address, handler_class)
      try:
          httpd.serve_forever()
      except KeyboardInterrupt:
          httpd.server_close()


if __name__ == '__main__':
    # vf = kinect_messages.messages_pb2.video_frame()
    # vf.result = 'OK'
    # vf.width = 640
    # vf.height = 480
    # vf.data = "1234"
    # dict_obj = MessageToDict(vf)

    server = http_server_imp(8081)
    server.run()