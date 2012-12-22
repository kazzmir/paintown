#include <map>
#include <string>

#include "move-list.h"
#include "util/token.h"
#include "../object/animation.h"
#include "../object/animation_event.h"
#include "../object/player.h"
#include "util/gui/scroll-list.h"
#include "util/input/input-map.h"
#include "util/input/input-manager.h"
#include "util/menu/menu.h"
#include "util/configuration.h"
#include "util/graphics/bitmap.h"
#include "util/init.h"

using std::endl;
using std::map;
using std::vector;
using std::string;

static const int GFX_X = 640;
static const int GFX_Y = 480;

namespace Paintown{

enum MoveListInput{
    Quit,
    Up,
    Down
};

/* shows moving lines on the grid */
struct Snake{
    enum Direction{
        Up, Down, Left, Right
    };

    Snake(int x1, int y1, int x2, int y2, Direction direction):
        x1(x1), y1(y1), x2(x2), y2(y2),
        direction(direction){
        }

    Snake(const Snake & copy):
        x1(copy.x1), y1(copy.y1),
        x2(copy.x2), y2(copy.y2),
        direction(copy.direction){
        }

    double x1, y1, x2, y2;
    Direction direction;

    void move(){
        double speed = 7;
        switch (direction){
            case Up: y1 -= speed; y2 -= speed; break;
            case Down: y1 += speed; y2 += speed; break;
            case Left: x1 -= speed; x2 -= speed; break;
            case Right: x1 += speed; x2 += speed; break;
        }
    }
};

class MoveItem: public Gui::ScrollItem {
public:
    MoveItem(const string & name, const Effects::Gradient & gradient):
        name(name),
        gradient(gradient){
        }

    string name;
    const Effects::Gradient & gradient;

    virtual void draw(int x, int y, const Graphics::Bitmap & where, const Font & font, int distance) const {
        Graphics::Color color = Graphics::makeColor(255, 255, 255);
        if (distance == 0){
            color = gradient.current();
        }
        font.printf(x, y, color, where, name, 0);
    }

    virtual int size(const Font & font) const {
        return font.textLength(name.c_str());
    }
};

static bool hasProjectileEvent(const Util::ReferenceCount<Animation> & animation){
    const vector<AnimationEvent *> & events = animation->getEvents();
    for (vector<AnimationEvent*>::const_iterator it = events.begin(); it != events.end(); it++){
        AnimationEvent * event = *it;
        if (event->getType() == AnimationEvent::ProjectileEvent){
            return true;
        }
    }

    return false;
}

static map<string, Util::ReferenceCount<Animation> > getAttacks(const map<string, Util::ReferenceCount<Animation> > & movements){
    map<string, Util::ReferenceCount<Animation> > out;
    for (map<std::string, Util::ReferenceCount<Animation> >::const_iterator find = movements.begin(); find != movements.end(); find++){
        string name = find->first;
        Util::ReferenceCount<Animation> animation = find->second;
        if (animation->isAttack() || hasProjectileEvent(animation)){
            out[name] = animation;
        }
    }
    return out;
}

struct MoveNode{
    MoveNode(){
    }

    MoveNode(const string & name):
        name(name){
        }

    /* how many nodes point to this node */
    int rank() const {
        return edgesFrom.size();
    }

    const vector<string> & nextNodes() const {
        return edgesTo;
    }

    const string & getName() const {
        return name;
    }

    string name;
    vector<string> edgesTo;
    vector<string> edgesFrom;
};

/* a directed acyclic graph (DAG)
 * nodes with no incoming edges (rank 0) are top level.
 * animations that have (sequence foo) in them will have an edge
 * from the referenced node, foo.
 */
class MoveGraph{
public:
    MoveGraph(){
    }

    void addNode(const string & name){
        if (nodes.find(name) == nodes.end()){
            nodes[name] = MoveNode(name);
        }
    }

    void addEdge(const string & from, const string & to){
        MoveNode & nodeFrom = nodes[from];
        MoveNode & nodeTo = nodes[to];
        nodeFrom.edgesTo.push_back(to);
        nodeTo.edgesFrom.push_back(from);
    }

