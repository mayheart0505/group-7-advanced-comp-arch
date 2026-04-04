
/*
 * xa_formats.c
 *
 * Copyright (C) 1996 by Mark Podlipec.
 * All rights reserved.
 *
 * This software may be freely copied, modified and redistributed without
 * fee for non-commerical purposes provided that this copyright notice is
 * preserved intact on all copies and modified copies.
 *
 * There is no warranty or other guarantee of fitness of this software.
 * It is provided solely "as is". The author(s) disclaim(s) all
 * responsibility and liability with respect to this software's usage
 * or its effect upon hardware or computer systems.
 *
 */

#include "xanim.h"

xaULONG Is_DL_File();
extern xaLONG Is_QT_File();

xaULONG IFF_Read_File();
xaULONG GIF_Read_Anim();
xaULONG TXT_Read_File();
xaULONG Fli_Read_File();
xaULONG DL_Read_File();
/* xaULONG PFX_Read_File(); */
xaULONG MOVI_Read_File();
xaULONG SET_Read_File();
xaULONG RLE_Read_File();
xaULONG AVI_Read_File();
xaULONG QT_Read_File();
xaULONG JFIF_Read_File();
xaULONG MPG_Read_File();
xaULONG JMOV_Read_File();
xaULONG ARM_Read_File();
xaULONG WAV_Read_File();
xaULONG AU_Read_File();
xaULONG DUM_Read_File();


#define XA_ID_FORM	0x464f524d
#define XA_ID_LIST	0x4c495354
#define XA_ID_PROP	0x50524f50
#define XA_ID_GIF8	0x47494638
#define XA_ID_TXT9	0x74787439
#define XA_ID_MOVI	0x4D4F5649
#define XA_ID_xSce	0x78536365
#define XA_ID_neEd	0x6e654564
#define XA_ID_xMov	0x784d6f76
#define XA_ID_ieSe	0x69655365
#define XA_ID_1b3	0x000001b3
#define XA_ID_1ba	0x000001ba
#define XA_ID_ARMo	0x41524D6F
#define XA_ID_vie	0x76696500
#define XA_ID_J_MO	0x6A5F6D6F
#define XA_ID_snd	0x2e736e64
#define XA_ID_RIFF	0x52494646
#define XA_ID_AVI	0x41564920
#define XA_ID_WAVE	0x57415645

#define BUFF_FILL_SIZE 0x20

extern xaULONG XA_BUF_Init_Fill();

#define BUFF_Extract_MSB32(xin,i) \
(  ((xaULONG)(xin->buf[i])<<24)  | ((xaULONG)(xin->buf[i+1])<<16) \
 | ((xaULONG)(xin->buf[i+2])<<8) | (xaULONG)(xin->buf[i+3]) )

