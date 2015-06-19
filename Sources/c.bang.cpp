/*
 * CicmWrapper
 *
 * A wrapper for Pure Data
 *
 * Copyright (C) 2013 Pierre Guillot, CICM - Université Paris 8
 * All rights reserved.
 *
 * Website  : http://www.mshparisnord.fr/HoaLibrary/
 * Contacts : cicm.mshparisnord@gmail.com
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published
 * by the Free Software Foundation; either version 2 of the License.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

/*!
 * \example c.bang.cpp
 * \brief This is an example of how to use and create a basic GUI.
 * \details This example shows how to initialize a GUI class with methods for painting and mouse interractions and attributes.
 */

//
// c.bang.cpp
// This is an example of how to use and create a basic GUI.
// This example shows how to initialize a GUI class with methods for painting and mouse interractions and attributes.
//

// This header includes cicm_wrapped.h
#include "../c.library.h"

/*! @addtogroup groupbang The Bang Example Part
 * @brief The example part for t_ebox.
 * @details This part is an example shows how to initialize a GUI class with methods for painting and mouse interractions and attributes.
 *  @{
 */

//
// The GUI bang structure.
// It is a basic GUI struture with attributes and basic Pd stuffs.
//
/**
 * @example c.bang.cpp
 * @struct t_bang
 * @brief The GUI bang structure.
 * @details It is a basic GUI struture with attributes and basic Pd stuffs.
 */
typedef struct t_bang
{
    // The t_ebox that allows to create a GUI.
	t_ebox      b_box;              /*!< The t_ebox that allows to create a GUI. */
    // The t_outlet of the object.
    t_outlet*   b_out;              /*!< The t_outlet of the object. */
    // The struture for the t_eattr background color.
	t_rgba		b_color_background; /*!< The struture for the t_eattr background color. */
    // The struture for the t_eattr border color.
	t_rgba		b_color_border;     /*!< The struture for the t_eattr border color. */
    // The struture for the t_eattr bang color.
	t_rgba		b_color_bang;       /*!< The struture for the t_eattr bang color. */
    // The t_clock of the object.
    t_clock*    b_clock;            /*!< The t_clock of the object. */
    // If the object is performming a bang.
    char        b_active;           /*!< If the object is performming a bang. */
} t_bang;

// The t_eclass for the t_bang.
/*! The t_eclass for the t_bang. */
static t_eclass *bang_class;

//! @cond
static void *bang_new(t_symbol *s, int argc, t_atom *argv);
static void bang_free(t_bang *x);
static void bang_activate(t_bang *x);
static void bang_deactivate(t_bang *x);
static void bang_anything(t_bang *x, t_symbol *s, int argc, t_atom *argv);
static void bang_getdrawparams(t_bang *x, t_object *patcherview, t_edrawparams *params);
static void bang_oksize(t_bang *x, t_rect *newrect);
static void bang_paint(t_bang *x, t_object *view);
static void bang_mousedown(t_bang *x, t_object *patcherview, t_pt pt, long modifiers);
static void bang_mouseup(t_bang *x, t_object *patcherview, t_pt pt, long modifiers);
//! @endcond

// Setups the bang_class for GUI behavior.
// This function is called by Pd to initialize the class of t_eclass of the object called c.bang (bang_class). In the function,
// you should set up the default method, behavior and attributes of your object. If your object's name is simple like just
// "bang",this function should be named "bang_setup" otherwise, if your object's name is more complex like "c.bang" the
// function's name should replace the special character with the matching UTF8-hexadecimal value and put "setup" before the name
// of the object. In this example, the dot is replaced with "0x2e" and the function name becomes "setup_c0x2ebang".
/*!
 * \fn          extern "C" void setup_c0x2ebang(void)
 * \brief       Setups the bang_class for GUI behavior.
 * \details     This function is called by Pd to initialize the class of t_eclass of the object called c.bang (bang_class). In the function, you should set up the default method, behavior and attributes of your object. If your object's name is simple like just \"bang\", this function should be named \"bang_setup\" otherwise, if your object's name is more complex like \"c.bang\" the function's name should replace the special character with the matching UTF8-hexadecimal value and put \"setup\" before the name of the object. In this example, the dot is replaced with \"c0x2e\" and the function name becomes \"setup_c0x2ebang\".
 */
