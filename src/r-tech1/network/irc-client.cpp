#include "r-tech1/network/irc-client.h"

#include <iostream>
#include <vector>
#include <stdexcept>

#include "r-tech1/graphics/bitmap.h"
#include "r-tech1/font.h"
#include "r-tech1/debug.h"
#include "r-tech1/exceptions/load_exception.h"
#include "r-tech1/token_exception.h"
#include "r-tech1/input/input.h"
#include "r-tech1/input/input-manager.h"
#include "r-tech1/sound/sound.h"
#include "r-tech1/exceptions/exception.h"
#include "r-tech1/network/network.h"
#include "r-tech1/network/chat.h"
#include "r-tech1/network/irc.h"
#include "r-tech1/thread.h"
#include "r-tech1/pointer.h"
#include "r-tech1/system.h"

#include "mugen/util.h"
#include "mugen/search.h"
#include "mugen/exception.h"
#include "mugen/options.h"

#include "mugen/widgets.h"

#include <queue>
