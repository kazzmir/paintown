#ifndef _paintown_trigger_h
#define _paintown_trigger_h

#include "r-tech1/token_exception.h"

class Token;
class Scene;

/* design goals of a trigger
 * 1. can contain arbitrary code (possibly scheme since we are
 * already using s-expressions)
 *   (trigger (scheme s-expr))
 *   or
 *   (trigger (python file.py))
 *   or
 *   (trigger add-light (x 50) (y 100) (strength 20))
 * 2. fired based on arbitrary constraints
 *   (trigger (check blah))
 */
class Trigger{
public:
    Trigger();

    /* do something
     * should a World object be passed in?
     */ 
    virtual void execute(Scene * level) = 0;

    /* whether or not this trigger should execute */
    virtual bool shouldExecute() const = 0;

    virtual ~Trigger();

    static Trigger * parse(const Token * token);
};

#endif
