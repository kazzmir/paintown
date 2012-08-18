#!/usr/bin/env python

import os
def createLayout(name):
    os.makedirs('%s/Contents' % name)
    os.makedirs('%s/Contents/MacOS' % name)
    os.makedirs('%s/Contents/Resources' % name)

def createPlist(plist, executable, version, description):
    file = open(plist, 'w')
    file.write("""
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
<plist version=\"1.0\">\n"
<dict>"
    <key>CFBundleExecutable</key>
    <string>%(executable)s</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleSignature</key>
    <string>%(signature)s</string>
    <key>CFBundleVersion</key>
    <string>%(version)s</string>
    <key>CFBundleDocumentTypes</key>
    <array>
        <dict>
            <key>CFBundleTypeExtensions</key>
            <array>
                <string>*</string>
            </array>
            <key>CFBundleTypeName</key>
            <string>NSStringPboardType</string>
            <key>CFBundleTypeOSTypes</key>
            <array>
                <string>****</string>
            </array>
            <key>CFBundleTypeRole</key>
            <string>Viewer</string>
        </dict>
    </array>
    <key>CFBundleGetInfoString</key>
    <string>%(long-version)s</string>
    </dict>
</plist>
""" % {'executable' : executable,
        'version' : version,
        'long-version' : description,
        'signature' : '????'})
    file.close()

def createPkgInfo(info):
    file = open(info, 'w')
    file.write('APPL????')
    file.close()

def setup(executable, version, description, icon):
    import shutil
    app = '%s.app' % executable
    createLayout(app)
    shutil.copy(executable, '%s/Contents/MacOS/' % app)
    createPlist('%s/Contents/Info.plist' % app, executable, version, description)
    createPkgInfo('%s/Contents/PkgInfo' % app)
    shutil.copy(icon, '%s/Contents/Resources/' % app)
    print "Done"

import sys
if len(sys.argv) < 4:
    print "Arguments <executable> <version> <description> <icon>"
    sys.exit(0)

setup(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
