[Statedef -3]
[State -3, stored pause]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && NumHelper(9)>0 && (Helper(9),MoveContact)>0 && Var(50)<6
ignorehitpause=1
fv=Var(50)+0
value=Helper(9),HitPauseTime
[State -3, stored pause]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && NumHelper(9)>0 && (Helper(9),MoveContact)>0 && Var(50)<6
ignorehitpause=1
fv=Var(50)+6
value=HitPauseTime
[State -3, next]
type=VarAdd
trigger1=TeamSide=1 && StateNo=700 && NumHelper(9)>0 && (Helper(9),MoveContact)>0 && Var(50)<6
ignorehitpause=1
v=50
value=1
[Statedef -1]
[State -1, inert]
type=Null
trigger1=0
