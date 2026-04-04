
/*
 * xa_qt.c
 *
 * Copyright (C) 1993,1994,1995,1996 by Mark Podlipec.
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
/* REVISIONS ***********
 * 31Aug94  RPZA was using *iptr+=row_inc instead of iptr+=row_inc.
 * 15Sep94  Added support for RAW32 format. straight RGB with 0x00 up front.
 * 19Sep94  Fixed stereo audio bug. Needed to double bps with stereo snd.
 * 20Sep94  I forgot to declare fin in the QT_Read_Audio_STSD(fin) and
 *	    that caused problems on the Alpha machines.
 * 20Sep94  Added RAW4,RAW16,RAW24,RAW32,Gray CVID and Gray Other codecs.
 * 07Nov94  Fixed bug in RLE,RLE16,RLE24,RLE32 and RLE1 code, where I
 *          had improperly guessed what a header meant. Now it's a different
 *	    hopeful more correct guess.
 * 29Dec94  Above bug wasn't fixe in RLE(8bit), now it is.
 * 30Jan95  Appletalk on SONY uses directory .afprsrc for resource forks
 *	    instead of .resource like other programs use. 
 *	    patch written by Kazushi Yoshida.
 * 11Feb95  Fixed Bug with RLE depth 1 codec.
 * 04Mar95  Added Dithering(+F option) to Cinepak Video Codec.
 * 08Mar95  Fixed audio Bug with SGI generated quicktimes audio depth > 8.
 * 17Mar95  Fixed bug that was causing quicktime to erroneously send
 *          back a FULL_IM flag allowing serious skipping to occur. This
 *          causes on screen corruption if the Video Codec doesn't 
 *          really support serious skipping.
 * 11Apr95  Fixed bug in QT_Create_Gray_Cmap that caused last color of
 *	    colormap not to be generated correctly. Only affected
 *	    the Gray Quicktime codecs.
 * 16Jun95  Removed Cinepak Codec per Radius request.
 * 15Sep95  Code snippet to support erroneous Quicktime files that
 *	    have the length of the "mdat" atom, but not the "mdat" ID.
 * 15Sep95  Better check for truncated Quicktime files
 * 26Feb96  Fixed prob in Read_Video_Data, where timing chunks were
 *          incremented properly resulting in video/audio sync problems.
 *  1Mar96  Moved Video Codecs into xa_qt_decs.c file
 * 19Mar96  Modified for support of audio only files.
 */
#include "xa_qt.h"
#include "xa_codecs.h"

static XA_CODEC_HDR qt_codec_hdr;

/******************** Radius Cinepak External Library XAnim funtions ********/
#ifdef XA_CINEPAK
extern xaULONG  Cinepak_What_Rev_API();
extern xaLONG   Cinepak_Codec_Query();
#define XA_CINEPAK_QUERY_CNT 1
#else
#define XA_CINEPAK_QUERY_CNT 0
#endif


/******************** Intel Indeo External Library XAnim funtions ***********/
#ifdef XA_INDEO
extern xaULONG  Indeo_What_Rev_API();
extern xaLONG  Indeo_Codec_Query();
#define XA_INDEO_QUERY_CNT 1
#else
#define XA_INDEO_QUERY_CNT 0
#endif

/******************** Kodak Photo CD External Library XAnim funtions *********/
#ifdef XA_KPCD
extern xaULONG  KPCD_What_Rev_API();
extern xaLONG  KPCD_Codec_Query();
#define XA_KPCD_QUERY_CNT 1
#else
#define XA_KPCD_QUERY_CNT 0
#endif

extern xaLONG QT_Codec_Query();
extern xaLONG QT_UNK_Codec_Query();

#define QT_QUERY_CNT  2 + XA_INDEO_QUERY_CNT + XA_CINEPAK_QUERY_CNT \
			+ XA_KPCD_QUERY_CNT

xaLONG (*qt_query_func[])() = { 
#ifdef XA_INDEO
		Indeo_Codec_Query,
#endif
#ifdef XA_CINEPAK
		Cinepak_Codec_Query,
#endif
#ifdef XA_KPCD
		KPCD_Codec_Query,
#endif
		QT_Codec_Query,
		QT_UNK_Codec_Query};


xaULONG QT_Read_Video_Codec_HDR();
xaULONG QT_Read_Audio_Codec_HDR();
void QT_Audio_Type();
xaULONG QT_Read_File();


void QT_Create_Default_Cmap();
void QT_Create_Gray_Cmap();
extern char *XA_rindex();

void CMAP_Cache_Clear();
void CMAP_Cache_Init();


xaUSHORT qt_gamma_adj[32];

QTV_CODEC_HDR *qtv_codecs;
QTS_CODEC_HDR *qts_codecs;
xaULONG qtv_codec_num,qts_codec_num;

XA_ACTION *ACT_Get_Action();
XA_CHDR *ACT_Get_CMAP();
XA_CHDR *CMAP_Create_332();
XA_CHDR *CMAP_Create_422();
XA_CHDR *CMAP_Create_Gray();
void ACT_Add_CHDR_To_Action();
void ACT_Setup_Mapped();
XA_CHDR *CMAP_Create_CHDR_From_True();
xaUBYTE *UTIL_RGB_To_FS_Map();
xaUBYTE *UTIL_RGB_To_Map();
xaULONG CMAP_Find_Closest();
xaULONG UTIL_Get_MSB_Long();
xaLONG UTIL_Get_MSB_Short();
xaULONG UTIL_Get_MSB_UShort();
extern XA_ANIM_SETUP *XA_Get_Anim_Setup();
extern ACT_Setup_DeltaOLD();
void XA_Free_Anim_Setup();


FILE *QT_Open_File();
xaULONG QT_Parse_Chunks();
xaULONG QT_Parse_Bin();
xaULONG QT_Read_Video_Data();
xaULONG QT_Read_Audio_Data();

void QT_Print_ID();
void QT_Read_MVHD();
void QT_Read_TKHD();
void QT_Read_ELST();
void QT_Read_MDHD();
void QT_Read_HDLR();
xaULONG QT_Read_Video_STSD();
void QT_Read_Audio_STSD();
void QT_Read_Name();
void QT_Read_STTS();
void QT_Read_STSS();
void QT_Read_STCO();
void QT_Read_STSZ();
void QT_Read_STSC();
void QT_Read_STGS();
void QT_Free_Stuff();
void QT_Codec_List();



QT_MVHDR qt_mvhdr;
QT_TKHDR qt_tkhdr;
QT_MDHDR qt_mdhdr;
QT_HDLR_HDR qt_hdlr_hdr;

char qt_rfilename[256];
char qt_dfilename[256];
static xaULONG qt_video_flag;
static xaULONG qt_data_flag;
static xaULONG qt_v_flag, qt_s_flag;
static xaULONG qt_moov_flag;

static xaULONG qt_frame_cnt;
static xaULONG qt_mv_timescale,qt_md_timescale;
static xaULONG qt_vid_timescale, qt_aud_timescale;
static xaULONG qt_tk_timescale,qts_tk_timescale,qtv_tk_timescale;

#define QT_CODEC_UNK   0x000

/*** SOUND SUPPORT ****/
static xaULONG qt_audio_attempt;
static xaULONG qt_audio_type;
static xaULONG qt_audio_freq;
static xaULONG qt_audio_chans;
static xaULONG qt_audio_bps;
static xaULONG qt_audio_end;
xaULONG XA_Add_Sound();


QT_FRAME *qt_frame_start,*qt_frame_cur;
 
QT_FRAME *QT_Add_Frame(time,timelo,act)
xaULONG time,timelo;
XA_ACTION *act;
{
  QT_FRAME *fframe;
 
  fframe = (QT_FRAME *) malloc(sizeof(QT_FRAME));
  if (fframe == 0) TheEnd1("QT_Add_Frame: malloc err");
 
  fframe->time = time;
  fframe->timelo = timelo;
  fframe->act = act;
  fframe->next = 0;
 
  if (qt_frame_start == 0) qt_frame_start = fframe;
  else qt_frame_cur->next = fframe;
 
  qt_frame_cur = fframe;
  qt_frame_cnt++;
  return(fframe);
}
 
void QT_Free_Frame_List(fframes)
QT_FRAME *fframes;
{
  QT_FRAME *ftmp;
  while(fframes != 0)
  {
    ftmp = fframes;
    fframes = fframes->next;
    FREE(ftmp,0x9000);
  }
}


xaLONG Is_QT_File(filename)
char *filename;
{
  FILE *fin;
  xaULONG ret;

  if ( (fin=QT_Open_File(filename,qt_rfilename,qt_dfilename)) == 0)
				return(xaNOFILE);
  ret = QT_Parse_Bin(fin);
  fclose(fin);
  if ( ret != 0 ) return(xaTRUE);
  return(xaFALSE);
}

/* FOR PARSING Quicktime Files */
xaULONG *qtv_samp_sizes,*qts_samp_sizes;
xaULONG qtv_samp_num,qts_samp_num;
xaULONG qt_init_duration,qts_init_duration, qtv_init_duration;
xaULONG qt_start_offset,qts_start_offset, qtv_start_offset;

QT_S2CHUNK_HDR *qtv_s2chunks,*qts_s2chunks;
xaULONG qtv_s2chunk_num,qts_s2chunk_num;

QT_T2SAMP_HDR *qtv_t2samps,*qts_t2samps;
xaULONG qtv_t2samp_num,qts_t2samp_num;
 
xaULONG qtv_chunkoff_num,qts_chunkoff_num;
xaULONG *qtv_chunkoffs,*qts_chunkoffs;

xaULONG qtv_codec_lstnum,qts_codec_lstnum;
xaULONG qtv_chunkoff_lstnum,qts_chunkoff_lstnum;
xaULONG qtv_samp_lstnum,qts_samp_lstnum;
xaULONG qtv_s2chunk_lstnum,qts_s2chunk_lstnum;
xaULONG qt_stgs_num;

xaULONG qt_has_ctab;

