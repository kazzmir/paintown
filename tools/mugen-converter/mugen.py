# Mugen Classes

# Exception handler
class MugenException(Exception):
    pass

# Command class
class Command():
    def __init__(self):
        self.state = None
        self.keys = []
        self.time = None
        self.bufferTime = None

# StateDef holds it's parameters declared in statedef and it's relative procedure states
class StateDef():
    def __init__(self, player, world):
        self.player = player
        self.world = world
        self.stateType = None
        self.moveType = None
        self.physics = None
        self.anim = None
        self.velset = None
        self.ctrl = None
        self.poweradd = None
        self.juggle = None
        self.facep2 = None
        self.hitdefpersist = None
        self.movehitpersist = None
        self.hitcountpersist = None
        self.sprpriority = None
    
    def evaluate(self, world):
        pass

# TODO Controller classes
class Controller():
    def __init__(self):
        pass

# Character Class
class Character():
    def __init__(self):
        # From character definition file
        self.name = None
        self.displayName = None
        self.versionDate = []
        self.mugenVersion = []
        self.author = None
        self.paletteDefaults = []
        self.cmdFile = None
        self.stateFiles = []
        self.spriteFile = None
        self.animFile = None
        self.soundFile = None
        self.paletteFiles = {}
        self.introStoryboard = None
        self.endingStoryboard = None
        
        # Defaults (cmd file)
        self.commandTime = None;
        self.commandBufferTime = None;
        
        # Data (constants)
        self.life = None
        self.attack = None
        self.defence = None
        self.fallDefenceUp = None
        self.liedownTime = None
        self.airjuggle = None
        self.sparkno = None
        self.guardSparkno = None
        self.koEcho = None
        self.volume = None
        self.intPersistIndex = None
        self.floatPersistIndex = None

        # Size (constants)
        self.xscale = None
        self.yscale = None
        self.groundBack = None
        self.groundFront = None
        self.airBack = None
        self.airFront = None
        self.height = None
        self.attackDist = None
        self.projAttackDist = None
        self.projDoscale = None
        self.headPos = []
        self.midPos = []
        self.shadowoffset = None
        self.drawOffset = []

        # Velocity (constants)
        self.walkFwd = []
        self.walkBack = []
        self.runFwd = []
        self.runBack = []
        self.jumpNeu = []
        self.jumpBack = []
        self.jumpFwd = []
        self.runjumpBack = []
        self.runjumpFwd = []
        self.airjumpNeu = []
        self.airjumpBack = []
        self.airjumpFwd = []

        # Movement (constants)
        self.airjumpNum = None
        self.airjumpHeight = None
        self.yaccel = None
        self.standFriction = None
        self.crouchFriction = None
        
        # Command Holders
        self.commands = {}
        
        # State holders
        self.states = {}
        
        # Current State
        self.currentState = None
        
        # States -3, -2, -1
        self.neg3State = None
        self.neg2State = None
        self.neg1State = None

    def setName(self, name):
        self.name = name
        
    def addStateFile(self, stateFile):
        for f in self.stateFiles:
            if f == stateFile:
                return
        self.stateFiles.append(stateFile)

    def addCommand(self, state, keys, time=None, bufferTime=None):
        cmd = Command()
        cmd.state = state
        cmd.keys = keys
        if time:
            cmd.time = time
        if bufferTime:
            cmd.bufferTime = bufferTime
        
        self.commands[state] = cmd
        
    def addState(self, number, state):
        self.states[number] = state
        
    def getState(self, number):
        try:
            return self.states[number]
        except KeyError:
            raise MugenException()
        
    def changeState(self, number, world):
        try:
            self.currentState = self.getState(number)(self, world)
        except MugenException:
            self.addState(number, StateDef)
            self.currentState = self.states[number](self, world)

    def act(self, world):
        # State -3, -2, -1
        self.neg3State.evaluate(world)
        self.neg2State.evaluate(world)
        self.neg1State.evaluate(world)
        
        # Current state
        self.currentState.evaluate(world)

# LN definition
def ln(number):
    return math.log(number, math.e)
