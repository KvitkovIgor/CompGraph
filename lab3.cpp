#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define UC unsigned char

class Picture
{
public:
    FILE *out;
    int h, w, max, color;
    char format[3];
    char symb;
    enum COLOR_TYPE
    {
        RGBTYPE,
        HSLTYPE,
        HSVTYPE,
        YCBCR601TYPE,
        YCBCR709TYPE,
        YCOCGTYPE,
        CMYTYPE
    };
    UC *RGB;
    UC *HSV;
    UC *HSL;
    UC *ycbcr601;
    UC *ycbcr709;
    UC *ycocg;
    UC *cmy;
    Picture(FILE *in_file, COLOR_TYPE from_color)
    {
        fscanf(in_file, "%2s %d %d %d%c", &format, &w, &h, &max, &symb);
        if (format[1] == '5')
            color = 1;
        else if (format[1] == '6')
            color = 3;

        RGB = (UC *)malloc(color * w * h);
        HSV = (UC *)malloc(color * w * h);
        HSL = (UC *)malloc(color * w * h);
        ycbcr601 = (UC *)malloc(color * w * h);
        ycbcr709 = (UC *)malloc(color * w * h);
        ycocg = (UC *)malloc(color * w * h);
        cmy = (UC *)malloc(color * w * h);
        
        switch (from_color)
        {
            case RGBTYPE:
                fread(RGB, 1, w * h * color, in_file);
                break;
            case HSLTYPE:
                fread(HSL, 1, w * h * color, in_file);
                from_HSL();
                break;
            case HSVTYPE:
                fread(HSV, 1, w * h * color, in_file);
                from_HSV();
                break;
            case YCBCR601TYPE:
                fread(ycbcr601, 1, w * h * color, in_file);
                from_YCbCr601();
                break;
            case YCBCR709TYPE:
                fread(ycbcr709, 1, w * h * color, in_file);
                from_YCbCr709();
                break;
            case YCOCGTYPE:
                fread(ycocg, 1, w * h * color, in_file);
                from_YCoCg();
                break;
            case CMYTYPE:
                fread(cmy, 1, w * h * color, in_file);
                from_CMY();
                break;
        }
    }
    Picture(const Picture &pic) :h(pic.h), w(pic.w), max(pic.max), color(pic.color), symb(pic.symb)
    {
        for (int i = 0; i < 3; i++)
            format[i] = pic.format[i];
        RGB = (UC *)malloc(color * w * h * sizeof(UC));
        for (int i = 0; i < h*w*color; i++)
            RGB[i] = pic.RGB[i];
    }
    ~Picture()
    {
        free(RGB);
    }
    void to_HSV()
    {
        float* cRGB = (float*)malloc(color * w * h * sizeof(float));
        for (int i = 0; i < color * w * h; i += 3)
        {
            cRGB[i] = RGB[i];
            cRGB[i + 1] = RGB[i + 1];
            cRGB[i + 2] = RGB[i + 2];
            UC MAX = (cRGB[i] >= cRGB[i + 1]) && (cRGB[i] >= cRGB[i + 2]) ? cRGB[i] : cRGB[i + 1] >= cRGB[i + 2] ? cRGB[i + 1] : cRGB[i + 2];
            UC MIN = (cRGB[i] <= cRGB[i + 1]) && (cRGB[i] <= cRGB[i + 2]) ? cRGB[i] : cRGB[i + 1] <= cRGB[i + 2] ? cRGB[i + 1] : cRGB[i + 2];
            HSV[i + 2] = MAX;
            
            if (MAX == MIN) {
                HSV[i] = HSV[i + 1] = 0;
                continue;
            }
            if (MAX == cRGB[i + 1])
                HSV[i] = (60 * (cRGB[i + 2] - cRGB[i]) / (MAX - MIN) + 120) * 255. / 360;
            else if (MAX == cRGB[i + 2])
                HSV[i] = (60 * (cRGB[i] - cRGB[i + 1]) / (MAX - MIN) + 240) * 255. / 360;
            else if (cRGB[i + 1]>= cRGB[i + 2])
                HSV[i] = (60 * (cRGB[i + 1] - cRGB[i + 2]) / (MAX - MIN)) * 255. / 360;
            else if (cRGB[i + 1] < cRGB[i + 2])
                HSV[i] = (60 * (cRGB[i + 1] - cRGB[i + 2]) / (MAX - MIN) + 360) * 255. / 360;
            HSV[i + 1] = (MAX == 0 ? 0 : (1. - (float)MIN / MAX)*255);
        }

    }

