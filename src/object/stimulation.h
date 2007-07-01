#ifndef _paintown_stimulation_h
#define _paintown_stimulation_h

class Object;
class Character;

class Stimulation{
public:
	Stimulation();
	Stimulation( const Stimulation & copy );

	virtual void stimulate( Object & o ) const;
	virtual void stimulate( Character & c ) const;
	virtual Stimulation * copy() const;

	virtual ~Stimulation();
};

#endif
