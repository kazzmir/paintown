#include "bitmap.h"

bool operator==(const INTERNAL_COLOR&, const INTERNAL_COLOR&){
    /* FIXME */
    return false;
}

bool operator!=(const INTERNAL_COLOR& left, const INTERNAL_COLOR& right){
    return !(left == right);
}

bool operator<(const INTERNAL_COLOR&, const INTERNAL_COLOR&){
    /* FIXME */
    return false;
}
