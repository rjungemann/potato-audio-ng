// Standalone host for the Potato Noise engine. Reuses the same resource/editor.uidesc
// layout as the VST3 plug-in editor via VSTGUI's Standalone application framework, and
// drives the shared NoiseEngine DSP directly through RtAudio (no VST3 hosting involved).
#include "vstgui/standalone/include/iapplication.h"
#include "vstgui/standalone/include/iuidescwindow.h"
#include "vstgui/standalone/include/helpers/appdelegate.h"
#include "vstgui/standalone/include/helpers/value.h"
#include "vstgui/standalone/include/helpers/uidesc/modelbinding.h"
#include "vstgui/standalone/include/helpers/windowlistener.h"

#include "RtAudio.h"

#include "../NoiseEngine.h"

#include <iostream>
#include <memory>

using namespace VSTGUI;
using namespace VSTGUI::Standalone;

namespace PotatoAudio {
namespace {

constexpr unsigned int kDefaultSampleRate = 48000;
constexpr unsigned int kDefaultBufferFrames = 256;
constexpr unsigned int kNumChannels = 2;

int audioCallback (void* outputBuffer, void* /*inputBuffer*/, unsigned int numFrames,
                   double /*streamTime*/, RtAudioStreamStatus status, void* userData)
{
	if (status)
		std::cerr << "[PotatoNoise] stream underflow/overflow detected" << std::endl;

	auto* engine = static_cast<NoiseEngine*> (userData);
	auto* out = static_cast<float*> (outputBuffer);

	for (unsigned int i = 0; i < numFrames; ++i)
	{
		const float sample = engine->nextSample ();
		for (unsigned int ch = 0; ch < kNumChannels; ++ch)
			out[i * kNumChannels + ch] = sample;
	}

	return 0;
}

//------------------------------------------------------------------------
class StandaloneApplication : public Application::DelegateAdapter, public WindowListenerAdapter
{
public:
	StandaloneApplication ()
	: Application::DelegateAdapter ({"Potato Noise", "1.0.0", "com.potatoaudio.potatonoise"})
	{
	}

	void finishLaunching () override
	{
		if (!startAudio ())
		{
			IApplication::instance ().quit ();
			return;
		}

		auto bindings = UIDesc::ModelBindingCallbacks::make ();
		bindings->addValue (Value::make ("Amplitude", 0.0),
		                    UIDesc::ValueCalls::onPerformEdit ([this] (IValue& value) {
			                    mEngine.setAmplitude (static_cast<float> (value.getValue ()));
		                    }));

		UIDesc::Config config;
		config.uiDescFileName = "editor.uidesc";
		config.viewName = "view";
		config.modelBinding = bindings;
		config.windowConfig.title = "Potato Noise";
		config.windowConfig.autoSaveFrameName = "PotatoNoiseWindow";
		config.windowConfig.style.border ().close ().centered ();

		mWindow = UIDesc::makeWindow (config);
		if (mWindow)
		{
			mWindow->show ();
			mWindow->registerWindowListener (this);
		}
		else
		{
			std::cerr << "[PotatoNoise] failed to create window from editor.uidesc" << std::endl;
			stopAudio ();
			IApplication::instance ().quit ();
		}
	}

	void onClosed (const IWindow&) override
	{
		stopAudio ();
		IApplication::instance ().quit ();
	}

private:
	bool startAudio ()
	{
		auto deviceIds = mDac.getDeviceIds ();
		if (deviceIds.empty ())
		{
			std::cerr << "[PotatoNoise] no audio output devices found" << std::endl;
			return false;
		}

		RtAudio::StreamParameters outputParams;
		outputParams.deviceId = mDac.getDefaultOutputDevice ();
		outputParams.nChannels = kNumChannels;
		outputParams.firstChannel = 0;

		unsigned int sampleRate = kDefaultSampleRate;
		unsigned int bufferFrames = kDefaultBufferFrames;

		if (mDac.openStream (&outputParams, nullptr, RTAUDIO_FLOAT32, sampleRate, &bufferFrames,
		                     &audioCallback, &mEngine, nullptr))
		{
			std::cerr << "[PotatoNoise] " << mDac.getErrorText () << std::endl;
			return false;
		}

		mEngine.setSampleRate (static_cast<double> (sampleRate));

		if (mDac.startStream ())
		{
			std::cerr << "[PotatoNoise] " << mDac.getErrorText () << std::endl;
			return false;
		}

		return true;
	}

	void stopAudio ()
	{
		if (mDac.isStreamRunning ())
			mDac.stopStream ();
		if (mDac.isStreamOpen ())
			mDac.closeStream ();
	}

	NoiseEngine mEngine;
	RtAudio mDac;
	WindowPtr mWindow;
};

} // anonymous namespace
} // namespace PotatoAudio

static Application::Init gAppDelegate (std::make_unique<PotatoAudio::StandaloneApplication> ());
