#pragma once

#include "Vision/Meta/Meta.h"

#include "Vision/Entity/Components.h"

static StructMember Vector2Members[] = 
{
	{ "x", MetaType_float32, offsetof(Vision::Vector2, x), 1, 0, 0 },
	{ "y", MetaType_float32, offsetof(Vision::Vector2, y), 1, 0, 0 },
};

static StructMember Vector3Members[] = 
{
	{ "x", MetaType_float32, offsetof(Vision::Vector3, x), 1, 0, 0 },
	{ "y", MetaType_float32, offsetof(Vision::Vector3, y), 1, 0, 0 },
	{ "z", MetaType_float32, offsetof(Vision::Vector3, z), 1, 0, 0 },
};

static StructMember Vector4Members[] = 
{
	{ "x", MetaType_float32, offsetof(Vision::Vector4, x), 1, 0, 0 },
	{ "y", MetaType_float32, offsetof(Vision::Vector4, y), 1, 0, 0 },
	{ "z", MetaType_float32, offsetof(Vision::Vector4, z), 1, 0, 0 },
	{ "w", MetaType_float32, offsetof(Vision::Vector4, w), 1, 0, 0 },
};

static StructMember Matrix4Members[] = 
{
	{ "elements", MetaType_float32, offsetof(Vision::Matrix4, elements), 4, 1, 1 },
};

static StructMember TagComponentMembers[] = 
{
	{ "Tag", MetaType_char, offsetof(Vision::TagComponent, Tag), 1, 0, 1 },
};

static StructMember TransformComponentMembers[] = 
{
	{ "Position", MetaType_Vector3, offsetof(Vision::TransformComponent, Position), 1, 0, 0 },
	{ "Rotation", MetaType_Vector3, offsetof(Vision::TransformComponent, Rotation), 1, 0, 0 },
	{ "Scale", MetaType_Vector3, offsetof(Vision::TransformComponent, Scale), 1, 0, 0 },
};

static StructMember OrthographicCameraComponentMembers[] = 
{
	{ "AspectRatio", MetaType_float32, offsetof(Vision::OrthographicCameraComponent, AspectRatio), 1, 0, 0 },
	{ "Size", MetaType_float32, offsetof(Vision::OrthographicCameraComponent, Size), 1, 0, 0 },
	{ "Near", MetaType_float32, offsetof(Vision::OrthographicCameraComponent, Near), 1, 0, 0 },
	{ "Far", MetaType_float32, offsetof(Vision::OrthographicCameraComponent, Far), 1, 0, 0 },
	{ "Projection", MetaType_Matrix4, offsetof(Vision::OrthographicCameraComponent, Projection), 1, 0, 0 },
	{ "Static", MetaType_bool, offsetof(Vision::OrthographicCameraComponent, Static), 1, 0, 0 },
};

static StructMember PerspectiveCameraComponentMembers[] = 
{
	{ "AspectRatio", MetaType_float32, offsetof(Vision::PerspectiveCameraComponent, AspectRatio), 1, 0, 0 },
	{ "FieldOfView", MetaType_float32, offsetof(Vision::PerspectiveCameraComponent, FieldOfView), 1, 0, 0 },
	{ "Near", MetaType_float32, offsetof(Vision::PerspectiveCameraComponent, Near), 1, 0, 0 },
	{ "Far", MetaType_float32, offsetof(Vision::PerspectiveCameraComponent, Far), 1, 0, 0 },
	{ "Projection", MetaType_Matrix4, offsetof(Vision::PerspectiveCameraComponent, Projection), 1, 0, 0 },
	{ "Static", MetaType_bool, offsetof(Vision::PerspectiveCameraComponent, Static), 1, 0, 0 },
};

static StructMember SpriteRendererComponentMembers[] = 
{
	{ "Texture", MetaType_uint32, offsetof(Vision::SpriteRendererComponent, Texture), 1, 0, 0 },
	{ "Color", MetaType_Vector4, offsetof(Vision::SpriteRendererComponent, Color), 1, 0, 0 },
	{ "BottomLeftUV", MetaType_Vector2, offsetof(Vision::SpriteRendererComponent, BottomLeftUV), 1, 0, 0 },
	{ "TopRightUV", MetaType_Vector2, offsetof(Vision::SpriteRendererComponent, TopRightUV), 1, 0, 0 },
};

