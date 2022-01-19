#pragma once
#include <string>

struct RGBA
{
	union {
		struct {
			unsigned char m_R;
			unsigned char m_G;
			unsigned char m_B;
			unsigned char m_A;
		};
		unsigned int m_Value;
		unsigned char m_Array[4];
	};

	RGBA();
	RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
};

RGBA AverageMix(RGBA a, RGBA b);

class Picture
{
public:
	Picture(unsigned int w, unsigned int h);
	~Picture();
	void Print(const std::string& filename);
	void DrawPoint(unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void DrawPoint(unsigned int x, unsigned int y, RGBA color);
	void FreeDrawPoint(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	void FreeDrawPoint(int x, int y, RGBA color);
	RGBA GetPoint(unsigned int x, unsigned int y) const;
	RGBA FreeGetPoint(int x, int y) const;
private:
	unsigned int m_Weight;
	unsigned int m_Height;
	unsigned char* m_pContent;
};