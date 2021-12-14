#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// max picture size
#define H_P 4000
#define W_P 4000

// Main picture
unsigned char PIC[H_P][W_P][3];

// Picture's variables
int Ptype = -1, w, h, grad;

// Line size
double size;

// Picture type enum
enum Ptype{
    P5,
    P6
} PId;

// integer number part
int IPart(float x)
{
    return (int)x;
}

// fractional number part
float FPart(float x)
{
    while (x >= 0)
        x--;
    x++;
    return x;
}

// Put new pixel function for drawing lines by Wu algorithm function.
// * ARGUMENTS:
//   - pixel coordinates:
//     int x, y;
//   - size of line:
//     double size;
//   - alpha chanel part;
//     int alpha;
//   - line color:
//     unsigned char c.
// * RETURNS:
//   - None.
void PutPixel (unsigned char c, int x, int y, int alpha, double size )
{
    // ANTI-GAMMA CORRECTION
//    if (PIC[y][x][0] / 255.0 < 0.04045)
//        PIC[y][x][0] = ((PIC[y][x][0] / 255.0) * 25 / 323) * 255.0;
//    else
//        PIC[y][x][0] = (unsigned char)(pow(((211.0 * PIC[y][x][0] / 255.0 + 11) / 200.0), 11 / 5.0) * 255);
//
    if (size < 2)
        // Converting new pixel color with alpha channel part and line size
        PIC[y][x][0] = PIC[y][x][0] * (1 - alpha * size / 255.0) + c * size * alpha / 255.0;
    else
        PIC[y][x][0] = c;
            
    // GAMMA CORRECTION
//    PIC[y][x][0] = ((211.0 * pow((PIC[y][x][0] / 255.0), 5 / 11.0) - 11) / 211.0) * 255.0;
    
    printf("alpha: %i color: %i piccolor: %i \n ", alpha, c, PIC[y][x][0]);
}
// End of 'PutPixel' function

// Put new pixel function for drawing lines by Brezenhem algorithm function.
// * ARGUMENTS:
//   - pixel coordinates:
//     int x, y;
//   - alpha chanel part;
//     int alpha;
//   - line color:
//     unsigned char c.
// * RETURNS:
//   - None.
void PutPixel1 (unsigned char c, int x, int y, int alpha )
{
    // ANTI-GAMMA CORRECTION
//    if (PIC[y][x][0] / 255.0 < 0.04045)
//        PIC[y][x][0] = ((PIC[y][x][0] / 255.0) * 25 / 323.0) * 255.0;
//    else
//        PIC[y][x][0] = (unsigned char)(pow(((211.0 * PIC[y][x][0] / 255.0 + 11) / 211.0), 11 / 5.0) * 255.0);
    
    if (size < 2)
        PIC[y][x][0] = PIC[y][x][0] * (1 - alpha / 255.0) + c * alpha / 255.0;
    else
        PIC[y][x][0] = c;
    
    // GAMMA CORRECTION
//    PIC[y][x][0] = ((211.0 * pow((PIC[y][x][0] / 255.0), 5 / 12.0) - 11) / 211.0) * 255.0;
}
// End of 'PutPixel1' function


//
// Drawing lines functions for time when (y0 > y1 & x0 > x1) or (y0 < y1 & x0 < x1)
//

