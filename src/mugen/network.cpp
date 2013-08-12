#ifdef HAVE_NETWORKING

/* Server and client communicate to each other over a single TCP connection. Both send their inputs as soon
 * as possible to the other side. Occasionally the server will send a full world state. Once a second or so
 * the server will send a ping packet.
 *
 * Both client and server will run the game independant of the other. When any side receives an input
 * then that side will replay the game state from the time the input happened, or sometime soon before it.
 * In this way both the server and client should end up with the same game state.
 *
 * Due to subtle problems in floating point calculations it becomes necessary to update the client state
 * with the full world state.
 */

#include "network.h"
#include "behavior.h"
#include "util/system.h"
#include "world.h"
#include "character.h"
#include "game.h"
#include "config.h"
// #include "command.h"
#include "constraint.h"

#include "util/network/network.h"
#include "util/thread.h"
#include "util/token.h"
#include "util/lz4/lz4.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

/* FIXME: move this to token.h */
static Token * filterTokens(Token * start){
    if (start->isData()){
        return start->copy();
    }

    Token * out = new Token(start->getName());
    for (vector<Token*>::const_iterator it = start->getTokens()->begin(); it != start->getTokens()->end(); it++){
        Token * use = filterTokens(*it);
        if (use->isData() || use->numTokens() > 0){
            *out << use;
        }
    }
    return out;
}

namespace PaintownUtil = ::Util;

namespace Mugen{

NetworkObserver::NetworkObserver():
StageObserver(){
}
    
NetworkObserver::~NetworkObserver(){
}

/* FIXME: split this into InputNetworkBuffer and OutputNetworkBuffer */
class NetworkBuffer{
public:
    NetworkBuffer(int size = 128){
        length = 0;
        actualLength = size;
        buffer = new char[actualLength];
        contains = 0;
    }

    ~NetworkBuffer(){
        delete[] buffer;
    }

    NetworkBuffer & operator<<(int16_t data){
        checkBuffer(sizeof(data));
        Network::dump16(buffer + length, data);
        length += sizeof(data);
        return *this;
    }

    NetworkBuffer & operator<<(uint32_t data){
        checkBuffer(sizeof(data));
        Network::dump32(buffer + length, data);
        length += sizeof(data);
        return *this;
    }

    NetworkBuffer & operator>>(int16_t & data){
        if ((int)(length + sizeof(data)) < contains){
            char * where = buffer + length;
            char * out = Network::parse16(where, (uint16_t*) &data);
            length += out - where;
        } else {
            data = -1;
        }
        return *this;
    }

    NetworkBuffer & operator>>(uint32_t & data){
        if ((int)(length + sizeof(data)) < contains){
            char * where = buffer + length;
            char * out = Network::parse32(where, &data);
            length += out - where;
        } else {
            data = -1;
        }
        return *this;
    }

    /* send an lz4 compressed string */
    NetworkBuffer & operator<<(const string & str){
        // Global::debug(0) << "Size: " << compact.size() << std::endl;
        char * out = new char[LZ4_compressBound(str.size())];
        int compressed = LZ4_compress(str.c_str(), out, str.size());

        /* send compressed size, uncompressed size, then raw compressed data */
        *this << (int16_t) compressed;
        *this << (int16_t) str.size();
        add(out, compressed);
        return *this;
    }

#if 0
    // currently broken
    NetworkBuffer & operator>>(string & str){
        int16_t size = 0;
        *this >> size;
        /* FIXME: if we read less bytes than we need make sure to read the rest
         * before continuing.
         */
        // Global::debug(0) << "Read string of length " << size << std::endl;
        if (size > 0 && length + size <= contains){
            char out[size+1];
            memcpy(out, buffer + length, size);
            length += size;
            out[size] = 0;
            str = string(out);
        }
        return *this;
    }
#endif

    virtual void readAll(const Network::Socket & socket){
        contains = Network::readUptoBytes(socket, (uint8_t*) buffer, actualLength);
    }

