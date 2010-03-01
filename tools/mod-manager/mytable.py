from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtCore import *
from PyQt4.QtGui import *

class Mod:
    def __init__(self, path):
        import os
        self.path = path
        self.size = None
        self.name = os.path.basename(path)

def isZipFile(path):
    import zipfile
    return zipfile.is_zipfile(path)

def showError(error):
    print error

modPath = '/home/jon/.paintown'

class MyTable(QtGui.QTableWidget):
    def __init__(self, parent):
        QtGui.QTableWidget.__init__(self, parent)
        self.setAcceptDrops(True)

    def initialize(self):
        def maybeAdd(path):
            import os
            #print "Check path %s" % path
            if os.path.exists("%s/%s.txt" % (path, os.path.basename(path))):
                self.addMod(path)
        
        import os
        for path in os.listdir(modPath):
            maybeAdd('%s/%s' % (modPath, path))

    def addMod(self, path):
        column_name = 0
        column_enabled = 1
        column_size = 2

        mod = Mod(path)
        row = self.rowCount()
        self.setRowCount(self.rowCount() + 1)
        self.setColumnCount(3)

        self.setItem(row, column_name, QtGui.QTableWidgetItem(mod.name))
        # self.setItem(1, 1, QtGui.QTableWidgetItem('foobar'))
        #for c in range(0, self.columnCount()):
        #    for r in range(0, self.rowCount()):
        #        self.setItem(r, c, QtGui.QTableWidgetItem('foobar'))

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
