[Statedef -2]
[State -2, live KO boundary]
type=DisplayToClipboard
trigger1=1
text="P0:%.9g,%.9g,%.9g,%.9g,%.9g,%.9g\n"
params=GameTime,RoundState,Time,(EnemyNear,Life),(EnemyNear,StateNo),(EnemyNear,Alive)
[State -2, live counters]
type=AppendToClipboard
trigger1=1
text="P6:%.9g,%.9g,%.9g,%.9g,%.9g,%.9g\n"
params=Var(0),(EnemyNear,Var(0)),(EnemyNear,Time),NumExplod,NumHelper,RoundNo
[Statedef -1]
[State -1, inert]
type=Null
trigger1=0