    virtual void add(const char * data, int size){
        checkBuffer(size);
        memcpy(buffer + length, data, size);
        length += size;
    }

    char * getBuffer(){
        return buffer;
    }

    /* make sure N bytes can be stored */
    void checkBuffer(int bytes){
        if (length + bytes >= actualLength){
            increaseBuffer(bytes);
        }
    }

    void send(const Network::Socket & socket){
        Network::sendBytes(socket, (uint8_t*) getBuffer(), getLength());
    }

    /* will do a single nlWrite instead of trying to send all the bytes.
     * The buffer length had better be below the maximum packet size
     * which is something around 64k
     */
    /* mostly useful for udp */
    void sendAllxx(const Network::Socket & socket){
        Network::sendAllBytes(socket, (uint8_t*) getBuffer(), getLength());
    }

    int getLength() const {
        return length;
    }

    void increaseBuffer(int minimum){
        int newLength = PaintownUtil::max(actualLength * 2, actualLength + minimum);
        char * next = new char[newLength];
        memcpy(next, buffer, length);
        delete[] buffer;
        buffer = next;
        actualLength = newLength;
    }

protected:
    int length;
    char * buffer;
    int actualLength;
    int contains;
};

static const int16_t NetworkMagic = 0xd97f; 

class Packet{
public:
    enum Type{
        InputType,
        PingType,
        WorldType
    };

    Packet(Type type):
    type(type){
    }

    Type getType() const {
        return type;
    }

    Type type;
};

class InputPacket: public Packet {
public:
    InputPacket(const Input & inputs, uint32_t tick):
    Packet(InputType),
    inputs(inputs),
    tick(tick){
    }

    InputPacket():
    Packet(InputType),
    tick(0){
    }

    Input inputs;
    uint32_t tick;
};

class PingPacket: public Packet {
public:
    PingPacket(int ping):
    Packet(PingType),
    ping(ping){
    }

    int16_t ping;

    int16_t getPing() const {
        return ping;
    }
};

class WorldPacket: public Packet {
public:
    WorldPacket(const PaintownUtil::ReferenceCount<World> & world):
    Packet(WorldType),
    world(world){
    }

    PaintownUtil::ReferenceCount<World> world;

