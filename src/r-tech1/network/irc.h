#ifndef _util_network_irc_h
#define _util_network_irc_h

#include "network.h"
#include "chat.h"
#include "r-tech1/pointer.h"
#include "r-tech1/thread.h"

#include "r-tech1/gui/tab-container.h"
#include "r-tech1/gui/lineedit.h"
#include "r-tech1/gui/list.h"

#include <string>
#include <vector>
#include <queue>
#include <map>

namespace Network{
namespace IRC{
    
    class Command{
    public:
        enum Type{
            Unknown,
            Pass,
            Nick,
            User,
            Userhost,
            Server,
            Oper,
            Quit,
            Squit,
            Join,
            Part,
            Mode,
            Topic,
            Names,
            List,
            Invite,
            Kick,
            Version,
            Stats,
            Links,
            Time,
            Connect,
            Trace,
            Admin,
            Info,
            PrivateMessage,
            Notice,
            Who,
            Whois,
            Whowas,
            Kill,
            Ping,
            Pong,
            Error,
            ErrorNickInUse,
            ErrorNoSuchNick,
            ErrorNoSuchChannel,
            ErrorNeedMoreParams,
            ErrorBannedFromChannel,
            ErrorInviteOnlyChannel,
            ErrorBadChannelKey,
            ErrorChannelIsFull,
            ReplyNames,
            ReplyNamesEndOf,
            ReplyNoTopic,
            ReplyTopic,
            ReplyTopicAuthor,
            ReplyMOTD,
            ReplyMOTDStart,
            ReplyMOTDEndOf,
            ReplyUserhost,
        };
        // Initializes it from an incoming message off of socket
        Command(const std::string &);
        // Create a message with owner and type
        Command(const std::string &, const Type &);
        Command(const Command &);
        virtual ~Command();
        
        virtual const Command & operator=(const Command &);
        
        virtual std::string getSendable() const;
        virtual std::string getCTCPSendable() const;
        
        virtual inline const std::string & getOwner() const {
            return this->owner;
        }
        
        virtual inline const Type & getType() const {
            return this->type;
        }
        
        virtual inline void setParameters(const std::string & param1){
            this->parameters.clear();
            this->parameters.push_back(param1);
        }
        
        virtual inline void setParameters(const std::string & param1, const std::string & param2){
            this->parameters.clear();
            this->parameters.push_back(param1);
            this->parameters.push_back(param2);
        }
        
        virtual inline void setParameters(const std::string & param1, const std::string & param2, const std::string & param3){
            this->parameters.clear();
            this->parameters.push_back(param1);
            this->parameters.push_back(param2);
            this->parameters.push_back(param3);
        }
        
        virtual inline void setParameters(const std::string & param1, const std::string & param2, const std::string & param3, const std::string & param4){
            this->parameters.clear();
            this->parameters.push_back(param1);
            this->parameters.push_back(param2);
            this->parameters.push_back(param3);
            this->parameters.push_back(param4);
        }
        
        virtual inline void setParameters(const std::vector< std::string > & params){
            this->parameters = params;
        }
        
        virtual inline const std::vector< std::string > & getParameters() const {
            return this->parameters;
        }
        
        virtual inline bool hasCtcp() const {
            return !this->ctcp.empty();
        }
        
        virtual inline const std::vector< std::string > & getCtcp() const {
            return this->ctcp;
        }
        
    protected:
        std::string owner;
        Type type;
        std::vector< std::string > parameters;
        std::vector< std::string > ctcp;
    };
    
    class Channel{
    public:
        Channel();
        Channel(const std::string &);
        Channel(const Channel &);
        ~Channel();
        
        const Channel & operator=(const Channel &);
        
        void addUser(const std::string &);
        void removeUser(const std::string &);
        void replaceUser(const std::string &, const std::string &);
        void addUsers(const std::vector<std::string> &);
        
        inline const std::vector<std::string> & getUsers() const{
            return this->users;
        }
        
        inline const std::string & getName() const {
            return this->name;
        }
        
        inline void setTopic(const std::string & topic){
            this->topic = topic;
        }
        
        inline void setTopicAuthor(const std::string & topicAuthor, uint64_t topicDate){
            this->topicAuthor = topicAuthor;
            this->topicDate = topicDate;
        }
        
    protected:
        std::string name;
        std::string topic;
        std::string topicAuthor;
        uint64_t topicDate;
        std::vector<std::string> users;
    };
    
    // Channel ReferenceCount 
    typedef Util::ReferenceCount<Channel> ChannelPointer;

    class Client : public Chat::Threadable{
    public:
        Client(const std::string &, int port);
        virtual ~Client();
        
        virtual void connect(const std::string & name = "rtech-test");
        
        virtual void run();
        
        virtual bool hasCommands() const;

        virtual Command nextCommand() const;
        
        virtual void sendCommand(const Command &);
        
        virtual void sendCommand(const Command::Type &);
        virtual void sendCommand(const Command::Type &, const std::string &);
        virtual void sendCommand(const Command::Type &, const std::string &, const std::string &);
        virtual void sendCommand(const Command::Type &, const std::string &, const std::string &, const std::string &);
        virtual void sendCommand(const Command::Type &, const std::string &, const std::string &, const std::string &, const std::string &);
        
