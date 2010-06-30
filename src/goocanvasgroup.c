/*
 * GooCanvas. Copyright (C) 2005 Damon Chaplin.
 * Released under the GNU LGPL license. See COPYING for details.
 *
 * goocanvasgroup.c - group item.
 */

/**
 * SECTION:goocanvasgroup
 * @Title: GooCanvasGroup
 * @Short_Description: a group of items.
 *
 * #GooCanvasGroup represents a group of items. Groups can be nested to
 * any depth, to create a hierarchy of items. Items are ordered within each
 * group, with later items being displayed above earlier items.
 *
 * #GooCanvasGroup is a subclass of #GooCanvasItemSimple and so
 * inherits all of the style properties such as "stroke-color", "fill-color"
 * and "line-width". Setting a style property on a #GooCanvasGroup will affect
 * all children of the #GooCanvasGroup (unless the children override the
 * property setting).
 *
 * #GooCanvasGroup implements the #GooCanvasItem interface, so you can use
 * the #GooCanvasItem functions such as goo_canvas_item_raise() and
 * goo_canvas_item_rotate(), and the properties such as "visibility" and
 * "pointer-events".
 *
 * If the #GooCanvasGroup:width and #GooCanvasGroup:height properties are
 * set to positive values then the group is clipped to the given size.
 *
 * To create a #GooCanvasGroup use goo_canvas_group_new().
 *
 * To get or set the properties of an existing #GooCanvasGroup, use
 * g_object_get() and g_object_set().
 */
#include <config.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "goocanvasprivate.h"
#include "goocanvasgroup.h"
#include "goocanvas.h"
#include "goocanvasmarshal.h"
#include "goocanvasatk.h"


enum {
  PROP_0,

  PROP_X,
  PROP_Y,
  PROP_WIDTH,
  PROP_HEIGHT
};


G_DEFINE_TYPE (GooCanvasGroup, goo_canvas_group, GOO_TYPE_CANVAS_ITEM_SIMPLE)


static void
goo_canvas_group_init (GooCanvasGroup *group)
{
  group->x = 0.0;
  group->y = 0.0;
  group->width = -1.0;
  group->height = -1.0;
}


/**
 * goo_canvas_group_new:
 * @parent: the parent item, or %NULL. If a parent is specified, it will assume
 *  ownership of the item, and the item will automatically be freed when it is
 *  removed from the parent. Otherwise call g_object_unref() to free it.
 * @...: optional pairs of property names and values, and a terminating %NULL.
 * 
 * Creates a new group item.
 * 
 * Return value: a new group item.
 **/
GooCanvasItem*
goo_canvas_group_new (GooCanvasItem *parent,
		      ...)
{
  GooCanvasItem *item;
  GooCanvasGroup *group;
  va_list var_args;
  const char *first_property;

  item = g_object_new (GOO_TYPE_CANVAS_GROUP, NULL);
  group = (GooCanvasGroup*) item;

  va_start (var_args, parent);
  first_property = va_arg (var_args, char*);
  if (first_property)
    g_object_set_valist (G_OBJECT (item), first_property, var_args);
  va_end (var_args);

  if (parent)
    {
      goo_canvas_item_add_child (parent, item, -1);
      g_object_unref (item);
    }

  return item;
}


static void
goo_canvas_group_get_property (GObject               *object,
                               guint                  prop_id,
                               GValue                *value,
                               GParamSpec            *pspec)
{
  GooCanvasGroup *group = (GooCanvasGroup*) object;

  switch (prop_id)
    {
    case PROP_X:
      g_value_set_double (value, group->x);
      break;
    case PROP_Y:
      g_value_set_double (value, group->y);
      break;
    case PROP_WIDTH:
      g_value_set_double (value, group->width);
      break;
    case PROP_HEIGHT:
      g_value_set_double (value, group->height);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
goo_canvas_group_set_property (GObject                  *object,
                               guint                     prop_id,
                               const GValue             *value,
                               GParamSpec               *pspec)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) object;
  GooCanvasGroup *group = (GooCanvasGroup*) object;

  switch (prop_id)
    {
    case PROP_X:
      group->x = g_value_get_double (value);
      break;
    case PROP_Y:
      group->y = g_value_get_double (value);
      break;
    case PROP_WIDTH:
      group->width = g_value_get_double (value);
      break;
    case PROP_HEIGHT:
      group->height = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }

  goo_canvas_item_simple_changed (simple, TRUE);
}