    const PaintownUtil::ReferenceCount<World> & getWorld() const {
        return world;
    }
    
};

Input deserializeInput(const Token * token){
    Input out;

    /* FIXME: its more efficient to iterate over the elements in the token rather than explicitly
     * searching for each element by name
     */
    token->match("input/a", out.pressed.a);
    token->match("input/b", out.pressed.b);
    token->match("input/c", out.pressed.c);
    token->match("input/x", out.pressed.x);
    token->match("input/y", out.pressed.y);
    token->match("input/back", out.pressed.back);
    token->match("input/forward", out.pressed.forward);
    token->match("input/up", out.pressed.up);
    token->match("input/down", out.pressed.down);

    return out;
}

static std::string readLz4(const Network::Socket & socket){
    int16_t compressed = Network::read16(socket);
    int16_t uncompressed = Network::read16(socket);
    uint8_t * data = new uint8_t[compressed];
    Network::readBytes(socket, data, compressed);
    uint8_t * what = new uint8_t[uncompressed + 1];
    what[uncompressed] = '\0';
    LZ4_uncompress((const char *) data, (char *) what, uncompressed);
    std::string use((const char *) what);
    delete[] data;
    delete[] what;
    return use;
}

static PaintownUtil::ReferenceCount<Packet> readPacket(const Network::Socket & socket){
    int16_t magic = Network::read16(socket);
    if (magic != NetworkMagic){
        Global::debug(0) << "Error! Read invalid packet!" << std::endl;
        return PaintownUtil::ReferenceCount<Packet>(NULL);
    }
    int16_t type = Network::read16(socket);
    switch (type){
        case Packet::InputType: {
            uint32_t tick = Network::read32(socket);
            std::string use = readLz4(socket);
            Global::debug(1) << "Read raw input string " << use << std::endl;

            TokenReader reader;
            Token * head = reader.readTokenFromString(use);
            // Global::debug(0) << "Client received token " << head->toString() << std::endl;
            if (head != NULL){
                return PaintownUtil::ReferenceCount<Packet>(new InputPacket(deserializeInput(head), tick));
            }
            break;
        }
        case Packet::PingType: {
            int16_t clientPing = Network::read16(socket);
            return PaintownUtil::ReferenceCount<Packet>(new PingPacket((uint16_t) clientPing));
            break;
        }
        case Packet::WorldType: {
            TokenReader reader;
            std::string use = readLz4(socket);
            Token * head = reader.readTokenFromString(use);
            Global::debug(1) << "World: " << head->toString() << std::endl;
            if (head != NULL){
                return PaintownUtil::ReferenceCount<Packet>(new WorldPacket(PaintownUtil::ReferenceCount<World>(World::deserialize(head))));
            }
            break;
        }
        default: {
            std::ostringstream out;
            out << "Unknown packet type: " << type;
            throw MugenException(out.str(), __FILE__, __LINE__);
        }
    }
    return PaintownUtil::ReferenceCount<Packet>(NULL);
}

static void sendPacket(const Network::Socket & socket, const PaintownUtil::ReferenceCount<Packet> & packet){
    switch (packet->type){
        case Packet::InputType: {
            NetworkBuffer buffer;
            PaintownUtil::ReferenceCount<InputPacket> input = packet.convert<InputPacket>();
            buffer << NetworkMagic;
            buffer << (int16_t) Packet::InputType;
            buffer << input->tick;

            Token * data = new Token();
            *data << "input";

            *data->newToken() << "a" << input->inputs.pressed.a;
            *data->newToken() << "b" << input->inputs.pressed.b;
            *data->newToken() << "c" << input->inputs.pressed.c;
            *data->newToken() << "x" << input->inputs.pressed.x;
            *data->newToken() << "y" << input->inputs.pressed.y;
            *data->newToken() << "z" << input->inputs.pressed.z;
            *data->newToken() << "back" << input->inputs.pressed.back;
            *data->newToken() << "forward" << input->inputs.pressed.forward;
            *data->newToken() << "up" << input->inputs.pressed.up;
            *data->newToken() << "down" << input->inputs.pressed.down;

            buffer << data->toStringCompact();
            Global::debug(1) << "Sending input " << data->toStringCompact() << std::endl;
            delete data;

            // Global::debug(0) << "Send packet of " << buffer.getLength() << " bytes " << std::endl;
            buffer.send(socket);
            break;
        }
        case Packet::WorldType: {
            NetworkBuffer buffer;
            buffer << (int16_t) NetworkMagic;
            buffer << (int16_t) Packet::WorldType;

            PaintownUtil::ReferenceCount<WorldPacket> world = packet.convert<WorldPacket>();
            Token * test = world->getWorld()->serialize();
            Token * filtered = filterTokens(test);

            buffer << filtered->toStringCompact();
            Global::debug(1) << "World: " << filtered->toString() << std::endl;
            buffer.send(socket);

            /*
            // Global::debug(0) << "Snapshot: " << filtered->toString() << std::endl;
            string compact = filtered->toStringCompact();
            // Global::debug(0) << "Size: " << compact.size() << std::endl;
            char * out = new char[LZ4_compressBound(compact.size())];
            int compressed = LZ4_compress(compact.c_str(), out, compact.size());

            buffer << (int16_t) compressed;
            buffer << (int16_t) compact.size();
            buffer.add(out, compressed);
            buffer.send(socket);

            // Global::debug(0) << "Compressed size: " << compressed << std::endl;
            delete[] out;
            */

            delete test;
            delete filtered;

            break;
        }
        case Packet::PingType: {
            PaintownUtil::ReferenceCount<PingPacket> ping = packet.convert<PingPacket>();
            NetworkBuffer buffer;
            buffer << (int16_t) NetworkMagic;
            buffer << (int16_t) Packet::PingType;
            buffer << (int16_t) ping->getPing();
            buffer.send(socket);
            break;
        }
        default: {
            throw MugenException("Unknown packet type", __FILE__, __LINE__);
        }
    }
}

/* We don't have the latest commands so just assume if holdfwd/back/down was pressed
 * before that its still being pressed.
 */
class NetworkBehavior: public Behavior {
public:
    NetworkBehavior():
    lastTick(0){
    }

