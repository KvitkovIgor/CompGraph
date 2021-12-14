#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define UC unsigned char

class image
{
public:
    UC nChannels;
    uint32_t width, height;
    double *Y, *R, *G, *B;
    double maxY, sharpnessKnob;
    UC* pix;

    image(UC n, uint32_t w, uint32_t h, UC* img, int m, double sK)
    {
        nChannels = n;
        width = w;
        height = h;
        maxY = (double)m;
        sharpnessKnob = sK;
        pix = (UC*)malloc(width*height * nChannels);
        if (nChannels == 1)
        {
            Y = (double*)malloc(width*height * sizeof (double));
            for (uint32_t i = 0; i < width*height; i++)
                Y[i] = (double)img[i]/maxY;
        }
        else
        {
            R = (double*)malloc(width*height * sizeof (double));
            G = (double*)malloc(width*height * sizeof (double));
            B = (double*)malloc(width*height * sizeof (double));
            for (uint32_t i = 0; i < width*height; i++)
            {
                R[i] = (double)img[i*nChannels]/maxY;
                G[i] = (double)img[i*nChannels+1]/maxY;
                B[i] = (double)img[i*nChannels+2]/maxY;
            }
        }
    }

    void imagefree()
    {
        free(pix);
        if (nChannels == 1)
        {
            free(Y);
        }
        else
        {
            free(R);
            free(G);
            free(B);
        }
    }

