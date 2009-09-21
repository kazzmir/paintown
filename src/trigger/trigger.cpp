#include "trigger.h"
#include "util/token.h"

Trigger * Trigger::parse(Token * token) throw (TokenException) {
    Token * type = token->findToken("trigger/type");
    if (type == NULL){
        throw TokenException("Expected to find trigger/type");
    }
    return NULL;
}
