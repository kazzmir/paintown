[Statedef -3]
[State 5110, sample]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=8
fv=0
value=Enemy,GetHitVar(hitcount)
[State 5110, sample]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=8
fv=1
value=Enemy,GetHitVar(fallcount)
[State 5110, sample]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=8
fv=2
value=Enemy,GetHitVar(chainid)
[State 5110, sample]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=8
fv=3
value=Enemy,GetHitVar(xveladd)
[State 5110, sample]
type=VarSet
trigger1=TeamSide=1 && StateNo=700 && Time=8
fv=4
value=Enemy,GetHitVar(yveladd)
[Statedef -1]
[State -1, inert]
type=Null
trigger1=0
