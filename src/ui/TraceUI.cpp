//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"

int**** randnumbers; //2d array of random integers
extern TraceUI* traceUI;

static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ( (TraceUI*)(o->parent()->user_data()) );
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL );

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done=true;	// terminate the previous rendering
		} else{
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v) 
{
	TraceUI* pUI=whoami(o);
	
	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp" );
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI=whoami(o);

	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v) 
{
	TraceUI* pUI=(TraceUI *)(o->user_data());
	
	// terminate the rendering
	done=true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v) 
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI=(TraceUI*)(o->user_data());
	
	pUI->m_nSize=int( ((Fl_Slider *)o)->value() ) ;
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow( pUI->m_nSize, height );
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDepth=int( ((Fl_Slider *)o)->value() ) ;
}

void TraceUI::cb_conDASlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDACoeff[0] = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_linDASlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDACoeff[1] = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_quaDASlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDACoeff[2] = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_DistScaleSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nDistScale = pow(10.0,double(((Fl_Slider*)o)->value()));
}

void TraceUI::cb_SubPixelSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nSubPixels = int(((Fl_Slider*)o)->value());
}
void TraceUI::cb_Threshold(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nThreshold = int(((Fl_Slider*)o)->value());   // call back funciton for threshold
}

void TraceUI::cb_AmbientLightSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nAmbientLight = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_SampleRaySlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->m_nSampleRays = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_DOFdepth(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->DOFdepth = double(((Fl_Slider*)o)->value());
}

void TraceUI::cb_antialModeChoice(Fl_Widget* o, void* v)
{
	int mode = (int)v;
	((TraceUI*)(o->user_data()))->raytracer->setAalmode(mode);
}

void TraceUI::cb_glossyReflection(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	if (pUI->glossyReflectionOn == true) pUI->glossyReflectionOn = false;
	else pUI->glossyReflectionOn = true;
}

void TraceUI::cb_softShadow(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	if (pUI->softShadowOn == true) pUI->softShadowOn = false;
	else pUI->softShadowOn = true;
}

void TraceUI::cb_DOF(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	if (pUI->DOFOn == true) pUI->DOFOn = false;
	else pUI->DOFOn = true;
}

void TraceUI::cb_MC(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	if (pUI->MCon == true) pUI->MCon = false;
	else pUI->MCon = true;
}

void TraceUI::cb_Bg(Fl_Widget* o, void* v)
{
	TraceUI* pUI = ((TraceUI*)(o->user_data()));
	if (pUI->BgOn == true) {
		pUI->BgOn = false;
	}
	else {
		pUI->BgOn = true;
		// Set Bg Images
		string directory = fl_dir_chooser("Select a directory", NULL);
		traceUI->raytracer->setBg(directory+"/");
	}
}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI=((TraceUI*)(o->user_data()));
	
	if (pUI->raytracer->sceneLoaded()) {
		int width=pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow( width, height );

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);
		
		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done=false;
		clock_t prev, now;
		prev=clock();
		
		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		vec3f** RRAS = new vec3f * [height + 1]; // RRAS means adaptive result record for adaptive supersampling
		for (int j = 0; j < height + 1; ++j) { // Initialization with (-1,-1,-1)
			RRAS[j] = new vec3f[width + 1];
			for (int i = 0; i < width + 1; ++i) {
				RRAS[j][i] = vec3f(-1, -1, -1);
			}
		}

		srand((unsigned)time(0));
		int t = pow(traceUI->getSubPixels(), 2);
		int t2 = pow(traceUI->getNumSampleRays(), 2);
		randnumbers = new int*** [height];
		for (int j = 0; j < height; ++j) {
			randnumbers[j] = new int** [width];
			for (int i = 0; i < width; ++i) {
				randnumbers[j][i] = new int*[t];
				for (int k = 0; k < t; ++k) {
					randnumbers[j][i][k] = new int[t2+1];
					for (int l = 0; l < t2+1; ++l) {
						randnumbers[j][i][k][l] = rand();
					}
				}
			}
		}

		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
				if (done) break;
				
				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now-prev)/CLOCKS_PER_SEC)>0.5) {
					prev=now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel( x, y, RRAS );
		
			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);
			
		}

		for (int j = 0; j < height; ++j) {
			for (int i = 0; i < width; ++i) {
				for (int k = 0; k < t; ++k) {
					delete[] randnumbers[j][i][k];
				}
				delete[] randnumbers[j][i];
			}
			delete[] randnumbers[j];
		}
		delete[] randnumbers;
		randnumbers = nullptr;

		for (int j = 0; j < height + 1; ++j) { // deletion
			delete[] RRAS[j];
		}
		delete[] RRAS;
		RRAS = nullptr;

		done=true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);		
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done=true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

void TraceUI::setRayTracer(RayTracer *tracer)
{
	raytracer = tracer;
	m_traceGlWindow->setRayTracer(tracer);
}

int TraceUI::getSize()
{
	return m_nSize;
}

int TraceUI::getDepth()
{
	return m_nDepth;
}

// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