xaULONG XA_Open_And_ID_File(anim)
XA_ANIM_HDR *anim;
{ xaLONG ret;
  xaULONG anim_type, lw0,lw1,lw2,lw3;
  XA_INPUT *xin = anim->xin;

  if (xin == 0)				return(NOFILE_ANIM);
  if (xin->Open_File(xin) == xaFALSE)	return(NOFILE_ANIM);
  ret = XA_BUF_Init_Fill(xin, BUFF_FILL_SIZE);
  if (ret < BUFF_FILL_SIZE)
  { xin->Close_File(xin);
    return(NOFILE_ANIM);
  }

  lw0 = BUFF_Extract_MSB32(xin,0x00);
  lw1 = BUFF_Extract_MSB32(xin,0x04);
  lw2 = BUFF_Extract_MSB32(xin,0x08);
  lw3 = BUFF_Extract_MSB32(xin,0x0c);

  anim_type = UNKNOWN_ANIM;
  switch(lw0)
  {
    case XA_ID_FORM:
    case XA_ID_LIST:
    case XA_ID_PROP:
	anim_type = IFF_ANIM;
	anim->desc = "IFF";
	anim->Read_File = IFF_Read_File;
	break;
    case XA_ID_GIF8:
	anim_type = GIF_ANIM;
	anim->desc = "GIF";
	anim->Read_File = GIF_Read_Anim;
	break;
    case XA_ID_TXT9:
	anim_type = TXT_ANIM;
	anim->desc = "TXT91";
	anim->Read_File = TXT_Read_File;
	break;
    case XA_ID_MOVI:
	anim_type = SGI_ANIM;
	anim->desc = "SGI Movie";
	anim->Read_File = MOVI_Read_File;
	break;
    case XA_ID_1b3:
    case XA_ID_1ba:
	anim_type = MPG_ANIM;
	anim->desc = "MPEG";
	anim->Read_File = MPG_Read_File;
	break;
    case XA_ID_snd:
	anim_type = AU_ANIM;
	anim->desc = "AU Audio";
	anim->Read_File = AU_Read_File;
	break;
    case XA_ID_RIFF:
        switch(lw2)
	{
	  case XA_ID_AVI:
		anim_type = AVI_ANIM;
		anim->desc = "RIFF AVI";
		anim->Read_File = AVI_Read_File;
		break;
	  case XA_ID_WAVE:
		anim_type = WAV_ANIM;
		anim->desc = "RIFF WAVE Audio";
		anim->Read_File = WAV_Read_File;
		break;
	}
	break;
    case XA_ID_ARMo:
	if ( (lw1 & 0xffffff00) == XA_ID_vie)
        {
	  anim_type = ARM_ANIM;
	  anim->desc = "Replay Movie";
	  anim->Read_File = ARM_Read_File;
	}
	break;
    case XA_ID_J_MO:
	if ( (lw1 & 0xffffff00) == XA_ID_vie)
        {
	  anim_type = JMOV_ANIM;
	  anim->desc = "JPEG Movie";
	  anim->Read_File = JMOV_Read_File;
	}
	break;
    case XA_ID_xSce:
    case XA_ID_xMov:
	if (   ((lw0 == XA_ID_xSce) && (lw1 == XA_ID_neEd))
	    || ((lw0 == XA_ID_xMov) && (lw1 == XA_ID_ieSe)) )
        {
	  anim_type = SET_ANIM;
	  anim->desc = "MovieSetter";
	  anim->Read_File = SET_Read_File;
	}
	break;
  }
  if (anim_type == UNKNOWN_ANIM)
  { xaULONG ls0 = (lw0 >> 16) & 0xffff;
    xaULONG ls2 = (lw1 >> 16) & 0xffff;
    if (ls0 == 0x52cc) /* RLE */
    {
	anim_type = RLE_ANIM;
	anim->desc = "Utah Raster RLE";
	anim->Read_File = RLE_Read_File;
    }
    else if (ls0 == 0xffd8) /* JFIF */
    { anim_type = JFIF_ANIM;
      anim->desc = "JFIF Image";
      anim->Read_File = JFIF_Read_File;
    }
    else if (ls2 == 0x11af)
    { anim_type = FLI_ANIM;
      anim->desc = "FLI";
      anim->Read_File = Fli_Read_File;
    }
    else if (ls2 == 0x12af)
    { anim_type = FLI_ANIM;
      anim->desc = "FLC";
      anim->Read_File = Fli_Read_File;
    }
    else if (lw1 == 0x52434847)  /* RCHG PageFlipper PFX */
    { anim_type = PFX_ANIM;
      anim->desc = "PageFlipper PFX";
      anim->Read_File = 0; /* PFX_Read_File; */
    }
    else
    {   /* POD failure if not random */
      if ( Is_QT_File(anim->name) == xaTRUE )
      { anim_type = QT_ANIM;
	anim->desc = "Quicktime";
	anim->Read_File = QT_Read_File;
      }
      else if ( Is_DL_File(xin) == xaTRUE)
      { anim_type = DL_ANIM;
	anim->desc = "DL Animation ";
	anim->Read_File = DL_Read_File;
      }
    }
  }
  return(anim_type);
}

xaULONG Is_DL_File(xin)
XA_INPUT *xin;
{ xaULONG ret = xaFALSE;
  switch( xin->buf[0] )
  { case 0x03:
    case 0x01: 
	ret = xaTRUE;
	break;
    case 0x02:
	  /*  0 <= x <= 3   since unsigned < 3 suffices */
	if (xin->buf[1] <= 0x03) ret = xaTRUE;
	break;
  }
  return(ret); 
}

/*
 * DL:   BYTE0 => [0x01 | 0x03]
 *     | BYTE0 => 0x02  && (BYTE1 => [0x00 | 0x01 | 0x02 | 0x03])
 *
 * PFX:  SKIP_LONG MSB_LONG => pfx_RCHG(0x52434847)
 * FLI:  SKIP_LONG LSB_SHORT => [0xaf11 | 0xaf12]
 * JFIF: MSB_SHORT 0xFF 0xD8
 * MPEG: MSB_LONG 0x000001b3 OR 0x000001ba
 * RLE:  MSB_SHORT RLE_MAGIC 0x52cc
 * AVI:  MSBLONG0 1 2  (0=>RIFF 0x52494646 3=>AVI  0x41564920
 * WAV:       """"""        "       "      3=>WAVE 0x57415645
 * ARM: 1st 7 bytes ARMovie 0x41524D6F 0x76696500
 * MOVI: MOVI 0x4D4F5649
 * SET:  "xSceneEd"itor 0x78536365 0x6e654564
 *     | "xMovieSe"tter 0x784d6f76 0x69655365
 * JMOV: 1st 7 bytes "j_movie" 0x6A5F6D6F 0x76696500 
 *
 *
 */






