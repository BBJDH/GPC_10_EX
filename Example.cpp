#include <cassert>
#include <D3D11.h>

#include "FreeImage.h"

#if not defined _DEBUG
#define MUST(Expression) (      (         (Expression)))
#else
#define MUST(Expression) (assert(SUCCEEDED(Expression)))
#endif
    
namespace Example
{
    namespace
    {
        ID3D11Device        * Device;        //Create, ...
        ID3D11DeviceContext * DeviceContext; //Set, Draw, ...

        IDXGISwapChain * SwapChain; //Present, ...

        namespace Buffer
        {
            ID3D11Buffer * Vertex;
            
            
            ID3D11Buffer* Constant[3];

            template<typename Data>
            void Update(ID3D11Buffer* const buffer, Data const& data)
            {
                D3D11_MAPPED_SUBRESOURCE Subresource = D3D11_MAPPED_SUBRESOURCE();

                MUST(DeviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource));
                {
                    memcpy_s(Subresource.pData, Subresource.RowPitch, data, sizeof(data));
                }
                DeviceContext->Unmap(buffer, 0);
            };
            
        }

        ID3D11RenderTargetView * RenderTargetView;
    }

    LRESULT CALLBACK Procedure(HWND const hWindow, UINT const uMessage, WPARAM const wParameter, LPARAM const lParameter)
    {
        switch(uMessage)
        {
            case WM_CREATE:
            {
                {
                    DXGI_SWAP_CHAIN_DESC Descriptor = DXGI_SWAP_CHAIN_DESC();

                    Descriptor.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                    Descriptor.SampleDesc.Count  = 1;
                    Descriptor.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                    Descriptor.BufferCount       = 1;
                    Descriptor.OutputWindow      = hWindow;
                    Descriptor.Windowed          = true;
                    Descriptor.Flags             = 0;

                    MUST(D3D11CreateDeviceAndSwapChain
                    (
                        nullptr,
                        D3D_DRIVER_TYPE_HARDWARE,
                        nullptr,
                        D3D11_CREATE_DEVICE_SINGLETHREADED,
                        nullptr,
                        0,
                        D3D11_SDK_VERSION,
                        &Descriptor,
                        &SwapChain,
                        &Device,
                        nullptr,
                        &DeviceContext
                    ));
                }
                {
                    #include "Shader/Bytecode/Vertex.h"
                    {
                        D3D11_INPUT_ELEMENT_DESC const Descriptors[2]
                        {
                            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0 },
                            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1 }
                        };

                        ID3D11InputLayout * InputLayout = nullptr;
                        
                        MUST(Device->CreateInputLayout
                        (
                            Descriptors,
                            _ARRAYSIZE(Descriptors),
                            Bytecode,
                            sizeof(Bytecode),
                            &InputLayout
                        ));

                        DeviceContext->IASetInputLayout(InputLayout);

                        InputLayout->Release();
                    }
                    {
                        ID3D11VertexShader * VertexShader = nullptr;
                        
                        MUST(Device->CreateVertexShader
                        (
                            Bytecode,
                            sizeof(Bytecode),
                            nullptr,
                            &VertexShader
                        ));

                        DeviceContext->VSSetShader(VertexShader, nullptr, 0);

                        VertexShader->Release();
                    }
                }
                {
                    #include "Shader/Bytecode/Pixel.h"
                    {
                        ID3D11PixelShader * PixelShader = nullptr;

                        MUST(Device->CreatePixelShader
                        (
                            Bytecode,
                            sizeof(Bytecode),
                            nullptr,
                            &PixelShader
                        ));

                        DeviceContext->PSSetShader(PixelShader, nullptr, 0);

                        PixelShader->Release();
                    }
                }
                {
                    DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

                    float const Coordinates[4][2]
                    {
                        { -0.5f, +0.5f },
                        { +0.5f, +0.5f },
                        { -0.5f, -0.5f },
                        { +0.5f, -0.5f }
                    };

                    D3D11_BUFFER_DESC const Descriptor
                    {
                        sizeof(Coordinates),
                        D3D11_USAGE_IMMUTABLE,
                        D3D11_BIND_VERTEX_BUFFER,
                        0,
                    };

                    D3D11_SUBRESOURCE_DATA const Subresource
                    {
                        Coordinates,
                    };

                    ID3D11Buffer * Buffer = nullptr;

                    MUST(Device->CreateBuffer(&Descriptor, &Subresource, &Buffer));

                    UINT const Stride = sizeof(*Coordinates);
                    UINT const Offset = 0;

                    DeviceContext->IASetVertexBuffers(0, 1, &Buffer, &Stride, &Offset);

                    Buffer->Release();
                }
                {
                    D3D11_BUFFER_DESC const Descriptor
                    {
                        sizeof(float[4][2]),
                        D3D11_USAGE_DYNAMIC,
                        D3D11_BIND_VERTEX_BUFFER,
                        D3D11_CPU_ACCESS_WRITE,
                    };

                    MUST(Device->CreateBuffer(&Descriptor, nullptr, &Buffer::Vertex));

                    UINT const Stride = sizeof(float[2]);
                    UINT const Offset = 0;

                    DeviceContext->IASetVertexBuffers(1, 1, &Buffer::Vertex, &Stride, &Offset);
                }
                
                {
                    D3D11_BUFFER_DESC const Descriptor
                    {
                        sizeof(float[4][4]),
                        D3D11_USAGE_DYNAMIC,
                        D3D11_BIND_CONSTANT_BUFFER,
                        D3D11_CPU_ACCESS_WRITE
                    };

                    for (UINT u = 0; u < 3; ++u)
                        MUST(Device->CreateBuffer(&Descriptor, nullptr, &Buffer::Constant[u]));

                    DeviceContext->VSSetConstantBuffers(0, 3, Buffer::Constant);
                }
                {
                    D3D11_BLEND_DESC Descriptor = D3D11_BLEND_DESC();

                    Descriptor.RenderTarget->BlendEnable           = true;
                    Descriptor.RenderTarget->SrcBlend              = D3D11_BLEND_SRC_ALPHA;
                    Descriptor.RenderTarget->DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
                    Descriptor.RenderTarget->BlendOp               = D3D11_BLEND_OP_ADD;
                    Descriptor.RenderTarget->SrcBlendAlpha         = D3D11_BLEND_ZERO;
                    Descriptor.RenderTarget->DestBlendAlpha        = D3D11_BLEND_ONE;
                    Descriptor.RenderTarget->BlendOpAlpha          = D3D11_BLEND_OP_ADD;
                    Descriptor.RenderTarget->RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

                    ID3D11BlendState* BlendState = nullptr;

                    MUST(Device->CreateBlendState(&Descriptor, &BlendState));

                    //float const BlendFactor[4] { 0.1f, 0.1f, 0.1f, 0.1f } ;

                    DeviceContext->OMSetBlendState(BlendState, nullptr, D3D11_DEFAULT_SAMPLE_MASK);

                    BlendState->Release();
                }            
                {
                    char const * const File = "Asset/Fio[6].png";

                    FreeImage_Initialise();
                    {
                        FIBITMAP * Bitmap = FreeImage_Load(FreeImage_GetFileType(File), File);
                        {
                            FreeImage_FlipVertical(Bitmap);

                            if(FreeImage_GetBPP(Bitmap) != 32)
                            {
                                FIBITMAP * const Previous = Bitmap;

                                Bitmap = FreeImage_ConvertTo32Bits(Bitmap);

                                FreeImage_Unload(Previous);
                            }
                        }
                        {
                            D3D11_TEXTURE2D_DESC Descriptor = D3D11_TEXTURE2D_DESC();

                            Descriptor.Width              = FreeImage_GetWidth(Bitmap);
                            Descriptor.Height             = FreeImage_GetHeight(Bitmap);
                            Descriptor.MipLevels          = 1;
                            Descriptor.ArraySize          = 1;
                            Descriptor.Format             = DXGI_FORMAT_B8G8R8A8_UNORM;
                            Descriptor.SampleDesc.Count   = 1;
                            Descriptor.SampleDesc.Quality = 0;
                            Descriptor.Usage              = D3D11_USAGE_IMMUTABLE;
                            Descriptor.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
                            Descriptor.CPUAccessFlags     = 0;
                            Descriptor.MiscFlags          = 0;

                            D3D11_SUBRESOURCE_DATA Subresource = D3D11_SUBRESOURCE_DATA();

                            Subresource.pSysMem          = FreeImage_GetBits(Bitmap);
                            Subresource.SysMemPitch      = FreeImage_GetPitch(Bitmap);
                            Subresource.SysMemSlicePitch = 0;

                            ID3D11Texture2D * Texture2D = nullptr;

                            MUST(Device->CreateTexture2D(&Descriptor, &Subresource, &Texture2D));
                            {
                                ID3D11ShaderResourceView * ShaderResourceView = nullptr;

                                MUST(Device->CreateShaderResourceView(Texture2D, nullptr, &ShaderResourceView));

                                DeviceContext->PSSetShaderResources(0, 1, &ShaderResourceView);

                                ShaderResourceView->Release();
                            }
                            Texture2D->Release();
                        }
                        FreeImage_Unload(Bitmap);
                    }
                    FreeImage_DeInitialise();
                }

                return 0;
            }
            case WM_APP:
            {
                {                 
                    {
                        static struct
                        {
                            float const Width = 384.0f;
                            float const Height = 384.0f;
                        }
                        Frame;

                        unsigned static Count = 0;
                        unsigned static FPM = 10;
                        unsigned static Motion = 6;

                        /*static struct
                        {
                            float const Width = 156;
                            float const Height = 160;
                        }
                        Frame;

                        static unsigned Count = 0;
                        static unsigned Motion = 9;
                        static unsigned FPM = 10;*/

                        float const Coordinates[4][2]
                        {
                            { Frame.Width * (Count / FPM + 0.0f), Frame.Height * 0.0f },
                            { Frame.Width * (Count / FPM + 1.0f), Frame.Height * 0.0f },
                            { Frame.Width * (Count / FPM + 0.0f), Frame.Height * 1.0f },
                            { Frame.Width * (Count / FPM + 1.0f), Frame.Height * 1.0f }
                        };

                        ((Count += 1) %= FPM * Motion);

                        Buffer::Update(Buffer::Vertex, Coordinates);
                    }
                }
                {
                    {
                        float const W = 150; 
                        float const H = 150; 

                        float const X = 0;
                        float const Y = 0;

                        float const Transform[4][4]
                        {
                            W, 0, 0, X,
                            0, H, 0, Y,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                        };

                        Buffer::Update(Buffer::Constant[0], Transform);
                    }
                    {
                        float const Transform[4][4]
                        {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                        };

                        Buffer::Update(Buffer::Constant[1], Transform);
                    }
                    {
                        float const X = 2.0f / 500.0f;
                        float const Y = 2.0f / 500.0f;

                        float const Transform[4][4]
                        {
                            X, 0, 0, 0,
                            0, Y, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1
                        };

                        Buffer::Update(Buffer::Constant[2], Transform);
                    }
                }
                {
                    DeviceContext->Draw(4, 0);

                    MUST(SwapChain->Present(true, 0));

                    float const Color[4] { 0.0f, 0.0f, 0.0f, 1.0f };

                    DeviceContext->ClearRenderTargetView(RenderTargetView, Color);
                }

                return 0;
            } 
            case WM_DESTROY:
            {
                DeviceContext->ClearState();

                RenderTargetView->Release();

                for (UINT u = 0; u < 3; ++u)
                    Buffer::Constant[u]->Release();

                Buffer::Vertex->Release();

                SwapChain->Release();

                DeviceContext->Release();
                Device->Release();

                PostQuitMessage(0);

                return 0;
            }
            case WM_SIZE:
            {
                {
                    D3D11_VIEWPORT const Viewport
                    {
                        0.0f,
                        0.0f,
                        static_cast<float>(LOWORD(lParameter)),
                        static_cast<float>(HIWORD(lParameter)),
                    };

                    DeviceContext->RSSetViewports(1, &Viewport);
                }
                {
                    if(RenderTargetView != nullptr)
                    {
                        RenderTargetView->Release();

                        MUST(SwapChain->ResizeBuffers
                        (
                            1,
                            LOWORD(lParameter),
                            HIWORD(lParameter),
                            DXGI_FORMAT_B8G8R8A8_UNORM,
                            0
                        ));
                    }
                    {
                        ID3D11Texture2D * Texture2D = nullptr;

                        MUST(SwapChain->GetBuffer(0, IID_PPV_ARGS(&Texture2D)));
                        {
                            MUST(Device->CreateRenderTargetView(Texture2D, nullptr, &RenderTargetView));
                        }
                        Texture2D->Release();

                        DeviceContext->OMSetRenderTargets(1, &RenderTargetView, nullptr);
                    }
                }

                return 0;
            }
            default:
            {
                return DefWindowProc(hWindow, uMessage, wParameter, lParameter);
            }
        }
    }
}