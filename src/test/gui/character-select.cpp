#include "character-select.h"

#include "util/bitmap.h"
#include "util/font.h"
#include "util/pointer.h"
#include "util/stretch-bitmap.h"
#include "util/input/input.h"
#include "util/input/input-manager.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "util/file-system.h"
#include "util/load_exception.h"

#include "util/gui/animation.h"
#include "util/gui/select-list.h"

CharacterSelect::CharacterSelect(){
}

CharacterSelect::CharacterSelect(std::string & filename){
    TokenReader tr(Filesystem::AbsolutePath(filename).path());
    Token * token = tr.readToken();
    load(token);
}

CharacterSelect::CharacterSelect(Token * token){
    load(token);
}

CharacterSelect::~CharacterSelect(){
}

void CharacterSelect::load(Token * token){
    try {
        if ( *token != "character-select" ){
            throw LoadException(__FILE__, __LINE__, "Not a Character Select");
        }
        TokenView view = token->view();
        while (view.hasMore()){
            try{
                const Token * tok;
                view >> tok;
                if (tok->match("name", name)){
                }
            } catch ( const TokenException & ex ) {
                throw LoadException(__FILE__, __LINE__, ex, "Character Select parse error");
            } catch ( const LoadException & ex ) {
                throw ex;
            }
        }
    } catch (const TokenException & e){
        throw LoadException(__FILE__, __LINE__, e, "Error loading Character Select file.");
    }
}