extern "C" void setup_c0x2ebang(void)
{
    // We creates a new t_eclass for the t_bang structure.
    // You should use this method for all the objects that uses either the t_eobj, t_edspobj, t_ebox or t_edspbox structure.
    // First argument is the name of the object. Second argument is the new method of the object. Third argument is the free method
    // of the object (if you only have to call eobj_free, ebox_free or eobj_dspfree you can directly use this methods. Fourth
    // argument is the size of the structure. Fifth argument is the type of the object (you should use CLASS_DEFAULT for every
    // patchable object or CLASS_NOINLET if you don't want a default first inlet, CLASS_PD should be used for no patchable object).
    // Sixth argument is the kind parameters you creation method is excepting (for GUI, you should always use A_GIMME to be able
    // to parse attributes. The last argument is dummy and should always be zero for the moment.
    t_eclass *c = eclass_new("c.bang", (method)bang_new, (method)bang_free, (short)sizeof(t_bang), CLASS_DEFAULT, A_GIMME, 0);
    if(c)
    {
        // We initialize the defaults attributes and methods for the GUI.
        eclass_guiinit(c, 0);
        
        // We initialize of the specifics methods that the t_bang can receive. The "paint", "getdrawparams" and "oksize" names
        // are for the graphical methods ("getdrawparams" and "oksize" are optional but facilitates the setting of GUI.
        // The "mousedown" and "mouseup" are for the mouse interraction.
        // The other are the default message that a native Pd object can receive.
        eclass_addmethod(c, (method) bang_paint,           "paint",            A_NULL, 0);
        eclass_addmethod(c, (method) bang_getdrawparams,   "getdrawparams",    A_NULL, 0);
        eclass_addmethod(c, (method) bang_oksize,          "oksize",           A_NULL, 0);
        eclass_addmethod(c, (method) bang_anything,        "float",            A_FLOAT,0);
        eclass_addmethod(c, (method) bang_anything,        "bang",             A_NULL, 0);
        eclass_addmethod(c, (method) bang_anything,        "list",             A_GIMME,0);
        eclass_addmethod(c, (method) bang_anything,        "anything",         A_GIMME,0);
        eclass_addmethod(c, (method) bang_mousedown,       "mousedown",        A_NULL, 0);
        eclass_addmethod(c, (method) bang_mouseup,         "mouseup",          A_NULL, 0);
        
        // We intialize the attribute of the t_bang.
        // All the GUI classes has font attributes but we don't need them for the bang classe so we mark them invisible.
        CLASS_ATTR_INVISIBLE            (c, "fontname", 1);
        CLASS_ATTR_INVISIBLE            (c, "fontweight", 1);
        CLASS_ATTR_INVISIBLE            (c, "fontslant", 1);
        CLASS_ATTR_INVISIBLE            (c, "fontsize", 1);
        // All the GUI classes has a size attribute, we just set up the default value.
        CLASS_ATTR_DEFAULT              (c, "size", 0, "16. 16.");
        // We create a new t_rgba attribute that refers to the b_color_background member of the t_bang and that will match to
        // "bgcolor". The user will be able to change the background color with the "bgcolor" message.
        CLASS_ATTR_RGBA                 (c, "bgcolor", 0, t_bang, b_color_background);
        // We set up the label that will be displayed in the properties window of the object for the attribute.
        CLASS_ATTR_LABEL                (c, "bgcolor", 0, "Background Color");
        // We set up the order of the attribute in the properties window (this is unused for the moment).
        CLASS_ATTR_ORDER                (c, "bgcolor", 0, "1");
        // We set up the the default value of the color. This macro also defines that the attribute will automatically call ebox_redraw when its value has changed and that its value will be saved with the patcher.
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bgcolor", 0, "0.75 0.75 0.75 1.");
        // We set up the that the attribute should be displayed as a color slector in the properties window.
        CLASS_ATTR_STYLE                (c, "bgcolor", 0, "color");
        // We do the same thing for the border color and the bang color.
        CLASS_ATTR_RGBA                 (c, "bdcolor", 0, t_bang, b_color_border);
        CLASS_ATTR_LABEL                (c, "bdcolor", 0, "Border Color");
        CLASS_ATTR_ORDER                (c, "bdcolor", 0, "2");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bdcolor", 0, "0.5 0.5 0.5 1.");
        CLASS_ATTR_STYLE                (c, "bdcolor", 0, "color");
        
        CLASS_ATTR_RGBA                 (c, "bacolor", 0, t_bang, b_color_bang);
        CLASS_ATTR_LABEL                (c, "bacolor", 0, "Bang Color");
        CLASS_ATTR_ORDER                (c, "bacolor", 0, "3");
        CLASS_ATTR_DEFAULT_SAVE_PAINT   (c, "bacolor", 0, "0. 0. 0. 1.");
        CLASS_ATTR_STYLE                (c, "bacolor", 0, "color");
        
        // We register the class. This function is important it will set up some dsp members if needs and the properties window
        // if the class has attributes. The CLASS_BOX is for GUI otherwise use CLASS_OBJ this is pretty useless but it ensures
        // compatibilty with Max.
        eclass_register(CLASS_BOX, c);
        
        // We initialize bang_class with c. This is important because we use the adress of bang_class in the new method.
        bang_class = c;
    }
}

