// NES 2A03 APU sound chip emulator

// Nes_Snd_Emu 0.1.8
#ifndef NES_APU_H
#define NES_APU_H

#include "blargg_common.h"

typedef blargg_long nes_time_t; // CPU clock cycle count
typedef unsigned nes_addr_t; // 16-bit memory address

// Private oscillators used by Nes_Apu

// Nes_Snd_Emu 0.1.8
#ifndef NES_OSCS_H
#define NES_OSCS_H

#include "blargg_common.h"
#include "Blip_Buffer.h"

class Nes_Apu;

struct Nes_Osc
{
	unsigned char regs [4];
	bool reg_written [4];
	Blip_Buffer* output;
	int length_counter;// length counter (0 if unused by oscillator)
	int delay;      // delay until next (potential) transition
	int last_amp;   // last amplitude oscillator was outputting
	
	void clock_length( int halt_mask );
	int period() const {
		return (regs [3] & 7) * 0x100 + (regs [2] & 0xFF);
	}
	void reset() {
		delay = 0;
		last_amp = 0;
	}
	int update_amp( int amp ) {
		int delta = amp - last_amp;
		last_amp = amp;
		return delta;
	}
};

struct Nes_Envelope : Nes_Osc
{
	int envelope;
	int env_delay;
	
	void clock_envelope();
	int volume() const;
	void reset() {
		envelope = 0;
		env_delay = 0;
		Nes_Osc::reset();
	}
};

// Nes_Square
struct Nes_Square : Nes_Envelope
{
	enum { negate_flag = 0x08 };
	enum { shift_mask = 0x07 };
	enum { phase_range = 8 };
	int phase;
	int sweep_delay;
	
	typedef Blip_Synth<blip_good_quality,1> Synth;
	Synth const& synth; // shared between squares
	
	Nes_Square( Synth const* s ) : synth( *s ) { }
	
	void clock_sweep( int adjust );
	void run( nes_time_t, nes_time_t );
	void reset() {
		sweep_delay = 0;
		Nes_Envelope::reset();
	}
	nes_time_t maintain_phase( nes_time_t time, nes_time_t end_time,
			nes_time_t timer_period );
};

// Nes_Triangle
struct Nes_Triangle : Nes_Osc
{
	enum { phase_range = 16 };
	int phase;
	int linear_counter;
	Blip_Synth<blip_med_quality,1> synth;
	
	int calc_amp() const;
	void run( nes_time_t, nes_time_t );
	void clock_linear_counter();
	void reset() {
		linear_counter = 0;
		phase = 1;
		Nes_Osc::reset();
	}
	nes_time_t maintain_phase( nes_time_t time, nes_time_t end_time,
			nes_time_t timer_period );
};

// Nes_Noise
struct Nes_Noise : Nes_Envelope
{
	int noise;
	Blip_Synth<blip_med_quality,1> synth;
	
	void run( nes_time_t, nes_time_t );
	void reset() {
		noise = 1 << 14;
		Nes_Envelope::reset();
	}
};

// Nes_Dmc
struct Nes_Dmc : Nes_Osc
{
	int address;    // address of next byte to read
	int period;
	//int length_counter; // bytes remaining to play (already defined in Nes_Osc)
	int buf;
	int bits_remain;
	int bits;
	bool buf_full;
	bool silence;
	
	enum { loop_flag = 0x40 };
	
	int dac;
	
	nes_time_t next_irq;
	bool irq_enabled;
	bool irq_flag;
	bool pal_mode;
	bool nonlinear;
	
	int (*prg_reader)( void*, nes_addr_t ); // needs to be initialized to prg read function
	void* prg_reader_data;
	
	Nes_Apu* apu;
	
	Blip_Synth<blip_med_quality,1> synth;
	
	void start();
	void write_register( int, int );
	void run( nes_time_t, nes_time_t );
	void recalc_irq();
	void fill_buffer();
	void reload_sample();
	void reset();
	int count_reads( nes_time_t, nes_time_t* ) const;
	nes_time_t next_read_time() const;
};

#endif

struct apu_state_t;
class Nes_Buffer;

class Nes_Apu {
public:
	// Set buffer to generate all sound into, or disable sound if NULL
	void output( Blip_Buffer* );
	
	// Set memory reader callback used by DMC oscillator to fetch samples.
	// When callback is invoked, 'user_data' is passed unchanged as the
	// first parameter.
	void dmc_reader( int (*callback)( void* user_data, nes_addr_t ), void* user_data = NULL );
	
	// All time values are the number of CPU clock cycles relative to the
	// beginning of the current time frame. Before resetting the CPU clock
	// count, call end_frame( last_cpu_time ).
	
	// Write to register (0x4000-0x4017, except 0x4014 and 0x4016)
	enum { start_addr = 0x4000 };
	enum { end_addr   = 0x4017 };
	void write_register( nes_time_t, nes_addr_t, int data );
	
	// Read from status register at 0x4015
	enum { status_addr = 0x4015 };
	int read_status( nes_time_t );
	
