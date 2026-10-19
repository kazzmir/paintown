[Statedef -1]
[State -1, contact phase]
type=VarSet
trigger1=Facing=-1 && GameTime=Var(50)+1
fv=8
value=StateNo
[State -1, contact phase]
type=VarSet
trigger1=Facing=-1 && GameTime=Var(50)+1
fv=9
value=GetHitVar(damage)
[State -1, contact phase]
type=VarSet
trigger1=Facing=-1 && GameTime=Var(50)+1
fv=10
value=Life
[State -1, contact phase]
type=VarSet
trigger1=Facing=-1 && GameTime=Var(50)+1
fv=11
value=Time
