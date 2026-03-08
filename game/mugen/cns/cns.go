package cns

import (
	"fmt"
	"io"
	"strconv"
	"strings"

	"github.com/kazzmir/paintown/game/mugen/parsers"
)

type Data struct {
	Life              int
	Attack            int
	Defence           int
	FallDefenceUp     int
	LiedownTime       int
	AirJuggle         int
	SparkNo           int
	GuardSparkNo      int
	KOEcho            int
	Volume            int
	IntPersistIndex   int
	FloatPersistIndex int
}

type Size struct {
	XScale         float64
	YScale         float64
	GroundBack     int
	GroundFront    int
	AirBack        int
	AirFront       int
	Height         int
	AttackDist     int
	ProjAttackDist int
	ProjDoScale    int
	HeadPos        []int
	MidPos         []int
	ShadowOffset   int
	DrawOffset     []int
}

type Velocity struct {
	WalkFwd     float64
	WalkBack    float64
	RunFwd      []float64
	RunBack     []float64
	JumpNeu     []float64
	JumpBack    float64
	JumpFwd     float64
	RunJumpBack []float64
	RunJumpFwd  []float64
	AirJumpNeu  []float64
	AirJumpBack float64
	AirJumpFwd  float64
}

type Movement struct {
	AirJumpNum     int
	AirJumpHeight  int
	YAccel         float64
	StandFriction  float64
	CrouchFriction float64
}

type StateController struct {
	Name           string
	Type           string
	Persistent     int
	IgnoreHitPause int
	Triggers       []string
	// The rest of the keys we'll store loosely for now, or just provide access to the raw attributes
	// In a full implementation, each controller type (ChangeState, HitDef, etc) has specific fields.
	// We'll store them as a string map to avoid creating 50 struct types right now.
	Params map[string]string
}

type HitDef struct {
	Attr             string
	HitFlag          string
	GuardFlag        string
	AnimType         string
	AirAnimType      string
	Damage           []int // [hit, guard]
	PauseTime        []int // [hit, guard]
	ShakeTime        int
	SparkNo          int
	GuardSparkNo     int
	SparkXY          []int
	HitSound         []int
	GuardSound       []int
	GroundType       string
	GroundSlide      int
	GroundHitTime    int
	GuardSlide       int
	GuardHitTime     int
	AirHitTime       int
	GroundVel        []float64
	AirVel           []float64
	AirGuardVel      []float64
	GroundCornerPush float64
	AirCornerPush    float64
	DownVel          []float64
	DownHitTime      int
	GetPower         []int
	GivePower        []int
	Palfrm_Time      int
	Palfrm_Mul       []float64
	Palfrm_Add       []int
}

type StateDef struct {
	ID          int
	Name        string
	Type        string
	MoveType    string
	Physics     string
	Anim        int
	Ctrl        int
	VelSet      []float64
	PowerAdd    int
	Juggle      int
	FaceP2      int
	HitDefault  int
	SprPriority int

	Controllers []StateController
}

type CNS struct {
	Data     Data
	Size     Size
	Velocity Velocity
	Movement Movement
	States   map[int]*StateDef
}

func Parse(r io.Reader) (*CNS, error) {
	ast, err := parsers.Parse(r)
	if err != nil {
		return nil, err
	}

	return ParseFromAST(ast)
}

