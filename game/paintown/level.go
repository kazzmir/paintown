package paintown

import (
    "fmt"
    "strings"
    "log"
    "strconv"

    "github.com/kazzmir/paintown/game/lib/sexp"
    "github.com/kazzmir/paintown/game/data"

    "github.com/hajimehoshi/ebiten/v2"
)

type Panel struct {
    Index int
    Image *ebiten.Image
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
            backgroundImage = ebiten.NewImageFromImage(convertTransparency(backgroundImagePng))
        }
    }

    var frontPanels []*ebiten.Image
    frontPanelsElements := raw.FindAll("level", "frontpanel")
    for _, panel := range frontPanelsElements {
        frontPanelPng, err := data.LoadPng(panel.GetValue(0))
        if err != nil {
            log.Printf("Error loading front panel image '%v': %v", panel.GetValue(0), err)
        } else {
            frontPanels = append(frontPanels, ebiten.NewImageFromImage(convertTransparency(frontPanelPng)))
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
            panels[index] = ebiten.NewImageFromImage(convertTransparency(panelPng))
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

    return &Level{
        ZMinimum: zMinimum,
        ZMaximum: zMaximum,
        BackgroundParallax: backgroundParallax,
        ForegroundParallax: foregroundParallax,
        BackgroundImage: backgroundImage,
        FrontPanels: frontPanels,
        Panels: panels,
        PanelOrder: order,
    }, nil
}
