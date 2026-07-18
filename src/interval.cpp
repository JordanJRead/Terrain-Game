#include "interval.h"

Interval::Interval(double period) : mPeriod{ period } {}

bool Interval::update(double deltaTime) {
	mCurrentTime += deltaTime;
	if (mCurrentTime > mPeriod) {
		mCurrentTime -= mPeriod;
		return true;
	}
	return false;
}

double Interval::getPeriod() const {
	return mPeriod;
}