#pragma once

#include "Vision/Core/Common.h"

enum MetaType
{
	MetaType_bool,
	MetaType_char,
	MetaType_int8,
	MetaType_uint8,
	MetaType_int16,
	MetaType_uint16,
	MetaType_int32,
	MetaType_uint32,
	MetaType_int64,
	MetaType_uint64,
	MetaType_float32,
	MetaType_float64,
	MetaType_Vector2,
	MetaType_Vector3,
	MetaType_Vector4,
	MetaType_Matrix4,
	MetaType_TagComponent,
	MetaType_TransformComponent,
	MetaType_OrthographicCameraComponent,
	MetaType_PerspectiveCameraComponent,
	MetaType_SpriteRendererComponent,
	MetaType_Settings,
	MetaType_Vars,
	MetaType_String,
};

struct StructMember
{
	const char* Name;
	MetaType    Type;
	uint32      Offset;
	uint32      ArrayCount;
	bool        IsPointer;
	bool        IsArray;
};

#include "Vision/Meta/ComponentsMeta.h"
#include "Vision/Meta/VarsMeta.h"
#include "Vision/Meta/StringMeta.h"

#define HandleMetaTypeCases(MemberPointer) \
	case MetaType_Vector2: InspectStruct(Vector2Members, (sizeof(Vector2Members) / sizeof(Vector2Members[0])), (Vector2*)MemberPointer, member->Name, memberFn, depth + 1); break; \
	case MetaType_Vector3: InspectStruct(Vector3Members, (sizeof(Vector3Members) / sizeof(Vector3Members[0])), (Vector3*)MemberPointer, member->Name, memberFn, depth + 1); break; \
	case MetaType_Vector4: InspectStruct(Vector4Members, (sizeof(Vector4Members) / sizeof(Vector4Members[0])), (Vector4*)MemberPointer, member->Name, memberFn, depth + 1); break; \
	case MetaType_Matrix4: InspectStruct(Matrix4Members, (sizeof(Matrix4Members) / sizeof(Matrix4Members[0])), (Matrix4*)MemberPointer, member->Name, memberFn, depth + 1); break; \
	case MetaType_TagComponent: InspectStruct(TagComponentMembers, (sizeof(TagComponentMembers) / sizeof(TagComponentMembers[0])), (TagComponent*)MemberPointer, member->Name, memberFn, depth + 1); break; \
	case MetaType_TransformComponent: InspectStruct(TransformComponentMembers, (sizeof(TransformComponentMembers) / sizeof(TransformComponentMembers[0])), (TransformComponent*)MemberPointer, member->Name, memberFn, depth + 1); break; \
	case MetaType_OrthographicCameraComponent: InspectStruct(OrthographicCameraComponentMembers, (sizeof(OrthographicCameraComponentMembers) / sizeof(OrthographicCameraComponentMembers[0])), (OrthographicCameraComponent*)MemberPointer, member->Name, memberFn, depth + 1); break; \
	case MetaType_PerspectiveCameraComponent: InspectStruct(PerspectiveCameraComponentMembers, (sizeof(PerspectiveCameraComponentMembers) / sizeof(PerspectiveCameraComponentMembers[0])), (PerspectiveCameraComponent*)MemberPointer, member->Name, memberFn, depth + 1); break; \
	case MetaType_SpriteRendererComponent: InspectStruct(SpriteRendererComponentMembers, (sizeof(SpriteRendererComponentMembers) / sizeof(SpriteRendererComponentMembers[0])), (SpriteRendererComponent*)MemberPointer, member->Name, memberFn, depth + 1); break; \
	case MetaType_Settings: InspectStruct(SettingsMembers, (sizeof(SettingsMembers) / sizeof(SettingsMembers[0])), (Settings*)MemberPointer, member->Name, memberFn, depth + 1); break; \
	case MetaType_Vars: InspectStruct(VarsMembers, (sizeof(VarsMembers) / sizeof(VarsMembers[0])), (Vars*)MemberPointer, member->Name, memberFn, depth + 1); break; \