Fl_Menu_Item TraceUI::AntialiasingModeMenu[4 + 1] = {
	{"None",NULL, (Fl_Callback*)cb_antialModeChoice, (void*)0},
	{"Supersampling(BW2)",NULL, (Fl_Callback*)cb_antialModeChoice, (void*)1},
	{"Jittering(BW2)",NULL, (Fl_Callback*)cb_antialModeChoice, (void*)2},
	{"Adaptive(BW8)",NULL, (Fl_Callback*)cb_antialModeChoice, (void*)3},
	{0}
};

TraceUI::TraceUI() {
	// init.
	m_nDepth = 0;
	m_nSize = 150;
	m_nDACoeff[0] = 1; m_nDACoeff[1] = 0; m_nDACoeff[2] = 0;
	m_nDistScale = 1;
	m_nSubPixels = 1;
	m_nThreshold = 0;
	m_nAmbientLight = 0;
	glossyReflectionOn = false;
	softShadowOn = false;
	DOFOn = false;
	DOFdepth = 1;
	MCon = true;
	BgOn = false;
	m_nSampleRays = 4;
	m_mainWindow = new Fl_Window(100, 40, 400, 500, "Ray <Not Loaded>");
		m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
		// install menu bar
		m_menubar = new Fl_Menu_Bar(0, 0, 320, 25);
		m_menubar->menu(menuitems);

		// Install Sliders
		installSlider(m_depthSlider, 1, "Depth", 0, 10, 1, m_nDepth, cb_depthSlides);
		installSlider(m_sizeSlider, 2, "Size", 64, 512, 1, m_nSize, cb_sizeSlides);
		installSlider(m_conDASlider, 3, "Attenuation-Constant", 0, 1, 0.01, m_nDACoeff[0], cb_conDASlides);
		installSlider(m_linDASlider, 4, "Attenuation-Linear", 0, 1, 0.01, m_nDACoeff[1], cb_linDASlides);
		installSlider(m_quaDASlider, 5, "Attenuation-Quadratic", 0, 1, 0.01, m_nDACoeff[2], cb_quaDASlides);
		installSlider(m_DistScaleSlider, 6, "distance scale factor (log10)", -4, 1, 0.01, m_nDistScale, cb_DistScaleSlides);
		installSlider(m_SubPixelSlider, 7, "Sub-Pixel Samples", 1, 5, 1, m_nSubPixels, cb_SubPixelSlides);
		installSlider(m_Termination, 8, "threshold", 0, 1.0, 0.01, m_nThreshold, cb_Threshold);
		installSlider(m_AmbientLightSlider, 9, "Ambient Light", 0, 1, 0.01, m_nAmbientLight, cb_AmbientLightSlides);
		installSlider(m_DOFdepthSlider, 10, "DOF Focus Depth", 1, 10, 0.1, DOFdepth, cb_DOFdepth);
		installSlider(m_nSampleRaySlider, 11, "Sqrt of No. of Sample Rays", 1, 4, 1, m_nSampleRays, cb_SampleRaySlides);

		//Install Choices and Buttons
		installChoice(m_antialModeChoice, 1, "Antialiasing Mode", AntialiasingModeMenu, cb_antialModeChoice);
		installLightButton(m_glossyReflection, 2, "Glossy Reflection", cb_glossyReflection);
		installLightButton(m_softShadow, 3, "Soft Shadow", cb_softShadow);
		installLightButton(m_DOF, 4, "Depth of Field", cb_DOF);
		installLightButton(m_Bg, 6, "Background Image Box", cb_Bg);

		m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
		m_renderButton->user_data((void*)(this));
		m_renderButton->callback(cb_render);

		m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
		m_stopButton->user_data((void*)(this));
		m_stopButton->callback(cb_stop);

		m_mainWindow->callback(cb_exit2);
		m_mainWindow->when(FL_HIDE);
    m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}

void TraceUI::installSlider(Fl_Slider* &Slider, int indx, const char* name, double Min, double Max, double step, double initval, void (*cb)(Fl_Widget*, void*)) {
	Slider = new Fl_Value_Slider(10, 25*indx+5, 180, 20, name);
	Slider->user_data((void*)(this));	// record self to be used by static callback functions
	Slider->type(FL_HOR_NICE_SLIDER);
	Slider->labelfont(FL_COURIER);
	Slider->labelsize(12);
	Slider->minimum(Min);
	Slider->maximum(Max);
	Slider->step(step);
	Slider->value(initval);
	Slider->align(FL_ALIGN_RIGHT);
	Slider->callback(cb);
}

void TraceUI::installChoice(Fl_Choice* &Choice, int indx, const char* name, Fl_Menu_Item* menu, void (*cb)(Fl_Widget*, void*)) {
	Choice = new Fl_Choice(130+190*((indx-1)%2), 25*num_Sliders+30+25*((indx-1)/2), 60, 20, name);
	Choice->user_data((void*)(this));	 // record self to be used by static callback functions
	Choice->menu(menu);
	Choice->callback(cb);
}

void TraceUI::installLightButton(Fl_Light_Button*& Button, int indx, const char* name, void (*cb)(Fl_Widget*, void*)) {
	Button = new Fl_Light_Button(130 + 190 * ((indx - 1) % 2), 25 * num_Sliders + 30 + 25 * ((indx - 1) / 2), 60, 20, name);
	Button->user_data((void*)(this));   // record self to be used by static callback functions
	Button->callback(cb);
	Button->align(FL_ALIGN_LEFT);
}