// Drawing line by Wu algorithm function.
// * ARGUMENTS:
//   - points coordinates:
//     double x0, y0, x1, y1;
//   - line color;
//     unsigned char c;
//   - line size;
//     double size.
// * RETURNS:
//   - None.
void DrawWuLine (unsigned char c, double x0, double y0, double x1, double y1, double size )
{
           double dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
           double dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);

           if (dy < dx)
           {
               if (x1 < x0)
               {
                   x1 += x0; x0 = x1 - x0; x1 -= x0;
                   y1 += y0; y0 = y1 - y0; y1 -= y0;
               }
               float grad = (float)dy / dx;
               float intery = y0 + grad;
         
               for (int x = x0; x < x1; x++)
               {
                   // Сalculating the distance to the center of the square
                   double distx = (x0 - (int)x0) > 0.5 ? 1 - ((x0 - (int)x0 - 0.5) / 0.5) : 1 - (x0 - (int)x0) / 0.5;
                   double disty = (intery - (int)intery) > 0.5 ? 1 - ((intery - (int)intery - 0.5) / 0.5) : 1 - (intery - (int)intery) / 0.5;
                   
                   PutPixel(c, x, IPart(intery), 255 - (int)(FPart(intery) * 255), size * sqrt(disty * disty + distx * distx));
                   PutPixel(c, x, IPart(intery) + 1, (int)(FPart(intery) * 255), size  * sqrt(disty * disty + distx * distx));
                   intery += grad;
               }
           }
           else
           {
               if (y1 < y0)
               {
                   x1 += x0; x0 = x1 - x0; x1 -= x0;
                   y1 += y0; y0 = y1 - y0; y1 -= y0;
               }
               float grad = (float)dx / dy;
               float interx = x0 + grad;
         
               for (int y = y0; y < y1; y++)
               {
                   // Сalculating the distance to the center of the square
                   double distx = (y0 - (int)y0) > 0.5 ? 1 - ((y0 - (int)y0 - 0.5) / 0.5) : 1 - (y0 - (int)y0) / 0.5;
                   double disty = (interx - (int)interx) > 0.5 ? 1 - ((interx - (int)interx - 0.5) / 0.5) : 1 - (interx - (int)interx) / 0.5;
                   
                   PutPixel(c, IPart(interx), y, 255 - (int)(FPart(interx) * 255), size * sqrt(disty * disty + distx * distx));
                   PutPixel(c, IPart(interx) + 1, y, (int)(FPart(interx) * 255), size * sqrt(disty * disty + distx * distx));
                   interx += grad;
               }
           }
}
// End of 'DrawWuLine' function

// Drawing line by Brezenhem algorithm function.
// * ARGUMENTS:
//   - points coordinates:
//     double x0, y0, x1, y1;
//   - line color;
//     unsigned char c;
// * RETURNS:
//   - None.
void DrawWuLine1 (unsigned char c, double x0, double y0, double x1, double y1 )
{
           double dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
           double dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);

           if (dy < dx)
           {
               if (x1 < x0)
               {
                   x1 += x0; x0 = x1 - x0; x1 -= x0;
                   y1 += y0; y0 = y1 - y0; y1 -= y0;
               }
               float grad = (float)dy / dx;
               float intery = y0 + grad;
         
               for (int x = x0; x < x1; x++)
               {
                   PutPixel1(c, x, IPart(intery), 255 - (int)FPart(intery));
                   PutPixel1(c, x, IPart(intery) + 1, (int)(FPart(intery)));
                   intery += grad;
               }
           }
           else
           {
               if (y1 < y0)
               {
                   x1 += x0; x0 = x1 - x0; x1 -= x0;
                   y1 += y0; y0 = y1 - y0; y1 -= y0;
               }
               float grad = (float)dx / dy;
               float interx = x0 + grad;
         
               for (int y = y0; y < y1; y++)
               {
                   PutPixel1(c, IPart(interx), y, 255 - (int)(FPart(interx) * 255));
                   PutPixel1(c, IPart(interx) + 1, y, (int)(FPart(interx) * 255));
                   interx += grad;
               }
           }
}
// End of 'DrawWuLine1' function

//
// Drawing lines functions for time when (y0 > y1 & x0 < x1) or (y0 < y1 & x0 > x1)
//

