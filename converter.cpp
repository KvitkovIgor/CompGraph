#include <cstdio>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <string>

// max picture size
#define H_P 450
#define W_P 450


int Ptype = -1;

// Picture type enum
enum Ptype{
    P5,
    P6
} PId;

unsigned char max( unsigned char x, unsigned char y )
{
    return x > y ? x : y;
}
unsigned char min( unsigned char x, unsigned char y )
{
    return x > y ? y : x;
}

// Dithering type enum
enum ColorSpaceType
{
    RGB,
    HSL,
    HSV,
    YCBCR601,
    YCBCR709,
    YCOCG,
    CMY,
};

class Picture
{
public:
    int PictureID;
    int FilesCounter;
    char FileNames[3][100];
    FILE *F[3];
    unsigned char PIC[3][H_P][W_P][3];
    int Ptype[3], w[3], h[3], grad[3];
};


//
// HSL FORMAT
//

void RgbToHsl( Picture rgb, Picture *hsl )
{
    for (int f = 0; f < rgb.FilesCounter; f++)
        for (int i = 0; i < rgb.h[f]; i++)
            for (int j = 0; j < rgb.w[f]; j++)
            {
                unsigned char r = 0, g = 0, b = 0, maximun, minimum, hs, ss, ls;
                double h, s, l;
                
                if (rgb.FilesCounter == 1)
                {
                    r = rgb.PIC[0][i][j][0];
                    g = rgb.PIC[0][i][j][1];
                    b = rgb.PIC[0][i][j][2];
                }
                else
                {
                    r = rgb.PIC[0][i][j][0];
                    g = rgb.PIC[1][i][j][1];
                    b = rgb.PIC[2][i][j][2];
                }
                maximun = max(max(r, g), b);
                minimum = min(min(r, g), b);

                l = 0.5 * (maximun + minimum);
                ls = l;

                if (0 < l / 255.0 && l / 255.0 <= 0.5)
                    s = ((maximun / 255.0) - (minimum / 255.0)) / (2 * l / 255.0) * 255.0;
                else if (0.5 < l / 255.0 && l / 255.0 <= 1)
                    s = ((maximun / 255.0) - (minimum / 255.0)) / (2 - 2 * l / 255.0) * 255.0;
                else if (maximun == minimum || l == 0)
                    s = 0;
                ss = s;
                if (s > 255)
                    ss = 255;
                else if (s < 0)
                    ss = 0;

                if (maximun == b)
                    h = 60 * ((r - g) * 1.0) / (maximun - minimum) + 240;
                else if (maximun == g)
                    h = 60 * ((b - r) * 1.0) / (maximun - minimum) + 120;
                else if (maximun == r && g < b)
                    h = 60 * ((g - b) * 1.0) / (maximun - minimum) + 360;
                else if (maximun == r && g >= b)
                    h = 60 * ((g - b) * 1.0) / (maximun - minimum);
                else if (maximun == minimum)
                    h = 0;

                h = h * 360.0 / 255.0;
                hs = h;
                if (h > 255)
                    hs = 255;
                else if (h < 0)
                    hs = 0;

                if (hsl->FilesCounter == 1)
                {
                    hsl->PIC[0][i][j][0] = hs;
                    hsl->PIC[0][i][j][1] = ss;
                    hsl->PIC[0][i][j][2] = ls;
                }
                else
                {
                    hsl->PIC[0][i][j][0] = hs;
                    hsl->PIC[1][i][j][1] = ss;
                    hsl->PIC[2][i][j][2] = ls;
                }
            }
}

void HslToRgb( Picture hsl, Picture *rgb )
{
    for (int f = 0; f < hsl.FilesCounter; f++)
        for (int i = 0; i < hsl.h[f]; i++)
            for (int j = 0; j < hsl.w[f]; j++)
            {
                unsigned char h = 0, s = 0, l = 0, maximun, minimum, rs, gs, bs;
                double ls, hs, ss, q, p, tr, tg, tb, hk;
                if (hsl.FilesCounter == 1)
                {
                    h = hsl.PIC[0][i][j][0];
                    s = hsl.PIC[0][i][j][1];
                    l = hsl.PIC[0][i][j][2];
                }
                else
                {
                    h = hsl.PIC[0][i][j][0];
                    s = hsl.PIC[1][i][j][1];
                    l = hsl.PIC[2][i][j][2];
                }
                hs = h / 255.0 * 360;
                ss = s / 255.0;
                ls = l / 255.0;
                
                if (ls < 0.5)
                    q = ls * (1.0 + ss);
                else if (ls >= 0.5)
                    q = ls + ss - (ls * ss);
                p = 2 * l - q;
                hk = hs / 360.0;
                tr = hk + 1 / 3.0;
                tg = hk;
                tb = hk - 1 / 3.0;
                
                if (tr < 0)
                    tr++;
                else if (tr > 1)
                    tr--;
                if (tg < 0)
                    tg++;
                else if (tg > 1)
                    tg--;
                if (tb < 0)
                    tb++;
                else if (tb > 1)
                    tb--;
                
                if (tr < 1 / 6.0)
                    tr = p + ((q - p) * 6.0 * tr);
                else if (tr >= 1 / 6.0 && tr < 0.5)
                    tr = q;
                else if (tr >= 0.5 && tr < 2 / 3.0)
                    tr = p + ((q - p) * (2 / 3.0 - tr) * 6.0);
                else
                    tr = p;
                
                if (tg < 1 / 6.0)
                    tg = p + ((q - p) * 6.0 * tg);
                else if (tg >= 1 / 6.0 && tg < 0.5)
                    tg = q;
                else if (tg >= 0.5 && tg < 2 / 3.0)
                    tg = p + ((q - p) * (2 / 3.0 - tg) * 6.0);
                else
                    tg = p;
                
                if (tb < 1 / 6.0)
                    tb = p + ((q - p) * 6.0 * tb);
                else if (tb >= 1 / 6.0 && tb < 0.5)
                    tb = q;
                else if (tb >= 0.5 && tb < 2 / 3.0)
                    tb = p + ((q - p) * (2 / 3.0 - tb) * 6.0);
                else
                    tb = p;
                
                tg *= 255;
                tb *= 255;
                tr *= 255;
                
                if (tr > 255.0)
                    tr = 255;
                else if (tr < 0)
                    tr = 0;
                if (tg > 255.0)
                    tg = 255;
                else if (tg < 0)
                    tg = 0;
                if (tb > 255.0)
                    tb = 255;
                else if (tb < 0)
                    tb = 0;
                
                rs = tr;
                gs = tg;
                bs = tb;
                
                if (rgb->FilesCounter == 1)
                {
                    rgb->PIC[0][i][j][0] = rs;
                    rgb->PIC[0][i][j][1] = gs;
                    rgb->PIC[0][i][j][2] = bs;
                }
                else
                {
                    rgb->PIC[0][i][j][0] = rs;
                    rgb->PIC[1][i][j][1] = gs;
                    rgb->PIC[2][i][j][2] = bs;
                }
            }
}

//
// END OF 'HSL' FORMAT
//

//
// HSV FORMAT
//