/* main() */
xaULONG QT_Read_File(fname,anim_hdr,audio_attempt)
char *fname;
XA_ANIM_HDR *anim_hdr;
xaULONG audio_attempt;  /* xaTRUE is audio is to be attempted */
{ /* XA_INPUT *xin = anim_hdr->xin; */
  FILE *fin;
  xaLONG i,t_time;
  xaULONG t_timelo;
  XA_ANIM_SETUP *qt;
  xaULONG qt_has_audio, qt_has_video;


  qt = XA_Get_Anim_Setup();
  qt->vid_time 		= XA_GET_TIME( 100 ); /* default 10 fps */
  qt->compression	= QT_CODEC_UNK;


  qt_has_audio	= xaFALSE;
  qt_has_video	= xaFALSE;
  qt_has_ctab	= xaFALSE;
  qt_stgs_num	= 0;
  qtv_codec_lstnum	= qts_codec_lstnum = 0;
  qtv_chunkoff_lstnum	= qts_chunkoff_lstnum = 0;
  qtv_samp_lstnum	= qts_samp_lstnum = 0;
  qtv_codecs = 0;
  qts_codecs = 0;
  qtv_codec_num 	= qts_codec_num = 0;
  qt_data_flag = xaFALSE;
  qt_video_flag		= 0;
  qt_v_flag		= qt_s_flag = 0;
  qt_moov_flag = xaFALSE;

  qt_frame_cnt = 0;
  qt_frame_start = 0;
  qt_frame_cur = 0;

  qt_vid_timescale	= qt_aud_timescale = 1000;
  qt_mv_timescale	= qt_md_timescale = 1000;
  qts_tk_timescale 	= qtv_tk_timescale 	= 1000;
  qtv_chunkoff_num	= qts_chunkoff_num = 0;
  qtv_chunkoffs		= qts_chunkoffs = 0;
  qtv_s2chunk_num	= qts_s2chunk_lstnum = 0;
  qtv_s2chunks		= qts_s2chunks = 0;
  qtv_s2chunk_num	= qts_s2chunk_lstnum = 0;
  qtv_t2samp_num	= qts_t2samp_num = 0;
  qtv_t2samps		= qts_t2samps = 0;
  qtv_samp_sizes	= qts_samp_sizes = 0;
  qtv_samp_num		= qts_samp_num = 0;
  qt_init_duration	= qts_init_duration	= qtv_init_duration = 0;
  qt_start_offset 	= qts_start_offset	= qtv_start_offset = 0;

  qt_audio_attempt	= audio_attempt;

  for(i=0;i<32;i++) qt_gamma_adj[i] = xa_gamma_adj[ ((i<<3)|(i>>2)) ];

  if ( (fin=QT_Open_File(fname,qt_rfilename,qt_dfilename)) == 0)
  {
    fprintf(stdout,"QT_Read: can't open %s\n",qt_rfilename);
    XA_Free_Anim_Setup(qt);
    return(xaFALSE);
  }

  if ( QT_Parse_Bin(fin) == 0 )
  {
    fprintf(stdout,"Not quicktime file\n");
    XA_Free_Anim_Setup(qt);
    return(xaFALSE);
  }

  if (QT_Parse_Chunks(anim_hdr,qt,fin) == xaFALSE)
  {
    QT_Free_Stuff();
    XA_Free_Anim_Setup(qt);
    return(xaFALSE);
  }
  if (qt_data_flag == xaFALSE) 
  { /* mdat was not in .rscr file need to open .data file */
    fclose(fin); /* close .rscr file */
    if (qt_dfilename[0] == 0)
    { fprintf(stdout,"QT_Data: No data in %s file. Can't find .data file.\n",
		qt_rfilename);
      return(xaFALSE);
    }
    if ( (fin=fopen(qt_dfilename,XA_OPEN_MODE)) == 0) 
    { fprintf(stdout,"QT_Data: can't open %s file.\n",qt_dfilename);
      return(xaFALSE);
    }
  } else strcpy(qt_dfilename,qt_rfilename); /* r file is d file */

DEBUG_LEVEL1 fprintf(stdout,"reading data\n");

  if (qtv_samp_sizes)	qt_has_video = QT_Read_Video_Data(qt,fin,anim_hdr);
  if ((qts_samp_sizes) && (qt_audio_attempt==xaTRUE))
			qt_has_audio = QT_Read_Audio_Data(qt,fin,anim_hdr);
  fclose(fin);

  if ((qt_has_video == xaFALSE) || (qt_frame_cnt == 0))
  {
    if (qt_has_audio == xaFALSE)  /* no video and no audio */
    { if (qt_moov_flag==xaTRUE)
	fprintf(stdout,"QT: file possibly truncated.\n");
      else
	fprintf(stdout,"QT: file possibly truncated or missing .rsrc info.\n");
      return(xaFALSE);
    }

    if (qtv_samp_sizes)
	fprintf(stdout,"QT Notice: No supported Video frames - treating as audio only file\n");

    anim_hdr->total_time = (qt_mvhdr.duration * 1000) / (qt_mvhdr.timescale);
  }
  else  
  {
    anim_hdr->frame_lst = (XA_FRAME *)
                                malloc( sizeof(XA_FRAME) * (qt_frame_cnt+1));
    if (anim_hdr->frame_lst == NULL) TheEnd1("QT_Read_File: frame malloc err");
 
    qt_frame_cur = qt_frame_start;
    i = 0;
    t_time = 0;
    t_timelo = 0;
    while(qt_frame_cur != 0)
    { if (i >= qt_frame_cnt)
      { fprintf(stdout,"QT_Read_Anim: frame inconsistency %d %d\n",
                i,qt_frame_cnt); break;
      }
      anim_hdr->frame_lst[i].time_dur = qt_frame_cur->time;
      anim_hdr->frame_lst[i].zztime = t_time;
      t_time	+= qt_frame_cur->time;
      t_timelo	+= qt_frame_cur->timelo;
      while(t_timelo >= (1<<24)) {t_time++; t_timelo -= (1<<24);}
      anim_hdr->frame_lst[i].act = qt_frame_cur->act;
      qt_frame_cur = qt_frame_cur->next;
      i++;
    }
    if (i > 0)
    { anim_hdr->last_frame = i - 1;
      anim_hdr->total_time = anim_hdr->frame_lst[i-1].zztime
                                + anim_hdr->frame_lst[i-1].time_dur;
    }
    else { anim_hdr->last_frame = 0; anim_hdr->total_time = 0; }

    if (xa_verbose)
    { fprintf(stdout, "  Frame Stats: Size=%dx%d  Frames=%d",
				qt->imagex,qt->imagey,qt_frame_cnt);
      if (anim_hdr->total_time) 
      { float fps = (float)(1000 * qt_frame_cnt)/(float)(anim_hdr->total_time);
        fprintf(stdout, "  avfps=%2.1f\n",fps);
      } 
      else fprintf(stdout,"\n");
    }
    anim_hdr->imagex = qt->max_imagex;
    anim_hdr->imagey = qt->max_imagey;
    anim_hdr->imagec = qt->imagec;
    anim_hdr->imaged = 8; /* nop */
    anim_hdr->frame_lst[i].time_dur = 0;
    anim_hdr->frame_lst[i].zztime = -1;
    anim_hdr->frame_lst[i].act  = 0;
    anim_hdr->loop_frame = 0;
  }

  if (xa_buffer_flag == xaFALSE) anim_hdr->anim_flags |= ANIM_SNG_BUF;
  anim_hdr->max_fvid_size = qt->max_fvid_size;
  anim_hdr->max_faud_size = qt->max_faud_size;
  if (xa_file_flag == xaTRUE) 
  { xaULONG len;
    anim_hdr->anim_flags |= ANIM_USE_FILE;
    len = strlen(qt_dfilename) + 1;
    anim_hdr->fname = (char *)malloc(len);
    if (anim_hdr->fname==0) TheEnd1("QT: malloc fname err");
    strcpy(anim_hdr->fname,qt_dfilename);
  }
  QT_Free_Stuff();
  XA_Free_Anim_Setup(qt);
  return(xaTRUE);
}

void QT_Free_Stuff()
{
  QT_Free_Frame_List(qt_frame_start);
  if (qtv_samp_sizes) FREE(qtv_samp_sizes,0x9003);
  if (qts_samp_sizes) FREE(qts_samp_sizes,0x9004);
  if (qtv_codecs) FREE(qtv_codecs,0x9005);
  if (qts_codecs) FREE(qts_codecs,0x9006);
  if (qtv_t2samps) FREE(qtv_t2samps,0x9007);
  if (qts_t2samps) FREE(qts_t2samps,0x9008);
  if (qtv_s2chunks) FREE(qtv_s2chunks,0x9009);
  if (qts_s2chunks) FREE(qts_s2chunks,0x900a);
  if (qtv_chunkoffs) FREE(qtv_chunkoffs,0x900b);
  if (qts_chunkoffs) FREE(qts_chunkoffs,0x900c);
}

FILE *QT_Open_File(fname,r_file,d_file)
char *fname,*r_file,*d_file;
{
  FILE *fin;

  /* check to see if fname exists? */
  if ( (fin=fopen(fname,XA_OPEN_MODE)) != 0)  /* filename is as give */
  { /*three choices - with or without .rsrc ending, or using .resource subdir*/
    xaLONG len;
    FILE *ftst;
    /* path/fname exits. */

    /* check for  path/.resource/fname */
    {
      char *lastdirsep;
      strcpy(r_file,fname);			/* copy path/fname to r */
      lastdirsep = XA_rindex(r_file, '/');	/* find sep if any */
      if ( lastdirsep != (char *)(NULL) )
      {
        strcpy(d_file,lastdirsep);		/* save fname to d*/
	lastdirsep++; *lastdirsep = 0;		/* cut of fname off r*/
/*POD NOTE: eventually might want to check for both, but for now
 *          let's just wait and see. */
#ifdef sony
	/* For AppleTalk of NEWS-OS, .rsrc file is in .afprsrc directory */
	strcat(lastdirsep, ".afprsrc/");	/* add .afprsrc to r*/
#else
	strcat(lastdirsep, ".resource/");     /* add .resource to r*/
#endif
        strcat(r_file, d_file); 		/* add fname to r */
      }
      else /* no path */
      {
#ifdef sony
	strcpy(r_file,".afprsrc/");
#else
	strcpy(r_file,".resource/");
#endif
	strcat(r_file,fname);
      }
      if ( (ftst=fopen(r_file,"r")) != 0)
      {
	/* path/fname and path/.resource/fname exist - wrap it up */
	strcpy(d_file,fname);			/* setup .data name */
	fclose(fin);				/* close .data fork */
        return(ftst);		/* return .rsrc fork (in .resource) */
      }
    }
     
    /* Now check for .rsrc or .data endings */
    strcpy(r_file,fname);
    strcpy(d_file,fname);
    len = strlen(r_file) - 5;
    if (len > 0)
    { char *tmp;
      tmp = XA_rindex(d_file, '.');	/* get last "." */
      if ( tmp == (char *)(NULL) ) { *d_file = 0; return(fin); }
      else if (strcmp(tmp,".rsrc")==0)  /* fname has .rsrc ending */
      {
        strcpy(tmp,".data"); /* overwrite .rsrc with .data in d*/
	return(fin);
      }
      else if (strcmp(tmp,".data")==0)  /* fname has .data ending */
      {
        strcpy(tmp,".rsrc"); /* overwrite .rsrc with .data in d*/
        if ( (ftst=fopen(d_file,"r")) != 0) /* see if .rsrc exists */
	{
	  char t_file[256];  /* swap r and d files */
	  strcpy(t_file,r_file); strcpy(r_file,d_file); strcpy(d_file,t_file);
	  fclose(fin);		/* close .data file */
	  return(ftst);		/* return .rsrc file */
	}
	/* hopefully .data is flattened. find out later */
	else { *d_file = 0; return(fin); }
      }
      else { *d_file = 0; return(fin); }
    }
    else { *d_file = 0; return(fin); }
  }

  /* does fname.rsrc exist? */
  strcpy(r_file,fname);
  strcat(r_file,".rsrc");
  if ( (fin=fopen(r_file,XA_OPEN_MODE)) != 0)  /* fname.rsrc */
  { FILE *ftst;
    /* if so, check .data existence */
    strcpy(d_file,fname);
    strcat(d_file,".data");
    if ( (ftst=fopen(d_file,XA_OPEN_MODE)) != 0)	fclose(ftst);
    else *d_file = 0;
    return(fin);
  } else *d_file = 0;
  return(0);
}