    uint32_t lastTick;
    Input lastInput;
    std::map<uint32_t, Input> history;

    virtual void setInput(uint32_t tick, const Input & input){
        if (tick > lastTick){
            lastTick = tick;
            lastInput = input;
        }
        history[tick] = input;
    }

    /* finds the most recent input starting from `last' and working backwards.
     * if there is only an input for last-1 then we predict that the input for `last'
     * will be the same.
     *
     * this could potentially take time proportional to `last' but most of the time
     * the found input will be within 100 or so ticks so we won't have to search
     * that far.
     */
    Input getInput(uint32_t last){
        while (last > 0){
            if (history.find(last) != history.end()){
                return history[last];
            }
            last -= 1;
        }

        return Input();
    }

    Input getInput(const Stage & stage){
        return getInput(stage.getTicks());
    }

    virtual std::vector<std::string> currentCommands(const Stage & stage, Character * owner, const std::vector<Command2*> & commands, bool reversed){
        vector<string> out;

        Input input = getInput(stage);

        for (vector<Command2*>::const_iterator it = commands.begin(); it != commands.end(); it++){
            Command2 * command = *it;
            if (command->handle(input, stage.getTicks())){
                Global::debug(1) << "command: " << command->getName() << std::endl;
                out.push_back(command->getName());
            }
        }

        return out;
    }

    /* called when the player changes direction. useful for updating
     * the input mapping.
     */
    virtual void flip(){
    }

    virtual ~NetworkBehavior(){
    }
};

/* Acts like human behavior except it captures history */
class HumanNetworkBehavior: public HumanBehavior {
public:
    HumanNetworkBehavior(const InputMap<Keys> & right, const InputMap<Keys> & left):
    HumanBehavior(right, left){
    }

    std::map<uint32_t, Input> history;

    virtual void setInput(uint32_t tick, const Input & input){
        history[tick] = input;
    }

    using HumanBehavior::getInput;
    Input getInput(const Stage & stage, bool reversed){
        if (history.find(stage.getTicks()) != history.end()){
            return history[stage.getTicks()];
        }

        input = updateInput(getInput(reversed), input);
        return input;
    }
    
    virtual std::vector<std::string> currentCommands(const Stage & stage, Character * owner, const std::vector<Command2*> & commands, bool reversed){
        vector<string> out;

        Input use = getInput(stage, reversed);
        history[stage.getTicks()] = use;

        for (vector<Command2*>::const_iterator it = commands.begin(); it != commands.end(); it++){
            Command2 * command = *it;
            if (command->handle(use, stage.getTicks())){
                Global::debug(1) << "command: " << command->getName() << std::endl;
                out.push_back(command->getName());
            }
        }

        return out;
    }

    /* called when the player changes direction. useful for updating
     * the input mapping.
     */
    virtual void flip(){
        bool forward = input.pressed.forward;
        input.pressed.forward = input.pressed.back;
        input.pressed.back = forward;
    }

    virtual ~HumanNetworkBehavior(){
    }
};

class HostHandler{
public:
    HostHandler(){
    }

    virtual void handlePing(const PaintownUtil::ReferenceCount<PingPacket> & packet) = 0;
    virtual void handleInput(const PaintownUtil::ReferenceCount<InputPacket> & packet) = 0;
    virtual void handleWorld(const PaintownUtil::ReferenceCount<WorldPacket> & packet) = 0;