void RgbToHsv( Picture rgb, Picture *hsv )
{
    for (int f = 0; f < rgb.FilesCounter; f++)
        for (int i = 0; i < rgb.h[f]; i++)
            for (int j = 0; j < rgb.w[f]; j++)
            {
                unsigned char r = 0, g = 0, b = 0, maximun, minimum, hs, ss, vs;
                double h = 0, s = 0, v = 0;
                
                if (rgb.FilesCounter == 1)
                {
                    r = rgb.PIC[0][i][j][0];
                    g = rgb.PIC[0][i][j][1];
                    b = rgb.PIC[0][i][j][2];
                }
                else
                {
                    r = rgb.PIC[0][i][j][0];
                    g = rgb.PIC[1][i][j][1];
                    b = rgb.PIC[2][i][j][2];
                }
                maximun = max(max(r, g), b);
                minimum = min(min(r, g), b);
                
                v = maximun;
                vs = v;
                
                if (maximun == 0)
                    s = 0;
                else
                    s = 1 - minimum / maximun;
                s *= 255;
                ss = s;
                if (s > 255)
                    ss = 255;
                else if (s < 0)
                    ss = 0;
                
                if (maximun == b)
                    h = 60 * ((r - g) * 1.0) / (maximun - minimum) + 240;
                else if (maximun == g)
                    h = 60 * ((b - r) * 1.0) / (maximun - minimum) + 120;
                else if (maximun == r && g < b)
                    h = 60 * ((g - b) * 1.0) / (maximun - minimum) + 360;
                else if (maximun == r && g >= b)
                    h = 60 * ((g - b) * 1.0) / (maximun - minimum);
                else if (maximun == minimum)
                    h = 0;
                
                h = h * 360.0 / 255.0;
                hs = h;
                if (h > 255)
                    hs = 255;
                else if (h < 0)
                    hs = 0;
                
                if (hsv->FilesCounter == 1)
                {
                    hsv->PIC[0][i][j][0] = hs;
                    hsv->PIC[0][i][j][1] = ss;
                    hsv->PIC[0][i][j][2] = vs;
                }
                else
                {
                    hsv->PIC[0][i][j][0] = hs;
                    hsv->PIC[1][i][j][1] = ss;
                    hsv->PIC[2][i][j][2] = vs;
                }
            }
}

void HsvToRgb( Picture hsv, Picture *rgb )
{
    for (int f = 0; f < hsv.FilesCounter; f++)
        for (int i = 0; i < hsv.h[f]; i++)
            for (int j = 0; j < hsv.w[f]; j++)
            {
                unsigned char h = 0, s = 0, v = 0, rs = 0, gs = 0, bs = 0;
                double ss, hs, vs, hmod, vmin, vdec, vinc, a;
                if (hsv.FilesCounter == 1)
                {
                    h = hsv.PIC[0][i][j][0];
                    s = hsv.PIC[0][i][j][1];
                    v = hsv.PIC[0][i][j][2];
                }
                else
                {
                    h = hsv.PIC[0][i][j][0];
                    s = hsv.PIC[1][i][j][1];
                    v = hsv.PIC[2][i][j][2];
                }
                
                hs = h / 255.0 * 360;
                ss = s / 255.0 * 100;
                vs = v / 255.0 * 100;
                
                hmod = (int)hs / 60 % 6;
                vmin = (100 - ss) * vs / 100;
                a = (vs - vmin) * ((int)hs % 60) / 60.0;
                vinc = vmin + a;
                vdec = vs - a;
                
                if (hmod == 0)
                {
                    rs = v / 100.0 * 255;
                    gs = vinc / 100.0 * 255;
                    bs = vmin / 100.0 * 255;
                }
                else if (hmod == 1)
                {
                    rs = vdec / 100.0 * 255;
                    gs = v / 100.0 * 255;
                    bs = vmin / 100.0 * 255;
                }
                else if (hmod == 2)
                {
                    rs = vmin / 100.0 * 255;
                    gs = v / 100.0 * 255;
                    bs = vinc / 100.0 * 255;
                }
                else if (hmod == 3)
                {
                    rs = vmin / 100.0 * 255;
                    gs = vdec / 100.0 * 255;
                    bs = v / 100.0 * 255;
                }
                else if (hmod == 4)
                {
                    rs = vinc / 100.0 * 255;
                    gs = vmin / 100.0 * 255;
                    bs = v / 100.0 * 255;
                }
                else if (hmod == 5)
                {
                    rs = v / 100.0 * 255;
                    gs = vmin / 100.0 * 255;
                    bs = vdec / 100.0 * 255;
                }
                
                if (rgb->FilesCounter == 1)
                {
                    rgb->PIC[0][i][j][0] = rs;
                    rgb->PIC[0][i][j][1] = gs;
                    rgb->PIC[0][i][j][2] = bs;
                }
                else
                {
                    rgb->PIC[0][i][j][0] = rs;
                    rgb->PIC[1][i][j][1] = gs;
                    rgb->PIC[2][i][j][2] = bs;
                }
            }
}

//
// END OF 'HSV' FORMAT
//

//
// YCBCR601 FORMAT
//

void RgbToYcbcr601( Picture rgb, Picture *ycc )
{
    for (int f = 0; f < rgb.FilesCounter; f++)
        for (int i = 0; i < rgb.h[f]; i++)
            for (int j = 0; j < rgb.w[f]; j++)
            {
                unsigned char r = 0, g = 0, b = 0;
                double rs = 0, gs = 0, bs = 0, kr = 0.299, kg = 0.587, kb = 0.114, y1, pb, pr;
                
                if (rgb.FilesCounter == 1)
                {
                    r = rgb.PIC[0][i][j][0];
                    g = rgb.PIC[0][i][j][1];
                    b = rgb.PIC[0][i][j][2];
                }
                else
                {
                    r = rgb.PIC[0][i][j][0];
                    g = rgb.PIC[1][i][j][1];
                    b = rgb.PIC[2][i][j][2];
                }
                
                rs = r / 255.0;
                gs = g / 255.0;
                bs = b / 255.0;
                
                y1 = kr * rs + kg * gs + kb * bs;
                pb = -0.5 * kr / (1 - kb) * rs + -0.5 * kg / (1 - kb) * gs + 1 / 2 * bs;
                pr = -0.5 * kg / (1 - kr) * gs + -0.5 * kb / (1 - kr) * bs + 1 / 2 * rs;
                
                r = 16 + 219 * y1;
                g = 128 + 224 * pb;
                b = 128 + 224 * pr;
                
                r = max(0, min(255, r));
                b = max(0, min(255, b));
                g = max(0, min(255, g));
                
                if (ycc->FilesCounter == 1)
                {
                    ycc->PIC[0][i][j][0] = r;
                    ycc->PIC[0][i][j][1] = g;
                    ycc->PIC[0][i][j][2] = b;
                }
                else
                {
                    ycc->PIC[0][i][j][0] = r;
                    ycc->PIC[1][i][j][1] = g;
                    ycc->PIC[2][i][j][2] = b;
                }
            }
}