xaULONG QT_Parse_Chunks(anim_hdr,qt,fin)
XA_ANIM_HDR *anim_hdr;
XA_ANIM_SETUP *qt;
FILE *fin;
{
  xaLONG file_len;
  xaULONG id,len;

  file_len = 1;
  while(file_len > 0)
  {
    len = UTIL_Get_MSB_Long(fin);
    id  = UTIL_Get_MSB_Long(fin);

    if ( (len == 0) && (id == QT_mdat) )
    {
      fprintf(stdout,"QT: mdat len is 0. You also need a .rsrc file.\n");
      return(xaFALSE);
    }
    if (len < 8) { file_len = 0; continue; } /* just bad - finish this */
    if (file_len == 1)
    {
      if (id == QT_moov) file_len = len + 8;
      else file_len = len + 8;
    }
    DEBUG_LEVEL2 fprintf(stdout,"%c%c%c%c %04x len = %x file_len =  %x\n",
	(char)(id >> 24),(char)((id>>16)&0xff),
	(char)((id>>8)&0xff),(char)(id&0xff),id,len,file_len);

    switch(id)
    {
    /*--------------ATOMS------------------*/
      case QT_trak:
	qt_v_flag = qt_s_flag = 0;
	qtv_codec_lstnum = qtv_codec_num;
	qts_codec_lstnum = qts_codec_num;
	qtv_chunkoff_lstnum = qtv_chunkoff_num;
	qts_chunkoff_lstnum = qts_chunkoff_num;
	qtv_samp_lstnum = qtv_samp_num;
	qts_samp_lstnum = qts_samp_num;
	qtv_s2chunk_lstnum = qtv_s2chunk_num;
	qts_s2chunk_lstnum = qts_s2chunk_num;
	file_len -= 8;
	break;
      case QT_moov:
        qt_moov_flag = xaTRUE;
	file_len -= 8;
	break;
      case QT_mdia:
      case QT_minf:
      case QT_stbl:
      case QT_edts:
	file_len -= 8;
	break;
    /*---------------STUFF------------------*/
      case QT_mvhd:
	QT_Read_MVHD(fin,&qt_mvhdr);
	file_len -= len;
	break;
      case QT_tkhd:
	QT_Read_TKHD(fin,&qt_tkhdr);
	file_len -= len;
	break;
      case QT_elst:
	QT_Read_ELST(fin,&qt_start_offset,&qt_init_duration);
	file_len -= len;
	break;
      case QT_mdhd:
	QT_Read_MDHD(fin,&qt_mdhdr);
	file_len -= len;
	break;
      case QT_hdlr:
	QT_Read_HDLR(fin,len,&qt_hdlr_hdr);
	file_len -= len;
	break;
    /*--------------DATA CHUNKS-------------*/
      case QT_mdat:  /* data is included in .rsrc - assumed flatness */
	fseek(fin,(len-8),1); /* skip over it for now */
	qt_data_flag = xaTRUE;
	break;
      case QT_stsd:
	qt_video_flag = 0;
	if (qt_v_flag) 
	{
	  if (QT_Read_Video_STSD(anim_hdr,qt,fin) == xaFALSE) return(xaFALSE);
	}
	else if (qt_s_flag) QT_Read_Audio_STSD(fin);
        else fseek(fin,(len-8),1);
	file_len -= len;
	break;
      case QT_stts:
	if (qt_v_flag) 
		QT_Read_STTS(fin,(len-8),&qtv_t2samp_num,&qtv_t2samps);
/*POD NOTE: AUDIO don't need? probably, just haven't been bit by it yet.
	else if (qt_s_flag) 
		QT_Read_STTS(fin,(len-8),&qts_t2samp_num,&qts_t2samps);
*/
        else	fseek(fin,(len-8),1);
	file_len -= len;
	break;
      case QT_stss:
	QT_Read_STSS(fin);
	file_len -= len;
	break;
      case QT_stco:
	if (qt_v_flag) QT_Read_STCO(fin,&qtv_chunkoff_num,&qtv_chunkoffs);
	else if (qt_s_flag) QT_Read_STCO(fin,&qts_chunkoff_num,&qts_chunkoffs);
	else fseek(fin,(len-8),1);
	file_len -= len;
	break;
      case QT_stsz:
	if (qt_v_flag) QT_Read_STSZ(fin,len,&qtv_samp_num,&qtv_samp_sizes);
	else if (qt_s_flag) 
		QT_Read_STSZ(fin,len,&qts_samp_num,&qts_samp_sizes);
	else fseek(fin,(len-8),1);
	file_len -= len;
	break;
      case QT_stsc:
	if (qt_v_flag) QT_Read_STSC(fin,len,&qtv_s2chunk_num,&qtv_s2chunks,
			qtv_chunkoff_lstnum,qtv_codec_num,qtv_codec_lstnum);
	else if (qt_s_flag) QT_Read_STSC(fin,len,&qts_s2chunk_num,&qts_s2chunks,
			qts_chunkoff_lstnum,qts_codec_num,qts_codec_lstnum);
	else fseek(fin,(len-8),1);
	file_len -= len;
	break;
      case QT_stgs:
	QT_Read_STGS(fin,len);
	file_len -= len;
	break;
    /*-----------Sound Codec Headers--------------*/
      case QT_raw00:
    /*-----------Video/Sound Codec Headers--------------*/
      case QT_raw:
    /*-----------Video Codec Headers--------------*/
      case QT_smc:
      case QT_rpza:
      case QT_rle:
      case QT_cvid:
	fprintf(stdout,"QT: Warning %08x\n",id);
        fseek(fin,(len-8),1);  /* skip over */
	file_len -= len;
	break;
    /*-----------TYPE OF TRAK---------------*/
      case QT_vmhd:
        fseek(fin,(len-8),1);
	file_len -= len; qt_v_flag = 1;
	qt_vid_timescale = qt_md_timescale;
	qtv_tk_timescale = qt_tk_timescale;
	qtv_init_duration = qt_init_duration; qt_init_duration = 0;
	qtv_start_offset = qt_start_offset; qt_start_offset = 0;
	break;
      case QT_smhd:
        fseek(fin,(len-8),1);
	file_len -= len; qt_s_flag = 1;
	qt_aud_timescale = qt_md_timescale;
	qts_tk_timescale = qt_tk_timescale;
	qts_init_duration = qt_init_duration; qt_init_duration = 0;
	qts_start_offset = qt_start_offset; qt_start_offset = 0;
	break;
    /************ CTAB ******************
     * Color Table to be used for display 16/24 bit animations on
     * 8 Bit displays.
     *************************************/
      case QT_ctab:
	{ xaULONG i,tmp,start,end;
          if (x11_display_type != XA_PSEUDOCOLOR)
	  {
	     while(len > 0) {fgetc(fin); len--; } 
	     break;
	  }
if (xa_verbose) fprintf(stdout,"QT: has ctab\n");
          tmp   = UTIL_Get_MSB_Long(fin);  /* ?? */
          start = UTIL_Get_MSB_UShort(fin);  /* start */
          end   = UTIL_Get_MSB_UShort(fin);  /* end */
	  len -= 8;
	  for(i=start; i <= end; i++)
	  { xaULONG idx,r,g,b;
	    idx = UTIL_Get_MSB_UShort(fin);
	    r   = UTIL_Get_MSB_UShort(fin);
	    g   = UTIL_Get_MSB_UShort(fin);
	    b   = UTIL_Get_MSB_UShort(fin);  len -= 8;
	    /* if (cflag & 0x8000)  idx = i; */
	    if (idx < qt->imagec)
	    {
	      qt->cmap[idx].red   = r;
	      qt->cmap[idx].green = g;
	      qt->cmap[idx].blue  = b;
	    }
	    if (len <= 0) break;
	  } /* end of for i */
	  while(len > 0) {fgetc(fin); len--; } 
	  qt->imagec = 256;
	  qt->chdr = ACT_Get_CMAP(qt->cmap,qt->imagec,0,qt->imagec,0,8,8,8);
	  qt_has_ctab = xaTRUE;
	}
	break;
    /*--------------IGNORED FOR NOW---------*/
      case QT_gmhd:
      case QT_text:
      case QT_clip:
      case QT_skip:
      case QT_udta:
      case QT_dinf:
        fseek(fin,(len-8),1);  /* skip over */
	file_len -= len;
	break;
    /*--------------UNKNOWN-----------------*/
      default:
	if ( !feof(fin) && (len <= file_len) )
	{
	  xaLONG i;
	  QT_Print_ID(stdout,id,1);
	  fprintf(stdout," len = %x\n",len);
	  i = (xaLONG)(len) - 8;
	  while(i > 0) { i--; getc(fin); if (feof(fin)) i = 0; }
	}
	file_len -= len;
	break;
    } /* end of switch */
    if ( feof(fin) ) 
    {
      file_len = 0;
      if ((qt_moov_flag == xaFALSE) && (qt_data_flag == xaTRUE))
      { fprintf(stdout,"QT: file possibly truncated or missing .rsrc info.\n");
	return(xaFALSE);
      }
    }
  } /* end of while */
  return(xaTRUE);
}

void QT_Print_ID(fout,id,flag)
FILE *fout;
xaLONG id,flag;
{ fprintf(fout,"%c",     (char)((id >> 24) & 0xff));
  fprintf(fout,"%c",     (char)((id >> 16) & 0xff));
  fprintf(fout,"%c",     (char)((id >>  8) & 0xff));
  fprintf(fout,"%c",     (char) (id        & 0xff));
  if (flag) fprintf(fout,"(%x)",id);
}


