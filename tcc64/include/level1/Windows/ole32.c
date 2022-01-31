#ifndef WINDOWS_OLE32_INCLUDED
#define WINDOWS_OLE32_INCLUDED


#include <types.c>


typedef enum
{
	COM_OK = 0,
	COM_FALSE = 1
}
COM_Result;

typedef enum
{
	CLSCTX_INPROC_SERVER          = 0x1,
	CLSCTX_INPROC_HANDLER         = 0x2,
	CLSCTX_LOCAL_SERVER           = 0x4,
	CLSCTX_INPROC_SERVER16        = 0x8,
	CLSCTX_REMOTE_SERVER          = 0x10,
	CLSCTX_INPROC_HANDLER16       = 0x20,
	CLSCTX_RESERVED1              = 0x40,
	CLSCTX_RESERVED2              = 0x80,
	CLSCTX_RESERVED3              = 0x100,
	CLSCTX_RESERVED4              = 0x200,
	CLSCTX_NO_CODE_DOWNLOAD       = 0x400,
	CLSCTX_RESERVED5              = 0x800,
	CLSCTX_NO_CUSTOM_MARSHAL      = 0x1000,
	CLSCTX_ENABLE_CODE_DOWNLOAD   = 0x2000,
	CLSCTX_NO_FAILURE_LOG         = 0x4000,
	CLSCTX_DISABLE_AAA            = 0x8000,
	CLSCTX_ENABLE_AAA             = 0x10000,
	CLSCTX_FROM_DEFAULT_CONTEXT   = 0x20000,
	CLSCTX_ACTIVATE_32_BIT_SERVER = 0x40000,
	CLSCTX_ACTIVATE_64_BIT_SERVER = 0x80000,
	CLSCTX_ENABLE_CLOAKING        = 0x100000,
	CLSCTX_APPCONTAINER           = 0x400000,
	CLSCTX_ACTIVATE_AAA_AS_IU     = 0x800000,
	CLSCTX_PS_DLL                 = 0x80000000
}
Class_Context;


typedef struct
{
	Number32 a;
	Number16 b;
	Number16 c;
	Byte     d;
	Byte     e;
	Byte     f;
	Byte     g;
	Byte     h;
	Byte     i;
	Byte     j;
	Byte     k;
}
GUID;


#define IUnknown_InterfaceID {0x00000000, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
typedef struct
{
	stdcall COM_Result (*QueryInterface) (Byte* this, GUID* ref_id, Byte* object);
	stdcall Number32   (*AddRef)         (Byte* this);
	stdcall Number32   (*Release)        (Byte* this);
}
IUnknown;


typedef struct
{
	IUnknown* virtual_table;
}
COM_Object;


import COM_Result CoInitialize(void* reserved);
import void       CoUninitialize();
import COM_Result CoCreateInstance(GUID* class_id, COM_Object* outer, Class_Context class_context, GUID* interface_id, COM_Object** object);
import COM_Result CoGetObject(Number16* name, void* options, GUID* class_id, COM_Object** object);
import COM_Result CoCreateGuid(GUID* id);


#endif//WINDOWS_OLE32_INCLUDED


/*
#define PropertyStore_ClassID {0x9a02e012, 0x6303, 0x4e1e, 0xb9, 0xa1, 0x63, 0x0f 0x80, 0x25, 0x92, 0xc5} //9a02e012-6303-4e1e-b9a1-630f802592c5

#define PropertyStore_InterfaceID {0x886d8eeb, 0x8cf2, 0x4446, 0x8d, 0x02, 0xcd, 0xba, 0x1d, 0xbd, 0xcf, 0x99} //886d8eeb-8cf2-4446-8d02-cdba1dbdcf99
typedef struct
{
	IUnknown unknown;

	stdcall COM_Result (*GetCount) (Byte* this, Number32* properties);
	stdcall COM_Result (*GetAt)    (Byte* this, Number32 property, void* key);
	stdcall COM_Result (*GetValue) (Byte* this, void* key, void** value);
	stdcall COM_Result (*SetValue) (Byte* this, void* key, void* value);
	stdcall COM_Result (*Commit)   (Byte* this);
}
IPropertyStore;


#define WICImagingFactory_ClassID {0xcacaf262, 0x9370, 0x4615, 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0xa} //0xcacaf262, 0x9370, 0x4615, 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0xa

#define WICBitmapDecoder_InterfaceID {0x9EDDE9E7, 0x8DEE, 0x47ea, 0x99, 0xDF, 0xE6, 0xFA, 0xF2, 0xED, 0x44, 0xBF} //9EDDE9E7-8DEE-47ea-99DF-E6FAF2ED44BF
typedef struct
{
	IUnknown unknown;

	stdcall COM_Result (*QueryCapability)        (Byte* this, void* pIStream, Number32* pdwCapability);
	stdcall COM_Result (*Initialize)             (Byte* this, void* pIStream, Number32 cacheOptions);
	stdcall COM_Result (*GetContainerFormat)     (Byte* this, GUID *pguidContainerFormat);
	stdcall COM_Result (*GetDecoderInfo)         (Byte* this, void** ppIDecoderInfo);
	stdcall COM_Result (*CopyPalette)            (Byte* this, void* pIPalette);
	stdcall COM_Result (*GetMetadataQueryReader) (Byte* this, void** ppIMetadataQueryReader);
	stdcall COM_Result (*GetPreview)             (Byte* this, void** ppIBitmapSource);
	stdcall COM_Result (*GetColorContexts)       (Byte* this, Number32 cCount, void** ppIColorContexts, Number32* pcActualCount);
	stdcall COM_Result (*GetThumbnail)           (Byte* this, void** ppIThumbnail);
	stdcall COM_Result (*GetFrameCount)          (Byte* this, Number32* pCount);
	stdcall COM_Result (*GetFrame)               (Byte* this, Number32 index, void** ppIBitmapFrame);
}
IBitmapDecoder;


#define PropertyThumbnailHandler_ClassID {0x9DBD2C50, 0x62AD, 0x11D0, 0xB8, 0x06, 0x00, 0xC0, 0x4F, 0xD7, 0x06, 0xEC} //{9DBD2C50-62AD-11D0-B806-00C04FD706EC}

#define IExtractImage_InterfaceID {0xBB2E617C, 0x0920, 0x11d1, 0x9A, 0x0B, 0x00, 0xC0, 0x4F, 0xC2, 0xD6, 0xC1} //BB2E617C-0920-11d1-9A0B-00C04FC2D6C1
typedef struct
{
	IUnknown unknown;

	stdcall COM_Result (*GetLocation)(Byte* this, Number16* path, Number32 cch, Number32* priority, Number* size, Number32 rec_clr_depth, Number32* flags);
	stdcall COM_Result (*Extract)(Byte* this, Byte* bitmap); //HBITMAP!!!
}
IExtractImage;


Number32 main()
{
	GUID            property_thumbnail_handler_GUID = PropertyThumbnailHandler_ClassID;
	GUID            extract_image_GUID = IExtractImage_InterfaceID;
	COM_Object*     extract_image_object;
	IExtractImage*  extract_image;

	if(CoInitialize(0))
		goto error;
	
	if(CoCreateInstance(&property_thumbnail_handler_GUID, 0, CLSCTX_INPROC_SERVER, &extract_image_GUID, &extract_image_object))
		goto error;

	extract_image = extract_image_object->virtual_table;

	print(
		_Number, extract_image->unknown.AddRef(extract_image_object)
		//_Number, extract_image->Extract(extract_image_object, 0)
	);

	CoUninitialize();

	return 0;

error:
	return 1;
*/