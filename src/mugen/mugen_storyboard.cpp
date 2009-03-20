#include "mugen_storyboard.h"

#include "util/bitmap.h"
#include "init.h"
#include "resource.h"
#include "util/funcs.h"
#include "game/console.h"
#include "object/animation.h"
#include "object/object.h"
#include "object/character.h"
#include "object/object_attack.h"
#include "object/player.h"
#include "globals.h"
#include "factory/font_render.h"

#include "mugen_animation.h"
#include "mugen_background.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "mugen_font.h"

const int DEFAULT_WIDTH = 320;
const int DEFAULT_HEIGHT = 240;
const int DEFAULT_SCREEN_X_AXIS = 160;
const int DEFAULT_SCREEN_Y_AXIS = 0;

MugenLayer::MugenLayer():
actionno(-1),
startTime(0),
animation(0){
    offset.x = 0;
    offset.y = 0;
}
MugenLayer::~MugenLayer(){
}
void MugenLayer::act(){
    if (actionno != -1){
	animation->logic();
    }
}
void MugenLayer::draw(const int xaxis, const int yaxis, Bitmap *bmp){
    if (actionno != -1){
	animation->render(xaxis + offset.x, yaxis + offset.y, *bmp);
    }
}
	
MugenScene::MugenScene():
clearColor(Bitmap::makeColor(0,0,0)),
ticker(0),
endTime(0),
backgroundName(""){
    for (int i = 0; i < 10; ++i){
	// initiate the layers
	MugenLayer *layer = new MugenLayer();
	layers.push_back(layer);
    }
    defaultAxis.x = 0;
    defaultAxis.y = 0;
}
MugenScene::~MugenScene(){
    for ( std::vector< MugenBackground * >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	if ((*i))delete (*i);
    }
    // layers
    for ( std::vector< MugenLayer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
	if ((*i))delete (*i);
    }
    // foregrounds
    for ( std::vector< MugenBackground * >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	if ((*i))delete (*i);
    }
}
void MugenScene::act(){
    // backgrounds
    for ( std::vector< MugenBackground * >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	MugenBackground *background = *i;
	background->logic( 0, 0, 0, 0 );
    }
    // layers
    for ( std::vector< MugenLayer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
	MugenLayer *layer = *i;
	if(ticker >= layer->startTime)layer->act();
    }
    // foregrounds
    for ( std::vector< MugenBackground * >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	MugenBackground *foreground = *i;
	foreground->logic( 0, 0, 0, 0 );
    }
    // Fader
    fader.act();
    if (fader.getFadeInTime() <= ticker && fader.getState() != NOFADE ){
	fader.setState(NOFADE);
    } else if (ticker >= (fader.getFadeOutTime() - endTime) && fader.getState() != FADEOUT){
	fader.setState(FADEOUT);
    }
    // tick tick
    ticker++;
}
void MugenScene::draw(Bitmap *bmp){
    // backgrounds
    for (std::vector< MugenBackground * >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	MugenBackground *background = *i;
	background->render( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT, bmp );
    }
    // layers
    for (std::vector< MugenLayer *>::iterator i = layers.begin(); i != layers.end(); ++i ){
	MugenLayer *layer = *i;
	layer->draw(defaultAxis.x,defaultAxis.y,bmp);
    }
    // foregrounds
    for (std::vector< MugenBackground * >::iterator i = foregrounds.begin(); i != foregrounds.end(); ++i ){
	MugenBackground *foreground = *i;
	foreground->render( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT, bmp );
    }
    // fader
    fader.draw(bmp);
}