void QT_Read_MVHD(fin,qt_mvhdr)
FILE *fin;
QT_MVHDR *qt_mvhdr;
{
  xaULONG i,j;

  qt_mvhdr->version =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->creation =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->modtime =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->timescale =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->duration =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->rate =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->volume =	UTIL_Get_MSB_UShort(fin);
  qt_mvhdr->r1  =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->r2  =	UTIL_Get_MSB_Long(fin);
  for(i=0;i<3;i++) for(j=0;j<3;j++) 
	qt_mvhdr->matrix[i][j]=UTIL_Get_MSB_Long(fin);
  qt_mvhdr->r3  =	UTIL_Get_MSB_UShort(fin);
  qt_mvhdr->r4  =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->pv_time =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->post_time =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->sel_time =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->sel_durat =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->cur_time =	UTIL_Get_MSB_Long(fin);
  qt_mvhdr->nxt_tk_id =	UTIL_Get_MSB_Long(fin);

  if (qt_mvhdr->timescale) qt_mv_timescale = qt_mvhdr->timescale;
  else qt_mv_timescale = 1000;
  qt_vid_timescale = qt_mv_timescale;

  
  DEBUG_LEVEL2
  { fprintf(stdout,"mv   ver = %x  timescale = %x  duration = %x\n",
	qt_mvhdr->version,qt_mvhdr->timescale, qt_mvhdr->duration);
    fprintf(stdout,"     rate = %x volume = %x  nxt_tk = %x\n",
	qt_mvhdr->rate,qt_mvhdr->volume,qt_mvhdr->nxt_tk_id);
  }
}

void QT_Read_TKHD(fin,qt_tkhdr)
FILE *fin;
QT_TKHDR *qt_tkhdr;
{ xaULONG i,j;

  qt_tkhdr->version =	UTIL_Get_MSB_Long(fin);
  qt_tkhdr->creation =	UTIL_Get_MSB_Long(fin);
  qt_tkhdr->modtime =	UTIL_Get_MSB_Long(fin);
  qt_tkhdr->trackid =	UTIL_Get_MSB_Long(fin);
  qt_tkhdr->timescale =	UTIL_Get_MSB_Long(fin);
  qt_tkhdr->duration =	UTIL_Get_MSB_Long(fin);
  qt_tkhdr->time_off =	UTIL_Get_MSB_Long(fin);
  qt_tkhdr->priority  =	UTIL_Get_MSB_Long(fin);
  qt_tkhdr->layer  =	UTIL_Get_MSB_UShort(fin);
  qt_tkhdr->alt_group = UTIL_Get_MSB_UShort(fin);
  qt_tkhdr->volume  =	UTIL_Get_MSB_UShort(fin);
  for(i=0;i<3;i++) for(j=0;j<3;j++) 
			qt_tkhdr->matrix[i][j]=UTIL_Get_MSB_Long(fin);
  qt_tkhdr->tk_width =	UTIL_Get_MSB_Long(fin);
  qt_tkhdr->tk_height =	UTIL_Get_MSB_Long(fin);
  qt_tkhdr->pad  =	UTIL_Get_MSB_UShort(fin);

  if (qt_tkhdr->timescale) qt_tk_timescale = qt_tkhdr->timescale;
  else qt_tk_timescale = qt_mv_timescale;

  DEBUG_LEVEL2
  { fprintf(stdout,"tk   ver = %x  tk_id = %x  timescale = %x\n",
	qt_tkhdr->version,qt_tkhdr->trackid,qt_tkhdr->timescale);
    fprintf(stdout,"     dur= %x timoff= %x tk_width= %x  tk_height= %x\n",
	qt_tkhdr->duration,qt_tkhdr->time_off,qt_tkhdr->tk_width,qt_tkhdr->tk_height);
  }
}


/* PODPOD */
/* implement different between start_offset and init_duration */
void QT_Read_ELST(fin,qt_start_offset,qt_init_duration)
FILE *fin;
xaULONG  *qt_start_offset;
xaULONG  *qt_init_duration;
{ xaULONG i,num,version;

 version = UTIL_Get_MSB_Long(fin);
 num = UTIL_Get_MSB_Long(fin);
 DEBUG_LEVEL2 fprintf(stdout,"    ELST ver %x num %x\n",version,num);
 for(i=0; i < num; i++)
 { xaULONG duration,time,rate,pad;
   duration = UTIL_Get_MSB_Long(fin); 
   time = UTIL_Get_MSB_Long(fin); 
   rate = UTIL_Get_MSB_UShort(fin); 
   pad  = UTIL_Get_MSB_UShort(fin); 

/* This is currently a kludge with limited support */
   if (i==0)
   {  if (time == 0xffffffff)	*qt_init_duration += duration;
      else if (time != 0x0)	*qt_start_offset += time;
   }
   DEBUG_LEVEL2 fprintf(stdout,"    -) dur %x tim %x rate %x\n",
		duration,time,rate);
 }
}

void QT_Read_MDHD(fin,qt_mdhdr)
FILE *fin;
QT_MDHDR *qt_mdhdr;
{ qt_mdhdr->version =	UTIL_Get_MSB_Long(fin);
  qt_mdhdr->creation =	UTIL_Get_MSB_Long(fin);
  qt_mdhdr->modtime =	UTIL_Get_MSB_Long(fin);
  qt_mdhdr->timescale =	UTIL_Get_MSB_Long(fin);
  qt_mdhdr->duration =	UTIL_Get_MSB_Long(fin);
  qt_mdhdr->language =	UTIL_Get_MSB_UShort(fin);
  qt_mdhdr->quality =	UTIL_Get_MSB_UShort(fin);

  if (qt_mdhdr->timescale) qt_md_timescale = qt_mdhdr->timescale;
  else qt_md_timescale = qt_tk_timescale;

  DEBUG_LEVEL2
  { fprintf(stdout,"md   ver = %x  timescale = %x  duration = %x\n",
	qt_mdhdr->version,qt_mdhdr->timescale,qt_mdhdr->duration);
    fprintf(stdout,"     lang= %x quality= %x\n", 
	qt_mdhdr->language,qt_mdhdr->quality);
  }
}


void QT_Read_HDLR(fin,len,qt_hdlr_hdr)
FILE *fin;
xaLONG len;
QT_HDLR_HDR *qt_hdlr_hdr;
{ qt_hdlr_hdr->version =	UTIL_Get_MSB_Long(fin);
  qt_hdlr_hdr->type =	UTIL_Get_MSB_Long(fin);
  qt_hdlr_hdr->subtype =	UTIL_Get_MSB_Long(fin);
  qt_hdlr_hdr->vendor =	UTIL_Get_MSB_Long(fin);
  qt_hdlr_hdr->flags =	UTIL_Get_MSB_Long(fin);
  qt_hdlr_hdr->mask =	UTIL_Get_MSB_Long(fin);

  DEBUG_LEVEL2
  { fprintf(stdout,"     ver = %x  ",qt_hdlr_hdr->version);
    QT_Print_ID(stdout,qt_hdlr_hdr->type,1);
    QT_Print_ID(stdout,qt_hdlr_hdr->subtype,1);
    QT_Print_ID(stdout,qt_hdlr_hdr->vendor,0);
    fprintf(stdout,"\n     flags= %x mask= %x\n",
	qt_hdlr_hdr->flags,qt_hdlr_hdr->mask);
  }
  /* Read Handler Name if Present */
  if (len > 32)
  { len -= 32;
    QT_Read_Name(fin,len);
  }
}


/*********************************************
 * Read and Parse Video Codecs
 *
 **********/
xaULONG QT_Read_Video_STSD(anim_hdr,qt,fin)
XA_ANIM_HDR *anim_hdr;
XA_ANIM_SETUP *qt;
FILE *fin;
{ xaULONG i,version,num,cur,sup;
  version = UTIL_Get_MSB_Long(fin);
  num = UTIL_Get_MSB_Long(fin);
  DEBUG_LEVEL2 fprintf(stdout,"     ver = %x  num = %x\n", version,num);
  if (qtv_codecs == 0)
  { qtv_codec_num = num;
    qtv_codecs = (QTV_CODEC_HDR *)malloc(qtv_codec_num * sizeof(QTV_CODEC_HDR));
    if (qtv_codecs==0) TheEnd1("QT STSD: malloc err");
    cur = 0;
  }
  else
  { QTV_CODEC_HDR *tcodecs;
    tcodecs = (QTV_CODEC_HDR *)malloc((qtv_codec_num+num) * sizeof(QTV_CODEC_HDR));
    if (tcodecs==0) TheEnd1("QT STSD: malloc err");
    for(i=0;i<qtv_codec_num;i++) tcodecs[i] = qtv_codecs[i];
    cur = qtv_codec_num;
    qtv_codec_num += num;
    FREE(qtv_codecs,0x900d);
    qtv_codecs = tcodecs;
  }
  sup = 0;
  for(i=0; i < num; i++)
  {
    sup |= QT_Read_Video_Codec_HDR(anim_hdr,qt, &qtv_codecs[cur], fin ); 
DEBUG_LEVEL1 fprintf(stdout,"CODEC %d) sup=%d\n",i,sup);
    cur++;
  }
  if (sup == 0) return(xaFALSE);
  qt_video_flag = 1; 
  if ( (qt->pic==0) && (xa_buffer_flag == xaTRUE))
  {
    qt->pic_size = qt->max_imagex * qt->max_imagey;
    if ( (cmap_true_map_flag == xaTRUE) && (qt->depth > 8) )
		qt->pic = (xaUBYTE *) malloc(3 * qt->pic_size);
    else	qt->pic = (xaUBYTE *) malloc( XA_PIC_SIZE(qt->pic_size) );
    if (qt->pic == 0) TheEnd1("QT_Buffer_Action: malloc failed");
  }
  return(xaTRUE);
}