func ParseFromAST(ast *parsers.File) (*CNS, error) {
	cns := &CNS{
		States: make(map[int]*StateDef),
		Movement: Movement{
			StandFriction:  0.85,
			CrouchFriction: 0.82,
			YAccel:         0.44,
		},
	}

	var currentState *StateDef

	for _, section := range ast.Sections {
		secName := strings.ToLower(strings.TrimSpace(section.Name))

		if secName == "data" {
			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())
				if key == "life" {
					cns.Data.Life = getAsInt(attr.Value)
				}
				if key == "attack" {
					cns.Data.Attack = getAsInt(attr.Value)
				}
				if key == "defence" {
					cns.Data.Defence = getAsInt(attr.Value)
				}
				if key == "fall.defence_up" {
					cns.Data.FallDefenceUp = getAsInt(attr.Value)
				}
				if key == "liedown.time" {
					cns.Data.LiedownTime = getAsInt(attr.Value)
				}
				if key == "airjuggle" {
					cns.Data.AirJuggle = getAsInt(attr.Value)
				}
				if key == "sparkno" {
					cns.Data.SparkNo = getAsInt(attr.Value)
				}
				if key == "guard.sparkno" {
					cns.Data.GuardSparkNo = getAsInt(attr.Value)
				}
				if key == "ko.echo" {
					cns.Data.KOEcho = getAsInt(attr.Value)
				}
				if key == "volume" {
					cns.Data.Volume = getAsInt(attr.Value)
				}
				if key == "intpersistindex" {
					cns.Data.IntPersistIndex = getAsInt(attr.Value)
				}
				if key == "floatpersistindex" {
					cns.Data.FloatPersistIndex = getAsInt(attr.Value)
				}
			}
		} else if secName == "size" {
			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())
				if key == "xscale" {
					cns.Size.XScale = getAsFloat(attr.Value)
				}
				if key == "yscale" {
					cns.Size.YScale = getAsFloat(attr.Value)
				}
				if key == "ground.back" {
					cns.Size.GroundBack = getAsInt(attr.Value)
				}
				if key == "ground.front" {
					cns.Size.GroundFront = getAsInt(attr.Value)
				}
				if key == "air.back" {
					cns.Size.AirBack = getAsInt(attr.Value)
				}
				if key == "air.front" {
					cns.Size.AirFront = getAsInt(attr.Value)
				}
				if key == "height" {
					cns.Size.Height = getAsInt(attr.Value)
				}
				if key == "attack.dist" {
					cns.Size.AttackDist = getAsInt(attr.Value)
				}
				if key == "proj.attack.dist" {
					cns.Size.ProjAttackDist = getAsInt(attr.Value)
				}
				if key == "proj.doscale" {
					cns.Size.ProjDoScale = getAsInt(attr.Value)
				}
				if key == "head.pos" {
					cns.Size.HeadPos = getAsIntArray(attr.Value)
				}
				if key == "mid.pos" {
					cns.Size.MidPos = getAsIntArray(attr.Value)
				}
				if key == "shadowoffset" {
					cns.Size.ShadowOffset = getAsInt(attr.Value)
				}
				if key == "draw.offset" {
					cns.Size.DrawOffset = getAsIntArray(attr.Value)
				}
			}
		} else if secName == "velocity" {
			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())
				if key == "walk.fwd" {
					cns.Velocity.WalkFwd = getAsFloat(attr.Value)
				}
				if key == "walk.back" {
					cns.Velocity.WalkBack = getAsFloat(attr.Value)
				}
				if key == "run.fwd" {
					cns.Velocity.RunFwd = getAsFloatArray(attr.Value)
				}
				if key == "run.back" {
					cns.Velocity.RunBack = getAsFloatArray(attr.Value)
				}
				if key == "jump.neu" {
					cns.Velocity.JumpNeu = getAsFloatArray(attr.Value)
				}
				if key == "jump.back" {
					cns.Velocity.JumpBack = getAsFloat(attr.Value)
				}
				if key == "jump.fwd" {
					cns.Velocity.JumpFwd = getAsFloat(attr.Value)
				}
				if key == "runjump.back" {
					cns.Velocity.RunJumpBack = getAsFloatArray(attr.Value)
				}
				if key == "runjump.fwd" {
					cns.Velocity.RunJumpFwd = getAsFloatArray(attr.Value)
				}
				if key == "airjump.neu" {
					cns.Velocity.AirJumpNeu = getAsFloatArray(attr.Value)
				}
				if key == "airjump.back" {
					cns.Velocity.AirJumpBack = getAsFloat(attr.Value)
				}
				if key == "airjump.fwd" {
					cns.Velocity.AirJumpFwd = getAsFloat(attr.Value)
				}
			}
		} else if secName == "movement" {
			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())
				if key == "airjump.num" {
					cns.Movement.AirJumpNum = getAsInt(attr.Value)
				}
				if key == "airjump.height" {
					cns.Movement.AirJumpHeight = getAsInt(attr.Value)
				}
				if key == "yaccel" {
					cns.Movement.YAccel = getAsFloat(attr.Value)
				}
				if key == "stand.friction" {
					cns.Movement.StandFriction = getAsFloat(attr.Value)
				}
				if key == "crouch.friction" {
					cns.Movement.CrouchFriction = getAsFloat(attr.Value)
				}
			}
		} else if fields := strings.Fields(strings.ReplaceAll(secName, ",", " ")); len(fields) >= 2 && fields[0] == "statedef" {
			idPart := fields[1]
			stateID, err := strconv.Atoi(idPart)
			if err != nil {
				continue
			}

			currentState = &StateDef{
				ID:          stateID,
				Controllers: make([]StateController, 0),
				Anim:        -1, // Default to "no change"
				Ctrl:        -1, // Default to "no change"
			}

			// Extract name if present: [Statedef 0, Standing]
			if idx := strings.Index(section.Name, ","); idx != -1 {
				currentState.Name = strings.TrimSpace(section.Name[idx+1:])
			}

			cns.States[stateID] = currentState

			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())
				if key == "type" {
					currentState.Type = getAsString(attr.Value)
				}
				if key == "movetype" {
					currentState.MoveType = getAsString(attr.Value)
				}
				if key == "physics" {
					currentState.Physics = getAsString(attr.Value)
				}
				if key == "anim" {
					currentState.Anim = getAsInt(attr.Value)
				}
				if key == "ctrl" {
					currentState.Ctrl = getAsInt(attr.Value)
				}
				if key == "velset" {
					currentState.VelSet = getAsFloatArray(attr.Value)
				}
				if key == "poweradd" {
					currentState.PowerAdd = getAsInt(attr.Value)
				}
				if key == "juggle" {
					currentState.Juggle = getAsInt(attr.Value)
				}
				if key == "facep2" {
					currentState.FaceP2 = getAsInt(attr.Value)
				}
				if key == "hitdefpersist" { /* ignore for now */
				}
				if key == "movehitpersist" { /* ignore for now */
				}
				if key == "hitcountpersist" { /* ignore for now */
				}
				if key == "sprpriority" {
					currentState.SprPriority = getAsInt(attr.Value)
				}
			}
		} else if strings.HasPrefix(secName, "state ") {
			stateNoStr := strings.TrimSpace(strings.TrimPrefix(secName, "state "))
			// Remove comments and metadata from the header line
			if idx := strings.Index(stateNoStr, ","); idx != -1 {
				stateNoStr = strings.TrimSpace(stateNoStr[:idx])
			}
			stateNo, err := strconv.Atoi(stateNoStr)

			if err == nil {
				// If it's a negative state or we already have it, use it.
				// Otherwise, if currentState is nil, we might need to handle this as a virtual state.
				if stateNo < 0 {
					if _, ok := cns.States[stateNo]; !ok {
						cns.States[stateNo] = &StateDef{
							ID:          stateNo,
							Controllers: make([]StateController, 0),
						}
					}
					currentState = cns.States[stateNo]
				}
			}

			if currentState == nil {
				// Controller without a StateDef? Valid in MUGEN maybe for injected common1 states but we skip for strictness
				continue
			}

			ctrl := StateController{
				Triggers:       make([]string, 0),
				Params:         make(map[string]string),
				Persistent:     1, // Default is 1
				IgnoreHitPause: 0, // Default is 0
			}

			parts := strings.SplitN(section.Name, ",", 2)
			if len(parts) > 1 {
				ctrl.Name = strings.TrimSpace(parts[1])
			}

			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())

				if key == "type" {
					ctrl.Type = getAsString(attr.Value)
				} else if key == "persistent" {
					ctrl.Persistent = getAsInt(attr.Value)
				} else if key == "ignorehitpause" {
					ctrl.IgnoreHitPause = getAsInt(attr.Value)
				} else if strings.HasPrefix(key, "trigger") {
					strVal := renderRawValue(attr.Value)
					ctrl.Triggers = append(ctrl.Triggers, fmt.Sprintf("%s = %s", attr.ID.String(), strVal))
				} else {
					ctrl.Params[key] = renderRawValue(attr.Value)
				}
			}

			currentState.Controllers = append(currentState.Controllers, ctrl)
		}
	}

	return cns, nil
}

