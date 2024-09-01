#ifndef Engine_Struct_h__
#define Engine_Struct_h__

#include "Engine_Typedef.h"

namespace Engine
{
	struct tagTextureInfo {
		IDirect3DBaseTexture9* pTexture;
		_bool bDelete;
		_vec2 vImgSize;
	};


	typedef struct tagVertexColor
	{
		_vec3		vPosition;		
		_ulong		dwColor;

	}VTXCOL;

	const _ulong	FVF_COL = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0;

	typedef struct tagVertexTexture
	{
		_vec3		vPosition;
		_vec3		vNormal;
		_vec2		vTexUV;

	}VTXTEX;
	
	const _ulong	FVF_TEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	
	typedef struct tagVertexCubeTexture
	{
		_vec3		vPosition;
		_vec3		vNormal;
		_vec3		vTexUV;

	}VTXCUBE;

	const _ulong	FVF_CUBE = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0); // 텍스처의 UV 좌표 값을 FLOAT형 3개로 표현하겠다는 매크로(괄호안의 숫자 0의 의미는 본래 버텍스에 텍스쳐 UV값이 여러개가 올 수 있는데 그중 0번째 값을 지정하겠다는 의미)

	typedef struct tagIndex16
	{
		_ushort  _0;
		_ushort  _1;
		_ushort  _2;

	}INDEX16;

	typedef struct tagIndex32
	{
		_ulong	_0;
		_ulong	_1;
		_ulong	_2;

	}INDEX32;

	typedef struct tagParticle
	{
		_vec3		vPosition;
		D3DXCOLOR	tColor;
	} PARTICLE;

	const _ulong	FVF_PARTICLE = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	
	typedef struct tagParticleAttribute
	{
		_vec3		vPosition;
		_vec3		vVelocity;
		_vec3		vAcceleration;
		_float		fLifeTime;
		_float		fAge;
		D3DXCOLOR	tColor;
		D3DXCOLOR	tColorFade;
		bool		bIsAlive;
	} PARTICLE_ATTRIBUTE;

	typedef struct tagBoundingBox
	{
		tagBoundingBox()
		{
			_min.x = INFINITY;
			_min.y = INFINITY;
			_min.z = INFINITY;

			_max.x = -INFINITY;
			_max.y = -INFINITY;
			_max.z = -INFINITY;
		};

		bool isPointInside(_vec3& p)
		{
			if (p.x >= _min.x && p.y >= _min.y && p.z >= _min.z &&
				p.x <= _max.x && p.y <= _max.y && p.z <= _max.z)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		_vec3 _min;
		_vec3 _max;

	} BOUNDING_BOX;

	typedef struct tagBoundingSphere
	{
		tagBoundingSphere()
		{
			_radius = 0.0f;
		};

		bool isPointInside(_vec3& p)
		{
			_vec3 vLength = _vec3(p.x - _center.x, p.y - _center.y, p.z - _center.z);

			if (D3DXVec3Length(&vLength) < _radius)
				return true;

			else
				return false;
		}

		D3DXVECTOR3 _center;
		float       _radius;

	} BOUNDING_SPHERE;
}


#endif // Engine_Struct_h__
