#include "PluginController.h"
#include "PluginIDs.h"
#include "PluginProcessor.h"
#include "version.h"

#include "public.sdk/source/main/pluginfactory.h"

#define stringPluginName "Potato Noise"

using namespace Steinberg::Vst;
using namespace Steinberg;
using namespace PotatoAudio;

BEGIN_FACTORY_DEF ("Potato Audio", "https://github.com/", "mailto:rtaljun@gmail.com")

	DEF_CLASS2 (INLINE_UID_FROM_FUID (kPotatoNoiseProcessorUID),
	           PClassInfo::kManyInstances,
	           kVstAudioEffectClass,
	           stringPluginName,
	           Vst::kDistributable,
	           PotatoNoiseVST3Category,
	           FULL_VERSION_STR,
	           kVstVersionString,
	           PotatoNoiseProcessor::createInstance)

	DEF_CLASS2 (INLINE_UID_FROM_FUID (kPotatoNoiseControllerUID),
	           PClassInfo::kManyInstances,
	           kVstComponentControllerClass,
	           stringPluginName "Controller",
	           0,
	           "",
	           FULL_VERSION_STR,
	           kVstVersionString,
	           PotatoNoiseController::createInstance)

END_FACTORY
