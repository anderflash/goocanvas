/*
 * GooCanvas. Copyright (C) 2005 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvasellipse.h - ellipse item.
 */
#ifndef __GOO_CANVAS_ELLIPSE_H__
#define __GOO_CANVAS_ELLIPSE_H__

#include <gtk/gtk.h>
#include "goocanvasitemsimple.h"

G_BEGIN_DECLS


#define GOO_TYPE_CANVAS_ELLIPSE            (goo_canvas_ellipse_get_type ())
#define GOO_CANVAS_ELLIPSE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOO_TYPE_CANVAS_ELLIPSE, GooCanvasEllipse))
#define GOO_CANVAS_ELLIPSE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GOO_TYPE_CANVAS_ELLIPSE, GooCanvasEllipseClass))
#define GOO_IS_CANVAS_ELLIPSE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOO_TYPE_CANVAS_ELLIPSE))
#define GOO_IS_CANVAS_ELLIPSE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GOO_TYPE_CANVAS_ELLIPSE))
#define GOO_CANVAS_ELLIPSE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GOO_TYPE_CANVAS_ELLIPSE, GooCanvasEllipseClass))


typedef struct _GooCanvasEllipse       GooCanvasEllipse;
typedef struct _GooCanvasEllipseClass  GooCanvasEllipseClass;

/**
 * GooCanvasEllipse
 *
 * The #GooCanvasEllipse-struct struct contains private data only.
 */
struct _GooCanvasEllipse
{
  GooCanvasItemSimple parent_object;

  gdouble center_x, center_y, radius_x, radius_y;
};

struct _GooCanvasEllipseClass
{
  GooCanvasItemSimpleClass parent_class;

  /*< private >*/

  /* Padding for future expansion */
  void (*_goo_canvas_reserved1) (void);
  void (*_goo_canvas_reserved2) (void);
  void (*_goo_canvas_reserved3) (void);
  void (*_goo_canvas_reserved4) (void);
};


GType               goo_canvas_ellipse_get_type  (void) G_GNUC_CONST;

GooCanvasItem*      goo_canvas_ellipse_new	 (GooCanvasItem      *parent,
						  gdouble             center_x,
						  gdouble             center_y,
						  gdouble             radius_x,
						  gdouble             radius_y,
						  ...);



G_END_DECLS

#endif /* __GOO_CANVAS_ELLIPSE_H__ */
