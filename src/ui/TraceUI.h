//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;
	Fl_Slider*			m_conDASlider;
	Fl_Slider*			m_linDASlider;
	Fl_Slider*			m_quaDASlider;
	Fl_Slider*			m_DistScaleSlider;
	Fl_Slider*			m_SubPixelSlider;
	Fl_Slider*			m_Termination;
	int					num_Sliders = 7;

	static Fl_Menu_Item	AntialiasingModeMenu[5];
	Fl_Choice*			m_antialModeChoice;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();
	void installSlider(Fl_Slider* &slider, int indx, const char* name, double Min, double Max, double step, double initval, void (*cb)(Fl_Widget*, void*));
	void installChoice(Fl_Choice* &Choice, int indx, const char* name, Fl_Menu_Item* menu, void (*cb)(Fl_Widget*, void*));

	void		setRayTracer(RayTracer *tracer);

	int			getSize();
	int			getDepth();
	double*		getDACoeff() {return m_nDACoeff;}
	double		getDistScale() { return m_nDistScale; }
	int			getSubPixels() { return m_nSubPixels; }
	double      getThreshold() { return m_nThreshold; }

private:
	RayTracer*	raytracer;

	int			m_nSize;
	int			m_nDepth;
	double		m_nDACoeff[3];
	double		m_nDistScale;
	int			m_nSubPixels;
	double		m_nThreshold;

// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);
	static void cb_conDASlides(Fl_Widget* o, void* v);
	static void cb_linDASlides(Fl_Widget* o, void* v);
	static void cb_quaDASlides(Fl_Widget* o, void* v);
	static void cb_DistScaleSlides(Fl_Widget* o, void* v);
	static void cb_SubPixelSlides(Fl_Widget* o, void* v);
	static void cb_Threshold(Fl_Widget* o, void* v);

	static void cb_antialModeChoice(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
};

#endif
