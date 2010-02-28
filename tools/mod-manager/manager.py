#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtCore import *
from PyQt4.QtGui import *

class MyTable(QtGui.QTableWidget):
    def __init__(self, parent):
        QtGui.QTableWidget.__init__(self, parent)
        self.setAcceptDrops(True)
        self.resize(100, 100)

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

def run(app):
    # Set the look and feel
    QtGui.QApplication.setStyle(QtGui.QStyleFactory.create("Cleanlooks"))

    main = uic.loadUi("main.ui")

    #main.setAcceptDrops(True)
    #main.mods.setAcceptDrops(True)
    table = MyTable(main)
    # main.addWidget(MyTable())

    # Center on screen
    screen = QtGui.QDesktopWidget().screenGeometry()
    size = main.geometry()
    main.move((screen.width()-size.width())/2, (screen.height()-size.height())/2)

    # show widget
    main.show()

    # Run
    app.exec_()

import sys
run(QtGui.QApplication(sys.argv))
