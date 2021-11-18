#include "TS808ToneSection.h"

namespace TS808
{
	void ToneAndVolume::setSampleRate(float sampleRate)
	{
		_samplingFrequency = sampleRate; 
		_sampleInterval = 1.f / _samplingFrequency;

		_updateCoefficients();
	}

	template < typename T> 
	T ToneAndVolume::processSample(T sample)
	{
		float Vo = b0 * sample + b1 * x1 + b2 * x2 + b3 * x3 + b4 * x4;
		float Vx = sample / (R7 * Gz) + x1 / Gz + x2 * R2 / (G_lowPass * Gz * P2_1);
		x1 = (2.f / RC1) * (Vx)-x1;
		x2 = (2.f / RC2) * (vx / Gr + x2 * (P2_1 + R8) / Gr) - x2;
		x3 = (2.f / RC3) * (vx / Gs + x3 * (P2_2 + R8) / Gs) - x3;
		x4 = 2 * Vo / P3_2 + x4;
		return Vo;
	}

	void ToneAndVolume::_updateCoefficients()
	{
		RC5 = _sampleInterval / (2.f * C5);
		RC6 = _sampleInterval / (2.f * C6);
		RC7 = _sampleInterval / (2.f * C7);
		RC8 = _sampleInterval / (2.f * C8);

		G_lowPass = 1.f + (RC5 / P2_1) + (R7 / P2_1); // Low pass filter 
		G_highPass = 1.f + (RC6 / P2_1) + (R8 / P2_1); // High pass filter
		Gx = 1.f + R7 / (G_highPass * P2_1);		// Op amp non-inverting input 

		Gz = (1.f / RC5 + 1.f / R7 + 1.f / (G_lowPass * P2_1)); // Inverting Input 
		Go = (1.f + P3_1 / P3_2 + R11 / P3_2 + RC8 / P3_2);		// Output resistors + cap 
		Gr = 1.f + P2_1 / RC6 + R8 / RC6;
		Gs = 1.f + P2_1 / RC7 + R8 / RC7;

		// Filter Coefficients 
		b0 = Gx / (Go * R7 * Gz);
		b1 = Gx / (Go * Gz);
		b2 = Gx * RC6 / (G_lowPass * Gz * Go * P2_1);
		b3 = -RC7 * R9 / (Go * G_highPass * P2_2);
		b4 = -RC8 / Go;
	}
}