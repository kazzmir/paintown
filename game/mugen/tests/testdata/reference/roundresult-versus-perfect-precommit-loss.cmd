[Statedef -2]

[State -2, perfect boundary]
type=LifeAdd
trigger1=RoundNo=1 && TeamSide=1 && StateNo=700 && Time=13
value=-100
kill=0

[State -2, initial metadata]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20)=0
ignorehitpause=1
v=(RoundNo-1)*20+15
value=RoundNo

[State -2, initial metadata]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20)=0
ignorehitpause=1
v=(RoundNo-1)*20+16
value=RoundsExisted

[State -2, initial metadata]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20)=0
ignorehitpause=1
v=(RoundNo-1)*20+17
value=StateNo

[State -2, initial metadata]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20)=0
ignorehitpause=1
v=(RoundNo-1)*20+18
value=Power

[State -2, power seed]
type=PowerSet
trigger1=RoundNo=1 && RoundState=2 && Ctrl && Time=5
value=1234

[State -2, round result]
type=LifeAdd
trigger1=RoundState=2 && Ctrl && Time=10 && (StateNo=700 || StateNo=701) && (TeamSide=2)
ignorehitpause=1
value=-Life
kill=1

[State -2, round result]
type=ChangeState
trigger1=!Alive && StateNo!=5150
ignorehitpause=1
value=5150
ctrl=0

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20+0)=0 && (1)
ignorehitpause=1
v=(RoundNo-1)*20+0
value=GameTime+1

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20+1)=0 && (RoundState=2 && Ctrl)
ignorehitpause=1
v=(RoundNo-1)*20+1
value=GameTime+1

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20+2)=0 && (RoundState=3)
ignorehitpause=1
v=(RoundNo-1)*20+2
value=GameTime+1

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20+3)=0 && (Win)
ignorehitpause=1
v=(RoundNo-1)*20+3
value=GameTime+1

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20+4)=0 && (Lose)
ignorehitpause=1
v=(RoundNo-1)*20+4
value=GameTime+1

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20+5)=0 && (DrawGame)
ignorehitpause=1
v=(RoundNo-1)*20+5
value=GameTime+1

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20+7)=0 && (MatchOver)
ignorehitpause=1
v=(RoundNo-1)*20+7
value=GameTime+1

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20+8)=0 && (RoundState>=3 && !Ctrl)
ignorehitpause=1
v=(RoundNo-1)*20+8
value=GameTime+1

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20+9)=0 && (RoundState=4)
ignorehitpause=1
v=(RoundNo-1)*20+9
value=GameTime+1

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20+10)=0 && (StateNo=180)
ignorehitpause=1
v=(RoundNo-1)*20+10
value=GameTime+1

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20+11)=0 && (StateNo=170)
ignorehitpause=1
v=(RoundNo-1)*20+11
value=GameTime+1

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && Var((RoundNo-1)*20+12)=0 && (StateNo=175)
ignorehitpause=1
v=(RoundNo-1)*20+12
value=GameTime+1

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && (RoundState=3)
ignorehitpause=1
v=(RoundNo-1)*20+6
value=WinKO+2*WinTime+4*WinPerfect+8*LoseKO+16*LoseTime+32*DrawGame

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && (RoundState=3)
ignorehitpause=1
v=(RoundNo-1)*20+13
value=Life

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && (RoundState=3)
ignorehitpause=1
v=(RoundNo-1)*20+14
value=EnemyNear,Life

[State -2, round result]
type=VarSet
trigger1=RoundNo<=3 && RoundState=4 && Var((RoundNo-1)*20+19)=0
ignorehitpause=1
v=(RoundNo-1)*20+19
value=MatchOver+1

[State -2, round result]
type=AssertSpecial
trigger1=MatchOver
ignorehitpause=1
flag=roundnotover

[State -2, round result]
type=AssertSpecial
trigger1=RoundNo>3
ignorehitpause=1
flag=intro

[State -2, perfect witness]
type=VarSet
trigger1=RoundNo=1 && StateNo=180 && Time=5
v=17
value=WinPerfect+10*Win

[State -2, post result life]
type=VarSet
trigger1=RoundNo=1 && StateNo=180 && Time=5
v=18
value=Life
