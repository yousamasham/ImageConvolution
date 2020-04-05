#include<stdio.h>
#include<stdlib.h>

typedef struct {
     unsigned char red,green,blue;
} PPMPixel;

typedef struct {
     int x, y;
     PPMPixel *data;
} PPMImage;

#define CREATOR "RPFELGUEIRAS"
#define RGB_COMPONENT_COLOR 255

static PPMImage *readPPM(const char *filename)
{
         char buff[16];
         PPMImage *img;
         FILE *fp;
         int c, rgb_comp_color;
         //open PPM file for reading
         fp = fopen(filename, "rb");
         if (!fp) {
              fprintf(stderr, "Unable to open file '%s'\n", filename);
              exit(1);
         }

         //read image format
         if (!fgets(buff, sizeof(buff), fp)) {
              perror(filename);
              exit(1);
         }


    //alloc memory form image
    img = (PPMImage *)malloc(sizeof(PPMImage));
    if (!img) {
         fprintf(stderr, "Unable to allocate memory\n");
         exit(1);
    }

    //check for comments
    c = getc(fp);
    while (c == '#') {
    while (getc(fp) != '\n') ;
         c = getc(fp);
    }

    ungetc(c, fp);
    //read image size information
    if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
         fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
         exit(1);
    }

    //read rgb component
    if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
         fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
         exit(1);
    }

    //check rgb component depth
    if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
         fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
         exit(1);
    }

    while (fgetc(fp) != '\n') ;
    //memory allocation for pixel data
    img->data = (PPMPixel*)malloc(img->x * img->y * sizeof(PPMPixel));

    if (!img) {
         fprintf(stderr, "Unable to allocate memory\n");
         exit(1);
    }

    //read pixel data from file
    if (fread(img->data, 3 * img->x, img->y, fp) != img->y) {
         fprintf(stderr, "Error loading image '%s'\n", filename);
         exit(1);
    }

    fclose(fp);
    return img;
}
void writePPM(const char *filename, PPMImage *img)
{
    FILE *fp;
    //open file for output
    fp = fopen(filename, "wb");
    if (!fp) {
         fprintf(stderr, "Unable to open file '%s'\n", filename);
         exit(1);
    }

    //write the header file
    //image format
    fprintf(fp, "P3\n");

    //image size
    fprintf(fp, "%d %d\n",img->x,img->y);

    // rgb component depth
    fprintf(fp, "%d\n",RGB_COMPONENT_COLOR);

    // pixel data
    fwrite(img->data, 3 * img->x, img->y, fp);
    fclose(fp);
}

double GET_PIXEL_CHECK(PPMImage *img,int ix,int iy,int l){
	if(ix>=img->y || iy>=img->x )return 0;

	if(l==0)return img->data[iy*(img->y)+ix].red;
	if(l==1)return img->data[iy*(img->y)+ix].green;
	if(l==2)return img->data[iy*(img->y)+ix].blue;

}

void filter(PPMImage *img,int *K,double divisor,int Ks,PPMImage *out)
{
    int i,ix,iy,kx,ky,l;
    if(img){

         double cp[3];
         for(ix=0;ix<img->y;ix++){
         	for(iy=0;iy<img->x;iy++){
         		cp[0]=cp[1]=cp[2]=0;

         		for(kx=-Ks;kx<=Ks;kx++){
         			for(ky=-Ks;ky<=Ks;ky++){

         				for(l=0;l<3;l++){
         				    cp[l] += (K[(kx+Ks) + (ky+Ks)*(2*Ks+1)]/divisor) * ((double)GET_PIXEL_CHECK(img, ix+kx, iy+ky, l));
						 }
					 }
				 }
         		for(l=0; l<3; l++) cp[l] = (cp[l]>255.0) ? 255.0 : ((cp[l]<0.0) ? 0.0 : cp[l]) ;
         		out->data[iy*(out->y)+ix].red=cp[0];
				out->data[iy*(out->y)+ix].green=cp[1];
				out->data[iy*(out->y)+ix].blue=cp[2];
			 }
		 }

	}

}

int main(int argc, char** argv){
    PPMImage *image;
    image = readPPM(argv[0]);
    PPMImage *output;
    output=(PPMImage *)malloc(sizeof(PPMImage));
    output->x=image->x;
    output->y=image->y;
    output->data=(PPMPixel*)malloc( (image->x * image->y) * sizeof(PPMPixel));
    int sz,scale;
    int *K;
    FILE* file = fopen (argv[1], "r");
    fscanf (file, "%d", &sz);
	  K=(int*) malloc(sizeof(int)*(sz*sz));

	fscanf (file, "%d", &scale);

	int i,cnt=0;
    while (!feof (file))
    {
      fscanf (file, "%d", &i);
      K[cnt]=i;
      cnt++;
    }

	filter(image,K,scale,(sz-1)/2,output);

    writePPM(argv[2],image);
    printf("Completed");
    getchar();
}