// Drawing line by Brezenhem algorithm function.
// * ARGUMENTS:
//   - points coordinates:
//     double x0, y0, x1, y1;
//   - line color;
//     unsigned char c;
// * RETURNS:
//   - None.
void DrawWuLineY (unsigned char c, double x0, double y0, double x1, double y1 )
{
           double dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
           double dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);

           if (dy < dx)
           {
               if (x1 <  x0)
               {
                    x1 += x0; x0 = x1 - x0; x1 -= x0;
                    y1 += y0; y0 = y1 - y0; y1 -= y0;
               }
               
               float grad = (float)dy / dx;
               float intery = y0 - grad;
         
               for (int x = x0; x < x1; x++)
               {
                   PutPixel1(c, x, IPart(intery), 255 - ((int)FPart(intery) * 255));
                   PutPixel1(c, x, IPart(intery) + 1, (int)(FPart(intery) * 255));
                   intery -= grad;
               }
           }
           else
           {
               if (y1 > y0)
               {
                   x1 += x0; x0 = x1 - x0; x1 -= x0;
                   y1 += y0; y0 = y1 - y0; y1 -= y0;
               }
               float grad = (float)dx / dy;
               float interx = x0 + grad;
         
               for (int y = y0; y > y1; y--)
               {
                   PutPixel1(c, IPart(interx), y, 255 - (int)(FPart(interx) * 255));
                   PutPixel1(c, IPart(interx) + 1, y, (int)(FPart(interx) * 255));
                   interx += grad;
               }
           }
}
// End of 'DrawWuLineY' function

// Drawing line by Wu algorithm function.
// * ARGUMENTS:
//   - points coordinates:
//     double x0, y0, x1, y1;
//   - line color;
//     unsigned char c;
//   - line size;
//     double size.
// * RETURNS:
//   - None.
void DrawWuLineY1 (unsigned char c, double x0, double y0, double x1, double y1, double size )
{
           double dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
           double dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);

           if (dy < dx)
           {
               if (x1 < x0)
               {
                    x1 += x0; x0 = x1 - x0; x1 -= x0;
                    y1 += y0; y0 = y1 - y0; y1 -= y0;
               }
               float grad = (float)dy / dx;
               float intery = y0 - grad;
         
               for (int x = x0; x < x1; x++)
               {
                   // Сalculating the distance to the center of the square
                   double distx = (x0 - (int)x0) > 0.5 ? 1 - ((x0 - (int)x0 - 0.5) / 0.5) : 1 - (x0 - (int)x0) / 0.5;
                   double disty = (intery - (int)intery) > 0.5 ? 1 - ((intery - (int)intery - 0.5) / 0.5) : 1 - (intery - (int)intery) / 0.5;
                   
                   PutPixel(c, x, IPart(intery), 255 - ((int)FPart(intery) * 255), size * sqrt(disty * disty + distx * distx));
                   PutPixel(c, x, IPart(intery) + 1, (int)(FPart(intery) * 255), size * sqrt( disty * disty + distx * distx));
                   intery -= grad;
               }
           }
           else
           {
               if (y1 > y0)
               {
                   x1 += x0; x0 = x1 - x0; x1 -= x0;
                   y1 += y0; y0 = y1 - y0; y1 -= y0;
               }
               float grad = (float)dx / dy;
               float interx = x0 + grad;
         
               for (int y = y0; y > y1; y--)
               {
                   // Сalculating the distance to the center of the square
                   double distx = (y0 - (int)y0) > 0.5 ? 1 - ((y0 - (int)y0 - 0.5) / 0.5) : 1 - (y0 - (int)y0) / 0.5;
                   double disty = (interx - (int)interx) > 0.5 ? 1 - ((interx - (int)interx - 0.5) / 0.5) : 1 - (interx - (int)interx) / 0.5;
                   
                   PutPixel(c, IPart(interx), y, 255 - (int)(FPart(interx) * 255), size * sqrt(disty * disty + distx * distx));
                   PutPixel(c, IPart(interx) + 1, y, (int)(FPart(interx) * 255), size * sqrt(disty * disty + distx * distx));
                   interx += grad;
               }
           }
}
// End of 'DrawWuLineY1' function

//
// Working with pnm image functions
//

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
}
// End of 'SavePNM' function


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
}
// End of 'LoadPNM' function

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
}
// End of 'strtonumb' function


