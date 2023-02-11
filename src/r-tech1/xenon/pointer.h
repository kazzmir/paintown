#ifndef _rtech1_util_pointer_h
#define _rtech1_util_pointer_h

#include <stdlib.h>

namespace Util{

/* Some helpful pointer classes, probably equivalent to stuff in boost
 */

template <class Data>
class ReferenceCount{
public:
    explicit ReferenceCount(Data * what = NULL):
    count(NULL),
    data(what){
        count = new int;
        *count = 1;
    }

    ReferenceCount(const ReferenceCount<Data> & him){
        data = him.data;
        count = him.count;
        *count += 1;
    }

    ReferenceCount & operator=(const ReferenceCount<Data> & him){
        /* this object might have a link to `him' so if we release first
         * then `him' will get deleted before we can capture his data.
         * we need to increase his count first and then release our data
         * to insure that `him' will still be alive.
         */
        int * temp_count = him.count;
        *temp_count += 1;
        Data * temp_data = him.data;
        release();
        data = temp_data;
        count = temp_count;

        return *this;
    }

    ReferenceCount & operator=(Data * what){
        release();
        count = new int;
        *count = 1;
        data = what;
        return *this;
    }

    /* use this if you need to convert between classes in a heirarchy.
     * Base
     * Child: public Base
     * ReferenceCount<Base> base;
     * ReferenceCount<Child> child;
     * base = child; is illegal but
     * base = child.convert<Base>(); is ok
     */
    /*
    template <class Convert>
    ReferenceCount<Convert> convert() const {
        ReferenceCount<Convert> what;
        what.release();
        what.count = count;
        *what.count += 1;
        what.data = (Convert*) data;
        return what;
    }
    */

    template <class Convert>
    operator ReferenceCount<Convert>() const {
        ReferenceCount<Convert> out;
        out.release();
        out.data = (Convert*) this->data;
        out.count = this->count;
        *out.count += 1;
        return out;
    }

    bool operator!() const {
        return !this->data;
    }

    Data * operator->() const {
        return data;
    }
    
    Data & operator*() const {
        return *data;
    }

    /* get the raw pointer -- be ultra careful! */
    Data * raw() const {
        return data;
    }

    bool operator<(const ReferenceCount<Data> & him) const {
        return data < him.data;
    }

    bool operator==(const ReferenceCount<Data> & him) const {
        return data == him.data;
    }

    bool operator!=(const ReferenceCount<Data> & him) const {
        return !(*this == him);
    }

    bool operator==(const void * what) const {
        return data == what;
    }

    bool operator!=(const void * what) const {
        return !(*this == what);
    }

    virtual ~ReferenceCount(){
        release();
    }

    /* this has to be public to make the convert() function work but pretend
     * its private!
     */
public:
    void release(){
        *count -= 1;
        if (*count == 0){
            delete data;
            delete count;
            data = NULL;
            count = NULL;
        }
    }
    int * count;
    Data * data;
};

/* Same as ReferenceCount but by default passes a new instance of Data to the constructor
 * of ReferenceCount
 */
template <class Data>
class NewReferenceCount: public ReferenceCount<Data> {
public:
    NewReferenceCount():
    ReferenceCount<Data>(new Data()){
    }
};

/* Initializes its pointer to NULL and deletes the data in the destructor.
 * how is this different from the ReferenceCount class above? its basically
 * the same thing but only allows one owner at a time.
 */
template <class Data>
class ClassPointer{
public:
    ClassPointer():
        data(NULL){
        }

    ClassPointer(Data * him):
    data(him){
    }

    ClassPointer & operator=(Data * him){
        if (data != NULL){
            delete data;
        }

        data = him;
        return *this;
    }

    Data & operator*() const {
        return *data;
    }

    bool operator==(const void * what) const {
        return data == what;
    }

    bool operator!=(const void * what) const {
        return !(*this == what);
    }

    Data* operator->() const {
        return data;
    }

    virtual ~ClassPointer(){
        delete data;
    }

private:
    Data* data;
};

}

#endif