    MoveNode getNode(const string & name) const {
        if (nodes.find(name) != nodes.end()){
            return nodes.find(name)->second;
        }

        return MoveNode();
    }

    vector<MoveNode> rank0Nodes() const {
        vector<MoveNode> out;
        for (map<string, MoveNode>::const_iterator it = nodes.begin(); it != nodes.end(); it++){
            if (it->second.rank() == 0){
                out.push_back(it->second);
            }
        }
        return out;
    }

    map<string, MoveNode> nodes;
};

static Util::ReferenceCount<Animation> findAnimation(const map<string, Util::ReferenceCount<Animation> > & movements, const string & name){
    if (movements.find(name) != movements.end()){
        return movements.find(name)->second;
    }
    return Util::ReferenceCount<Animation>(NULL);
}

static Util::ReferenceCount<MoveGraph> buildMoveGraph(const map<string, Util::ReferenceCount<Animation> > & movements){
    Util::ReferenceCount<MoveGraph> graph(new MoveGraph());

    for (map<string, Util::ReferenceCount<Animation> >::const_iterator it = movements.begin(); it != movements.end(); it++){
        string name = it->first;
        Util::ReferenceCount<Animation> animation = it->second;
        graph->addNode(animation->getName());
        const vector<string> & sequences = animation->getSequences();
        for (vector<string>::const_iterator sequence_it = sequences.begin(); sequence_it != sequences.end(); sequence_it++){
            string previous = *sequence_it;
            Util::ReferenceCount<Animation> who = findAnimation(movements, previous);
            if (who != NULL){
                graph->addNode(who->getName());
                graph->addEdge(who->getName(), name);
            }
        }
    }

    return graph;
}

static void dumpGraph(Util::ReferenceCount<MoveGraph> graph){
    vector<MoveNode> top = graph->rank0Nodes();
    Global::debug(0) << "Graph" << std::endl;
    for (vector<MoveNode>::iterator it = top.begin(); it != top.end(); it++){
        const MoveNode & node = *it;
        Global::debug(0) << node.getName() << std::endl;
    }
    Global::debug(0) << "End Graph" << std::endl;
}

static const int gridDistance = 7;
class MoveList{
public:
    MoveList(const InputSource & source, Util::ReferenceCount<Character> playerCopy, Gui::PopupBox & area, const Menu::Context & context):
        playerCopy(playerCopy),
        area(area),
        context(context),
        gradient(Menu::standardGradient()),
        idleWait(0),
        nextAnimation("idle"),
        counter(0),
        up(Storage::instance().find(Filesystem::RelativePath("sprites/arrows/up.png")).path()),
        down(Storage::instance().find(Filesystem::RelativePath("sprites/arrows/down.png")).path()),
        left(Storage::instance().find(Filesystem::RelativePath("sprites/arrows/left.png")).path()),
        right(Storage::instance().find(Filesystem::RelativePath("sprites/arrows/right.png")).path()),
        attack1(Storage::instance().find(Filesystem::RelativePath("sprites/arrows/attack-1.png")).path()),
        attack2(Storage::instance().find(Filesystem::RelativePath("sprites/arrows/attack-2.png")).path()),
        attack3(Storage::instance().find(Filesystem::RelativePath("sprites/arrows/attack-3.png")).path()),
        jump(Storage::instance().find(Filesystem::RelativePath("sprites/arrows/jump.png")).path()){
            playerCopy->testAnimation("idle");

            if (source.useKeyboard()){
                input.set(Keyboard::Key_ESC, 0, false, Quit);
                /* some standard way to set up the keys should be used here */
                input.set(Configuration::getUp(source.getKeyboard()), 0, true, Up);
                input.set(Configuration::getDown(source.getKeyboard()), 0, true, Down);
            }

            if (source.useJoystick()){
                int config = source.getJoystick();
                input.set(Joystick::Up, 0, true, Up);
                input.set(Joystick::Down, 0, true, Down);
                input.set(Joystick::Quit, 0, false, Quit);
            }

            list.setJustification(Gui::LeftJustify);

            const map<string, Util::ReferenceCount<Animation> > movements = getAttacks(playerCopy->getMovements());
            graph = buildMoveGraph(movements);
            // dumpGraph(graph);
            vector<MoveNode> start = graph->rank0Nodes();
            for (vector<MoveNode>::iterator it = start.begin(); it != start.end(); it++){
                string name = it->getName();
                list.addItem(Util::ReferenceCount<MoveItem>(new MoveItem(name, gradient)).convert<Gui::ScrollItem>());
            }
            /*
               for (map<std::string, Util::ReferenceCount<Animation> >::const_iterator find = movements.begin(); find != movements.end(); find++){
               list.addItem(Util::ReferenceCount<MoveItem>(new MoveItem(find->first, gradient)).convert<Gui::ScrollItem>());
               }
               */

            changeAnimation(list.getCurrentIndex());
        }