void Ycbcr601ToRgb( Picture ycc, Picture *rgb )
{
    for (int f = 0; f < ycc.FilesCounter; f++)
        for (int i = 0; i < ycc.h[f]; i++)
            for (int j = 0; j < ycc.w[f]; j++)
            {
                unsigned char y1 = 0, cb = 0, cr = 0;
                double rs = 0, gs = 0, bs = 0, kr = 0.299, kg = 0.587, kb = 0.114, r, g, b;
                if (ycc.FilesCounter == 1)
                {
                    y1 = ycc.PIC[0][i][j][0];
                    cb = ycc.PIC[0][i][j][1];
                    cr = ycc.PIC[0][i][j][2];
                }
                else
                {
                    y1 = ycc.PIC[0][i][j][0];
                    cb = ycc.PIC[1][i][j][1];
                    cr = ycc.PIC[2][i][j][2];
                }
                    
                r = (y1 - 16) / 219.0;
                g = (cb - 128) / 224.0;
                b = (cr - 128) / 224.0;
                
                rs = (r + (2 - 2 * kr) * b) * 255.0;
                gs = (r - kb / kg * (2 - kb) * g - kr / kg * (2 - kr) * b) * 255.0;
                bs = (r + (2 - 2 * kb) * g) * 255.0;
                
                rs = max(0, min(255, rs));
                bs = max(0, min(255, bs));
                gs = max(0, min(255, gs));

                if (rgb->FilesCounter == 1)
                {
                    rgb->PIC[0][i][j][0] = rs;
                    rgb->PIC[0][i][j][1] = gs;
                    rgb->PIC[0][i][j][2] = bs;
                }
                else
                {
                    rgb->PIC[0][i][j][0] = rs;
                    rgb->PIC[1][i][j][1] = gs;
                    rgb->PIC[2][i][j][2] = bs;
                }
            }
}

//
// END OF YCBCR601 FORMAT
//

//
// YCBCR709 FORMAT
//

void RgbToYcbcr709( Picture rgb, Picture *ycc )
{
    for (int f = 0; f < rgb.FilesCounter; f++)
        for (int i = 0; i < rgb.h[f]; i++)
            for (int j = 0; j < rgb.w[f]; j++)
            {
                unsigned char r = 0, g = 0, b = 0;
                double rs = 0, gs = 0, bs = 0, kr = 0.2126, kg = 0.7152, kb = 0.0722, y1, pb, pr;
                
                if (rgb.FilesCounter == 1)
                {
                    r = rgb.PIC[0][i][j][0];
                    g = rgb.PIC[0][i][j][1];
                    b = rgb.PIC[0][i][j][2];
                }
                else
                {
                    r = rgb.PIC[0][i][j][0];
                    g = rgb.PIC[1][i][j][1];
                    b = rgb.PIC[2][i][j][2];
                }
                
                rs = r / 255.0;
                gs = g / 255.0;
                bs = b / 255.0;
                
                y1 = kr * rs + kg * gs + kb * bs;
                pb = -0.5 * kr / (1 - kb) * rs + -0.5 * kg / (1 - kb) * gs + 1 / 2 * bs;
                pr = -0.5 * kg / (1 - kr) * gs + -0.5 * kb / (1 - kr) * bs + 1 / 2 * rs;
                
                r = 16 + 219 * y1;
                g = 128 + 224 * pb;
                b = 128 + 224 * pr;
                
                r = max(0, min(255, r));
                b = max(0, min(255, b));
                g = max(0, min(255, g));
                
                if (ycc->FilesCounter == 1)
                {
                    ycc->PIC[0][i][j][0] = r;
                    ycc->PIC[0][i][j][1] = g;
                    ycc->PIC[0][i][j][2] = b;
                }
                else
                {
                    ycc->PIC[0][i][j][0] = r;
                    ycc->PIC[1][i][j][1] = g;
                    ycc->PIC[2][i][j][2] = b;
                }
            }
}

void Ycbcr709ToRgb( Picture ycc, Picture *rgb )
{
    for (int f = 0; f < ycc.FilesCounter; f++)
        for (int i = 0; i < ycc.h[f]; i++)
            for (int j = 0; j < ycc.w[f]; j++)
            {
                unsigned char y1 = 0, cb = 0, cr = 0;
                double rs = 0, gs = 0, bs = 0, kr = 0.2126, kg = 0.7152, kb = 0.0722, r, g, b;
                if (ycc.FilesCounter == 1)
                {
                    y1 = ycc.PIC[0][i][j][0];
                    cb = ycc.PIC[0][i][j][1];
                    cr = ycc.PIC[0][i][j][2];
                }
                else
                {
                    y1 = ycc.PIC[0][i][j][0];
                    cb = ycc.PIC[1][i][j][1];
                    cr = ycc.PIC[2][i][j][2];
                }
                    
                r = (y1 - 16) / 219.0;
                g = (cb - 128) / 224.0;
                b = (cr - 128) / 224.0;
                
                rs = (r + (2 - 2 * kr) * b) * 255.0;
                gs = (r - kb / kg * (2 - kb) * g - kr / kg * (2 - kr) * b) * 255.0;
                bs = (r + (2 - 2 * kb) * g) * 255.0;
                
                rs = max(0, min(255, rs));
                bs = max(0, min(255, bs));
                gs = max(0, min(255, gs));

                if (rgb->FilesCounter == 1)
                {
                    rgb->PIC[0][i][j][0] = rs;
                    rgb->PIC[0][i][j][1] = gs;
                    rgb->PIC[0][i][j][2] = bs;
                }
                else
                {
                    rgb->PIC[0][i][j][0] = rs;
                    rgb->PIC[1][i][j][1] = gs;
                    rgb->PIC[2][i][j][2] = bs;
                }
            }
}

//
// END OF YCBCR709 FORMAT
//

//
// YCОCG FORMAT
//

void RgbToYcocg( Picture rgb, Picture *ycc )
{
    for (int f = 0; f < rgb.FilesCounter; f++)
        for (int i = 0; i < rgb.h[f]; i++)
            for (int j = 0; j < rgb.w[f]; j++)
            {
                unsigned char r = 0, g = 0, b = 0;
                double rs = 0, gs = 0, bs = 0, y1, pb, pr;
                
                if (rgb.FilesCounter == 1)
                {
                    r = rgb.PIC[0][i][j][0];
                    g = rgb.PIC[0][i][j][1];
                    b = rgb.PIC[0][i][j][2];
                }
                else
                {
                    r = rgb.PIC[0][i][j][0];
                    g = rgb.PIC[1][i][j][1];
                    b = rgb.PIC[2][i][j][2];
                }
                
                rs = 0.25 * r + 0.5 * g + 0.25 * b;
                gs = 0.5 * r - 0.5 * b;
                bs = -0.25 * r + 0.5 * g - 0.25 * b;
            
                rs = max(0, min(255, rs));
                bs = max(0, min(255, bs));
                gs = max(0, min(255, gs));

                if (ycc->FilesCounter == 1)
                {
                    ycc->PIC[0][i][j][0] = r;
                    ycc->PIC[0][i][j][1] = g;
                    ycc->PIC[0][i][j][2] = b;
                }
                else
                {
                    ycc->PIC[0][i][j][0] = r;
                    ycc->PIC[1][i][j][1] = g;
                    ycc->PIC[2][i][j][2] = b;
                }
            }
}

