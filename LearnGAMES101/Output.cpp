#include "Output.h"

using namespace LearnGames;

LearnGames::ColorDepthBuffer::ColorDepthBuffer(uint64_t width, uint64_t height, uint64_t msaa_way, Vector4 background_color)
	:m_Width(width), m_Height(height), m_MSAA(msaa_way), m_pColorBuffer(new Vector4[width * height * msaa_way]), m_pDepthBuffer(new float[width * height * msaa_way])
{
	assert(width);
	assert(height);
	assert(msaa_way);
	for (uint64_t x = 0; x < width; ++x)
	{
		for (uint64_t y = 0; y < height; ++y)
		{
			for (uint64_t idx = 0; idx < msaa_way; ++idx)
			{
				GetColor(x, y, idx) = background_color;
				GetDepth(x, y, idx) = std::numeric_limits<float>::max();
			}
		}
	}
}

LearnGames::ColorDepthBuffer::~ColorDepthBuffer()
{
	if (m_pColorBuffer)
	{
		assert(m_pDepthBuffer);
		delete[] m_pColorBuffer;
		delete[] m_pDepthBuffer;
	}
}

LearnGames::ColorDepthBuffer::ColorDepthBuffer(const ColorDepthBuffer& c)
	:m_Width(c.m_Width), m_Height(c.m_Height), m_MSAA(c.m_MSAA), m_pColorBuffer(new Vector4[c.m_Width * c.m_Height * c.m_MSAA]), m_pDepthBuffer(new float[c.m_Width * c.m_Height * c.m_MSAA])
{
	assert(c.m_Width);
	assert(c.m_Height);
	assert(c.m_MSAA);
	for (uint64_t x = 0; x < c.m_Width; ++x)
	{
		for (uint64_t y = 0; y < c.m_Height; ++y)
		{
			for (uint64_t idx = 0; idx < c.m_MSAA; ++idx)
			{
				GetColor(x, y, idx) = c.GetColor(x, y, idx);
				GetDepth(x, y, idx) = c.GetDepth(x, y, idx);
			}
		}
	}
}

LearnGames::ColorDepthBuffer::ColorDepthBuffer(ColorDepthBuffer&& c)
	:m_Width(c.m_Width), m_Height(c.m_Height), m_MSAA(c.m_MSAA), m_pColorBuffer(c.m_pColorBuffer), m_pDepthBuffer(c.m_pDepthBuffer)
{
	assert(c.m_Width);
	assert(c.m_Height);
	assert(c.m_MSAA);
	c.m_pColorBuffer = nullptr;
	c.m_pDepthBuffer = nullptr;
}

ColorDepthBuffer& LearnGames::ColorDepthBuffer::operator=(const ColorDepthBuffer& c)
{
	assert(this != &c);
	assert(c.m_Width);
	assert(c.m_Height);
	assert(c.m_MSAA);
	RawClear();
	m_Width = c.m_Width;
	m_Height = c.m_Height;
	m_MSAA = c.m_MSAA;
	m_pColorBuffer = new Vector4[c.m_Width * c.m_Height * c.m_MSAA];
	m_pDepthBuffer = new float[c.m_Width * c.m_Height * c.m_MSAA];
	for (uint64_t x = 0; x < c.m_Width; ++x)
	{
		for (uint64_t y = 0; y < c.m_Height; ++y)
		{
			for (uint64_t idx = 0; idx < c.m_MSAA; ++idx)
			{
				GetColor(x, y, idx) = c.GetColor(x, y, idx);
				GetDepth(x, y, idx) = c.GetDepth(x, y, idx);
			}
		}
	}
	return *this;
}

ColorDepthBuffer& LearnGames::ColorDepthBuffer::operator=(ColorDepthBuffer&& c)
{
	assert(this != &c);
	assert(c.m_Width);
	assert(c.m_Height);
	assert(c.m_MSAA);
	RawClear();
	m_Width = c.m_Width;
	m_Height = c.m_Height;
	m_MSAA = c.m_MSAA;
	m_pColorBuffer = c.m_pColorBuffer;
	m_pDepthBuffer = c.m_pDepthBuffer;
	c.m_pColorBuffer = nullptr;
	c.m_pDepthBuffer = nullptr;
	return *this;
}

Vector4& LearnGames::ColorDepthBuffer::GetColor(uint64_t x, uint64_t y, uint64_t msaa_idx)
{
	assert(x);
	assert(y);
	assert(msaa_idx);
	return m_pColorBuffer[(y * m_Width + x) * m_MSAA + msaa_idx];
}

const Vector4& LearnGames::ColorDepthBuffer::GetColor(uint64_t x, uint64_t y, uint64_t msaa_idx) const
{
	assert(x);
	assert(y);
	assert(msaa_idx);
	return m_pColorBuffer[(y * m_Width + x) * m_MSAA + msaa_idx];
}

float& LearnGames::ColorDepthBuffer::GetDepth(uint64_t x, uint64_t y, uint64_t msaa_idx)
{
	assert(x);
	assert(y);
	assert(msaa_idx);
	return m_pDepthBuffer[(y * m_Width + x) * m_MSAA + msaa_idx];
}

const float& LearnGames::ColorDepthBuffer::GetDepth(uint64_t x, uint64_t y, uint64_t msaa_idx) const
{
	assert(x);
	assert(y);
	assert(msaa_idx);
	return m_pDepthBuffer[(y * m_Width + x) * m_MSAA + msaa_idx];
}

uint64_t LearnGames::ColorDepthBuffer::GetWidth() const
{
	return m_Width;
}

uint64_t LearnGames::ColorDepthBuffer::GetHeight() const
{
	return m_Height;
}

uint64_t LearnGames::ColorDepthBuffer::GetMSAA() const
{
	return m_MSAA;
}

Picture LearnGames::ColorDepthBuffer::PrintToPicture() const
{
	Picture re(m_Width, m_Height);

	for (uint64_t i = 0; i < m_Width; ++i)
	{
		for (uint64_t j = 0; j < m_Height; ++j)
		{
			Vector4 fcolor = Vector4::Zero();
			for (uint64_t idx = 0; idx < m_MSAA; ++idx)
				fcolor += GetColor(i, j, idx);
			fcolor /= (float)m_MSAA;
			re.DrawPoint(i, j, Vector4ToRGBA(fcolor));
		}
	}

	return re;
}

void LearnGames::ColorDepthBuffer::ClearContent(Vector4 background_color)
{
	for (uint64_t x = 0; x < m_Width; ++x)
	{
		for (uint64_t y = 0; y < m_Height; ++y)
		{
			for (uint64_t idx = 0; idx < m_MSAA; ++idx)
			{
				GetColor(x, y, idx) = background_color;
				GetDepth(x, y, idx) = std::numeric_limits<float>::max();
			}
		}
	}
}

void LearnGames::ColorDepthBuffer::RawClear()
{
	if (m_pColorBuffer)
	{
		assert(m_pDepthBuffer);
		delete[] m_pColorBuffer;
		delete[] m_pDepthBuffer;
	}
}

RGBA LearnGames::Vector4ToRGBA(const Vector4& fcolor)
{
	RGBA color(0, 0, 0, 0);
	for (uint64_t i = 0; i < 4; ++i)
	{
		int buf = floor(fcolor(i) * 256.0f);
		if (buf > 255)
			buf = 255;
		if (buf < 0)
			buf = 0;
		color.m_Array[i] = buf;
	}
	return color;
}
