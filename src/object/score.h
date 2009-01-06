#ifndef _paintown_score_h
#define _paintown_score_h

/* keeps track of score stuff */
class Score{
public:
    Score();
    Score(const Score& copy);

    virtual ~Score();

    virtual void increaseScore(int much);

    virtual inline const unsigned int getScore() const {
        return score;
    }

    virtual inline void setScore(unsigned int score){
        this->score = score;
    }
	
private:
    unsigned int score;
};

#endif
