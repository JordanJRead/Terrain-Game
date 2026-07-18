#ifndef INTERVAL_H
#define INTERVAL_H

class Interval {
public:
	Interval(double period);
	bool update(double deltaTime);
	double getPeriod() const;

private:
	double mPeriod;
	double mCurrentTime{};
};

#endif