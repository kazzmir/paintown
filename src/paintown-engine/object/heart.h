#ifndef _heart_h
#define _heart_h

class HeartFactory;

class Heart{
protected:

	Heart();
	virtual ~Heart();

public:

	inline bool getAlive() const{
		return alive;
	}

	inline void setAlive( const bool b ){
		alive = b;
	}

	friend class HeartFactory;

private:
	bool alive;
};

#endif
