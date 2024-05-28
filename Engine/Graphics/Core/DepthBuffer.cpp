#include "DepthBuffer.h"

#include "Helper.h"
#include "Graphics.h"

void DepthBuffer::Create(const std::wstring& name, uint32_t width, uint32_t height, DXGI_FORMAT format) {
    auto flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    auto desc = DescribeTex2D(width, height, 1, format, flags);

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = format;
    clearValue.DepthStencil.Depth = clearValue_;

    CreateTextureResource(name, desc, clearValue);
    CreateViews();
}

void DepthBuffer::CreateArray(const std::wstring& name, uint32_t width, uint32_t height, uint32_t arraySize, DXGI_FORMAT format) {
    auto flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    auto desc = DescribeTex2D(width, height, arraySize, format, flags);

    D3D12_CLEAR_VALUE clearValue{};
    clearValue.Format = format;
    clearValue.DepthStencil.Depth = clearValue_;

    CreateTextureResource(name, desc, clearValue);
    CreateViews();
}

void DepthBuffer::CreateViews() {
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.Format = Helper::GetDSVFormat(format_);
    srvDesc.Format = Helper::GetDepthFormat(format_);
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    if (arraySize_ > 1) {
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
        dsvDesc.Texture2DArray.MipSlice = 0;
        dsvDesc.Texture2DArray.FirstArraySlice = 0;
        dsvDesc.Texture2DArray.ArraySize = UINT(arraySize_);

        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.MipLevels = 1;
        srvDesc.Texture2DArray.MostDetailedMip = 0;
        srvDesc.Texture2DArray.FirstArraySlice = 0;
        srvDesc.Texture2DArray.ArraySize = UINT(arraySize_);
    }
    else {
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Texture2D.MipSlice = 0;

        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.MostDetailedMip = 0;
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDescs[NumDSVs]{};
    for (uint32_t i = 0; i < (uint32_t)NumDSVs; ++i) {
        dsvDescs[i] = dsvDesc;
    }
    dsvDescs[ReadOnly].Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;

    auto graphics = Graphics::GetInstance();
    auto device = graphics->GetDevice();
    for (uint32_t i = 0; i < (uint32_t)NumDSVs; ++i) {
        if (dsvHandles_[i].IsNull()) {
            dsvHandles_[i] = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        }
        device->CreateDepthStencilView(resource_.Get(), &dsvDescs[i], dsvHandles_[i]);
    }
    if (srvHandle_.IsNull()) {
        srvHandle_ = graphics->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);

}
