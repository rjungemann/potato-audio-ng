#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"

namespace PotatoAudio {

class PotatoNoiseController : public Steinberg::Vst::EditControllerEx1
{
public:
	PotatoNoiseController () = default;
	~PotatoNoiseController () SMTG_OVERRIDE = default;

	static Steinberg::FUnknown* createInstance (void* /*context*/)
	{
		return (Steinberg::Vst::IEditController*)new PotatoNoiseController;
	}

	// IPluginBase
	Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;

	// EditController
	Steinberg::tresult PLUGIN_API setComponentState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::IPlugView* PLUGIN_API createView (Steinberg::FIDString name) SMTG_OVERRIDE;

	DEFINE_INTERFACES
	END_DEFINE_INTERFACES (EditController)
	DELEGATE_REFCOUNT (EditController)
};

} // namespace PotatoAudio
