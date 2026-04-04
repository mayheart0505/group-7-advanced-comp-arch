
/* xa_kpcd.c */
/*****************************************************************************
 * Kodak Photo CD (KPCD) Format    
 *
 *****************************************************************************/

/* REVISIONS *************************************************************
 * 0.0  16May96  Initial file by Mark Podlipec (podlipec@shell.portal.com)
 */

#include "xa_types.h"	/* XAnim data types and structures */

/*********** Global Functions */
xaLONG KPCD_Codec_Query();

/*********** Local but used Externally Functions */
xaULONG KPCD_Decode_KPCD();

/*********** Local Functions */

/*********** External XAnim functions that are called/needed */
extern void XA_Gen_YUV_Tabs();
extern void JPG_Setup_Samp_Limit_Table();
extern void JPG_Alloc_MCU_Bufs();
extern void *XA_YUV221111_Func();

/******* external XAnim variables used in various places **************/
extern xaLONG xa_debug;

extern YUVBufs jpg_YUVBufs;
extern YUVTabs def_yuv_tabs;


/******* Defines for XAnim Video Codec Interface **********************/

#define XAPI_REV 0x0002

typedef struct
{
  void *anim_hdr;		/* used to add stuff to Free Chain */
  xaULONG compression;		/* input/output compression */
  xaULONG x,y;			/* input/output x,y */
  xaULONG depth;		/* input depth */
  void *extra;			/* extra for delta */
  xaULONG xapi_rev;		/* XAnim API rev */
  xaULONG (*decoder)();		/* decoder routine */
  char *description;		/* text string */
  xaULONG avi_ctab_flag;	/* AVI ctable to be read */
  xaULONG (*avi_read_ext)();	/* routine to read extended data */

} XA_CODEC_HDR;

#define CODEC_SUPPORTED    1
#define CODEC_UNKNOWN      0
#define CODEC_UNSUPPORTED -1

typedef struct
{
  xaULONG cmd;                  /* decode or query */
  xaULONG skip_flag;            /* skip_flag */
  xaULONG imagex,imagey;        /* Image Buffer Size */
  xaULONG imaged;               /* Image depth */
  XA_CHDR *chdr;                /* Color Map Header */
  xaULONG map_flag;             /* remap image? */
  xaULONG *map;                 /* map to use */
  xaULONG xs,ys;                /* pos of changed area */
  xaULONG xe,ye;                /* size of change area */
  xaULONG special;              /* Special Info */
  xaULONG bytes_pixel;		/* bytes per pixel */
  xaULONG image_type;		/* type of image */
  xaULONG tmp1;			/* future expansion */
  xaULONG tmp2;			/* future expansion */
  xaULONG tmp3;			/* future expansion */
  xaULONG tmp4;			/* future expansion */
  void *extra;                  /* Decompression specific info */
} XA_DEC2_INFO;


#define ID_kpcd  0x6b706364
#define ID_KPCD  0x4b504344


/********* Start of Functions *****************************************/

/***************************
 *
 ***********/
xaULONG KPCD_What_Rev_API() { return(XAPI_REV); }

/***************************
 *
 ***********/
xaLONG KPCD_Codec_Query(codec)
XA_CODEC_HDR *codec;
{ xaLONG ret = CODEC_UNKNOWN;

  codec->extra = 0;
  codec->decoder = 0;
  codec->description = 0;
  codec->avi_read_ext = 0;
  codec->xapi_rev = XAPI_REV;

  switch(codec->compression)
  {
    case ID_kpcd:
    case ID_KPCD:
	codec->compression = ID_KPCD;
	codec->x = 4 * ((codec->x + 3)/4);
	codec->decoder = KPCD_Decode_KPCD;
	codec->description = "Kodak Photo CD";
	codec->avi_ctab_flag = xaFALSE;
	/* KPCD_Init_Stuff();  */
	JPG_Alloc_MCU_Bufs(codec->anim_hdr,codec->x,codec->y,xaTRUE);
	XA_Gen_YUV_Tabs(codec->anim_hdr);
	JPG_Setup_Samp_Limit_Table(codec->anim_hdr);
	ret = CODEC_SUPPORTED;
	break;

    default:
	ret = CODEC_UNKNOWN;
	break;
  }
 return(ret);
}


/************************************
 * This XAnim Delta Function decodes 
 *
 **********/
xaULONG
KPCD_Decode_KPCD(image,delta,dsize,dec_info)
xaUBYTE *image;         /* Image Buffer. */
xaUBYTE *delta;         /* delta data. */
xaULONG dsize;          /* delta size */
XA_DEC2_INFO *dec_info;  /* Decoder Info Header */
{ xaULONG imagex = dec_info->imagex;    xaULONG imagey = dec_info->imagey;
  xaULONG map_flag = dec_info->map_flag;        xaULONG *map = dec_info->map;
  XA_CHDR *chdr = dec_info->chdr;
  xaUBYTE *dptr = delta;
  xaULONG ycnt, row_inc = imagex << 1;
  void (*color_func)() = (void (*)())XA_YUV221111_Func(dec_info->image_type);
  xaUBYTE *ydp, *udp, *vdp;

  dec_info->xs = dec_info->ys = 0;
  dec_info->xe = imagex; dec_info->ye = imagey;
  if (dec_info->skip_flag > 0) return(ACT_DLTA_DROP);
  if (chdr) { if (chdr->new_chdr) chdr = chdr->new_chdr; }

  row_inc *= dec_info->bytes_pixel;

  /* setup input plane pointers */
  ydp = vdp = udp = dptr;
  vdp += imagex * imagey;
  udp += (imagex * imagey) + ((imagex * imagey) >> 2);

  ycnt = 0;
  while(imagey > 0)  /* && (dsize > 0) ) */
  { xaUBYTE *yp,*up,*vp;
    xaULONG x; 

    if (ycnt == 0)
      { yp = jpg_YUVBufs.Ybuf; up = jpg_YUVBufs.Ubuf; vp = jpg_YUVBufs.Vbuf; }

    /* read y line each time */
    x = imagex;		while(x--)	*yp++ = *dptr++;
   
    ycnt++;
    imagey--;
    if ((ycnt >= 2) || (imagey == 0))
    { 
	/* read chroma every other or last with an odd image size if possible*/
      x = imagex>>1;	while(x--)	*up++ = *dptr++;
      x = imagex>>1;	while(x--)	*vp++ = *dptr++;

      color_func(image,imagex,ycnt,imagex,ycnt,
                &jpg_YUVBufs, &def_yuv_tabs, map_flag, map, chdr);
      ycnt = 0;
      image += row_inc;
    }
  } /*end of y */
  if (map_flag) return(ACT_DLTA_MAPD);
  else return(ACT_DLTA_NORM);
}