xaULONG QT_Read_Video_Codec_HDR(anim_hdr,qt,c_hdr,fin)
XA_ANIM_HDR *anim_hdr;
XA_ANIM_SETUP *qt;
QTV_CODEC_HDR *c_hdr;
FILE *fin;
{
  xaULONG id;
  xaLONG len,i,codec_ret;
  xaULONG unk_0,unk_1,unk_2,unk_3,unk_4,unk_5,unk_6,unk_7,flag;
  xaULONG vendor,temp_qual,spat_qual,h_res,v_res;
  
  len		= UTIL_Get_MSB_Long(fin);
  id 		= UTIL_Get_MSB_Long(fin);
  unk_0		= UTIL_Get_MSB_Long(fin);
  unk_1		= UTIL_Get_MSB_Long(fin);
  unk_2		= UTIL_Get_MSB_UShort(fin);
  unk_3		= UTIL_Get_MSB_UShort(fin);
  vendor	= UTIL_Get_MSB_Long(fin);
  temp_qual	= UTIL_Get_MSB_Long(fin);
  spat_qual	= UTIL_Get_MSB_Long(fin);
  qt->imagex	= UTIL_Get_MSB_UShort(fin);
  qt->imagey	= UTIL_Get_MSB_UShort(fin);
  h_res		= UTIL_Get_MSB_UShort(fin);
  unk_4		= UTIL_Get_MSB_UShort(fin);
  v_res		= UTIL_Get_MSB_UShort(fin);
  unk_5		= UTIL_Get_MSB_UShort(fin);
  unk_6		= UTIL_Get_MSB_Long(fin);
  unk_7		= UTIL_Get_MSB_UShort(fin);
  QT_Read_Name(fin,32);
  qt->depth	= UTIL_Get_MSB_UShort(fin);
  flag		= UTIL_Get_MSB_UShort(fin);
  len -= 0x56;

  /* init now in case of early out */
  c_hdr->width	= qt->imagex;
  c_hdr->height	= qt->imagey;
  c_hdr->depth	= qt->depth;
  c_hdr->compression = id;
  c_hdr->chdr	= 0;

  if (   (qt->depth == 8) || (qt->depth == 40)
      || (qt->depth == 4) || (qt->depth == 36) ) /* generate colormap */
  {
    if (qt->depth & 0x04) qt->imagec = 16;
    else qt->imagec = 256;

    if (qt->depth < 32) QT_Create_Default_Cmap(qt->cmap,qt->imagec);
    else /* grayscale */
    {
      if ((id == QT_jpeg) || (id == QT_cvid)) 
		QT_Create_Gray_Cmap(qt->cmap,1,qt->imagec);
      else	QT_Create_Gray_Cmap(qt->cmap,0,qt->imagec);
    }

    if ( !(flag & 0x08) && (len) ) /* colormap isn't default */
    {
      xaULONG start,end,p,r,g,b,cflag;
DEBUG_LEVEL1 fprintf(stdout,"reading colormap. flag %x\n",flag);
      start = UTIL_Get_MSB_Long(fin); /* is this start or something else? */
      cflag = UTIL_Get_MSB_UShort(fin); /* is this end or total number? */
      end   = UTIL_Get_MSB_UShort(fin); /* is this end or total number? */
      len -= 8;
DEBUG_LEVEL1 fprintf(stdout," start %x end %x cflag %x\n",start,end,cflag);
      for(i = start; i <= end; i++)
      {
        p = UTIL_Get_MSB_UShort(fin); 
        r = UTIL_Get_MSB_UShort(fin); 
        g = UTIL_Get_MSB_UShort(fin); 
        b = UTIL_Get_MSB_UShort(fin);  len -= 8;
        if (cflag & 0x8000) p = i;
        if (p<qt->imagec)
        {
	  qt->cmap[p].red   = r;
	  qt->cmap[p].green = g;
	  qt->cmap[p].blue  = b;
        }
        if (len <= 0) break;
      }
    } 
  }

  while(len > 0) {fgetc(fin); len--; }

  qt->compression = qt_codec_hdr.compression = id;
  qt_codec_hdr.x = qt->imagex;
  qt_codec_hdr.y = qt->imagey;
  qt_codec_hdr.depth = qt->depth;
  qt_codec_hdr.anim_hdr = (void *)anim_hdr;
  qt_codec_hdr.avi_ctab_flag = xaFALSE;

  /* Query to see if Video Compression is supported or not */
  { xaULONG q = 0;
    while(q < (QT_QUERY_CNT) )
    {
      codec_ret = qt_query_func[q](&qt_codec_hdr);
      if (codec_ret == CODEC_SUPPORTED)
      {
	qt->imagex = qt_codec_hdr.x;
	qt->imagey = qt_codec_hdr.y;
	qt->depth  = qt_codec_hdr.depth;
	qt->compression = qt_codec_hdr.compression;
	c_hdr->xapi_rev	= qt_codec_hdr.xapi_rev;
	c_hdr->decoder	= qt_codec_hdr.decoder;
	c_hdr->depth  = qt_codec_hdr.depth;		/* depth */
	c_hdr->dlta_extra	= qt_codec_hdr.extra;
	break;
      }
      else if (codec_ret == CODEC_UNSUPPORTED) break;
      q++;
    }
  }

  /*** Return False if Codec is Unknown or Not Supported */
  if (codec_ret != CODEC_SUPPORTED)
  { char tmpbuf[256];
    if (codec_ret == CODEC_UNKNOWN)
    { xaULONG ii,a[4],dd = qt->compression;
      for(ii=0; ii<4; ii++)
      { a[ii] = dd & 0xff;  dd >>= 8;
        if ((a[ii] < ' ') || (a[ii] > 'z')) a[ii] = '.';
      }
      sprintf(tmpbuf,"Unknown %c%c%c%c(%08x)",
		(char)a[3],(char)a[2],(char)a[1],(char)a[0],qt->compression);
      qt_codec_hdr.description = tmpbuf;
    }
    if (xa_verbose) 
		fprintf(stdout,"  Video Codec: %s",qt_codec_hdr.description);
    else	fprintf(stdout,"QT Video Codec: %s",qt_codec_hdr.description);
    fprintf(stdout," is unsupported by this executable.(E%x)\n",qt->depth);
    return(0);
  }
  if (qt->depth == 40) qt->depth = 8; 

  /* Print out Video Codec info */
  if (xa_verbose) fprintf(stdout,"  Video Codec: %s depth=%d\n",
                                qt_codec_hdr.description,qt->depth);

 
  if (qt->depth == 1)
  {
    qt->imagec = 2;
    qt->cmap[0].red = qt->cmap[0].green = qt->cmap[0].blue = 0; 
    qt->cmap[1].red = qt->cmap[1].green = qt->cmap[1].blue = 0xff; 
    qt->chdr = ACT_Get_CMAP(qt->cmap,qt->imagec,0,qt->imagec,0,8,8,8);
  }
  else if ((qt->depth == 8) || (qt->depth == 4))
  {
    qt->chdr = ACT_Get_CMAP(qt->cmap,qt->imagec,0,qt->imagec,0,16,16,16);
  }
  else if ( (qt->depth >= 16) && (qt->depth <= 32) )
  {
    if (   (cmap_true_map_flag == xaFALSE) /* depth 16 and not true_map */
           || (xa_buffer_flag == xaFALSE) )
    {
      if (cmap_true_to_332 == xaTRUE)
		qt->chdr = CMAP_Create_332(qt->cmap,&qt->imagec);
      else	qt->chdr = CMAP_Create_Gray(qt->cmap,&qt->imagec);
    }
    else { qt->imagec = 0; qt->chdr = 0; }
  }
  c_hdr->width	= qt->imagex;
  c_hdr->height	= qt->imagey;
  c_hdr->depth	= qt->depth;
  c_hdr->compression = qt->compression;
  c_hdr->chdr	= qt->chdr;
  if (qt->imagex > qt->max_imagex) qt->max_imagex = qt->imagex;
  if (qt->imagey > qt->max_imagey) qt->max_imagey = qt->imagey;
  return(1);
}

void QT_Read_Name(fin,r_len)
FILE *fin;
xaLONG r_len;
{
  xaULONG len,d,i;

  len = fgetc(fin); r_len--;
  if (r_len == 0) r_len = len;
  if (len > r_len) fprintf(stdout,"QT_Name: len(%d) > r_len(%d)\n",len,r_len);
  DEBUG_LEVEL2 fprintf(stdout,"      (%d/%d) ",len,r_len);
  for(i=0;i<r_len;i++)
  {
    d = fgetc(fin) & 0x7f;
    if (i < len) DEBUG_LEVEL2 fputc(d,stdout);
  }
  DEBUG_LEVEL2 fputc('\n',stdout);
}

/* Time To Sample */
void QT_Read_STTS(fin,len,qt_t2samp_num,qt_t2samps)
FILE *fin;
xaLONG len;
xaULONG *qt_t2samp_num;
QT_T2SAMP_HDR **qt_t2samps;
{
  xaULONG version,num,i,samp_cnt,duration,cur; 
  xaULONG t2samp_num = *qt_t2samp_num;
  QT_T2SAMP_HDR *t2samps = *qt_t2samps;

  version	= UTIL_Get_MSB_Long(fin);
  num		= UTIL_Get_MSB_Long(fin);  len -= 8;
  DEBUG_LEVEL2 fprintf(stdout,"    ver=%x num of entries = %x\n",version,num);
/* POD TEST chunk len/num mismatch - deal with it - ignore given num??*/
  num = len >> 3;
  if (t2samps==0)
  {
    t2samp_num = num;
    t2samps = (QT_T2SAMP_HDR *)malloc(num * sizeof(QT_T2SAMP_HDR));
    if (t2samps==0) TheEnd1("QT_Read_STTS: malloc err");
    cur = 0;
  }
  else
  { QT_T2SAMP_HDR *t_t2samp;
    t_t2samp = (QT_T2SAMP_HDR *)
			malloc((t2samp_num + num) * sizeof(QT_T2SAMP_HDR));
    if (t_t2samp==0) TheEnd1("QT_Read_STTS: malloc err");
    for(i=0;i<t2samp_num;i++) t_t2samp[i] = t2samps[i];
    cur = t2samp_num;
    t2samp_num += num;
    FREE(t2samps,0x900e);
    t2samps = t_t2samp;
  }
  for(i=0;i<num;i++)
  { double ftime;
    samp_cnt	= UTIL_Get_MSB_Long(fin);
    duration	= UTIL_Get_MSB_Long(fin);  len -= 8;
    if (duration == 0) duration = 1;
    /* NOTE: convert to 1000ms per second */
    t2samps[cur].cnt = samp_cnt;
    ftime = (1000.0 * (double)(duration)) / (double)(qt_vid_timescale);
    t2samps[cur].time = (xaULONG)(ftime);
    ftime -= (double)(t2samps[cur].time);
    t2samps[cur].timelo = (xaULONG)(ftime * (double)(1<<24));
    DEBUG_LEVEL2 fprintf(stdout,"      %d) samp_cnt=%x duration = %x time %d timelo %d ftime %f\n",
	i,samp_cnt,duration,t2samps[cur].time,t2samps[cur].timelo,ftime);
    cur++;
  }
  *qt_t2samp_num = t2samp_num;
  *qt_t2samps = t2samps;
  while(len > 0) {len--; getc(fin); }
}


/* Sync Sample */
void QT_Read_STSS(fin)
FILE *fin;
{
  xaULONG version,num,i,j;
  version	= UTIL_Get_MSB_Long(fin);
  num		= UTIL_Get_MSB_Long(fin);
  DEBUG_LEVEL2 
  {
    fprintf(stdout,"    ver=%x num of entries = %x\n",version,num);
    j = 0;
    fprintf(stdout,"      ");
  }
  for(i=0;i<num;i++)
  {
    xaULONG samp_num;
    samp_num	= UTIL_Get_MSB_Long(fin);
    DEBUG_LEVEL2
    {
      fprintf(stdout,"%x ",samp_num); j++;
      if (j >= 8) {fprintf(stdout,"\n      "); j=0; }
    }
  }
  DEBUG_LEVEL2 fprintf(stdout,"\n");
}


