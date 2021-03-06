/*
 * EffecTV - Realtime Digital Video Effector
 * Copyright (C) 2001-2006 FUKUCHI Kentaro
 *
 * SimuraTV - color distortion and mirrored image effector
 * Copyright (C) 2001-2002 FUKUCHI Kentaro
 *
 */

#include <stdlib.h>
#include <string.h>
#include <gmerlin_effectv.h>
#include <utils.h>

static int start(effect*);
static int stop(effect*);
static int draw(effect*,RGB32 *src, RGB32 *dest);

typedef struct
  {
  int stat;
  RGB32 color;
  int mirror;
  int hwidth;
  int hheight;
  } simuratv_t;

static void mirror_no(effect * e, RGB32 *, RGB32 *);
static void mirror_u(effect * e, RGB32 *, RGB32 *);
static void mirror_d(effect * e, RGB32 *, RGB32 *);
static void mirror_r(effect * e, RGB32 *, RGB32 *);
static void mirror_l(effect * e, RGB32 *, RGB32 *);
static void mirror_ul(effect * e, RGB32 *, RGB32 *);
static void mirror_ur(effect * e, RGB32 *, RGB32 *);
static void mirror_dl(effect * e, RGB32 *, RGB32 *);
static void mirror_dr(effect * e, RGB32 *, RGB32 *);

static effect *simuraRegister(void)
{
	effect *entry;
        simuratv_t * priv;
#if 0
        int i;
	RGB32 tmp[26];
	
	for(i=0; i<26; i++) {
		tmp[keytable[i] - 'a'] = colortable[i];
	}
	for(i=0; i<26; i++) {
		colortable[i] = tmp[i];
	}
#endif

	entry = calloc(1, sizeof(effect));
	if(entry == NULL) {
		return NULL;
	}
        priv = calloc(1, sizeof(*priv));
        entry->priv = priv;
	entry->start = start;
	entry->stop = stop;
	entry->draw = draw;

	return entry;
}

static int start(effect * e)
  {
  simuratv_t * priv = e->priv;
  
  priv->hwidth = e->video_width/2;
  priv->hheight = e->video_height/2;
  
  priv->stat = 1;
  return 0;
  }

static int stop(effect * e)
  {
  simuratv_t * priv = e->priv;
  priv->stat = 0;
  return 0;
  }

static int draw(effect * e, RGB32 *src, RGB32 *dest)
  {
  simuratv_t * priv = e->priv;
  switch(priv->mirror)
    {
    case 1:
      mirror_l(e, src, dest);
      break;
    case 2:
      mirror_r(e, src, dest);
      break;
    case 3:
      mirror_d(e, src, dest);
      break;
    case 4:
      mirror_dl(e, src, dest);
      break;
    case 5:
      mirror_dr(e, src, dest);
      break;
    case 6:
      mirror_u(e, src, dest);
      break;
    case 7:
      mirror_ul(e, src, dest);
      break;
    case 8:
      mirror_ur(e, src, dest);
      break;
    case 0:
    default:
      mirror_no(e, src, dest);
      break;
  }

  return 0;
  }


static void mirror_no(effect * e, RGB32 *src, RGB32 *dest)
  {
  int i;
  simuratv_t * priv = e->priv;

  for(i=0; i<e->video_area; i++)
    {
    dest[i] = src[i] ^ priv->color;
    }
  }