    Gui::NormalList list;

    Util::ReferenceCount<Character> playerCopy;
    Gui::PopupBox & area;
    InputMap<MoveListInput> input;
    const Menu::Context & context;
    Effects::Gradient gradient;
    int idleWait;
    string nextAnimation;
    Util::ReferenceCount<MoveGraph> graph;
    unsigned int counter;

    Graphics::Bitmap up;
    Graphics::Bitmap down;
    Graphics::Bitmap left;
    Graphics::Bitmap right;

    Graphics::Bitmap attack1, attack2, attack3;
    Graphics::Bitmap jump;

    vector<Snake> snakes;


    void updatePlayer(){
        int limit = 30;
        /* idle for X frames and then do the move */
        if (idleWait < limit){
            idleWait += 1;
            if (playerCopy->testAnimation()){
                playerCopy->testReset();
            }
            if (idleWait >= limit){
                playerCopy->testAnimation(nextAnimation);
                playerCopy->testReset();
            }
        } else {
            if (playerCopy->testAnimation()){
                MoveNode node = graph->getNode(playerCopy->getCurrentMovement()->getName());
                if (node.nextNodes().size() == 0){
                    idleWait = 0;
                    playerCopy->testAnimation("idle");
                    playerCopy->setFacing(Object::FACING_RIGHT);
                } else {
                    vector<string> nexts = node.nextNodes();
                    /* FIXME: handle all branches */
                    string first = nexts[0];
                    playerCopy->testAnimation(first);
                    playerCopy->testReset();
                }
            }
        }
    }

    void previous(){
        list.previous();
        changeAnimation(list.getCurrentIndex());
    }

    void next(){
        list.next();
        changeAnimation(list.getCurrentIndex());
    }

    void handleInput(){
        vector<InputMap<MoveListInput>::InputEvent> events = InputManager::getEvents(input, InputSource());
        unsigned int old = list.getCurrentIndex();
        for (vector<InputMap<MoveListInput>::InputEvent>::iterator it = events.begin(); it != events.end(); it++){
            const InputMap<MoveListInput>::InputEvent & event = *it;
            if (!event.enabled){
                continue;
            }

            if (area.isOpen() && event[Quit]){
                area.close();
            }

            if (event[Up]){
                list.previous();
                context.playSound(Menu::Up);
            }

            if (event[Down]){
                list.next();
                context.playSound(Menu::Down);
            }
        }

        if (old != list.getCurrentIndex()){
            changeAnimation(list.getCurrentIndex());
        }
    }

    void moveSnakes(){
        for (vector<Snake>::iterator it = snakes.begin(); it != snakes.end(); /**/){
            Snake & snake = *it;
            snake.move();
            int max = 200;
            if (snake.x1 < -max || snake.x1 > area.getArea().getWidth() + max ||
                snake.y1 < -max || snake.y1 > area.getArea().getHeight() + max){
                it = snakes.erase(it);
            } else {
                it++;
            }
        }
    }

    void run(){
        updatePlayer();

        counter += 1;

        /* TODO: maybe change the font here */
        // const Font & font = Font::getDefaultFont(20, 20);
        // area.act(font);
        gradient.update();
        // handleInput();
        moveSnakes();
    }

