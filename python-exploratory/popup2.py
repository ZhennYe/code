from PyQt4.QtCore import *
from PyQt4.QtGui import *


class AppForm(QMainWindow):
    def __init__(self, parent=None):
        QMainWindow.__init__(self, parent)
        self.create_main_frame()       

    def create_main_frame(self):        
        page = QWidget()        

        self.button = QPushButton('Task completed!', page)
        self.edit1 = QLineEdit()
        self.edit2 = QLineEdit()

        vbox1 = QVBoxLayout()
        vbox1.addWidget(self.button)
        page.setLayout(vbox1)
        self.setCentralWidget(page)

        self.connect(self.button, SIGNAL("clicked()"), self.clicked)

    def clicked(self):
        return
        #QMessageBox.about(self, "My message box", "Text1 = %s, Text2 = %s" % (
        #    self.edit1.text(), self.edit2.text()))

def task_complete():
    import sys
    app = QApplication(sys.argv)
    form = AppForm()
    form.show()
    app.exec_()


#############
if __name__ == '__main__':
  task_complete()