// Allocates and initializes the t_bang structure.
// The function uses eobj_new to allocate the t_bang with the bang_class and initializes the default values.
/*!
 * \fn          static void *bang_new(t_symbol *s, int argc, t_atom *argv)
 * \brief       Allocates and initializes the t_bang structure.
 * \details     The function uses eobj_new to allocate the t_bang with the bang_class and initializes the default values.
 * \param s     The name of the object (in this case \"c.bang\").
 * \param argc  The number of t_atom parameters.
 * \param argv  The array of t_atom parameters.
 * \return      The pointer to the t_bang if the allocation has been successful, otherwise NULL.
 */
static void *bang_new(t_symbol *s, int argc, t_atom *argv)
{
    // We calls eobj_new with the bang_class to allocate the memory for the t_bang and to initialize some default values
    // and methods.
    t_bang *x  = (t_bang *)eobj_new(bang_class);
    // We transform the array of t_atoms into a t_binbuf. You can alos use directly the array of t_atoms but you should prefer
    // to use a t_binbuf to increases the compatibilty with Max that use the it's counterpart t_dictionnary.
    t_binbuf* d = binbuf_via_atoms(argc,argv);
    if(x && d)
    {
        // We initialize the defaults members of the t_ebox. EBOX_GROWLINK mark that the width and the height of the
        // object are linked. The method also initializes the attributes with their defaults values.
        ebox_new((t_ebox *)x, 0 | EBOX_GROWLINK);
        // We allocate and intialize a new outlet that we send only bang message.
        x->b_out = outlet_new((t_object *)x, &s_bang);
        // We sets that the t_bang is currently inactive.
        x->b_active = 0;
        // We initialize a new t_clock to make the object blink few ms when it received a message. We usually define a
        // specific tick_method but here we can use the bang_deactivate method.
        x->b_clock          = clock_new(x,(t_method)bang_deactivate);
        // We parse the t_binbuf to initialize the values of the attributes. You can also use ebox_attrprocess_viatoms but you
        // should prefer this method.
        ebox_attrprocess_viabinbuf(x, d);
        // Indicates that the t_ebox can be drawn.
        ebox_ready((t_ebox *)x);
    }
     // Returns the t_bang
    return (x);
}

// Frees the t_bang structure.
// The function just calls ebox_free() and frees the t_clock.
/*!
 * \fn          static void bang_free(t_bang *x)
 * \brief       Frees the t_bang structure.
 * \details     The function just calls ebox_free() and frees the t_clock.
 * \param x     The t_bang pointer.
 */
static void bang_free(t_bang *x)
{
    // We free the t_ebox, it will detach the graphical interface from Tcl/Tk
    ebox_free((t_ebox *)x);
    // We free the t_clock
    clock_free(x->b_clock);
}

// Defines the default graphical parameters of a GUI.
// The function is used to defines the border size, the border size (dummy mostly for Max compatibility)
// the border color and the background color of a t_ebox.
/*!
 * \fn          static void bang_getdrawparams(t_bang *x, t_object *patcherview, t_edrawparams *params)
 * \brief       Defines the default graphical parameters of a GUI.
 * \details     The function is used to defines the border size, the border size (dummy mostly for Max compatibility) the border color and the background color of a t_ebox.
 * \param x         The t_bang pointer.
 * \param view      The view pointer (dummy).
 * \param params    The t_edrawparams structure to set up.
 */
