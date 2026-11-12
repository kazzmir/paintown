[Statedef -1]
[State -1, enforce measured entry after the engine's intro setup]
type=ChangeState
triggerall=roundstate=2
triggerall=ctrl
trigger1=var(3)=0
trigger1=var(3):=1
value=5900
[State -1, control]
type=VarSet
trigger1=1
v=0
value=ctrl
[State -1, animation]
type=VarSet
trigger1=1
v=1
value=anim
[State -1, state]
type=VarSet
trigger1=1
v=2
value=stateno
