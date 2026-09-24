#include "PluginProcessor.h"
#include "PluginIDs.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

using namespace Steinberg;

namespace PotatoAudio {

PotatoNoiseProcessor::PotatoNoiseProcessor ()
{
	setControllerClass (kPotatoNoiseControllerUID);
}

tresult PLUGIN_API PotatoNoiseProcessor::initialize (FUnknown* context)
{
	tresult result = AudioEffect::initialize (context);
	if (result != kResultOk)
		return result;

	// Pure generator: no audio input, stereo output only.
	addAudioOutput (STR16 ("Stereo Out"), Vst::SpeakerArr::kStereo);

	return kResultOk;
}

tresult PLUGIN_API PotatoNoiseProcessor::terminate ()
{
	return AudioEffect::terminate ();
}

tresult PLUGIN_API PotatoNoiseProcessor::setActive (TBool state)
{
	return AudioEffect::setActive (state);
}

tresult PLUGIN_API PotatoNoiseProcessor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	mEngine.setSampleRate (newSetup.sampleRate);
	return AudioEffect::setupProcessing (newSetup);
}

tresult PLUGIN_API PotatoNoiseProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;
	return kResultFalse;
}

tresult PLUGIN_API PotatoNoiseProcessor::process (Vst::ProcessData& data)
{
	// Read incoming parameter changes (only Amplitude is registered).
	if (data.inputParameterChanges)
	{
		int32 numParamsChanged = data.inputParameterChanges->getParameterCount ();
		for (int32 index = 0; index < numParamsChanged; index++)
		{
			Vst::IParamValueQueue* paramQueue = data.inputParameterChanges->getParameterData (index);
			if (!paramQueue || paramQueue->getParameterId () != kParamAmplitudeId)
				continue;

			Vst::ParamValue value;
			int32 sampleOffset;
			int32 numPoints = paramQueue->getPointCount ();
			if (numPoints > 0 && paramQueue->getPoint (numPoints - 1, sampleOffset, value) == kResultTrue)
				mEngine.setAmplitude (static_cast<float> (value));
		}
	}

	if (data.numOutputs == 0 || data.numSamples <= 0)
		return kResultOk;

	auto& output = data.outputs[0];
	mEngine.process (output.channelBuffers32, output.numChannels, data.numSamples);

	output.silenceFlags = mEngine.isSilent () ? (static_cast<uint64> (1) << output.numChannels) - 1 : 0;

	return kResultOk;
}

tresult PLUGIN_API PotatoNoiseProcessor::setState (IBStream* state)
{
	if (!state)
		return kResultFalse;

	IBStreamer streamer (state, kLittleEndian);

	float savedAmplitude = 0.f;
	if (streamer.readFloat (savedAmplitude) == false)
		return kResultFalse;

	mEngine.setAmplitude (savedAmplitude);

	return kResultOk;
}

tresult PLUGIN_API PotatoNoiseProcessor::getState (IBStream* state)
{
	IBStreamer streamer (state, kLittleEndian);
	streamer.writeFloat (mEngine.getAmplitude ());
	return kResultOk;
}

} // namespace PotatoAudio
