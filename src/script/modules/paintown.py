class Object:
    def __init__(self, object):
        self.object = object

    def takeDamage(self, him, damage):
        pass

    def tick(self):
        pass

    def getX(self):
        import paintown_internal
        return paintown_internal.getX(self.getObject())

    def setX(self, value):
        import paintown_internal
        paintown_internal.setX(self.getObject(), value)
    
    def getY(self):
        import paintown_internal
        return paintown_internal.getY(self.getObject())
    
    def setY(self, value):
        import paintown_internal
        paintown_internal.setY(self.getObject(), value)

    def getZ(self):
        import paintown_internal
        return paintown_internal.getZ(self.getObject())
    
    def setZ(self, value):
        import paintown_internal
        paintown_internal.setZ(self.getObject(), value)

    def getObject(self):
        return self.object

    def getHealth(self):
        import paintown_internal
        return paintown_internal.getHealth(self.getObject())

    def setHealth(self, health):
        import paintown_internal
        return paintown_internal.setHealth(self.getObject(), health)

class Character(Object):
    def __init__(self, character):
        Object.__init__(self, character)

class Engine:
    def __init__(self):
        self.world = None
        pass

    def createWorld(self, world):
        self.world = world

    def levelLength(self):
        import paintown_internal
        assert(self.world != None)
        return paintown_internal.levelLength(self.world)

    def createCharacter(self, character):
        return Character(character)

    def currentBlock(self):
        import paintown_internal
        assert(self.world != None)
        return getBlock(paintown_internal.currentBlock(self.world))

    def getBlock(self,id):
        return Block(self.world, id)

    def tick(self):
        pass

engines = []
def register(engine):
    engines.append(engine)

def createCharacter(character):
    return engines[0].createCharacter(character)

def createWorld(world):
    for engine in engines:
        engine.createWorld(world)

def tick():
    for engine in engines:
        engine.tick()

