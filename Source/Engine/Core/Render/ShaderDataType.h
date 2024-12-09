#pragma once
#include "pch.h"
namespace ST
{
    enum  ShaderDataType
    {
        None = 0,
        Float1,
        Float2,
        Float3,
        Float4,
        Int1,
        Int2,
        Int3,
        Int4,
        Bool,
        Mat3,
        Mat4
    };
    static int GetShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::None:
            return -1;
        case ShaderDataType::Float1:
            return 4*1;
        case ShaderDataType::Float2:
            return 4*2;
        case ShaderDataType::Float3:
            return 4*3;
        case ShaderDataType::Float4:
            return 4*4;
        case ShaderDataType::Int1:
            return 4*1;
        case ShaderDataType::Int2:
            return 4*2;
        case ShaderDataType::Int3:
            return 4*3;
        case ShaderDataType::Int4:
            return 4*4;
        case ShaderDataType::Bool:
            return 1;
        case ShaderDataType::Mat3:
            return 4*3*3;
        case ShaderDataType::Mat4:
            return 4*4*4;
        }
        return -1;
    }
    static int GetShaderDataTypeCount(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::None:
            return -1;
        case ShaderDataType::Float1:
            return 1;
        case ShaderDataType::Float2:
            return 2;
        case ShaderDataType::Float3:
            return 3;
        case ShaderDataType::Float4:
            return 4;
        case ShaderDataType::Int1:
            return 1;
        case ShaderDataType::Int2:
            return 2;
        case ShaderDataType::Int3:
            return 3;
        case ShaderDataType::Int4:
            return 4;
        case ShaderDataType::Bool:
            return 1;
        case ShaderDataType::Mat3:
            return 3*3;
        case ShaderDataType::Mat4:
            return 4*4;
        }
        return -1;
    }
    static GLenum ShaderDataType2GLType(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::None:
            return -1;
        case ShaderDataType::Float1:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
            return GL_FLOAT;
        case ShaderDataType::Int1:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
            return GL_INT;
        case ShaderDataType::Bool:
            return GL_BOOL;
        }
        return -1;
    }
}