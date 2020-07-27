/*
 * Copyright © 2018 Benjamin Otte
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Benjamin Otte <otte@gnome.org>
 */

#include "gtkgstmediastreamprivate.h"
#include "gtkgstpaintableprivate.h"

#include <gst/player/gstplayer.h>
#include <gst/player/gstplayer-g-main-context-signal-dispatcher.h>

struct _GtkGstMediaStream
{
  GtkMediaStream parent_instance;

  GdkPaintable *paintable;
};

struct _GtkGstMediaStreamClass
{
  GtkMediaStreamClass parent_class;
};

#define TO_GST_TIME(ts) ((ts) * (GST_SECOND / G_USEC_PER_SEC))
#define FROM_GST_TIME(ts) ((ts) / (GST_SECOND / G_USEC_PER_SEC))

static void
gtk_gst_media_stream_paintable_snapshot (GdkPaintable *paintable,
                                       GdkSnapshot  *snapshot,
                                       double        width,
                                       double        height)
{
  GtkGstMediaStream *self = GTK_GST_MEDIA_STREAM (paintable);

  gdk_paintable_snapshot (self->paintable, snapshot, width, height);
}

static GdkPaintable *
gtk_gst_media_stream_paintable_get_current_image (GdkPaintable *paintable)
{
  GtkGstMediaStream *self = GTK_GST_MEDIA_STREAM (paintable);

  return gdk_paintable_get_current_image (self->paintable);
}

static int
gtk_gst_media_stream_paintable_get_intrinsic_width (GdkPaintable *paintable)
{
  GtkGstMediaStream *self = GTK_GST_MEDIA_STREAM (paintable);

  return gdk_paintable_get_intrinsic_width (self->paintable);
}

static int
gtk_gst_media_stream_paintable_get_intrinsic_height (GdkPaintable *paintable)
{
  GtkGstMediaStream *self = GTK_GST_MEDIA_STREAM (paintable);

  return gdk_paintable_get_intrinsic_height (self->paintable);
}

static double gtk_gst_media_stream_paintable_get_intrinsic_aspect_ratio (GdkPaintable *paintable)
{
  GtkGstMediaStream *self = GTK_GST_MEDIA_STREAM (paintable);

  return gdk_paintable_get_intrinsic_aspect_ratio (self->paintable);
};

static void
gtk_gst_media_stream_paintable_init (GdkPaintableInterface *iface)
{
  iface->snapshot = gtk_gst_media_stream_paintable_snapshot;
  iface->get_current_image = gtk_gst_media_stream_paintable_get_current_image;
  iface->get_intrinsic_width = gtk_gst_media_stream_paintable_get_intrinsic_width;
  iface->get_intrinsic_height = gtk_gst_media_stream_paintable_get_intrinsic_height;
  iface->get_intrinsic_aspect_ratio = gtk_gst_media_stream_paintable_get_intrinsic_aspect_ratio;
}

G_DEFINE_TYPE_EXTENDED (GtkGstMediaStream, gtk_gst_media_stream, GTK_TYPE_MEDIA_STREAM, 0,
                        G_IMPLEMENT_INTERFACE (GDK_TYPE_PAINTABLE,
                                               gtk_gst_media_stream_paintable_init))


static gboolean
gtk_gst_media_stream_play (GtkMediaStream *stream)
{
  return TRUE;
}

static void
gtk_gst_media_stream_pause (GtkMediaStream *stream)
{
}

static void
gtk_gst_media_stream_seek (GtkMediaStream *stream,
                         gint64          timestamp)
{
}

static void
gtk_gst_media_stream_update_audio (GtkMediaStream *stream,
                                 gboolean        muted,
                                 double          volume)
{
}

static void
gtk_gst_media_stream_dispose (GObject *object)
{
  GtkGstMediaStream *self = GTK_GST_MEDIA_STREAM (object);

  if (self->paintable) {
    g_signal_handlers_disconnect_by_func (self->paintable, gdk_paintable_invalidate_size, self);
    g_signal_handlers_disconnect_by_func (self->paintable, gdk_paintable_invalidate_contents, self);
    g_clear_object (&self->paintable);
  }

  G_OBJECT_CLASS (gtk_gst_media_stream_parent_class)->dispose (object);
}

static void
gtk_gst_media_stream_class_init (GtkGstMediaStreamClass *klass)
{
  GtkMediaStreamClass *stream_class = GTK_MEDIA_STREAM_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  stream_class->play = gtk_gst_media_stream_play;
  stream_class->pause = gtk_gst_media_stream_pause;
  stream_class->seek = gtk_gst_media_stream_seek;
  stream_class->update_audio = gtk_gst_media_stream_update_audio;

  gobject_class->dispose = gtk_gst_media_stream_dispose;
}

static void
gtk_gst_media_stream_init (GtkGstMediaStream *self)
{
}


void
gtk_gst_media_stream_set_sink (GtkGstMediaStream *self, GdkPaintable *paintable)
{
  g_set_object (&self->paintable, paintable);
  g_signal_connect_swapped (self->paintable, "invalidate-size", G_CALLBACK (gdk_paintable_invalidate_size), self);
  g_signal_connect_swapped (self->paintable, "invalidate-contents", G_CALLBACK (gdk_paintable_invalidate_contents), self);
}
