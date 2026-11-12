[Statedef -3]
[State -3, observation 0]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=30
fv=0
value=Enemy,FVar(20)
[State -3, observation 1]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=30
fv=1
value=Enemy,FVar(21)
[State -3, observation 2]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=30
fv=2
value=Enemy,FVar(22)
[State -3, observation 3]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=30
fv=3
value=Enemy,FVar(23)
[State -3, observation 4]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=30
fv=4
value=Enemy,GetHitVar(fallcount)
[State -3, observation 5]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=30
fv=5
value=Enemy,GetHitVar(hitcount)
[State -3, observation 6]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=30
fv=6
value=Enemy,GetHitVar(chainid)
[State -3, observation 7]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=30
fv=7
value=Enemy,GetHitVar(chainid)
[State -3, observation 8]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=30
fv=8
value=Enemy,GetHitVar(xveladd)
[State -3, observation 9]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=30
fv=9
value=Enemy,GetHitVar(yveladd)
[State -3, observation 10]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=30
fv=10
value=Enemy,GetHitVar(xveladd)
[State -3, observation 11]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=30
fv=11
value=Enemy,GetHitVar(fall)
[Statedef -1]
[State -1, inert]
type=Null
trigger1=0
