/*
 *  24位深的bmp图片转换为16位深RGB565格式的bmp图片
 *  Original: from network
 *  Modified by Golden.Chen 2019/02/16
 */
#include <iostream>
#include <stdio.h>
#include <string.h>
 
using namespace std;
 
typedef unsigned char  uint8;
typedef unsigned short uint16; //32bit system
typedef unsigned int   uint32;//32bit system
 
#define SRC_BMP_BPP    24   /* bits per pixel */
#define DST_BMP_BPP    16
 
struct bmp_filehead //size: 14B
{
    uint16 bmtype; //0x424d ='BM'
    uint16 bmtotalsize; //total file size 
    uint16 bmtotalsizeH;
    uint16 bmReserved;//reserve word,fixed to : 0x0000
    uint16 bmReservedH; 
    uint16 bmOffBits;//offset of image data
    uint16 bmOffBitsH;
}bmphead;
 
struct bmp_infohead //size: 40B
{
    uint32 bmpheadsize;//size of infoheader
    uint32 bmp_width;//image width
    uint32 bmp_height;//image height
    uint16 bmplans;//planes,0 or 1
    uint16 bitcount;//bits per pixel,24
    uint32 compress_type;/* 0 - rgb, 1 - rle8, 2 - rle4, 3 - BITFEILDS */
    uint32 imagesize;//size of image data
 
    uint32 biXPelsPerMeter;
    uint32 biYPelsPerMeter;
    uint32 biClrUsed;
    uint32 biClrImportant;
}infohead;
 
int bmp565_write_header(struct bmp_filehead *pfhd,struct bmp_infohead *pinfo,FILE *fp)
{
	int ret;
	uint32 widthsize,head_size,imgdata_offset;
	uint8 rgb_mask[12] = {// RGBQUAD MASK     
        	0x00, 0xF8, 0x00, 0x00, //red mask     
 		0xE0, 0x07, 0x00, 0x00, //green mask    
		0x1F, 0x00, 0x00, 0x00  //blue mask
	}; 
	
	widthsize = (((uint32)pinfo->bmp_width * DST_BMP_BPP + 31) >> 5) << 2;
	head_size = sizeof(struct bmp_filehead) + sizeof(struct bmp_infohead) + sizeof(rgb_mask);
 
	/* base on src data ,initialize bmp_infoheader*/
	pinfo->bitcount = DST_BMP_BPP;
	pinfo->compress_type = 3 ; //only valid when BPP == 16 or 32
	pinfo->imagesize = pinfo->bmp_height * widthsize;
	pinfo->biXPelsPerMeter = 0;
	pinfo->biYPelsPerMeter = 0;
	pinfo->biClrUsed = 0;
	pinfo->biClrImportant = 0;
 
	/* base on src data ,initialize bmp_fileheader*/
	pfhd->bmtotalsize  = (head_size + pinfo->imagesize) & 0xFFFF;
	pfhd->bmtotalsizeH = (head_size + pinfo->imagesize) >> 16;
	pfhd->bmOffBits  = head_size & 0xFFFF;
	pfhd->bmOffBitsH = head_size >> 16;
 
	//write header data to fp
	ret = fwrite(pfhd,  1, sizeof(struct bmp_filehead), fp);
	if(ret != sizeof(struct bmp_filehead)) return -1;
 
	ret = fwrite(pinfo, 1, sizeof(struct bmp_infohead), fp);
	if(ret != sizeof(struct bmp_infohead)) return -1;
 
	ret = fwrite(rgb_mask, 1, sizeof(rgb_mask), fp);
	if(ret != sizeof(rgb_mask)) return -1;
 
	return 0;
}
 
#define bmp565_write_imgdata(buf,block,count,fp)  \
	fwrite(buf, block, count, fp);
 
