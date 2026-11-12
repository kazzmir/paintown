[Statedef -3]
[State -3, stored pause]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && MoveContact>0 && Var(50)<6
ignorehitpause=0
fv=Var(50)+0
value=HitPauseTime
[State -3, stored pause]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && MoveContact>0 && Var(50)<6
ignorehitpause=0
fv=Var(50)+6
value=Enemy,HitPauseTime
[State -3, next]
type=VarAdd
trigger1=TeamSide=1 && StateNo=700 && MoveContact>0 && Var(50)<6
ignorehitpause=0
v=50
value=1
[Statedef -1]
[State -1, inert]
type=Null
trigger1=0