void Ycocg9ToRgb( Picture ycc, Picture *rgb )
{
    for (int f = 0; f < ycc.FilesCounter; f++)
        for (int i = 0; i < ycc.h[f]; i++)
            for (int j = 0; j < ycc.w[f]; j++)
            {
                double rs = 0, gs = 0, bs = 0, r, g, b;
                if (ycc.FilesCounter == 1)
                {
                    r = ycc.PIC[0][i][j][0];
                    g = ycc.PIC[0][i][j][1];
                    b = ycc.PIC[0][i][j][2];
                }
                else
                {
                    r = ycc.PIC[0][i][j][0];
                    g = ycc.PIC[1][i][j][1];
                    b = ycc.PIC[2][i][j][2];
                }
                    
                rs = 1 * r + 1 * g - 1 * b;
                gs = 1 * r + 1 * b;
                bs = 1 * r - 1 * g - 1 * b;
        
                rs = max(0, min(255, rs));
                bs = max(0, min(255, bs));
                gs = max(0, min(255, gs));

                if (rgb->FilesCounter == 1)
                {
                    rgb->PIC[0][i][j][0] = rs;
                    rgb->PIC[0][i][j][1] = gs;
                    rgb->PIC[0][i][j][2] = bs;
                }
                else
                {
                    rgb->PIC[0][i][j][0] = rs;
                    rgb->PIC[1][i][j][1] = gs;
                    rgb->PIC[2][i][j][2] = bs;
                }
            }
}

//
// END OF YCBCR709 FORMAT
//

// Save pnm image to file function.
// * ARGUMENTS:
//   - file structure:
//     FILE *F1.
// * RETURNS:
//   - None.
void SavePNM ( FILE *F1, int Index, Picture x )
{
    int i, j, k;
    
    if (PId == P5)
    {
        fprintf(F1, "P5\n");
        fprintf(F1, "%i %i\n", x.w[Index], x.h[Index]);
        fprintf(F1, "%i\n", x.grad[Index]);
        for (i = 0; i < x.h[Index]; i++)
        {
            for (j = 0; j < x.w[Index]; j++)
            {
                fprintf(F1, "%c", x.PIC[Index][i][j][0]);
            }
        }
    }
    else if (PId == P6)
    {
        std::cout << Index << "\n";
        fprintf(F1, "P6\n");
        fprintf(F1, "%i %i\n", x.w[0], x.h[0]);
        fprintf(F1, "%i\n", x.grad[0]);
        for (i = 0; i < x.h[Index]; i++)
        {
             for (j = 0; j < x.w[Index]; j++)
             {
                 if (x.FilesCounter == 1)
                     for (k = 0; k < 3; k++)
                        fprintf(F1, "%c", x.PIC[Index][i][j][k]);
                 else if (x.FilesCounter == 3)
                 {
                     unsigned char b = 0;
                     if (Index == 0)
                     {
                         fprintf(F1, "%c", x.PIC[Index][i][j][Index]);
                         fprintf(F1, "%c", b);
                         fprintf(F1, "%c", b);
                     }
                     else if (Index == 1)
                     {
                        fprintf(F1, "%c", b);
                        fprintf(F1, "%c", x.PIC[Index][i][j][Index]);
                        fprintf(F1, "%c", b);
                     }
                     else if (Index == 2)
                     {
                        fprintf(F1, "%c", b);
                        fprintf(F1, "%c", b);
                        fprintf(F1, "%c", x.PIC[Index][i][j][Index]);
                     }
                 }
             }
         }
     }
} // End of 'SavePNM' function


// Load pnm image from file function.
// * ARGUMENTS:
//   - file structure:
//     FILE *F1.
// * RETURNS:
//   - None.
int LoadPNM ( FILE *F1, int Index, Picture *x )
{
    int i, j, k;
    char s[4], b;
    
    s[0] = 0;
    s[1] = 0;
    s[2] = 0;
    s[3] = 0;
    fscanf(F1, "%s", s);
    if (s[1] == '5' && s[2] == 0 && (s[0] == 'P' || s[0] == 'p') && s[3] == 0)
    {
        PId = P5;
        fscanf(F1, "%i %i", &x->w[Index], &x->h[Index]);
        if (x->w[Index] > W_P || x->h[Index] > H_P)
        {
            printf("Too big picture!\n");
            return 0;
        }
        fscanf(F1, "%i", &x->grad[Index]);
        if (x->grad[Index] > 255 || x->grad[Index] < 0)
        {
            printf("Gradaction is out of range!\n");
            return 0;
        }
        fread(&b, 1, 1, F1);
        for (i = 0; i < x->h[Index]; i++)
            for (j = 0; j < x->w[Index]; j++)
            {
                if (fread(&b, 1, 1, F1) > 0)
                    for (k = 0; k < 3; k++)
                        x->PIC[Index][i][j][k] = b;
                else
                    return 0;
        }
        if (fread(&b, 1, 1, F1) > 0)
        {
            printf("File crashed!\n");
            return 0;
        }
        return 1;
     }
     else if (s[1] == '6' && s[2] == 0 && (s[0] == 'P' || s[0] == 'p') && s[3] == 0)
     {
         PId = P6;
         fscanf(F1, "%i %i", &x->w[Index], &x->h[Index]);
         if (x->w[Index] > W_P || x->h[Index] > H_P)
         {
             printf("Too big picture!\n");
             return 0;
         }
         fscanf(F1, "%i", &x->grad[Index]);
         if (x->grad[Index] > 255 || x->grad[Index] < 0)
         {
             printf("Gradaction is out of range!\n");
             return 0;
         }
         fread(&b, 1, 1, F1);
         for (i = 0; i < x->h[Index]; i++)
             for (j = 0; j < x->w[Index]; j++)
                 for (k = 0; k < 3; k++)
                 {
                     if (fread(&b, 1, 1, F1) > 0)
                         x->PIC[Index][i][j][k] = b;
                     else
                         return 0;
                 }
         if (fread(&b, 1, 1, F1) > 0)
         {
             printf("File crashed!\n");
             return 0;
         }
         return 1;
    }
    else
    {
        printf("No picture type!\n");
        return 0;
    }
} // End of 'LoadPNM' function

//
// Support functions
//

// Converting string to number function.
// * ARGUMENTS:
//   - string:
//     char *str.
// * RETURNS:
//   - (double) converted number.
double strtonumb( char *str )
{
    int i = 0;
    double numb = 0, af = 0, p = 10;
    
    while (str[i] != 0)
    {
        if (str[i] == '.')
        {
            i++;
            while (str[i] != 0)
            {
                af += (str[i] - '0') / p;
                p *= 10;
                i++;
            }
        }
        else
        {
            numb = numb * 10 + str[i] - '0';
            i++;
        }
    }
    numb += af;
    if (i == 0 || str[i] != 0)
        return -1;
    return numb;
} // End of 'strtonumb' function

