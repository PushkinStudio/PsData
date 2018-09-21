
#include "PsDataEditorPlugin.h"
#include "Pins/PsDataPinFactory.h"
#include "PsData.h"

#include "EdGraphUtilities.h"

#define LOCTEXT_NAMESPACE "PsDataEditorPluginModule"

void FPsDataEditorPluginModule::StartupModule()
{
	TSharedPtr<FPsDataPinFactory> PinFactory = MakeShareable(new FPsDataPinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(PinFactory);
}

void FPsDataEditorPluginModule::ShutdownModule()
{

}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FPsDataEditorPluginModule, PsDataEditorPlugin)