"""
>> Necessary things to support:
>> >>
>> >> 1) Global variables (string variables with a numerical value attached)
>> >>
> > ok
>> >> 2) Entity variables (string variables with a numerical value attached)
>> >>
> > ok
>> >> 3) Local variables (string variables with a numerical value attached)
>> >>
> > ok - comes with python
>> >> 4) Global indexed variables (numerical values in an index with a
>> >> numerical value attached)
>> >>
> > ok
>> >> 5) Entity indexed variables (numerical values in an index with a
>> >> numerical value attached)
>> >>
> > ok
>> >> 6) Local indexed variables (numerical values in an index with a
>> >> numerical value attached)
>> >>
> > ok - comes with python
>> >> 7) Change entity properties (properties that can be changed on a per
>> >> entity basis, i.e. different states, animations, position velocity etc.)
>> >>
> > ok
>> >> 8) Get entity properties (getting the values of the properties of an
>> >> entity for use in a function)
>> >>
> > ok
>> >> 9) Get Constants (getting the values of constants the engine uses for
>> >> the game to apply to functions)
>> >>
> > ok
>> >> 10) Change level/area properties (properties that can be changed on a
>> >> per level/area basis, i.e. different weather, music, gravity, etc.)
>> >>
> > ok
>> >> 11) Get level properties (getting the values of the properties of a
>> >> level/area for use in a function)
>> >>
> > ok
>> >> 12) NULL() to see if a variable is empty
>> >>
> > ok - part of python
>> >> 13) Draw and load sprites for use in variable situations, i.e. HUD
>> >> elements, various effects, etc.
>> >>
> > ok, probably
>> >> 14) Spawn entities/objects
>> >>
> > ok
>> >> 15) Kill entities/objects
>> >>
> > ok
>> >> 16) Clear variables (Global, Entity, Local)
>> >>
> > ok, part of python
>> >> 17) Load and play sound samples, music, effects
>> >>
> > ok
>> >> 18) Drawmethods for drawing dots, lines, and boxes on screen.
>> >>
> > ok, but may be somewhat restricted
>> >> 19) Text object creation to create text in areas of the screen.
>> >>
> > ok
>> >> 20) Event based script handling, basically scripts that are activated
>> >> on key stokes/button push, hitting, blocking, when an animation is
>> >> played, when something gets damaged, etc.
>> >>
>> >> Please see
>> >>
> >
http://www.lavalit.com/index.php?PHPSESSID=c2ee31164c10ff1a49806fceba34219c&topic=164.0
>> >>
>> >>
>> >> or the attached script notes document I have for OpenBOR, contains a
>> >> lot of functions generally used in beat-em ups
>> >>
> > ok
>> >> 21) Detecting line of sight and related functions (ie. spawning
>> >> entities off-camera)
>> >>
> > ok
>> >> 22) Setting characters/entities visible
>> >>
> > as in visible/invisible? ok
>> >> 23) Triggers : Either placed in the editor or defined/spawned in
>> >> script... with ON_ENTER, ON_EXIT and multiple entry, screening for
>> >> particular entities, etc.  Point based Triggers (point + radius,
>> >> sphere or box) with multiple point based definitions.
>> >>
> > ok
>> >> 24) Support for broadcast messages that can be sent and watched for by
>> >> parts of the script is highly desirable
>> >>
> > ok, part of the event system.
>> >> 25) Arrays (we also had dynamic arrays, ie. no need to predefine the
>> >> number of elements in the declaration)
>> >>
> > ok - part of python
>> >> 26) Cutscenes (we had a cutscene camera that we could activate, and
>> >> tell to zoom, rotate, follow a path we defined in script or in the
>> >> editor, and focus on a certain point, pan between two points [at a
>> >> speed we defined], blend out of or into the game camera etc)
>> >>
> > ok
>> >> 27) Play interactions between two participants (co-ordinated
>> >> animations, possibly interactive)
>> >>
> > it depends on how complicated, but ok
>> >> 28) Support for includes (script libraries made stuff like timers and
>> >> cutscenes so much easier for us as time went on!!!)
>> >>
> > ok, part of python
>> >> 29) Make sure to make it easy to expose additional functionality and
>> >> variables to the designers
>> >>
> > ok
>> >> 30) You may want to look into creating a designer-definable
>> >> state-based AI machine, as well
>> >>
> > ok, part of python
>> >> 31) Timers
>> >>
> > ok, part of python
>> >> 32) Markers that can be dropped at a point and broadcast information
>> >> to nearby entities or AI's (position, radius, effect)
>> >>
> > ok
>> >> 33) Setting the time for the game (not game clock but the game's time
>> >> like time of day)
>> >>
> > ok
>> >> 34) Getting the time for the game
>> >>
> > ok
>> >> 35) Changing the time for the game including the speed of how fast
>> >> time passes
>> >>
> > ok
>> >> 36) Get the total time played
>> >>
> > ok
>> >> 37) Get the total time played for current session
>> >>
> > ok
>> >> 38) Enumerated lists, index with a list per index entry using
>> >> words/strings
>> >>
> > ok, part of python
>> >> 39) Widgets (in 2D and 3D when it is implemented) parameters including
>> >> position, velocity, scale.
>> >>
>> >>
>> >> Optional things that would help a ton
>> >>
>> >> 1) Multi-threads
>> >>
> > ok, each script will be its own thread.
>> >> 2) Spawn threads
>> >>
> > same as above
>> >> 3) Kill threads
>> >>
> > probably will work
>> >> 4) Physics, apply force in vector, access to physics properties
>> >>
> > ok
>> >> 5) Use the real world the for play functions, like happy birthday
>> >> stuff for the player
>> >>
> > ok, part of python
>> >>
>> >> Parameters for some of the functions we'll need:
>> >>
>> >> Entities ( id, name, AI_flags, state, anim, position, velocity,
>> >> exists, line_of_sight, disposition, collision, inventory, karma,
> > gravity)
>> >>
>> >>    id = id number/entity number
>> >>
>> >>    name = entity's name
>> >>
>> >>    AI_flags = flags to set for AI states
>> >>
>> >>    state = current state for the entity i.e. talking, walking, etc.
>> >>
>> >>    anim = current animation of entity
>> >>
>> >>    position = current position of entity (x, y, z)
>> >>
>> >>    velocity = movement speed of entity (most likely a float)
>> >>
>> >>    exists = if the entity exists or not
>> >>
>> >>    line_of_sight = radius of line of sight (cube or sphere)
>> >>
>> >>    disposition = status of entity towards other entities (i.e. how it
>> >> feels towards another entity, angry, happy,  aggressive etc.)
>> >>
>> >>    collision = collsion properties of the entity, if it has it or not,
>> >> specifics related to collision detection system
>> >>
>> >>    inventory = inventory of the character can be an index with lists,
>> >> also if the character has an index or not, contains all the characters
>> >> items by id or name
>> >>
>> >>    karma = represents how the other entities view this character (most
>> >> likely an int)
>> >>
>> >>    gravity = on/off if character is affected by level's/area's
>> >> gravity, also has parameter for altering overall gravity with a
>> >> numerical float value to affect velocity on x, y, z
>> >>
>> >> Levels ( id, name, zone, weather, loaded, gravity)
>> >>      id = id number/level number
>> >>
>> >>    name = level's name
>> >>
>> >>    zone = area the level is in, where it is located on the world
>> >>
>> >>    weather = what is the current weather of the area/level
>> >>
>> >>    loaded = if the area is loaded or not
>> >>
>> >>    gravity = how characters'/entities' movements are affected in the
>> >> level, i.e. normal, water, space, or better yet a numerical float
>> >> value to affect velocity on x, y, z for all entities, can be altered
>> >> individually for entities
> > all those variables are fine.
"""
