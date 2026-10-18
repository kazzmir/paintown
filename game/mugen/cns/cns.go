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
	Power             int
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
	LanguageVersion      string
	Line                 int
	Name                 string
	Type                 string
	Persistent           int
	IgnoreHitPause       int
	PersistentSource     string
	IgnoreHitPauseSource string
	Triggers             []string
	// The rest of the keys we'll store loosely for now, or just provide access to the raw attributes
	// In a full implementation, each controller type (ChangeState, HitDef, etc) has specific fields.
	// We'll store them as a string map to avoid creating 50 struct types right now.
	Params map[string]string
}

type HitDef struct {
	ID            int
	ChainID       *int
	NoChainID     []int
	AirJuggle     int
	MinDist       []float64
	MaxDist       []float64
	SnapTime      int
	HitOnce       bool
	AffectTeam    string
	Priority      int
	PriorityClass string
	// Nil preserves the ordinary one-hit multiplicity for programmatic definitions.
	NumHits *int
	// Nil preserves the native contact defaults: attacker one, receiver zero.
	P1SprPriority *int
	P2SprPriority *int
	// Nil means no custom transition. Keep explicit values separate so a
	// zero-valued, programmatic HitDef does not change either actor's state.
	P1StateNo          *int
	P2StateNo          *int
	P2GetP1State       bool
	P2Facing           int
	P1Facing           int
	P1GetP2Facing      int
	Attr               string
	HitFlag            string
	HitFlagSpecified   bool
	GuardFlag          string
	AnimType           string
	AirAnimType        string
	FallAnimType       string
	Damage             []int // [hit, guard]
	PauseTime          []int // [hit, guard]
	GuardPauseTime     []int // [attacker pause, defender shake]
	ShakeTime          int
	SparkNo            int
	GuardSparkNo       int
	SparkOwn           bool
	GuardSparkOwn      bool
	SparkXY            []int
	HitSound           []int
	GuardSound         []int
	HitSoundOwn        bool
	GuardSoundOwn      bool
	GroundType         string
	AirType            string
	GroundSlide        int
	GroundHitTime      int
	GuardSlide         int
	GuardHitTime       int
	GuardCtrlTime      int
	AirGuardCtrlTime   int
	GuardVel           float64
	GuardDist          float64
	AirHitTime         int
	GroundVel          []float64
	AirVel             []float64
	AirGuardVel        []float64
	GroundCornerPush   float64
	AirCornerPush      float64
	DownCornerPush     float64
	GuardCornerPush    float64
	AirGuardCornerPush float64
	DownVel            []float64
	DownHitTime        int
	DownBounce         bool
	GetPower           []int
	GivePower          []int
	PalFXTime          int
	PalFXMul           [3]int
	PalFXAdd           [3]int
	Fall               int
	AirFall            int
	ForceNoFall        bool
	ForceStand         *bool // nil: derive from the unscaled ground Y velocity
	// Nil kill flags preserve the native true default.
	Kill            *bool
	GuardKill       *bool
	FallKill        *bool
	FallDamage      int
	FallVel         []float64
	FallXUnchanged  bool
	FallRecover     bool
	FallRecoverTime int
	YAccel          float64
}

type StateDef struct {
	LanguageVersion string
	Source          string
	Line            int
	// Parameters retains entry expressions; numeric fields are legacy accessors.
	Parameters  map[string]string
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
	// ConstantFields retains numeric fields not represented by legacy structs,
	// including the 1.0/1.1 recovery/friction additions and tuple components.
	ConstantFields map[string][]float64
}

func NewStateDef(id int) *StateDef {
	return &StateDef{ID: id, Type: "S", MoveType: "I", Physics: "N", Anim: -1, Ctrl: -1, SprPriority: -255, Parameters: make(map[string]string)}
}

// SetLanguageVersion binds character-owned definitions before compilation.
// An empty version is reserved for unbound pure-engine programs (target 1.1).
func (c *CNS) SetLanguageVersion(version string) {
	for _, state := range c.States {
		state.LanguageVersion = version
	}
}

func Parse(r io.Reader) (*CNS, error) {
	ast, err := parsers.Parse(r)
	if err != nil {
		return nil, err
	}

	return ParseFromAST(ast)
}

// ParseNamed retains the source filename for later compile/runtime diagnostics.
func ParseNamed(r io.Reader, name string) (*CNS, error) {
	parsed, err := Parse(r)
	if err != nil {
		return nil, fmt.Errorf("%s: %w", name, err)
	}
	for _, state := range parsed.States {
		state.Source = name
	}
	return parsed, nil
}

