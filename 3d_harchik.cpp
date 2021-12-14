#include <iostream>
#include <cmath>


// max picture size
#define H_P 4000
#define W_P 4000

// Main picture
unsigned char PIC[H_P][W_P][3];
unsigned char PIC1[H_P][W_P][3];

// Picture's variables
int Ptype = -1, w, h, grad;
int Ptype1 = -1, w1, h1, grad1;

// Gradient
int gradient1;

int    bitn1, // Bitness of disering
       size1; // Line size

// Picture type enum
enum Ptype{
    P5,
    P6
} PId, PId1;

// Save pnm image to file function.
// * ARGUMENTS:
//   - file structure:
//     FILE *F1.
// * RETURNS:
//   - None.
void SavePNM ( FILE *F1 )
{
    int i, j, k;
    
    if (PId == P5)
    {
        fprintf(F1, "P5\n");
        fprintf(F1, "%i %i\n", w, h);
        fprintf(F1, "%i\n", grad);
        for (i = 0; i < h; i++)
        {
            for (j = 0; j < w; j++)
            {
                fprintf(F1, "%c", PIC[i][j][0]);
            }
        }
    }
    else if (PId == P6)
    {
         fprintf(F1, "P6\n");
         fprintf(F1, "%i %i\n", w, h);
         fprintf(F1, "%i\n", grad);
         for (i = 0; i < h; i++)
         {
             for (j = 0; j < w; j++)
                 for (k = 0; k < 3; k++)
                 {
                     fprintf(F1, "%c", PIC[i][j][k]);
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
int LoadPNM ( FILE *F1 )
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
        fscanf(F1, "%i %i", &w, &h);
        if (w > W_P || h > H_P)
        {
            printf("Too big picture!\n");
            return 0;
        }
        fscanf(F1, "%i", &grad);
        if (grad > 255 || grad < 0)
        {
            printf("Gradaction is out of range!\n");
            return 0;
        }
        fread(&b, 1, 1, F1);
        for (i = 0; i < h; i++)
            for (j = 0; j < w; j++)
            {
                if (fread(&b, 1, 1, F1) > 0)
                    for (k = 0; k < 3; k++)
                      PIC[i][j][k] = b;
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
         fscanf(F1, "%i %i", &w, &h);
         if (w > H_P || h > W_P)
         {
             printf("Too big picture!\n");
             return 0;
         }
         fscanf(F1, "%i", &grad);
         if (grad > 255 || grad < 0)
         {
             printf("Gradaction is out of range!\n");
             return 0;
         }
         fread(&b, 1, 1, F1);
         for (i = 0; i < h; i++)
             for (j = 0; j < w; j++)
                 for (k = 0; k < 3; k++)
                 {
                     if (fread(&b, 1, 1, F1) > 0)
                         PIC[i][j][k] = b;
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

// Save pnm image to file function.
// * ARGUMENTS:
//   - file structure:
//     FILE *F1.
// * RETURNS:
//   - None.
void SavePNM1 ( FILE *F1 )
{
    int i, j, k;
    
    if (PId1 == P5)
    {
        fprintf(F1, "P5\n");
        fprintf(F1, "%i %i\n", w1, h1);
        fprintf(F1, "%i\n", grad1);
        for (i = 0; i < h1; i++)
        {
            for (j = 0; j < w1; j++)
            {
                fprintf(F1, "%c", PIC1[i][j][0]);
            }
        }
    }
    else if (PId1 == P6)
    {
         fprintf(F1, "P6\n");
         fprintf(F1, "%i %i\n", w1, h1);
         fprintf(F1, "%i\n", grad1);
         for (i = 0; i < h1; i++)
         {
             for (j = 0; j < w1; j++)
                 for (k = 0; k < 3; k++)
                 {
                     fprintf(F1, "%c", PIC1[i][j][k]);
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
int LoadPNM1 ( FILE *F1 )
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
        PId1 = P5;
        fscanf(F1, "%i %i", &w1, &h1);
        if (w1 > W_P || h1 > H_P)
        {
            printf("Too big picture!\n");
            return 0;
        }
        fscanf(F1, "%i", &grad1);
        if (grad1 > 255 || grad1 < 0)
        {
            printf("Gradaction is out of range!\n");
            return 0;
        }
        fread(&b, 1, 1, F1);
        for (i = 0; i < h1; i++)
            for (j = 0; j < w1; j++)
            {
                if (fread(&b, 1, 1, F1) > 0)
                    for (k = 0; k < 3; k++)
                      PIC1[i][j][k] = b;
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
         PId1 = P6;
         fscanf(F1, "%i %i", &w1, &h1);
         if (w1 > H_P || h1 > W_P)
         {
             printf("Too big picture!\n");
             return 0;
         }
         fscanf(F1, "%i", &grad1);
         if (grad1 > 255 || grad1 < 0)
         {
             printf("Gradaction is out of range!\n");
             return 0;
         }
         fread(&b, 1, 1, F1);
         for (i = 0; i < h1; i++)
             for (j = 0; j < w1; j++)
                 for (k = 0; k < 3; k++)
                 {
                     if (fread(&b, 1, 1, F1) > 0)
                         PIC1[i][j][k] = b;
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

// Converting string to number function.
// * ARGUMENTS:
//   - point coordinates:
//     int x, int y.
// * RETURNS:
//   - (int) point in array - 1 or no - 0.
int inRage( int x, int y )
{
    return x < w && x >=0 && y >= 0 && y < h;
} // End of 'inRage' function

/* Forward Haar wavelet transform: */
void ForwardHaar1D(double* data, int length)
{
    const double inv_sqrt2 = 1/sqrt((double)2.0);

    double norm = 1.0f/sqrt((double)length);

    for(int i=0; i < length; i++) {
        data[i] *= norm;
    }
    double tmp[4000];
    while(length > 1) {
        length /= 2;

        for(int i=0; i < length; i++) {
            tmp[i] = (data[2*i] + data[2*i+1]) * inv_sqrt2;
            tmp[length + i] = (data[2*i] - data[2*i+1]) * inv_sqrt2;
        }

        memcpy(data, tmp, length*2*sizeof(double));
    }
}

   /* Transpose matrix: */
void Transpose(double *data, int width, int height)
{
    double *B = new double[width*height];

    for(int y=0; y < height; y++) {
        for(int x=0; x < width; x++) {
            B[x*height + y] = data[y*width + x];
        }
    }

    memcpy(data, B, sizeof(double)*width*height);

    delete [] B;
}

/* Forward 2d Haar wavelet transform: */
void ForwardHaar2D(double* data, int width, int height)
{
    for(int i=0; i < height; i++)
        ForwardHaar1D(&data[i*width], width);

    Transpose(data, width, height);

    for(int i=0; i < width; i++)
        ForwardHaar1D(&data[i*height], height);
    
    Transpose(data, height, width);
}

/* Inverse 1d Haar transform */
void InverseHaar1D(double* data, int length)
{
    const double inv_sqrt2 = 1/sqrt((double)2.0);
    double inv_norm = sqrt((double)length);

    int k = 1;

    double tmp[4000];
    while(k < length)  {
        for(int i=0; i < k; i++) {
            tmp[2 * i] = (data[i] + data[k + i]) * inv_sqrt2;
            tmp[2 * i + 1] = (data[i] - data[k + i]) * inv_sqrt2;
        }

        memcpy(data, tmp, sizeof(double)*(k*2));

        k *= 2;
    }
    for(int i=0; i < length; i++) {
        data[i] *= inv_norm;
    }
    
}

   /* Inverse 2d Haar wavelet transform */
void InverseHaar2D(double* data, int width, int height)
{
    for(int i=0; i < height; i++) {
        InverseHaar1D(&data[i*width], width);
    }

    Transpose(data, width, height);
    for(int i=0; i < width; i++) {
        InverseHaar1D(&data[i*height], height);
    }

    Transpose(data, height, width);
}

/* Image denoising by soft-thresholding */
void WaveletThresholdDenoising(int width, int height, double* src, double* des, double threshold)
{
    int x, y;

    /* Forward 2d Haar transform */
    ForwardHaar2D(src, width, height);

    /* soft-thresholding */
    for(y=0; y < height; y++)
    {
        for(x=0; x < width; x++)
        {
            if (src[y*width+x] > threshold)
                src[y*width+x] = src[y*width+x] - threshold;

            else if (src[y*width+x] < -threshold)
                src[y*width+x] = src[y*width+x] + threshold;
            else
                src[y*width+x] = 0;
        }
    }
    /* Inverse 2D Haar transform */
    InverseHaar2D(src, width, height);

    for (y=0; y<height; y++)
    {
        for (x=0; x<width; x++)
        {
            double temp =  src[y*width+x];
            if (temp < 0) temp = 0;
            else if (temp >255) temp = 255;
            else
               des[y*width+x] = (unsigned char)temp;
        }
    }
}

int main(int argc, const char * argv[]) {
    FILE *F1, *F2, *F3, *F4;
    double *src, *dtr;
    double *srcg, *dtrg, *srcr, *dtrr, *srcb, *dtrb;
    double *src1, *dtr1;
    double *srcg1, *dtrg1, *srcr1, *dtrr1, *srcb1, *dtrb1;
    double *src2, *dtr2;
    double *srcg2, *dtrg2, *srcr2, *dtrr2, *srcb2, *dtrb2;
    
    F1 = fopen("a.pnm", "rb");
    F2 = fopen("out2da.pgm", "rb");
    F3 = fopen("out2daa.pgm", "wb");
    F4 = fopen("a.pgm", "wb");
    
    LoadPNM(F1);
    if (PId == P5)
    {
        dtr = new double[w * h];
        src = new double[w * h];
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                src[w * i + j] = PIC[i][j][0];
    }
    else
    {
        dtrr = new double[w * h];
        srcr = new double[w * h];
        dtrb = new double[w * h];
        srcb = new double[w * h];
        dtrg = new double[w * h];
        srcg = new double[w * h];
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                {
                    srcr[w * i + j] = PIC[i][j][0];
                    srcg[w * i + j] = PIC[i][j][1];
                    srcb[w * i + j] = PIC[i][j][2];
                }
    }
    LoadPNM1(F2);
    if (PId1 == P5)
    {
        dtr1 = new double[w1 * h1];
        src1 = new double[w1 * h1];
        for (int i = 0; i < h1; i++)
            for (int j = 0; j < w1; j++)
                src1[w1 * i + j] = PIC1[i][j][0];
    }
    else
    {
        dtrr1 = new double[w1 * h1];
        srcr1 = new double[w1 * h1];
        dtrb1 = new double[w1 * h1];
        srcb1 = new double[w1 * h1];
        dtrg1 = new double[w1 * h1];
        srcg1 = new double[w1 * h1];
        for (int i = 0; i < h1; i++)
            for (int j = 0; j < w1; j++)
                {
                    srcr1[w1 * i + j] = PIC1[i][j][0];
                    srcg1[w1 * i + j] = PIC1[i][j][1];
                    srcb1[w1 * i + j] = PIC1[i][j][2];
                }
    }
    
    dtr2 = new double[w1 * 2 * h1];
    src2 = new double[w1 * 2 * h1];
    dtrr2 = new double[w1 * 2 * h1];
    srcr2 = new double[w1 * 2 * h1];
    dtrb2 = new double[w1 * 2 * h1];
    srcb2 = new double[w1 * 2 * h1];
    dtrg2 = new double[w1 * 2 * h1];
    srcg2 = new double[w1 * 2 * h1];
    
    if (PId == P5)
    {
        if (PId1 == P5)
        {
            for (int i = 0; i < h; i++)
            {
                for (int j = 0; j < w; j++)
                    src2[w1 * 2 * i + j] = src[w * i + j];
                for (int j = w; j < 2 * w; j++)
                    src2[w1 * 2 * i + j] = src1[w * i + j - w];
            }
        }
    }
    else
    {
        if (PId1 == P6)
        {
            for (int i = 0; i < h; i++)
            {
                for (int j = 0; j < w; j++)
                {
                    srcr2[w1 * 2 * i + j] = srcr[w1 * i + j];
                    srcg2[w1 * 2 * i + j] = srcg[w1 * i + j];
                    srcb2[w1 * 2 * i + j] = srcb[w1 * i + j];
                }
                for (int j = w; j < 2 * w; j++)
                {
                    srcr2[w1 * 2 * i + j] = srcr1[w1 * i + j - w];
                    srcg2[w1 * 2 * i + j] = srcg1[w1 * i + j - w];
                    srcb2[w1 * 2 * i + j] = srcb1[w1 * i + j - w];
                }
            }
        }
    }

        
    
    if (PId == P5)
    {
        WaveletThresholdDenoising(w * 2, h, src2, dtr2, 0.01);
    }
    else if (PId == P6)
    {
        WaveletThresholdDenoising(w * 2, h, srcr2, dtrr2, 0.01);
        WaveletThresholdDenoising(w * 2, h, srcg2, dtrg2, 0.01);
        WaveletThresholdDenoising(w * 2, h, srcb2, dtrb2, 0.01);
    }
    
    if (PId == P5)
    {
        if (PId1 == P5)
        {
            for (int i = 0; i < h; i++)
            {
                for (int j = 0; j < w; j++)
                    dtr[w1 * i + j] = dtr2[w1 * 2 * i + j];
                for (int j = w; j < 2 * w; j++)
                    dtr1[w1 * i + j - w] = dtr2[w1 * 2 * i + j];
            }
        }
    }
    else
    {
        if (PId1 == P6)
        {
            for (int i = 0; i < h; i++)
            {
                for (int j = 0; j < w; j++)
                {
                    dtrr[w1 * i + j] = dtrr2[w1 * 2 * i + j];
                    dtrg[w1 * i + j] = dtrg2[w1 * 2 * i + j];
                    dtrb[w1 * i + j] = dtrb2[w1 * 2 * i + j];
                }
                for (int j = w; j < 2 * w; j++)
                {
                    dtrr1[w1 * i + j - w] = dtrr2[w1 * 2 * i + j];
                    dtrg1[w1 * i + j - w] = dtrg2[w1 * 2 * i + j];
                    dtrb1[w1 * i + j - w] = dtrb2[w1 * 2 * i + j];
                }
            }
        }
    }

    
    if (PId == P5)
    {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                PIC[i][j][0] = (unsigned char)dtr[w * i + j];
    }
    else
    {
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                {
                    PIC[i][j][0] = (unsigned char)dtrr[w * i + j];
                    PIC[i][j][1] = (unsigned char)dtrg[w * i + j];
                    PIC[i][j][2] = (unsigned char)dtrb[w * i + j];
                }
    }
    if (PId1 == P5)
    {
        for (int i = 0; i < h1; i++)
            for (int j = 0; j < w1; j++)
                PIC1[i][j][0] = (unsigned char)dtr1[w1 * i + j];
    }
    else
    {
        for (int i = 0; i < h1; i++)
            for (int j = 0; j < w1; j++)
                {
                    PIC1[i][j][0] = (unsigned char)dtrr1[w1 * i + j];
                    PIC1[i][j][1] = (unsigned char)dtrg1[w1 * i + j];
                    PIC1[i][j][2] = (unsigned char)dtrb1[w1 * i + j];
                }
    }

    SavePNM(F3);
    SavePNM1(F4);
    
    std::cout << "Hello, World!\n";
    return 0;
}
