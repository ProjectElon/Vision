#pragma once

#include "Meta.h"

#include "../src/Vision/Entity/Components.h"

static StructMember Vector2Members[] = 
{
	{ "x", MetaType_float32, offsetof(Vision::Vector2, x), 0, sizeof(float32), 8 },
	{ "y", MetaType_float32, offsetof(Vision::Vector2, y), 0, sizeof(float32), 8 },
};

static StructMember Vector3Members[] = 
{
	{ "x", MetaType_float32, offsetof(Vision::Vector3, x), 0, sizeof(float32), 8 },
	{ "y", MetaType_float32, offsetof(Vision::Vector3, y), 0, sizeof(float32), 8 },
	{ "z", MetaType_float32, offsetof(Vision::Vector3, z), 0, sizeof(float32), 8 },
};

static StructMember Vector4Members[] = 
{
	{ "x", MetaType_float32, offsetof(Vision::Vector4, x), 0, sizeof(float32), 8 },
	{ "y", MetaType_float32, offsetof(Vision::Vector4, y), 0, sizeof(float32), 8 },
	{ "z", MetaType_float32, offsetof(Vision::Vector4, z), 0, sizeof(float32), 8 },
	{ "w", MetaType_float32, offsetof(Vision::Vector4, w), 0, sizeof(float32), 8 },
};

static StructMember Matrix4Members[] = 
{
	{ "elements", MetaType_float32, offsetof(Vision::Matrix4, elements), 4, sizeof(float32), 56 },
};

static StructMember TagComponentMembers[] = 
{
	{ "Tag", MetaType_String, offsetof(Vision::TagComponent, Tag), 0, sizeof(Vision::String), 0 },
};

static StructMember TransformComponentMembers[] = 
{
	{ "Position", MetaType_Vector3, offsetof(Vision::TransformComponent, Position), 0, sizeof(Vision::Vector3), 0 },
	{ "Rotation", MetaType_Vector3, offsetof(Vision::TransformComponent, Rotation), 0, sizeof(Vision::Vector3), 0 },
	{ "Scale", MetaType_Vector3, offsetof(Vision::TransformComponent, Scale), 0, sizeof(Vision::Vector3), 0 },
};

static StructMember OrthographicCameraComponentMembers[] = 
{
	{ "AspectRatio", MetaType_float32, offsetof(Vision::OrthographicCameraComponent, AspectRatio), 0, sizeof(float32), 8 },
	{ "Size", MetaType_float32, offsetof(Vision::OrthographicCameraComponent, Size), 0, sizeof(float32), 8 },
	{ "Near", MetaType_float32, offsetof(Vision::OrthographicCameraComponent, Near), 0, sizeof(float32), 8 },
	{ "Far", MetaType_float32, offsetof(Vision::OrthographicCameraComponent, Far), 0, sizeof(float32), 8 },
	{ "Projection", MetaType_Matrix4, offsetof(Vision::OrthographicCameraComponent, Projection), 0, sizeof(Vision::Matrix4), 0 },
	{ "Static", MetaType_bool, offsetof(Vision::OrthographicCameraComponent, Static), 0, sizeof(bool), 8 },
};

static StructMember PerspectiveCameraComponentMembers[] = 
{
	{ "AspectRatio", MetaType_float32, offsetof(Vision::PerspectiveCameraComponent, AspectRatio), 0, sizeof(float32), 8 },
	{ "FieldOfView", MetaType_float32, offsetof(Vision::PerspectiveCameraComponent, FieldOfView), 0, sizeof(float32), 8 },
	{ "Near", MetaType_float32, offsetof(Vision::PerspectiveCameraComponent, Near), 0, sizeof(float32), 8 },
	{ "Far", MetaType_float32, offsetof(Vision::PerspectiveCameraComponent, Far), 0, sizeof(float32), 8 },
	{ "Projection", MetaType_Matrix4, offsetof(Vision::PerspectiveCameraComponent, Projection), 0, sizeof(Vision::Matrix4), 0 },
	{ "Static", MetaType_bool, offsetof(Vision::PerspectiveCameraComponent, Static), 0, sizeof(bool), 8 },
};

static StructMember SpriteRendererComponentMembers[] = 
{
	{ "Texture", MetaType_uint32, offsetof(Vision::SpriteRendererComponent, Texture), 0, sizeof(uint32), 8 },
	{ "Color", MetaType_Vector4, offsetof(Vision::SpriteRendererComponent, Color), 0, sizeof(Vision::Vector4), 0 },
	{ "BottomLeftUV", MetaType_Vector2, offsetof(Vision::SpriteRendererComponent, BottomLeftUV), 0, sizeof(Vision::Vector2), 0 },
	{ "TopRightUV", MetaType_Vector2, offsetof(Vision::SpriteRendererComponent, TopRightUV), 0, sizeof(Vision::Vector2), 0 },
};

static StructMember MeshRendererComponentMembers[] = 
{
	{ "MeshAssetID", MetaType_uint32, offsetof(Vision::MeshRendererComponent, MeshAssetID), 0, sizeof(uint32), 8 },
	{ "TextureAssetID", MetaType_uint32, offsetof(Vision::MeshRendererComponent, TextureAssetID), 0, sizeof(uint32), 8 },
	{ "ShaderAssetID", MetaType_uint32, offsetof(Vision::MeshRendererComponent, ShaderAssetID), 0, sizeof(uint32), 8 },
};

