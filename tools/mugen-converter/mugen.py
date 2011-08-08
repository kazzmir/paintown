# Mugen Classes

# TODO Add state and controller classes


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
        self.paletteFiles = []
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
        self.headPos = None
        self.midPos = None
        self.shadowoffset = None
        self.drawOffset = None

        # Velocity (constants)
        self.walkFwd = None
        self.walkBack = None
        self.runFwd = None
        self.runBack = None
        self.jumpNeu = None
        self.jumpBack = None
        self.jumpFwd = None
        self.runjumpBack = None
        self.runjumpFwd = None
        self.airjumpNeu = None
        self.airjumpBack = None
        self.airjumpFwd = None

        # Movement (constants)
        self.airjumpNum = None
        self.airjumpHeight = None
        self.yaccel = None
        self.stand.friction = None
        self.crouch.friction = None

    def setName(self, name):
        self.name = name
        