    virtual ~HostHandler(){
    }
};

class PacketHandler{
public:
    PacketHandler(const Network::Socket & socket, HostHandler & host):
    socket(socket),
    host(host),
    sendThread(this, send),
    receiveThread(this, receive),
    alive_(true){
    }

    Network::Socket socket;
    HostHandler & host;
    
    PaintownUtil::Thread::LockObject lock;
    
    vector<PaintownUtil::ReferenceCount<Packet> > outBox;
    vector<PaintownUtil::ReferenceCount<Packet> > inBox;

    PaintownUtil::Thread::ThreadObject sendThread;
    PaintownUtil::Thread::ThreadObject receiveThread;
    
    bool alive_;

    bool alive(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        return alive_;
    }

    void kill(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        alive_ = false;
    }

    void sendPacket(const PaintownUtil::ReferenceCount<Packet> & packet){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        outBox.push_back(packet);
    }

    PaintownUtil::ReferenceCount<Packet> getSendPacket(){
        PaintownUtil::ReferenceCount<Packet> out;

        {
            PaintownUtil::Thread::ScopedLock scoped(lock);
            if (outBox.size() > 0){
                out = outBox.front();
                outBox.erase(outBox.begin());
            }
        }

        return out;
    }

    void doSend(){
        while (alive()){
            PaintownUtil::ReferenceCount<Packet> nextPacket = getSendPacket();
            if (nextPacket != NULL){
                Global::debug(1) << "Send packet type " << nextPacket->getType() << " at " << System::currentMilliseconds() << std::endl;
                Mugen::sendPacket(socket, nextPacket);
            } else {
                PaintownUtil::rest(1);
            }
        }
    }

    static void * send(void * self_){
        PacketHandler * self = (PacketHandler*) self_;
        self->doSend();
        return NULL;
    }

    void handlePacket(const PaintownUtil::ReferenceCount<Packet> & packet){
        Global::debug(1) << "Received packet type " << packet->getType() << " at " << System::currentMilliseconds() << std::endl;
        switch (packet->getType()){
            case Packet::PingType: {
                host.handlePing(packet.convert<PingPacket>());
                break;
            }
            case Packet::InputType: {
                host.handleInput(packet.convert<InputPacket>());
                break;
            }
            case Packet::WorldType: {
                host.handleWorld(packet.convert<WorldPacket>());
                break;
            }
        }
    }

    void doReceive(){
        while (alive()){
            PaintownUtil::ReferenceCount<Packet> nextPacket = readPacket(socket);
            if (nextPacket != NULL){
                handlePacket(nextPacket);
            }
        }
    }

    static void * receive(void * self_){
        PacketHandler * self = (PacketHandler*) self_;
        self->doReceive();
        return NULL;
    }
    
    virtual void start(){
        sendThread.start();
        receiveThread.start();
    }
};

class NetworkServerObserver: public NetworkObserver, public HostHandler {
public:
    NetworkServerObserver(Network::Socket reliable, const PaintownUtil::ReferenceCount<Character> & player1, const PaintownUtil::ReferenceCount<Character> & player2, HumanBehavior & player1Behavior, NetworkBehavior & player2Behavior):
    NetworkObserver(),
    handler(reliable, *this),
    reliable(reliable),
    player1(player1),
    player2(player2),
    player1Behavior(player1Behavior),
    player2Behavior(player2Behavior),
    count(0),
    lastPing(System::currentMilliseconds()),
    ping(0){
    }

    PacketHandler handler;

    PaintownUtil::Thread::LockObject lock;
    Network::Socket reliable;
    PaintownUtil::ReferenceCount<Character> player1;
    PaintownUtil::ReferenceCount<Character> player2;
    HumanBehavior & player1Behavior;
    NetworkBehavior & player2Behavior;

    Input lastInput;
    
    /* mapping from logical ping to the time in milliseconds when the ping was sent */
    std::map<int, uint64_t> pings;

