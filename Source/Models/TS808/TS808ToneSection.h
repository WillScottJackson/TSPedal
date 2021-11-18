#pragma once

// Tone and Volume section of an Ibanez Tube Screamer
// Reference: https://www.electrosmash.com/tube-screamer-analysis

namespace TS808
{
	class ToneAndVolume
	{
	public:
		template <typename T>
		T processSample(T sample);

		void setSampleRate(float sampleRate);

		template <typename T>
		void setParameters(T tone, T outputVolume);

	private: 
		void _updateCoefficients();

		float _samplingFrequency = 48000.0f;
		float _sampleInterval = 1.f/ _samplingFrequency;

		// Capacitor value, + impedance using discrete Kirchoff (DK) method, i.e. 
		// substitute capacitor for resistance + current source 
		const float C5 = .22e-6;
		float RC5 = _sampleInterval / (2.f * C5);

		const float C6 = .22e-6;
		float RC6 = _sampleInterval / (2.f * C6);

		const float C7 = .22e-6;
		float RC7 = _sampleInterval / (2.f * C7);

		const float C8 = 1e-6;
		float RC8 = _sampleInterval / (2.f * C8);

		// Fixed Resistors 
		const float R7 = 1000.f;			// Low pass 
		const float R8 = 220.f;			// High pass 
		const float R9 = 10000.f;
		const float R10 = 1000.f;
		const float R11 = 1000.f;

		// Potentiometer value 
		// P2 
		float _P2Position = 0.5f;
		float P2_1 = 20000.f * _P2Position;
		float P2_2 = 20000.f * (1.f - _P2Position);
		
		// P3
		float _P3Position = 0.5f;
		float P3_1 = 100e3 * (1.f - _P3Position);
		float P3_2 = 100e3 * _P3Position;

		// Sums of resistances 
		float G_lowPass = 1.f + (RC5 / P2_1) + (R7 / P2_1); // Low pass filter 
		float G_highPass = 1.f + (RC6 / P2_1) + (R8 / P2_1); // High pass filter
		float Gx = 1.f + R7 / (G_highPass * P2_1);		// Op amp non-inverting input 

		float Gz = (1.f / RC5 + 1.f / R7 + 1.f / (G_lowPass * P2_1)); // Inverting Input 
		float Go = (1.f + P3_1 / P3_2 + R11 / P3_2 + RC8 / P3_2);		// Output resistors + cap 
		float Gr = 1.f + P2_1 / RC6 + R8 / RC6;							
		float Gs = 1.f + P2_1 / RC7 + R8 / RC7;

		// States
		float x1 = 0.f;
		float x2 = 0.f;
		float x3 = 0.f;
		float x4 = 0.f;

		// Filter Coefficients 
		float b0 = Gx / (Go * R7 * Gz);
		float b1 = Gx / (Go * Gz);
		float b2 = Gx * RC6 / (G_lowPass * Gz * Go * P2_1);
		float b3 = -RC7 * R9 / (Go * G_highPass * P2_2);
		float b4 = -RC8 / Go;
	};
}