    vector<Util::ReferenceCount<Animation> > getCombo(const Util::ReferenceCount<Character> & player, const string & name){
        vector<Util::ReferenceCount<Animation> > out;
        MoveNode node = graph->getNode(name);
        Util::ReferenceCount<Animation> animation = player->getMovement(node.getName());
        if (animation != NULL){
            out.push_back(animation);
        }
        vector<string> next = node.nextNodes();
        if (next.size() > 0){
            /* FIXME: handle the whole tree */
            vector<Util::ReferenceCount<Animation> > more = getCombo(player, next[0]);
            out.insert(out.end(), more.begin(), more.end());
        }
        return out;
    }

    void changeAnimation(int animation){
        int count = 0;
        vector<MoveNode> nodes = graph->rank0Nodes();
        vector<MoveNode>::iterator find;
        // const map<string, Util::ReferenceCount<Animation> > movements = getAttacks(playerCopy->getMovements());
        // map<std::string, Util::ReferenceCount<Animation> >::const_iterator find;
        for (find = nodes.begin(); count != animation && find != nodes.end(); find++, count += 1){ /**/ }

        if (find != nodes.end()){
            nextAnimation = find->getName();
            idleWait = 0;
            playerCopy->testAnimation("idle");
            playerCopy->setFacing(Object::FACING_RIGHT);
            /*
               string name = find->first;
               playerCopy->testAnimation(name);
               playerCopy->setFacing(Object::FACING_RIGHT);
               */
        }
    }

    bool done(){
        return ! area.isActive();
    }


    int doDraw(const Graphics::Bitmap & what, int x, int y, bool highlight, const Graphics::Bitmap & where){
        if (highlight){
            where.rectangleFill(x - 1, y - 1, x + what.getWidth() + 1, y + what.getHeight() + 1, Graphics::makeColor(128, 128, 128));
        }
        what.draw(x, y, where);
        return x + what.getWidth() + 5;
    }

    void drawKeys(const vector<Util::ReferenceCount<Animation> > & combo, const Util::ReferenceCount<Animation> & movement, int x, int y, const Graphics::Bitmap & where){

        for (vector<Util::ReferenceCount<Animation> >::const_iterator it = combo.begin(); it != combo.end(); it++){
            const Util::ReferenceCount<Animation> & animation = *it;
            const vector<KeyPress> & keys = animation->getKeys();
            bool highlight = animation == movement;
            for (vector<KeyPress>::const_iterator it = keys.begin(); it != keys.end(); it++){
                const KeyPress & key = *it;
                if (key.combo.size() > 0){
                    Input::PaintownInput press = key.combo[0];
                    switch (press){
                        case Input::Forward: x = doDraw(right, x, y, highlight, where); break;
                        case Input::Back: x = doDraw(left, x, y, highlight, where); break;
                        case Input::Up: x = doDraw(up, x, y, highlight, where); break;
                        case Input::Down: x = doDraw(down, x, y, highlight, where); break;
                        case Input::Attack1: x = doDraw(attack1, x, y, highlight, where); break;
                        case Input::Attack2: x = doDraw(attack2, x, y, highlight, where); break;
                        case Input::Attack3: x = doDraw(attack3, x, y, highlight, where); break;
                        case Input::Jump: x = doDraw(jump, x, y, highlight, where); break;
                        default: break;
                    }
                }
            }
        }
    }

