#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace PotatoAudio {

enum PotatoNoiseParams : Steinberg::Vst::ParamID
{
	kParamAmplitudeId = 100
};

static const Steinberg::FUID kPotatoNoiseProcessorUID (0x3EF53F44, 0x00C14FB9, 0x6A292E96, 0x5F7C617B);
static const Steinberg::FUID kPotatoNoiseControllerUID (0x2DD48E91, 0xC0DB5EBA, 0x94B6F71C, 0xEB622D52);

#define PotatoNoiseVST3Category "Fx|Generator"

} // namespace PotatoAudio