    uint32_t count;
    uint64_t lastPing;
    uint16_t ping;

    void kill(){
        handler.kill();
    }

    uint16_t nextPing(){
        uint16_t out = ping;
        ping += 1;
        return out;
    }

    virtual void handlePing(const PaintownUtil::ReferenceCount<PingPacket> & ping){
        int16_t client = ping->getPing();
        if (pings.find(client) != pings.end()){
            Global::debug(1) << "Client ping: " << (System::currentMilliseconds() - pings[client]) << std::endl;
            pings.erase(client);
        }
    }

    virtual void handleInput(const PaintownUtil::ReferenceCount<InputPacket> & input){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        Global::debug(1) << "Input back: " << input->inputs.pressed.back <<
                                 " forward: " << input->inputs.pressed.forward <<
                                 " up: " << input->inputs.pressed.up <<
                                 " down: " << input->inputs.pressed.down <<
                                 std::endl;
        inputs[input->tick] = input->inputs;
    }

    virtual void handleWorld(const PaintownUtil::ReferenceCount<WorldPacket> & world){
        Global::debug(0) << "Should not have gotten a world packet from the client" << std::endl;
    }
    
    virtual void start(){
        handler.start();
    }

    PaintownUtil::ReferenceCount<World> lastState;
    std::map<uint32_t, Input> inputs;

    std::map<uint32_t, Input> getInputs(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        std::map<uint32_t, Input> out = inputs;
        inputs.clear();
        return out;
    }
    
    virtual void beforeLogic(Stage & stage){

        if (lastState == NULL){
            lastState = stage.snapshotState();
        }

        count += 1;

        uint32_t currentTicks = stage.getTicks();

        std::map<uint32_t, Input> useInputs = getInputs();
        if (useInputs.size() > 0){
            for (std::map<uint32_t, Input>::iterator it = useInputs.begin(); it != useInputs.end(); it++){
                uint32_t tick = it->first;
                const Input & input = it->second;
                // player2->setInputs(tick, input.inputs);
                player2Behavior.setInput(tick, input);
            }

            if (lastState != NULL && currentTicks > lastState->getStageData().ticker){
                stage.updateState(*lastState);
                Mugen::Sound::disableSounds();
                for (uint32_t i = 0; i < currentTicks - lastState->getStageData().ticker; i++){
                    stage.logic();
                }
                Mugen::Sound::enableSounds();
            }
        }

        if (System::currentMilliseconds() - lastPing > 1000){
            lastPing = System::currentMilliseconds();
            uint16_t ping = nextPing();
            pings[ping] = lastPing;
            handler.sendPacket(PaintownUtil::ReferenceCount<Packet>(new PingPacket(ping)));
        }
    }

    virtual void afterLogic(Stage & stage){
        Input latest = player1Behavior.getInput();
        if (latest != lastInput){
            lastInput = latest;
            /*
            Global::debug(0) << "Tick " << stage.getTicks() << std::endl;
            for (vector<string>::iterator it = inputs.begin(); it != inputs.end(); it++){
                Global::debug(0) << "Input: " << *it << std::endl;
            }
            */
            handler.sendPacket(PaintownUtil::ReferenceCount<Packet>(new InputPacket(latest, stage.getTicks())));
        }

        if (count % 30 == 0){
            lastState = stage.snapshotState();
            handler.sendPacket(PaintownUtil::ReferenceCount<Packet>(new WorldPacket(lastState)));
        }
    }
};

static void debugWorld(const std::string & what, Stage & stage){
    PaintownUtil::ReferenceCount<World> world = stage.snapshotState();
    Token * test = world->serialize();
    Token * filtered = filterTokens(test);

    Global::debug(0) << stage.getTicks() << " " << what << ": " << filtered->toString() << std::endl;
    delete test;
    delete filtered;
}

class NetworkClientObserver: public NetworkObserver, public HostHandler {
public:
    NetworkClientObserver(Network::Socket socket, const PaintownUtil::ReferenceCount<Character> & player1, const PaintownUtil::ReferenceCount<Character> & player2, HumanBehavior & player1Behavior, NetworkBehavior & player2Behavior):
    NetworkObserver(),
    handler(socket, *this),
    socket(socket),
    player1(player1),
    player2(player2),
    player1Behavior(player1Behavior),
    player2Behavior(player2Behavior),
    alive_(true){
    }