static void bang_getdrawparams(t_bang *x, t_object *view, t_edrawparams *params)
{
    // We define a border size of 2 px.
	params->d_borderthickness   = 2;
    // We define a corner size of 2 px (dummy).
	params->d_cornersize        = 2;
    // We define the border color with our border attribute color.
    params->d_bordercolor       = x->b_color_border;
    // We define the background color with our border attribute color. The background color will be used when the
    // t_bang is inactive to draw the circle.
    params->d_boxfillcolor      = x->b_color_border;
}

// Defines and validates the size of a GUI.
// The function is validate the new size of a GUI. It can be used to restrict the size.
/*!
 * \fn          static void bang_oksize(t_bang *x, t_rect *newrect)
 * \brief       Defines and validates the size of a GUI.
 * \details     The function is validate the new size of a GUI. It can be used to restrict the size.
 * \param x         The t_bang pointer.
 * \param newrect   The t_rect pointer to change if needed.
 */
static void bang_oksize(t_bang *x, t_rect *newrect)
{
    // We defines a minimum height and width of 15 px.
    newrect->width = pd_clip_min(newrect->width, 15.);
    newrect->height = pd_clip_min(newrect->height, 15.);
    // We defines that the width and the height can't be an even number (to center the bang circle).
    if((int)newrect->width % 2 == 0)
        newrect->width++;
    if((int)newrect->height % 2 == 0)
        newrect->height++;
}

// Paints the t_ebox.
// The function is called internally when the t_ebox should be repainted. This is where you should draw all the stuffs
// you want.
/*!
 * \fn          static void bang_paint(t_bang *x, t_object *view)
 * \brief       Paints the t_ebox.
 * \details     The function is called internally when the t_ebox should be repainted. This is where you should draw all the stuffs
 you want.
 * \param x      The t_bang pointer.
 * \param view   The view that ask to be repainted (dummy mostly for Max compatibility).
 */
static void bang_paint(t_bang *x, t_object *view)
{
    float size;
    t_rect rect;
    // We defines a initialize a t_rect with the size of the t_ebox.
    ebox_get_rect_for_view((t_ebox *)x, &rect);
    // We ask to retrieves the background t_elayer with a that has the size of the t_ebox. This layer will be binded to the t_ebox
    // with the t_symbol cream_sym_background_layer aka \"background_layer\". You should always prefer to use static t_symbol to
    // avoid to call gensym function.
    t_elayer *g = ebox_start_layer((t_ebox *)x, cream_sym_background_layer, rect.width, rect.height);
    // If it is a new t_elayer or if the layer has been ivalidated we can draw something in it, otherwise the pointer is NULL.
    if(g)
    {
        
        size = rect.width * 0.5;
        // We set up the bang color is the t_bang is currently active.
        // Otherwise we use the background color.
        if(x->b_active)
        {
            egraphics_set_color_rgba(g, &x->b_color_bang);
        }
        else
        {
            egraphics_set_color_rgba(g, &x->b_color_background);
        }
        // We add a circle at the center the t_elayer.
        egraphics_circle(g, floor(size + 0.5), floor(size + 0.5), size * 0.9);
        // We fill the t_elayer with the drawing.
        egraphics_fill(g);
        // We mark the layer as ready to be painted.
        ebox_end_layer((t_ebox*)x, cream_sym_background_layer);
    }
    // We tell the t_ebox to painted the \"background_layer\" layer at the 0 0 position. If the t_elayer was invalid or new
    // that means that we have drawn something new in it, otherwise t_ebox paint the t_elayer like it was before.
    ebox_paint_layer((t_ebox *)x, cream_sym_background_layer, 0., 0.);
}

static void bang_mousedown(t_bang *x, t_object *patcherview, t_pt pt, long modifiers)
{
    bang_activate(x);
}

static void bang_mouseup(t_bang *x, t_object *patcherview, t_pt pt, long modifiers)
{
    bang_deactivate(x);
}

static void bang_anything(t_bang *x, t_symbol *s, int argc, t_atom *argv)
{
    bang_activate(x);
    clock_delay(x->b_clock, 100);
}

static void bang_activate(t_bang *x)
{
    x->b_active = 1;
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
    outlet_bang(x->b_out);
    if(ebox_getsender((t_ebox *) x))
        pd_bang(ebox_getsender((t_ebox *) x));
}

static void bang_deactivate(t_bang *x)
{
    x->b_active = 0;
    ebox_invalidate_layer((t_ebox *)x, cream_sym_background_layer);
    ebox_redraw((t_ebox *)x);
}


/** @} */





