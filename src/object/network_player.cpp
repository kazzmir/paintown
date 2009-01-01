#include "animation.h"
#include "factory/font_factory.h"
#include "factory/font_render.h"
#include "globals.h"
#include <math.h>
#include "nameplacer.h"
#include "network_character.h"
#include "network_player.h"
#include "object.h"
#include "object_messages.h"
#include <sstream>
#include "util/bitmap.h"
#include "util/font.h"
#include "util/funcs.h"
#include <vector>
#include "world.h"

using namespace std;

NetworkPlayer::NetworkPlayer(const string & filename, int alliance) throw ( LoadException ):
NetworkCharacter( filename, alliance ),
score(0),
attack_bonus(0){
    initializeAttackGradient();
}

NetworkPlayer::NetworkPlayer(const Character & chr) throw( LoadException ):
NetworkCharacter( chr ),
score(0),
attack_bonus(0){
    initializeAttackGradient();
}

NetworkPlayer::~NetworkPlayer(){
}
	
Object * NetworkPlayer::copy(){
	return new NetworkPlayer(*this);
}

void NetworkPlayer::initializeAttackGradient(){
    Util::blend_palette(attack_gradient, num_attack_gradient / 2, Bitmap::makeColor(255,255,255), Bitmap::makeColor(255,255,0));
    Util::blend_palette(attack_gradient + num_attack_gradient / 2, num_attack_gradient / 2, Bitmap::makeColor(255,255,0), Bitmap::makeColor(255,0,0));
}

void NetworkPlayer::interpretMessage( Network::Message & message ){
    int type;
    message >> type;
    message.reset();
    NetworkCharacter::interpretMessage(message);
    switch (type){
        case PlayerMessages::Score : {
            unsigned int s;
            message >> s;
            score = s;
            break;
        }
    }
}
        
void NetworkPlayer::attacked( World * world, Object * something, vector< Object * > & objects ){
    NetworkCharacter::attacked(world, something, objects);
    score += (int)(85 * (1 + attack_bonus));
    attack_bonus += 1;
    Global::debug(1) << "Attack bonus : " << attack_bonus << endl;
    world->addMessage(scoreMessage());
}

Network::Message NetworkPlayer::scoreMessage(){
    Network::Message m;

    m.id = getId();
    m << PlayerMessages::Score;
    m << score;
    m << (int)(attack_bonus * 10);

    return m;
}

void NetworkPlayer::draw( Bitmap * work, int rel_x ){
    Character::draw( work, rel_x );
    static const char * PLAYER_FONT = "/fonts/arial.ttf";

    int x1, y1;
    NamePlacer::getPlacement( x1, y1, name_id );

    if ( icon )
        icon->draw( x1, y1, *work );

    int hasIcon = icon ? icon->getWidth() : 0;

    const Font & player_font = Font::getFont( Util::getDataPath() + PLAYER_FONT, 20, 20 );
    const string & name = getName();
    int nameHeight = player_font.getHeight( name ) / 2;
    nameHeight = 20 / 2;
    FontRender * render = FontRender::getInstance();
    render->addMessage( player_font, (hasIcon + x1) * 2, y1 * 2, Bitmap::makeColor(255,255,255), -1, name );

    ostringstream score_s;
    score_s << score;
    int attack_color = (int)(attack_bonus * 10);
    if (attack_color > num_attack_gradient - 1){
        attack_color = num_attack_gradient - 1;
    }

    int attack_x = (int)((hasIcon + x1) * 2 + 100 * Bitmap::getScale() - player_font.textLength(score_s.str().c_str()));

    render->addMessage(player_font, attack_x, y1 * 2, attack_gradient[attack_color], -1, score_s.str().c_str());

    drawLifeBar( hasIcon + x1, y1 + nameHeight, work );
    // int max = getMaxHealth() < 100 ? getMaxHealth() : 100;
    // render->addMessage( player_font, (x1 + hasIcon + max + 5) * 2, y1 + nameHeight, Bitmap::makeColor(255,255,255), -1, "x %d", getLives() );
}

/* just performs the current animation */
void NetworkPlayer::act( vector< Object * > * others, World * world, vector< Object * > * add ){
    NetworkCharacter::act(others, world, add);

    if (attack_bonus > 0){
        attack_bonus -= 0.02;
    } else {
        attack_bonus = 0;
    }
}
