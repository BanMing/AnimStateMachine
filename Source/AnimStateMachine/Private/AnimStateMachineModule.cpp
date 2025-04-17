
#include "AnimStateMachineModule.h"

void FAnimStateMachineModule::StartupModule()
{
	IModuleInterface::StartupModule();
}

void FAnimStateMachineModule::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
}

IMPLEMENT_MODULE(FAnimStateMachineModule, AnimStateMachine)