int main( int argc, char * argv[] )
{
    FILE *F1, *F2;
    int gamma, i = 0;
    double x1, x2, y1, y2;
    
    if ((F1 = fopen(argv[1], "rb")) == NULL )
    {
        printf("Cant open file for loading picture!\n");
        return 1;
    }
    if ((F2 = fopen(argv[2], "wb")) == NULL )
    {
        printf("Cant open file for saving picture!\n");
        fclose(F1);
        return 1;
    }
    
    // Converting all variables
    x1 = strtonumb(argv[3]);
    y1 = strtonumb(argv[4]);
    x2 = strtonumb(argv[5]);
    y2 = strtonumb(argv[6]);
    size = strtonumb(argv[7]);
    gamma = strtonumb(argv[8]);
    
    // Load image from file
    LoadPNM(F1);
    printf(" x1 = %lf\n x2 = %lf\n y1 = %lf\n y2 = %lf\n gamma = %i\n size = %lf\n", x1, x2, y1, y2, gamma, size);
    
    // Checkin variables on the possibility of using
    if (x1 > 0 && x1 < w && x2 > 0 && x2 < h && y1 < h && y1 > 0 && y2 > 0 && y2 < h && size > 0 && gamma >=0 && gamma <= 255)
    {
        if (FPart(x1) == 0 && FPart(y1) == 0 && FPart(x2) == 0 && FPart(y2) == 0)
        {
            // If poins are interger, converting to center of square
            x1 += 0.5;
            y1 += 0.5;
            x2 += 0.5;
            y2 += 0.5;
        }
        
        // Checking which function's type should be used
        if ((x2 >= x1 && y2 > y1) || (x2 <= x1 && y2 < y1))
        {
            if (size <= 1)
            {
                DrawWuLine(gamma, x1, y1, x2, y2, size);
            }
            else if (size < 2)
            {
                DrawWuLine1(gamma, x1, y1, x2, y2);
                DrawWuLine(gamma, x1 + 1, y1, x2 + 1, y2, size / 2);
                DrawWuLine(gamma, x1 - 1, y1, x2 - 1, y2, size / 2);
            }
            else
            {
                // Drawing lines when size > 2
                
                double sizeold = size;
                for (i = 0; size > 0; size -= 2)
                {
                    // the line is increasing relative to the central
                    if (size > 2)
                    {
                        DrawWuLine1(gamma, x1 + i, y1, x2 + i, y2);
                        DrawWuLine1(gamma, x1 - i, y1, x2 - i, y2);
                    }
                    else
                    {
                        DrawWuLine(gamma, x1 + i, y1, x2 + i, y2, size / 2);
                        DrawWuLine(gamma, x1 - i, y1, x2 - i, y2, size / 2);
                    }
                    i++;
                }
                
                // bright selection of starting points (NOT FINISHED)
                size = sizeold;
                for (i = 0; size > 0; size -= 2)
                {
                    if (x1 - (int)x1 > 0.5)
                    {
                        if (x2 - (int)x2 > 0.5)
                        {
                            if (size > 2)
                            {
                                DrawWuLineY(gamma, (int)x1 - size / 2 + 2, (int)y1 - i, (int)x1 + size / 2 + 1, (int)y1 - i);
                                DrawWuLineY(gamma, (int)x2 - size / 2, (int)y2 + i, (int)(x2 - 1) + size / 2, (int)y2 + i);
                            }
                        }
                        else
                        {
                            if (size > 2)
                            {
                                DrawWuLineY(gamma, (int)x1 - size / 2 + 2, (int)y1 - i, (int)x1 + size / 2 + 1, (int)y1 - i);
                                DrawWuLineY(gamma, (int)x2 - size / 2 + 1, (int)y2 + i, (int)(x2 - 1) + size / 2, (int)y2 + i);
                            }
                        }
                    }
                    else
                    {
                        if (x2 - (int)x2 > 0.5)
                        {
                            if (size > 2)
                            {
                                DrawWuLineY(gamma, (int)x1 - size / 2 + 2, (int)y1 - i, (int)x1 + size / 2 + 1, (int)y1 - i);
                                DrawWuLineY(gamma, (int)x2 - size / 2, (int)y2 + i, (int)(x2 - 1) + size / 2, (int)y2 + i);
                            }
                        }
                        else
                        {
                            if (size > 2)
                            {
                                DrawWuLineY(gamma, (int)x1 - size / 2 + 1, (int)y1 - i, (int)x1 + size / 2 + 1, (int)y1 - i);
                                DrawWuLineY(gamma, (int)x2 - size / 2 + 1, (int)y2 + i, (int)(x2 - 1) + size / 2, (int)y2 + i);
                            }
                        }

                    }
                    i++;
                }
            }
        }
        else
        {
            if (size <= 1)
            {
                DrawWuLineY1(gamma, x1, y1, x2, y2, size);
            }
            else if (size < 2)
            {
                DrawWuLineY(gamma, x1, y1, x2, y2);
                DrawWuLineY1(gamma, x1, y1 + 1, x2, y2 + 1, size / 2);
                DrawWuLineY1(gamma, x1, y1 - 1, x2, y2 - 1, size / 2);
            }
            else
            {
                // Drawing lines when size > 2
                
                double sizeold = size;
                for (i = 0; size > 0; size -= 2)
                {
                    // the line is increasing relative to the central
                    if (size > 2)
                    {
                        DrawWuLineY(gamma, x1, y1 + i, x2, y2 + i);
                        DrawWuLineY(gamma, x1, y1 - i, x2, y2 - i);
                    }
                    else
                    {
                        DrawWuLineY1(gamma, x1, y1 + i, x2, y2 + i, size / 2);
                        DrawWuLineY1(gamma, x1, y1 - i, x2, y2 - i, size / 2);
                    }
                    i++;
                }
                
                // bright selection of starting points (NOT FINISHED)
                size = sizeold;
                for (i = 0; size > 0; size -= 2)
                {
                    if (y1 - (int)y1 > 0.5)
                    {
                        if (y2 - (int)y2 > 0.5)
                        {
                            if (size > 2)
                            {
                                DrawWuLine1(gamma, (int)x1 - i, (int)(y1 + 1) - size / 2, (int)x1 - i, (int)(y1 - 1) + size / 2);
                                DrawWuLine1(gamma, (int)x2 + i, (int)(y2 ) - size / 2, (int)x2 + i, (int)(y2 - 1) + size / 2);
                            }
                        }
                        else
                         {
                             if (size > 2)
                             {
                                 DrawWuLine1(gamma, (int)x1 - i, (int)(y1 + 1) - size / 2, (int)x1 - i, (int)(y1) + size / 2);
                                 DrawWuLine1(gamma, (int)x2 + i, (int)(y2 + 1) - size / 2, (int)x2 + i, (int)(y2) + size / 2);
                             }
                         }
                    }
                    else
                    {
                        if (y2 - (int)y2 > 0.5)
                        {
                            if (size > 2)
                            {
                                DrawWuLine1(gamma, (int)x1 - i, (int)(y1 + 1) - size / 2, (int)x1 - i, (int)(y1 - 1) + size / 2);
                                DrawWuLine1(gamma, (int)x2 + i, (int)(y2 + 1) - size / 2, (int)x2 + i, (int)(y2) + size / 2);
                            }
                        }
                        else
                         {
                             if (size > 2)
                             {
                                 DrawWuLine1(gamma, (int)x1 - i, (int)(y1 + 1) - size / 2, (int)x1 - i, (int)(y1) + size / 2);
                                 DrawWuLine1(gamma, (int)x2 + i, (int)(y2 + 1) - size / 2, (int)x2 + i, (int)(y2 - 1) + size / 2);
                             }
                         }
                    }
                    
                    i++;
                }
            }
        }
        SavePNM(F2);
    }
    else
        printf("Wrong paraments!\n");
    
    fclose(F1);
    fclose(F2);
}