/* Sample to Chunk */
void QT_Read_STSC(fin,len,qt_s2chunk_num,qt_s2chunks,
		chunkoff_lstnum,codec_num,codec_lstnum)
FILE *fin;
xaLONG len;
xaULONG *qt_s2chunk_num;
QT_S2CHUNK_HDR **qt_s2chunks;
xaULONG chunkoff_lstnum,codec_num,codec_lstnum;
{
  xaULONG version,num,i,cur,stsc_type,last;
  xaULONG s2chunk_num = *qt_s2chunk_num;
  QT_S2CHUNK_HDR *s2chunks = *qt_s2chunks;

  version	= UTIL_Get_MSB_Long(fin);
  num		= UTIL_Get_MSB_Long(fin);	len -= 16;
  i = (num)?(len/num):(0);
  if (i == 16) 
  { 
    DEBUG_LEVEL2 fprintf(stdout,"STSC: OLD STYLE\n");
    len -= num * 16; stsc_type = 0;
  }
  else 
  { 
    DEBUG_LEVEL2 fprintf(stdout,"STSC: NEW STYLE\n");
    len -= num * 12; stsc_type = 1;
  }

  DEBUG_LEVEL2 fprintf(stdout,"    ver=%x num of entries = %x\n",version,num);
  if (s2chunks == 0)
  {
    s2chunk_num = num;
    s2chunks = (QT_S2CHUNK_HDR *)malloc((num+1) * sizeof(QT_S2CHUNK_HDR));
    cur = 0;
  }
  else
  { QT_S2CHUNK_HDR *ts2c;
    ts2c = (QT_S2CHUNK_HDR *)
	malloc( (s2chunk_num + num + 1) * sizeof(QT_S2CHUNK_HDR));
    for(i=0;i<s2chunk_num;i++) ts2c[i] = s2chunks[i];
    cur = s2chunk_num;
    s2chunk_num += num;
    FREE(s2chunks,0x900f);
    s2chunks = ts2c;
  }
  last = 0;
  for(i=0;i<num;i++)
  {
    xaULONG first_chk,samp_per,chunk_tag;
    if (stsc_type == 0)  /* 4 entries */
    { xaULONG tmp;
      first_chk	= UTIL_Get_MSB_Long(fin);
      tmp	= UTIL_Get_MSB_Long(fin);
      samp_per	= UTIL_Get_MSB_Long(fin);
      chunk_tag	= UTIL_Get_MSB_Long(fin);
      if (i > 0) s2chunks[cur-1].num   = first_chk - last;
      last = first_chk;
      if (i==(num-1))
      {
	if (qt_stgs_num)
	{
	  s2chunks[cur].num   = (qt_stgs_num - first_chk) + 1;
	  qt_stgs_num = 0;
        }
	else 
	{
	  fprintf(stdout,"STSC: old style but not stgs chunk warning\n");
	  s2chunks[cur].num = 100000;
	}
      }
    }
    else		/* 3 entries */
    {
      first_chk	= UTIL_Get_MSB_Long(fin);
      samp_per	= UTIL_Get_MSB_Long(fin);
      chunk_tag	= UTIL_Get_MSB_Long(fin);
      s2chunks[cur].num   = samp_per;
    }
    DEBUG_LEVEL2 
     fprintf(stdout,"      %d-%d) first_chunk=%x samp_per_chk=%x chk_tag=%x\n",
					i,cur,first_chk,samp_per,chunk_tag);
        /* start at 0 not 1  and account for previous chunks */
    s2chunks[cur].first = first_chk - 1 + chunkoff_lstnum;
    if (chunk_tag > (codec_num - codec_lstnum)) 
	{ samp_per = chunk_tag = 1; }
    s2chunks[cur].tag   = chunk_tag - 1 + codec_lstnum;
    cur++;
  }
  s2chunks[cur].first = 0;
  s2chunks[cur].num   = 0;
  s2chunks[cur].tag   = 0;
  DEBUG_LEVEL2 fprintf(stdout,"    STSC left over %d\n",len);
  while (len > 0) { fgetc(fin); len--; }
  *qt_s2chunk_num = s2chunk_num;
  *qt_s2chunks = s2chunks;
}

/* Sample Size */
void QT_Read_STSZ(fin,len,qt_samp_num,qt_samp_sizes)
FILE *fin;
xaLONG len;
xaULONG *qt_samp_num,**qt_samp_sizes;
{
  xaULONG version,samp_size,num,i,cur;
  xaULONG samp_num   = *qt_samp_num;
  xaULONG *samp_sizes = *qt_samp_sizes;

  version	= UTIL_Get_MSB_Long(fin);
  samp_size	= UTIL_Get_MSB_Long(fin);
  num		= UTIL_Get_MSB_Long(fin);
  len = (len - 20) / 4;   /* number of stored samples */
  DEBUG_LEVEL2 fprintf(stdout,"    ver=%x samp_size=%x entries= %x stored entries=%x\n",version,samp_size,num,len);

  if (samp_size == 1) num = 1; /* for AUDIO PODNOTE: rethink this */
  if (samp_sizes == 0)
  {
    samp_num = num;
    samp_sizes = (xaULONG *)malloc(num * sizeof(xaULONG));
    if (samp_sizes == 0) {fprintf(stdout,"malloc err 0\n"); exit(0);}
    cur = 0;
  }
  else /*TRAK*/
  {
    xaULONG *tsamps;
    tsamps = (xaULONG *)malloc((samp_num + num) * sizeof(xaULONG));
    if (tsamps == 0) {fprintf(stdout,"malloc err 0\n"); exit(0);}
    for(i=0; i<samp_num; i++) tsamps[i] = samp_sizes[i];
    cur = samp_num;
    samp_num += num;
    FREE(samp_sizes,0x9010);
    samp_sizes = tsamps;
  }
  for(i=0;i<num;i++) 
  {
    if (i < len) samp_sizes[cur] = UTIL_Get_MSB_Long(fin);
    else if (i==0) samp_sizes[cur] = samp_size;
           else samp_sizes[cur] = samp_sizes[cur-1];
    cur++;
  }
  *qt_samp_num = samp_num;
  *qt_samp_sizes = samp_sizes;
}

/* Chunk Offset */
void QT_Read_STCO(fin,qt_chunkoff_num,qt_chunkoffs)
FILE *fin;
xaULONG *qt_chunkoff_num;
xaULONG **qt_chunkoffs;
{
  xaULONG version,num,i,cur;
  xaULONG chunkoff_num = *qt_chunkoff_num;
  xaULONG *chunkoffs = *qt_chunkoffs;

  version	= UTIL_Get_MSB_Long(fin);
  num		= UTIL_Get_MSB_Long(fin);
  DEBUG_LEVEL2 fprintf(stdout,"    ver=%x entries= %x\n",version,num);
  if (chunkoffs == 0)
  {
    chunkoff_num = num;
    chunkoffs = (xaULONG *)malloc(num * sizeof(xaULONG) );
    cur = 0;
  }
  else
  {
    xaULONG *tchunks;
    tchunks = (xaULONG *)malloc((chunkoff_num + num) * sizeof(xaULONG));
    if (tchunks == 0) {fprintf(stdout,"malloc err 0\n"); exit(0);}
    for(i=0; i<chunkoff_num; i++) tchunks[i] = chunkoffs[i];
    cur = chunkoff_num;
    chunkoff_num += num;
    FREE(chunkoffs,0x9011);
    chunkoffs = tchunks;
  }
  for(i=0;i<num;i++) {chunkoffs[cur] = UTIL_Get_MSB_Long(fin); cur++; }
  *qt_chunkoff_num = chunkoff_num;
  *qt_chunkoffs = chunkoffs;
 DEBUG_LEVEL2
 { for(i=0;i<num;i++)  fprintf(stdout,"  STCO %d) %x\n",i,
		chunkoffs[ i ]); 
 }
}



