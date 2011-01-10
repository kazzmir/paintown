#ifndef _paintown_object_messages_h
#define _paintown_object_messages_h

/* all network messages */

namespace ObjectMessages{
    enum NetworkCodes{
        Moved = 0,
        Collided = 1,
    };
}

namespace CharacterMessages{
    enum NetworkCodes{
        Animation = 20,
        Jump = 21,
        Explode = 22,
        Health = 23,
        Fall = 24,
        Ungrab = 25,
        ShowName = 26,
        SetName = 29
    };
}

namespace PlayerMessages{
    enum NetworkCodes{
        Score = 27,
        Confirm = 28,
    };
}

/* used for sending over the network */

#endif
