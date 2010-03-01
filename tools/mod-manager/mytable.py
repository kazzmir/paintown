from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtCore import *
from PyQt4.QtGui import *

def isZipFile(path):
    import zipfile
    return zipfile.is_zipfile(path)

def showError(error):
    print error

class MyTable(QtGui.QTableWidget):
    def __init__(self, parent):
        QtGui.QTableWidget.__init__(self, parent)
        self.setAcceptDrops(True)

    def dragEnterEvent(self, event):
        # print "Has urls? %s" % event.mimeData().hasUrls()
        if event.mimeData().hasUrls():
            # print "Accept event"
            event.accept()
        else:
            event.ignore()
        # print "Drag event: mime %s" % event.mimeData().urls()

    def dragMoveEvent(self, event):
        event.accept()
        # print "Drag move event"

    def doFile(self, path):
        if isZipFile(path):
            print "Installing mod %s" % path

    def dropEvent(self, event):
        for file in event.mimeData().urls():
            path = file.path()
            if isZipFile(path):
                self.doFile(path)
            else:
                showError("'%s' is not a zip file. Please drag and drop .zip files" % path)
