#include <stdio.h>
#include <malloc.h>

#include "nedclib.h"
#include "rawbin/rs.h"

int raw_pos;
int bin_pos;

int bin_type=0; //0 = Single File NEDC bin.  1 = Multi file NEDC bin.  2 = Multi file DRPD bin.

NEDCLIB_API int nedc_fopen(FILE** f, const char* name, const char* mode) {
  FILE* handle = fopen(name, mode);
  *f = handle;
  return !handle;
}

int count_bin(FILE *f)
{
  int count=0;
  int i;
  unsigned char *data;

  fseek(f,0,SEEK_END);
  i=ftell(f);
  fseek(f,0,SEEK_SET);
  if((i==0x81C)||(i==0x51C))
  {
    bin_type=0;  //Nintendo e-Reader, old style bin format.
    return 1;  //Count is one dotcode bin.
  }
  else
  {
    bin_type = 1;  
    fseek(f,0,SEEK_SET);
  }
  data = (unsigned char*)malloc(0x840); //Worst case bin file size, no error correction.
  if(data==NULL)
    return 0;  //Couldn't allocate the memory for dotcode bins.
  while(read_next_bin(f,data))
    count++;
  fseek(f,0,SEEK_SET);  //Rewind back to first bin.
  

  free(data);
  return count;
}

int read_next_bin(FILE *f, unsigned char *bindata)
{
  int result;
  bin_pos=ftell(f);
  switch(bin_type)
  {
  case 0:
    result = (int)fread(bindata,1,0x81C,f);
    break;
  case 1:
    result = (int)fread(bindata,1,0x30,f);
    result += (int)fread(&bindata[0x30],1,(bindata[6]==8)?0x810:0x510,f);
    break;
  }

  return result;
}


int count_raw(FILE *f)
{
  int count=0;
  unsigned char *data;
  
  data = (unsigned char*)malloc(0xB60);  //Worst case raw file size.
  if(data==NULL)
    return 0;  //Unable to allocate memory (and therefore count the raw data files.)
  fseek(f,0,SEEK_SET);
  while(read_next_raw(f,data))
    count++;
  free(data);
  fseek(f,0,SEEK_SET);
  return count;
}



int read_next_raw(FILE *f, unsigned char *rawdata)
{
  size_t result;

  raw_pos = ftell(f);

  unsigned char rawheader[24];
  int i = 0, j = 0, k = 0, l = 0;

  if(is_rs_initialized())
    l=1;
  else
    initialize_rs();

  for(i=0,result=0;i<24;i+=2)
  {
    result+=fread(&rawheader[i],1,2,f);
    fseek(f,0x66,SEEK_CUR);
  }
  if(result==24)
  {
    fseek(f,raw_pos,SEEK_SET);
    //int rs_decode(unsigned char *data, unsigned char *erasure, int size, int parity, int encoder)
    //if(rs_decode(rawheader,(unsigned char*)erasuredata,24,16,0)>=0)
    if(correct_errors(rawheader,24,16)>=0)
    {
      i=rawheader[4]*rawheader[7];
      if((i%0x66)>0)
        i+=(0x66-(i%0x66));
      i/=0x66;
      k=i;
      i*=0x68;
      if(i>0xB60)
      {
        if(l==0) free_rs();
        return 0;
      }
      result = fread(rawdata,1,i,f);
      if(i!=result)
      {
        if(l==0) free_rs();
        return 0;  //Raw data shorter than calculated.
      }
      j=0;
      while((j*12)<k)
      {
        for(i=0;(i<12)&&(((j*12)+i)<k);i++)
        {
          rawdata[(((j*12)+i)*0x68)+0]=rawheader[(i*2)+0];
          rawdata[(((j*12)+i)*0x68)+1]=rawheader[(i*2)+1];
        }
        j++;
      }
      if(l==0) free_rs();
      return (int)result;
    }
    else
    {
      if(l==0) free_rs();
      return 0;  //Reed solomon header failure.  May not be a valid raw file.
    }
  }
  else
  {
    if(l==0) free_rs();
    return 0;  //A dotcode file must have at least 12 dotcode blocks.
          //due to the standard of 8 byte header + 16 byte error correction.
  }
    
}

void backtrack_raw(FILE *f)
{
  fseek(f,raw_pos,SEEK_SET);
}

int close_raw(FILE *f, int return_res)
{
  if(f!=NULL)
    fclose(f);
  return return_res;
}

NEDCLIB_API void nedclib_version(void) {
  //printf("nedclib.dll version %d.%d\n",version_major,version_minor);
  //printf("Copyright 2007 CaitSith2\n\n");
}

NEDCLIB_API bool is_vpk(unsigned char *bindata) {
  return bindata[0] == 'v' && bindata[1] == 'p' && bindata[2] == 'k' && bindata[3] == '0';
}

NEDCLIB_API bool is_nes(unsigned char *nesdata) {
  return nesdata[0] == 'N' && nesdata[1] == 'E' && nesdata[2] == 'S' && nesdata[3] == 0x1A;
}

NEDCLIB_API bool is_bmp(char *bmpfile) {
  FILE* f = fopen(bmpfile, "rb");
  if(!f) return false;
  
  uint16_t bmp_header;
  fread(&bmp_header, sizeof(bmp_header), 1, f);

  uint32_t bmp_size;
  fread(&bmp_size, sizeof(bmp_size), 1, f);
  
  fseek(f, 0, SEEK_END);
  size_t file_size = ftell(f);
  fclose(f);
  
  return bmp_size == file_size && bmp_header == 0x4D42;
}

NEDCLIB_API int MultiStrip=0;

NEDCLIB_API int version_major = VERSION_MAJOR;
NEDCLIB_API int version_minor = VERSION_MINOR;
