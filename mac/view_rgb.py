#!/usr/bin python
#thanks taritree(!)

import os,sys
from PyQt4 import QtGui, QtCore
import pyqtgraph as pg
from pyrgb.display import rgbdisplay as rgbd


app = QtGui.QApplication([])
rgbdisplay = rgbd.RGBDisplay(sys.argv[1:])
rgbdisplay.show()

if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
    print "exec called ..."
    rgbdisplay.show()
    QtGui.QApplication.instance().exec_()