func getAsInt(v parsers.Value) int {
	if nv, ok := v.(parsers.NumberValue); ok {
		return int(nv.Val)
	}
	if kv, ok := v.(parsers.KeywordValue); ok {
		val, _ := strconv.Atoi(strings.TrimSpace(kv.Val))
		return val
	}
	if sv, ok := v.(parsers.StringValue); ok {
		val, _ := strconv.Atoi(strings.TrimSpace(sv.Val))
		return val
	}
	return 0
}

func getAsFloat(v parsers.Value) float64 {
	if nv, ok := v.(parsers.NumberValue); ok {
		return nv.Val
	}
	if kv, ok := v.(parsers.KeywordValue); ok {
		val, _ := strconv.ParseFloat(strings.TrimSpace(kv.Val), 64)
		return val
	}
	if sv, ok := v.(parsers.StringValue); ok {
		val, _ := strconv.ParseFloat(strings.TrimSpace(sv.Val), 64)
		return val
	}
	return 0
}

func getAsString(v parsers.Value) string {
	if kv, ok := v.(parsers.KeywordValue); ok {
		return strings.TrimSpace(kv.Val)
	}
	if sv, ok := v.(parsers.StringValue); ok {
		return strings.TrimSpace(sv.Val)
	}
	// Number representation fallback
	if nv, ok := v.(parsers.NumberValue); ok {
		return fmt.Sprintf("%v", nv.Val)
	}
	return ""
}

func getAsIntArray(v parsers.Value) []int {
	if list, ok := v.(*parsers.ValueList); ok {
		var arr []int
		for _, item := range list.Values {
			arr = append(arr, getAsInt(item))
		}
		return arr
	}
	return []int{getAsInt(v)}
}