	// Run all oscillators up to specified time, end current time frame, then
	// start a new time frame at time 0. Time frames have no effect on emulation
	// and each can be whatever length is convenient.
	void end_frame( nes_time_t );
	
// Additional optional features (can be ignored without any problem)

	// Reset internal frame counter, registers, and all oscillators.
	// Use PAL timing if pal_timing is true, otherwise use NTSC timing.
	// Set the DMC oscillator's initial DAC value to initial_dmc_dac without
	// any audible click.
	void reset( bool pal_mode = false, int initial_dmc_dac = 0 );
	
	// Adjust frame period
	void set_tempo( double );
	
	// Save/load exact emulation state
	void save_state( apu_state_t* out ) const;
	void load_state( apu_state_t const& );
	
	// Set overall volume (default is 1.0)
	void volume( double );
	
	// Set treble equalization (see notes.txt)
	void treble_eq( const blip_eq_t& );
	
	// Set sound output of specific oscillator to buffer. If buffer is NULL,
	// the specified oscillator is muted and emulation accuracy is reduced.
	// The oscillators are indexed as follows: 0) Square 1, 1) Square 2,
	// 2) Triangle, 3) Noise, 4) DMC.
	enum { osc_count = 5 };
	void osc_output( int index, Blip_Buffer* buffer );
	
	// Set IRQ time callback that is invoked when the time of earliest IRQ
	// may have changed, or NULL to disable. When callback is invoked,
	// 'user_data' is passed unchanged as the first parameter.
	void irq_notifier( void (*callback)( void* user_data ), void* user_data = NULL );
	
	// Get time that APU-generated IRQ will occur if no further register reads
	// or writes occur. If IRQ is already pending, returns irq_waiting. If no
	// IRQ will occur, returns no_irq.
	enum { no_irq = INT_MAX / 2 + 1 };
	enum { irq_waiting = 0 };
	nes_time_t earliest_irq( nes_time_t ) const;
	
	// Count number of DMC reads that would occur if 'run_until( t )' were executed.
	// If last_read is not NULL, set *last_read to the earliest time that
	// 'count_dmc_reads( time )' would result in the same result.
	int count_dmc_reads( nes_time_t t, nes_time_t* last_read = NULL ) const;
	
	// Time when next DMC memory read will occur
	nes_time_t next_dmc_read_time() const;
	
	// Run DMC until specified time, so that any DMC memory reads can be
	// accounted for (i.e. inserting CPU wait states).
	void run_until( nes_time_t );
	
public:
	Nes_Apu();
	BLARGG_DISABLE_NOTHROW
private:
	friend class Nes_Nonlinearizer;
	void enable_nonlinear( double volume );
	static double nonlinear_tnd_gain() { return 0.75; }
private:
	friend struct Nes_Dmc;
	
	// noncopyable
	Nes_Apu( const Nes_Apu& );
	Nes_Apu& operator = ( const Nes_Apu& );
	
	Nes_Osc*            oscs [osc_count];
	Nes_Square          square1;
	Nes_Square          square2;
	Nes_Noise           noise;
	Nes_Triangle        triangle;
	Nes_Dmc             dmc;
	
	double tempo_;
	nes_time_t last_time; // has been run until this time in current frame
	nes_time_t last_dmc_time;
	nes_time_t earliest_irq_;
	nes_time_t next_irq;
	int frame_period;
	int frame_delay; // cycles until frame counter runs next
	int frame; // current frame (0-3)
	int osc_enables;
	int frame_mode;
	bool irq_flag;
	void (*irq_notifier_)( void* user_data );
	void* irq_data;
	Nes_Square::Synth square_synth; // shared by squares
	
	void irq_changed();
	void state_restored();
	void run_until_( nes_time_t );
	
	// TODO: remove
	friend class Nes_Core;
};

inline void Nes_Apu::osc_output( int osc, Blip_Buffer* buf )
{
	assert( (unsigned) osc < osc_count );
	oscs [osc]->output = buf;
}

inline nes_time_t Nes_Apu::earliest_irq( nes_time_t ) const
{
	return earliest_irq_;
}

inline void Nes_Apu::dmc_reader( int (*func)( void*, nes_addr_t ), void* user_data )
{
	dmc.prg_reader_data = user_data;
	dmc.prg_reader = func;
}

inline void Nes_Apu::irq_notifier( void (*func)( void* user_data ), void* user_data )
{
	irq_notifier_ = func;
	irq_data = user_data;
}

inline int Nes_Apu::count_dmc_reads( nes_time_t time, nes_time_t* last_read ) const
{
	return dmc.count_reads( time, last_read );
}
	
inline nes_time_t Nes_Dmc::next_read_time() const
{
	if ( length_counter == 0 )
		return Nes_Apu::no_irq; // not reading
	
	return apu->last_dmc_time + delay + long (bits_remain - 1) * period;
}

inline nes_time_t Nes_Apu::next_dmc_read_time() const { return dmc.next_read_time(); }

#endif
