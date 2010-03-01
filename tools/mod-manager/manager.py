#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtCore import *
from PyQt4.QtGui import *

class Signals:
    pass
Signals.Triggered = "triggered()"
Signals.Close = "close()"
Signals.Clicked = "clicked()"

def run(app):
    # Set the look and feel
    QtGui.QApplication.setStyle(QtGui.QStyleFactory.create("Cleanlooks"))

    main = uic.loadUi("main.ui")
    main.mods.initialize()

    exit = main.actionQuit
    exit.setShortcut('Ctrl+Q')
    exit.setStatusTip('Quit')
    main.connect(exit, SIGNAL(Signals.Triggered), SLOT(Signals.Close))

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