static void
goo_canvas_group_update  (GooCanvasItem   *item,
			  gboolean         entire_tree,
			  cairo_t         *cr,
			  GooCanvasBounds *bounds)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  GooCanvasBounds child_bounds;
  gboolean initial_bounds = TRUE;
  gint i;

  if (entire_tree || simple->need_update)
    {
      if (simple->need_entire_subtree_update)
	entire_tree = TRUE;

      simple->need_update = FALSE;
      simple->need_entire_subtree_update = FALSE;

      simple->bounds.x1 = simple->bounds.y1 = 0.0;
      simple->bounds.x2 = simple->bounds.y2 = 0.0;

      if (simple->children)
	{
	  cairo_save (cr);
	  if (simple->transform)
	    cairo_transform (cr, simple->transform);

	  cairo_translate (cr, group->x, group->y);

	  for (i = 0; i < simple->children->len; i++)
	    {
	      GooCanvasItem *child = simple->children->pdata[i];

	      goo_canvas_item_update (child, entire_tree, cr, &child_bounds);
          
	      /* If the child has non-empty bounds, compute the union. */
	      if (child_bounds.x1 < child_bounds.x2
		  && child_bounds.y1 < child_bounds.y2)
		{
		  if (initial_bounds)
		    {
		      simple->bounds.x1 = child_bounds.x1;
		      simple->bounds.y1 = child_bounds.y1;
		      simple->bounds.x2 = child_bounds.x2;
		      simple->bounds.y2 = child_bounds.y2;
		      initial_bounds = FALSE;
		    }
		  else
		    {
		      simple->bounds.x1 = MIN (simple->bounds.x1, child_bounds.x1);
		      simple->bounds.y1 = MIN (simple->bounds.y1, child_bounds.y1);
		      simple->bounds.x2 = MAX (simple->bounds.x2, child_bounds.x2);
		      simple->bounds.y2 = MAX (simple->bounds.y2, child_bounds.y2);
		    }
		}
	    }

	  cairo_restore (cr);
	}
    }

#if 0
  g_print ("Group '%s' bounds with children: %g, %g  %g x %g\n",
	   (gchar*) g_object_get_data (G_OBJECT (simple), "id"),
	   simple->bounds.x1, simple->bounds.y1,
	   simple->bounds.x2 - simple->bounds.x1,
	   simple->bounds.y2 - simple->bounds.y1);
#endif

  *bounds = simple->bounds;
}


static GList*
goo_canvas_group_get_items_at (GooCanvasItem  *item,
			       gdouble         x,
			       gdouble         y,
			       cairo_t        *cr,
			       gboolean        is_pointer_event,
			       gboolean        parent_visible,
			       GList          *found_items)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  gboolean visible = parent_visible;
  int i;

  if (simple->need_update)
    goo_canvas_item_ensure_updated (item);

  /* Skip the item if the point isn't in the item's bounds. */
  if (simple->bounds.x1 > x || simple->bounds.x2 < x
      || simple->bounds.y1 > y || simple->bounds.y2 < y)
    return found_items;

  if (simple->visibility <= GOO_CANVAS_ITEM_INVISIBLE
      || (simple->visibility == GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD
	  && simple->canvas->scale < simple->visibility_threshold))
    visible = FALSE;

  /* Check if the group should receive events. */
  if (is_pointer_event
      && (simple->pointer_events == GOO_CANVAS_EVENTS_NONE
	  || ((simple->pointer_events & GOO_CANVAS_EVENTS_VISIBLE_MASK)
	      && !visible)))
    return found_items;

  cairo_save (cr);
  if (simple->transform)
    cairo_transform (cr, simple->transform);

  cairo_translate (cr, group->x, group->y);

  /* If the group has a clip path, check if the point is inside it. */
  if (simple->clip_path_commands)
    {
      double user_x = x, user_y = y;

      cairo_device_to_user (cr, &user_x, &user_y);
      goo_canvas_create_path (simple->clip_path_commands, cr);
      cairo_set_fill_rule (cr, simple->clip_fill_rule);
      if (!cairo_in_fill (cr, user_x, user_y))
	{
	  cairo_restore (cr);
	  return found_items;
	}
    }

  if (group->width > 0.0 && group->height > 0.0)
    {
      double user_x = x, user_y = y;

      cairo_device_to_user (cr, &user_x, &user_y);
      if (user_x < 0.0 || user_x >= group->width
	  || user_y < 0.0 || user_y >= group->height)
	{
	  cairo_restore (cr);
	  return found_items;
	}
    }

  /* Step up from the bottom of the children to the top, adding any items
     found to the start of the list. */
  if (simple->children)
    {
      for (i = 0; i < simple->children->len; i++)
	{
	  GooCanvasItem *child = simple->children->pdata[i];

	  found_items = goo_canvas_item_get_items_at (child, x, y, cr,
						      is_pointer_event, visible,
						      found_items);
	}
    }

  cairo_restore (cr);

  return found_items;
}


