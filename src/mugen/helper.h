#include "stage.h"
#include "character.h"
#include "behavior.h"

namespace Mugen{
/* copy all data from the parent somehow, maybe lazily. to speed things up */
class Helper: public Character {
public:
    Helper(Character & parent);
    virtual ~Helper();

protected:
    virtual bool doStates(MugenStage & stage, const std::vector<std::string> & active, int stateNumber);

protected:
    Character & owner;
    HitDefinition hit;
    DummyBehavior dummy;

};

}