xaULONG QT_Read_Video_Data(qt,fin,anim_hdr)
XA_ANIM_SETUP *qt;
FILE *fin;
XA_ANIM_HDR *anim_hdr;
{
  xaULONG d,ret,i;
  xaULONG cur_samp,cur_s2chunk,nxt_s2chunk;
  xaULONG cur_t2samp,nxt_t2samp;
  xaULONG tag;
  xaULONG cur_off;
  xaULONG unsupported_flag = xaFALSE;
  XA_ACTION *act;

  qtv_init_duration += qts_start_offset;

  qt->cmap_frame_num = qtv_chunkoff_num / cmap_sample_cnt;

  nxt_t2samp = cur_t2samp = 0;
  if (qtv_t2samps)
  {
    if (xa_jiffy_flag) { qt->vid_time = xa_jiffy_flag; qt->vid_timelo = 0; }
    else
    { 
      qt->vid_time   = qtv_t2samps[cur_t2samp].time;
      qt->vid_timelo = qtv_t2samps[cur_t2samp].timelo;
    }
    nxt_t2samp += qtv_t2samps[cur_t2samp].cnt;
  } else { qt->vid_time = XA_GET_TIME(100); qt->vid_timelo = 0; }

  cur_off=0;
  cur_samp = 0;
  cur_s2chunk = 0;
  nxt_s2chunk = qtv_s2chunks[cur_s2chunk + 1].first;
  tag =  qtv_s2chunks[cur_s2chunk].tag;
  qt->imagex = qtv_codecs[tag].width;
  qt->imagey = qtv_codecs[tag].height;
  qt->depth  = qtv_codecs[tag].depth;
  qt->compression  = qtv_codecs[tag].compression;
  qt->chdr   = qtv_codecs[tag].chdr;

  /* Loop through chunk offsets */
  for(i=0; i < qtv_chunkoff_num; i++)
  {
    xaULONG size,chunk_off,num_samps;
    ACT_DLTA_HDR *dlta_hdr;

    chunk_off =  qtv_chunkoffs[i];

/* survive RPZA despite corruption(offsets commonly corrupted).*/
/* MOVE THIS INTO RPZA DECODE
   check size of RPZA again size in codec.
*/

    if ( (i == nxt_s2chunk) && ((cur_s2chunk+1) < qtv_s2chunk_num) )
    {
      cur_s2chunk++;
      nxt_s2chunk = qtv_s2chunks[cur_s2chunk + 1].first;
    }
    num_samps = qtv_s2chunks[cur_s2chunk].num;

    /* Check tags and possibly move to new codec */
    if (qtv_s2chunks[cur_s2chunk].tag >= qtv_codec_num) 
    {
      fprintf(stdout,"QT Data: Warning stsc chunk invalid %d tag %d\n",
		cur_s2chunk,qtv_s2chunks[cur_s2chunk].tag);
    } 
    else if (qtv_s2chunks[cur_s2chunk].tag != tag)
    {
      tag =  qtv_s2chunks[cur_s2chunk].tag;
      qt->imagex = qtv_codecs[tag].width;
      qt->imagey = qtv_codecs[tag].height;
      qt->depth  = qtv_codecs[tag].depth;
      qt->compression  = qtv_codecs[tag].compression;
      qt->chdr   = qtv_codecs[tag].chdr;
    }

    /* Read number of samples in each chunk */
    cur_off = chunk_off;
    while(num_samps--)
    {

DEBUG_LEVEL2 fprintf(stdout,"T2S: cur-t2 %d cur-smp %d nxt-t2 %d tot t2 %d\n", cur_t2samp,cur_samp,nxt_t2samp,qtv_t2samp_num);
    if ( (cur_samp >= nxt_t2samp) && (cur_t2samp < qtv_t2samp_num) )
    {
      cur_t2samp++;
      if (xa_jiffy_flag) { qt->vid_time = xa_jiffy_flag; qt->vid_timelo = 0; }
      else
      { qt->vid_time   = qtv_t2samps[cur_t2samp].time;
	qt->vid_timelo = qtv_t2samps[cur_t2samp].timelo;
      }
      nxt_t2samp += qtv_t2samps[cur_t2samp].cnt;
    }

      size = qtv_samp_sizes[cur_samp];

      act = ACT_Get_Action(anim_hdr,ACT_DELTA);
      if (xa_file_flag == xaTRUE)
      {
	dlta_hdr = (ACT_DLTA_HDR *) malloc(sizeof(ACT_DLTA_HDR));
	if (dlta_hdr == 0) TheEnd1("QT rle: malloc failed");
	act->data = (xaUBYTE *)dlta_hdr;
	dlta_hdr->flags = ACT_SNGL_BUF;
	dlta_hdr->fsize = size;
	dlta_hdr->fpos  = cur_off;
	if (size > qt->max_fvid_size) qt->max_fvid_size = size;
      }
      else
      {
	d = size + (sizeof(ACT_DLTA_HDR));
	dlta_hdr = (ACT_DLTA_HDR *) malloc( d );
	if (dlta_hdr == 0) TheEnd1("QT rle: malloc failed");
	act->data = (xaUBYTE *)dlta_hdr;
	dlta_hdr->flags = ACT_SNGL_BUF | DLTA_DATA;
	dlta_hdr->fpos = 0; dlta_hdr->fsize = size;
	fseek(fin,cur_off,0);
	ret = fread( dlta_hdr->data, size, 1, fin);
	if (ret != 1) 
        { fprintf(stdout,"QT: video read err\n"); 
          if (qt_frame_cnt)	return(xaTRUE);
	  else			return(xaFALSE); }
      }
      cur_off += size;

      if (qtv_init_duration)
      { xaLONG t_time, t_timelo; double ftime;

        ftime = (1000.0 * (double)(qtv_init_duration)) 
					/ (double)(qtv_tk_timescale);
        t_time = (xaULONG)(ftime);
        ftime -= (double)(t_time);
        t_timelo = (xaULONG)(ftime * (double)(1<<24));
	t_time   += qt->vid_time;
	t_timelo += qt->vid_timelo;
	while(t_timelo >= 1<<24) { t_timelo -= 1<<24; t_time++; }
        QT_Add_Frame(t_time,t_timelo,act);
        qtv_init_duration = 0;
      }
      else 
	QT_Add_Frame(qt->vid_time,qt->vid_timelo,act);
      dlta_hdr->xpos = dlta_hdr->ypos = 0;
      dlta_hdr->xsize = qt->imagex;
      dlta_hdr->ysize = qt->imagey;
      dlta_hdr->special = 0;
      if (qtv_codecs[tag].decoder)
      { dlta_hdr->extra = qtv_codecs[tag].dlta_extra;
	dlta_hdr->delta = qtv_codecs[tag].decoder;
	dlta_hdr->xapi_rev = qtv_codecs[tag].xapi_rev;
      }
      else
      {
	if (unsupported_flag == xaFALSE) /* only output single warning */
	{
	  fprintf(stdout,"QT: Sections of this movie use an unsupported Video Codec\n");
	  fprintf(stdout," and are therefore NOT viewable.\n");
	  unsupported_flag = xaTRUE;
        }
        act->type = ACT_NOP;
      }
      ACT_Setup_DeltaOLD(qt,act,dlta_hdr,fin);
      cur_samp++;
      if (cur_samp >= qtv_samp_num) break;
    } /* end of sample number */
    if (cur_samp >= qtv_samp_num) break;
  } /* end of chunk_offset loop */
  return(xaTRUE);
}


xaULONG QT_Parse_Bin(fin)
FILE *fin;
{
  xaULONG pos,len,cid,total;
 
  fseek(fin,0,2);
  total = ftell(fin);
 
/* Read over Header */
  fseek(fin,0,0);
  pos = len = UTIL_Get_MSB_Long(fin);
  cid = UTIL_Get_MSB_Long(fin);
  if (cid == QT_mdat)
  {
    fseek(fin,0,0);
    if (len == 0)
    {
      fprintf(stdout,"QT: This is only .data fork. Need .rsrc fork\n");
      return(0);
    }
    else return(1);
  }
  else if (cid == QT_moov) 
  { fseek(fin,0,0);
    return(1);
  }

  if (len < 4) return(0);
 
DEBUG_LEVEL1 fprintf(stdout,"QT_Parse_Bin: pos %x Total %x\n",pos,total);
 
  while( pos < total )
  { xaULONG  d1,d2;

    fseek(fin,pos,0);

    len = UTIL_Get_MSB_Long(fin);
    d1 = UTIL_Get_MSB_Long(fin);
    d2 = UTIL_Get_MSB_Long(fin);

/* Search for QT_moov Chunk
 *
 * MacBinary Format
 *   len: length of BIN chunk something
 *   d1 : possibly QT Chunk size
 *   d2 : possibly QT Chunk ID
 *
 * Quicktime Format
 *   len: QT Chunk size
 *   d1 : QT Chunk ID
 *
 * NOTE: this is to catch Quicktime files missing the "mdat" atom and
 * it will only work is the very next chunk is "moov".
 */

    if (d1 == QT_moov)  /* Quicktime Format */
    {
      qt_data_flag = xaTRUE;   /* .data chunk is unmarked */
      fseek(fin,pos,0);
      return(1);
    }

    pos += 4;        /* move past "len" */

    if (d2 == QT_moov) /* MacBinary Format */
    {
      fseek(fin,pos,0);
      return(1);
    }
    if (len == 0) return(0);
    pos += len;
  }
  return(0);
}


/*********************************************
 * Read and Parse Audio Codecs
 *
 **********/
void QT_Read_Audio_STSD(fin)
FILE *fin;
{ xaULONG i,version,num,cur,sup;
  version = UTIL_Get_MSB_Long(fin);
  num = UTIL_Get_MSB_Long(fin);
  DEBUG_LEVEL2 fprintf(stdout,"     ver = %x  num = %x\n", version,num);
  if (qts_codecs == 0)
  { qts_codec_num = num;
    qts_codecs = (QTS_CODEC_HDR *)malloc(qts_codec_num * sizeof(QTS_CODEC_HDR));
    if (qts_codecs==0) TheEnd1("QT STSD: malloc err");
    cur = 0;
  }
  else
  { QTS_CODEC_HDR *tcodecs;
    tcodecs = (QTS_CODEC_HDR *)malloc((qts_codec_num+num) * sizeof(QTS_CODEC_HDR))
;
    if (tcodecs==0) TheEnd1("QT STSD: malloc err");
    for(i=0;i<qts_codec_num;i++) tcodecs[i] = qts_codecs[i];
    cur = qts_codec_num;
    qts_codec_num += num;
    FREE(qts_codecs,0x9014);
    qts_codecs = tcodecs;
  }
  sup = 0;
  for(i=0; i < num; i++)
  {
    sup |= QT_Read_Audio_Codec_HDR( &qts_codecs[cur], fin );
    cur++;
  }
  if (sup == 0)
  {
    if (qt_audio_attempt == xaTRUE) 
    {
      fprintf(stdout,"QT Audio Codec not supported\n");
      qt_audio_attempt = xaFALSE;
    }
  }
}


xaULONG QT_Read_Audio_Codec_HDR(c_hdr,fin)
QTS_CODEC_HDR *c_hdr;
FILE *fin;
{ xaULONG len;
  xaULONG ret = 1;
  len			= UTIL_Get_MSB_Long(fin);
  c_hdr->compression	= UTIL_Get_MSB_Long(fin);
  c_hdr->dref_id	= UTIL_Get_MSB_Long(fin);
  c_hdr->version	= UTIL_Get_MSB_Long(fin);
  c_hdr->codec_rev	= UTIL_Get_MSB_Long(fin);
  c_hdr->vendor		= UTIL_Get_MSB_Long(fin);
  c_hdr->chan_num	= UTIL_Get_MSB_UShort(fin);
  c_hdr->bits_samp	= UTIL_Get_MSB_UShort(fin);

/* Some Mac Shareware program screws up when converting AVI's to QT's */
  if (   (c_hdr->bits_samp == 1)
      && (   (c_hdr->compression == QT_twos)
          || (c_hdr->compression == QT_raw)
          || (c_hdr->compression == QT_raw00)
     )  )                                       c_hdr->bits_samp = 8;

  c_hdr->comp_id	= UTIL_Get_MSB_UShort(fin);
  c_hdr->pack_size	= UTIL_Get_MSB_UShort(fin);
  c_hdr->samp_rate	= UTIL_Get_MSB_UShort(fin);
  c_hdr->pad		= UTIL_Get_MSB_UShort(fin);

  if (xa_verbose)
  {
    fprintf(stdout,"  Audio Codec: "); QT_Audio_Type(c_hdr->compression);
    fprintf(stdout," Rate=%d Chans=%d Bps=%d\n",
	c_hdr->samp_rate,c_hdr->chan_num,c_hdr->bits_samp);
  }

  if (c_hdr->compression == QT_twos) c_hdr->compression =XA_AUDIO_SIGNED;
  else if (c_hdr->compression == QT_raw) c_hdr->compression =XA_AUDIO_LINEAR;
  else if (c_hdr->compression == QT_raw00) c_hdr->compression =XA_AUDIO_LINEAR;
  else if (c_hdr->compression == QT_ima4) c_hdr->compression =XA_AUDIO_IMA4;
  else ret = 0;
  if (c_hdr->bits_samp==8) c_hdr->bps = 1;
  else if (c_hdr->bits_samp==16) c_hdr->bps = 2;
  else if (c_hdr->bits_samp==32) c_hdr->bps = 4;
  else c_hdr->bps = 100 + c_hdr->bits_samp;

  if (c_hdr->bps > 2) ret = 0;
  if (c_hdr->chan_num > 2) ret = 0;

  if (   (c_hdr->bps==2) 
      && ((c_hdr->compression & XA_AUDIO_TYPE_MASK) != XA_AUDIO_IMA4) )
  {
    c_hdr->compression |= XA_AUDIO_BPS_2_MSK;
    c_hdr->compression |= XA_AUDIO_BIGEND_MSK; /* only has meaning >= 2 bps */
  }
  if (c_hdr->chan_num==2)
  {
    c_hdr->compression |= XA_AUDIO_STEREO_MSK;
    c_hdr->bps *= 2;
  }

  return(ret);
}

