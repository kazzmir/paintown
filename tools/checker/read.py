import lxml.etree

def blah(xml):
    xml.xpath("/GCC_XML/Method[@file='f3']")

def read(file):
    return lxml.etree.parse(file)