MugenStoryboard::MugenStoryboard( const string & s ):
location(s),
spriteFile(""),
startscene(0){
}
MugenStoryboard::~MugenStoryboard(){
    cleanup();
}
void MugenStoryboard::load() throw (MugenException){
     // Lets look for our def since some assholes think that all file systems are case insensitive
    std::string baseDir = Util::getDataPath() + "mugen/data/" + MugenUtil::getFileDir(location);
    const std::string ourDefFile = MugenUtil::fixFileName( baseDir, MugenUtil::stripDir(location) );
    // get real basedir
    //baseDir = MugenUtil::getFileDir( ourDefFile );
    Global::debug(1) << baseDir << endl;
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate storyboard definition file for: " + location );
    
    std::string filesdir = "";
    
    Global::debug(1) << "Got subdir: " << filesdir << endl;
     
    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    /* Extract info for our first section of our stage */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	std::string head = collection[i]->getHeader();
	MugenUtil::fixCase(head);
	if( head == "info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("name")!=std::string::npos ){
		    std::string temp;
		    *content->getNext() >> temp;
		    Global::debug(1) << "Read name '" << temp << "'" << endl;
		} else if ( itemhead.find("author")!=std::string::npos ){
		    std::string temp;
		    *content->getNext() >> temp;
                    Global::debug(1) << "Made by: '" << temp << "'" << endl;
		} else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}
	else if( head == "scenedef" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		MugenUtil::fixCase(itemhead);
		if ( itemhead.find("spr")!=std::string::npos ){
		    *content->getNext() >> spriteFile;
		    Global::debug(1) << "Got Sprite File: '" << spriteFile << "'" << endl;
		    MugenUtil::readSprites( MugenUtil::getCorrectFileLocation(baseDir, spriteFile), "", sprites );
		} else if ( itemhead.find("startscene")!=std::string::npos ){
		    *content->getNext() >> startscene;
                    Global::debug(1) << "Starting storyboard at: '" << startscene << "'" << endl;
		} else throw MugenException( "Unhandled option in SceneDef Section: " + itemhead );
	    }
	}
	else if( head == "scene " ){
	    MugenScene *scene = new MugenScene();
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		MugenUtil::fixCase(itemhead);
		if ( itemhead.find("fadein.time")!=std::string::npos ){
		    int time;
		    *content->getNext() >> time;
		    scene->fader.setFadeInTime(time);
		} else if ( itemhead.find("fadein.color")!=std::string::npos ){
		    int r,g,b;
		    *content->getNext() >> r;
		    *content->getNext() >> g;
		    *content->getNext() >> b;
		    scene->fader.setFadeInColor(Bitmap::makeColor(r,g,b));
		} else if ( itemhead.find("fadeout.time")!=std::string::npos ){
		    int time;
		    *content->getNext() >> time;
		    scene->fader.setFadeOutTime(time);
		} else if ( itemhead.find("fadeout.color")!=std::string::npos ){
		    int r,g,b;
		    *content->getNext() >> r;
		    *content->getNext() >> g;
		    *content->getNext() >> b;
		    scene->fader.setFadeOutColor(Bitmap::makeColor(r,g,b));
		} else if ( itemhead.find("bg.name")!=std::string::npos ){
		    *content->getNext() >> scene->backgroundName;
		} else if ( itemhead.find("clearcolor")!=std::string::npos ){
		    int r,g,b;
		    *content->getNext() >> r;
		    *content->getNext() >> g;
		    *content->getNext() >> b;
		    scene->clearColor = Bitmap::makeColor(r,g,b);
		} else if ( itemhead.find("end.time")!=std::string::npos ){
		    *content->getNext() >> scene->endTime;
		} else if ( itemhead.find("layer0.anim")!=std::string::npos ){
		    *content->getNext() >> scene->layers[0]->actionno;
		} else if ( itemhead.find("layer0.offset")!=std::string::npos ){
		    *content->getNext() >> scene->layers[0]->offset.x;
		    *content->getNext() >> scene->layers[0]->offset.y;
		} else if ( itemhead.find("layer0.starttime")!=std::string::npos ){
		    *content->getNext() >> scene->layers[0]->startTime;
		} else if ( itemhead.find("layer1.anim")!=std::string::npos ){
		    *content->getNext() >> scene->layers[1]->actionno;
		} else if ( itemhead.find("layer1.offset")!=std::string::npos ){
		    *content->getNext() >> scene->layers[1]->offset.x;
		    *content->getNext() >> scene->layers[1]->offset.y;
		} else if ( itemhead.find("layer1.starttime")!=std::string::npos ){
		    *content->getNext() >> scene->layers[1]->startTime;
		} else if ( itemhead.find("layer0.anim")!=std::string::npos ){
		    *content->getNext() >> scene->layers[2]->actionno;
		} else if ( itemhead.find("layer2.offset")!=std::string::npos ){
		    *content->getNext() >> scene->layers[2]->offset.x;
		    *content->getNext() >> scene->layers[2]->offset.y;
		} else if ( itemhead.find("layer2.starttime")!=std::string::npos ){
		    *content->getNext() >> scene->layers[2]->startTime;
		} else if ( itemhead.find("layer3.anim")!=std::string::npos ){
		    *content->getNext() >> scene->layers[3]->actionno;
		} else if ( itemhead.find("layer3.offset")!=std::string::npos ){
		    *content->getNext() >> scene->layers[3]->offset.x;
		    *content->getNext() >> scene->layers[3]->offset.y;
		} else if ( itemhead.find("layer3.starttime")!=std::string::npos ){
		    *content->getNext() >> scene->layers[3]->startTime;
		} else if ( itemhead.find("layer4.anim")!=std::string::npos ){
		    *content->getNext() >> scene->layers[4]->actionno;
		} else if ( itemhead.find("layer4.offset")!=std::string::npos ){
		    *content->getNext() >> scene->layers[4]->offset.x;
		    *content->getNext() >> scene->layers[4]->offset.y;
		} else if ( itemhead.find("layer4.starttime")!=std::string::npos ){
		    *content->getNext() >> scene->layers[4]->startTime;
		} else if ( itemhead.find("layer5.anim")!=std::string::npos ){
		    *content->getNext() >> scene->layers[5]->actionno;
		} else if ( itemhead.find("layer5.offset")!=std::string::npos ){
		    *content->getNext() >> scene->layers[5]->offset.x;
		    *content->getNext() >> scene->layers[5]->offset.y;
		} else if ( itemhead.find("layer5.starttime")!=std::string::npos ){
		    *content->getNext() >> scene->layers[5]->startTime;
		} else if ( itemhead.find("layer6.anim")!=std::string::npos ){
		    *content->getNext() >> scene->layers[6]->actionno;
		} else if ( itemhead.find("layer6.offset")!=std::string::npos ){
		    *content->getNext() >> scene->layers[6]->offset.x;
		    *content->getNext() >> scene->layers[6]->offset.y;
		} else if ( itemhead.find("layer6.starttime")!=std::string::npos ){
		    *content->getNext() >> scene->layers[6]->startTime;
		} else if ( itemhead.find("layer7.anim")!=std::string::npos ){
		    *content->getNext() >> scene->layers[7]->actionno;
		} else if ( itemhead.find("layer7.offset")!=std::string::npos ){
		    *content->getNext() >> scene->layers[7]->offset.x;
		    *content->getNext() >> scene->layers[7]->offset.y;
		} else if ( itemhead.find("layer7.starttime")!=std::string::npos ){
		    *content->getNext() >> scene->layers[7]->startTime;
		} else if ( itemhead.find("layer8.anim")!=std::string::npos ){
		    *content->getNext() >> scene->layers[8]->actionno;
		} else if ( itemhead.find("layer8.offset")!=std::string::npos ){
		    *content->getNext() >> scene->layers[8]->offset.x;
		    *content->getNext() >> scene->layers[8]->offset.y;
		} else if ( itemhead.find("layer8.starttime")!=std::string::npos ){
		    *content->getNext() >> scene->layers[8]->startTime;
		} else if ( itemhead.find("layer9.anim")!=std::string::npos ){
		    *content->getNext() >> scene->layers[9]->actionno;
		} else if ( itemhead.find("layer9.offset")!=std::string::npos ){
		    *content->getNext() >> scene->layers[9]->offset.x;
		    *content->getNext() >> scene->layers[9]->offset.y;
		} else if ( itemhead.find("layer9.starttime")!=std::string::npos ){
		    *content->getNext() >> scene->layers[9]->startTime;
		} else if ( itemhead.find("bgm")!=std::string::npos ){
		    // do nothing
		} else if ( itemhead.find("bgm.loop")!=std::string::npos ){
		    // do nothing
		} else throw MugenException( "Unhandled option in Scene Section: " + itemhead );
	    }
	    scenes.push_back(scene);
	}
	else if( head.find("begin action") != std::string::npos ){
	    head.replace(0,13,"");
	    int h;
	    MugenItem(head) >> h;
	    animations[h] = MugenUtil::getAnimation(collection[i], sprites);
	}
	else if( head.find("def") != std::string::npos ){
	    head.replace(0,13,"");
	    int h;
	    MugenItem(head) >> h;
	    animations[h] = MugenUtil::getAnimation(collection[i], sprites);
	}
	else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
    }
    // Set up the animations for those that have action numbers assigned (not -1 )
    // Also do their preload
    for( std::vector< MugenScene * >::iterator s = scenes.begin(); s != scenes.end(); ++s ){
	MugenScene *scene = *s;
	// backgrounds
	for( std::vector< MugenBackground * >::iterator i = scene->backgrounds.begin(); i != scene->backgrounds.end(); ++i ){
	    MugenBackground *background = *i;
	    if( background->getActionNumber() != -1 ){
		background->setAnimation( animations[ background->getActionNumber() ] );
	    }
	    // now load
	    background->preload( DEFAULT_SCREEN_X_AXIS, DEFAULT_SCREEN_Y_AXIS );
	}
	// foregrounds
	for( std::vector< MugenBackground * >::iterator i = scene->foregrounds.begin(); i != scene->foregrounds.end(); ++i ){
	    MugenBackground *foreground = *i;
	    if( foreground->getActionNumber() != -1 ){
		foreground->setAnimation( animations[ foreground->getActionNumber() ] );
	    }
	    // now load
	    foreground->preload( DEFAULT_SCREEN_X_AXIS, DEFAULT_SCREEN_Y_AXIS );
	}
	// layers
	for( std::vector< MugenLayer *>::iterator i = scene->layers.begin(); i != scene->layers.end(); ++i ){
	   MugenLayer *layer = *i;
	   if (layer->actionno != -1){
	       layer->animation = animations[ layer->actionno ];
	   }
	}
    }
}
void MugenStoryboard::run(Bitmap *bmp){
}
void MugenStoryboard::cleanup(){
    
    // Get rid of animation lists;
    for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	if( i->second )delete i->second;
    }
    
    // Get rid of background lists;
    for( std::vector< MugenScene * >::iterator i = scenes.begin() ; i != scenes.end() ; ++i ){
	if( (*i) )delete (*i);
    }
    // sprites
    for( std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
      for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    
}