    void from_HSV()
    {
        float* cHSV = (float*)malloc(color * w * h * sizeof(float));
        for (int i = 0; i < color * w * h; i += 3)
        {
            cHSV[i] = HSV[i];
            cHSV[i + 1] = HSV[i + 1];
            cHSV[i + 2] = HSV[i + 2];

            float V_min = round((255. - cHSV[i + 1]) * cHSV[i + 2] / 255);
            float a = round((cHSV[i + 2] - V_min) * (int(round(cHSV[i] * 360. / 255)) % 60 / 60.));
            float V_inc = round(V_min + a);
            float V_dec = round(cHSV[i + 2] - a);
            float Hi = (int(round(cHSV[i] * 360. / 255) / 60)) % 6;
            if (Hi == 0)
            {
                RGB[i] = cHSV[i + 2];
                RGB[i + 1] = V_inc;
                RGB[i + 2] = V_min;
            }
            else if (Hi == 1)
            {
                RGB[i] = V_dec;
                RGB[i + 1] = cHSV[i + 2];
                RGB[i + 2] = V_min;
            }
            else if (Hi == 2)
            {
                RGB[i] = V_min;
                RGB[i + 1] = cHSV[i + 2];
                RGB[i + 2] = V_inc;
            }
            else if (Hi == 3)
            {
                RGB[i] = V_min;
                RGB[i + 1] = V_dec;
                RGB[i + 2] = cHSV[i + 2];
            }
            else if (Hi == 4)
            {
                RGB[i] = V_inc;
                RGB[i + 1] = V_min;
                RGB[i + 2] = cHSV[i + 2];
            }
            else if (Hi == 5)
            {
                RGB[i] = cHSV[i + 2];
                RGB[i + 1] = V_min;
                RGB[i + 2] = V_dec;
            }
        }
    }

