#pragma once

#include "SDL2/SDL.h"

#define DotProduct(x, y) ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2])
#define VectorSubtract(a, b, c)   \
	{                             \
		(c)[0] = (a)[0] - (b)[0]; \
		(c)[1] = (a)[1] - (b)[1]; \
		(c)[2] = (a)[2] - (b)[2]; \
	}
#define VectorAdd(a, b, c)        \
	{                             \
		(c)[0] = (a)[0] + (b)[0]; \
		(c)[1] = (a)[1] + (b)[1]; \
		(c)[2] = (a)[2] + (b)[2]; \
	}
#define VectorCopy(a, b) \
	{                    \
		(b)[0] = (a)[0]; \
		(b)[1] = (a)[1]; \
		(b)[2] = (a)[2]; \
	}
inline void VectorScale(const float* in, float scale, float* out)
{
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}
inline void VectorInverse(float* v)
{
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}
inline void VectorMA(const float* veca, float scale, const float* vecb, float* vecc)
{
	vecc[0] = veca[0] + scale * vecb[0];
	vecc[1] = veca[1] + scale * vecb[1];
	vecc[2] = veca[2] + scale * vecb[2];
}

inline void VectorClear(float* vec)
{
	vec[0] = vec[1] = vec[2] = 0;
}

inline int VectorCompare(const float* v1, const float* v2)
{
	int i;

	for (i = 0; i < 3; i++)
		if (v1[i] != v2[i])
			return 0;

	return 1;
}

// Messagebox Wrapper
#define MB_OK 0x00000000L
#define MB_OKCANCEL 0x00000001L
#define MB_ABORTRETRYIGNORE 0x00000002L
#define MB_YESNOCANCEL 0x00000003L
#define MB_YESNO 0x00000004L

inline int MessageBox(void*, const char* pszText, const char* pszTitle, int)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, pszTitle, pszText, nullptr);
	return 0;
}

// wglGetProcAddress wrapper

inline void* wglGetProcAddress(const char* proc)
{
	return SDL_GL_GetProcAddress(proc);
}