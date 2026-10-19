// Package resource resolves MUGEN file references without process-global paths.
package resource

import (
	"errors"
	"fmt"
	"io/fs"
	"os"
	"path/filepath"
	"strings"
)

// Resolve searches explicit directories in order, honoring Windows separators
// and case-insensitive names on case-sensitive hosts. Absolute names bypass the
// search list. It never falls back to an unrelated current-directory file.
func Resolve(name string, directories ...string) (string, error) {
	name = strings.TrimSpace(name)
	if len(name) >= 2 && name[0] == '"' && name[len(name)-1] == '"' {
		name = name[1 : len(name)-1]
	}
	name = filepath.FromSlash(strings.ReplaceAll(name, "\\", "/"))
	if name == "" {
		return "", fmt.Errorf("empty resource name: %w", fs.ErrNotExist)
	}
	var candidates []string
	if filepath.IsAbs(name) {
		candidates = []string{name}
	} else {
		for _, directory := range directories {
			if directory != "" {
				candidates = append(candidates, filepath.Join(directory, name))
			}
		}
	}
	seen := make(map[string]bool)
	for _, candidate := range candidates {
		absolute, err := filepath.Abs(candidate)
		if err != nil {
			return "", err
		}
		if seen[absolute] {
			continue
		}
		seen[absolute] = true
		resolved, err := foldPath(absolute)
		if err != nil {
			if errors.Is(err, fs.ErrNotExist) {
				continue
			}
			return "", fmt.Errorf("resource %q: %w", name, err)
		}
		info, err := os.Stat(resolved)
		if err != nil {
			return "", fmt.Errorf("resource %q: %w", name, err)
		}
		if info.Mode().IsRegular() {
			return resolved, nil
		}
	}
	return "", fmt.Errorf("resource %q not found (searched %s): %w", name, strings.Join(candidates, ", "), fs.ErrNotExist)
}

func foldPath(path string) (string, error) {
	if _, err := os.Stat(path); err == nil {
		return path, nil
	} else if !errors.Is(err, fs.ErrNotExist) {
		return "", err
	}
	volume := filepath.VolumeName(path)
	current := volume + string(filepath.Separator)
	relative := strings.TrimPrefix(path, volume)
	parts := strings.Split(strings.TrimLeft(relative, string(filepath.Separator)), string(filepath.Separator))
	for _, part := range parts {
		exact := filepath.Join(current, part)
		if _, err := os.Stat(exact); err == nil {
			current = exact
			continue
		} else if !errors.Is(err, fs.ErrNotExist) {
			return "", err
		}
		entries, err := os.ReadDir(current)
		if err != nil {
			return "", err
		}
		match := ""
		for _, entry := range entries {
			if entry.Name() == part {
				match = entry.Name()
				break
			}
			if strings.EqualFold(entry.Name(), part) {
				if match != "" {
					return "", fmt.Errorf("ambiguous case-insensitive path component %q in %s", part, current)
				}
				match = entry.Name()
			}
		}
		if match == "" {
			return "", &fs.PathError{Op: "resolve", Path: path, Err: fs.ErrNotExist}
		}
		current = filepath.Join(current, match)
	}
	return current, nil
}