    void to_HSL()
    {
        float* cRGB = (float*)malloc(color * w * h * sizeof(float));
        for (int i = 0; i < color * w * h; i += 3)
        {
            cRGB[i] = RGB[i] / 255.0;
            cRGB[i + 1] = RGB[i + 1] / 255.0;
            cRGB[i + 2] = RGB[i + 2] / 255.0;
            float MAX = (cRGB[i] >= cRGB[i + 1]) && (cRGB[i] >= cRGB[i + 2]) ? cRGB[i] : cRGB[i + 1] >= cRGB[i + 2] ? cRGB[i + 1] : cRGB[i + 2];
            float MIN = (cRGB[i] <= cRGB[i + 1]) && (cRGB[i] <= cRGB[i + 2]) ? cRGB[i] : cRGB[i + 1] <= cRGB[i + 2] ? cRGB[i + 1] : cRGB[i + 2];
            float L = (MAX + MIN) / 2.;
            HSL[i + 2] = round(L * 255);

            if ((MAX == MIN) || (L == 0.)) {
                HSL[i] = HSL[i + 1] = 0;
                continue;
            }
            if (0.5 < L)
                HSL[i + 1] = round(255 * (MAX - MIN) / (2. - 2. * L));
            else
                HSL[i + 1] = round(255 * ((MAX - MIN) / (2. * L)));

            if (MAX == cRGB[i])
            {
                if (cRGB[i + 1] >= cRGB[i + 2])
                    HSL[i] = round((60 * (cRGB[i + 1] - cRGB[i + 2]) / (MAX - MIN)) * 255. / 360.);
                else if (cRGB[i + 1] < cRGB[i + 2])
                    HSL[i] = round((60 * (cRGB[i + 1] - cRGB[i + 2]) / (MAX - MIN) + 360) * 255. / 360.);
            }
            else if (MAX == cRGB[i + 1])
                HSL[i] = round((60. * (cRGB[i + 2] - cRGB[i]) / (MAX - MIN) + 120.) * 255. / 360.);
            else
                HSL[i] = round((60 * (cRGB[i] - cRGB[i + 1]) / (MAX - MIN) + 240) * 255. / 360.);
        }
    }
    
    
    void from_HSL()
    {
        float* cHSL = (float*)malloc(color * w * h * sizeof(float));

        for (int i = 0; i < w * h; i++)
        {
            cHSL[i * 3] = HSL[i * 3] / 255.;
            cHSL[i * 3 + 1] = HSL[i * 3 + 1] / 255.;
            cHSL[i * 3 + 2] = HSL[i * 3 + 2] / 255.;
            float Q = (cHSL[i * 3 + 2] < 0.5 ? (cHSL[i * 3 + 2] *(1. + cHSL[i * 3 + 1])) : (cHSL[i * 3 + 2] + cHSL[i * 3 + 1] - (cHSL[i * 3 + 2] * cHSL[i * 3 + 1])));
            float P = 2. * cHSL[i * 3 + 2] - Q;
            float Hi = cHSL[i * 3];
            float Tc[3];
            Tc[0] = Hi + 1. / 3;
            Tc[1] = Hi;
            Tc[2] = Hi - 1. / 3;
            for (int k = 0; k < 3; k++)
            {
                Tc[k] = Tc[k] < 0 ? Tc[k] + 1. : Tc[k] > 1 ? Tc[k] - 1.0 : Tc[k];
                if (Tc[k] < (1. / 6.))
                    RGB[i * 3 + k] = round(255.*(P + ((Q - P) * 6.0 * Tc[k])));
                else if (Tc[k] < (1.0 / 2.0))
                    RGB[i * 3 + k] = round(255.* Q);
                else if (Tc[k] < (2.0 / 3.0))
                    RGB[i * 3 + k] = round(255.*(P + ((Q - P) * (2.0 / 3.0 - Tc[k]) * 6.0)));
                else
                    RGB[i * 3 + k] = round(255.*P);
            }
        }
    }



    void to_YCbCr601()
    {
        float Kr = 0.299;
        float Kg = 0.587;
        float Kb = 0.114;
        for (int i = 0; i < color * w * h; i += 3)
        {
            ycbcr601[i] = round(Kr*RGB[i] + Kg * RGB[i + 1] + Kb * RGB[i + 2]);
            ycbcr601[i + 1] = 128 + round(0.5 *((RGB[i + 2] - ycbcr601[i]) / (1 - Kb)));
            ycbcr601[i + 2] = 128 + round(0.5 *((RGB[i] - ycbcr601[i]) / (1 - Kr)));
        }
    }