    PacketHandler handler;

    Network::Socket socket;
    PaintownUtil::ReferenceCount<Character> player1;
    PaintownUtil::ReferenceCount<Character> player2;
    HumanBehavior & player1Behavior;
    NetworkBehavior & player2Behavior;
    PaintownUtil::ReferenceCount<World> world;
    PaintownUtil::Thread::LockObject lock;
    bool alive_;
    std::map<uint32_t, Input> inputs;
    Input lastInput;

    void setWorld(const PaintownUtil::ReferenceCount<World> & world){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        this->world = world;
    }

    PaintownUtil::ReferenceCount<World> getWorld(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        PaintownUtil::ReferenceCount<World> out = world;
        world = NULL;
        return out;
    }

    bool alive(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        return alive_;
    }

    virtual void kill(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        alive_ = false;
    }

    virtual void handlePing(const PaintownUtil::ReferenceCount<PingPacket> & packet){
        handler.sendPacket(packet.convert<Packet>());
    }
    
    virtual void handleInput(const PaintownUtil::ReferenceCount<InputPacket> & packet){
        addInput(packet->tick, packet->inputs);
    }

    virtual void handleWorld(const PaintownUtil::ReferenceCount<WorldPacket> & packet){
        setWorld(packet->getWorld());
    }
    
    virtual void start(){
        handler.start();
    }

    void addInput(uint32_t tick, const Input & input){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        inputs[tick] = input;
    }

    std::map<uint32_t, Input> getInputs(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        std::map<uint32_t, Input> out = inputs;
        inputs.clear();
        return out;
    }

    PaintownUtil::ReferenceCount<World> lastState;
    
    virtual void beforeLogic(Stage & stage){
        uint32_t currentTicks = stage.getTicks();
        PaintownUtil::ReferenceCount<World> next = getWorld();
        bool replay = false;
        if (next != NULL){
            replay = true;
            stage.updateState(*next);
            lastState = next;
        }

        std::map<uint32_t, Input> useInputs = getInputs();
        if (useInputs.size() > 0){
            replay = true;
            for (std::map<uint32_t, Input>::iterator it = useInputs.begin(); it != useInputs.end(); it++){
                uint32_t tick = it->first;
                const Input & input = it->second;
                player2Behavior.setInput(tick, input);
                /*
                player2->setInputs(tick, input.inputs);
                */
            }
        }

        if (replay){
            if (lastState != NULL && currentTicks > lastState->getStageData().ticker){
                stage.updateState(*lastState);
                Mugen::Sound::disableSounds();
                Global::debug(1) << "At " << currentTicks << " Client replaying " << (currentTicks - lastState->getStageData().ticker) << std::endl;
                for (uint32_t i = 0; i < currentTicks - lastState->getStageData().ticker; i++){
                    stage.logic();
                }
                Mugen::Sound::enableSounds();
            }
        }
    }

