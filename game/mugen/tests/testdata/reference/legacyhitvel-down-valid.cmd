[Statedef -3]
[State -3, frozen snapshot]
type=VarSet
trigger1=TeamSide=1 && GameTime=200
fv=0
value=Enemy,HitVel X
[State -3, frozen snapshot]
type=VarSet
trigger1=TeamSide=1 && GameTime=200
fv=1
value=Enemy,HitVel Y
[State -3, frozen snapshot]
type=VarSet
trigger1=TeamSide=1 && GameTime=200
fv=2
value=Enemy,GetHitVar(xvel)
[State -3, frozen snapshot]
type=VarSet
trigger1=TeamSide=1 && GameTime=200
fv=3
value=Enemy,GetHitVar(yvel)
[State -3, frozen snapshot]
type=VarSet
trigger1=TeamSide=1 && GameTime=200
fv=4
value=Enemy,FVar(28)
[State -3, frozen snapshot]
type=VarSet
trigger1=TeamSide=1 && GameTime=200
fv=5
value=Enemy,FVar(29)
[State -3, frozen snapshot]
type=VarSet
trigger1=TeamSide=1 && GameTime=200
fv=6
value=Enemy,Vel X
[State -3, frozen snapshot]
type=VarSet
trigger1=TeamSide=1 && GameTime=200
fv=7
value=Enemy,Vel Y
[State -3, frozen snapshot]
type=VarSet
trigger1=TeamSide=1 && GameTime=200
fv=8
value=HitVel X
[State -3, frozen snapshot]
type=VarSet
trigger1=TeamSide=1 && GameTime=200
fv=9
value=HitVel Y
[State -3, frozen snapshot]
type=VarSet
trigger1=TeamSide=1 && GameTime=200
fv=10
value=Enemy,FVar(26)
[State -3, frozen snapshot]
type=VarSet
trigger1=TeamSide=1 && GameTime=200
fv=11
value=Enemy,FVar(27)
[State -3, before hit X]
type=VarSet
trigger1=GameTime=120
fv=26
value=HitVel X
[State -3, before hit Y]
type=VarSet
trigger1=GameTime=120
fv=27
value=HitVel Y
[Statedef -1]
[State -1, inert]
type=Null
trigger1=0
