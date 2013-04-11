#ifdef HAVE_NETWORKING

#include "network.h"
#include "behavior.h"
#include "util/system.h"
#include "world.h"
#include "character.h"

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
        if (length + sizeof(data) < contains){
            char * where = buffer + length;
            char * out = Network::parse16(where, (uint16_t*) &data);
            length += out - where;
        } else {
            data = -1;
        }
        return *this;
    }

    NetworkBuffer & operator>>(uint32_t & data){
        if (length + sizeof(data) < contains){
            char * where = buffer + length;
            char * out = Network::parse32(where, &data);
            length += out - where;
        } else {
            data = -1;
        }
        return *this;
    }

    NetworkBuffer & operator<<(const string & str){
        *this << (int16_t) str.size();
        add(str.c_str(), str.size());
        return *this;
    }

    NetworkBuffer & operator>>(string & str){
        int16_t size = 0;
        *this >> size;
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
    void sendAll(const Network::Socket & socket){
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

enum MessageType{
    WorldState = 0,
    Ping
};

class Packet{
public:
    enum Type{
        Input
    };

    Packet(Type type):
    type(type){
    }

    Type type;
};

class InputPacket: public Packet {
public:
    InputPacket(const vector<string> & inputs, uint32_t tick):
    Packet(Input),
    inputs(inputs),
    tick(tick){
    }

    InputPacket():
    Packet(Input),
    tick(0){
    }

    vector<string> inputs;
    uint32_t tick;
};

static PaintownUtil::ReferenceCount<Packet> readPacket(NetworkBuffer & buffer){
    int16_t type;
    buffer >> type;
    switch (type){
        case Packet::Input: {
            uint32_t ticks = 0;
            int16_t inputCount = 0;
            buffer >> ticks;
            buffer >> inputCount;
            // Global::debug(0) << "Tick " << ticks << " inputs " << inputCount << std::endl;
            vector<string> inputs;
            for (int i = 0; i < inputCount; i++){
                string input;
                buffer >> input;
                inputs.push_back(input);
            }
            return PaintownUtil::ReferenceCount<Packet>(new InputPacket(inputs, ticks));
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
        case Packet::Input: {
            NetworkBuffer buffer;
            PaintownUtil::ReferenceCount<InputPacket> input = packet.convert<InputPacket>();
            buffer << NetworkMagic;
            buffer << (int16_t) Packet::Input;
            buffer << input->tick;
            buffer << (int16_t) input->inputs.size();
            for (vector<string>::iterator it = input->inputs.begin(); it != input->inputs.end(); it++){
                buffer << *it;
            }
            // Global::debug(0) << "Send packet of " << buffer.getLength() << " bytes " << std::endl;
            buffer.sendAll(socket);
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
    vector<string> lastCommands;

    virtual void setCommands(uint32_t tick, const vector<string> & commands){
        if (tick > lastTick){
            lastTick = tick;
            lastCommands = commands;
        }
    }

    virtual std::vector<std::string> currentCommands(const Stage & stage, Character * owner, const std::vector<Command*> & commands, bool reversed){
        vector<string> out;
        // if (stage.getTicks() > lastTick && stage.getTicks() - lastTick < 3){
            for (vector<string>::iterator it = lastCommands.begin(); it != lastCommands.end(); it++){
                if (*it == "holdfwd" ||
                    *it == "holdback" ||
                    *it == "holddown"){
                    out.push_back(*it);
                }
            }
        // }
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

class NetworkServerObserver: public NetworkObserver {
public:
    NetworkServerObserver(Network::Socket reliable, Network::Socket unreliable, const PaintownUtil::ReferenceCount<Character> & player1, const PaintownUtil::ReferenceCount<Character> & player2, NetworkBehavior & player2Behavior):
    NetworkObserver(),
    reliable(reliable),
    unreliable(unreliable),
    player1(player1),
    player2(player2),
    player2Behavior(player2Behavior),
    thread(this, send),
    pingThread(this, readPings),
    clientThread(this, clientInput),
    alive_(true),
    count(0),
    lastPing(System::currentMilliseconds()),
    ping(0){
    }

    PaintownUtil::Thread::LockObject lock;
    Network::Socket reliable;
    Network::Socket unreliable;
    PaintownUtil::ReferenceCount<Character> player1;
    PaintownUtil::ReferenceCount<Character> player2;
    NetworkBehavior & player2Behavior;
    PaintownUtil::Thread::ThreadObject thread;
    PaintownUtil::Thread::ThreadObject pingThread;
    PaintownUtil::Thread::ThreadObject clientThread;
    vector<PaintownUtil::ReferenceCount<World> > states;
    
    /* mapping from logical ping to the time in milliseconds when the ping was sent */
    std::map<int, uint64_t> pings;

    bool alive_;
    uint32_t count;
    uint64_t lastPing;
    uint16_t ping;

    void sendState(char * data, int compressed, int uncompressed){
        // Global::debug(0, "server") << "Send " << compressed << " bytes" << std::endl;
        NetworkBuffer buffer;
        buffer << (int16_t) NetworkMagic;
        buffer << (int16_t) WorldState;
        buffer << (int16_t) compressed;
        buffer << (int16_t) uncompressed;
        buffer.add(data, compressed);
        buffer.send(reliable);
    }

    bool hasStates(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        return states.size() > 0;
    }

    PaintownUtil::ReferenceCount<World> getState(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        PaintownUtil::ReferenceCount<World> out = states.front();
        states.erase(states.begin());
        return out;
    }

    bool alive(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        return alive_;
    }

    void kill(){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        alive_ = false;
        Network::close(unreliable);
    }

    void sendWorldState(){
        PaintownUtil::ReferenceCount<World> snapshot = getState();

        Token * test = snapshot->serialize();
        Token * filtered = filterTokens(test);
        // Global::debug(0) << "Snapshot: " << filtered->toString() << std::endl;
        string compact = filtered->toStringCompact();
        // Global::debug(0) << "Size: " << compact.size() << std::endl;
        char * out = new char[LZ4_compressBound(compact.size())];
        int compressed = LZ4_compress(compact.c_str(), out, compact.size());

        sendState(out, compressed, compact.size());

        // Global::debug(0) << "Compressed size: " << compressed << std::endl;
        delete[] out;

        delete test;
        delete filtered;
    }

    uint16_t nextPing(){
        uint16_t out = ping;
        ping += 1;
        return out;
    }

    void sendPing(uint16_t ping){
        NetworkBuffer buffer;
        buffer << (int16_t) NetworkMagic;
        buffer << (int16_t) Ping;
        buffer << (int16_t) ping;
        buffer.send(reliable);
    }

    void doHandlePings(){
        while (alive()){
            int16_t magic = Network::read16(reliable);
            if (magic != NetworkMagic){
                Global::debug(0) << "Garbage ping packet" << std::endl;
                continue;
            }
            int16_t type = Network::read16(reliable);
            if (type != Ping){
                Global::debug(0) << "Invalid ping type: " << type << std::endl;
            }
            uint16_t clientPing = (uint16_t) Network::read16(reliable);
            if (pings.find(clientPing) != pings.end()){
                Global::debug(0) << "Client ping: " << (System::currentMilliseconds() - pings[clientPing]) << std::endl;
                pings.erase(clientPing);
            }
        }
    }

    void doSend(){
        while (alive()){
            if (hasStates()){
                sendWorldState();
            } else {
                if (System::currentMilliseconds() - lastPing > 1000){
                    lastPing = System::currentMilliseconds();
                    uint16_t ping = nextPing();
                    pings[ping] = lastPing;
                    sendPing(ping);
                }
                PaintownUtil::rest(1);
            }
        }
    }

    void addState(const PaintownUtil::ReferenceCount<World> & state){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        states.clear();
        states.push_back(state);
    }

    static void * send(void * self_){
        NetworkServerObserver * self = (NetworkServerObserver*) self_;
        self->doSend();
        return NULL;
    }

    static void * readPings(void * self_){
        NetworkServerObserver * self = (NetworkServerObserver*) self_;
        self->doHandlePings();
        return NULL;
    }

    void doClientInput(){
        try{
            while (alive()){
                NetworkBuffer buffer(1024);
                buffer.readAll(unreliable);
                int16_t magic = 0;
                buffer >> magic;
                if (magic != NetworkMagic){
                    Global::debug(0) << "Garbage input packet: " << magic << std::endl;
                    continue;
                }

                PaintownUtil::ReferenceCount<Packet> packet = readPacket(buffer);
                if (packet != NULL){
                    switch (packet->type){
                        case Packet::Input: {
                            PaintownUtil::ReferenceCount<InputPacket> input = packet.convert<InputPacket>();
                            addInput(*input);
                            /*
                            Global::debug(0) << "Received inputs " << input->inputs.size() << std::endl;
                            for (vector<string>::iterator it = input->inputs.begin(); it != input->inputs.end(); it++){
                                Global::debug(0) << " '" << *it << "'" << std::endl;
                            }
                            */
                            break;
                        }
                    }
                }
            }
        } catch (const Exception::Base & ex){
            Global::debug(0) << "Error in client read input thread. " << ex.getTrace() << std::endl;
        }
    }

    static void * clientInput(void * self_){
        NetworkServerObserver * self = (NetworkServerObserver*) self_;
        self->doClientInput();
        return NULL;
    }
    
    virtual void start(){
        thread.start();
        pingThread.start();
        clientThread.start();
    }

    PaintownUtil::ReferenceCount<World> lastState;
    std::map<uint32_t, InputPacket> inputs;

    void addInput(const InputPacket & input){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        inputs[input.tick] = input;
    }
    
    virtual void beforeLogic(Stage & stage){
        if (count % 30 == 0){
            lastState = stage.snapshotState();
            addState(lastState);
        }
        count += 1;

        uint32_t currentTicks = stage.getTicks();

        PaintownUtil::Thread::ScopedLock scoped(lock);
        if (inputs.size() > 0){
            for (std::map<uint32_t, InputPacket>::iterator it = inputs.begin(); it != inputs.end(); it++){
                uint32_t tick = it->first;
                const InputPacket & input = it->second;
                player2->setInputs(tick, input.inputs);
                player2Behavior.setCommands(tick, input.inputs);
            }
            inputs.clear();

            if (lastState != NULL && currentTicks > lastState->getStageData().ticker){
                stage.updateState(*lastState);
                Mugen::Sound::disableSounds();
                for (int i = 0; i < currentTicks - lastState->getStageData().ticker; i++){
                    stage.logic();
                }
                Mugen::Sound::enableSounds();
            }
        }

    }

    virtual void afterLogic(Stage & stage){
        vector<string> inputs = player1->currentInputs();
        if (inputs.size() >= 0){
            /*
            Global::debug(0) << "Tick " << stage.getTicks() << std::endl;
            for (vector<string>::iterator it = inputs.begin(); it != inputs.end(); it++){
                Global::debug(0) << "Input: " << *it << std::endl;
            }
            */
            PaintownUtil::ReferenceCount<InputPacket> packet(new InputPacket(inputs, stage.getTicks()));
            sendPacket(unreliable, packet.convert<Packet>());
        }
    }
};

class NetworkClientObserver: public NetworkObserver {
public:
    NetworkClientObserver(Network::Socket socket, Network::Socket unreliable, const PaintownUtil::ReferenceCount<Character> & player1, const PaintownUtil::ReferenceCount<Character> & player2, NetworkBehavior & player2Behavior):
    NetworkObserver(),
    socket(socket),
    unreliable(unreliable),
    player1(player1),
    player2(player2),
    player2Behavior(player2Behavior),
    thread(this, receive),
    input(this, input_),
    alive_(true){
    }

    Network::Socket socket;
    Network::Socket unreliable;
    PaintownUtil::ReferenceCount<Character> player1;
    PaintownUtil::ReferenceCount<Character> player2;
    NetworkBehavior & player2Behavior;
    PaintownUtil::Thread::ThreadObject thread;
    PaintownUtil::Thread::ThreadObject input;
    PaintownUtil::ReferenceCount<World> world;
    PaintownUtil::Thread::LockObject lock;
    bool alive_;
    std::map<uint32_t, InputPacket> inputs;

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
        Network::close(unreliable);
    }

    void sendPing(int16_t ping, Network::Socket socket){
        NetworkBuffer buffer;
        buffer << (int16_t) NetworkMagic;
        buffer << (int16_t) Ping;
        buffer << (int16_t) ping;
        buffer.send(socket);
    }

    void doReceive(){
        while (alive()){

            int16_t magic = Network::read16(socket);
            if (magic != NetworkMagic){
                Global::debug(0) << "Garbage message packet: " << magic << std::endl;
                continue;
            }

            int16_t type = Network::read16(socket);

            switch (type){
                case WorldState: {
                    int16_t compressed = Network::read16(socket);
                    int16_t uncompressed = Network::read16(socket);
                    uint8_t * data = new uint8_t[compressed];
                    Network::readBytes(socket, data, compressed);
                    uint8_t * what = new uint8_t[uncompressed + 1];
                    what[uncompressed] = '\0';
                    LZ4_uncompress((const char *) data, (char *) what, uncompressed);
                    TokenReader reader;
                    std::string use((const char *) what);
                    Token * head = reader.readTokenFromString(use);
                    // Global::debug(0) << "Client received token " << head->toString() << std::endl;
                    if (head != NULL){
                        PaintownUtil::ReferenceCount<World> world(World::deserialize(head));
                        setWorld(world);
                    }
                    break;
                }
                case Ping: {
                    int16_t ping = Network::read16(socket);
                    sendPing(ping, socket);
                    break;
                }
            }
        }
    }

    static void * receive(void * self){
        ((NetworkClientObserver*)self)->doReceive();
        return NULL;
    }
    
    virtual void start(){
        thread.start();
        input.start();
    }

    static void * input_(void * self){
        ((NetworkClientObserver*)self)->doClientInput();
        return NULL;
    }

    void addInput(const InputPacket & input){
        PaintownUtil::Thread::ScopedLock scoped(lock);
        inputs[input.tick] = input;
    }

    void doClientInput(){
        try{
            while (alive()){
                NetworkBuffer buffer(1024);
                buffer.readAll(unreliable);
                int16_t magic = 0;
                buffer >> magic;
                if (magic != NetworkMagic){
                    Global::debug(0) << "Garbage udp packet: " << magic << std::endl;
                    continue;
                }

                PaintownUtil::ReferenceCount<Packet> packet = readPacket(buffer);
                if (packet != NULL){
                    switch (packet->type){
                        case Packet::Input: {
                            PaintownUtil::ReferenceCount<InputPacket> input = packet.convert<InputPacket>();
                            addInput(*input);
                            /*
                            Global::debug(0) << "Received inputs " << input->inputs.size() << std::endl;
                            for (vector<string>::iterator it = input->inputs.begin(); it != input->inputs.end(); it++){
                                Global::debug(0) << " '" << *it << "'" << std::endl;
                            }
                            */
                            break;
                        }
                    }
                }
            }
        } catch (const Exception::Base & ex){
            Global::debug(0) << "Error in client read input thread. " << ex.getTrace() << std::endl;
        }
    }

    PaintownUtil::ReferenceCount<World> lastState;
    
    virtual void beforeLogic(Stage & stage){
        uint32_t currentTicks = stage.getTicks();
        PaintownUtil::ReferenceCount<World> next = getWorld();
        if (next != NULL){
            stage.updateState(*next);
            lastState = next;
        }

        PaintownUtil::Thread::ScopedLock scoped(lock);
        if (inputs.size() > 0){
            for (std::map<uint32_t, InputPacket>::iterator it = inputs.begin(); it != inputs.end(); it++){
                uint32_t tick = it->first;
                const InputPacket & input = it->second;
                player2->setInputs(tick, input.inputs);
                player2Behavior.setCommands(tick, input.inputs);
            }
            inputs.clear();

            if (lastState != NULL && currentTicks > lastState->getStageData().ticker){
                stage.updateState(*lastState);
                Mugen::Sound::disableSounds();
                for (int i = 0; i < currentTicks - lastState->getStageData().ticker; i++){
                    stage.logic();
                }
                Mugen::Sound::enableSounds();
            }
        }
    }

    virtual void afterLogic(Stage & stage){
        vector<string> inputs = player1->currentInputs();
        if (inputs.size() >= 0){
            /*
            Global::debug(0) << "Tick " << stage.getTicks() << std::endl;
            for (vector<string>::iterator it = inputs.begin(); it != inputs.end(); it++){
                Global::debug(0) << "Input: " << *it << std::endl;
            }
            */
            PaintownUtil::ReferenceCount<InputPacket> packet(new InputPacket(inputs, stage.getTicks()));
            sendPacket(unreliable, packet.convert<Packet>());
        }
    }
};

}

#endif