int main(int argc, char *argv[])
{
	FILE *src_fp,*dst_fp;
	char src_file[64] = "";
	char dst_file[64] = "";
	uint8 *imagebuf=NULL,*bmpwidbuf=NULL;
	uint32 src_widsize,dst_widsize;
	uint16 k,q,d;
	int ret=0,n;
 
	if(argc < 2)
	{
		printf("\nUsage: %s <src file> \n",argv[0]);
		printf("\n       %s <src file>  <output file>\n",argv[0]);
		return -1;
	}
 
	strcpy(src_file, argv[1]);
	if(argc == 3) {
		strcpy(dst_file, argv[2]);
		cout << "dst file name : " << dst_file << endl;
	} else {
		strncpy(dst_file, src_file, strlen(src_file)-4);
		strcat(dst_file, "_rgb565.bmp");
		cout << "dst file default name : " << dst_file << endl;
	}
 
 
	if((src_fp=fopen(src_file,"r")) == NULL) {
		cout<<"Can't Open file: "<< src_file <<endl;
		return -1;
	} 
	if((dst_fp=fopen(dst_file,"wb")) == NULL){
		fclose(src_fp);
		cout<<"Fail to Open dst file: "<< dst_file <<endl;
		return -1;
	}
 
	fseek(src_fp, 0, 0);//fseek(src_fp, offset, fromwhere);
	ret=fread(&bmphead,sizeof(uint8),sizeof(bmphead),src_fp);
	if(ret != sizeof(bmphead)) {
		cout<<"read bmp header failed!"<<endl;
		goto out_err;
	}
 
	ret=fread(&infohead,sizeof(uint8),sizeof(infohead),src_fp);
	if(ret != sizeof(infohead)) {
		cout<<"read bmp info header failed!"<<endl;
		goto out_err;
	}
 
	if(infohead.bitcount != SRC_BMP_BPP) {
		printf("Error: unsupport bpp%d, only support 24-bit bmp!",infohead.bitcount);
		goto out_err;
	}
 
	printf("filesize: %ld, dataoffset: %ld, imagesize: %d\nimage Width: %d, Height: %d\n", \
		(long)(bmphead.bmtotalsize | (bmphead.bmtotalsizeH<<16)),  \
		(long)(bmphead.bmOffBits | (bmphead.bmOffBitsH<<16)),  \
		infohead.imagesize, infohead.bmp_width,infohead.bmp_height);
 
	/* seek to offset indicated by file header */
	fseek(src_fp, (long)(bmphead.bmOffBits|(bmphead.bmOffBitsH<<16)), SEEK_SET);
 
	/* Please note : BMP width size is 4-bytes aliagn in fact */
	src_widsize = ((infohead.bmp_width * infohead.bitcount + 31)>>5)<<2;
	if((bmpwidbuf=(uint8 *)malloc(src_widsize)) == NULL) {
		cout << "Fail to malloc bmpwidbuf!"<<endl;
		goto out_err;
	}
	dst_widsize = ((infohead.bmp_width * DST_BMP_BPP + 31)>>5)<<2;
	if((imagebuf=(uint8 *)malloc(dst_widsize * infohead.bmp_height)) == NULL) {
		cout << "Fail to malloc imagebuf!"<<endl;
		goto out_err;
	}
 
	for(k=0,n=0;k<infohead.bmp_height;k++) {
		ret = fread(bmpwidbuf,sizeof(uint8),src_widsize,src_fp);
		if(ret != src_widsize) {
			cout<<"read bmp data failed !" <<endl;
			goto out_err;
		}
		for(q=0;q<infohead.bmp_width;q++) {
		    d =  ((uint16)(bmpwidbuf[3*q + 2]>>3)<<11)//R G B
			+((uint16)(bmpwidbuf[3*q + 1]>>2)<<5)
			+ (uint16)(bmpwidbuf[3*q + 0]>>3);
		    imagebuf[n++] = d & 0xff;
		    imagebuf[n++] = d >> 8;
		}
		while(n%dst_widsize != 0) imagebuf[n++] = 0x00;
	}
 
	fclose(src_fp);
	free(bmpwidbuf);
 
	cout<<"write bmp 565 start !"<<endl;
	fseek(dst_fp, 0, SEEK_SET);
	ret = bmp565_write_header(&bmphead, &infohead, dst_fp);
	if(!ret) 
		ret = bmp565_write_imgdata(imagebuf, dst_widsize,infohead.bmp_height, dst_fp);
	if(ret != infohead.bmp_height)
		cout<<"write bmp 565 fail!"<<endl;
	else
		cout<<"write bmp 565 success!"<<endl;
 
	fclose(dst_fp);
	free(imagebuf);
	return 0;
	
out_err:
	if(imagebuf) free(imagebuf);
	if(bmpwidbuf) free(bmpwidbuf);
	fclose(src_fp);
	fclose(dst_fp);
	return -1;
}
 