        virtual void setName(const std::string &);
        
        virtual inline const std::string & getName() const {
            return this->username;
        }
        
        virtual void joinChannel(const std::string &);
        virtual void partChannel(const std::string &);
        
        virtual inline ChannelPointer getChannel() const {
            if (this->activeChannels.empty()){
                return ChannelPointer(NULL);
            }
            return this->activeChannels[this->currentChannel];
        }
        
        virtual inline std::vector< ChannelPointer > & channelList(){
            return this->activeChannels;
        }
        
        virtual std::string channelListAsString();
        
        virtual unsigned int getChannelIndex(const std::string &);
        
        virtual bool isCurrentChannel(const std::string &);
        
        virtual void setChannel(unsigned int channel);
        
        virtual void nextChannel();
        
        virtual void previousChannel();
        
        virtual void sendMessage(const std::string &);
        
        virtual void sendPong(const Command &);
        
        virtual inline void setMessagesDisabled(bool disabled){
            this->disableMessages = disabled;
        }
        
    protected:
        
        void removeChannel(const std::string &);
        
        ChannelPointer findChannel(const std::string &);
        
        std::string readMessage();
        
        //! Doesn't do anything to the command just handle some internal changes like username and channel stuff
        void checkResponseAndHandle(const Command &);
        
        Network::Socket socket;
        std::string previousUsername;
        std::string username;
        unsigned int previousActiveChannel;
        unsigned int currentChannel;
        std::vector< ChannelPointer > activeChannels;
        std::string hostname;
        int port;
        bool end;
        mutable std::queue< Command > commands;
        bool disableMessages;
    };
    
namespace Message{
    
    class EventInterface{
    public:
        EventInterface();
        virtual ~EventInterface();
        // Notify by message from local user
        virtual void localNotify(const std::string &);
        // Notify by command from local user
        virtual void localCommand(const std::vector<std::string> &);
        // Notify by message from remote
        virtual void remoteNotify(const std::string &);
        // Notify by command from remote
        virtual void remoteCommand(const Command &);
    };
    
    class HandlerInterface{
    public:
        HandlerInterface();
        virtual ~HandlerInterface();
        
        typedef void *(*NotifyCallback)(const std::string &);
        
        virtual inline void subscribe(NotifyCallback subscriber){
            this->callbacks.push_back(subscriber);
        }
        
        virtual inline void subscribe(EventInterface * subscriber){
            this->subscribers.push_back(subscriber);
        }
    protected:
        // Callbacks for events and messages
        std::vector<NotifyCallback> callbacks;
        std::vector<EventInterface *> subscribers;
    };
    
    class QueueInterface{
    public:
        QueueInterface();
        virtual ~QueueInterface();
        
        virtual void addMessage(const std::string &) = 0;
        
        class FontWrapper{
        public:
            FontWrapper();
            virtual ~FontWrapper();
            
            virtual int getWidth(const std::string &)=0;
            virtual int getHeight()=0;
        };
        virtual void processMessages(FontWrapper &, int width, int height);
        
    protected:        
        std::queue<std::string> messages;
        std::deque<std::string> buffer;
    };
}
    // Create a tabbed chatter to implement in games
    class ChatInterface: public Message::HandlerInterface{
    public:
        ChatInterface(const std::string &, int port, const std::string &);
        virtual ~ChatInterface();
        void act();
        void draw(const Graphics::Bitmap &);
        
        void nextChannel();
        void previousChannel();
        void gotoChannel(const std::string &);
        
        Util::ReferenceCount<Client> getClient();
        
        Util::ReferenceCount<Gui::TabItem> getCurrentTab();
        
        Util::ReferenceCount<Gui::TabItem> getTabByName(const std::string &);
        
        void addMessageToTab(const std::string &);
        
        void addMessageToTab(const std::string &, const std::string &);
        
        inline void setWidthRatio(double ratio){
            this->widthRatio = ratio;
        }
        inline void setHeightRatio(double ratio){
            this->heightRatio = ratio;
        }
        
        inline Gui::LineEdit & getInputBox() { 
            return this->inputBox;
        }
        
    protected:
        void updateDimensions();
        void remoteNotify(const std::string &);
        void processRemoteCommands();
        
        static void submit(void *);
        // Notifications and commands for submit
        void localNotify(const std::string &);
        void localCommand(const std::vector<std::string> &);
        
        // Update user list
        void updateUserList();
        void removeUser(const std::string &);
        void changeUserName(const std::string &, const std::string &);
        
        Util::ReferenceCount<Client> client;
        const std::string & host;
        Gui::TabContainer chatBox;
        Gui::LineEdit inputBox;
        Gui::SimpleList listBox;
        double widthRatio;
        double heightRatio;
        int width;
        int height;
        int checkWidth;
        int checkHeight;
        Util::ReferenceCount<Gui::TabItem> serverTab;
        // check ctcp reply
        std::map<std::string, uint64_t> pingReply;
        std::map< std::string, std::vector<std::string> > namesRequest;
    };
    
}// end irc
}
#endif
