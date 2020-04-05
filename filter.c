/*
AUTHOR: Yousam Asham
MACID: ashamy1
STUDENT#: 400174082
SFWRENG 2S03 Dr. Nedialov
*/
#include<stdio.h>
#include<stdlib.h>
#define COLOR_COMPONENT_GOAL 255
#define MAX 15
typedef struct {
     int red,green,blue;
} PPMPixel;

typedef struct{
  int n;
  float scale;
  int *nums;
}kernel;

typedef struct {
     int x, y;
     PPMPixel *data;
} PPMImage;

PPMImage *readPPM(const char *file){
  char buffer[16];
  int component;
  PPMImage *image;
  FILE *fptr;
  fptr = fopen(file, "rb");
  if (!fptr){
    printf("Unable to read PPM image");
    exit(1);
  }
  if (!fgets(buffer, sizeof(buffer), fptr)){
    perror(file);
    exit(1);
  }
  if (buffer[0] != 'P' || buffer[1] != '3'){
    printf("Invalid image format (must be 'P3')\n");
    exit(1);
  }
  image = (PPMImage *)malloc(sizeof(PPMImage));
  if (!image){
    printf("Unable to allocate memory\n");
    exit(1);
  }
  if (fscanf(fptr, "%d %d", &image->x, &image->y) != 2){
    printf("Invalid image size (error loading '%s')\n", file);
    exit(1);
  }
  if (fscanf(fptr, "%d", &component) != 1){
    printf("Invalid rgb component (error loading '%s')\n", file);
    exit(1);
  }
  if (component!= COLOR_COMPONENT_GOAL){
    printf("'%s' does not have 8-bits components\n", file);
    exit(1);
  }
  while (fgetc(fptr) != '\n');
    image->data = (PPMPixel*)malloc(image->x * image->y * sizeof(PPMPixel));
    if (!image) {
      printf("Unable to allocate memory\n");
      exit(1);
    }
  int index=0;
  char s=fgetc(fptr);
  while (index < image->x * image->y ){
    if (s != EOF){
      ungetc(s, fptr);
      fscanf(fptr, "%d %d %d", &image->data[index].red, &image->data[index].green, &image->data[index].blue);
    }
    index++;
    s = fgetc(fptr);
  }
    fclose(fptr);
    return image;
}

kernel* readKernel(const char* filename){
  kernel* ker = malloc(sizeof(kernel));
  FILE* kernelfp = fopen(filename, "r");
  char a = fgetc(kernelfp);
  int n = a - '0';
  ker->n = (a-'0');
  for (int i = 0 ; i < 3 ; i ++)
  a = fgetc(kernelfp);
  float temp = a - '0';
  ker->scale = (float)1 / temp;
  a = fgetc(kernelfp);
  a = fgetc(kernelfp);
  ker->nums = (int*)malloc(n * n * sizeof(int));
  for (int j = 0 ; j < n ; j++){
    for (int k = 0 ; k < n ; k++){
    fscanf(kernelfp, "%d", &(ker->nums[j*n + k]));
    a = fgetc(kernelfp);
    //printf("%d\n",ker->nums[j*n + k]);
  }
  a = fgetc(kernelfp);
}
  fclose(kernelfp);
  return ker;
}PPMImage* conv(PPMImage* img, kernel* ker){
  int nx = img->x - ker->n + 1;
  int ny = img->y - ker->n + 1;
  PPMImage* convImage = malloc(sizeof(PPMImage));
  convImage->x = nx;
  convImage->y = ny;
  convImage->data = (PPMPixel*)malloc(nx * ny * sizeof(PPMPixel));
  int x = img->x;
  int y = img->y;
  int kern = ker->n;
  for (int i = 0 ; i < nx ; i++){
    for (int j = 0 ; j < nx ; j++){
      convImage->data[i*nx+j].red = 0;
      convImage->data[i*nx+j].green = 0;
      convImage->data[i*nx+j].blue = 0;
    }
  }
  for (int i = 0 ; i < nx ; i++){
    for (int j = 0 ; j < ny ; j++){
      for (int n1 = 0 ; n1 < kern ; n1++){
        for (int n2 = 0 ; n2 < kern ; n2++){
          //for (int p = 0 ; p < kern ; p++){
            //for (int h = 0; h < kern ; h++){
              convImage->data[i*nx+j].red += (int)((ker->scale) * (ker->nums[n1*kern+n2]*img->data[i*x+j].red));
              convImage->data[i*nx+j].green += (int)((ker->scale) * (ker->nums[n1*kern+n2]*img->data[i*x+j].green));
              convImage->data[i*nx+j].blue += (int)((ker->scale) * (ker->nums[n1*kern+n2]*img->data[i*x+j].blue));
            //}
          //}
        }
      }
    }
  }
return convImage;
}
void outputConvImg(PPMImage* convImage, char* file){
  FILE* convFile = fopen(file,"wb");
  fprintf(convFile,"P3\n");
  fprintf(convFile,"%d %d\n", convImage->x, convImage->y);
  fprintf(convFile,"%d\n", COLOR_COMPONENT_GOAL);
  for (int i = 0 ; i < convImage->x; i++){
    for (int j = 0 ; j < convImage->y; j++){
      fprintf(convFile,"%d %d %d ", convImage->data[i*(convImage->x)+j].red, convImage->data[i*(convImage->x)+j].green, convImage->data[i*(convImage->x)+j].blue);
    }
    fprintf(convFile, "\n");
  }
  fclose(convFile);
  return;
}
int main(int argc, char** argv){
  if (argc != 4){
    printf("Usage: ./filter <input file name> <kernel file name> <output file name>\n");
  }
  char* file = argv[1];
  char* kernelFile = argv[2];
  char* ofile = argv[3];
  PPMImage* pic = readPPM(file);
  kernel* ker = readKernel(kernelFile);
  PPMImage* convPic = conv(pic, ker);
  outputConvImg(convPic, ofile);
  return 0;
}
