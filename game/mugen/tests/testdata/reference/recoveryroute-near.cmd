[Command]
name="recovery"
command=/x
time=1
buffer.time=1
[Statedef -3]
[State -3, observed]
type=VarSet
trigger1=TeamSide=1
fv=0
value=StateNo
[State -3, observed]
type=VarSet
trigger1=TeamSide=1
fv=1
value=CanRecover
[State -3, observed]
type=VarSet
trigger1=TeamSide=1
fv=2
value=Pos Y
[State -3, observed]
type=VarSet
trigger1=TeamSide=1
fv=3
value=Vel Y
[Statedef -1]
[State -1, inert]
type=Null
trigger1=0
