package resource

import (
	"archive/zip"
	"errors"
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
)

// CharacterPacks owns transient ZIP expansions. Repeated selections, rounds and
// alternate DEFs share one expansion; Close removes only this owner's files.
// Its zero value is ready to use. Ordinary character directories need no copy.
type CharacterPacks struct {
	expanded map[string]string
}

const maxCharacterPackBytes = 1 << 30

// Resolve accepts a character directory, DEF, pack.zip or pack.zip:member.def.
// The latter two forms are documented in the supplied beta's select.def.
func (p *CharacterPacks) Resolve(reference string, directories ...string) (string, error) {
	reference = strings.Trim(strings.TrimSpace(reference), "\"")
	reference = filepath.FromSlash(strings.ReplaceAll(reference, "\\", "/"))
	lower := strings.ToLower(reference)
	archive, member := reference, ""
	if split := strings.Index(lower, ".zip:"); split >= 0 {
		archive, member = reference[:split+4], reference[split+5:]
		if member == "" {
			return "", fmt.Errorf("character archive %q has a blank DEF member", archive)
		}
	}
	if !strings.EqualFold(filepath.Ext(archive), ".zip") {
		if !strings.EqualFold(filepath.Ext(reference), ".def") {
			reference = filepath.Join(reference, filepath.Base(reference)+".def")
		}
		return Resolve(reference, directories...)
	}
	if member == "" {
		member = strings.TrimSuffix(filepath.Base(archive), filepath.Ext(archive)) + ".def"
	}
	if !localPackPath(member) || !strings.EqualFold(filepath.Ext(member), ".def") {
		return "", fmt.Errorf("character archive %q: invalid DEF member %q", archive, member)
	}
	source, err := Resolve(archive, directories...)
	if err != nil {
		return "", err
	}
	dir := p.expanded[source]
	if dir == "" {
		dir, err = expandCharacterPack(source)
		if err != nil {
			return "", fmt.Errorf("character archive %s: %w", source, err)
		}
		if p.expanded == nil {
			p.expanded = make(map[string]string)
		}
		p.expanded[source] = dir
	}
	definition, err := Resolve(member, dir)
	if err != nil {
		return "", fmt.Errorf("character archive %s DEF %q: %w", source, member, err)
	}
	return definition, nil
}

func localPackPath(name string) bool {
	return name != "" && filepath.IsLocal(name) && !strings.ContainsAny(name, ":\x00")
}

func expandCharacterPack(source string) (directory string, err error) {
	archive, err := zip.OpenReader(source)
	if err != nil {
		return "", err
	}
	defer archive.Close()
	if len(archive.File) == 0 || len(archive.File) > 65536 {
		return "", fmt.Errorf("ZIP must contain 1..65536 entries")
	}
	directory, err = os.MkdirTemp("", "mugen-character-")
	if err != nil {
		return "", err
	}
	// Nothing from a failed expansion survives, including truncated/CRC-bad
	// members and directory traversal attempts. No archive controls this path.
	defer func() {
		if err != nil {
			os.RemoveAll(directory)
		}
	}()
	var expanded int64
	for _, file := range archive.File {
		name := filepath.FromSlash(strings.ReplaceAll(file.Name, "\\", "/"))
		if !localPackPath(name) || file.Mode()&os.ModeSymlink != 0 {
			return directory, fmt.Errorf("invalid ZIP entry %q", file.Name)
		}
		output := filepath.Join(directory, name)
		if file.FileInfo().IsDir() {
			if err = os.MkdirAll(output, 0700); err != nil {
				return directory, err
			}
			continue
		}
		if !file.Mode().IsRegular() || file.UncompressedSize64 > uint64(maxCharacterPackBytes-expanded) {
			return directory, fmt.Errorf("ZIP entry %q is not a regular file or exceeds the 1 GiB pack limit", file.Name)
		}
		if err = os.MkdirAll(filepath.Dir(output), 0700); err != nil {
			return directory, err
		}
		input, openErr := file.Open()
		if openErr != nil {
			return directory, fmt.Errorf("ZIP entry %q: %w", file.Name, openErr)
		}
		target, createErr := os.OpenFile(output, os.O_CREATE|os.O_EXCL|os.O_WRONLY, 0600)
		if createErr != nil {
			input.Close()
			return directory, fmt.Errorf("ZIP entry %q: %w", file.Name, createErr)
		}
		written, copyErr := io.Copy(target, io.LimitReader(input, maxCharacterPackBytes-expanded+1))
		err = errors.Join(copyErr, input.Close(), target.Close())
		if err != nil {
			return directory, fmt.Errorf("ZIP entry %q: %w", file.Name, err)
		}
		if written > maxCharacterPackBytes-expanded || uint64(written) != file.UncompressedSize64 {
			return directory, fmt.Errorf("ZIP entry %q has inconsistent/excessive expanded size", file.Name)
		}
		expanded += written
	}
	return directory, nil
}

func (p *CharacterPacks) Close() error {
	var failures []error
	for source, directory := range p.expanded {
		if err := os.RemoveAll(directory); err != nil {
			failures = append(failures, fmt.Errorf("remove character archive %s: %w", source, err))
		} else {
			delete(p.expanded, source)
		}
	}
	return errors.Join(failures...)
}
