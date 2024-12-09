#include "VertexArray.h"
#include "pch.h"
#include "Buffer.h"
namespace ST
{
    VertexArray::VertexArray()
    {
        glGenVertexArrays(1,&_arraryId);
        glBindVertexArray(_arraryId);
    }

    void VertexArray::AddVertexBuffer(ST_REF<VertexBuffer> vertexBuffer)
    {
        glBindVertexArray(_arraryId);
        vertexBuffer->Bind();

        _vertexBuffers.push_back(vertexBuffer);
        int stride=0;
        for(auto& element : vertexBuffer->GetBufferLayout())
        {
            stride+=GetShaderDataTypeSize(element._type);
        }
        int index=0;
        int offset=0;
        for(auto& element : vertexBuffer->GetBufferLayout())
        {
            glVertexAttribPointer(
                index,
                GetShaderDataTypeCount(element._type),
                ShaderDataType2GLType(element._type),
                element.normalized? GL_TRUE:GL_FALSE,
                stride,
                (void*)offset);
            glEnableVertexAttribArray(index);
            offset+=GetShaderDataTypeSize(element._type);
            ++index;
        }
    }

    void VertexArray::SetIndexBuffer(ST_REF<IndexBuffer> indexBuffer)
    {
        glBindVertexArray(_arraryId);
        indexBuffer->Bind();
        _indexBuffer=indexBuffer;

    }
}