static void mirror_u(effect * e, RGB32 *src, RGB32 *dest)
  {
  int x, y;
  simuratv_t * priv = e->priv;

  for(y=0; y<priv->hheight; y++) {
  for(x=0; x<e->video_width; x++) {
  dest[y*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  dest[(e->video_height-y-1)*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  }
  }
  }

static void mirror_d(effect * e, RGB32 *src, RGB32 *dest)
  {
  int x, y;
  simuratv_t * priv = e->priv;

  for(y=priv->hheight; y<e->video_height; y++) {
  for(x=0; x<e->video_width; x++) {
  dest[y*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  dest[(e->video_height-y-1)*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  }
  }
  }

static void mirror_l(effect * e, RGB32 *src, RGB32 *dest)
  {
  int x, y;
  simuratv_t * priv = e->priv;

  for(y=0; y<e->video_height; y++) {
  for(x=0; x<priv->hwidth; x++) {
  dest[y*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  dest[y*e->video_width+(e->video_width-x-1)] = src[y*e->video_width+x] ^ priv->color;
  }
  }
  }

static void mirror_r(effect * e, RGB32 *src, RGB32 *dest)
  {
  int x, y;
  simuratv_t * priv = e->priv;

  for(y=0; y<e->video_height; y++) {
  for(x=priv->hwidth; x<e->video_width; x++) {
  dest[y*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  dest[y*e->video_width+(e->video_width-x-1)] = src[y*e->video_width+x] ^ priv->color;
  }
  }
  }

static void mirror_ul(effect * e, RGB32 *src, RGB32 *dest)
  {
  int x, y;
  simuratv_t * priv = e->priv;

  for(y=0; y<priv->hheight; y++) {
  for(x=0; x<priv->hwidth; x++) {
  dest[y*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  dest[y*e->video_width+(e->video_width-x-1)] = src[y*e->video_width+x] ^ priv->color;
  dest[(e->video_height-y-1)*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  dest[(e->video_height-y-1)*e->video_width+(e->video_width-x-1)] = src[y*e->video_width+x] ^ priv->color;
  }
  }
  }

static void mirror_ur(effect * e, RGB32 *src, RGB32 *dest)
  {
  int x, y;
  simuratv_t * priv = e->priv;

  for(y=0; y<priv->hheight; y++) {
  for(x=priv->hwidth; x<e->video_width; x++) {
  dest[y*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  dest[y*e->video_width+(e->video_width-x-1)] = src[y*e->video_width+x] ^ priv->color;
  dest[(e->video_height-y-1)*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  dest[(e->video_height-y-1)*e->video_width+(e->video_width-x-1)] = src[y*e->video_width+x] ^ priv->color;
  }
  }
  }

static void mirror_dl(effect * e, RGB32 *src, RGB32 *dest)
  {
  int x, y;
  simuratv_t * priv = e->priv;

  for(y=priv->hheight; y<e->video_height; y++) {
  for(x=0; x<priv->hwidth; x++) {
  dest[y*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  dest[y*e->video_width+(e->video_width-x-1)] = src[y*e->video_width+x] ^ priv->color;
  dest[(e->video_height-y-1)*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  dest[(e->video_height-y-1)*e->video_width+(e->video_width-x-1)] = src[y*e->video_width+x] ^ priv->color;
  }
  }
  }

static void mirror_dr(effect * e, RGB32 *src, RGB32 *dest)
  {
  int x, y;
  simuratv_t * priv = e->priv;

  for(y=priv->hheight; y<e->video_height; y++) {
  for(x=priv->hwidth; x<e->video_width; x++) {
  dest[y*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  dest[y*e->video_width+(e->video_width-x-1)] = src[y*e->video_width+x] ^ priv->color;
  dest[(e->video_height-y-1)*e->video_width+x] = src[y*e->video_width+x] ^ priv->color;
  dest[(e->video_height-y-1)*e->video_width+(e->video_width-x-1)] = src[y*e->video_width+x] ^ priv->color;
  }
  }
  }

static const bg_parameter_info_t parameters[] =
  {
    {
      .name =      "color",
      .long_name = TRS("Color"),
      .type = BG_PARAMETER_COLOR_RGB,
      .flags = BG_PARAMETER_SYNC,
      .val_default = GAVL_VALUE_INIT_COLOR_RGB(0.0, 0.0, 0.0),
    },
    {
      .name =      "mirror_mode",
      .long_name = TRS("Mirror mode"),
      .type = BG_PARAMETER_STRINGLIST,
      .flags = BG_PARAMETER_SYNC,
      .val_default = GAVL_VALUE_INIT_STRING("no"),
      .multi_names = (char const *[]){ "no", "l", "r", "d", "dl", "dr", "u", "ul", "ur", NULL },
      .multi_labels = (char const *[]){ TRS("None"), TRS("Left"), TRS("Right"), TRS("Down"), TRS("Down left"), TRS("Down right"), TRS("Upper"), TRS("Upper left"), TRS("Upper right"), NULL },
    },
    { /* End of parameters */ },
  };

static const bg_parameter_info_t * get_parameters(void * data)
  {
  return parameters;
  }

static void set_parameter(void * data, const char * name,
                          const gavl_value_t *val)
  {
  bg_effectv_plugin_t * vp = data;
  simuratv_t * priv = vp->e->priv;

  if(!name)
    return;
  EFFECTV_SET_PARAM_COLOR(color);

  if(!strcmp(name, "mirror_mode"))
    {
    if(!strcmp(val->v.str, "no"))
      priv->mirror = 0;
    if(!strcmp(val->v.str, "l"))
      priv->mirror = 1;
    if(!strcmp(val->v.str, "r"))
      priv->mirror = 2;
    if(!strcmp(val->v.str, "d"))
      priv->mirror = 3;
    if(!strcmp(val->v.str, "dl"))
      priv->mirror = 4;
    if(!strcmp(val->v.str, "dr"))
      priv->mirror = 5;
    if(!strcmp(val->v.str, "u"))
      priv->mirror = 6;
    if(!strcmp(val->v.str, "ul"))
      priv->mirror = 7;
    if(!strcmp(val->v.str, "ur"))
      priv->mirror = 8;
    }
  
  }
static void * create_simuratv()
  {
  return bg_effectv_create(simuraRegister, 0);
  }



const bg_fv_plugin_t the_plugin = 
  {
    .common =
    {
      BG_LOCALE,
      .name =      "fv_simuratv",
      .long_name = TRS("SimuraTV"),
      .description = TRS("The origin of SimuraTV is \"SimuraEffect\", a VJ (Video Jockey) tool I made in 1995. Color effect and image mirroring are all of SimuraTV. Ported from EffecTV (http://effectv.sourceforge.net)."),
      .type =     BG_PLUGIN_FILTER_VIDEO,
      .flags =    BG_PLUGIN_FILTER_1,
      .create =   create_simuratv,
      .destroy =   bg_effectv_destroy,
      .get_parameters =   get_parameters,
      .set_parameter =    set_parameter,
      .priority =         1,
    },
    
    .connect = bg_effectv_connect,
  };

/* Include this into all plugin modules exactly once
   to let the plugin loader obtain the API version */
BG_GET_PLUGIN_API_VERSION;
