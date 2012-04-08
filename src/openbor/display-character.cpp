#include "display-character.h"
#include "pack-reader.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util.h"
#include "paintown-engine/object/animation.h"
#include "paintown-engine/object/draw-effect.h"
#include "animation.h"
#include <vector>

using namespace std;

namespace Bor{

DisplayCharacter::DisplayCharacter(PackReader & reader, const Filesystem::AbsolutePath & file):
Paintown::DisplayCharacter(file),
reader(reader){
}

static vector<Token> getAnimations(Token * head){
    vector<Token> out;
    TokenView view = head->view();
    /* FIXME: this code has issues.. */
    while (view.hasMore()){
        const Token * current;
        view >> current;
        /* labels make this easier */
        again:
        if (*current == "anim"){
            bool done = false;
            Token top(*current);
            while (!done){
                view >> current;
                if (*current == "anim"){
                    done = true;
                } else {
                    top.addToken((Token*) current);
                }
                if (!head->hasTokens()){
                    done = true;
                }
            }
            out.push_back(top);
            if (*current == "anim"){
                goto again;
            }
        }
    }
    return out;
}
    
void DisplayCharacter::load(){
    char * raw = reader.readFile(reader.getFile(path.path()));
    string data = Bor::doParse(raw, reader.getFileLength(path.path()));
    // Global::debug(0) << "Bor input: '" << parsed << "'" << endl;
    delete[] raw;

    TokenReader reader;
    Token * head = reader.readTokenFromString(data);

    try{
        const Token * name = head->findToken("_/name");
        if (name != NULL){
            string what;
            name->view() >> what;
            setName(what);
        }

        vector<const Token *> remaps = head->findTokens("_/remap");
        for (vector<const Token*>::iterator it = remaps.begin(); it != remaps.end(); it++){
            const Token * remap = *it;
            string first;
            string second;
            remap->view() >> first >> second;
            if (newRemap(first, second)){
                addRemap(new Paintown::Remap(Filesystem::RelativePath(first), Filesystem::RelativePath(second)));
            }
        }

        vector<Token> animations = getAnimations(head);
        for (vector<Token>::iterator it = animations.begin(); it != animations.end(); it++){
            Token & animation = *it;
            string name;
            animation.view() >> name;
            // animation.resetToken();
            /* only care about the 'idle' animation */
            if (name == "idle"){
                Paintown::Animation * ani = new Animation(&animation, this);
                ani->setName("idle");
                setMovement(ani, "idle");
            }
        }
    } catch (const TokenException & ex){
        // Global::debug(0) << "Could not read " << path << " : " << ex.getReason() << endl;
        throw LoadException(__FILE__, __LINE__, ex, "Could not open character file: " + path.path());
    } catch (const Filesystem::NotFound & ex){
        throw LoadException(__FILE__, __LINE__, ex, "Could not open character file: " + path.path());
    }

    if (getMovement( "idle" ) == NULL){
        throw LoadException(__FILE__, __LINE__, "No 'idle' animation given for " + path.path());
    }

    animation_current = getMovement("idle");
    /* run the animation up till the first frame */
    animation_current->Act();
    effects.push_back(new Paintown::DrawNormalEffect(this));
}

}
