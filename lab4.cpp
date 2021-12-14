#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <string>

#define UC unsigned char

class image
{
public:
    UC *img;
    int width, height, n, max;
    char type;
    
    image( char *filename )
    {
        FILE *f;
        f = fopen(filename, "rb");
        if (!f)
        {
            fprintf(stderr, "Error: No such file.\n");
            return;
        }
        char str[5], c;
        fscanf(f, "%4s%d%d%d%c", str, &width, &height, &max, &c);
        type = str[1];
        if (type == '5')
            n = 1;
        else if (type == '6')
            n = 3;
        else
        {
            n = 0;
            fprintf(stderr, "Error: Unknown file format.\n");
            return;
        }

        img = (UC *)malloc(width * height * n * sizeof(UC));
        fread(img, n, width * height, f);
        fclose(f);
    }
    void SaveImage( char *filename );
    void toYCbCr601( void );
    void fromYCbCr601( void );
    void findMinMax( int c, UC* realMin, UC* realMax, double pixIgnorPer );
    void contrastRGB( int offset, double factor );
    void contrastYCbCr( int offset, double factor );
};

void image::SaveImage( char *filename )
{
    FILE *f;
    f = fopen(filename, "wb");
    fprintf(f, "P%c\n%d %d\n255\n", type, width, height);
    fwrite(img, n, width * height, f);
    free(img);
    fclose(f);
}
void image::toYCbCr601( void )
{
    UC *y601 = (UC *)malloc(width * height * 3);

    double R, G, B;
    double Y, Cb, Cr;
    double Kr = 0.299;
    double Kb = 0.114;
    double Kg = 1 - Kr - Kb;

    for (int i = 0; i < width * height; i++)
    {
        R = img[i*3];
        G = img[i*3 + 1];
        B = img[i*3 + 2];

        Y = round(Kr * R + Kg * G + Kb * B);
        Cb = 128 + round(0.5 * (B - Y) / (1 - Kb));
        Cr = 128 + round(0.5 * (R - Y) / (1 - Kr));

        y601[i * 3] = Y > 255 ? 255 : Y < 0 ? 0 : Y;
        y601[i * 3 + 1] = Cb > 255 ? 255 : Cb < 0 ? 0 : Cb;
        y601[i * 3 + 2] = Cr > 255 ? 255 : Cr < 0 ? 0 : Cr;
    }
    img = y601;
}
void image::fromYCbCr601( void )
{
    UC* rgb = (UC*)malloc(width * height * 3);

    double Y, Cb, Cr;
    double Kr = 0.299;
    double Kb = 0.114;
    double Kg = 1 - Kr - Kb;
    double R, G, B;

    for (int i = 0; i < width * height; i++)
    {
        Y = img[i * 3];
        Cb = img[i * 3 + 1];
        Cr = img[i * 3 + 2];

        R = Y + 2 * (1 - Kr) * (Cr - 128);
        B = Y + 2 * (1 - Kb) * (Cb - 128);
        G = (Y - Kb * B - Kr * R) / Kg;

        rgb[i * 3] = R > 255 ? 255 : R < 0 ? 0 : round(R);
        rgb[i * 3 + 1] = G > 255 ? 255 : G < 0 ? 0: round(G);
        rgb[i * 3 + 2] = B > 255 ? 255 : B < 0 ? 0 : round(B);
    }
    img = rgb;
}
void image::findMinMax( int c, UC *realMin, UC *realMax, double pixIgnorPer = 0.0 )
{
    if (pixIgnorPer == 0)
    {
        *realMin = 255;
        *realMax = 0;
        for (int i = 0; i < width*height*n; i+=c)
        {
            if (img[i] > *realMax)
                *realMax = img[i];
            if (img[i] < *realMin)
                *realMin = img[i];
        }
    }
    else
    {
        int edgemin = round(width * height / 100 * pixIgnorPer);
        int edgemax = edgemin;
        int hyst[256];
        for (int i = 0; i < 256; i++)
            hyst[i] = 0;
        for (int i = 0; i < width * height * n; i += c)
            hyst[img[i]] += 1;

        for (int i = 0; i < 256; i++)
        {
            if (hyst[i] <= edgemin)
            {
                edgemin -= hyst[i];
            }
            else
            {
                *realMin = i;
                break;
            }
        }
        for (int i = 255; i >= 0; i--)
        {
            if (hyst[i] <= edgemax)
            {
                edgemax -= hyst[i];
            }
            else
            {
                *realMax = i;
                break;
            }
        }
    }

}
void image::contrastRGB( int offset, double factor )
{
    int newPix;
    for (int i = 0; i < width * height * n; i++)
    {
        newPix = round((img[i] - offset)*factor);
        img[i] = newPix > 255 ? 255 : newPix < 0 ? 0 : newPix;
    }
}
void image::contrastYCbCr( int offset, double factor )
{
    if (n == 3)
        toYCbCr601();
    int newPix;
    for (int i = 0; i < width * height * n; i += n)
    {
        newPix = round((img[i] - offset)*factor);
        img[i] = newPix > 255 ? 255 : newPix < 0 ? 0 : newPix;
    }
    if (n == 3)
        fromYCbCr601();
}

int main( int argc, char** argv )
{
    UC transform;
    int offset = 0;
    double factor = 0.0;

    if (argc < 4)
    {
        fprintf(stderr, "Error: Invalid number of commands.\n");
        return 1;
    }
    transform = atoi(argv[3]);
    if (transform < 2 && argc < 6)
    {
        fprintf(stderr, "Transform %d requires explicit indication of offset and factor.\n", transform);
        return 1;
    }
    if (transform < 2)
    {
        offset = atoi(argv[4]);
        factor = atof(argv[5]);
    }
    
    image Img(argv[1]);
    
    UC Min;
    UC Max;
    
    switch (transform)
    {
    case 0:
        Img.contrastRGB(offset, factor);
        break;
    case 1:
        Img.contrastYCbCr(offset, factor); 
        break;
    case 2:
        Img.findMinMax(1, &Min, &Max);
        offset = Min;
        factor = 255.0 / (double)(Max - Min);
        printf("%i %f\n", offset, factor);
        Img.contrastRGB(offset, factor);
        break;
    case 3:
        Img.findMinMax(Img.n, &Min, &Max);
        offset = Min;
        factor = 255.0 / (double)(Max - Min);
        printf("%i %f\n", offset, factor);
        Img.contrastYCbCr(offset, factor);
        break;
    case 4:
        Img.findMinMax(1, &Min, &Max, 0.39);
        offset = Min;
        factor = 255.0 / (double)(Max - Min);
        printf("%i %f\n", offset, factor);
        Img.contrastRGB(offset, factor);
        break;
    case 5:
        Img.findMinMax(Img.n, &Min, &Max, 0.39);
        offset = Min;
        factor = 255.0 / (double)(Max - Min);
        printf("%i %f\n", offset, factor);
        Img.contrastYCbCr(offset, factor);
        break;
    default:
        fprintf(stderr, "Unknown command %d.\n", transform);
        return 1;
    }
    Img.SaveImage(argv[2]);
    return 0;
}
