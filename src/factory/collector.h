#ifndef _paintown_collector_h
#define _paintown_collector_h

/* this class calls other static classes destroy() methods */
class Collector{
public:
    friend int paintown_main(int, char**);
protected:
    Collector();
    ~Collector();
};

#endif
