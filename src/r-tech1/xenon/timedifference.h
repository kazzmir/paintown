#ifndef _time_difference_h
#define _time_difference_h

#include <time.h>
#include <sys/time.h>
#include <string>

class TimeDifference{
public:
    TimeDifference();

    void startTime();
    void endTime();

    unsigned long long int getTime();

    const std::string printTime();
    const std::string printTime(const std::string & description, int runs = 1);

    const std::string printAverageTime(const std::string & description, int runs);

    ~TimeDifference();

protected:
    unsigned long long start, end;

};

/* Put this in a scope where you want to measure time differences. The constructor will
 * get the start time and the destructor will get the end time and also print the difference
 * between the two.
 * {
 *   TimeCheck check;
 *   call_some_stuff();
 * }
 *
 * Will print out how long `call_some_stuff' took. This class is somewhat useful in cases that the
 * code could abort early due to a return or an exception so its not always clear where to
 * call endTime() for a TimeDifference object.
 */
class TimeCheck{
public:
    TimeCheck(const std::string & description);
    virtual ~TimeCheck();

protected:
    std::string description;
    TimeDifference time;
};

#endif