    void from_YCbCr601()
    {
        float Kr = 0.299;
        float Kg = 0.587;
        float Kb = 0.114;
        int Ycbcr;
        for (int i = 0; i < color * w * h; i+=3)
        {
            Ycbcr = round(ycbcr601[i] + (ycbcr601[i + 2] - 128) * (2 - 2 * Kr));
            RGB[i] = Ycbcr > 255 ? 255 : Ycbcr < 0 ? 0 : Ycbcr;
            Ycbcr = round(ycbcr601[i] - Kb / Kg * (2 - 2 * Kb) * (ycbcr601[i + 1] - 128) - Kr / Kg * (2 - 2 * Kr) * (ycbcr601[i + 2] - 128));
            RGB[i + 1] = Ycbcr > 255 ? 255 : Ycbcr < 0 ? 0 : Ycbcr;
            Ycbcr = round(ycbcr601[i] + (ycbcr601[i + 1] - 128) * (2 - 2 * Kb));
            RGB[i + 2] = Ycbcr > 255 ? 255 : Ycbcr < 0 ? 0 : Ycbcr;
        }
    }
    void to_YCbCr709()
    {
        float Kr = 0.2126;
        float Kg = 0.7152;
        float Kb = 0.0722;
        for (int i = 0; i < color * w * h; i += 3)
        {
            ycbcr709[i] = round(Kr*RGB[i] + Kg * RGB[i + 1] + Kb * RGB[i + 2]);
            ycbcr709[i + 1] = 128 + round(0.5 *((RGB[i + 2] - ycbcr709[i]) / (1 - Kb)));
            ycbcr709[i + 2] = 128 + round(0.5 *((RGB[i] - ycbcr709[i]) / (1 - Kr)));
        }
    }
    void from_YCbCr709()
    {
        float Kr = 0.2126;
        float Kg = 0.7152;
        float Kb = 0.0722;
        int Ycbcr;
        for (int i = 0; i < color * w * h; i += 3)
        {
            Ycbcr = round(ycbcr709[i] + (ycbcr709[i + 2] - 128) * (2 - 2 * Kr));
            RGB[i] = Ycbcr > 255 ? 255 : Ycbcr < 0 ? 0 : Ycbcr;
            Ycbcr = round(ycbcr709[i] - Kb / Kg * (2 - 2 * Kb) * (ycbcr709[i + 1] - 128) - Kr / Kg * (2 - 2 * Kr) * (ycbcr709[i + 2] - 128));
            RGB[i + 1] = Ycbcr > 255 ? 255 : Ycbcr < 0 ? 0 : Ycbcr;
            Ycbcr = round(ycbcr709[i] + (ycbcr709[i + 1] - 128) * (2 - 2 * Kb));
            RGB[i + 2] = Ycbcr > 255 ? 255 : Ycbcr < 0 ? 0 : Ycbcr;
        }
    }
    void to_YCoCg()
    {
        for (int i = 0; i < color * w * h; i += 3)
        {
            ycocg[i] = round(0.25 * RGB[i]+0.5 * RGB[i+1]+0.25 * RGB[i+2]);
            ycocg[i + 1] = round(128+0.5 * RGB[i] - 0.5 * RGB[i + 2]);
            ycocg[i + 2] = round(128-0.25 * RGB[i] + 0.5 * RGB[i + 1] - 0.25 * RGB[i + 2]);
        }
    }
    void from_YCoCg()
    {
        for (int i = 0; i < color * w * h; i += 3)
        {
            int dif = ycocg[i] - ycocg[i+2] + 128;
            RGB[i] = round(dif + ycocg[i+1]-128) > 255 ? 255 : round(dif + ycocg[i + 1]-128) < 0 ? 0 : round(dif + ycocg[i + 1]-128);
            RGB[i + 1] = round(ycocg[i] + ycocg[i + 2]-128) > 255 ? 255 : round(ycocg[i] + ycocg[i + 2]-128) < 0 ? 0 : round(ycocg[i] + ycocg[i + 2]-128);
            RGB[i + 2] = round(dif - ycocg[i + 1]+128) > 255 ? 255 : round(dif - ycocg[i + 1]+128) < 0 ? 0 : round(dif - ycocg[i + 1]+128);
        }
    }
    void to_CMY()
    {
        for (int i = 0; i < color* w * h; i += 3)
        {
            cmy[i] = 255 - RGB[i];
            cmy[i + 1] = 255 - RGB[i + 1];
            cmy[i + 2] = 255 - RGB[i + 2];
        }
    }
    void from_CMY()
    {
        for (int i = 0; i < color * w * h; i += 3)
        {
            RGB[i] = 255 - cmy[i];
            RGB[i + 1] = 255 - cmy[i + 1];
            RGB[i + 2] = 255 - cmy[i + 2];
        }
    }
};

