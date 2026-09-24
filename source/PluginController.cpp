#include "PluginController.h"
#include "PluginIDs.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "vstgui/plugin-bindings/vst3editor.h"

using namespace Steinberg;

namespace PotatoAudio {

tresult PLUGIN_API PotatoNoiseController::initialize (FUnknown* context)
{
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk)
		return result;

	parameters.addParameter (STR16 ("Amplitude"), STR16 (""), 0, 0.0,
	                         Vst::ParameterInfo::kCanAutomate, kParamAmplitudeId, 0,
	                         STR16 ("Amp"));

	return kResultOk;
}

tresult PLUGIN_API PotatoNoiseController::terminate ()
{
	return EditControllerEx1::terminate ();
}

tresult PLUGIN_API PotatoNoiseController::setComponentState (IBStream* state)
{
	if (!state)
		return kResultFalse;

	IBStreamer streamer (state, kLittleEndian);

	float savedAmplitude = 0.f;
	if (streamer.readFloat (savedAmplitude) == false)
		return kResultFalse;

	setParamNormalized (kParamAmplitudeId, savedAmplitude);

	return kResultOk;
}

IPlugView* PLUGIN_API PotatoNoiseController::createView (FIDString name)
{
	if (FIDStringsEqual (name, Vst::ViewType::kEditor))
		return new VSTGUI::VST3Editor (this, "view", "editor.uidesc");
	return nullptr;
}

} // namespace PotatoAudio