    void grid(const Graphics::Bitmap & buffer, unsigned int counter){
        Graphics::Color color = Graphics::makeColor(0x22, 0x44, 0x99);
        Graphics::Color white = Graphics::makeColor(196, 196, 196);
        for (int x = 0; x < buffer.getWidth(); x += gridDistance){
            buffer.line(x, 0, x, buffer.getHeight(), color);
        }
        for (int y = 0; y < buffer.getHeight(); y += gridDistance){
            buffer.line(0, y, buffer.getWidth(), y, color);
        }

        for (vector<Snake>::iterator it = snakes.begin(); it != snakes.end(); it++){
            const Snake & snake = *it;
            buffer.line(snake.x1, snake.y1, snake.x2, snake.y2, white);
        }

        if (Util::rnd(70) == 0){
            Snake::Direction direction = Snake::Direction(Util::rnd(4));
            switch (direction){
                case Snake::Up: {
                    int x = Util::rnd(0, buffer.getWidth() / gridDistance) * gridDistance;
                    int y1 = buffer.getHeight() + Util::rnd(50, 100);
                    int y2 = y1 + Util::rnd(10, 20);
                    snakes.push_back(Snake(x, y1, x, y2, direction));
                    break;
                }
                case Snake::Down: {
                    int x = Util::rnd(0, buffer.getWidth() / gridDistance) * gridDistance;
                    int y1 = Util::rnd(-100, -50);
                    int y2 = y1 + Util::rnd(10, 20);
                    snakes.push_back(Snake(x, y1, x, y2, direction));
                    break;
                }
                case Snake::Left: {
                    int y = Util::rnd(0, buffer.getHeight() / gridDistance) * gridDistance;
                    int x1 = buffer.getWidth() + Util::rnd(50, 100);
                    int x2 = x1 + Util::rnd(10, 20);
                    snakes.push_back(Snake(x1, y, x2, y, direction));
                    break;
                }
                case Snake::Right: {
                    int y = Util::rnd(0, buffer.getHeight() / gridDistance) * gridDistance;
                    int x1 = Util::rnd(-100, -50);
                    int x2 = x1 + Util::rnd(10, 20);
                    snakes.push_back(Snake(x1, y, x2, y, direction));
                    break;
                }
            }
        }


        /*
           int positionX = distance * (counter / 5 % (10 + (buffer.getWidth() > buffer.getHeight() ? buffer.getWidth() : buffer.getHeight())));
           int positionY = distance * (counter / 5 % (3 + (buffer.getWidth() > buffer.getHeight() ? buffer.getWidth() : buffer.getHeight())));
           buffer.line(positionX, 0, positionX, buffer.getHeight(), white);
           buffer.line(0, positionY, buffer.getWidth(), positionY, white);
           */
    }

    void draw(const Graphics::Bitmap & buffer, const Font & font){
        // area.render(buffer);
        Graphics::Bitmap space(buffer,
                               area.getArea().getX() + area.getTransforms().getRadius(),
                               area.getArea().getY(),
                               area.getArea().getWidth() - area.getTransforms().getRadius(),
                               area.getArea().getHeight() - area.getTransforms().getRadius());
        // space.clear();
        // const Font & font = Font::getDefaultFont(20, 20);
        // listMovements(space, selected);
        int margin = 180;
        int playerX = space.getWidth() - margin - area.getTransforms().getRadius();
        if (playerX < 1){
            playerX = 1;
        }
        int playerY = (int)(playerX / 1.3333);
        if (playerY < 1){
            playerY = 1;
        }
        Graphics::Bitmap playerArea(space, margin, 30, playerX, playerY);
        Graphics::StretchedBitmap show(playerArea.getWidth() / 1.7, playerArea.getHeight() / 1.7, playerArea);
        show.start();
        grid(show, counter);
        playerCopy->setX(show.getWidth() / 2);
        playerCopy->setY(0);
        playerCopy->setZ(show.getHeight() - 10);
        playerCopy->draw(&show, 0, 0);
        show.finish();

        list.render(Graphics::Bitmap(space, 2, 0, space.getWidth(), space.getHeight()), font);

        // int x = playerCopy->getX();
        int x = margin + 50;
        int y = space.getHeight() - 50;
        // drawKeys(playerCopy->getMovement(logic.nextAnimation), x, y, space);
        /* FIXME: show keys for the entire combo */
        drawKeys(getCombo(playerCopy, nextAnimation), playerCopy->getCurrentMovement(), x, y, space);

        // space.border(0, 2, Graphics::makeColor(128, 128, 128));
        // buffer.BlitToScreen();
    }
};

class Runner: public Util::Logic, public Util::Draw {
    public:
        Runner(MoveList & main, Gui::PopupBox & area):
            main(main),
        area(area){
            // background.BlitFromScreen(0, 0);
        }

        MoveList & main;
        Gui::PopupBox & area;
        Graphics::Bitmap background;

        virtual void run(){
            const Font & font = Menu::menuFontParameter.current()->get();
            main.run();
            main.handleInput();
            area.act(font);
        }

