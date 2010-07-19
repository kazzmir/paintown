#include "display-character.h"
#include "pack-reader.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util.h"
#include "object/animation.h"
#include "object/draw-effect.h"
#include "animation.h"
#include <vector>

using namespace std;

namespace Bor{

DisplayCharacter::DisplayCharacter(PackReader & reader, std::string file):
::DisplayCharacter(file),
reader(reader){
}

static vector<Token> getAnimations(Token * head){
    vector<Token> out;
    while (head->hasTokens()){
        Token * current;
        *head >> current;
        /* labels make this easier */
        again:
        if (*current == "anim"){
            bool done = false;
            Token top(*current);
            while (!done){
                *head >> current;
                if (*current == "anim"){
                    done = true;
                } else {
                    top.addToken(current);
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
    char * raw = reader.readFile(reader.getFile(path));
    string data = Bor::doParse(raw, reader.getFileLength(path));
    // Global::debug(0) << "Bor input: '" << parsed << "'" << endl;
    delete[] raw;

    TokenReader reader;
    Token * head = reader.readTokenFromString(data);

    try{
        Token * name = head->findToken("_/name");
        if (name != NULL){
            string what;
            *name >> what;
            setName(what);
        }

        vector<Token *> remaps = head->findTokens("_/remap");
        for (vector<Token*>::iterator it = remaps.begin(); it != remaps.end(); it++){
            Token * remap = *it;
            string first;
            string second;
            *remap >> first >> second;
            addRemap(new Remap(Filesystem::RelativePath(first), Filesystem::RelativePath(second), mapper[0]));
        }

        vector<Token> animations = getAnimations(head);
        for (vector<Token>::iterator it = animations.begin(); it != animations.end(); it++){
            Token & animation = *it;
            string name;
            animation >> name;
            // animation.resetToken();
            /* only care about the 'idle' animation */
            if (name == "idle"){
                ::Animation * ani = new Animation(&animation, this);
                ani->setName("idle");

                if (getMovement("idle") != NULL){
                    delete getMovement("idle");
                }

                setMovement(ani, "idle");
            }
        }
    } catch (const TokenException & ex){
        // Global::debug(0) << "Could not read " << path << " : " << ex.getReason() << endl;
        throw LoadException(__FILE__, __LINE__, ex, "Could not open character file: " + path);
    } catch (const Filesystem::NotFound & ex){
        throw LoadException(__FILE__, __LINE__, ex, "Could not open character file: " + path);
    }

    if (getMovement( "idle" ) == NULL){
        throw LoadException(__FILE__, __LINE__, "No 'idle' animation given for " + path );
    }

    animation_current = getMovement("idle");
    /* run the animation up till the first frame */
    animation_current->Act();
    effects.push_back(new DrawNormalEffect(this));
}

}
