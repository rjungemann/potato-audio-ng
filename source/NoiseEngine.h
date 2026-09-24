// NoiseEngine: plain C++ white noise DSP core shared by the VST3 plug-in and the
// standalone application. Has no dependency on the VST3 SDK or VSTGUI so it can be
// driven directly by either host.
#pragma once

#include <atomic>
#include <cmath>
#include <cstdint>

namespace PotatoAudio {

class NoiseEngine
{
public:
	/** Set the target amplitude (0..1). Safe to call from any thread. */
	void setAmplitude (float amplitude) noexcept
	{
		mTargetAmplitude.store (amplitude, std::memory_order_relaxed);
	}

	float getAmplitude () const noexcept { return mTargetAmplitude.load (std::memory_order_relaxed); }

	/** Configure the amplitude smoothing time based on the current sample rate. Call from the
	 *  audio thread before processing starts (e.g. in setupProcessing / when opening the stream). */
	void setSampleRate (double sampleRate) noexcept
	{
		if (sampleRate <= 0.0)
			sampleRate = 44100.0;
		constexpr double smoothingTimeSeconds = 0.005;
		mSmoothingCoeff = 1.0f - std::exp (-1.0f / static_cast<float> (smoothingTimeSeconds * sampleRate));
	}

	/** Reset the internal pseudo-random state (mainly useful for reproducible tests). */
	void reset (uint32_t seed = 0x1234567u) noexcept { mState = seed ? seed : 0x1234567u; }

	/** Generate the next white noise sample, already scaled by the (smoothed) amplitude. */
	float nextSample () noexcept
	{
		mSmoothedAmplitude += (mTargetAmplitude.load (std::memory_order_relaxed) - mSmoothedAmplitude) * mSmoothingCoeff;

		// xorshift32: cheap, dependency-free pseudo-random generator.
		mState ^= mState << 13;
		mState ^= mState >> 17;
		mState ^= mState << 5;

		constexpr float inv = 1.0f / 2147483648.0f; // 1 / 2^31
		const float white = static_cast<float> (static_cast<int32_t> (mState)) * inv;

		return white * mSmoothedAmplitude;
	}

	/** Fill numChannels de-interleaved buffers with numSamples of noise each. */
	void process (float* const* channels, int32_t numChannels, int32_t numSamples) noexcept
	{
		for (int32_t i = 0; i < numSamples; ++i)
			for (int32_t ch = 0; ch < numChannels; ++ch)
				channels[ch][i] = nextSample ();
	}

	bool isSilent () const noexcept { return mTargetAmplitude.load (std::memory_order_relaxed) <= 0.0f && mSmoothedAmplitude <= 0.0f; }

private:
	std::atomic<float> mTargetAmplitude { 0.0f };
	float mSmoothedAmplitude = 0.0f;
	float mSmoothingCoeff = 1.0f;
	uint32_t mState = 0x1234567u;
};

} // namespace PotatoAudio
