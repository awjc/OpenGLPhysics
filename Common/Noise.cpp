//
// Coherent noise function over 1, 2 or 3 dimensions
// (copyright Ken Perlin)
// 
// Note: For the original noise functions here, the range for nD noise is [-0.5 * sqrt(n), 0.5* sqrt(n)]
// I.e., for 1D it's [-0.5, 0.5], for 2D it's about [-0.71, 0.71], and for 3D it's about [-0.87, 0.87]
// (Source: http://www.gamedev.net/topic/285533-2d-perlin-noise-gradient-noise-range--/)
//

//#include "App.h"
#include <cmath>
//#include "os.h"
#include "Angel.h"

void init3DNoiseTexture(int texSize, GLubyte* texPtr);
GLubyte * make3DNoiseTexture();

//int Noise3DTexSize = 64;
//GLubyte* Noise3DTexPtr;

#define MAXB 0x100
#define N 0x1000
#define NP 12   // 2^N
#define NM 0xfff

#define s_curve(t) ( t * t * (3. - 2. * t) )
#define lerp(t, a, b) ( a + t * (b - a) )
#define setup(i, b0, b1, r0, r1)\
        t = vec[i] + N;\
        b0 = ((int)t) & BM;\
        b1 = (b0+1) & BM;\
        r0 = t - (int)t;\
        r1 = r0 - 1.;
#define at2(rx, ry) ( rx * q[0] + ry * q[1] )
#define at3(rx, ry, rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

static void initNoise();

static int p[MAXB + MAXB + 2];
static double g3[MAXB + MAXB + 2][3];
static double g2[MAXB + MAXB + 2][2];
static double g1[MAXB + MAXB + 2];

int start;
int B;
int BM;

//void CreateNoise3D()
//{
//	make3DNoiseTexture();
//	init3DNoiseTexture(Noise3DTexSize, Noise3DTexPtr);
//}

void SetNoiseFrequency(int frequency)
{
	start = 1;
	B = frequency;
	BM = B-1;
}

double noise1(double arg)
{
	int bx0, bx1;
	double rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;
	if (start)
	{
		start = 0;
		initNoise();
	}

	setup(0, bx0, bx1, rx0, rx1);

	sx = s_curve(rx0);
	u = rx0 * g1[p[bx0]];
	v = rx1 * g1[p[bx1]];

	return(lerp(sx, u, v));
}

