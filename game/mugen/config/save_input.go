package config

import (
	"bytes"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strconv"
	"strings"

	"gopkg.in/yaml.v3"
)

// SaveInputConfig merges only supported input fields into the active YAML.
// Preserve other settings/comments and back up the complete original bytes.
// Invalid/ambiguous documents and failed backups leave the original untouched.
func (c *MugenConfig) SaveInputConfig() (backup string, err error) {
	path := c.ExtendedConfigPath
	if path == "" {
		return "", fmt.Errorf("no YAML configuration destination")
	}
	if resolved, resolveErr := filepath.EvalSymlinks(path); resolveErr == nil {
		path = resolved
	} else if !os.IsNotExist(resolveErr) {
		return "", resolveErr
	}
	original, err := os.ReadFile(path)
	exists := err == nil
	if err != nil && !os.IsNotExist(err) {
		return "", err
	}
	mode := os.FileMode(0600)
	if exists {
		info, err := os.Stat(path)
		if err != nil {
			return "", err
		}
		if !info.Mode().IsRegular() {
			return "", fmt.Errorf("YAML destination is not a regular file")
		}
		mode = info.Mode().Perm()
	}
	var document yaml.Node
	decoder := yaml.NewDecoder(bytes.NewReader(original))
	if err := decoder.Decode(&document); err != nil && err != io.EOF {
		return "", fmt.Errorf("existing YAML: %w", err)
	}
	var extra yaml.Node
	if err := decoder.Decode(&extra); err != io.EOF {
		return "", fmt.Errorf("existing YAML must contain one document")
	}
	if len(document.Content) == 0 {
		document = yaml.Node{Kind: yaml.DocumentNode, Content: []*yaml.Node{{Kind: yaml.MappingNode, Tag: "!!map"}}}
	}
	root := document.Content[0]
	if root.Kind != yaml.MappingNode {
		return "", fmt.Errorf("existing YAML must be a mapping")
	}
	var validation any
	if err := document.Decode(&validation); err != nil {
		return "", fmt.Errorf("existing YAML: %w", err)
	}
	input, err := inputMapping(root, "input")
	if err != nil {
		return "", err
	}
	keys := [2]Keys{c.Input.Player1Keyboard, c.Input.Player2Keyboard}
	for side, player := range []string{"p1", "p2"} {
		seat, err := inputMapping(input, player)
		if err != nil {
			return "", err
		}
		setInputNode(seat, "keyboard_enabled", inputScalar("!!bool", strconv.FormatBool(!c.Input.KeyboardDisabled[side])))
		pad, err := inputMapping(seat, "gamepad")
		if err != nil {
			return "", err
		}
		setInputNode(pad, "enabled", inputScalar("!!bool", strconv.FormatBool(c.Input.Gamepads[side].Enabled)))
		setInputNode(pad, "deadzone", inputScalar("!!float", strconv.FormatFloat(c.Input.Gamepads[side].Deadzone, 'f', -1, 64)))
		keyboard, err := inputMapping(seat, "keyboard")
		if err != nil {
			return "", err
		}
		joy, err := inputMapping(seat, "joy")
		if err != nil {
			return "", err
		}
		for _, action := range []string{"jump", "crouch", "left", "right", "a", "b", "c", "x", "y", "z", "start", "escape"} {
			if bindings, present := keys[side].Bindings(action); present {
				var names []string
				for _, key := range bindings {
					names = append(names, key.String())
				}
				setInputBinding(keyboard, action, inputNames(names))
			}
			if controls, present := c.Input.Gamepads[side].Bindings(action); present {
				var names []string
				for _, control := range controls {
					names = append(names, control.String())
				}
				setInputBinding(joy, action, inputNames(names))
			}
		}
	}
	var output bytes.Buffer
	encoder := yaml.NewEncoder(&output)
	encoder.SetIndent(2)
	if err := encoder.Encode(&document); err != nil {
		return "", err
	}
	if err := encoder.Close(); err != nil {
		return "", err
	}
	var verified ExtendedConfig
	if err := yaml.Unmarshal(output.Bytes(), &verified); err != nil {
		return "", fmt.Errorf("saved YAML validation: %w", err)
	}
	if exists {
		backup = path + ".bak"
		if err := atomicInputFile(backup, original, mode); err != nil {
			return "", fmt.Errorf("backup: %w", err)
		}
	}
	if err := atomicInputFile(path, output.Bytes(), mode); err != nil {
		return backup, err
	}
	return backup, nil
}

