/*
 * EffecTV - Realtime Digital Video Effector
 * Copyright (C) 2001-2006 FUKUCHI Kentaro
 *
 * ColourfulStreak - streak effect with color.
 *                   It blends Red, Green and Blue layers independently. The
 *                   number of frames for blending are different to each layers.
 * Copyright (C) 2005 Ryo-ta
 *
 * Ported to EffecTV by Kentaro Fukuchi
 *
 * This is heavy effect because of the 3 divisions per pixel.
 * If you want a light effect, you can declare 'blendnum' as a constant value,
 * and disable the parameter controll (comment out the inside of 'event()').
 *
 */

#include <stdlib.h>
#include <string.h>
#include <gmerlin_effectv.h>
#include "utils.h"

#define MAX_PLANES 32

static int start(effect*);
static int stop(effect*);
static int draw(effect*, RGB32 *src, RGB32 *dest);


typedef struct
  {
  int state;
  unsigned char *buffer;
  unsigned char *Rplane;
  unsigned char *Gplane;
  unsigned char *Bplane;
  int plane;
  int blendnum;
  } colstreaktv_t;

static effect *colstreakRegister(void)
{
	effect *entry;
        colstreaktv_t * priv;
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

static int start(effect*e)
{
        colstreaktv_t * priv = e->priv;
	priv->buffer = malloc(e->video_area * MAX_PLANES * 3);
	if(priv->buffer == NULL)
		return -1;
	memset(priv->buffer, 0, e->video_area * MAX_PLANES * 3);

	priv->Bplane = priv->buffer;
	priv->Gplane = priv->buffer + e->video_area * MAX_PLANES;
	priv->Rplane = priv->buffer + e->video_area * MAX_PLANES * 2;

	priv->plane = 0;

	priv->state = 1;
	return 0;
}

static int stop(effect*e)
{
        colstreaktv_t * priv = e->priv;
	if(priv->state) {
		if(priv->buffer) {
			free(priv->buffer);
			priv->buffer = NULL;
		}
		priv->state = 0;
	}
	return 0;
}

static int draw(effect*e, RGB32 *src, RGB32 *dest)
{
        colstreaktv_t * priv = e->priv;
	int i, j, cf, pf;
	RGB32 v;
	unsigned int R, G, B;
	unsigned char *pR, *pG, *pB;
        
	pf = priv->plane - 1;
	if(pf < 0) pf += MAX_PLANES;

	pB = priv->Bplane;
	pG = priv->Gplane;
	pR = priv->Rplane;

	for(i=0; i<e->video_area; i++) {
		v = *src++;
		B = (unsigned char)v;
		G = (unsigned char)(v >> 8);
		R = (unsigned char)(v >> 16);
		pB[priv->plane] = B;
		pG[priv->plane] = G;
		pR[priv->plane] = R;

		cf = pf;
		for(j=1; j<priv->blendnum; j++) {
			B += pB[cf];
			cf = (cf + MAX_PLANES - 1) & (MAX_PLANES - 1);
		}
		B /= priv->blendnum;

		cf = pf;
		for(j=1; j<priv->blendnum*2; j++) {
			G += pG[cf];
			cf = (cf + MAX_PLANES - 1) & (MAX_PLANES - 1);
		}
		G /= priv->blendnum * 2;

		cf = pf;
		for(j=1; j<priv->blendnum*3; j++) {
			R += pR[cf];
			cf = (cf + MAX_PLANES - 1) & (MAX_PLANES - 1);
		}
		R /= priv->blendnum * 3;
		*dest++ = (R<<16)|(G<<8)|B;

		pB += MAX_PLANES;
		pG += MAX_PLANES;
		pR += MAX_PLANES;
	}
	priv->plane++;
	priv->plane = priv->plane & (MAX_PLANES-1);

	return 0;
}

static const bg_parameter_info_t parameters[] =
  {
    {
      .name =      "blendnum",
      .long_name = TRS("Blend num"),
      .type = BG_PARAMETER_SLIDER_INT,
      .flags = BG_PARAMETER_SYNC,
      .val_min =     GAVL_VALUE_INIT_INT(1),
      .val_max =     GAVL_VALUE_INIT_INT(MAX_PLANES/3),
      .val_default = GAVL_VALUE_INIT_INT(4),
    },
    { /* */ },
  };

static const bg_parameter_info_t * get_parameters(void * data)
  {
  return parameters;
  }

static void set_parameter(void * data, const char * name,
                          const gavl_value_t *val)
  {
  bg_effectv_plugin_t * vp = data;
  colstreaktv_t * priv = vp->e->priv;
  if(!name)
    return;
  EFFECTV_SET_PARAM_INT(blendnum);
  }


static void * create_colstreaktv()
  {
  return bg_effectv_create(colstreakRegister, 0);
  }



const bg_fv_plugin_t the_plugin = 
  {
    .common =
    {
      BG_LOCALE,
      .name =      "fv_colstreaktv",
      .long_name = TRS("ColstreakTV"),
      .description = TRS("Make after images but the power of the effects are different between red, green and blue layers, so it provides colourful after images. Ported from EffecTV (http://effectv.sourceforge.net)."),
      .type =     BG_PLUGIN_FILTER_VIDEO,
      .flags =    BG_PLUGIN_FILTER_1,
      .create =   create_colstreaktv,
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