        bool done(){
            return main.done();
        }

        /* FIXME: the background shifts by one or two pixels when its shown */
        void drawFirst(const Graphics::Bitmap & buffer){
            background.resize(GFX_X, GFX_Y);
            buffer.Stretch(background);
        }

        void draw(const Graphics::Bitmap & buffer){
            Graphics::StretchedBitmap work(640, 480, buffer, Graphics::qualityFilterName(Configuration::getQualityFilter()));
            const Font & font = Menu::menuFontParameter.current()->get();
            work.start();
            background.Blit(work);
            area.render(work, font);
            main.draw(work, font);
            work.finish();
            buffer.BlitToScreen();
        }

        double ticks(double system){
            return system * Global::ticksPerSecond(90);
        }
};

void showMoveList(Player * player, const Menu::Context & context){
    if (getAttacks(player->getMovements()).size() == 0){
        /* no attacks, failure! */
        Global::debug(0) << "No attacks for " << player->getName() << " so the move list can't be shown" << endl;
        return;
    }

    Util::ReferenceCount<Character> playerCopy(new Character(*player));
    Gui::PopupBox area;
    area.location.setDimensions(GFX_X - 75, GFX_Y - 75);
    area.location.setCenterPosition(Gui::RelativePoint(0, 0));
    area.transforms.setRadius(20);

    area.colors.body = Graphics::makeColor(0,0,0);
    area.colors.bodyAlpha = 220;
    area.colors.border = Graphics::makeColor(200,200,200);
    area.colors.borderAlpha = 200;

    area.open();
    MoveList all(player->getInput(), playerCopy, area, context);
    Runner runner(all, area);

    Util::standardLoop(runner, runner);
}

class MoveListTab: public Gui::Tab {
public:
    MoveListTab(Player * player, const Menu::Context & context):
        opened(false),
        player(player),
        main(player->getInput(), Util::ReferenceCount<Character>(new Character(*player)), getContext().getBoard(), context){
            setName(player->getName());

            /*
               getContext().transforms.setRadius(20);
               */

            getContext().colors.body = Graphics::makeColor(0,0,0);
            getContext().colors.bodyAlpha = 220;
            getContext().colors.border = Graphics::makeColor(200,200,200);
            getContext().colors.borderAlpha = 200;
        }

    bool opened;
    Player * player;
    MoveList main;

    /* don't keep re-opening the context box */
    virtual void open(){
        if (!opened){
            Gui::Tab::open();
            opened = true;
        }
    }

    /* don't ever close either */
    virtual void close(){
    }

    virtual void previous(const Font & font){
        main.previous();
    }

    virtual void next(const Font & font){
        main.next();
    }

    virtual void act(const Font & font){
        context.act(font);
        main.run();
    }

    virtual void render(const Graphics::Bitmap & work, const Font & font){
        context.render(work, font);
        main.draw(work, font);
    }
};

}

OptionMoveList::OptionMoveList(const Gui::ContextBox & parent, const Token * token, const vector<Paintown::Player *> & players):
MenuOption(parent, token),
players(players){
    readName(token);
}

OptionMoveList::~OptionMoveList(){
}

void OptionMoveList::logic(){
}

void OptionMoveList::run(const Menu::Context & context){
    if (players.size() > 1){
        Util::ReferenceCount<Menu::TabRenderer> renderer = Util::ReferenceCount<Menu::TabRenderer>(new Menu::TabRenderer());
        Menu::Menu menu(renderer.convert<Menu::Renderer>());
        Gui::TabbedBox & box = renderer->getBox();
        box.location.setDimensions(GFX_X - 50, GFX_Y - 50);
        box.location.setCenterPosition(Gui::RelativePoint(0, 0));
        for (vector<Paintown::Player*>::iterator it = players.begin(); it != players.end(); it++){
            box.addTab(new Paintown::MoveListTab(*it, context));
        }

        try{
            menu.run(context);
        } catch (const Menu::MenuException & fail){
        } catch (const Exception::Return & fail){
        }
    } else if (players.size() == 1){
        Paintown::showMoveList(players[0], context);
    }
}
