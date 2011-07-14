#!/usr/bin/env python

import xml.dom.minidom

# http://sourceforge.net/apps/trac/paintown/report/1
server = 'http://sourceforge.net'
    
def findElementIn(element, kind, className):
    for check in element.getElementsByTagName(kind):
        if check.hasAttribute('class') and check.getAttribute('class') == className:
            return check
    raise Exception("Could not find %s with class '%s' in element %s" % (kind, className, element))

def findTd(element, className):
    return findElementIn(element, 'td', className)

def findA(element, className):
    return findElementIn(element, 'a', className)

def getUrl(row):
    url = findTd(row, 'ticket')
    link = url.getElementsByTagName('a')[0]
    return server + link.getAttribute('href')

def getDescription(row):
    description = findTd(row, 'summary')
    link = description.getElementsByTagName('a')[0]
    return link.firstChild.data

def getDate(row):
    date = findTd(row, 'date')
    return date.firstChild.data.lstrip().rstrip()

def parseRow(row):
    url = getUrl(row)
    description = getDescription(row)
    date = getDate(row)
    print "%s %s %s" % (url, description, date) 

def parse(data):
    dom = xml.dom.minidom.parseString(data)
    body = dom.getElementsByTagName("body")
    table = findElementIn(body[0], 'table', 'listing tickets')
    tbody = table.getElementsByTagName("tbody")
    if len(tbody) == 0:
        raise Exception("couldnt find a tbody in the tickets table")
    for row in tbody[0].getElementsByTagName("tr"):
        parseRow(row)

def get_url(url):
    import httplib
    connection = httplib.HTTPConnection('sourceforge.net')
    connection.request('GET', url)
    response = connection.getresponse()
    if response.status == 200:
        all = response.read()
        connection.close()
        return all
    else:
        connection.close()
        raise Exception("error %s %s" % (response.status, response.reason))

parse(get_url('/apps/trac/paintown/report/1'))
#import sys
#parse(sys.argv[1])
