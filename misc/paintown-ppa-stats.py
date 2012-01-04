#!/usr/bin/python
#usage python ppastats.py PPATEAM (ex: webupd8team) PPA (ex: gthumb) DIST (Ubuntu version eg maverick) ARCH (ubuntu arch eg i386 or amd64)

#example - highest downloaded file: python ppastats.py webupd8team y-ppa-manager maverick amd64 | tr '\t' ',' | cut -d ',' -f3 | sort -gr

import sys
from launchpadlib.launchpad import Launchpad

def stats(distro, arch):
    PPAOWNER = 'rafkind'
    PPA = 'paintown'
    desired_dist_and_arch = 'https://api.launchpad.net/devel/ubuntu/' + distro + '/' + arch

    cachedir = "~/.launchpadlib/cache/"
    lp_ = Launchpad.login_anonymously('ppastats', 'edge', cachedir, version='devel')
    owner = lp_.people[PPAOWNER]
    archive = owner.getPPAByName(name=PPA)

    for individualarchive in archive.getPublishedBinaries(status = 'Published', distro_arch_series = desired_dist_and_arch):
        x = individualarchive.getDownloadCount()
        if x > 0:
            print "%(distro)s\t%(arch)s\t%(package)s\t%(version)s\t%(download)s" % {'distro': distro,
                   'arch': arch,
                   'package': individualarchive.binary_package_name,
                   'version': individualarchive.binary_package_version,
                   'download': str(individualarchive.getDownloadCount())}
        elif x < 1:
            print '0'

def print_distros(distro):
    archs = ['i386', 'amd64']
    for arch in archs:
        stats(distro, arch)

distros = ['natty', 'oneiric', 'maverick', 'lucid']
for distro in distros:
    print_distros(distro)
