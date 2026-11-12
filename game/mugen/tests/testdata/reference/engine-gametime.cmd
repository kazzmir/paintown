[Statedef -2]
[State -2, clock dump]
type=DisplayToClipboard
trigger1=1
text="P0:%d,%d,%d,%d,%d,%d\n"
params=Var(0),Var(1),Var(2),Var(3),Var(4),Var(5)
[State -2, clock dump]
type=AppendToClipboard
trigger1=1
text="P6:%d,%d,%d,%d,%d,%d\n"
params=Var(6),Var(7),Var(8),Var(9),Var(10),Var(11)
[Statedef -1]
[State -1, inert]
type=Null
trigger1=0
