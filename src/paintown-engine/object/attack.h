#ifndef _paintown_attack_h
#define _paintown_attack_h

class Token;

/* a container class for an attack */
class Attack{
public:
    Attack();
    Attack(const Token & tok);
    Attack(const int x1, const int y1, const int x2, const int y2, const int damage, double forceX, double forceY);
    Attack(const Attack & a);

    void print();

    Attack & operator =(const Attack & a);

    int getXLen() const;
    int getYLen() const;

    /* true if the size of the box is 0 */
    bool empty() const;

public:

    inline int getX1() const {
        return x1;
    }

    inline int getX2() const {
        return x2;
    }

    inline int getY1() const {
        return y1;
    }

    inline int getY2() const {
        return y2;
    }

    inline int getDamage() const {
        return damage;
    }

    inline double getForceX() const {
        return forceX;
    }

    inline double getForceY() const {
        return forceY;
    }

public:
    inline void setX1( const int x ){
        x1 = x;
    }

    inline void setY1( const int y ){
        y1 = y;
    }

    inline void setX2( const int x ){
        x2 = x;
    }

    inline void setY2( const int y ){
        y2 = y;
    }

    inline void setDamage( const int d ){
        damage = d;
    }

    inline void setForceX(double x){
        forceX = x;
    }

    inline void setForceY(double y){
        forceY = y;
    }

protected:
    int x1, y1, x2, y2;
    int damage;
    double forceX;
    double forceY;
};

#endif