void QT_Audio_Type(type)
xaULONG type;
{ switch(type)
  { case QT_raw:	fprintf(stdout,"PCM"); break;
    case QT_raw00:	fprintf(stdout,"PCM0"); break;
    case QT_twos:	fprintf(stdout,"TWOS"); break;
    case QT_MAC6:	fprintf(stdout,"MAC6"); break;
    case QT_ima4:	fprintf(stdout,"IMA4"); break;
    default:		fprintf(stdout,"(%c%c%c%c)(%08x)",
			  (char)((type>>24)&0xff), (char)((type>>16)&0xff),
			  (char)((type>>8)&0xff),(char)((type)&0xff),type); 
	break;
  }
}

/**************
 *
 * 
 *******/
xaULONG QT_Read_Audio_Data(qt,fin,anim_hdr)
XA_ANIM_SETUP *qt;
FILE *fin;
XA_ANIM_HDR *anim_hdr;
{
  xaULONG ret,i;
  xaULONG cur_s2chunk,nxt_s2chunk;
  xaULONG tag;

DEBUG_LEVEL1 fprintf(stdout,"QT_Read_Audio: attempt %x co# %d\n",
				qt_audio_attempt,qts_chunkoff_num);

  cur_s2chunk	= 0;
  nxt_s2chunk	= qts_s2chunks[cur_s2chunk + 1].first;
  tag		= qts_s2chunks[cur_s2chunk].tag;
  qt_audio_freq	 = qts_codecs[tag].samp_rate;
  qt_audio_chans = qts_codecs[tag].chan_num;
  qt_audio_bps	 = qts_codecs[tag].bps;
  qt_audio_type	 = qts_codecs[tag].compression;
  qt_audio_end	 = 1;

  /* Initial Silence if any. PODNOTE: Eventually Modify for Middle silence */
  qts_init_duration += qtv_start_offset;
  if (qts_init_duration)
  { xaULONG snd_size;

    snd_size = (qt_audio_freq * qts_init_duration) / qts_tk_timescale;
    if (XA_Add_Sound(anim_hdr,0,XA_AUDIO_NOP, -1, qt_audio_freq,
	snd_size, &qt->aud_time, &qt->aud_timelo, 0, 0) == xaFALSE) 
							return(xaFALSE);
  }


  /* Loop through chunk offsets */
  for(i=0; i < qts_chunkoff_num; i++)
  { xaULONG size,chunk_off,num_samps,snd_size;
    xaULONG blockalign, sampsblock;

    if ( (i == nxt_s2chunk) && ((cur_s2chunk+1) < qts_s2chunk_num) )
    {
      cur_s2chunk++;
      nxt_s2chunk = qts_s2chunks[cur_s2chunk+1].first;
    }
    num_samps = qts_s2chunks[cur_s2chunk].num; /* * sttz */

    /* Check tags and possibly move to new codec */
    if (qts_s2chunks[cur_s2chunk].tag >= qts_codec_num) 
    {
      fprintf(stdout,"QT Data: Warning stsc chunk invalid %d tag %d\n",
		cur_s2chunk,qts_s2chunks[cur_s2chunk].tag);
    } 
    else if (qts_s2chunks[cur_s2chunk].tag != tag)
    {
      tag =  qts_s2chunks[cur_s2chunk].tag;
      qt_audio_freq  = qts_codecs[tag].samp_rate;
      qt_audio_chans = qts_codecs[tag].chan_num;
      qt_audio_bps   = qts_codecs[tag].bps;
      qt_audio_type  = qts_codecs[tag].compression;
      qt_audio_end   = 1;
    }

/* NOTE: THE STSZ CHUNKS FOR AUDIO IS TOTALLY F****D UP AND INCONSISTENT
 * ACROSS MANY ANIMATIONS. CURRENTLY JUST IGNORE THIS. */
    if (i < qts_samp_num) size = qts_samp_sizes[i];
    else if (qts_samp_num) size = qts_samp_sizes[qts_samp_num-1];
    else size = qt_audio_bps;

    if (size > qt_audio_bps) 
    {
	fprintf(stdout,"QT UNIQE AUDIO: sz %d bps %d\n",size,qt_audio_bps);
    }

    chunk_off =  qts_chunkoffs[i];


/*
fprintf(stdout,"SND: off %08x size %08x\n",chunk_off, size);
*/
    if (qt_audio_type == XA_AUDIO_IMA4_M)
    { xaULONG numblks = num_samps / 0x40;
      snd_size = numblks * 0x22; 
      blockalign = 0x22;
      sampsblock = 0x40;
    }
    else if (qt_audio_type == XA_AUDIO_IMA4_S)
    { xaULONG numblks = num_samps / 0x40;
      snd_size = numblks * 0x44; 
      blockalign = 0x44;
      sampsblock = 0x40;
    }
    else
    { snd_size = num_samps * qt_audio_bps;
      blockalign = qt_audio_bps; /* not really valid yet */
      sampsblock = 0x1;
    }

DEBUG_LEVEL1 fprintf(stdout,"snd_size %x  numsamps %x size %x bps %d off %x\n",
		snd_size,num_samps,size,qt_audio_bps,chunk_off);

    if (xa_file_flag == xaTRUE)
    {
      if (XA_Add_Sound(anim_hdr,0,qt_audio_type, chunk_off, qt_audio_freq,
		snd_size, &qt->aud_time, &qt->aud_timelo, 
		blockalign,sampsblock) == xaFALSE) return(xaFALSE);
      if (snd_size > qt->max_faud_size) qt->max_faud_size = snd_size;
    }
    else
    { xaUBYTE *snd_data = (xaUBYTE *)malloc(snd_size);
      if (snd_data==0) TheEnd1("QT aud_dat: malloc err");
      fseek(fin,chunk_off,0);  /* move to start of chunk data */
      ret = fread(snd_data, snd_size, 1, fin);
      if (ret != 1)
      { fprintf(stdout,"QT: snd rd err\n"); 
        return(xaTRUE);
      }
      if (XA_Add_Sound(anim_hdr,snd_data,qt_audio_type, -1, qt_audio_freq,
		snd_size, &qt->aud_time, &qt->aud_timelo,
		blockalign,sampsblock) == xaFALSE) return(xaFALSE);

    }
  } /* end of chunk_offset loop */
  return(xaTRUE);
}


/********
 * Have No Clue
 *
 ****/
void QT_Read_STGS(fin,len)
FILE *fin;
xaLONG len;
{
  xaULONG i,version,num;
  xaULONG samps,pad;

  version	= UTIL_Get_MSB_Long(fin);
  num		= UTIL_Get_MSB_Long(fin); len -= 16;
  qt_stgs_num = 0;
  for(i=0; i<num; i++)
  {
    samps	= UTIL_Get_MSB_Long(fin);
    pad		= UTIL_Get_MSB_Long(fin);	len -= 8;
    qt_stgs_num += samps;
  }
  while(len > 0) {len--; getc(fin); }
}


/* MAX */
int qt_4map[] = {
0xff, 0xfb, 0xff,
0xef, 0xd9, 0xbb,
0xe8, 0xc9, 0xb1,
0x93, 0x65, 0x5e,
0xfc, 0xde, 0xe8,
0x9d, 0x88, 0x91,
0xff, 0xff, 0xff,
0xff, 0xff, 0xff,
0xff, 0xff, 0xff,
0x47, 0x48, 0x37,
0x7a, 0x5e, 0x55,
0xdf, 0xd0, 0xab,
0xff, 0xfb, 0xf9,
0xe8, 0xca, 0xc5,
0x8a, 0x7c, 0x77
};


/**************************************
 * QT_Create_Default_Cmap
 *
 * This routine recreates the Default Apple colormap.
 * It is an educated quess after looking at two quicktime animations
 * and may not be totally correct, but seems to work okay.
 */
void QT_Create_Default_Cmap(cmap,cnum)
ColorReg *cmap;
xaULONG cnum;
{
  xaLONG r,g,b,i;

  if (cnum == 16)
  { 
    for(i=0;i<15;i++)
    { int d = i * 3;
      cmap[i].red   = 0x101 * qt_4map[d];
      cmap[i].green = 0x101 * qt_4map[d+1];
      cmap[i].blue  = 0x101 * qt_4map[d+2];
    }
  }
  else
  { static xaUBYTE pat[10] = {0xee,0xdd,0xbb,0xaa,0x88,0x77,0x55,0x44,0x22,0x11};
    r = g = b = 0xff;
    for(i=0;i<215;i++)
    {
      cmap[i].red   = 0x101 * r;
      cmap[i].green = 0x101 * g;
      cmap[i].blue  = 0x101 * b;
      b -= 0x33;
      if (b < 0) { b = 0xff; g -= 0x33; if (g < 0) { g = 0xff; r -= 0x33; } }
    }
    for(i=0;i<10;i++)
    { xaULONG d = 0x101 * pat[i];
      xaULONG ip = 215 + i; 
      cmap[ip].red   = d; cmap[ip].green = cmap[ip].blue  = 0; ip += 10;
      cmap[ip].green = d; cmap[ip].red   = cmap[ip].blue  = 0; ip += 10;
      cmap[ip].blue  = d; cmap[ip].red   = cmap[ip].green = 0; ip += 10;
      cmap[ip].red   = cmap[ip].green = cmap[ip].blue  = d;
    }
    cmap[255].red = cmap[255].green = cmap[255].blue  = 0x00;
  }
}
   

/**************************************
 * QT_Create_Gray_Cmap
 *
 * This routine recreates the Default Gray Apple colormap.
 */
void QT_Create_Gray_Cmap(cmap,flag,num)
ColorReg *cmap;
xaULONG flag;	/* flag=0  0=>255;  flag=1 255=>0 */
xaULONG num;	/* size of color map */
{
  xaLONG g,i;

  if (num == 256)
  {
    g = (flag)?(0x00):(0xff);
    for(i=0;i<256;i++)
    {
      cmap[i].red   = 0x101 * g;
      cmap[i].green = 0x101 * g;
      cmap[i].blue  = 0x101 * g;
      cmap[i].gray  = 0x101 * g;
      if (flag) g++; else g--;
    }
  }
  else if (num == 16)
  {
    g = 0xf;
    for(i=0;i<16;i++)
    {
      cmap[i].red   = 0x1111 * g;
      cmap[i].green = 0x1111 * g;
      cmap[i].blue  = 0x1111 * g;
      cmap[i].gray  = 0x1111 * g;
      g--;
    }
  }
}



