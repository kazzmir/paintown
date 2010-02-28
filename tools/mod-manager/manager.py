#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from PyQt4 import QtCore, QtGui, uic
from PyQt4.QtCore import *
from PyQt4.QtGui import *

app = QtGui.QApplication(sys.argv)

# Set the look and feel
QtGui.QApplication.setStyle(QtGui.QStyleFactory.create("Cleanlooks"))

main = uic.loadUi("main.ui")

# Center on screen
screen = QtGui.QDesktopWidget().screenGeometry()
size = main.geometry()
main.move((screen.width()-size.width())/2, (screen.height()-size.height())/2)

# show widget
main.show()

# Run
app.exec_()
