class Object:
    def __init__(self, object):
        self.object = object

    def getId(self):
        import paintown_internal
        return paintown_internal.getId(self.getObject())

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

    # Callbacks
    def didCollide(self, him):
        pass

    def takeDamage(self, him, damage):
        pass

    def tick(self):
        pass

class Character(Object):
    def __init__(self, character):
        Object.__init__(self, character)

    def isPlayer(self):
        return False
    
    def didAttack(self, him):
        pass

class Player(Character):
    def __init__(self, player):
        Character.__init__(self, player)
    
    def isPlayer(self):
        return True

    def getScore(self):
        import paintown_internal
        return paintown_internal.getScore(self.getObject())

    def increaseScore(self, much):
        self.setScore(self.getScore() + much)

    def setScore(self, score):
        import paintown_internal
        paintown_internal.setScore(self.getObject(), score)

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

    def findObject(self, id):
        import paintown_internal
        return paintown_internal.findObject(self.world, id)

    def getObjects(self):
        import paintown_internal
        return paintown_internal.getObjects(self.world)

    def getEnemies(self):
        return filter(lambda n: not n.isPlayer(), self.getObjects())
    
    def getPlayers(self):
        return filter(lambda n: n.isPlayer(), self.getObjects())

    def addCharacter(self, path, name, map, health, x, z):
        import paintown_internal
        return paintown_internal.addCharacter(self.world, path, name, map, health, x, z)

    def cacheCharacter(self, path):
        import paintown_internal
        paintown_internal.cacheCharacter(path)

    def createCharacter(self, character):
        return Character(character)

    def createPlayer(self, player):
        return Player(player)

    # def currentBlock(self):
    #     import paintown_internal
    #     assert(self.world != None)
    #     return getBlock(paintown_internal.currentBlock(self.world))

    # def getBlock(self,id):
    #     return Block(self.world, id)

    def tick(self):
        pass

engines = []
def register(engine):
    engines.append(engine)

def checkEngine():
    if len(engines) == 0:
        raise PaintownException("No engines were registered!")

def createCharacter(character):
    checkEngine()
    return engines[0].createCharacter(character)

def createPlayer(player):
    checkEngine()
    return engines[0].createPlayer(player)

def createWorld(world):
    checkEngine()
    for engine in engines:
        engine.createWorld(world)

def tick():
    checkEngine()
    for engine in engines:
        engine.tick()