func inputScalar(tag, value string) *yaml.Node {
	return &yaml.Node{Kind: yaml.ScalarNode, Tag: tag, Value: value}
}

func inputNames(names []string) *yaml.Node {
	if len(names) == 1 {
		return inputScalar("!!str", names[0])
	}
	node := &yaml.Node{Kind: yaml.SequenceNode, Tag: "!!seq", Style: yaml.FlowStyle}
	for _, name := range names {
		node.Content = append(node.Content, inputScalar("!!str", name))
	}
	return node
}

func inputMapping(parent *yaml.Node, name string) (*yaml.Node, error) {
	for at := 0; at < len(parent.Content); at += 2 {
		if parent.Content[at].Value == name {
			node := parent.Content[at+1]
			if node.Kind != yaml.MappingNode {
				return nil, fmt.Errorf("input field %q must be an editable mapping (not an alias)", name)
			}
			return node, nil
		}
	}
	node := &yaml.Node{Kind: yaml.MappingNode, Tag: "!!map"}
	parent.Content = append(parent.Content, inputScalar("!!str", name), node)
	return node, nil
}

func setInputNode(parent *yaml.Node, name string, value *yaml.Node) {
	for at := 0; at < len(parent.Content); at += 2 {
		if parent.Content[at].Value == name {
			previous := parent.Content[at+1]
			value.HeadComment, value.LineComment, value.FootComment = previous.HeadComment, previous.LineComment, previous.FootComment
			value.Anchor = previous.Anchor
			parent.Content[at+1] = value
			return
		}
	}
	parent.Content = append(parent.Content, inputScalar("!!str", name), value)
}

// Bindings accept case-insensitive action names and Up/Down aliases on load.
// Replace every spelling of an edited action so an old alias cannot override
// its saved canonical name during the next map-based merge.
func setInputBinding(parent *yaml.Node, name string, value *yaml.Node) {
	found := false
	for at := 0; at < len(parent.Content); at += 2 {
		key, previous := parent.Content[at], parent.Content[at+1]
		canonical := strings.ToLower(key.Value)
		if canonical == "up" {
			canonical = "jump"
		} else if canonical == "down" {
			canonical = "crouch"
		}
		if canonical != name {
			continue
		}
		if !found {
			found, key.Value = true, name
			value.HeadComment, value.LineComment, value.FootComment = previous.HeadComment, previous.LineComment, previous.FootComment
			value.Anchor = previous.Anchor
			parent.Content[at+1] = value
		} else {
			value.HeadComment = strings.TrimSpace(strings.Join([]string{value.HeadComment, key.HeadComment, key.LineComment, key.FootComment,
				previous.HeadComment, previous.LineComment, previous.FootComment}, "\n"))
			parent.Content = append(parent.Content[:at], parent.Content[at+2:]...)
			at -= 2
		}
	}
	if !found {
		parent.Content = append(parent.Content, inputScalar("!!str", name), value)
	}
}

func atomicInputFile(path string, data []byte, mode os.FileMode) error {
	file, err := os.CreateTemp(filepath.Dir(path), ".mugen-input-*")
	if err != nil {
		return err
	}
	defer os.Remove(file.Name())
	defer file.Close()
	if err := file.Chmod(mode); err != nil {
		return err
	}
	if _, err := file.Write(data); err != nil {
		return err
	}
	if err := file.Sync(); err != nil {
		return err
	}
	if err := file.Close(); err != nil {
		return err
	}
	return os.Rename(file.Name(), path)
}
