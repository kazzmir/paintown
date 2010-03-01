#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtCore import *
from PyQt4.QtGui import *

def run(app):
    # Set the look and feel
    QtGui.QApplication.setStyle(QtGui.QStyleFactory.create("Cleanlooks"))

    main = uic.loadUi("main.ui")

    #main.setAcceptDrops(True)
    #main.mods.setAcceptDrops(True)
    # table = MyTable(main.mods)
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
