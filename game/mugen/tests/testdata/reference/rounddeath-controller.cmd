[Statedef -2]

[State -2, KO trace]
type=LifeAdd
trigger1=RoundNo=1 && TeamSide=1 && RoundState=2 && Ctrl && Time=10
ignorehitpause=1
value=-Life
kill=1

[State -2, KO trace]
type=VarSet
trigger1=RoundNo=1 && !Alive && Var(0)<8
ignorehitpause=1
v=1+Var(0)
value=StateNo

[State -2, KO trace]
type=VarSet
trigger1=RoundNo=1 && !Alive && Var(0)<8
ignorehitpause=1
v=10+Var(0)
value=Time

[State -2, KO trace]
type=VarSet
trigger1=RoundNo=1 && !Alive && Var(0)<8
ignorehitpause=1
v=20+Var(0)
value=GameTime+1

[State -2, KO trace]
type=VarSet
trigger1=RoundNo=1 && !Alive && Var(0)<8
ignorehitpause=1
v=30+Var(0)
value=RoundState

[State -2, KO trace]
type=VarAdd
trigger1=RoundNo=1 && !Alive && Var(0)<8
ignorehitpause=1
v=0
value=1

[State -2, KO trace]
type=ChangeState
trigger1=!Alive && StateNo!=5150
ignorehitpause=1
value=5150
ctrl=0

[State -2, KO trace]
type=AssertSpecial
trigger1=RoundNo>=2
ignorehitpause=1
flag=intro
