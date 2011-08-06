# Mugen Classes

# TODO Add state and controller classes


# Character Class
class Character():
    def __init__(self):
        self.name = None
        self.displayName = None
        self.versionDate = []
        self.mugenVersion = []
        self.author = None
        self.paletteDefaults = []
        self.cmdFile = None
        self.cnsFile = None
        self.stFile = None
        self.stcommonFile = None
        self.spriteFile = None
        self.animFile = None
        self.soundFile = None
        self.palFiles = []
        self.introStoryboard = None
        self.endingStoryboard = None

    def setName(self, name):
        self.name = name
        