func ParseFromAST(ast *parsers.File) (*CNS, error) {
	cns := &CNS{
		States: make(map[int]*StateDef),
		Data:   Data{Power: 3000},
		Movement: Movement{
			StandFriction:  0.85,
			CrouchFriction: 0.82,
			YAccel:         0.44,
		},
	}
	if err := cns.readConstantFields(ast); err != nil {
		return nil, err
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
				if key == "power" {
					cns.Data.Power = getAsInt(attr.Value)
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

			currentState = NewStateDef(stateID)
			currentState.Line = section.Line

			// Extract name if present: [Statedef 0, Standing]
			if idx := strings.Index(section.Name, ","); idx != -1 {
				currentState.Name = strings.TrimSpace(section.Name[idx+1:])
			}

			// First definition wins. Keep parsing a duplicate into its own
			// detached block so none of its controllers leak into the first.
			if _, exists := cns.States[stateID]; !exists {
				cns.States[stateID] = currentState
			}

			for _, attr := range section.Attributes {
				key := strings.ToLower(attr.ID.String())
				currentState.Parameters[key] = attributeExpression(attr)
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
			// Controller header numbers are labels, not state references.
			// Before any Statedef there is no block to attach them to.
			if currentState == nil {
				continue
			}

			ctrl := StateController{
				Line:           section.Line,
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
					ctrl.PersistentSource = attributeExpression(attr)
				} else if key == "ignorehitpause" {
					ctrl.IgnoreHitPause = getAsInt(attr.Value)
					ctrl.IgnoreHitPauseSource = attributeExpression(attr)
				} else if strings.HasPrefix(key, "trigger") {
					strVal := attributeExpression(attr)
					ctrl.Triggers = append(ctrl.Triggers, fmt.Sprintf("%s = %s", attr.ID.String(), strVal))
				} else {
					// Beta uses the first ordinary parameter. Repeated trigger
					// lines above remain separate conditions in their group.
					if _, present := ctrl.Params[key]; !present {
						ctrl.Params[key] = attributeExpression(attr)
					}
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
		return strconv.Quote(sv.Val)
	}
	return getAsString(v)
}

func attributeExpression(attr *parsers.Attribute) string {
	if attr.RawValue != "" {
		return attr.RawValue
	}
	return renderRawValue(attr.Value)
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
	hd := HitDef{
		ID:              0,
		Priority:        4,
		PriorityClass:   "Hit",
		PalFXMul:        [3]int{256, 256, 256},
		FallRecover:     true,
		FallRecoverTime: 4,
		FallXUnchanged:  true,
		HitFlag:         "MAF", AnimType: "Light", GroundType: "High", AirHitTime: 20, DownHitTime: 20,
	}
	for key, destination := range map[string]**bool{"kill": &hd.Kill, "guard.kill": &hd.GuardKill, "fall.kill": &hd.FallKill} {
		if raw, present := params[key]; present {
			number, _ := strconv.Atoi(raw)
			value := number != 0
			*destination = &value
		}
	}
	for key, destination := range map[string]**int{"p1stateno": &hd.P1StateNo, "p2stateno": &hd.P2StateNo} {
		if raw, present := params[key]; present {
			value, _ := strconv.Atoi(raw)
			*destination = &value
		}
	}
	hd.P2GetP1State = hd.P2StateNo != nil
	if raw, present := params["p2getp1state"]; present {
		value, _ := strconv.Atoi(raw)
		hd.P2GetP1State = value != 0
	}
	if raw, present := params["p2facing"]; present {
		hd.P2Facing, _ = strconv.Atoi(raw)
	}
	if raw, present := params["p1facing"]; present {
		hd.P1Facing, _ = strconv.Atoi(raw)
	}
	if raw, present := params["p1getp2facing"]; present {
		hd.P1GetP2Facing, _ = strconv.Atoi(raw)
	}
	if v, ok := params["id"]; ok {
		hd.ID, _ = strconv.Atoi(v)
		hd.ID = max(0, hd.ID)
	}
	if raw, present := params["chainid"]; present {
		value, _ := strconv.Atoi(raw)
		hd.ChainID = &value
	}
	if raw, present := params["nochainid"]; present {
		hd.NoChainID = ParseIntList(raw)
	}
	hd.AirJuggle, _ = strconv.Atoi(params["air.juggle"])
	hd.PalFXTime, _ = strconv.Atoi(params["palfx.time"])
	for key, dest := range map[string]*[3]int{"palfx.mul": &hd.PalFXMul, "palfx.add": &hd.PalFXAdd} {
		if raw, present := params[key]; present {
			values := ParseIntList(raw)
			copy(dest[:], values)
		}
	}
	if raw, present := params["mindist"]; present {
		hd.MinDist = ParseFloatList(raw)
	}
	if raw, present := params["maxdist"]; present {
		hd.MaxDist = ParseFloatList(raw)
	}
	if raw, present := params["snap"]; present {
		values := ParseFloatList(raw)
		if len(values) > 2 {
			hd.SnapTime = int(values[2])
			values = values[:2]
		}
		hd.MinDist, hd.MaxDist = values, append([]float64(nil), values...)
	}
	if raw, present := params["snap.time"]; present {
		hd.SnapTime, _ = strconv.Atoi(raw)
	}
	if v, ok := params["attr"]; ok {
		hd.Attr = v
	}
	_, attackClass, _ := strings.Cut(hd.Attr, ",")
	hd.HitOnce = strings.HasSuffix(strings.ToUpper(strings.TrimSpace(attackClass)), "T")
	if raw, present := params["hitonce"]; present {
		value, _ := strconv.Atoi(raw)
		hd.HitOnce = value != 0
	}
	hd.AffectTeam = "E"
	if raw := strings.ToUpper(strings.TrimSpace(params["affectteam"])); raw != "" {
		hd.AffectTeam = raw[:1]
	}
	if raw, present := params["priority"]; present {
		parts := strings.SplitN(raw, ",", 2)
		hd.Priority, _ = strconv.Atoi(strings.TrimSpace(parts[0]))
		hd.Priority = max(0, hd.Priority)
		if len(parts) == 2 {
			kind := strings.ToUpper(strings.TrimSpace(parts[1]))
			if strings.HasPrefix(kind, "D") {
				hd.PriorityClass = "Dodge"
			} else if strings.HasPrefix(kind, "M") {
				hd.PriorityClass = "Miss"
			}
		}
	}
	if raw, present := params["numhits"]; present {
		value, _ := strconv.Atoi(raw)
		hd.NumHits = &value
	}
	for key, destination := range map[string]**int{"p1sprpriority": &hd.P1SprPriority, "p2sprpriority": &hd.P2SprPriority} {
		if raw, present := params[key]; present {
			value, _ := strconv.Atoi(raw)
			*destination = &value
		}
	}
	if v, ok := params["hitflag"]; ok {
		hd.HitFlag = v
		hd.HitFlagSpecified = true
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
		hd.GuardPauseTime = ParseIntList(v)
	}
	if v, ok := params["sparkno"]; ok {
		v, hd.SparkOwn = literalResourceBank(v)
		hd.SparkNo, _ = strconv.Atoi(v)
	}
	if v, ok := params["guard.sparkno"]; ok {
		v, hd.GuardSparkOwn = literalResourceBank(v)
		hd.GuardSparkNo, _ = strconv.Atoi(v)
	}
	if v, ok := params["sparkxy"]; ok {
		hd.SparkXY = ParseIntList(v)
	}
	if v, ok := params["hitsound"]; ok {
		v, hd.HitSoundOwn = literalResourceBank(v)
		hd.HitSound = ParseIntList(v)
	}
	if v, ok := params["guardsound"]; ok {
		v, hd.GuardSoundOwn = literalResourceBank(v)
		hd.GuardSound = ParseIntList(v)
	}
	if v, ok := params["ground.type"]; ok {
		hd.GroundType = v
	}
	hd.AirType = hd.GroundType
	if v, ok := params["air.type"]; ok {
		hd.AirType = v
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
	for key, value := range map[string]*float64{
		"down.cornerpush.veloff":     &hd.DownCornerPush,
		"guard.cornerpush.veloff":    &hd.GuardCornerPush,
		"airguard.cornerpush.veloff": &hd.AirGuardCornerPush,
	} {
		if raw, present := params[key]; present {
			*value, _ = strconv.ParseFloat(raw, 64)
		}
	}
	if v, ok := params["down.velocity"]; ok {
		hd.DownVel = ParseFloatList(v)
	}
	if v, ok := params["down.hittime"]; ok {
		hd.DownHitTime, _ = strconv.Atoi(v)
	}
	if v, ok := params["down.bounce"]; ok {
		value, _ := strconv.Atoi(v)
		hd.DownBounce = value != 0
	}
	if v, ok := params["getpower"]; ok {
		hd.GetPower = ParseIntList(v)
	}
	if v, ok := params["givepower"]; ok {
		hd.GivePower = ParseIntList(v)
	}

	if v, ok := params["fall"]; ok {
		hd.Fall, _ = strconv.Atoi(v)
	}
	hd.AirFall = hd.Fall
	if v, ok := params["air.fall"]; ok {
		hd.AirFall, _ = strconv.Atoi(v)
	}
	if raw, present := params["forcenofall"]; present {
		value, _ := strconv.Atoi(raw)
		hd.ForceNoFall = value != 0
	}
	if raw, present := params["forcestand"]; present {
		value, _ := strconv.Atoi(raw)
		force := value != 0
		hd.ForceStand = &force
	}
	if v, ok := params["fall.damage"]; ok {
		hd.FallDamage, _ = strconv.Atoi(v)
	}
	if v, ok := params["fall.xvelocity"]; ok {
		hd.FallVel = ParseFloatList(v)
		hd.FallXUnchanged = false
	}
	if v, ok := params["fall.yvelocity"]; ok {
		yvels := ParseFloatList(v)
		if len(yvels) > 0 {
			if len(hd.FallVel) == 0 {
				hd.FallVel = []float64{0, yvels[0]}
			} else if len(hd.FallVel) == 1 {
				hd.FallVel = append(hd.FallVel, yvels[0])
			} else {
				hd.FallVel[1] = yvels[0]
			}
		}
	}
	if v, ok := params["fall.recover"]; ok {
		val, _ := strconv.Atoi(v)
		hd.FallRecover = (val != 0)
	}
	if v, ok := params["fall.recovertime"]; ok {
		if value, err := strconv.Atoi(v); err == nil && value >= 0 {
			hd.FallRecoverTime = value
		}
	}
	if v, ok := params["yaccel"]; ok {
		hd.YAccel, _ = strconv.ParseFloat(v, 64)
	}
	if _, ok := params["guard.pausetime"]; !ok {
		hd.GuardPauseTime = append([]int(nil), hd.PauseTime...)
	}
	if _, ok := params["guard.hittime"]; !ok {
		hd.GuardHitTime = hd.GroundHitTime
	}
	if _, ok := params["guard.slidetime"]; !ok {
		// Beta 1 inherits ground.slidetime, despite the manual saying
		// guard.hittime. Native guard-defaults pins the distinction.
		hd.GuardSlide = hd.GroundSlide
	}
	hd.GuardCtrlTime = hd.GuardSlide
	if value, ok := params["guard.ctrltime"]; ok {
		hd.GuardCtrlTime, _ = strconv.Atoi(value)
	}
	hd.AirGuardCtrlTime = hd.GuardCtrlTime
	if value, ok := params["airguard.ctrltime"]; ok {
		hd.AirGuardCtrlTime, _ = strconv.Atoi(value)
	}
	if len(hd.GroundVel) > 0 {
		hd.GuardVel = hd.GroundVel[0]
	}
	if value, ok := params["guard.velocity"]; ok {
		hd.GuardVel, _ = strconv.ParseFloat(value, 64)
	}
	if value, ok := params["guard.dist"]; ok {
		hd.GuardDist, _ = strconv.ParseFloat(value, 64)
	}
	// Each omitted air-guard component inherits separately; an explicit
	// single X must not replace the inherited Y with an artificial zero.
	if len(hd.AirGuardVel) == 0 {
		x := 0.0
		if len(hd.AirVel) > 0 {
			x = hd.AirVel[0] * 1.5
		}
		hd.AirGuardVel = []float64{x}
	}
	if len(hd.AirGuardVel) == 1 {
		y := 0.0
		if len(hd.AirVel) > 1 {
			y = hd.AirVel[1] / 2
		}
		hd.AirGuardVel = append(hd.AirGuardVel, y)
	}
	// Downed-hit components inherit unscaled air velocity independently.
	if len(hd.DownVel) == 0 {
		x := 0.0
		if len(hd.AirVel) > 0 {
			x = hd.AirVel[0]
		}
		hd.DownVel = []float64{x}
	}
	if len(hd.DownVel) == 1 {
		y := 0.0
		if len(hd.AirVel) > 1 {
			y = hd.AirVel[1]
		}
		hd.DownVel = append(hd.DownVel, y)
	}
	if _, ok := params["air.animtype"]; !ok {
		hd.AirAnimType = hd.AnimType
	}
	hd.FallAnimType = "Back"
	if strings.EqualFold(hd.AirAnimType, "Up") || strings.EqualFold(hd.AirAnimType, "DiagUp") {
		hd.FallAnimType = hd.AirAnimType
	}
	if raw, present := params["fall.animtype"]; present {
		hd.FallAnimType = raw
	}

	return hd
}

// Compiled controllers evaluate expressions before this literal compatibility
// parser. S selects the attacker's own bank; F or omission selects common.
func literalResourceBank(value string) (string, bool) {
	value = strings.TrimSpace(value)
	if len(value) > 0 && (value[0] == 'S' || value[0] == 's') {
		return value[1:], true
	}
	if len(value) > 0 && (value[0] == 'F' || value[0] == 'f') {
		return value[1:], false
	}
	return value, false
}
