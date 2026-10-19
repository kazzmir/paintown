package engine

// RandomStream is the beta's Park–Miller generator. A session stream selects
// match seeds; roots, helpers and redirected CNS share a separate match stream.
// Round resets do not reseed it.
type RandomStream struct{ seed, state int64 }

func NewRandomStream(seed int64) *RandomStream {
	r := &RandomStream{}
	r.Seed(seed)
	return r
}

func (r *RandomStream) Seed(seed int64) {
	if seed <= 0 || seed >= 2147483647 {
		seed = 1
	}
	r.seed, r.state = seed, seed
}

func (r *RandomStream) InitialSeed() int64 { return r.seed }

func (r *RandomStream) Next() int64 {
	if r.state == 0 {
		r.Seed(1)
	}
	r.state = r.state * 16807 % 2147483647
	return r.state
}

// Range includes both endpoints and always draws, including singleton ranges.
func (r *RandomStream) Range(low, high int32) int32 {
	if low > high {
		low, high = high, low
	}
	return int32(int64(low) + r.Next()%(int64(high)-int64(low)+1))
}
