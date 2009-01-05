#ifndef _collector_h
#define _collector_h

class Resource;
/* this class calls other static classes destroy() methods */
class Collector{
public:
	Collector();
	~Collector();
private:
        Resource * resource;
};

#endif
