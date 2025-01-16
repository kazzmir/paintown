package main

import (
    "log"
    "os"
    "fmt"
    "io"
    "image"
    "image/png"
    "bufio"
    "path/filepath"

    "github.com/kazzmir/paintown/game/lib/coroutine"

    "github.com/hajimehoshi/ebiten/v2"
    "github.com/hajimehoshi/ebiten/v2/inpututil"
    "github.com/hajimehoshi/ebiten/v2/ebitenutil"
)

type DrawFunc func (*ebiten.Image)

type PaintownLevel struct {
}

type Engine struct {
    Coroutine *coroutine.Coroutine
    Drawer func(*ebiten.Image)
}

func dataPath(path string) string {
    return "paintown-data/" + path
}

func loadPng(path string) (image.Image, error) {
    file, err := os.Open(dataPath(path))
    if err != nil {
        return nil, err
    }
    defer file.Close()

    img, err := png.Decode(file)
    if err != nil {
        return nil, err
    }

    return img, nil
}

type PaintownCharacter struct {
    Definition CharacterDefinition
}

type CharacterDefinition struct {
    Name string
}

type SExpr struct {
    Name string
    // an sexpr with no children is just a value
    Children []*SExpr
}

func (sexpr *SExpr) GetChild(name string) *SExpr {
    for _, child := range sexpr.Children {
        if child.Name == name {
            return child
        }
    }

    return nil
}

func (sexpr *SExpr) IsValue() bool {
    return len(sexpr.Children) == 0
}

func (sexpr *SExpr) GetValue(index int) string {
    if index < len(sexpr.Children) {
        child := sexpr.Children[index]
        if child.IsValue() {
            return child.Name
        }

        return ""
    } else {
        return ""
    }
}

func tokenize(reader io.Reader) []string {
    byteReader, ok := reader.(io.ByteReader)
    if !ok {
        byteReader = bufio.NewReader(reader)
    }

    var tokens []string

    whitespace := func (char byte) bool {
        return char == ' ' || char == '\t' || char == '\n' || char == '\r'
    }

    for {
        char, err := byteReader.ReadByte()
        if err != nil {
            break
        }

        switch {
            case char == '(':
                tokens = append(tokens, "(")
            case char == ')':
                tokens = append(tokens, ")")
            case whitespace(char):
            default:
                token := ""
                for char != '(' && char != ')' && !whitespace(char) {
                    token += string(char)
                    char, err = byteReader.ReadByte()
                    if err != nil {
                        break
                    }
                }

                tokens = append(tokens, token)

                if char == '(' {
                    tokens = append(tokens, "(")
                } else if char == ')' {
                    tokens = append(tokens, ")")
                }
        }
    }

    return tokens
}

func parseSExpr(reader io.Reader) (*SExpr, error) {
    tokens := tokenize(reader)

    var root *SExpr
    var current *SExpr
    var parents []*SExpr

    _ = current

    for _, token := range tokens {
        if token == "(" {
            parents = append(parents, current)
        }
    }

    if len(parents) != 0 {
        return nil, fmt.Errorf("Unbalanced open parentheses")
    }

    return root, nil
}

func readSExpression(path string) (*SExpr, error) {
    file, err := os.Open(path)
    if err != nil {
        return nil, err
    }
    defer file.Close()

    parsed, err := parseSExpr(file)
    if err != nil {
        return nil, fmt.Errorf("Unable to parse %v: %v", path, err)
    }

    return parsed, nil
}

// a definition file is a parentheses delimited set of values
func loadDefinition(path string) (CharacterDefinition, error) {
    raw, err := readSExpression(path)
    if err != nil {
        return CharacterDefinition{}, err
    }

    value := raw.GetChild("name")
    return CharacterDefinition{
        Name: value.GetValue(0),
    }, nil
}

func MakePaintownPlayer(name string) (*PaintownCharacter, error) {
    path := dataPath("players/" + name)

    definitionPath := filepath.Join(path, name + ".txt")
    definition, err := loadDefinition(definitionPath)
    if err != nil {
        return nil, err
    }

    player := &PaintownCharacter{
        Definition: definition,
    }

    return player, nil
}

func runGame(yield coroutine.YieldFunc, setDraw func(drawer DrawFunc) DrawFunc) error {

    drawer := func (screen *ebiten.Image) {
    }

    oldDrawer := setDraw(drawer)
    defer setDraw(oldDrawer)

    for {
        keys := inpututil.AppendJustPressedKeys(nil)
        for _, key := range keys {
            if key == ebiten.KeyTab {
                return nil
            }
        }

        yield()
    }
}

func makeRunMenu(setDraw func(drawer DrawFunc) DrawFunc) (func (yield coroutine.YieldFunc) error, error) {
    backgroundPng, err := loadPng("menu/paintown.png")
    if err != nil {
        return nil, err
    }

    background := ebiten.NewImageFromImage(backgroundPng)

    drawer := func(screen *ebiten.Image) {
        var options ebiten.DrawImageOptions
        screen.DrawImage(background, &options)
        ebitenutil.DebugPrintAt(screen, "Start", 300, 240)
    }

    logic := func(yield coroutine.YieldFunc) error {
        oldDrawer := setDraw(drawer)
        defer setDraw(oldDrawer)

        for {
            keys := inpututil.AppendJustPressedKeys(nil)
            for _, key := range keys {
                if key == ebiten.KeyEnter {
                    err := runGame(yield, setDraw)
                    if err != nil {
                        return err
                    }
                }
            }

            yield()
        }
    }

    return logic, nil
}

func MakeEngine() (*Engine, error) {
    var engine *Engine

    setDraw := func(drawer DrawFunc) DrawFunc {
        old := engine.Drawer
        engine.Drawer = drawer
        return old
    }

    menuLogic, err := makeRunMenu(setDraw)

    if err != nil {
        return nil, err
    }

    engine = &Engine{
        Coroutine: coroutine.MakeCoroutine(menuLogic),
        Drawer: func(screen *ebiten.Image) {
        },
    }

    return engine, nil
}

func (engine *Engine) Update() error {
    keys := inpututil.AppendJustPressedKeys(nil)

    for _, key := range keys {
        if key == ebiten.KeyEscape || key == ebiten.KeyCapsLock {
            return ebiten.Termination
        }
    }

    return engine.Coroutine.Run()
}

func (engine *Engine) Draw(screen *ebiten.Image) {
    engine.Drawer(screen)
}

func (engine *Engine) Layout(outsideWidth, outsideHeight int) (screenWidth, screenHeight int) {
    return 640, 480
}

func main(){
    log.SetFlags(log.Ldate | log.Lmicroseconds | log.Lshortfile)

    ebiten.SetWindowSize(1024, 768)
    ebiten.SetWindowTitle("Paintown")
    ebiten.SetWindowResizingMode(ebiten.WindowResizingModeEnabled)

    engine, err := MakeEngine()
    if err != nil {
        log.Printf("Error: %v", err)
    }

    err = ebiten.RunGame(engine)
    if err != nil {
        log.Printf("Error: %v", err)
    }
}
