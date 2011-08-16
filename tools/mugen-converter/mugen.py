# Mugen Classes

# Command class
class Command():
    def __init__(self):
        self.state = None
        self.keys = []
        self.time = None
        self.bufferTime = None

# StateDef holds it's parameters declared in statedef and it's relative procedure states
class StateDef():
    def __init__(self, world):
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
        self.commands = []
        
        # State holders
        self.states = {}

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
        
        self.commands.append(cmd)
        
    def addState(self, number, state):
        self.states[number] = state