func getAsFloatArray(v parsers.Value) []float64 {
	if list, ok := v.(*parsers.ValueList); ok {
		var arr []float64
		for _, item := range list.Values {
			arr = append(arr, getAsFloat(item))
		}
		return arr
	}
	return []float64{getAsFloat(v)}
}

func renderRawValue(v parsers.Value) string {
	if list, ok := v.(*parsers.ValueList); ok {
		var parts []string
		for _, item := range list.Values {
			parts = append(parts, renderRawValue(item))
		}
		return strings.Join(parts, ", ")
	}
	if sv, ok := v.(parsers.StringValue); ok {
		return sv.Val
	}
	return getAsString(v)
}

func ParseIntList(s string) []int {
	parts := strings.Split(s, ",")
	var res []int
	for _, p := range parts {
		val, _ := strconv.Atoi(strings.TrimSpace(p))
		res = append(res, val)
	}
	return res
}

func ParseFloatList(s string) []float64 {
	parts := strings.Split(s, ",")
	var res []float64
	for _, p := range parts {
		val, _ := strconv.ParseFloat(strings.TrimSpace(p), 64)
		res = append(res, val)
	}
	return res
}

func ParseHitDef(params map[string]string) HitDef {
	hd := HitDef{}
	if v, ok := params["attr"]; ok {
		hd.Attr = v
	}
	if v, ok := params["hitflag"]; ok {
		hd.HitFlag = v
	}
	if v, ok := params["guardflag"]; ok {
		hd.GuardFlag = v
	}
	if v, ok := params["animtype"]; ok {
		hd.AnimType = v
	}
	if v, ok := params["air.animtype"]; ok {
		hd.AirAnimType = v
	}
	if v, ok := params["damage"]; ok {
		hd.Damage = ParseIntList(v)
	}
	if v, ok := params["pausetime"]; ok {
		hd.PauseTime = ParseIntList(v)
	}
	if v, ok := params["guard.pausetime"]; ok {
		// If guard.pausetime is missing, it often defaults to hit pausetime or similar
		// But let's just parse it if it exists.
		p := ParseIntList(v)
		if len(hd.PauseTime) >= 1 {
			if len(p) == 1 {
				hd.PauseTime = []int{hd.PauseTime[0], p[0]}
			} else if len(p) >= 2 {
				hd.PauseTime = p[:2]
			}
		}
	}
	if v, ok := params["sparkno"]; ok {
		hd.SparkNo, _ = strconv.Atoi(v)
	}
	if v, ok := params["guard.sparkno"]; ok {
		hd.GuardSparkNo, _ = strconv.Atoi(v)
	}
	if v, ok := params["sparkxy"]; ok {
		hd.SparkXY = ParseIntList(v)
	}
	if v, ok := params["hitsound"]; ok {
		hd.HitSound = ParseIntList(v)
	}
	if v, ok := params["guardsound"]; ok {
		hd.GuardSound = ParseIntList(v)
	}
	if v, ok := params["ground.type"]; ok {
		hd.GroundType = v
	}
	if v, ok := params["ground.slidetime"]; ok {
		hd.GroundSlide, _ = strconv.Atoi(v)
	}
	if v, ok := params["ground.hittime"]; ok {
		hd.GroundHitTime, _ = strconv.Atoi(v)
	}
	if v, ok := params["guard.slidetime"]; ok {
		hd.GuardSlide, _ = strconv.Atoi(v)
	}
	if v, ok := params["guard.hittime"]; ok {
		hd.GuardHitTime, _ = strconv.Atoi(v)
	}
	if v, ok := params["air.hittime"]; ok {
		hd.AirHitTime, _ = strconv.Atoi(v)
	}
	if v, ok := params["ground.velocity"]; ok {
		hd.GroundVel = ParseFloatList(v)
	}
	if v, ok := params["air.velocity"]; ok {
		hd.AirVel = ParseFloatList(v)
	}
	if v, ok := params["airguard.velocity"]; ok {
		hd.AirGuardVel = ParseFloatList(v)
	}
	if v, ok := params["ground.cornerpush.veloff"]; ok {
		hd.GroundCornerPush, _ = strconv.ParseFloat(v, 64)
	}
	if v, ok := params["air.cornerpush.veloff"]; ok {
		hd.AirCornerPush, _ = strconv.ParseFloat(v, 64)
	}
	if v, ok := params["down.velocity"]; ok {
		hd.DownVel = ParseFloatList(v)
	}
	if v, ok := params["down.hittime"]; ok {
		hd.DownHitTime, _ = strconv.Atoi(v)
	}
	if v, ok := params["getpower"]; ok {
		hd.GetPower = ParseIntList(v)
	}
	if v, ok := params["givepower"]; ok {
		hd.GivePower = ParseIntList(v)
	}

	return hd
}
