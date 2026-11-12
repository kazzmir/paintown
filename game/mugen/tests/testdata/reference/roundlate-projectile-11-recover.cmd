[Statedef -2]

[State -2, contact witness]
type=VarSet
trigger1=!IsHelper && RoundNo=1 && TeamSide=1 && MoveType=H && Var(17)=0
v=18
value=GetHitVar(damage)

[State -2, contact witness]
type=VarSet
trigger1=!IsHelper && RoundNo=1 && TeamSide=1 && MoveType=H && Var(17)=0
v=19
value=Life

[State -2, contact witness]
type=VarSet
trigger1=!IsHelper && RoundNo=1 && TeamSide=1 && MoveType=H && Var(17)=0
v=17
value=GameTime+1

[State -2, spawn witness]
type=VarSet
trigger1=!IsHelper && RoundNo=1 && TeamSide=1 && (EnemyNear,NumProj)>Var(16)
v=16
value=EnemyNear,NumProj

[State -2, late KO]
type=Projectile
triggerall=!IsHelper
trigger1=RoundNo=1 && TeamSide=2 && StateNo=701 && Time=1
ignorehitpause=1
projid=7
projanim=9992
projremovetime=500
velocity=-10,0
offset=360,0
projedgebound=10000
projstagebound=10000
attr=S,NP
hitflag=MAF
damage=1000
pausetime=0,0
ground.hittime=1
ground.slidetime=0
ground.velocity=0,0
air.velocity=0,0
sparkno=-1
hitsound=-1,0

[State -2, late KO]
type=LifeAdd
triggerall=!IsHelper
trigger1=RoundNo=1 && TeamSide=2 && StateNo=701 && Time=10
ignorehitpause=1
value=-Life
kill=1

[State -2, late KO]
type=ChangeState
triggerall=!IsHelper
trigger1=!Alive && StateNo!=5150
ignorehitpause=1
value=5150
ctrl=0

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=Var(0)=0 && (RoundNo=1 && Ctrl)
ignorehitpause=1
v=0
value=GameTime+1

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=Var(1)=0 && (RoundState=3)
ignorehitpause=1
v=1
value=GameTime+1

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=Var(2)=0 && (Win)
ignorehitpause=1
v=2
value=GameTime+1

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=Var(3)=0 && (Lose)
ignorehitpause=1
v=3
value=GameTime+1

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=Var(4)=0 && (DrawGame)
ignorehitpause=1
v=4
value=GameTime+1

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=Var(5)=0 && (RoundState=4)
ignorehitpause=1
v=5
value=GameTime+1

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=Var(6)=0 && (StateNo=180)
ignorehitpause=1
v=6
value=GameTime+1

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=Var(7)=0 && (StateNo=175)
ignorehitpause=1
v=7
value=GameTime+1

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=Var(8)=0 && (RoundNo=2)
ignorehitpause=1
v=8
value=GameTime+1

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState>=3
ignorehitpause=1
v=9
value=Life

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState>=3
ignorehitpause=1
v=10
value=EnemyNear,Life

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState>=3
ignorehitpause=1
v=11
value=Win

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState>=3
ignorehitpause=1
v=12
value=Lose

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState>=3
ignorehitpause=1
v=13
value=DrawGame

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState>=3
ignorehitpause=1
v=14
value=NumHelper

[State -2, late KO]
type=VarSet
triggerall=!IsHelper
trigger1=RoundNo=1 && RoundState>=3
ignorehitpause=1
v=15
value=RoundState

[State -2, late KO]
type=AssertSpecial
triggerall=!IsHelper
trigger1=RoundNo>1
ignorehitpause=1
flag=intro