double noise2(double vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	int i, j;

	if (start)
	{
		start = 0;
		initNoise();
	}

	setup(0, bx0, bx1, rx0, rx1);
	setup(1, by0, by1, ry0, ry1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	sx = s_curve(rx0);
	sy = s_curve(ry0);

	q = g2[b00]; u = at2(rx0, ry0);
	q = g2[b10]; v = at2(rx1, ry0);
	a = lerp(sx, u, v);

	q = g2[b01]; u = at2(rx0, ry1);
	q = g2[b11]; v = at2(rx1, ry1);
	b = lerp(sx, u, v);

	return lerp(sy, a, b);
}

double noise3(double vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	int i, j;

	if (start)
	{
		start = 0;
		initNoise();
	}

	setup(0, bx0, bx1, rx0, rx1);
	setup(1, by0, by1, ry0, ry1);
	setup(2, bz0, bz1, rz0, rz1);

	i = p[bx0];
	j = p[bx1];

	b00 = p[i + by0];
	b10 = p[j + by0];
	b01 = p[i + by1];
	b11 = p[j + by1];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

	q = g3[b00 + bz0]; u = at3(rx0, ry0, rz0);
	q = g3[b10 + bz0]; v = at3(rx1, ry0, rz0);
	a = lerp(t, u, v);

	q = g3[b01 + bz0]; u = at3(rx0, ry1, rz0);
	q = g3[b11 + bz0]; v = at3(rx1, ry1, rz0);
	b = lerp(t, u, v);

	c = lerp(sy, a, b);

	q = g3[b00 + bz1]; u = at3(rx0, ry0, rz1);
	q = g3[b10 + bz1]; v = at3(rx1, ry0, rz1);
	a = lerp(t, u, v);

	q = g3[b01 + bz1]; u = at3(rx0, ry1, rz1);
	q = g3[b11 + bz1]; v = at3(rx1, ry1, rz1);
	b = lerp(t, u, v);

	d = lerp(sy, a, b);

	return lerp(sz, c, d);
}

void normalize2(double v[2])
{
	double s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

void normalize3(double v[3])
{
	double s;

	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

void initNoise()
{
	int i, j, k;

	srand(30757);
	for (i = 0; i < B; i++)
	{
		p[i] = i;
		g1[i] = (double)((rand() % (B + B)) - B) / B;

		for (j = 0; j < 2; j++)
			g2[i][j] = (double)((rand() % (B + B)) - B) / B;
		normalize2(g2[i]);

		for (j = 0; j < 3; j++)
			g3[i][j] = (double)((rand() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i)
	{
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	for (i = 0; i < B + 2; i++)
	{
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0; j < 2; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0; j < 3; j++)
			g3[B + i][j] = g3[i][j];
	}
}

// My harmonic summing functions - PDB

//
// In what follows "alpha" is the weight when the sum is formed.
// Typically it is 2, As this approaches 1 the function is noisier.
// "beta" is the harmonic scaling/spacing, typically 2.
//

double PerlinNoise1D(double x,double alpha,double beta,int n)
{
	int i;
	double val,sum = 0;
	double p,scale = 1;

	p = x;
	for (i = 0; i < n; i++)
	{
		val = noise1(p);
		sum += val / scale;
		scale *= alpha;
		p *= beta;
	}
	return(sum);
}

double PerlinNoise2D(double x, double y, double alpha, double beta, int n)
{
	int i;
	double val, sum = 0;
	double p[2], scale = 1;

	p[0] = x;
	p[1] = y;
	for (i = 0; i < n; i++)
	{
		val = noise2(p);
		sum += val / scale;
		scale *= alpha;
		p[0] *= beta;
		p[1] *= beta;
	}
	return(sum);
	}

double PerlinNoise3D(double x, double y, double z, double alpha, double beta, int n)
{
	int i;
	double val,sum = 0;
	double p[3],scale = 1;

	p[0] = x;
	p[1] = y;
	p[2] = z;
	for (i = 0; i < n; i++)
	{
		val = noise3(p);
		sum += val / scale;
		scale *= alpha;
		p[0] *= beta;
		p[1] *= beta;
		p[2] *= beta;
	}
	return(sum);
}

GLubyte* make3DNoiseTexture(int Noise3DTexSize)
{
	int f, i, j, k, inc;
	int startFrequency = 4;
	int numOctaves = 4;
	double ni[3];
	double inci, incj, inck;
	int frequency = startFrequency;
	GLubyte* ptr;
	double amp = 0.5;

	GLubyte* Noise3DTexPtr;
	Noise3DTexPtr = (GLubyte*) malloc(Noise3DTexSize * Noise3DTexSize * Noise3DTexSize * 4);
	for (f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= 0.5)
	{
		//wxGetApp().Statusf("Generating 3D noise: octave %d/%d...", f + 1, numOctaves);
		SetNoiseFrequency(frequency);
		ptr = Noise3DTexPtr;
		ni[0] = ni[1] = ni[2] = 0;

		inci = 1.0 / (Noise3DTexSize / frequency);
		for (i = 0; i < Noise3DTexSize; ++i, ni[0] += inci)
		{
			incj = 1.0 / (Noise3DTexSize / frequency);
			for (j = 0; j < Noise3DTexSize; ++j, ni[1] += incj)
			{
				inck = 1.0 / (Noise3DTexSize / frequency);
				for (k = 0; k < Noise3DTexSize; ++k, ni[2] += inck, ptr += 4)
					*(ptr + inc) = (GLubyte) (((noise3(ni) + 1.0) * amp) * 128.0);
			}
		}
	}
  return Noise3DTexPtr;
	//wxGetApp().Ready();
}

// Similar to make3DNoiseTexture but creates a 2D texture
GLubyte* make2DNoiseTexture(int Noise3DTexSize)
{
  int inc = 0; // 0, 1, 2, 3 for the r, g, b, and a channel, respectively
	int f, i, j;
	int startFrequency = 4;
	int numOctaves = 4;
	double ni[2];
	double inci, incj, inck;
	int frequency = startFrequency;
	GLubyte* ptr;
	double amp = 0.5;

	GLubyte* imageBytes = (GLubyte*) malloc(Noise3DTexSize * Noise3DTexSize * 4);
	for (f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= 0.5)
	{
		SetNoiseFrequency(frequency);
		ptr = imageBytes; // start this over for each frequency
		ni[0] = ni[1] = 0;

		inci = 1.0 / (Noise3DTexSize / frequency);
		for (i = 0; i < Noise3DTexSize; ++i, ni[0] += inci)
		{
			incj = 1.0 / (Noise3DTexSize / frequency);
			for (j = 0; j < Noise3DTexSize; ++j, ni[1] += incj)
			{
					*(ptr + inc) = (GLubyte) (((noise2(ni) + 1.0) * amp) * 128.0);
          ptr += 4;
			}
		}
	}
  return imageBytes;
}



void init3DNoiseTexture(int texSize, GLubyte* texPtr)
{
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, texSize, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texPtr);
	free(texPtr);
}

// Returns a greyscale RGB noise texture with the given starting frequency and number
// of octaves.  The resulting values are always scaled to use approximately the full 
// range of grey values [0, 255].  
//
// Allocated bytes should be freed by the caller.
GLubyte* noiseTexture2D(int startFrequency, int numOctaves, int * nWidth, int * nHeight, int * nComponents, GLenum * eFormat)
{
  const int size = 128;
  const int numComponents = 3; // RGB 

  GLubyte* imageBytes = (GLubyte*) malloc(size * size * numComponents);
  GLubyte* ptr = imageBytes;

  for (int i = 0; i < size; ++i)
  {
    for (int j = 0; j < size; ++j)
    {
      // Start with the base frequency and add the requested number of octaves.
      // We use amplitude 1 for the first one and divide by 2 each time we double
      // the frequency.  The added amplitude is also added to the 'totalAmp' value.
      int frequency = startFrequency;
      double amp = 1.0;
      double totalAmp = 1.0;
      double ni[2];
      ni[0] = ni[1] = 0;
      double noiseValue = 0;
      for (int f = 0; f < numOctaves; ++f) 
      {
        SetNoiseFrequency(frequency);
        double delta = 1.0 / (size / frequency);
        ni[0] = i * delta;
        ni[1] = j * delta;
        noiseValue += noise2(ni) * amp;
        frequency *= 2;
        amp *= 0.5;
        totalAmp += amp;
      }

      // The range is initially (-.71, .71), so after adding multiple octaves
      // we have a range (-.71 * scale, .71 * scale).  So we scale it back to (-.5, .5), then shift up 
      // by .5, to get a value in range (0, 1) and then multiply by 256 to get a byte value [0, 255]
      double scale = 0.5 / (.71 * totalAmp);
      GLubyte value =  (GLubyte) ((noiseValue * scale + 0.5) * 256.0);
      *(ptr + 0) = value;
      *(ptr + 1) = value;
      *(ptr + 2) = value;
      ptr += numComponents;
    }
  }

  *nWidth = *nHeight = size;
  *nComponents = GL_RGB;
  *eFormat = GL_RGB;
  return imageBytes;
}
