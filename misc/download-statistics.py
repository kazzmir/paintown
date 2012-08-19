#!/usr/bin/env python

# Reads the download statistics from sourceforge.net using their json REST api
# and prints them out

def readJson(start, end, version = None):
    import json, httplib
    url = "/projects/paintown/files/paintown"
    if version != None:
        import re
        url += "/%s" % re.sub(' ', '%20', version)
    url += "/stats/json?start_date=%s&end_date=%s" % (start, end)
    connection = httplib.HTTPConnection('sourceforge.net')
    connection.request('GET', url)
    response = connection.getresponse()
    if response.status == 200:
        # print "Got response %s" % response
        all = response.read()
        # print "Response: %s" % all
        object = json.loads(all)
        connection.close()
        return object
    elif response.status == 301:
        headers = response.getheaders()
        new_location = headers['location']
        raise Exception("error 301")
    else:
        raise Exception("error %s %s" % (response.status, response.reason))

def lastMonth():
    import datetime
    return (datetime.date.today() + datetime.timedelta(-30)).isoformat()

def lastWeek():
    import datetime
    return (datetime.date.today() + datetime.timedelta(-7)).isoformat()

def today():
    import datetime
    return datetime.date.today().isoformat()

def hack():
    f = open('save', 'r')
    import json
    data = json.load(f)
    f.close()
    return data

def downloads(data):
    return data['summaries']['time']['downloads']

# data = readJson("2011-03-01", "2011-03-10", "3.4.0")
# print today()
# print lastWeek()
# print data
# data = readJson(lastWeek(), today(), "3.4.0")
# data = hack()

print "Today is %s" % today()
print

try:
    print "Downloads for the last week %s" % downloads(readJson(lastWeek(), today()))
    print "Downloads for the last month %s" % downloads(readJson(lastMonth(), today()))
except Exception, e:
    print "Couldn't get downloads!"
    print e

print

for version in ["2.1", "3.0", "paintown 3.1", "3.2", "3.3", "3.3.1", "3.4.0", '3.5.0', '3.6.0']:
    try:
        print "Downloads for version %s in the last week %s" % (version, downloads(readJson(lastWeek(), today(), version)))
    except Exception:
        pass
