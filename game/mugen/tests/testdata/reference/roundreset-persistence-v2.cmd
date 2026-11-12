[Statedef -2]

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=5
ignorehitpause=1
v=0
value=70

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=5
ignorehitpause=1
v=29
value=71

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=5
ignorehitpause=1
v=30
value=72

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=5
ignorehitpause=1
fv=0
value=1.25

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=5
ignorehitpause=1
fv=19
value=2.5

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=5
ignorehitpause=1
fv=20
value=3.75

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=5
ignorehitpause=1
fv=39
value=5.5

[State -2, reset probe]
type=PowerSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=5
ignorehitpause=1
value=1234

[State -2, reset probe]
type=Helper
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=5
ignorehitpause=1
id=7
stateno=9000
postype=p1
pos=1000,0
keyctrl=0

[State -2, reset probe]
type=Explod
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=5
ignorehitpause=1
id=8
anim=9990
removetime=-1
postype=p1
pos=1000,0

[State -2, reset probe]
type=Projectile
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=5
ignorehitpause=1
projid=9
projanim=9990
projremovetime=1000
velocity=0,0
offset=1000,0
projedgebound=10000
projstagebound=10000
attr=S,NP
hitflag=MAF
damage=1
sparkno=-1
hitsound=-1,0

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=7
ignorehitpause=1
v=45
value=NumHelper

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=7
ignorehitpause=1
v=46
value=NumProj

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=7
ignorehitpause=1
v=47
value=NumExplod

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=7
ignorehitpause=1
v=48
value=ID

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=7
ignorehitpause=1
v=49
value=Helper(7),ID

[State -2, reset probe]
type=LifeAdd
triggerall=!IsHelper
trigger1=RoundNo=1 && TeamSide=2 && RoundState=2 && Ctrl && Time=20
ignorehitpause=1
value=-Life
kill=1

[State -2, reset probe]
type=ChangeState
triggerall=!IsHelper
trigger1=!Alive && StateNo!=5150
ignorehitpause=1
value=5150
ctrl=0

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=32
value=Var(0)

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=33
value=Var(29)

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=34
value=Var(30)

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=35
value=FVar(0)*100

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=36
value=FVar(19)*100

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=37
value=FVar(20)*100

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=38
value=FVar(39)*100

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=39
value=Power

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=40
value=NumHelper

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=41
value=NumProj

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=42
value=NumExplod

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=43
value=PalNo

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=44
value=ID-Var(48)

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=50
value=PlayerIDExist(Var(49))

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=51
value=Life

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=52
value=RoundNo

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=53
value=RoundsExisted

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=54
value=StateNo

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=55
value=GameTime+1

[State -2, reset probe]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=2 && Var(31)=0
ignorehitpause=1
v=31
value=1

[State -2, reset probe]
type=AssertSpecial
triggerall=!IsHelper
trigger1=RoundNo>=2
ignorehitpause=1
flag=intro
