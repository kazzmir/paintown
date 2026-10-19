[Statedef -1]
[State -1, order]
type=VarSet
triggerall=var(58)=0
trigger1=StateNo=0 || StateNo=10 || StateNo=20
v=0
value=var(0)*10+1
[State -1, transition]
type=ChangeState
triggerall=var(58)=0
trigger1=StateNo=20
value=30
[State -1, after transition]
type=VarAdd
triggerall=var(58)=0
trigger1=StateNo=0 || StateNo=10 || StateNo=20 || StateNo=30
v=3
value=1
