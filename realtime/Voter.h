#ifndef REALTIME_VOTER_H_
#define REALTIME_VOTER_H_

constexpr int ALLOWED_DECISIONS_NUMBER = 3;

constexpr int ORIENTATION_UNKNOWN = 0;
constexpr int LEFT_ORIENTED       = 1;
constexpr int RIGHT_ORIENTED      = 2;

class Voter
{
public:
    void addOrientation(int orientation);
    std::string make_decision();
    void reset();
private:
    int decisions[ALLOWED_DECISIONS_NUMBER] = {0, 0, 0};
};



#endif /* REALTIME_VOTER_H_ */
