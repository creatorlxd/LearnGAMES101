#define _CRT_SECURE_NO_WARNINGS
#include "Picture.h"
#include "../svpng.h"
#include <cstring>

Picture::Picture(unsigned int w, unsigned int h)
	:m_Width(w), m_Height(h), m_pContent(new unsigned char[w * h * 4])
{
	memset(m_pContent, 0, sizeof(unsigned char) * w * h * 4);
}

Picture::Picture(const Picture& pic)
	: m_Width(pic.m_Width), m_Height(pic.m_Height), m_pContent(new unsigned char[pic.m_Width * pic.m_Height * 4])
{
	memcpy(m_pContent, pic.m_pContent, sizeof(unsigned char) * pic.m_Width * pic.m_Height * 4);
}

Picture::Picture(Picture&& pic)
	: m_Width(pic.m_Width), m_Height(pic.m_Height), m_pContent(pic.m_pContent)
{
	pic.m_pContent = nullptr;
}

Picture::~Picture()
{
	if (m_pContent)
		delete[] m_pContent;
}

void Picture::Print(const std::string& filename)
{
	FILE* fp = fopen(filename.c_str(), "wb");
	svpng(fp, m_Width, m_Height, m_pContent, 1);
	fclose(fp);
}

void Picture::DrawPoint(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	unsigned int idx = (m_Width * y + x) * 4;
	m_pContent[idx++] = r;
	m_pContent[idx++] = g;
	m_pContent[idx++] = b;
	m_pContent[idx] = a;
}

void Picture::FreeDrawPoint(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	if (x < 0 || x >= m_Width)
		return;
	if (y < 0 || y >= m_Height)
		return;
	DrawPoint(x, y, r, g, b, a);
}

void Picture::DrawPoint(unsigned int x, unsigned int y, RGBA color)
{
	DrawPoint(x, y, color.m_R, color.m_G, color.m_B, color.m_A);
}

RGBA Picture::GetPoint(unsigned int x, unsigned int y) const
{
	unsigned int idx = (m_Width * y + x) * 4;
	return RGBA(m_pContent[idx++], m_pContent[idx++], m_pContent[idx++], m_pContent[idx]);
}

RGBA Picture::FreeGetPoint(int x, int y) const
{
	if (x < 0 || x >= m_Width)
		return RGBA();
	if (y < 0 || y >= m_Height)
		return RGBA();
	return GetPoint(x, y);
}

unsigned int Picture::GetWidth() const
{
	return m_Width;
}

unsigned int Picture::GetHeight() const
{
	return m_Height;
}

Picture& Picture::operator=(const Picture& pic)
{
	if (m_pContent)
		delete[] m_pContent;
	m_Width = pic.m_Width;
	m_Height = pic.m_Height;
	m_pContent = new unsigned char[pic.m_Width * pic.m_Height * 4];
	memcpy(m_pContent, pic.m_pContent, sizeof(unsigned char) * pic.m_Width * pic.m_Height * 4);
	return *this;
}

Picture& Picture::operator=(Picture&& pic)
{
	if (m_pContent)
		delete[] m_pContent;
	m_Width = pic.m_Width;
	m_Height = pic.m_Height;
	m_pContent = pic.m_pContent;
	pic.m_pContent = nullptr;
	return *this;
}

void Picture::FreeDrawPoint(int x, int y, RGBA color)
{
	FreeDrawPoint(x, y, color.m_R, color.m_G, color.m_B, color.m_A);
}

RGBA::RGBA()
	:m_R(0), m_G(0), m_B(0), m_A(0)
{
}

RGBA::RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	: m_R(r), m_G(g), m_B(b), m_A(a)
{
}

RGBA AverageMix(RGBA a, RGBA b)
{
	RGBA re;
	for (int i = 0; i < 4; ++i)
	{
		re.m_Array[i] = (a.m_Array[i] + b.m_Array[i]) / 2;
	}
	return re;
}
