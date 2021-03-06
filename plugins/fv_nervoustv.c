/*
 * EffecTV - Realtime Digital Video Effector
 * Copyright (C) 2001-2006 FUKUCHI Kentaro
 *
 * nervousTV - The name says it all...
 * Copyright (C) 2002 TANNENBAUM Edo
 *
 * 2002/2/9 
 *   Original code copied same frame twice, and did not use memcpy().
 *   I modifed those point.
 *   -Kentaro Fukuchi
 */

#include <stdlib.h>
#include <string.h>
#include <gmerlin_effectv.h>
#include "utils.h"

#define PLANES 32

static int start(effect * e);
static int stop(effect * e);
static int draw(effect * e, RGB32 *src, RGB32 *dest);


typedef struct
  {
  int state;
  RGB32 *buffer;
  RGB32 *planetable[PLANES];
  int mode;
  int plane;
  int stock;
  int timer;
  int stride;
  int readplane;
  } nervous_t;


static effect *nervousRegister(void)
  {
  effect *entry;
  nervous_t * priv;
  
  entry = calloc(1, sizeof(effect));
  if(entry == NULL)
    {
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
  int i;
  nervous_t * priv = e->priv;
  
  priv->buffer = malloc(e->video_area*PIXEL_SIZE*PLANES);
  if(priv->buffer == NULL)
    return -1;
  memset(priv->buffer, 0, e->video_area*PIXEL_SIZE*PLANES);
  for(i=0;i<PLANES;i++)
    priv->planetable[i] = &priv->buffer[e->video_area*i];
  
  priv->plane = 0;
  priv->stock = 0;
  priv->timer = 0;
  priv->readplane = 0;
  
  priv->state = 1;
  return 0;
  }

static int stop(effect * e)
  {
  nervous_t * priv = e->priv;
  if(priv->state)
    {
    if(priv->buffer)
      free(priv->buffer);
    priv->state = 0;
    }
  return 0;
  }

static int draw(effect * e, RGB32 *src, RGB32 *dest)
  {
  nervous_t * priv = e->priv;
  memcpy(priv->planetable[priv->plane], src, e->video_area * PIXEL_SIZE);
  if(priv->stock < PLANES)
    {
    priv->stock++;
    }
  
  if(priv->mode)
    {
    if(priv->timer)
      {
      priv->readplane = priv->readplane + priv->stride;
      while(priv->readplane < 0) priv->readplane += priv->stock;
      while(priv->readplane >= priv->stock) priv->readplane -= priv->stock;
      priv->timer--;
      }
    else
      {
      priv->readplane = inline_fastrand(e) % priv->stock;
      priv->stride = inline_fastrand(e) % 5 - 2;
      if(priv->stride >= 0) priv->stride++;
      priv->timer = inline_fastrand(e) % 6 + 2;
      }
    }
  else
    {
    if(priv->stock > 0)
      priv->readplane = inline_fastrand(e) % priv->stock;
    }
  memcpy(dest, priv->planetable[priv->readplane], e->video_area * PIXEL_SIZE);
  priv->plane++;
  if (priv->plane == PLANES) priv->plane=0;
  
  return 0;
  }


static const bg_parameter_info_t parameters[] =
  {
    {
      .name =      "mode",
      .long_name = TRS("Mode"),
      .type = BG_PARAMETER_STRINGLIST,
      .flags = BG_PARAMETER_SYNC,
      .val_default = GAVL_VALUE_INIT_STRING("random"),
      .multi_names = (char const *[]){ "random", "scratch", NULL },
      .multi_labels = (char const *[]){ TRS("Random"), TRS("Scratch"), NULL },
    },
    { }
  };

static const bg_parameter_info_t * get_parameters(void * data)
  {
  return parameters;
  }

static void set_parameter(void * data, const char * name,
                          const gavl_value_t *val)
  {
  bg_effectv_plugin_t * vp = data;
  nervous_t * priv = vp->e->priv;
  
  if(!name)
    return;
  if(!strcmp(name, "mode"))
    {
    if(!strcmp(val->v.str, "random"))
      priv->mode = 0;
    else
      priv->mode = 1;
    }
  }

static void * create_nervoustv()
  {
  return bg_effectv_create(nervousRegister, BG_EFFECTV_COLOR_AGNOSTIC);
  }

const bg_fv_plugin_t the_plugin = 
  {
    .common =
    {
      BG_LOCALE,
      .name =      "fv_nervoustv",
      .long_name = TRS("NervousTV"),
      .description = TRS("NervousTV randomly re-arranges the past second of video, creating a chaotic expression of past movements. It shuffles time. There is no effect on still images. Ported from EffecTV (http://effectv.sourceforge.net)."),
      .type =     BG_PLUGIN_FILTER_VIDEO,
      .flags =    BG_PLUGIN_FILTER_1,
      .create =   create_nervoustv,
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
