package paintown

import (
    "fmt"
    "strings"
    "log"
    "strconv"
    "image"

    "github.com/kazzmir/paintown/game/lib/sexp"
    "github.com/kazzmir/paintown/game/graphics"
    "github.com/kazzmir/paintown/game/data"

    "github.com/hajimehoshi/ebiten/v2"
)

type Panel struct {
    Index int
    Image *ebiten.Image
}

type StimulationKind int
const (
    StimulationKindNone StimulationKind = iota
    StimulationKindHealth
    StimulationKindPower
)

type Stimulation struct {
    // health, or something else
    Kind StimulationKind
    Value int
}

func (stimulation *Stimulation) Apply(player *PlayerState) {
    switch stimulation.Kind {
        case StimulationKindHealth:
            player.AddHealth(float64(stimulation.Value))
        case StimulationKindPower:
            player.AddPower(float64(stimulation.Value), 100)
    }
}

type BlockObject struct {
    Id int
    Name string
    Type string

    // path to definition file
    Path string

    // for remapping graphics
    Map int

    // non-zero for enemies
    Health int

    // spawn point relative to start of block
    Coords image.Point

    Stimulation Stimulation
}

type Block struct {
    Id int
    Length int
    Objects []BlockObject
}

type Level struct {
    ZMinimum int
    ZMaximum int
    Atmosphere string

    BackgroundParallax float32
    ForegroundParallax float32

    BackgroundImage *ebiten.Image
    FrontPanels []*ebiten.Image
    Panels map[int]*ebiten.Image

    PanelOrder []int
    Description string
    Blocks []Block
}

func parseBlockObject(object *sexp.SExpr) BlockObject {
    id, _ := sexp.ReadValue[int](object, "id", 0)
    name, _ := sexp.ReadValue[string](object, "name", 0)
    type_, _ := sexp.ReadValue[string](object, "type", 0)
    path, _ := sexp.ReadValue[string](object, "path", 0)
    mapIndex, _ := sexp.ReadValue[int](object, "map", 0)
    health, _ := sexp.ReadValue[int](object, "health", 0)

    coordsElement := object.GetChild("coords")
    var coords image.Point
    if coordsElement != nil {
        x, _ := coordsElement.GetInt(0)
        y, _ := coordsElement.GetInt(1)
        coords = image.Point{X: x, Y: y}
    }

    var stimulation Stimulation

    stimulationElement := object.GetChild("stimulation")
    if stimulationElement != nil {
        healthValue, isHealth := sexp.ReadValue[int](stimulationElement, "health", 0)
        if isHealth {
            stimulation.Kind = StimulationKindHealth
            stimulation.Value = healthValue
        }

        powerValue, isPower := sexp.ReadValue[int](stimulationElement, "power", 0)
        if isPower {
            stimulation.Kind = StimulationKindPower
            stimulation.Value = powerValue
        }
    }

    return BlockObject{
        Id: id,
        Name: name,
        Type: type_,
        Path: path,
        Map: mapIndex,
        Coords: coords,
        Health: health,
        Stimulation: stimulation,
    }
}

func parseBlock(block *sexp.SExpr) Block {
    id, _ := sexp.ReadValue[int](block, "id", 0)
    length, _ := sexp.ReadValue[int](block, "length", 0)

    var objects []BlockObject
    objectsRaw := block.FindAll("block", "object")
    for _, object := range objectsRaw {
        objects = append(objects, parseBlockObject(object))
    }

    return Block{
        Id: id,
        Length: length,
        Objects: objects,
    }
}

func LoadLevel(path string) (*Level, error) {
    raw, err := sexp.ReadSExpression(data.DataPath(path))
    if err != nil {
        return nil, err
    }

    if strings.ToLower(raw.Name) != "level" {
        return nil, fmt.Errorf("Expected 'level' as root element, got '%v'", raw.Name)
    }

    zMinimum, _ := sexp.ReadValue[int](raw, "z/minimum", 0)
    zMaximum, _ := sexp.ReadValue[int](raw, "z/maximum", 0)

    backgroundParallax, _ := sexp.ReadValue[float32](raw, "background-parallax", 0)
    foregroundParallax, _ := sexp.ReadValue[float32](raw, "foreground-parallax", 0)

    if backgroundParallax <= 0 {
        backgroundParallax = 1
    }

    if foregroundParallax <= 0 {
        foregroundParallax = 1
    }

    var backgroundImage *ebiten.Image
    backgroundPath, _ := sexp.ReadValue[string](raw, "background", 0)
    if backgroundPath != "" {
        backgroundImagePng, err := data.LoadPng(backgroundPath)
        if err != nil {
            log.Printf("Error loading background image '%v': %v", backgroundPath, err)
        } else {
            backgroundImage = ebiten.NewImageFromImage(graphics.ConvertTransparency(backgroundImagePng))
        }
    }

    var frontPanels []*ebiten.Image
    frontPanelsElements := raw.FindAll("level", "frontpanel")
    for _, panel := range frontPanelsElements {
        frontPanelPng, err := data.LoadPng(panel.GetValue(0))
        if err != nil {
            log.Printf("Error loading front panel image '%v': %v", panel.GetValue(0), err)
        } else {
            frontPanels = append(frontPanels, ebiten.NewImageFromImage(graphics.ConvertTransparency(frontPanelPng)))
        }
    }

    panels := make(map[int]*ebiten.Image)
    panelElements := raw.FindAll("level", "panel")
    for _, panelElement := range panelElements {
        index, _ := panelElement.GetInt(0)
        imagePath := panelElement.GetValue(1)
        panelPng, err := data.LoadPng(imagePath)
        if err != nil {
            log.Printf("Error loading panel image '%v': %v", imagePath, err)
        } else {
            panels[index] = ebiten.NewImageFromImage(graphics.ConvertTransparency(panelPng))
        }
    }

    var order []int
    orderElement := raw.GetChild("order")
    if orderElement != nil {
        for _, child := range orderElement.Children {
            index, err := strconv.Atoi(child.Name)
            if err != nil {
                log.Printf("Error parsing panel order index '%v': %v", child.Name, err)
            } else {
                order = append(order, index)
            }
        }
    }

    description, ok := sexp.ReadValue[string](raw, "description", 0)
    if !ok {
        description = ""
    }

    var blocks []Block

    blocksRaw := raw.FindAll("level", "block")
    for _, block := range blocksRaw {
        blocks = append(blocks, parseBlock(block))
    }

    enemies := 0
    for _, block := range blocks {
        for _, object := range block.Objects {
            if object.Type == "enemy" {
                enemies += 1
            }
        }
    }

    log.Printf("Loaded level with %d blocks and %d enemies", len(blocks), enemies)

    return &Level{
        ZMinimum: zMinimum,
        ZMaximum: zMaximum,
        BackgroundParallax: backgroundParallax,
        ForegroundParallax: foregroundParallax,
        BackgroundImage: backgroundImage,
        FrontPanels: frontPanels,
        Panels: panels,
        PanelOrder: order,
        Description: description,
        Blocks: blocks,
    }, nil
}
