#include "EngineUtil.h"

std::wstring StringToWString(const std::string& str)
{
	std::wstring wstr;
	std::size_t size;
	wstr.resize(str.length());
	mbstowcs_s(&size, &wstr[0], wstr.size() + 1, str.c_str(), str.size());
	return wstr;
}

std::string WStringToString(const std::wstring& wstr)
{
	std::string str;
	std::size_t size;
	str.resize(wstr.length());
	wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
	return str;
}

D3DMATERIAL9 InitMtrl(D3DXCOLOR ambient, D3DXCOLOR diffuse, D3DXCOLOR specular, D3DXCOLOR emissive, const float power)
{
	D3DMATERIAL9 mtrl{};
	mtrl.Ambient = ambient;
	mtrl.Diffuse = diffuse;
	mtrl.Specular = specular;
	mtrl.Emissive = emissive;
	mtrl.Power = power;
	return mtrl;
}