// Sierra dithering algorithm picture function.
// * ARGUMENTS:
//    - counter of string arguments:
//      int argc;
//    - string arguments:
//      char * argv[].
// * RETURNS:
//    - (int) main return.
int main( int argc, char * argv[] )
{
    Picture In, Out;
    
    for (int i = 1; i < argc; i++)
        if (strcmp("-f", argv[i]) == 0)
        {
            std::cout << argv[i + 1];
            if (strcmp(argv[i + 1], "RGB") == 0)
                In.PictureID = 0;
            else if (strcmp(argv[i + 1], "HSL") == 0)
                In.PictureID = 1;
            else if (strcmp(argv[i + 1], "HSV") == 0)
                In.PictureID = 2;
            else if (strcmp(argv[i + 1], "YCBCR601") == 0)
                In.PictureID = 3;
            else if (strcmp(argv[i + 1], "YCBCR709") == 0)
                In.PictureID = 4;
            else if (strcmp(argv[i + 1], "YCOCG") == 0)
                In.PictureID = 5;
            else if (strcmp(argv[i + 1], "CMY") == 0)
                In.PictureID = 6;
            else
                printf("Wrong\n");
            break;
        }

    for (int i = 1; i < argc + 1; i++)
        if (strcmp("-t", argv[i]) == 0)
        {
            std::cout << argv[i + 1];
            if (strcmp(argv[i + 1], "RGB") == 0)
                Out.PictureID = 0;
            else if (strcmp(argv[i + 1], "HSL") == 0)
                Out.PictureID = 1;
            else if (strcmp(argv[i + 1], "HSV") == 0)
                Out.PictureID = 2;
            else if (strcmp(argv[i + 1], "YCBCR601") == 0)
                Out.PictureID = 3;
            else if (strcmp(argv[i + 1], "YCBCR709") == 0)
                Out.PictureID = 4;
            else if (strcmp(argv[i + 1], "YCOCG") == 0)
                Out.PictureID = 5;
            else if (strcmp(argv[i + 1], "CMY") == 0)
                Out.PictureID = 6;
            else
                printf("Wrong\n");
            break;
        }
        

    for (int i = 1; i < argc + 1; i++)
        if (strcmp("-i", argv[i]) == 0)
        {
            In.FilesCounter = strtonumb(argv[i + 1]);

            if (In.FilesCounter == 1)
            {
                strcpy(In.FileNames[0], argv[i + 2]);
                In.F[0] = fopen(argv[i + 2], "rb");
            }
            else if (In.FilesCounter == 3)
            {
                for (int k = 0; k < 3; k++)
                {
                    strcpy(In.FileNames[k], argv[i + 2 + k]);
                    In.F[k] = fopen(argv[i + 2 + k], "rb");
                }
            }
            break;
        }

    for (int i = 1; i < argc + 1; i++)
        if (strcmp("-o", argv[i]) == 0)
        {
            Out.FilesCounter = strtonumb(argv[i + 1]);

            if (Out.FilesCounter == 1)
            {
                strcpy(Out.FileNames[0], argv[i + 2]);
                Out.F[0] = fopen(argv[i + 2], "wb");
            }
            else if (Out.FilesCounter == 3)
            {
                for (int k = 0; k < 3; k++)
                {
                    strcpy(Out.FileNames[k], argv[i + 2 + k]);
                    Out.F[k] = fopen(argv[i + 2 + k], "wb");
                }
            }
            break;
        }
    if (Out.F[0] == NULL || In.F[0] == NULL)
        printf("Wrong\n");
    
    for (int i = 0; i < In.FilesCounter; i++)
        if (In.F[i] != NULL)
            LoadPNM(In.F[i], i, &In);

    if (Out.FilesCounter == 3)
    {
        for (int i = 0; i < 3; i++)
        {
            Out.h[i] = In.h[0];
            Out.w[i] = In.w[0];
            Out.grad[i] = In.grad[0];
        }
    }
    else if (Out.FilesCounter == 1)
    {
        Out.h[0] = In.h[0];
        Out.w[0] = In.w[0];
        Out.grad[0] = In.grad[0];
    }
    
    switch (In.PictureID)
    {
        case 1:
            std::cout << "HSL";
            for (int f = 0; f < In.FilesCounter; f++)
                for (int i = 0; i < In.h[f]; i++)
                    for (int j = 0; j < In.w[f]; j++)
                    {
                        unsigned char h = 0, s = 0, l = 0, maximun, minimum, rs, gs, bs;
                        double ls, hs, ss, q, p, tr, tg, tb, hk;
                        if (In.FilesCounter == 1)
                        {
                            h = In.PIC[0][i][j][0];
                            s = In.PIC[0][i][j][1];
                            l = In.PIC[0][i][j][2];
                        }
                        else
                        {
                            h = In.PIC[0][i][j][0];
                            s = In.PIC[1][i][j][1];
                            l = In.PIC[2][i][j][2];
                        }
                        hs = h / 255.0 * 360;
                        ss = s / 255.0;
                        ls = l / 255.0;
                        
                        if (ls < 0.5)
                            q = ls * (1.0 + ss);
                        else if (ls >= 0.5)
                            q = ls + ss - (ls * ss);
                        p = 2 * ls - q;
                        hk = hs / 360.0;
                        tr = hk + 1 / 3.0;
                        tg = hk;
                        tb = hk - 1 / 3.0;
                            
                        if (tr < 0)
                            tr++;
                        else if (tr > 1)
                            tr--;
                        if (tg < 0)
                            tg++;
                        else if (tg > 1)
                            tg--;
                        if (tb < 0)
                            tb++;
                        else if (tb > 1)
                            tb--;
                        
                        if (tr < 1 / 6.0)
                            tr = p + ((q - p) * 6.0 * tr);
                        else if (tr >= 1 / 6.0 && tr < 0.5)
                            tr = q;
                        else if (tr >= 0.5 && tr < 2 / 3.0)
                            tr = p + ((q - p) * (2 / 3.0 - tr) * 6.0);
                        else
                            tr = p;
                        
                        if (tg < 1 / 6.0)
                            tg = p + ((q - p) * 6.0 * tg);
                        else if (tg >= 1 / 6.0 && tg < 0.5)
                            tg = q;
                        else if (tg >= 0.5 && tg < 2 / 3.0)
                            tg = p + ((q - p) * (2 / 3.0 - tg) * 6.0);
                        else
                            tg = p;
                        
                        if (tb < 1 / 6.0)
                            tb = p + ((q - p) * 6.0 * tb);
                        else if (tb >= 1 / 6.0 && tb < 0.5)
                            tb = q;
                        else if (tb >= 0.5 && tb < 2 / 3.0)
                            tb = p + ((q - p) * (2 / 3.0 - tb) * 6.0);
                        else
                            tb = p;
                        
                        tg *= 255;
                        tb *= 255;
                        tr *= 255;
                        
                        if (tr > 255.0)
                            tr = 255;
                        else if (tr < 0)
                            tr = 0;
                        if (tg > 255.0)
                            tg = 255;
                        else if (tg < 0)
                            tg = 0;
                        if (tb > 255.0)
                            tb = 255;
                        else if (tb < 0)
                            tb = 0;
                        
                        rs = tr;
                        gs = tg;
                        bs = tb;
                        
                        if (Out.FilesCounter == 1)
                        {
                            Out.PIC[0][i][j][0] = rs;
                            Out.PIC[0][i][j][1] = gs;
                            Out.PIC[0][i][j][2] = bs;
                        }
                        else
                        {
                            Out.PIC[0][i][j][0] = rs;
                            Out.PIC[1][i][j][1] = gs;
                            Out.PIC[2][i][j][2] = bs;
                        }
                    }
            break;
        case 2:
            for (int f = 0; f < In.FilesCounter; f++)
                for (int i = 0; i < In.h[0]; i++)
                    for (int j = 0; j < In.w[0]; j++)
                    {
                        unsigned char h = 0, s = 0, v = 0;
                        double ss, hs, vs, hmod, vmin, vdec, vinc, a, rs = 0, gs = 0, bs = 0;
                        if (In.FilesCounter == 1)
                        {
                            h = In.PIC[0][i][j][0];
                            s = In.PIC[0][i][j][1];
                            v = In.PIC[0][i][j][2];
                        }
                        else
                        {
                            h = In.PIC[0][i][j][0];
                            s = In.PIC[1][i][j][1];
                            v = In.PIC[2][i][j][2];
                        }
                        
                        hs = h / 255.0 * 360;
                        ss = s / 255.0 * 100;
                        vs = v / 255.0 * 100;
                        
                        hmod = (int)hs / 60 % 6;
                        vmin = (100 - ss) * vs / 100.0 ;
                        a = (vs - vmin) * ((int)hs % 60) / 60.0;
                        vinc = vmin + a;
                        vdec = vs - a;
                        
                        if (hmod == 0)
                        {
                            rs = vs / 100.0 * 255;
                            gs = vinc / 100.0 * 255;
                            bs = vmin / 100.0 * 255;
                        }
                        else if (hmod == 1)
                        {
                            rs = vdec / 100.0 * 255;
                            gs = vs / 100.0 * 255;
                            bs = vmin / 100.0 * 255;
                        }
                        else if (hmod == 2)
                        {
                            rs = vmin / 100.0 * 255;
                            gs = vs / 100.0 * 255;
                            bs = vinc / 100.0 * 255;
                        }
                        else if (hmod == 3)
                        {
                            rs = vmin / 100.0 * 255;
                            gs = vdec / 100.0 * 255;
                            bs = v / 100.0 * 255;
                        }
                        else if (hmod == 4)
                        {
                            rs = vinc / 100.0 * 255;
                            gs = vmin / 100.0 * 255;
                            bs = vs / 100.0 * 255;
                        }
                        else if (hmod == 5)
                        {
                            rs = vs / 100.0 * 255;
                            gs = vmin / 100.0 * 255;
                            bs = vdec / 100.0 * 255;
                        }
                        
                        if (rs < 0)
                            rs = 0;
                        else if (rs > 255)
                            rs = 255;
                        if (gs < 0)
                            gs = 0;
                        else if (gs > 255)
                            gs = 255;
                        if (bs < 0)
                            bs = 0;
                        else if (bs > 255)
                            bs = 255;
                        
                        if (Out.FilesCounter == 1)
                        {
                            Out.PIC[0][i][j][0] = rs;
                            Out.PIC[0][i][j][1] = gs;
                            Out.PIC[0][i][j][2] = bs;
                        }
                        else
                        {
                            Out.PIC[0][i][j][0] = rs;
                            Out.PIC[1][i][j][1] = gs;
                            Out.PIC[2][i][j][2] = bs;
                        }
                    }

            break;
        case 3:
                for (int f = 0; f < In.FilesCounter; f++)
                    for (int i = 0; i < In.h[f]; i++)
                        for (int j = 0; j < In.w[f]; j++)
                        {
                            unsigned char y1 = 0, cb = 0, cr = 0;
                            double rs = 0, gs = 0, bs = 0, kr = 0.299, kg = 0.587, kb = 0.114, r, g, b;
                            if (In.FilesCounter == 1)
                            {
                                y1 = In.PIC[0][i][j][0];
                                cb = In.PIC[0][i][j][1];
                                cr = In.PIC[0][i][j][2];
                            }
                            else
                            {
                                y1 = In.PIC[0][i][j][0];
                                cb = In.PIC[1][i][j][1];
                                cr = In.PIC[2][i][j][2];
                            }
                                
                            r = y1;
                            g = cb - 128;
                            b = cr - 128;
                            
                            if (cb - 128 < 0)
                                 g = 0;
                            else if (cb - 128 > 255)
                                 g = 255;
                            if (cr - 128 < 0)
                                 b = 0;
                            else if (cr - 128 > 255)
                                 b = 255;

                            rs = (r + (2 - 2.0 * kr) * b);
                            gs = (r - kb / kg * (2.0 - kb) * g - kr / kg * (2.0 - kr) * b);
                            bs = (r + (2 - 2.0 * kb) * g);
                            
                            if (Out.FilesCounter == 1)
                            {
                                Out.PIC[0][i][j][0] = rs;
                                Out.PIC[0][i][j][1] = gs;
                                Out.PIC[0][i][j][2] = bs;
                            }
                            else
                            {
                                Out.PIC[0][i][j][0] = rs;
                                Out.PIC[1][i][j][1] = gs;
                                Out.PIC[2][i][j][2] = bs;
                            }
                        }
            break;
        case 4:
            for (int f = 0; f < In.FilesCounter; f++)
                for (int i = 0; i < In.h[f]; i++)
                    for (int j = 0; j < In.w[f]; j++)
                    {
                        unsigned char y1 = 0, cb = 0, cr = 0;
                        double rs = 0, gs = 0, bs = 0, kr = 0.2126, kg = 0.7152, kb = 0.0722, r, g, b;
                        
                        if (In.FilesCounter == 1)
                        {
                            y1 = In.PIC[0][i][j][0];
                            cb = In.PIC[0][i][j][1];
                            cr = In.PIC[0][i][j][2];
                        }
                        else
                        {
                            y1 = In.PIC[0][i][j][0];
                            cb = In.PIC[1][i][j][1];
                            cr = In.PIC[2][i][j][2];
                        }
                            
                        r = y1;
                        g = cb - 128;
                        b = cr - 128;
                        
                        if (cb - 128 < 0)
                             g = 0;
                        else if (cb - 128 > 255)
                             g = 255;
                        if (cr - 128 < 0)
                             b = 0;
                        else if (cr - 128 > 255)
                             b = 255;
                        
                        
                        rs = (r + (2 - 2.0 * kr) * b);
                        gs = (r - kb / kg * (2.0 - kb) * g - kr / kg * (2.0 - kr) * b);
                        bs = (r + (2 - 2.0 * kb) * g);
                        
                        std::cout << rs << gs << bs;
                        if (Out.FilesCounter == 1)
                        {
                            Out.PIC[0][i][j][0] = rs;
                            Out.PIC[0][i][j][1] = gs;
                            Out.PIC[0][i][j][2] = bs;
                        }
                        else
                        {
                            Out.PIC[0][i][j][0] = rs;
                            Out.PIC[1][i][j][1] = gs;
                            Out.PIC[2][i][j][2] = bs;
                        }
                    }

            break;
        case 5:
                for (int f = 0; f < In.FilesCounter; f++)
                    for (int i = 0; i < In.h[f]; i++)
                        for (int j = 0; j < In.w[f]; j++)
                        {
                            double rs = 0, gs = 0, bs = 0, r, g, b;
                            if (In.FilesCounter == 1)
                            {
                                r = In.PIC[0][i][j][0];
                                g = In.PIC[0][i][j][1];
                                b = In.PIC[0][i][j][2];
                            }
                            else
                            {
                                r = In.PIC[0][i][j][0];
                                g = In.PIC[1][i][j][1];
                                b = In.PIC[2][i][j][2];
                            }
                                
                            rs = 1 * r + 1 * g - 1 * b;
                            gs = 1 * r + 1 * b;
                            bs = 1 * r - 1 * g - 1 * b;
                    
                            if (Out.FilesCounter == 1)
                            {
                                Out.PIC[0][i][j][0] = rs;
                                Out.PIC[0][i][j][1] = gs;
                                Out.PIC[0][i][j][2] = bs;
                            }
                            else
                            {
                                Out.PIC[0][i][j][0] = rs;
                                Out.PIC[1][i][j][1] = gs;
                                Out.PIC[2][i][j][2] = bs;
                            }
                        }
            break;
        case CMY:
            std::cout << "CMY\n";
            for (int f = 0; f < In.FilesCounter; f++)
                for (int i = 0; i < In.h[f]; i++)
                    for (int j = 0; j < In.w[f]; j++)
                    {
                        double rs = 0, gs = 0, bs = 0, r, g, b;
                        if (In.FilesCounter == 1)
                        {
                            r = In.PIC[0][i][j][0];
                            g = In.PIC[0][i][j][1];
                            b = In.PIC[0][i][j][2];
                        }
                        else
                        {
                            r = In.PIC[0][i][j][0];
                            g = In.PIC[1][i][j][1];
                            b = In.PIC[2][i][j][2];
                        }
                            
                        rs = 255 - r;
                        gs = 255 - g;
                        bs = 255 - b;
                
                        if (Out.FilesCounter == 1)
                        {
                            Out.PIC[0][i][j][0] = rs;
                            Out.PIC[0][i][j][1] = gs;
                            Out.PIC[0][i][j][2] = bs;
                        }
                        else
                        {
                            Out.PIC[0][i][j][0] = rs;
                            Out.PIC[1][i][j][1] = gs;
                            Out.PIC[2][i][j][2] = bs;
                        }
                    }
            break;
        case 0:
            for (int i = 0; i < In.h[0]; i++)
            for (int j = 0; j < In.w[0]; j++)
                if (Out.FilesCounter == 1)
                {
                    if (In.FilesCounter == 1)
                    {
                        Out.PIC[0][i][j][0] = In.PIC[0][i][j][0];
                        Out.PIC[0][i][j][1] = In.PIC[0][i][j][1];
                        Out.PIC[0][i][j][2] = In.PIC[0][i][j][2];
                    }
                    else
                    {
                        Out.PIC[0][i][j][0] = In.PIC[0][i][j][0];
                        Out.PIC[0][i][j][1] = In.PIC[1][i][j][1];
                        Out.PIC[0][i][j][2] = In.PIC[2][i][j][2];
                    }
                }
                else
                {
                    if (In.FilesCounter == 1)
                    {
                        Out.PIC[0][i][j][0] = In.PIC[0][i][j][0];
                        Out.PIC[1][i][j][1] = In.PIC[0][i][j][1];
                        Out.PIC[2][i][j][2] = In.PIC[0][i][j][2];
                    }
                    else
                    {
                        Out.PIC[0][i][j][0] = In.PIC[0][i][j][0];
                        Out.PIC[1][i][j][1] = In.PIC[1][i][j][1];
                        Out.PIC[2][i][j][2] = In.PIC[2][i][j][2];
                    }
                }
            break;
    }
    
    switch (Out.PictureID)
    {
        case RGB:
            break;
        case HSL:
            std::cout << Out.FilesCounter;
            for (int f = 0; f < Out.FilesCounter; f++)
                for (int i = 0; i < Out.h[f]; i++)
                    for (int j = 0; j < Out.w[f]; j++)
                    {
                    unsigned char r = 0, g = 0, b = 0, maximun, minimum, hs, ss, ls;
                    double h, s, l;
                    
                    if (Out.FilesCounter == 1)
                    {
                        r = Out.PIC[0][i][j][0];
                        g = Out.PIC[0][i][j][1];
                        b = Out.PIC[0][i][j][2];
                    }
                    else
                    {
                        r = Out.PIC[0][i][j][0];
                        g = Out.PIC[1][i][j][1];
                        b = Out.PIC[2][i][j][2];
                    }
                    maximun = max(max(r, g), b);
                    minimum  = min(min(r, g), b);
                    l = 0.5 * (maximun + minimum);
                    ls = l;

                    if (0 < l / 255.0 && l / 255.0 <= 0.5)
                        s = ((maximun / 255.0) - (minimum / 255.0)) / (2 * l / 255.0) * 255.0;
                    else if (0.5 < l / 255.0 && l / 255.0 <= 1)
                        s = ((maximun / 255.0) - (minimum / 255.0)) / (2 - 2 * l / 255.0) * 255.0;
                    else if (maximun == minimum || l == 0)
                        s = 0;
                    ss = s;
                    if (s > 255)
                        ss = 255;
                    else if (s < 0)
                        ss = 0;

                    if (maximun == b)
                        h = 60 * ((r - g) * 1.0) / (maximun - minimum) + 240;
                    else if (maximun == g)
                        h = 60 * ((b - r) * 1.0) / (maximun - minimum) + 120;
                    else if (maximun == r && g < b)
                        h = 60 * ((g - b) * 1.0) / (maximun - minimum) + 360;
                    else if (maximun == r && g >= b)
                        h = 60 * ((g - b) * 1.0) / (maximun - minimum);
                    else if (maximun == minimum)
                        h = 0;
                        
                    if (h < 0)
                        h += 360;
                        
                    h = h / 360.0 * 255.0;
                    hs = h;
                    if (hs > 255)
                        hs = 255;
                    else if (hs < 0)
                        hs = 0;
                        
                    if (Out.FilesCounter == 1)
                    {
                        Out.PIC[0][i][j][0] = hs;
                        Out.PIC[0][i][j][1] = ss;
                        Out.PIC[0][i][j][2] = ls;
                    }
                    else
                    {
                        Out.PIC[0][i][j][0] = hs;
                        Out.PIC[1][i][j][1] = ss;
                        Out.PIC[2][i][j][2] = ls;
                    }
                }
            break;
        case HSV:
            for (int f = 0; f < Out.FilesCounter; f++)
            for (int i = 0; i < In.h[0]; i++)
                for (int j = 0; j < In.w[0]; j++)
                {
                    unsigned char r = 0, g = 0, b = 0, maximun, minimum, hs, ss, vs;
                    double h = 0, s = 0, v = 0;
                    
                    if (Out.FilesCounter == 1)
                    {
                        r = Out.PIC[0][i][j][0];
                        g = Out.PIC[0][i][j][1];
                        b = Out.PIC[0][i][j][2];
                    }
                    else
                    {
                        r = Out.PIC[0][i][j][0];
                        g = Out.PIC[1][i][j][1];
                        b = Out.PIC[2][i][j][2];
                    }
                    maximun = max(max(r, g), b);
                    minimum = min(min(r, g), b);
                    
                    v = maximun;
                    vs = v;
                    
                    if (maximun == 0)
                        s = 0;
                    else
                        s = 1 - minimum * 1.0 / maximun;
                    s *= 255;
                    ss = s;
                    if (s > 255)
                        ss = 255;
                    else if (s < 0)
                        ss = 0;
                    
                    if (maximun == b)
                        h = 60 * ((r - g) * 1.0) / (maximun - minimum) + 240;
                    else if (maximun == g)
                        h = 60 * ((b - r) * 1.0) / (maximun - minimum) + 120;
                    else if (maximun == r && g < b)
                        h = 60 * ((g - b) * 1.0) / (maximun - minimum) + 360;
                    else if (maximun == r && g >= b)
                        h = 60 * ((g - b) * 1.0) / (maximun - minimum);
                    else if (maximun == minimum)
                        h = 0;
                    
                    if (h < 0)
                        h += 360;
                    h = h / 360.0 * 255.0;
                    hs = h;
                    if (hs > 255)
                        hs = 255;
                    else if (hs < 0)
                        hs = 0;
                    
                    if (Out.FilesCounter == 1)
                    {
                        Out.PIC[0][i][j][0] = hs;
                        Out.PIC[0][i][j][1] = ss;
                        Out.PIC[0][i][j][2] = vs;
                    }
                    else
                    {
                        Out.PIC[0][i][j][0] = hs;
                        Out.PIC[1][i][j][1] = ss;
                        Out.PIC[2][i][j][2] = vs;
                    }
                }
            break;
        case YCBCR601:
            for (int f = 0; f < Out.FilesCounter; f++)
                for (int i = 0; i < Out.h[0]; i++)
                    for (int j = 0; j < Out.w[0]; j++)
                    {
                        unsigned char r = 0, g = 0, b = 0;
                        double rs = 0, gs = 0, bs = 0, kr = 0.299, kg = 0.587, kb = 0.114, y1, pb, pr;
                        
                        if (Out.FilesCounter == 1)
                        {
                            r = Out.PIC[0][i][j][0];
                            g = Out.PIC[0][i][j][1];
                            b = Out.PIC[0][i][j][2];
                        }
                        else
                        {
                            r = Out.PIC[0][i][j][0];
                            g = Out.PIC[1][i][j][1];
                            b = Out.PIC[2][i][j][2];
                        }
                    
                        rs = r;
                        gs = g;
                        bs = b;
                        
                        y1 = kr * rs + kg * gs + kb * bs;
                        pb = -0.5 * kr / (1 - kb) * rs + -0.5 * kg / (1 - kb) * gs + 1 / 2 * bs;
                        pr = -0.5 * kg / (1 - kr) * gs + -0.5 * kb / (1 - kr) * bs + 1 / 2 * rs;
                        
                        r = y1;
                        g = 128 + pb;
                        b = 128 + pr;
                        
                        if (y1 < 0)
                            r = 0;
                        else if (y1 > 255)
                            r = 255;
                        if (pb + 128 < 0)
                             g = 0;
                        else if (pb + 128 > 255)
                             g = 255;
                        if (pr + 128 < 0)
                             b = 0;
                        else if (pr + 128 > 255)
                             b = 255;
                        
                        if (Out.FilesCounter == 1)
                        {
                            Out.PIC[0][i][j][0] = r;
                            Out.PIC[0][i][j][1] = g;
                            Out.PIC[0][i][j][2] = b;
                        }
                        else
                        {
                            Out.PIC[0][i][j][0] = r;
                            Out.PIC[1][i][j][1] = g;
                            Out.PIC[2][i][j][2] = b;
                        }
                    }
            break;
        case YCBCR709:
            for (int f = 0; f < Out.FilesCounter; f++)
            for (int i = 0; i < Out.h[0]; i++)
                for (int j = 0; j < Out.w[0]; j++)
                {
                    unsigned char r = 0, g = 0, b = 0;
                    double rs = 0, gs = 0, bs = 0, kr = 0.2126, kg = 0.7152, kb = 0.0722, y1, pb, pr;
                    
                    if (Out.FilesCounter == 1)
                    {
                        r = Out.PIC[0][i][j][0];
                        g = Out.PIC[0][i][j][1];
                        b = Out.PIC[0][i][j][2];
                    }
                    else
                    {
                        r = Out.PIC[0][i][j][0];
                        g = Out.PIC[1][i][j][1];
                        b = Out.PIC[2][i][j][2];
                    }
                    
                    rs = r;
                    gs = g;
                    bs = b;
                    y1 = kr * rs + kg * gs + kb * bs;
                    pb = -0.5 * kr / (1 - kb) * rs + -0.5 * kg / (1 - kb) * gs + 1 / 2 * bs;
                    pr = -0.5 * kg / (1 - kr) * gs + -0.5 * kb / (1 - kr) * bs + 1 / 2 * rs;
                    
                    r = y1;
                    g = 128 + pb;
                    b = 128 + pr;
                    
                    if (y1 < 0)
                        r = 0;
                    else if (y1 > 255)
                        r = 255;
                    if (pb + 128 < 0)
                         g = 0;
                    else if (pb + 128 > 255)
                         g = 255;
                    if (pr + 128 < 0)
                         b = 0;
                    else if (pr + 128 > 255)
                         b = 255;
                    
                    
                    if (Out.FilesCounter == 1)
                    {
                        Out.PIC[0][i][j][0] = r;
                        Out.PIC[0][i][j][1] = g;
                        Out.PIC[0][i][j][2] = b;
                    }
                    else
                    {
                        Out.PIC[0][i][j][0] = r;
                        Out.PIC[1][i][j][1] = g;
                        Out.PIC[2][i][j][2] = b;
                    }
                }
            break;
        case YCOCG:
            for (int f = 0; f < Out.FilesCounter; f++)
                for (int i = 0; i < Out.h[f]; i++)
                    for (int j = 0; j < Out.w[f]; j++)
                    {
                        unsigned char r = 0, g = 0, b = 0;
                        double rs = 0, gs = 0, bs = 0, y1, pb, pr;
                        
                        if (Out.FilesCounter == 1)
                        {
                            r = Out.PIC[0][i][j][0];
                            g = Out.PIC[0][i][j][1];
                            b = Out.PIC[0][i][j][2];
                        }
                        else
                        {
                            r = Out.PIC[0][i][j][0];
                            g = Out.PIC[1][i][j][1];
                            b = Out.PIC[2][i][j][2];
                        }
                            
                        rs = 0.25 * r + 0.5 * g + 0.25 * b;
                        gs = 0.5 * r - 0.5 * b;
                        bs = -0.25 * r + 0.5 * g - 0.25 * b;
                    
                        if (Out.FilesCounter == 1)
                        {
                            Out.PIC[0][i][j][0] = rs;
                            Out.PIC[0][i][j][1] = gs;
                            Out.PIC[0][i][j][2] = bs;
                        }
                        else
                        {
                            Out.PIC[0][i][j][0] = rs;
                            Out.PIC[1][i][j][1] = gs;
                            Out.PIC[2][i][j][2] = bs;
                        }
                    }
            break;
        case CMY:
            for (int f = 0; f < Out.FilesCounter; f++)
                for (int i = 0; i < In.h[0]; i++)
                    for (int j = 0; j < In.w[0]; j++)
                    {
                        unsigned char r = 0, g = 0, b = 0;
                        double rs = 0, gs = 0, bs = 0;
                        
                        if (Out.FilesCounter == 1)
                        {
                            r = Out.PIC[0][i][j][0];
                            g = Out.PIC[0][i][j][1];
                            b = Out.PIC[0][i][j][2];
                        }
                        else
                        {
                            r = Out.PIC[0][i][j][0];
                            g = Out.PIC[1][i][j][1];
                            b = Out.PIC[2][i][j][2];
                        }
                        
                        rs = 255 - r;
                        gs = 255 - g;
                        bs = 255 - b;
                        
                        if (Out.FilesCounter == 1)
                        {
                            Out.PIC[0][i][j][0] = rs;
                            Out.PIC[0][i][j][1] = gs;
                            Out.PIC[0][i][j][2] = bs;
                        }
                        else
                        {
                            Out.PIC[0][i][j][0] = rs;
                            Out.PIC[1][i][j][1] = gs;
                            Out.PIC[2][i][j][2] = bs;
                        }
                    }

            break;
    }
    
    
    for (int i = 0; i < Out.FilesCounter; i++)
        SavePNM(Out.F[i], i, Out);
    
} // End of 'main' function

// END OF 'MAIN.CPP' FILE

