[Statedef -3]
[State -3, observation 0]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=2
fv=0
value=Enemy,GetHitVar(hitcount)
[State -3, observation 1]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=2
fv=1
value=Enemy,GetHitVar(hitid)
[State -3, observation 2]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=8
fv=2
value=Enemy,GetHitVar(hitcount)
[State -3, observation 3]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=8
fv=3
value=Enemy,GetHitVar(hitid)
[State -3, observation 4]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=8
fv=4
value=Enemy,GetHitVar(chainid)
[State -3, observation 5]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=8
fv=5
value=Enemy,GetHitVar(fallcount)
[State -3, observation 6]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=18
fv=6
value=Enemy,GetHitVar(hitcount)
[State -3, observation 7]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=18
fv=7
value=Enemy,GetHitVar(hitid)
[State -3, observation 8]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=18
fv=8
value=Enemy,GetHitVar(chainid)
[State -3, observation 9]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=18
fv=9
value=Enemy,GetHitVar(fallcount)
[State -3, observation 10]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=28
fv=10
value=Enemy,GetHitVar(hitcount)
[State -3, observation 11]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=28
fv=11
value=Enemy,GetHitVar(chainid)
[State -3, leave combo]
type=StateTypeSet
trigger1=TeamSide=2 && (Enemy,StateNo)=700 && (Enemy,Time)=20
movetype=I
[Statedef -1]
[State -1, inert]
type=Null
trigger1=0