int main(int argc, char *argv[])
{
    if (argc < 11)
    {
        fprintf(stderr, "Wrong number of arguments\n");
        return 1;
    }
    int from_color = -1;
    int to_color = -1;
    int num_input_file;
    int num_output_file;
    int count_in;
    int count_out;
    char space[7][10] = {"RGB", "HSL", "HSV", "YCbCr.601", "YCbCr.709", "YCoCg", "CMY"};
    for (int i = 0; i < 11; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
                case 'f':
                    for (int j = 0; j < 7; j++)
                    {
                        if (strcmp(argv[i + 1], space[j]) == 0)
                            from_color = j;
                    }
                    if (from_color == -1)
                    {
                        fprintf(stderr, "Wrong input color space\n");
                        return 1;
                    }
                    break;
                case 't':
                    for (int j = 0; j < 7; j++)
                    {
                        if (strcmp(argv[i + 1], space[j]) == 0)
                            to_color = j;
                    }
                    if (to_color == -1)
                    {
                        fprintf(stderr, "Wrong output color space\n");
                        return 1;
                    }
                    break;
                case 'i':
                    count_in = atoi(argv[i + 1]);
                    num_input_file = i + 2;
                    break;
                case 'o':
                    count_out = atoi(argv[i + 1]);
                    num_output_file = i + 2;
                    break;
            }
        }
    }
    FILE *in_file;
    FILE *in_file3;
    char file[50];
    strcpy(file, argv[num_input_file]);
    if (count_in == 1)
    {
        in_file = fopen(file, "r+b");
        if (!in_file)
        {
            fprintf(stderr, "Cannot open file\n");
            fclose(in_file);
            return 1;
        }
    }
    else
    {
        int h, w, max, color;
        char format[3];
        char symb;
        char file1[50];
        file[strlen(file) - 4] = '\0';
        sprintf(file, "%s_%d%s", file, 1, ".pgm");
        in_file = fopen(file, "rb");

        fscanf(in_file, "%2s %d %d %d%c", &format, &w, &h, &max, &symb);
        UC* img = (UC*)malloc(w*h * 3);
        UC* channel = (UC*)malloc(w*h);
        for (int i = 0; i < 3; i++)
        {
            file[strlen(file) - 6] = '\0';
            sprintf(file, "%s_%d%s", file, i + 1, ".pgm");
            in_file3 = fopen(file, "rb");
            if (!in_file3)
            {
                fprintf(stderr, "Cannot open file\n");
                fclose(in_file3);
                return 1;
            }
            fscanf(in_file3, "%2s %d %d %d%c", &format, &w, &h, &max, &symb);
            fread(channel, 1, w * h, in_file3);
            for (int k = 0; k < w * h; k++)
                img[k * 3 + i] = channel[k];
        }
        fclose(in_file3);
        in_file = fopen(argv[num_input_file], "wb");
        fprintf(in_file, "P6\n%d %d\n%d%c", w, h, max, symb);
        fwrite(img, 1, w * h * 3, in_file);
        fclose(in_file);
        in_file = fopen(argv[num_input_file], "rb");

    }
    Picture pict(in_file, (Picture::COLOR_TYPE)from_color);
    fclose(in_file);
    if (pict.RGB == NULL)
    {
        fprintf(stderr, "Not enough memory\n");
        return 1;
    }
    if (pict.format[1] != '5' && pict.format[1] != '6')
    {
        fprintf(stderr, "Wrong format of image (not P5 or P6)\n");
        return 1;
    }
    
    if (count_out == 1)
    {
        FILE *out_file = fopen(argv[num_output_file], "wb");
        fprintf(out_file, "P%c\n%d %d\n%d%c", pict.format[1], pict.w, pict.h, pict.max, pict.symb);

        switch ((Picture::COLOR_TYPE)to_color)
        {
            case Picture::RGBTYPE:
                fwrite(pict.RGB, 1, pict.w * pict.h * pict.color, out_file);
                break;
            case Picture::HSLTYPE:
                pict.to_HSL();
                fwrite(pict.HSL, 1, pict.w * pict.h * pict.color, out_file);
                break;
            case Picture::HSVTYPE:
                pict.to_HSV();
                fwrite(pict.HSV, 1, pict.w * pict.h * pict.color, out_file);
                break;
            case Picture::YCBCR601TYPE:
                pict.to_YCbCr601();
                fwrite(pict.ycbcr601, 1, pict.w * pict.h * pict.color, out_file);
                break;
            case Picture::YCBCR709TYPE:
                pict.to_YCbCr709();
                fwrite(pict.ycbcr709, 1, pict.w * pict.h * pict.color, out_file);
                break;
            case Picture::YCOCGTYPE:
                pict.to_YCoCg();
                fwrite(pict.ycocg, 1, pict.w *pict.h * pict.color, out_file);
                break;
            case Picture::CMYTYPE:
                pict.to_CMY();
                fwrite(pict.cmy, 1, pict.w * pict.h * pict.color, out_file);
                break;
        }
        fclose(out_file);
    }
    else
    {
            strcpy(file, argv[num_output_file]);
            file[strlen(file) - 4] = '\0';
            sprintf(file, "%s_%d%s", file, 1, ".pgm");
            FILE *out_file1 = fopen(file, "wb");
            file[strlen(file) - 6] = '\0';
            sprintf(file, "%s_%d%s", file, 2, ".pgm");
            FILE *out_file2 = fopen(file, "wb");
            file[strlen(file) - 6] = '\0';
            sprintf(file, "%s_%d%s", file, 3, ".pgm");
            FILE *out_file3 = fopen(file, "wb");
            fprintf(out_file1, "P5\n%d %d\n%d%c",pict.w, pict.h, pict.max, pict.symb);
            fprintf(out_file2, "P5\n%d %d\n%d%c",  pict.w, pict.h, pict.max, pict.symb);
            fprintf(out_file3, "P5\n%d %d\n%d%c", pict.w, pict.h, pict.max, pict.symb);
            UC *R = (UC *)malloc(pict.w * pict.h);
            UC *G = (UC *)malloc(pict.w * pict.h);
            UC *B = (UC *)malloc(pict.w * pict.h);
            
            switch ((Picture::COLOR_TYPE)to_color)
            {
                case Picture::RGBTYPE:
                    for (int k = 0; k < pict.w * pict.h; k++)
                    {
                        R[k] = pict.RGB[k * 3];
                        G[k] = pict.RGB[k * 3 + 1];
                        B[k] = pict.RGB[k * 3 + 2];
                    }
                    break;
                case Picture::HSLTYPE:
                    pict.to_HSL();
                    for (int k = 0; k < pict.w * pict.h; k++)
                    {
                        R[k] = pict.HSL[k * 3];
                        G[k] = pict.HSL[k * 3 + 1];
                        B[k] = pict.HSL[k * 3 + 2];
                    }
                    break;
                case Picture::HSVTYPE:
                    pict.to_HSV();
                    for (int k = 0; k < pict.w * pict.h; k++)
                    {
                        R[k] = pict.HSV[k * 3];
                        G[k] = pict.HSV[k * 3 + 1];
                        B[k] = pict.HSV[k * 3 + 2];
                    }
                    break;
                case Picture::YCBCR601TYPE:
                    pict.to_YCbCr601();
                    for (int k = 0; k < pict.w * pict.h; k++)
                    {
                        R[k] = pict.ycbcr601[k * 3];
                        G[k] = pict.ycbcr601[k * 3 + 1];
                        B[k] = pict.ycbcr601[k * 3 + 2];
                    }
                    break;
                case Picture::YCBCR709TYPE:
                    pict.to_YCbCr709();
                    for (int k = 0; k < pict.w * pict.h; k++)
                    {
                        R[k] = pict.ycbcr709[k * 3];
                        G[k] = pict.ycbcr709[k * 3 + 1];
                        B[k] = pict.ycbcr709[k * 3 + 2];
                    }
                    break;
                case Picture::YCOCGTYPE:
                    pict.to_YCoCg();
                    for (int k = 0; k < pict.w * pict.h; k++)
                    {
                        R[k] = pict.ycocg[k * 3];
                        G[k] = pict.ycocg[k * 3 + 1];
                        B[k] = pict.ycocg[k * 3 + 2];
                    }
                    break;
                case Picture::CMYTYPE:
                    pict.to_CMY();
                    for (int k = 0; k < pict.w * pict.h; k++)
                    {
                        R[k] = pict.cmy[k * 3];
                        G[k] = pict.cmy[k * 3 + 1];
                        B[k] = pict.cmy[k * 3 + 2];
                    }
                    break;
            }
            fwrite(R, 1, pict.w * pict.h, out_file1);
            fwrite(G, 1, pict.w * pict.h, out_file2);
            fwrite(B, 1, pict.w * pict.h, out_file3);
            fclose(out_file1);
            fclose(out_file2);
            fclose(out_file3);
    }
    
    if (count_in == 3)
        remove(argv[num_input_file]);
    
    return 0;
}
