[Statedef -2]
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && Var(0)=0 && (RoundNo=1 && RoundState=2 && Ctrl)
ignorehitpause=1
v=0
value=GameTime+1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && Var(1)=0 && (RoundNo=1 && RoundState>=3)
ignorehitpause=1
v=1
value=GameTime+1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && Var(2)=0 && (RoundNo=1 && RoundState=3)
ignorehitpause=1
v=2
value=GameTime+1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && Var(3)=0 && (RoundNo=1 && RoundState=4)
ignorehitpause=1
v=3
value=GameTime+1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && Var(4)=0 && (RoundNo=1 && Var(1)>0 && !Ctrl)
ignorehitpause=1
v=4
value=GameTime+1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && Var(5)=0 && (RoundNo=1 && MatchOver)
ignorehitpause=1
v=5
value=GameTime+1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && Var(6)=0 && (RoundNo=1 && StateNo=180)
ignorehitpause=1
v=6
value=GameTime+1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && Var(7)=0 && (RoundNo=2)
ignorehitpause=1
v=7
value=GameTime+1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && Var(9)=0 && (RoundNo=1 && Win)
ignorehitpause=1
v=9
value=GameTime+1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && Var(12)=0 && (RoundNo=1 && (EnemyNear,Lose))
ignorehitpause=1
v=12
value=GameTime+1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && Var(13)=0 && (RoundNo=1 && (EnemyNear,StateNo)=5150)
ignorehitpause=1
v=13
value=GameTime+1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && RoundNo=1 && Var(1)>0 && Var(10)<40
ignorehitpause=1
v=20+Var(10)
value=GameTime+1-Var(1)
[State -2, round clock]
type=VarAdd
trigger1=GameTime<=400 && TeamSide=1 && RoundNo=1 && Var(1)>0 && Var(10)<40
ignorehitpause=1
v=10
value=1
[State -2, round clock]
type=VarAdd
trigger1=GameTime<=400 && TeamSide=1 && RoundNo=1 && Var(1)>0 && RoundState<4
ignorehitpause=1
v=8
value=1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && RoundNo=1 && RoundState<4
ignorehitpause=1
v=11
value=GameTime+1
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && RoundNo=1 && StateNo=180 && Time=0
ignorehitpause=1
v=14
value=PrevStateNo
[State -2, round clock]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && RoundNo=1 && Win
ignorehitpause=1
v=15
value=WinKO+2*WinTime+4*WinPerfect
[State -2, round clock]
type=AssertSpecial
trigger1=RoundNo>1
ignorehitpause=1
flag=intro

[State -2, authored dead pose]
type=ChangeState
trigger1=TeamSide=2 && !Alive && StateNo!=5150
value=5150
ignorehitpause=1

[State -2, independent clock control]
type=VarSet
trigger1=GameTime<=400 && TeamSide=1 && Var(16)=0 && MoveHit
ignorehitpause=1
v=16
value=GameTime+1
