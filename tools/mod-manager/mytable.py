from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtCore import *
from PyQt4.QtGui import *

class MyTable(QtGui.QTableWidget):
    def __init__(self, parent):
        QtGui.QTableWidget.__init__(self, parent)
        self.setAcceptDrops(True)

    def dragEnterEvent(self, event):
        print "Has urls? %s" % event.mimeData().hasUrls()
        if event.mimeData().hasUrls():
            print "Accept event"
            event.accept()
        else:
            event.ignore()
        # print "Drag event: mime %s" % event.mimeData().urls()

    def dragMoveEvent(self, event):
        event.accept()
        pass
        # print "Drag move event"

    def dropEvent(self, event):
        print "Drop event %s" % event.mimeData().urls()

