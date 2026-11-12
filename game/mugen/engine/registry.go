package engine

import (
	"fmt"
	"math"
)

// Registry assigns simulation-owned IDs without process-global allocation.
// Retiring an entity never reuses its ID within the match.
type Registry[T any] struct {
	next int64
	live map[int32]T
}

func NewRegistry[T any](first int32) *Registry[T] {
	return &Registry[T]{next: int64(first), live: make(map[int32]T)}
}

func (r *Registry[T]) Allocate(value T) (int32, error) {
	if r.next < 0 || r.next > math.MaxInt32 {
		return 0, fmt.Errorf("entity ID allocation exhausted int32 range")
	}
	id := int32(r.next)
	r.next++
	r.live[id] = value
	return id, nil
}

func (r *Registry[T]) Lookup(id int32) (T, bool) { value, ok := r.live[id]; return value, ok }
func (r *Registry[T]) Remove(id int32)           { delete(r.live, id) }

// Replace keeps a persistent root identity while rebuilding its round state.
func (r *Registry[T]) Replace(id int32, value T) error {
	if _, ok := r.live[id]; !ok {
		return fmt.Errorf("cannot replace unknown entity ID %d", id)
	}
	r.live[id] = value
	return nil
}