    UC * buildOutPixArray( void )
    {
        UC * pix = (UC *)malloc(width*height*nChannels * sizeof(UC));
        if (nChannels == 1)
        {
            for (uint32_t i = 0; i < width*height; i++)
            {
                pix[i] = round(Y[i] * maxY);
            }
        }
        else
        {
            for (uint32_t i = 0; i < width*height; i++)
            {
                pix[i*nChannels] = round(R[i] * maxY);
                pix[i*nChannels + 1] = round(G[i] * maxY);
                pix[i*nChannels + 2] = round(B[i] * maxY);
            }
        }
        return pix;
    }
    void CAS()
    {
        if (nChannels == 1)
            CAS_P5();
        else
            CAS_P6();
    }
    void CAS_P5()
    {
        double mn, mn2;
        double mx, mx2;
        double d_mn, d_mx;
        double A;
        double developerMax;
        double a,b;
        double W;
        for (uint32_t i = 0; i < height; i++)
        {
            for (uint32_t j = 0; j < width; j++)
            {
                mn = 0;
                mx = 0;
                double min = Y[i*width+j];
                double max = Y[i*width+j];
                if (j>0)
                {
                    if (Y[i*width+j-1] < min)
                        min = Y[i*width+j-1];
                    if (Y[i*width+j-1] > max)
                        max = Y[i*width+j-1];
                }
                if (j+1<width)
                {
                    if (Y[i*width+j+1] < min)
                        min = Y[i*width+j+1];
                    if (Y[i*width+j+1] > max)
                        max = Y[i*width+j+1];
                }
                if (i>0)
                {
                    if (Y[(i-1)*width+j] < min)
                        min = Y[(i-1)*width+j];
                    if (Y[(i-1)*width+j] > max)
                        max = Y[(i-1)*width+j];
                }
                if (i+1<height)
                {
                    if (Y[(i+1)*width+j] < min)
                        min = Y[(i+1)*width+j];
                    if (Y[(i+1)*width+j] > max)
                        max = Y[(i+1)*width+j];
                }
                mn = min;
                mx = max;
                
                mn2 = mn;
                mx2 = mx;
                double min2 = mn2;
                double max2 = mx2;
                if (j > 0 && i > 0)
                {
                    if (Y[(i-1)*width+j-1] < min2)
                        min2 = Y[(i-1)*width+j-1];
                    if (Y[(i-1)*width+j-1] > max2)
                        max2 = Y[(i-1)*width+j-1];
                }
                if (j > 0 && i+1<height)
                {
                    if (Y[(i+1)*width+j-1] < min2)
                        min2 = Y[(i+1)*width+j-1];
                    if (Y[(i+1)*width+j-1] > max2)
                        max2 = Y[(i+1)*width+j-1];
                }
                if (j+1 < width && i > 0)
                {
                    if (Y[(i-1)*width+j+1] < min2)
                        min2 = Y[(i-1)*width+j+1];
                    if (Y[(i-1)*width+j+1] > max2)
                        max2 = Y[(i-1)*width+j+1];
                }
                if (j+1 < width && i+1 < height)
                {
                    if (Y[(i+1)*width+j+1] < min2)
                            min2 = Y[(i+1)*width+j+1];
                    if (Y[(i+1)*width+j+1] > max2)
                        max2 = Y[(i+1)*width+j+1];
                }
                mn2 = min2;
                mx2 = max2;
                
                mn = mn*0.5 + mn2*0.5;
                mx = mx*0.5 + mx2*0.5;
                d_mn = mn;
                d_mx = 2.0 - mx;
                A = d_mn < d_mx ? d_mn/mx : d_mx/mx;
                A = sqrt(A);
                a = -0.125;
                b = -0.2;
                developerMax = a + sharpnessKnob*(b - a);
                W = developerMax*A;
                double sum = Y[i*width+j];
                uint8_t nNeighPix = 0;
                if (j>0)
                {
                    sum += W*Y[i*width+j-1];
                    nNeighPix++;
                    
                }
                if (j+1<width)
                {
                    sum += W*Y[i*width+j+1];
                    nNeighPix++;
                }
                if (i>0)
                {
                    sum += W*Y[(i-1)*width+j];
                    nNeighPix++;
                }
                if (i+1<height)
                {
                    sum += W*Y[(i+1)*width+j];
                    nNeighPix++;
                }
                sum = sum / (W*nNeighPix + 1);
                sum = sum < 0 ? 0 : sum > 1 ? 1 : sum;
                if (nChannels == 1)
                    pix[i*width+j] = round(sum * maxY);
                else
                    pix[(i*width+j)*nChannels] = round(sum * maxY);
            }
        }
    }
    void CAS_P6()
    {
        double mn, mn2;
        double mx, mx2;
        double d_mn, d_mx;
        double A;
        double developerMax;
        double a,b;
        double W;
        for (uint32_t iComponent = 0; iComponent < nChannels; iComponent++)
        {
            for (uint32_t i = 0; i < height; i++)
            {
                for (uint32_t j = 0; j < width; j++)
                {
                    mn = 0;
                    mx = 0;

                    if (iComponent == 0)
                    {
                        double min = R[i*width+j];
                        double max = R[i*width+j];
                        if (j>0)
                        {
                            if (R[i*width+j-1] < min)
                                min = R[i*width+j-1];
                            if (R[i*width+j-1] > max)
                                max = R[i*width+j-1];
                        }
                        if (j+1<width)
                        {
                            if (R[i*width+j+1] < min)
                                min = R[i*width+j+1];
                            if (R[i*width+j+1] > max)
                                max = R[i*width+j+1];
                        }
                        if (i>0)
                        {
                            if (R[(i-1)*width+j] < min)
                                min = R[(i-1)*width+j];
                            if (R[(i-1)*width+j] > max)
                                max = R[(i-1)*width+j];
                        }
                        if (i+1<height)
                        {
                            if (R[(i+1)*width+j] < min)
                                min = R[(i+1)*width+j];
                            if (R[(i+1)*width+j] > max)
                                max = R[(i+1)*width+j];
                        }
                        mn = min;
                        mx = max;
                    }
                    else if (iComponent == 1)
                    {
                        double min = G[i*width+j];
                        double max = G[i*width+j];
                        if (j>0)
                        {
                            if (G[i*width+j-1] < min)
                                min = G[i*width+j-1];
                            if (G[i*width+j-1] > max)
                                max = G[i*width+j-1];
                        }
                        if (j+1<width)
                        {
                            if (G[i*width+j+1] < min)
                                min = G[i*width+j+1];
                            if (G[i*width+j+1] > max)
                                max = G[i*width+j+1];
                        }
                        if (i>0)
                        {
                            if (G[(i-1)*width+j] < min)
                                min = G[(i-1)*width+j];
                            if (G[(i-1)*width+j] > max)
                                max = G[(i-1)*width+j];
                        }
                        if (i+1<height)
                        {
                            if (G[(i+1)*width+j] < min)
                                min = G[(i+1)*width+j];
                            if (G[(i+1)*width+j] > max)
                                max = G[(i+1)*width+j];
                        }
                        mn = min;
                        mx = max;
                    }
                    else
                    {
                        double min = B[i*width+j];
                        double max = B[i*width+j];
                        if (j>0)
                        {
                            if (B[i*width+j-1] < min)
                                min = B[i*width+j-1];
                            if (B[i*width+j-1] > max)
                                max = B[i*width+j-1];
                        }
                        if (j+1<width)
                        {
                            if (B[i*width+j+1] < min)
                                min = B[i*width+j+1];
                            if (B[i*width+j+1] > max)
                                max = B[i*width+j+1];
                        }
                        if (i>0)
                        {
                            if (B[(i-1)*width+j] < min)
                                min = B[(i-1)*width+j];
                            if (B[(i-1)*width+j] > max)
                                max = B[(i-1)*width+j];
                        }
                        if (i+1<height)
                        {
                            if (B[(i+1)*width+j] < min)
                                min = B[(i+1)*width+j];
                            if (B[(i+1)*width+j] > max)
                                max = B[(i+1)*width+j];
                        }
                        mn = min;
                        mx = max;
                    }

                    mn2 = mn;
                    mx2 = mx;

                    if (iComponent == 0)
                    {
                        double min2 = mn2;
                        double max2 = mx2;
                        if (j > 0 && i > 0)
                        {
                            if (R[(i-1)*width+j-1] < min2)
                                min2 = R[(i-1)*width+j-1];
                            if (R[(i-1)*width+j-1] > max2)
                                max2 = R[(i-1)*width+j-1];
                        }
                        if (j > 0 && i+1<height)
                        {
                            if (R[(i+1)*width+j-1] < min2)
                                min2 = R[(i+1)*width+j-1];
                            if (R[(i+1)*width+j-1] > max2)
                                max2 = R[(i+1)*width+j-1];
                        }
                        if (j+1 < width && i > 0)
                        {
                            if (R[(i-1)*width+j+1] < min2)
                                min2 = R[(i-1)*width+j+1];
                            if (R[(i-1)*width+j+1] > max2)
                                max2 = R[(i-1)*width+j+1];
                        }
                        if (j+1 < width && i+1 < height)
                        {
                            if (R[(i+1)*width+j+1] < min2)
                                    min2 = R[(i+1)*width+j+1];
                            if (R[(i+1)*width+j+1] > max2)
                                max2 = R[(i+1)*width+j+1];
                        }
                        mn2 = min2;
                        mx2 = max2;
                    }
                    else if (iComponent == 1)
                    {
                        double min2 = mn2;
                        double max2 = mx2;
                        if (j > 0 && i > 0)
                        {
                            if (G[(i-1)*width+j-1] < min2)
                                min2 = G[(i-1)*width+j-1];
                            if (G[(i-1)*width+j-1] > max2)
                                max2 = G[(i-1)*width+j-1];
                        }
                        if (j > 0 && i+1<height)
                        {
                            if (G[(i+1)*width+j-1] < min2)
                                min2 = G[(i+1)*width+j-1];
                            if (G[(i+1)*width+j-1] > max2)
                                max2 = G[(i+1)*width+j-1];
                        }
                        if (j+1 < width && i > 0)
                        {
                            if (G[(i-1)*width+j+1] < min2)
                                min2 = G[(i-1)*width+j+1];
                            if (G[(i-1)*width+j+1] > max2)
                                max2 = G[(i-1)*width+j+1];
                        }
                        if (j+1 < width && i+1 < height)
                        {
                            if (G[(i+1)*width+j+1] < min2)
                                min2 = G[(i+1)*width+j+1];
                            if (G[(i+1)*width+j+1] > max2)
                                max2 = G[(i+1)*width+j+1];
                        }
                        mn2 = min2;
                        mx2 = max2;
                    }
                    else
                    {
                        double min2 = mn2;
                        double max2 = mx2;
                        if (j > 0 && i > 0)
                        {
                            if (B[(i-1)*width+j-1] < min2)
                                min2 = B[(i-1)*width+j-1];
                            if (B[(i-1)*width+j-1] > max2)
                                max2 = B[(i-1)*width+j-1];
                        }
                        if (j > 0 && i+1<height)
                        {
                            if (B[(i+1)*width+j-1] < min2)
                                min2 = B[(i+1)*width+j-1];
                            if (B[(i+1)*width+j-1] > max2)
                                max2 = B[(i+1)*width+j-1];
                        }
                        if (j+1 < width && i > 0)
                        {
                            if (B[(i-1)*width+j+1] < min2)
                                min2 = B[(i-1)*width+j+1];
                            if (B[(i-1)*width+j+1] > max2)
                                max2 = B[(i-1)*width+j+1];
                        }
                        if (j+1 < width && i+1 < height)
                        {
                            if (B[(i+1)*width+j+1] < min2)
                                min2 = B[(i+1)*width+j+1];
                            if (B[(i+1)*width+j+1] > max2)
                                max2 = B[(i+1)*width+j+1];
                        }
                        mn2 = min2;
                        mx2 = max2;
                    }
                    
                    mn = mn*0.5 + mn2*0.5;
                    mx = mx*0.5 + mx2*0.5;
                    d_mn = mn;
                    d_mx = 2.0 - mx;
                    A = d_mn < d_mx ? d_mn/mx : d_mx/mx;
                    A = sqrt(A);
                    a = -0.125;
                    b = -0.2;
                    developerMax = a + sharpnessKnob * (b - a);
                    W = developerMax*A;

                    if (iComponent == 0)
                    {
                        double sum = R[i*width+j];
                        uint8_t nNeighPix = 0;
                        if (j>0)
                        {
                            sum += W*R[i*width+j-1];
                            nNeighPix++;
                        }
                        if (j+1<width)
                        {
                            sum += W*R[i*width+j+1];
                            nNeighPix++;
                        }
                        if (i>0)
                        {
                            sum += W*R[(i-1)*width+j];
                            nNeighPix++;
                        }
                        if (i+1<height)
                        {
                            sum += W*R[(i+1)*width+j];
                            nNeighPix++;
                        }
                        sum = sum / (W*nNeighPix + 1);
                        sum = sum < 0 ? 0 : sum > 1 ? 1 : sum;

                        if (nChannels == 1)
                            pix[i*width+j] = round(sum * maxY);
                        else
                            pix[(i*width+j)*nChannels] = round(sum * maxY);
                    }
                    else if (iComponent == 1)
                    {
                        double sum = G[i*width+j];
                        uint8_t nNeighPix = 0;
                        if (j>0)
                        {
                            sum += W*G[i*width+j-1];
                            nNeighPix++;
                        }
                        if (j+1<width)
                        {
                            sum += W*G[i*width+j+1];
                            nNeighPix++;
                        }
                        if (i>0)
                        {
                            sum += W*G[(i-1)*width+j];
                            nNeighPix++;
                        }
                        if (i+1<height)
                        {
                            sum += W*G[(i+1)*width+j];
                            nNeighPix++;
                        }
                        sum = sum / (W*nNeighPix + 1);
                        sum = sum < 0 ? 0 : sum > 1 ? 1 : sum;

                        if (nChannels == 1)
                            pix[i*width+j] = round(sum * maxY);
                        else
                            pix[(i*width+j)*nChannels + 1] = round(sum * maxY);
                    }
                    else
                    {
                        double sum = B[i*width+j];
                        uint8_t nNeighPix = 0;
                        if (j>0)
                        {
                            sum += W*B[i*width+j-1];
                            nNeighPix++;
                        }
                        if (j+1<width)
                        {
                            sum += W*B[i*width+j+1];
                            nNeighPix++;
                        }
                        if (i>0)
                        {
                            sum += W*B[(i-1)*width+j];
                            nNeighPix++;
                        }
                        if (i+1<height)
                        {
                            sum += W*B[(i+1)*width+j];
                            nNeighPix++;
                        }
                        sum = sum / (W*nNeighPix + 1);
                        sum = sum < 0 ? 0 : sum > 1 ? 1 : sum;

                        if (nChannels == 1)
                            pix[i*width+j] = round(sum * maxY);
                        else
                            pix[(i*width+j)*nChannels + 2] = round(sum * maxY);
                    }
                }
            }
        }
    }
};

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        fprintf(stderr, "Error: Invalid number of command line arguments.\n");
        return 1;
    }
    FILE *fi, *fo;
    fi = fopen(argv[1], "rb");
    if (!fi)
    {
        fprintf(stderr, "Error: No such file.\n");
        return 1;
    }

    uint32_t width, height;
    int max;
    UC n;
    char str[5];
    char c;
    fscanf(fi, "%4s%d%d%d%c", str, &width, &height, &max, &c);
    char f = str[1];
    //PGM
    if (f == '5')
        n = 1;
    //PPM
    else if (f == '6')
        n = 3;
    else
    {
        n = 0;
        fprintf(stderr, "Error: Unknown file format.\n");
        return 1;
    }

    UC *img_name = (UC *)malloc(width*height * n * sizeof(UC));
    fread(img_name, n, width * height, fi);
    fclose(fi);

    double sK = atof(argv[3]); //Sharpness Knob
    if (sK < 0 || sK > 1)
    {
        fprintf(stderr, "Error: Invalid value of sharpen %f. Must be from 0.0 to 1.0\n", sK);
        return 1;
    }
    image img(n, width, height, img_name, max, sK);
    img.CAS();

    fo = fopen(argv[2], "wb");
    fprintf(fo, "P%c\n%d %d\n255\n", f, width, height);
    fwrite(img.pix, img.nChannels, img.width * img.height, fo);
    fclose(fo);
    img.imagefree();

    return 0;
}