static void
goo_canvas_group_paint (GooCanvasItem         *item,
			cairo_t               *cr,
			const GooCanvasBounds *bounds,
			gdouble                scale)
{
  GooCanvasItemSimple *simple = (GooCanvasItemSimple*) item;
  GooCanvasGroup *group = (GooCanvasGroup*) item;
  gint i;

  /* Skip the item if the bounds don't intersect the expose rectangle. */
  if (simple->bounds.x1 > bounds->x2 || simple->bounds.x2 < bounds->x1
      || simple->bounds.y1 > bounds->y2 || simple->bounds.y2 < bounds->y1)
    return;

  /* Check if the item should be visible. */
  if (simple->visibility <= GOO_CANVAS_ITEM_INVISIBLE
      || (simple->visibility == GOO_CANVAS_ITEM_VISIBLE_ABOVE_THRESHOLD
	  && scale < simple->visibility_threshold))
    return;

  /* Paint all the items in the group. */
  cairo_save (cr);
  if (simple->transform)
    cairo_transform (cr, simple->transform);

  cairo_translate (cr, group->x, group->y);

  /* Clip with the group's clip path, if it is set. */
  if (simple->clip_path_commands)
    {
      goo_canvas_create_path (simple->clip_path_commands, cr);
      cairo_set_fill_rule (cr, simple->clip_fill_rule);
      cairo_clip (cr);
    }

  if (group->width > 0.0 && group->height > 0.0)
    {
      cairo_rectangle (cr, 0.0, 0.0, group->width, group->height);
      cairo_clip (cr);
    }

  if (simple->children)
    {
      for (i = 0; i < simple->children->len; i++)
	{
	  GooCanvasItem *child = simple->children->pdata[i];
	  goo_canvas_item_paint (child, cr, bounds, scale);
	}
    }

  cairo_restore (cr);
}


static void
goo_canvas_group_class_init (GooCanvasGroupClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*) klass;
  GooCanvasItemClass *item_class = (GooCanvasItemClass*) klass;

  gobject_class->get_property = goo_canvas_group_get_property;
  gobject_class->set_property = goo_canvas_group_set_property;

  item_class->get_items_at   = goo_canvas_group_get_items_at;
  item_class->update         = goo_canvas_group_update;
  item_class->paint          = goo_canvas_group_paint;

  /* Register our accessible factory, but only if accessibility is enabled. */
  if (!ATK_IS_NO_OP_OBJECT_FACTORY (atk_registry_get_factory (atk_get_default_registry (), GTK_TYPE_WIDGET)))
    {
      atk_registry_set_factory_type (atk_get_default_registry (),
				     GOO_TYPE_CANVAS_GROUP,
				     goo_canvas_item_accessible_factory_get_type ());
    }

  g_object_class_install_property (gobject_class, PROP_X,
                                  g_param_spec_double ("x",
                                                       "X",
                                                       _("The x coordinate of the group"),
                                                       -G_MAXDOUBLE,
                                                       G_MAXDOUBLE, 0.0,
                                                       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_Y,
                                  g_param_spec_double ("y",
                                                       "Y",
                                                       _("The y coordinate of the group"),
                                                       -G_MAXDOUBLE,
                                                       G_MAXDOUBLE, 0.0,
                                                       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_WIDTH,
                                  g_param_spec_double ("width",
                                                       _("Width"),
                                                       _("The width of the group, or -1 to use the default width"),
                                                       -G_MAXDOUBLE,
                                                       G_MAXDOUBLE, -1.0,
                                                       G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_HEIGHT,
                                  g_param_spec_double ("height",
                                                       _("Height"),
                                                       _("The height of the group, or -1 to use the default height"),
                                                       -G_MAXDOUBLE,
                                                       G_MAXDOUBLE, -1.0,
                                                       G_PARAM_READWRITE));
}
