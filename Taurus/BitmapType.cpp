#include "Common.h"

void ImageC::operator delete(void* in_pVoid)
{
	API(GDIPLUS, GdipFree)(in_pVoid);
}
void* ImageC::operator new(size_t in_size)
{
	return API(GDIPLUS, GdipAlloc)(in_size);
}
void ImageC::operator delete[](void* in_pVoid)
{
	API(GDIPLUS, GdipFree)(in_pVoid);
}
void* ImageC::operator new[](size_t in_size)
{
	return API(GDIPLUS, GdipAlloc)(in_size);
}

ImageC::~ImageC()
{
	API(GDIPLUS, GdipDisposeImage)(nativeImage);
}

ImageC* ImageC::Clone()
{
	GpImage* cloneimage = NULL;
	SetStatus(API(GDIPLUS, GdipCloneImage)(nativeImage, &cloneimage));
	return new ImageC(cloneimage, lastResult);
}

Status ImageC::Save(IN IStream* stream, IN const CLSID* clsidEncoder, IN const EncoderParameters* encoderParams)
{
	return SetStatus(API(GDIPLUS, GdipSaveImageToStream)(nativeImage, stream, clsidEncoder, encoderParams));
}

void BitmapC::operator delete(void* in_pVoid)
{
	API(GDIPLUS, GdipFree)(in_pVoid);
}
void* BitmapC::operator new(size_t in_size)
{
	return API(GDIPLUS, GdipAlloc)(in_size);
}
void BitmapC::operator delete[](void* in_pVoid)
{
	API(GDIPLUS, GdipFree)(in_pVoid);
}
void* BitmapC::operator new[](size_t in_size)
{
	return API(GDIPLUS, GdipAlloc)(in_size);
}

BitmapC::BitmapC(IN HBITMAP hbm, IN HPALETTE hpal)
{
	GpBitmap* bitmap = NULL;
	API(GDIPLUS, GdipCreateBitmapFromHBITMAP)(hbm, hpal, &bitmap);
	SetNativeImage(bitmap);
}