from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtCore import *
from PyQt4.QtGui import *

import data

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

class MyTable(QtGui.QTableWidget):
    def __init__(self, parent):
        QtGui.QTableWidget.__init__(self, parent)
        self.setAcceptDrops(True)
        self.mods = []

    def initialize(self):
        def maybeAdd(path):
            import os
            #print "Check path %s" % path
            full = os.path.join(path, '%s.txt' % os.path.basename(path))
            if os.path.exists(full):
                self.addMod(path)
        
        self.setColumnCount(3)
        
        import os
        modpath = data.modPath()
        if not os.path.exists(modpath):
            os.makedirs(modpath)

        for path in os.listdir(modpath):
            full = os.path.join(modpath, path)
            maybeAdd(full)

    def addMod(self, path):
        column_name = 0
        column_enabled = 1
        column_size = 2

        mod = Mod(path)
        self.mods.append(mod)

        row = self.rowCount()
        self.setRowCount(self.rowCount() + 1)

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
        import zipfile, os
        modpath = data.modPath()
        print "Installing mod '%s' to %s" % (path, modpath)
        zip = zipfile.ZipFile(path, 'r')
        toplevel = os.path.dirname(zip.namelist()[0])
        try:
            os.makedirs(modpath)
        except Exception:
            pass
        zip.extractall(modpath)
        self.addMod('%s/%s' % (modpath, toplevel))

    def dropEvent(self, event):
        for file in event.mimeData().urls():
            # Warning: only works for file://, in the future
            # try to handle http:// and whatever else
            path = str(file.toLocalFile())
            if isZipFile(path):
                self.doFile(path)
            else:
                showError("'%s' is not a zip file. Please drag and drop .zip files" % path)