    virtual void afterLogic(Stage & stage){
        Input latest = player1Behavior.getInput();
        if (latest != lastInput){
            lastInput = latest;
            /*
            Global::debug(0) << "Tick " << stage.getTicks() << std::endl;
            for (vector<string>::iterator it = inputs.begin(); it != inputs.end(); it++){
                Global::debug(0) << "Input: " << *it << std::endl;
            }
            */
            handler.sendPacket(PaintownUtil::ReferenceCount<Packet>(new InputPacket(latest, stage.getTicks())));
        }
    }
};

void Game::startNetworkVersus1(const PaintownUtil::ReferenceCount<Character> & player1,
                               const PaintownUtil::ReferenceCount<Character> & player2,
                               Stage & stage,
                               bool server, const std::string & host, int port){

    try{
        Network::reuseSockets(true);

        Network::Socket socket = 0;
        if (server){
            Network::Socket remote = Network::openReliable(port);
            Network::listen(remote);
            Global::debug(0) << "Waiting for a connection on port " << port << std::endl;
            socket = Network::accept(remote);
            Network::close(remote);
            Global::debug(0) << "Got a connection" << std::endl;
        } else {
            int maxTries = 5;
            int tries = 0;
            for (tries = 0; tries < maxTries; tries++){
                Global::debug(0) << "Connecting to " << host << " on port " << port << ". Attempt " << (tries + 1) << "/" << maxTries << std::endl;
                try{
                    socket = Network::connectReliable(host, port); 
                    Global::debug(0) << "Connected" << std::endl;
                    break;
                } catch (const Network::NetworkException & fail){
                    Global::debug(0) << "Failed to connect: " << fail.getMessage() << std::endl;
                    PaintownUtil::rest(1000);
                }
            }
            if (tries == maxTries){
                throw MugenException("Could not connect", __FILE__, __LINE__);
            }
        }

        HumanNetworkBehavior player1Behavior(getPlayer1Keys(), getPlayer1InputLeft());
        NetworkBehavior player2Behavior;
        // DummyBehavior player2Behavior;

        // NetworkLocalBehavior player1Behavior(&local1Behavior, socket);
        // NetworkRemoteBehavior player2Behavior(socket);

        // Set regenerative health
        player1->setRegeneration(false);
        player2->setRegeneration(false);
        PaintownUtil::ReferenceCount<NetworkObserver> observer;
        if (server){
            player1->setBehavior(&player1Behavior);
            player2->setBehavior(&player2Behavior);
            observer = PaintownUtil::ReferenceCount<NetworkObserver>(new NetworkServerObserver(socket, player1, player2, player1Behavior, player2Behavior));
            stage.setObserver(observer.convert<StageObserver>());
        } else {
            player2->setBehavior(&player1Behavior);
            player1->setBehavior(&player2Behavior);
            observer = PaintownUtil::ReferenceCount<NetworkObserver>(new NetworkClientObserver(socket, player2, player1, player1Behavior, player2Behavior));
            stage.setObserver(observer.convert<StageObserver>());
        }

        RunMatchOptions options;

        options.setBehavior(&player1Behavior, NULL);

        /* server is player1 */
        if (server){
            stage.addPlayer1(player1.raw());
            stage.addPlayer2(player2.raw());
        } else {
            stage.addPlayer1(player1.raw());
            stage.addPlayer2(player2.raw());
        }

        stage.reset();
        int time = Mugen::Data::getInstance().getTime();
        Mugen::Data::getInstance().setTime(-1);

        /* Synchronize client and server at this point */
        if (server){
            int sync = Network::read16(socket);
            Network::send16(socket, sync);
        } else {
            Network::send16(socket, 0);
            Network::read16(socket);
        }

        observer->start();

        /*
           if (!Network::blocking(socket, false)){
           Global::debug(0) << "Could not set socket to be non-blocking" << std::endl;
           }
           */

        /*
           player1Behavior.begin();
           player2Behavior.begin();
           */

        /*
           if (!Network::noDelay(socket, true)){
           Global::debug(0) << "Could not set no delay!" << std::endl;
           }
           */

        try{
            runMatch(&stage, "", options);
        } catch (const QuitGameException & ex){
        } catch (const MugenException & ex){
            Global::debug(0) << ex.getTrace() << std::endl;
        } catch (const Exception::Base & ex){
            Global::debug(0) << ex.getTrace() << std::endl;
        }
        Mugen::Data::getInstance().setTime(time);

        observer->kill();

        Network::close(socket);

    } catch (const Network::NetworkException & fail){
        Global::debug(0) << "Network exception: " << fail.getMessage() << std::endl;
    }
}

}

#endif
