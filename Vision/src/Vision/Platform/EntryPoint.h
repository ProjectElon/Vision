#pragma once

#define RegisterEntryPointClass(ClassExtendsFromApplication)\
Vision::Application* Vision::CreateApplication()\
{\
	return new ClassExtendsFromApplication;\
}