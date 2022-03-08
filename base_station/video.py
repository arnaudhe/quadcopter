import cv2
import sys
from PyQt5.QtWidgets import  QWidget, QLabel, QApplication
from PyQt5.QtCore import QThread, Qt, pyqtSignal, pyqtSlot
from PyQt5.QtGui import QImage, QPixmap

class Video(QThread):

    GREY  = (120, 120, 120)
    WHITE = (255, 255, 255)
    RED   = (255, 0,   0)
    GREEN = (0,   255, 100)
    BLUE  = (0,   0,   255)

    changePixmap = pyqtSignal(QImage)

    @staticmethod
    def list_devices():
        index = 0
        arr = []
        while True:
            cap = cv2.VideoCapture(index)
            try:
                arr.append(index)
            except:
                break
            cap.release()
            index += 1
        return arr

    def __init__(self, app, size, camera_id):
        super().__init__(app)
        self.running = True
        self.camera_id = camera_id
        self.size = size

    def display_text(self, image, position, text, color):
        font_size    = 0.7
        rect_padding = 2
        char_height  = int(font_size * 23)
        char_width   = int(font_size * 20)
        rect_height  = char_height
        rect_width   = len(text) * char_width
        pos_x, pos_y = position
        cv2.rectangle(image, (pos_x - rect_padding, pos_y - rect_height - rect_padding), (pos_x + rect_width + rect_padding, pos_y + rect_padding), Video.GREY, -1)
        cv2.putText(image, text, position, cv2.FONT_HERSHEY_DUPLEX, font_size, color)

    def run(self):
        cap = cv2.VideoCapture(self.camera_id)
        while self.running:
            ret, frame = cap.read()
            if ret:
                # https://stackoverflow.com/a/55468544/6622587
                rgbImage = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
                self.display_text(rgbImage, (420, 460), "BATTERY : 100 %", Video.GREEN)
                h, w, ch = rgbImage.shape
                bytesPerLine = ch * w
                convertToQtFormat = QImage(rgbImage.data, w, h, bytesPerLine, QImage.Format_RGB888)
                p = convertToQtFormat.scaled(self.size[0], self.size[1], Qt.KeepAspectRatio)
                self.changePixmap.emit(p)

    def stop(self):
        self.running = False
        self.wait()

class VideoApp(QWidget):

    def __init__(self, camera_id):
        super().__init__()
        self.title = 'Video'
        self.left = 100
        self.top = 100
        self.width = 1280
        self.height = 960
        self.initUI()
        self.video = Video(self, (self.width, self.height), camera_id)
        self.video.changePixmap.connect(self.setImage)
        self.video.start()

    @pyqtSlot(QImage)
    def setImage(self, image):
        self.label.setPixmap(QPixmap.fromImage(image))

    def initUI(self):
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)
        self.resize(self.width, self.height)
        self.label = QLabel(self)
        self.label.resize(self.width, self.height)
        self.show()

    def stop(self):
        self.video.stop()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = VideoApp(1)
    exit_code = app.exec_()
    ex.stop()
    sys.exit(exit_code)
