[Command]
name="recovery"
command=/x
time=1
buffer.time=1
[Statedef -3]
[State -3, exact quarter-rule error]
type=VarSet
trigger1=StateNo=5110 && Time>0 && Var(50)-GetHitVar(recovertime)>1
fv=0
value=FVar(0)+abs(GetHitVar(recovertime)-((Var(50)-1)*3/4))
[State -3, accelerated frames]
type=VarAdd
trigger1=StateNo=5110 && Time>0 && Var(50)-GetHitVar(recovertime)>1
fv=1
value=1
[State -3, previous value]
type=VarSet
trigger1=StateNo=5110
v=50
value=GetHitVar(recovertime)
[Statedef -1]
[State -1, inert]
type=Null
trigger1=0
