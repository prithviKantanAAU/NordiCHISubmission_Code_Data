/* ------------------------------------------------------------
name: "2DSonification_MUS3"
Code generated with Faust 2.30.8 (https://faust.grame.fr)
Compilation options: -lang cpp -es 1 -scal -ftz 0
------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

#include <cmath>
#include <cstring>

/************************** BEGIN dsp.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __dsp__
#define __dsp__

#include <string>
#include <vector>

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

struct UI;
struct Meta;

/**
 * DSP memory manager.
 */

struct dsp_memory_manager {
    
    virtual ~dsp_memory_manager() {}
    
    virtual void* allocate(size_t size) = 0;
    virtual void destroy(void* ptr) = 0;
    
};

/**
* Signal processor definition.
*/

class dsp {

    public:

        dsp() {}
        virtual ~dsp() {}

        /* Return instance number of audio inputs */
        virtual int getNumInputs() = 0;
    
        /* Return instance number of audio outputs */
        virtual int getNumOutputs() = 0;
    
        /**
         * Trigger the ui_interface parameter with instance specific calls
         * to 'openTabBox', 'addButton', 'addVerticalSlider'... in order to build the UI.
         *
         * @param ui_interface - the user interface builder
         */
        virtual void buildUserInterface(UI* ui_interface) = 0;
    
        /* Return the sample rate currently used by the instance */
        virtual int getSampleRate() = 0;
    
        /**
         * Global init, calls the following methods:
         * - static class 'classInit': static tables initialization
         * - 'instanceInit': constants and instance state initialization
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void init(int sample_rate) = 0;

        /**
         * Init instance state
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void instanceInit(int sample_rate) = 0;
    
        /**
         * Init instance constant state
         *
         * @param sample_rate - the sampling rate in Hz
         */
        virtual void instanceConstants(int sample_rate) = 0;
    
        /* Init default control parameters values */
        virtual void instanceResetUserInterface() = 0;
    
        /* Init instance state (like delay lines...) but keep the control parameter values */
        virtual void instanceClear() = 0;
 
        /**
         * Return a clone of the instance.
         *
         * @return a copy of the instance on success, otherwise a null pointer.
         */
        virtual dsp* clone() = 0;
    
        /**
         * Trigger the Meta* parameter with instance specific calls to 'declare' (key, value) metadata.
         *
         * @param m - the Meta* meta user
         */
        virtual void metadata(Meta* m) = 0;
    
        /**
         * DSP instance computation, to be called with successive in/out audio buffers.
         *
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (eiher float, double or quad)
         *
         */
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) = 0;
    
        /**
         * DSP instance computation: alternative method to be used by subclasses.
         *
         * @param date_usec - the timestamp in microsec given by audio driver.
         * @param count - the number of frames to compute
         * @param inputs - the input audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         * @param outputs - the output audio buffers as an array of non-interleaved FAUSTFLOAT samples (either float, double or quad)
         *
         */
        virtual void compute(double /*date_usec*/, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { compute(count, inputs, outputs); }
       
};

/**
 * Generic DSP decorator.
 */

class decorator_dsp : public dsp {

    protected:

        dsp* fDSP;

    public:

        decorator_dsp(dsp* dsp = nullptr):fDSP(dsp) {}
        virtual ~decorator_dsp() { delete fDSP; }

        virtual int getNumInputs() { return fDSP->getNumInputs(); }
        virtual int getNumOutputs() { return fDSP->getNumOutputs(); }
        virtual void buildUserInterface(UI* ui_interface) { fDSP->buildUserInterface(ui_interface); }
        virtual int getSampleRate() { return fDSP->getSampleRate(); }
        virtual void init(int sample_rate) { fDSP->init(sample_rate); }
        virtual void instanceInit(int sample_rate) { fDSP->instanceInit(sample_rate); }
        virtual void instanceConstants(int sample_rate) { fDSP->instanceConstants(sample_rate); }
        virtual void instanceResetUserInterface() { fDSP->instanceResetUserInterface(); }
        virtual void instanceClear() { fDSP->instanceClear(); }
        virtual decorator_dsp* clone() { return new decorator_dsp(fDSP->clone()); }
        virtual void metadata(Meta* m) { fDSP->metadata(m); }
        // Beware: subclasses usually have to overload the two 'compute' methods
        virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(count, inputs, outputs); }
        virtual void compute(double date_usec, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) { fDSP->compute(date_usec, count, inputs, outputs); }
    
};

/**
 * DSP factory class, used with LLVM and Interpreter backends
 * to create DSP instances from a compiled DSP program.
 */

class dsp_factory {
    
    protected:
    
        // So that to force sub-classes to use deleteDSPFactory(dsp_factory* factory);
        virtual ~dsp_factory() {}
    
    public:
    
        virtual std::string getName() = 0;
        virtual std::string getSHAKey() = 0;
        virtual std::string getDSPCode() = 0;
        virtual std::string getCompileOptions() = 0;
        virtual std::vector<std::string> getLibraryList() = 0;
        virtual std::vector<std::string> getIncludePathnames() = 0;
    
        virtual dsp* createDSPInstance() = 0;
    
        virtual void setMemoryManager(dsp_memory_manager* manager) = 0;
        virtual dsp_memory_manager* getMemoryManager() = 0;
    
};

/**
 * On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
 * flags to avoid costly denormals.
 */

#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif
/**************************  END  dsp.h **************************/
/************************** BEGIN MapUI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef FAUST_MAPUI_H
#define FAUST_MAPUI_H

#include <vector>
#include <map>
#include <string>

/************************** BEGIN UI.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2020 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __UI_H__
#define __UI_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust DSP User Interface
 * User Interface as expected by the buildUserInterface() method of a DSP.
 * This abstract class contains only the method that the Faust compiler can
 * generate to describe a DSP user interface.
 ******************************************************************************/

struct Soundfile;

template <typename REAL>
struct UIReal
{
    UIReal() {}
    virtual ~UIReal() {}
    
    // -- widget's layouts
    
    virtual void openTabBox(const char* label) = 0;
    virtual void openHorizontalBox(const char* label) = 0;
    virtual void openVerticalBox(const char* label) = 0;
    virtual void closeBox() = 0;
    
    // -- active widgets
    
    virtual void addButton(const char* label, REAL* zone) = 0;
    virtual void addCheckButton(const char* label, REAL* zone) = 0;
    virtual void addVerticalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addHorizontalSlider(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    virtual void addNumEntry(const char* label, REAL* zone, REAL init, REAL min, REAL max, REAL step) = 0;
    
    // -- passive widgets
    
    virtual void addHorizontalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    virtual void addVerticalBargraph(const char* label, REAL* zone, REAL min, REAL max) = 0;
    
    // -- soundfiles
    
    virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) = 0;
    
    // -- metadata declarations
    
    virtual void declare(REAL* zone, const char* key, const char* val) {}
};

struct UI : public UIReal<FAUSTFLOAT>
{
    UI() {}
    virtual ~UI() {}
};

#endif
/**************************  END  UI.h **************************/
/************************** BEGIN PathBuilder.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef FAUST_PATHBUILDER_H
#define FAUST_PATHBUILDER_H

#include <vector>
#include <string>
#include <algorithm>

/*******************************************************************************
 * PathBuilder : Faust User Interface
 * Helper class to build complete hierarchical path for UI items.
 ******************************************************************************/

class PathBuilder
{

    protected:
    
        std::vector<std::string> fControlsLevel;
       
    public:
    
        PathBuilder() {}
        virtual ~PathBuilder() {}
    
        std::string buildPath(const std::string& label) 
        {
            std::string res = "/";
            for (size_t i = 0; i < fControlsLevel.size(); i++) {
                res += fControlsLevel[i];
                res += "/";
            }
            res += label;
            std::replace(res.begin(), res.end(), ' ', '_');
            return res;
        }
    
        std::string buildLabel(std::string label)
        {
            std::replace(label.begin(), label.end(), ' ', '_');
            return label;
        }
    
        void pushLabel(const std::string& label) { fControlsLevel.push_back(label); }
        void popLabel() { fControlsLevel.pop_back(); }
    
};

#endif  // FAUST_PATHBUILDER_H
/**************************  END  PathBuilder.h **************************/

/*******************************************************************************
 * MapUI : Faust User Interface
 * This class creates a map of complete hierarchical path and zones for each UI items.
 ******************************************************************************/

class MapUI : public UI, public PathBuilder
{
    
    protected:
    
        // Complete path map
        std::map<std::string, FAUSTFLOAT*> fPathZoneMap;
    
        // Label zone map
        std::map<std::string, FAUSTFLOAT*> fLabelZoneMap;
    
        std::string fNullStr = "";
    
    public:
        
        MapUI() {}
        virtual ~MapUI() {}
        
        // -- widget's layouts
        void openTabBox(const char* label)
        {
            pushLabel(label);
        }
        void openHorizontalBox(const char* label)
        {
            pushLabel(label);
        }
        void openVerticalBox(const char* label)
        {
            pushLabel(label);
        }
        void closeBox()
        {
            popLabel();
        }
        
        // -- active widgets
        void addButton(const char* label, FAUSTFLOAT* zone)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addCheckButton(const char* label, FAUSTFLOAT* zone)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT fmin, FAUSTFLOAT fmax, FAUSTFLOAT step)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        
        // -- passive widgets
        void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
        void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT fmin, FAUSTFLOAT fmax)
        {
            fPathZoneMap[buildPath(label)] = zone;
            fLabelZoneMap[label] = zone;
        }
    
        // -- soundfiles
        virtual void addSoundfile(const char* label, const char* filename, Soundfile** sf_zone) {}
        
        // -- metadata declarations
        virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val)
        {}
        
        // set/get
        void setParamValue(const std::string& path, FAUSTFLOAT value)
        {
            if (fPathZoneMap.find(path) != fPathZoneMap.end()) {
                *fPathZoneMap[path] = value;
            } else if (fLabelZoneMap.find(path) != fLabelZoneMap.end()) {
                *fLabelZoneMap[path] = value;
            }
        }
        
        FAUSTFLOAT getParamValue(const std::string& path)
        {
            if (fPathZoneMap.find(path) != fPathZoneMap.end()) {
                return *fPathZoneMap[path];
            } else if (fLabelZoneMap.find(path) != fLabelZoneMap.end()) {
                return *fLabelZoneMap[path];
            } else {
                return FAUSTFLOAT(0);
            }
        }
    
        // map access 
        std::map<std::string, FAUSTFLOAT*>& getMap() { return fPathZoneMap; }
        
        int getParamsCount() { return int(fPathZoneMap.size()); }
        
        const std::string& getParamAddress(int index)
        {
            if (index < 0 || index > int(fPathZoneMap.size())) {
                return fNullStr;
            } else {
                auto it = fPathZoneMap.begin();
                while (index-- > 0 && it++ != fPathZoneMap.end()) {}
                return it->first;
            }
        }
    
        const std::string& getParamAddress(FAUSTFLOAT* zone)
        {
            for (auto& it : fPathZoneMap) {
                if (it.second == zone) return it.first;
            }
            return fNullStr;
        }
    
        FAUSTFLOAT* getParamZone(const std::string& str)
        {
            if (fPathZoneMap.find(str) != fPathZoneMap.end()) {
                return fPathZoneMap[str];
            }
            if (fLabelZoneMap.find(str) != fLabelZoneMap.end()) {
                return fLabelZoneMap[str];
            }
            return nullptr;
        }
    
        FAUSTFLOAT* getParamZone(int index)
        {
            if (index < 0 || index > int(fPathZoneMap.size())) {
                return nullptr;
            } else {
                auto it = fPathZoneMap.begin();
                while (index-- > 0 && it++ != fPathZoneMap.end()) {}
                return it->second;
            }
        }
    
        static bool endsWith(const std::string& str, const std::string& end)
        {
            size_t l1 = str.length();
            size_t l2 = end.length();
            return (l1 >= l2) && (0 == str.compare(l1 - l2, l2, end));
        }
};


#endif // FAUST_MAPUI_H
/**************************  END  MapUI.h **************************/
/************************** BEGIN meta.h **************************/
/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2017 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 ************************************************************************/

#ifndef __meta__
#define __meta__

/**
 The base class of Meta handler to be used in dsp::metadata(Meta* m) method to retrieve (key, value) metadata.
 */
struct Meta
{
    virtual ~Meta() {};
    virtual void declare(const char* key, const char* value) = 0;
};

#endif
/**************************  END  meta.h **************************/

// BEGIN-FAUSTDSP

    // END-FAUSTDSP
#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

class mydspSIG0 {
	
  private:
	
	int iRec2[2];
	
  public:
	
	int getNumInputsmydspSIG0() {
		return 0;
	}
	int getNumOutputsmydspSIG0() {
		return 1;
	}
	int getInputRatemydspSIG0(int channel) {
		int rate;
		switch ((channel)) {
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	int getOutputRatemydspSIG0(int channel) {
		int rate;
		switch ((channel)) {
			case 0: {
				rate = 0;
				break;
			}
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	
	void instanceInitmydspSIG0(int sample_rate) {
		for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
			iRec2[l2] = 0;
		}
	}
	
	void fillmydspSIG0(int count, float* table) {
		for (int i = 0; (i < count); i = (i + 1)) {
			iRec2[0] = (iRec2[1] + 1);
			table[i] = std::sin((9.58738019e-05f * float((iRec2[0] + -1))));
			iRec2[1] = iRec2[0];
		}
	}

};

static mydspSIG0* newmydspSIG0() { return (mydspSIG0*)new mydspSIG0(); }
static void deletemydspSIG0(mydspSIG0* dsp) { delete dsp; }

const static float fmydspSIG1Wave0[350] = {0.525285006f,0.814173996f,0.483260989f,0.296745002f,0.975055993f,0.472243994f,0.409500986f,0.425363988f,0.687559009f,0.28838101f,0.309285015f,0.123053998f,0.286332995f,0.576705992f,0.908321977f,0.626973987f,0.0801851973f,0.309834987f,0.451429993f,0.132844999f,0.470634997f,0.417008013f,0.265112013f,0.075280197f,0.463470012f,0.471810013f,0.275323987f,0.547026992f,0.512519002f,0.394077986f,0.595404029f,0.941305995f,0.392500997f,0.381435007f,0.391232014f,0.118923999f,0.339495003f,0.101420999f,0.241754994f,0.0873254985f,0.37894401f,0.637705028f,0.171946004f,0.149858996f,0.233290002f,0.541809976f,1.0f,0.115553997f,0.244172007f,0.574329019f,0.606171012f,0.93839699f,0.392554998f,0.277359009f,0.86857003f,0.432489008f,0.408856004f,0.407932013f,0.299814999f,0.256659001f,0.549571991f,0.406347007f,0.312330991f,0.62757802f,0.670167029f,0.524648011f,0.406926006f,0.637524009f,0.555836976f,1.0f,0.818979025f,0.705347002f,0.678140998f,0.427381992f,0.674404025f,0.636105001f,0.643634975f,0.699136019f,0.836201012f,0.613084972f,0.31901899f,0.725259006f,0.545518994f,0.479860991f,0.498360008f,0.488653988f,0.861671984f,0.314287007f,0.671051979f,0.531904995f,0.421781003f,0.81506598f,0.772032022f,0.488721997f,0.0896674022f,0.291285992f,0.65872997f,0.635631979f,0.679356992f,0.459497005f,0.360240012f,0.58228898f,0.650605023f,0.490949988f,0.381909996f,0.157260999f,0.479624003f,0.477490991f,0.174435005f,0.0130939996f,0.879113019f,0.608069003f,0.268877f,0.604479015f,0.245130002f,0.170506999f,0.292887986f,0.545849025f,0.476646006f,0.922316015f,0.669192016f,0.578094006f,0.578796983f,0.311396003f,0.601209998f,0.54995501f,1.0f,0.66573f,0.980114996f,0.537847996f,0.0740531012f,0.252472013f,0.25575f,0.223974004f,0.0865103006f,0.138209f,0.198623002f,0.0453034006f,0.432453007f,0.292407006f,0.394410014f,0.857658982f,0.271667987f,0.201545f,0.583993971f,0.0602377988f,0.190617993f,0.849505007f,0.975542009f,0.173140004f,0.206471995f,0.344792992f,0.761011004f,0.558125019f,0.117245004f,0.0338485017f,0.337597013f,0.336645991f,0.174253002f,0.230169997f,0.934872985f,0.593647003f,0.393225014f,0.683704019f,0.056609299f,0.0405011997f,0.148972005f,0.338721991f,0.283419013f,0.394006997f,0.237474993f,0.26996401f,0.428312987f,0.177498996f,0.462585002f,0.443962991f,0.981792986f,0.408239007f,0.676527023f,0.402864993f,0.0163302999f,0.0515113994f,0.341390014f,0.311134994f,0.613276005f,0.805884004f,0.953289986f,0.406091005f,0.578705013f,0.386785001f,0.434103012f,0.775259972f,1.0f,0.635909021f,0.78205198f,0.0137182996f,0.0387725011f,0.618964016f,0.857070982f,0.131522f,0.184988007f,0.299495012f,0.789211988f,0.603114009f,0.0704988986f,0.0129338996f,0.252481014f,0.254121006f,0.189206004f,0.357713014f,0.950308025f,0.552573025f,0.466453999f,0.777360022f,0.0307886004f,0.0251943003f,0.378886014f,0.740186989f,0.247637004f,0.235201001f,0.493045002f,0.517849982f,0.883953989f,0.429473013f,0.409433007f,0.415266007f,0.940198004f,0.282334f,0.437889993f,0.375384986f,0.0157366004f,0.0171763003f,0.485554993f,0.461014986f,0.858958006f,0.907990992f,0.935190976f,0.375510007f,1.0f,0.585493028f,0.269980997f,0.423052996f,0.666067004f,0.435090005f,0.79025197f,0.00889586005f,0.0208844002f,0.449734986f,0.790808022f,0.159856007f,0.0895989984f,0.161546007f,0.528168023f,0.380641997f,0.0206275992f,0.0072642602f,0.0315352008f,0.0315840989f,0.197649002f,0.475057006f,0.517232001f,0.360922009f,0.421204001f,0.631340027f,0.00952139031f,0.0161049999f,0.499615014f,0.922958016f,0.214983001f,0.0655141026f,0.503970027f,0.514847994f,1.0f,0.483619004f,0.254027009f,0.228371993f,0.436105013f,0.233125001f,0.152242005f,0.279513001f,0.00906739011f,0.0132331997f,0.45125699f,0.388565987f,0.737226009f,0.479378015f,0.233036995f,0.103767f,0.845609009f,0.644127011f,0.261359006f,0.371457011f,0.527229011f,0.381372988f,0.334491998f,0.00833749026f,0.00861981977f,0.255919009f,0.254197001f,0.0872332975f,0.0461511984f,0.113017999f,0.345986009f,0.236343995f,0.0107800001f,0.00816505961f,0.405180991f,0.387180001f,0.343681008f,0.816492975f,0.25908199f,0.211906001f,0.432455003f,0.696886003f,0.00576223992f,0.0131310001f,0.455969006f,0.81160903f,0.426544011f,0.128489003f,0.215937003f,0.233934f,0.723070025f,0.351622999f,0.394230992f,0.323765993f,0.168803006f,0.276932001f,0.264683992f,0.227703005f,0.0068093501f,0.0170703009f,0.603017986f,0.476460993f,0.585924983f,0.716960013f,1.0f,0.576526999f,0.475524008f,0.447322011f,0.356902003f,0.597572982f,0.697246015f,0.505333006f,0.285421014f,0.0147193f,0.0141618f,0.136188f,0.0336536989f,0.216436997f};
class mydspSIG1 {
	
  private:
	
	int fmydspSIG1Wave0_idx;
	
  public:
	
	int getNumInputsmydspSIG1() {
		return 0;
	}
	int getNumOutputsmydspSIG1() {
		return 1;
	}
	int getInputRatemydspSIG1(int channel) {
		int rate;
		switch ((channel)) {
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	int getOutputRatemydspSIG1(int channel) {
		int rate;
		switch ((channel)) {
			case 0: {
				rate = 0;
				break;
			}
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	
	void instanceInitmydspSIG1(int sample_rate) {
		fmydspSIG1Wave0_idx = 0;
	}
	
	void fillmydspSIG1(int count, float* table) {
		for (int i = 0; (i < count); i = (i + 1)) {
			table[i] = fmydspSIG1Wave0[fmydspSIG1Wave0_idx];
			fmydspSIG1Wave0_idx = ((1 + fmydspSIG1Wave0_idx) % 350);
		}
	}

};

static mydspSIG1* newmydspSIG1() { return (mydspSIG1*)new mydspSIG1(); }
static void deletemydspSIG1(mydspSIG1* dsp) { delete dsp; }

static float ftbl0mydspSIG0[65536];
static float mydsp_faustpower2_f(float value) {
	return (value * value);
}
static float ftbl1mydspSIG1[350];

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

class mydsp : public dsp {
	
 private:
	
	FAUSTFLOAT fVslider0;
	int iVec0[2];
	int fSampleRate;
	float fConst1;
	float fConst2;
	FAUSTFLOAT fHslider0;
	float fRec1[2];
	float fConst3;
	float fRec3[2];
	float fRec0[2];
	float fConst4;
	float fConst5;
	float fConst6;
	float fConst7;
	FAUSTFLOAT fHslider1;
	float fRec6[2];
	FAUSTFLOAT fHslider2;
	float fRec7[2];
	float fConst8;
	float fRec8[2];
	float fRec5[2];
	float fVec1[2];
	int IOTA;
	float fVec2[4096];
	float fConst9;
	float fRec4[2];
	float fRec9[2];
	FAUSTFLOAT fHslider3;
	float fRec11[2];
	float fRec10[2];
	float fVec3[2];
	float fVec4[4096];
	FAUSTFLOAT fHslider4;
	float fRec13[2];
	float fRec12[2];
	float fVec5[2];
	float fVec6[4096];
	FAUSTFLOAT fHslider5;
	float fRec15[2];
	float fRec14[2];
	float fVec7[2];
	float fVec8[4096];
	FAUSTFLOAT fHslider6;
	float fRec16[2];
	float fConst10;
	float fConst13;
	float fConst16;
	float fConst18;
	int iRec20[2];
	float fConst19;
	float fConst20;
	float fRec19[3];
	float fConst21;
	float fConst22;
	float fConst23;
	float fRec18[3];
	float fConst24;
	int iRec22[2];
	int iVec9[2];
	int iRec21[2];
	float fConst26;
	float fConst27;
	float fRec17[3];
	float fConst28;
	float fConst30;
	float fConst31;
	float fRec23[3];
	float fConst32;
	float fConst34;
	float fConst35;
	float fRec24[3];
	float fConst36;
	float fConst38;
	float fConst39;
	float fRec25[3];
	float fConst40;
	float fConst42;
	float fConst43;
	float fRec26[3];
	float fConst44;
	float fConst46;
	float fConst47;
	float fRec27[3];
	float fConst48;
	float fConst50;
	float fConst51;
	float fRec28[3];
	float fConst52;
	float fConst54;
	float fConst55;
	float fRec29[3];
	float fConst56;
	float fConst58;
	float fConst59;
	float fRec30[3];
	float fConst60;
	float fConst62;
	float fConst63;
	float fRec31[3];
	float fConst64;
	float fConst66;
	float fConst67;
	float fRec32[3];
	float fConst68;
	float fConst70;
	float fConst71;
	float fRec33[3];
	float fConst72;
	float fConst74;
	float fConst75;
	float fRec34[3];
	float fConst76;
	float fConst78;
	float fConst79;
	float fRec35[3];
	float fConst80;
	float fConst82;
	float fConst83;
	float fRec36[3];
	float fConst84;
	float fConst86;
	float fConst87;
	float fRec37[3];
	float fConst88;
	float fConst90;
	float fConst91;
	float fRec38[3];
	float fConst92;
	float fConst94;
	float fConst95;
	float fRec39[3];
	float fConst96;
	float fConst98;
	float fConst99;
	float fRec40[3];
	float fConst100;
	float fConst102;
	float fConst103;
	float fRec41[3];
	float fConst104;
	float fConst106;
	float fConst107;
	float fRec42[3];
	float fConst108;
	float fConst110;
	float fConst111;
	float fRec43[3];
	float fConst112;
	float fConst114;
	float fConst115;
	float fRec44[3];
	float fConst116;
	float fConst118;
	float fConst119;
	float fRec45[3];
	float fConst120;
	float fConst122;
	float fConst123;
	float fRec46[3];
	float fConst124;
	float fConst126;
	float fConst127;
	float fRec47[3];
	float fConst128;
	float fConst130;
	float fConst131;
	float fRec48[3];
	float fConst132;
	float fConst134;
	float fConst135;
	float fRec49[3];
	float fConst136;
	float fConst138;
	float fConst139;
	float fRec50[3];
	float fConst140;
	float fConst142;
	float fConst143;
	float fRec51[3];
	float fConst144;
	float fConst146;
	float fConst147;
	float fRec52[3];
	float fConst148;
	float fConst150;
	float fConst151;
	float fRec53[3];
	float fConst152;
	float fConst154;
	float fConst155;
	float fRec54[3];
	float fConst156;
	float fConst158;
	float fConst159;
	float fRec55[3];
	float fConst160;
	float fConst162;
	float fConst163;
	float fRec56[3];
	float fConst164;
	float fConst166;
	float fConst167;
	float fRec57[3];
	float fConst168;
	float fConst170;
	float fConst171;
	float fRec58[3];
	float fConst172;
	float fConst174;
	float fConst175;
	float fRec59[3];
	float fConst176;
	float fConst178;
	float fConst179;
	float fRec60[3];
	float fConst180;
	float fConst182;
	float fConst183;
	float fRec61[3];
	float fConst184;
	float fConst186;
	float fConst187;
	float fRec62[3];
	float fConst188;
	float fConst190;
	float fConst191;
	float fRec63[3];
	float fConst192;
	float fConst194;
	float fConst195;
	float fRec64[3];
	float fConst196;
	float fConst198;
	float fConst199;
	float fRec65[3];
	float fConst200;
	float fConst202;
	float fConst203;
	float fRec66[3];
	float fConst204;
	float fConst206;
	float fConst207;
	float fRec67[3];
	float fConst208;
	float fConst210;
	float fConst211;
	float fRec68[3];
	float fConst212;
	float fConst214;
	float fConst215;
	float fRec69[3];
	float fConst216;
	float fConst218;
	float fConst219;
	float fRec70[3];
	float fConst220;
	float fConst222;
	float fConst223;
	float fRec71[3];
	float fConst224;
	float fConst225;
	float fConst226;
	float fConst236;
	float fConst237;
	float fConst238;
	float fConst240;
	float fConst241;
	float fRec83[2];
	float fRec82[2];
	float fVec10[32768];
	int iConst243;
	float fVec11[2048];
	int iConst244;
	float fVec12[2048];
	int iConst245;
	float fRec80[2];
	float fConst254;
	float fConst255;
	float fConst256;
	float fRec87[2];
	float fRec86[2];
	float fVec13[32768];
	int iConst258;
	float fVec14[4096];
	int iConst259;
	float fRec84[2];
	float fConst268;
	float fConst269;
	float fConst270;
	float fRec91[2];
	float fRec90[2];
	float fVec15[16384];
	int iConst272;
	float fVec16[4096];
	int iConst273;
	float fRec88[2];
	float fConst282;
	float fConst283;
	float fConst284;
	float fRec95[2];
	float fRec94[2];
	float fVec17[32768];
	int iConst286;
	float fVec18[4096];
	int iConst287;
	float fRec92[2];
	float fConst296;
	float fConst297;
	float fConst298;
	float fRec99[2];
	float fRec98[2];
	float fVec19[16384];
	int iConst300;
	float fVec20[2048];
	int iConst301;
	float fRec96[2];
	float fConst310;
	float fConst311;
	float fConst312;
	float fRec103[2];
	float fRec102[2];
	float fVec21[16384];
	int iConst314;
	float fVec22[4096];
	int iConst315;
	float fRec100[2];
	float fConst324;
	float fConst325;
	float fConst326;
	float fRec107[2];
	float fRec106[2];
	float fVec23[16384];
	int iConst328;
	float fVec24[4096];
	int iConst329;
	float fRec104[2];
	float fConst338;
	float fConst339;
	float fConst340;
	float fRec111[2];
	float fRec110[2];
	float fVec25[16384];
	int iConst342;
	float fVec26[2048];
	int iConst343;
	float fRec108[2];
	float fRec72[3];
	float fRec73[3];
	float fRec74[3];
	float fRec75[3];
	float fRec76[3];
	float fRec77[3];
	float fRec78[3];
	float fRec79[3];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.1");
		m->declare("compile_options", "-lang cpp -es 1 -scal -ftz 0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "0.1");
		m->declare("envelopes.lib/ar:author", "Yann Orlarey, Stéphane Letz");
		m->declare("envelopes.lib/author", "GRAME");
		m->declare("envelopes.lib/copyright", "GRAME");
		m->declare("envelopes.lib/license", "LGPL with exception");
		m->declare("envelopes.lib/name", "Faust Envelope Library");
		m->declare("envelopes.lib/version", "0.1");
		m->declare("filename", "2DSonification_MUS3.dsp");
		m->declare("filters.lib/allpass_comb:author", "Julius O. Smith III");
		m->declare("filters.lib/allpass_comb:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/allpass_comb:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/fir:author", "Julius O. Smith III");
		m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/highpass:author", "Julius O. Smith III");
		m->declare("filters.lib/highpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:author", "Julius O. Smith III");
		m->declare("filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/pole:author", "Julius O. Smith III");
		m->declare("filters.lib/pole:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/pole:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf1:author", "Julius O. Smith III");
		m->declare("filters.lib/tf1:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf1:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf1s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf1s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf1s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/version", "0.3");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.3");
		m->declare("name", "2DSonification_MUS3");
		m->declare("noises.lib/name", "Faust Noise Generator Library");
		m->declare("noises.lib/version", "0.0");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "0.1");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "0.1");
		m->declare("reverbs.lib/name", "Faust Reverb Library");
		m->declare("reverbs.lib/version", "0.0");
		m->declare("routes.lib/name", "Faust Signal Routing Library");
		m->declare("routes.lib/version", "0.2");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/version", "0.0");
	}

	virtual int getNumInputs() {
		return 0;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch ((channel)) {
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	virtual int getOutputRate(int channel) {
		int rate;
		switch ((channel)) {
			case 0: {
				rate = 1;
				break;
			}
			case 1: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
		}
		return rate;
	}
	
	static void classInit(int sample_rate) {
		mydspSIG0* sig0 = newmydspSIG0();
		sig0->instanceInitmydspSIG0(sample_rate);
		sig0->fillmydspSIG0(65536, ftbl0mydspSIG0);
		mydspSIG1* sig1 = newmydspSIG1();
		sig1->instanceInitmydspSIG1(sample_rate);
		sig1->fillmydspSIG1(350, ftbl1mydspSIG1);
		deletemydspSIG0(sig0);
		deletemydspSIG1(sig1);
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		float fConst0 = std::min<float>(192000.0f, std::max<float>(1.0f, float(fSampleRate)));
		fConst1 = std::exp((0.0f - (1000.0f / fConst0)));
		fConst2 = (1.0f - fConst1);
		fConst3 = (1.0f / fConst0);
		fConst4 = (8.39999962f / fConst0);
		fConst5 = (0.25f * fConst0);
		fConst6 = std::exp((0.0f - (100.0f / fConst0)));
		fConst7 = (1.0f - fConst6);
		fConst8 = (40.0f / fConst0);
		fConst9 = (0.5f * fConst0);
		fConst10 = ftbl1mydspSIG1[50];
		float fConst11 = std::tan((31415.9258f / fConst0));
		float fConst12 = (1.0f / fConst11);
		fConst13 = (1.0f / (((fConst12 + 1.41421354f) / fConst11) + 1.0f));
		float fConst14 = std::tan((31.415926f / fConst0));
		float fConst15 = (1.0f / fConst14);
		fConst16 = (1.0f / (((fConst15 + 1.41421354f) / fConst14) + 1.0f));
		float fConst17 = mydsp_faustpower2_f(fConst14);
		fConst18 = (1.0f / fConst17);
		fConst19 = (((fConst15 + -1.41421354f) / fConst14) + 1.0f);
		fConst20 = (2.0f * (1.0f - fConst18));
		fConst21 = (0.0f - (2.0f / fConst17));
		fConst22 = (((fConst12 + -1.41421354f) / fConst11) + 1.0f);
		fConst23 = (2.0f * (1.0f - (1.0f / mydsp_faustpower2_f(fConst11))));
		fConst24 = (1.0f / std::max<float>(1.0f, (0.00159999996f * fConst0)));
		float fConst25 = std::pow(0.00100000005f, (0.0448570587f / fConst0));
		fConst26 = (std::cos((2839.48462f / fConst0)) * (0.0f - (2.0f * fConst25)));
		fConst27 = mydsp_faustpower2_f(fConst25);
		fConst28 = ftbl1mydspSIG1[51];
		float fConst29 = std::pow(0.00100000005f, (0.0449536517f / fConst0));
		fConst30 = (std::cos((2858.84937f / fConst0)) * (0.0f - (2.0f * fConst29)));
		fConst31 = mydsp_faustpower2_f(fConst29);
		fConst32 = ftbl1mydspSIG1[52];
		float fConst33 = std::pow(0.00100000005f, (0.0609094799f / fConst0));
		fConst34 = (std::cos((5432.7124f / fConst0)) * (0.0f - (2.0f * fConst33)));
		fConst35 = mydsp_faustpower2_f(fConst33);
		fConst36 = ftbl1mydspSIG1[53];
		float fConst37 = std::pow(0.00100000005f, (0.0612352863f / fConst0));
		fConst38 = (std::cos((5475.18018f / fConst0)) * (0.0f - (2.0f * fConst37)));
		fConst39 = mydsp_faustpower2_f(fConst37);
		fConst40 = ftbl1mydspSIG1[54];
		float fConst41 = std::pow(0.00100000005f, (0.0721585602f / fConst0));
		fConst42 = (std::cos((6738.52783f / fConst0)) * (0.0f - (2.0f * fConst41)));
		fConst43 = mydsp_faustpower2_f(fConst41);
		fConst44 = ftbl1mydspSIG1[55];
		float fConst45 = std::pow(0.00100000005f, (0.0722505748f / fConst0));
		fConst46 = (std::cos((6748.01514f / fConst0)) * (0.0f - (2.0f * fConst45)));
		fConst47 = mydsp_faustpower2_f(fConst45);
		fConst48 = ftbl1mydspSIG1[56];
		float fConst49 = std::pow(0.00100000005f, (0.0874899849f / fConst0));
		fConst50 = (std::cos((8119.32031f / fConst0)) * (0.0f - (2.0f * fConst49)));
		fConst51 = mydsp_faustpower2_f(fConst49);
		fConst52 = ftbl1mydspSIG1[57];
		float fConst53 = std::pow(0.00100000005f, (0.0875099227f / fConst0));
		fConst54 = (std::cos((8120.89111f / fConst0)) * (0.0f - (2.0f * fConst53)));
		fConst55 = mydsp_faustpower2_f(fConst53);
		fConst56 = ftbl1mydspSIG1[58];
		float fConst57 = std::pow(0.00100000005f, (0.107014902f / fConst0));
		fConst58 = (std::cos((9453.68066f / fConst0)) * (0.0f - (2.0f * fConst57)));
		fConst59 = mydsp_faustpower2_f(fConst57);
		fConst60 = ftbl1mydspSIG1[59];
		float fConst61 = std::pow(0.00100000005f, (0.110012196f / fConst0));
		fConst62 = (std::cos((9628.41602f / fConst0)) * (0.0f - (2.0f * fConst61)));
		fConst63 = mydsp_faustpower2_f(fConst61);
		fConst64 = ftbl1mydspSIG1[60];
		float fConst65 = std::pow(0.00100000005f, (0.123582378f / fConst0));
		fConst66 = (std::cos((10343.3799f / fConst0)) * (0.0f - (2.0f * fConst65)));
		fConst67 = mydsp_faustpower2_f(fConst65);
		fConst68 = ftbl1mydspSIG1[61];
		float fConst69 = std::pow(0.00100000005f, (0.123685889f / fConst0));
		fConst70 = (std::cos((10348.4062f / fConst0)) * (0.0f - (2.0f * fConst69)));
		fConst71 = mydsp_faustpower2_f(fConst69);
		fConst72 = ftbl1mydspSIG1[62];
		float fConst73 = std::pow(0.00100000005f, (0.127769172f / fConst0));
		fConst74 = (std::cos((10542.1172f / fConst0)) * (0.0f - (2.0f * fConst73)));
		fConst75 = mydsp_faustpower2_f(fConst73);
		fConst76 = ftbl1mydspSIG1[63];
		float fConst77 = std::pow(0.00100000005f, (0.127809823f / fConst0));
		fConst78 = (std::cos((10544.002f / fConst0)) * (0.0f - (2.0f * fConst77)));
		fConst79 = mydsp_faustpower2_f(fConst77);
		fConst80 = ftbl1mydspSIG1[64];
		float fConst81 = std::pow(0.00100000005f, (0.157417208f / fConst0));
		fConst82 = (std::cos((11728.3818f / fConst0)) * (0.0f - (2.0f * fConst81)));
		fConst83 = mydsp_faustpower2_f(fConst81);
		fConst84 = ftbl1mydspSIG1[65];
		float fConst85 = std::pow(0.00100000005f, (0.160256028f / fConst0));
		fConst86 = (std::cos((11825.457f / fConst0)) * (0.0f - (2.0f * fConst85)));
		fConst87 = mydsp_faustpower2_f(fConst85);
		fConst88 = ftbl1mydspSIG1[66];
		float fConst89 = std::pow(0.00100000005f, (0.18117331f / fConst0));
		fConst90 = (std::cos((12473.3799f / fConst0)) * (0.0f - (2.0f * fConst89)));
		fConst91 = mydsp_faustpower2_f(fConst89);
		fConst92 = ftbl1mydspSIG1[67];
		float fConst93 = std::pow(0.00100000005f, (0.182209119f / fConst0));
		fConst94 = (std::cos((12502.7217f / fConst0)) * (0.0f - (2.0f * fConst93)));
		fConst95 = mydsp_faustpower2_f(fConst93);
		fConst96 = ftbl1mydspSIG1[68];
		float fConst97 = std::pow(0.00100000005f, (0.2133362f / fConst0));
		fConst98 = (std::cos((13288.4971f / fConst0)) * (0.0f - (2.0f * fConst97)));
		fConst99 = mydsp_faustpower2_f(fConst97);
		fConst100 = ftbl1mydspSIG1[69];
		float fConst101 = std::pow(0.00100000005f, (0.298688531f / fConst0));
		fConst102 = (std::cos((14808.2744f / fConst0)) * (0.0f - (2.0f * fConst101)));
		fConst103 = mydsp_faustpower2_f(fConst101);
		fConst104 = ftbl1mydspSIG1[70];
		float fConst105 = std::pow(0.00100000005f, (0.341466933f / fConst0));
		fConst106 = (std::cos((15358.1152f / fConst0)) * (0.0f - (2.0f * fConst105)));
		fConst107 = mydsp_faustpower2_f(fConst105);
		fConst108 = ftbl1mydspSIG1[71];
		float fConst109 = std::pow(0.00100000005f, (0.342488438f / fConst0));
		fConst110 = (std::cos((15370.0537f / fConst0)) * (0.0f - (2.0f * fConst109)));
		fConst111 = mydsp_faustpower2_f(fConst109);
		fConst112 = ftbl1mydspSIG1[72];
		float fConst113 = std::pow(0.00100000005f, (0.391345769f / fConst0));
		fConst114 = (std::cos((15888.7305f / fConst0)) * (0.0f - (2.0f * fConst113)));
		fConst115 = mydsp_faustpower2_f(fConst113);
		fConst116 = ftbl1mydspSIG1[73];
		float fConst117 = std::pow(0.00100000005f, (0.392745376f / fConst0));
		fConst118 = (std::cos((15902.2393f / fConst0)) * (0.0f - (2.0f * fConst117)));
		fConst119 = mydsp_faustpower2_f(fConst117);
		fConst120 = ftbl1mydspSIG1[74];
		float fConst121 = std::pow(0.00100000005f, (0.499480247f / fConst0));
		fConst122 = (std::cos((16768.9414f / fConst0)) * (0.0f - (2.0f * fConst121)));
		fConst123 = mydsp_faustpower2_f(fConst121);
		fConst124 = ftbl1mydspSIG1[75];
		float fConst125 = std::pow(0.00100000005f, (0.500184536f / fConst0));
		fConst126 = (std::cos((16773.7793f / fConst0)) * (0.0f - (2.0f * fConst125)));
		fConst127 = mydsp_faustpower2_f(fConst125);
		fConst128 = ftbl1mydspSIG1[76];
		float fConst129 = std::pow(0.00100000005f, (0.56877172f / fConst0));
		fConst130 = (std::cos((17203.7383f / fConst0)) * (0.0f - (2.0f * fConst129)));
		fConst131 = mydsp_faustpower2_f(fConst129);
		fConst132 = ftbl1mydspSIG1[77];
		float fConst133 = std::pow(0.00100000005f, (0.580974996f / fConst0));
		fConst134 = (std::cos((17272.6641f / fConst0)) * (0.0f - (2.0f * fConst133)));
		fConst135 = mydsp_faustpower2_f(fConst133);
		fConst136 = ftbl1mydspSIG1[78];
		float fConst137 = std::pow(0.00100000005f, (0.58267206f / fConst0));
		fConst138 = (std::cos((17282.0898f / fConst0)) * (0.0f - (2.0f * fConst137)));
		fConst139 = mydsp_faustpower2_f(fConst137);
		fConst140 = ftbl1mydspSIG1[79];
		float fConst141 = std::pow(0.00100000005f, (0.58560729f / fConst0));
		fConst142 = (std::cos((17298.3008f / fConst0)) * (0.0f - (2.0f * fConst141)));
		fConst143 = mydsp_faustpower2_f(fConst141);
		fConst144 = ftbl1mydspSIG1[80];
		float fConst145 = std::pow(0.00100000005f, (0.679538369f / fConst0));
		fConst146 = (std::cos((17763.7578f / fConst0)) * (0.0f - (2.0f * fConst145)));
		fConst147 = mydsp_faustpower2_f(fConst145);
		fConst148 = ftbl1mydspSIG1[81];
		float fConst149 = std::pow(0.00100000005f, (0.701286793f / fConst0));
		fConst150 = (std::cos((17858.8242f / fConst0)) * (0.0f - (2.0f * fConst149)));
		fConst151 = mydsp_faustpower2_f(fConst149);
		fConst152 = ftbl1mydspSIG1[82];
		float fConst153 = std::pow(0.00100000005f, (0.981793463f / fConst0));
		fConst154 = (std::cos((18802.8086f / fConst0)) * (0.0f - (2.0f * fConst153)));
		fConst155 = mydsp_faustpower2_f(fConst153);
		fConst156 = ftbl1mydspSIG1[83];
		float fConst157 = std::pow(0.00100000005f, (0.991939485f / fConst0));
		fConst158 = (std::cos((18829.7012f / fConst0)) * (0.0f - (2.0f * fConst157)));
		fConst159 = mydsp_faustpower2_f(fConst157);
		fConst160 = ftbl1mydspSIG1[84];
		float fConst161 = std::pow(0.00100000005f, (1.57685912f / fConst0));
		fConst162 = (std::cos((19934.0957f / fConst0)) * (0.0f - (2.0f * fConst161)));
		fConst163 = mydsp_faustpower2_f(fConst161);
		fConst164 = ftbl1mydspSIG1[85];
		float fConst165 = std::pow(0.00100000005f, (2.61620831f / fConst0));
		fConst166 = (std::cos((20927.9707f / fConst0)) * (0.0f - (2.0f * fConst165)));
		fConst167 = mydsp_faustpower2_f(fConst165);
		fConst168 = ftbl1mydspSIG1[86];
		float fConst169 = std::pow(0.00100000005f, (3.26228237f / fConst0));
		fConst170 = (std::cos((21302.0723f / fConst0)) * (0.0f - (2.0f * fConst169)));
		fConst171 = mydsp_faustpower2_f(fConst169);
		fConst172 = ftbl1mydspSIG1[87];
		float fConst173 = std::pow(0.00100000005f, (3.42948842f / fConst0));
		fConst174 = (std::cos((21382.3086f / fConst0)) * (0.0f - (2.0f * fConst173)));
		fConst175 = mydsp_faustpower2_f(fConst173);
		fConst176 = ftbl1mydspSIG1[88];
		float fConst177 = std::pow(0.00100000005f, (5.61909866f / fConst0));
		fConst178 = (std::cos((22094.3809f / fConst0)) * (0.0f - (2.0f * fConst177)));
		fConst179 = mydsp_faustpower2_f(fConst177);
		fConst180 = ftbl1mydspSIG1[89];
		float fConst181 = std::pow(0.00100000005f, (6.26517916f / fConst0));
		fConst182 = (std::cos((22233.3027f / fConst0)) * (0.0f - (2.0f * fConst181)));
		fConst183 = mydsp_faustpower2_f(fConst181);
		fConst184 = ftbl1mydspSIG1[90];
		float fConst185 = std::pow(0.00100000005f, (7.91042233f / fConst0));
		fConst186 = (std::cos((22511.334f / fConst0)) * (0.0f - (2.0f * fConst185)));
		fConst187 = mydsp_faustpower2_f(fConst185);
		fConst188 = ftbl1mydspSIG1[91];
		float fConst189 = std::pow(0.00100000005f, (7.95560217f / fConst0));
		fConst190 = (std::cos((22517.8047f / fConst0)) * (0.0f - (2.0f * fConst189)));
		fConst191 = mydsp_faustpower2_f(fConst189);
		fConst192 = ftbl1mydspSIG1[92];
		float fConst193 = std::pow(0.00100000005f, (21.1559925f / fConst0));
		fConst194 = (std::cos((23436.5332f / fConst0)) * (0.0f - (2.0f * fConst193)));
		fConst195 = mydsp_faustpower2_f(fConst193);
		fConst196 = ftbl1mydspSIG1[93];
		float fConst197 = std::pow(0.00100000005f, (22.8584423f / fConst0));
		fConst198 = (std::cos((23495.0293f / fConst0)) * (0.0f - (2.0f * fConst197)));
		fConst199 = mydsp_faustpower2_f(fConst197);
		fConst200 = ftbl1mydspSIG1[94];
		float fConst201 = std::pow(0.00100000005f, (27.0563507f / fConst0));
		fConst202 = (std::cos((23616.3574f / fConst0)) * (0.0f - (2.0f * fConst201)));
		fConst203 = mydsp_faustpower2_f(fConst201);
		fConst204 = ftbl1mydspSIG1[95];
		float fConst205 = std::pow(0.00100000005f, (173.368958f / fConst0));
		fConst206 = (std::cos((24528.1738f / fConst0)) * (0.0f - (2.0f * fConst205)));
		fConst207 = mydsp_faustpower2_f(fConst205);
		fConst208 = ftbl1mydspSIG1[96];
		float fConst209 = std::pow(0.00100000005f, (415.098541f / fConst0));
		fConst210 = (std::cos((24772.0234f / fConst0)) * (0.0f - (2.0f * fConst209)));
		fConst211 = mydsp_faustpower2_f(fConst209);
		fConst212 = ftbl1mydspSIG1[97];
		float fConst213 = std::pow(0.00100000005f, (1631.8125f / fConst0));
		fConst214 = (std::cos((25018.0098f / fConst0)) * (0.0f - (2.0f * fConst213)));
		fConst215 = mydsp_faustpower2_f(fConst213);
		fConst216 = ftbl1mydspSIG1[98];
		float fConst217 = std::pow(0.00100000005f, (1752.41809f / fConst0));
		fConst218 = (std::cos((25027.498f / fConst0)) * (0.0f - (2.0f * fConst217)));
		fConst219 = mydsp_faustpower2_f(fConst217);
		fConst220 = ftbl1mydspSIG1[99];
		float fConst221 = std::pow(0.00100000005f, (13161720.0f / fConst0));
		fConst222 = (std::cos((25346.1816f / fConst0)) * (0.0f - (2.0f * fConst221)));
		fConst223 = mydsp_faustpower2_f(fConst221);
		fConst224 = std::max<float>(1.0f, (0.00100000005f * fConst0));
		fConst225 = (1.0f / fConst224);
		fConst226 = (1.0f / std::max<float>(1.0f, (2.0f * fConst0)));
		float fConst227 = std::floor(((0.219990999f * fConst0) + 0.5f));
		float fConst228 = ((0.0f - (6.90775537f * fConst227)) / fConst0);
		float fConst229 = std::exp((0.25f * fConst228));
		float fConst230 = mydsp_faustpower2_f(fConst229);
		float fConst231 = std::cos((12566.3711f / fConst0));
		float fConst232 = (1.0f - (fConst230 * fConst231));
		float fConst233 = (1.0f - fConst230);
		float fConst234 = (fConst232 / fConst233);
		float fConst235 = std::sqrt(std::max<float>(0.0f, ((mydsp_faustpower2_f(fConst232) / mydsp_faustpower2_f(fConst233)) + -1.0f)));
		fConst236 = (fConst234 - fConst235);
		fConst237 = (fConst229 * (fConst235 + (1.0f - fConst234)));
		fConst238 = ((std::exp(fConst228) / fConst229) + -1.0f);
		float fConst239 = (1.0f / std::tan((314.159271f / fConst0)));
		fConst240 = (1.0f / (fConst239 + 1.0f));
		fConst241 = (1.0f - fConst239);
		float fConst242 = std::floor(((0.0191229992f * fConst0) + 0.5f));
		iConst243 = int(std::min<float>(16384.0f, std::max<float>(0.0f, (fConst227 - fConst242))));
		iConst244 = int(std::min<float>(8192.0f, std::max<float>(0.0f, (0.00999999978f * fConst0))));
		iConst245 = int(std::min<float>(1024.0f, std::max<float>(0.0f, (fConst242 + -1.0f))));
		float fConst246 = std::floor(((0.256891012f * fConst0) + 0.5f));
		float fConst247 = ((0.0f - (6.90775537f * fConst246)) / fConst0);
		float fConst248 = std::exp((0.25f * fConst247));
		float fConst249 = mydsp_faustpower2_f(fConst248);
		float fConst250 = (1.0f - (fConst249 * fConst231));
		float fConst251 = (1.0f - fConst249);
		float fConst252 = (fConst250 / fConst251);
		float fConst253 = std::sqrt(std::max<float>(0.0f, ((mydsp_faustpower2_f(fConst250) / mydsp_faustpower2_f(fConst251)) + -1.0f)));
		fConst254 = (fConst252 - fConst253);
		fConst255 = (fConst248 * (fConst253 + (1.0f - fConst252)));
		fConst256 = ((std::exp(fConst247) / fConst248) + -1.0f);
		float fConst257 = std::floor(((0.0273330007f * fConst0) + 0.5f));
		iConst258 = int(std::min<float>(16384.0f, std::max<float>(0.0f, (fConst246 - fConst257))));
		iConst259 = int(std::min<float>(2048.0f, std::max<float>(0.0f, (fConst257 + -1.0f))));
		float fConst260 = std::floor(((0.192303002f * fConst0) + 0.5f));
		float fConst261 = ((0.0f - (6.90775537f * fConst260)) / fConst0);
		float fConst262 = std::exp((0.25f * fConst261));
		float fConst263 = mydsp_faustpower2_f(fConst262);
		float fConst264 = (1.0f - (fConst263 * fConst231));
		float fConst265 = (1.0f - fConst263);
		float fConst266 = (fConst264 / fConst265);
		float fConst267 = std::sqrt(std::max<float>(0.0f, ((mydsp_faustpower2_f(fConst264) / mydsp_faustpower2_f(fConst265)) + -1.0f)));
		fConst268 = (fConst266 - fConst267);
		fConst269 = (fConst262 * (fConst267 + (1.0f - fConst266)));
		fConst270 = ((std::exp(fConst261) / fConst262) + -1.0f);
		float fConst271 = std::floor(((0.0292910002f * fConst0) + 0.5f));
		iConst272 = int(std::min<float>(8192.0f, std::max<float>(0.0f, (fConst260 - fConst271))));
		iConst273 = int(std::min<float>(2048.0f, std::max<float>(0.0f, (fConst271 + -1.0f))));
		float fConst274 = std::floor(((0.210389003f * fConst0) + 0.5f));
		float fConst275 = ((0.0f - (6.90775537f * fConst274)) / fConst0);
		float fConst276 = std::exp((0.25f * fConst275));
		float fConst277 = mydsp_faustpower2_f(fConst276);
		float fConst278 = (1.0f - (fConst277 * fConst231));
		float fConst279 = (1.0f - fConst277);
		float fConst280 = (fConst278 / fConst279);
		float fConst281 = std::sqrt(std::max<float>(0.0f, ((mydsp_faustpower2_f(fConst278) / mydsp_faustpower2_f(fConst279)) + -1.0f)));
		fConst282 = (fConst280 - fConst281);
		fConst283 = (fConst276 * (fConst281 + (1.0f - fConst280)));
		fConst284 = ((std::exp(fConst275) / fConst276) + -1.0f);
		float fConst285 = std::floor(((0.0244210009f * fConst0) + 0.5f));
		iConst286 = int(std::min<float>(16384.0f, std::max<float>(0.0f, (fConst274 - fConst285))));
		iConst287 = int(std::min<float>(2048.0f, std::max<float>(0.0f, (fConst285 + -1.0f))));
		float fConst288 = std::floor(((0.125f * fConst0) + 0.5f));
		float fConst289 = ((0.0f - (6.90775537f * fConst288)) / fConst0);
		float fConst290 = std::exp((0.25f * fConst289));
		float fConst291 = mydsp_faustpower2_f(fConst290);
		float fConst292 = (1.0f - (fConst291 * fConst231));
		float fConst293 = (1.0f - fConst291);
		float fConst294 = (fConst292 / fConst293);
		float fConst295 = std::sqrt(std::max<float>(0.0f, ((mydsp_faustpower2_f(fConst292) / mydsp_faustpower2_f(fConst293)) + -1.0f)));
		fConst296 = (fConst294 - fConst295);
		fConst297 = (fConst290 * (fConst295 + (1.0f - fConst294)));
		fConst298 = ((std::exp(fConst289) / fConst290) + -1.0f);
		float fConst299 = std::floor(((0.0134579996f * fConst0) + 0.5f));
		iConst300 = int(std::min<float>(8192.0f, std::max<float>(0.0f, (fConst288 - fConst299))));
		iConst301 = int(std::min<float>(1024.0f, std::max<float>(0.0f, (fConst299 + -1.0f))));
		float fConst302 = std::floor(((0.127837002f * fConst0) + 0.5f));
		float fConst303 = ((0.0f - (6.90775537f * fConst302)) / fConst0);
		float fConst304 = std::exp((0.25f * fConst303));
		float fConst305 = mydsp_faustpower2_f(fConst304);
		float fConst306 = (1.0f - (fConst305 * fConst231));
		float fConst307 = (1.0f - fConst305);
		float fConst308 = (fConst306 / fConst307);
		float fConst309 = std::sqrt(std::max<float>(0.0f, ((mydsp_faustpower2_f(fConst306) / mydsp_faustpower2_f(fConst307)) + -1.0f)));
		fConst310 = (fConst308 - fConst309);
		fConst311 = (fConst304 * (fConst309 + (1.0f - fConst308)));
		fConst312 = ((std::exp(fConst303) / fConst304) + -1.0f);
		float fConst313 = std::floor(((0.0316039994f * fConst0) + 0.5f));
		iConst314 = int(std::min<float>(8192.0f, std::max<float>(0.0f, (fConst302 - fConst313))));
		iConst315 = int(std::min<float>(2048.0f, std::max<float>(0.0f, (fConst313 + -1.0f))));
		float fConst316 = std::floor(((0.174713001f * fConst0) + 0.5f));
		float fConst317 = ((0.0f - (6.90775537f * fConst316)) / fConst0);
		float fConst318 = std::exp((0.25f * fConst317));
		float fConst319 = mydsp_faustpower2_f(fConst318);
		float fConst320 = (1.0f - (fConst319 * fConst231));
		float fConst321 = (1.0f - fConst319);
		float fConst322 = (fConst320 / fConst321);
		float fConst323 = std::sqrt(std::max<float>(0.0f, ((mydsp_faustpower2_f(fConst320) / mydsp_faustpower2_f(fConst321)) + -1.0f)));
		fConst324 = (fConst322 - fConst323);
		fConst325 = (fConst318 * (fConst323 + (1.0f - fConst322)));
		fConst326 = ((std::exp(fConst317) / fConst318) + -1.0f);
		float fConst327 = std::floor(((0.0229039993f * fConst0) + 0.5f));
		iConst328 = int(std::min<float>(8192.0f, std::max<float>(0.0f, (fConst316 - fConst327))));
		iConst329 = int(std::min<float>(2048.0f, std::max<float>(0.0f, (fConst327 + -1.0f))));
		float fConst330 = std::floor(((0.153128996f * fConst0) + 0.5f));
		float fConst331 = ((0.0f - (6.90775537f * fConst330)) / fConst0);
		float fConst332 = std::exp((0.25f * fConst331));
		float fConst333 = mydsp_faustpower2_f(fConst332);
		float fConst334 = (1.0f - (fConst333 * fConst231));
		float fConst335 = (1.0f - fConst333);
		float fConst336 = (fConst334 / fConst335);
		float fConst337 = std::sqrt(std::max<float>(0.0f, ((mydsp_faustpower2_f(fConst334) / mydsp_faustpower2_f(fConst335)) + -1.0f)));
		fConst338 = (fConst336 - fConst337);
		fConst339 = (fConst332 * (fConst337 + (1.0f - fConst336)));
		fConst340 = ((std::exp(fConst331) / fConst332) + -1.0f);
		float fConst341 = std::floor(((0.0203460008f * fConst0) + 0.5f));
		iConst342 = int(std::min<float>(8192.0f, std::max<float>(0.0f, (fConst330 - fConst341))));
		iConst343 = int(std::min<float>(1024.0f, std::max<float>(0.0f, (fConst341 + -1.0f))));
	}
	
	virtual void instanceResetUserInterface() {
		fVslider0 = FAUSTFLOAT(0.0f);
		fHslider0 = FAUSTFLOAT(0.0f);
		fHslider1 = FAUSTFLOAT(660.0f);
		fHslider2 = FAUSTFLOAT(0.0f);
		fHslider3 = FAUSTFLOAT(880.0f);
		fHslider4 = FAUSTFLOAT(1320.0f);
		fHslider5 = FAUSTFLOAT(440.0f);
		fHslider6 = FAUSTFLOAT(0.0f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
			iVec0[l0] = 0;
		}
		for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
			fRec1[l1] = 0.0f;
		}
		for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
			fRec3[l3] = 0.0f;
		}
		for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
			fRec0[l4] = 0.0f;
		}
		for (int l5 = 0; (l5 < 2); l5 = (l5 + 1)) {
			fRec6[l5] = 0.0f;
		}
		for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
			fRec7[l6] = 0.0f;
		}
		for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
			fRec8[l7] = 0.0f;
		}
		for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
			fRec5[l8] = 0.0f;
		}
		for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
			fVec1[l9] = 0.0f;
		}
		IOTA = 0;
		for (int l10 = 0; (l10 < 4096); l10 = (l10 + 1)) {
			fVec2[l10] = 0.0f;
		}
		for (int l11 = 0; (l11 < 2); l11 = (l11 + 1)) {
			fRec4[l11] = 0.0f;
		}
		for (int l12 = 0; (l12 < 2); l12 = (l12 + 1)) {
			fRec9[l12] = 0.0f;
		}
		for (int l13 = 0; (l13 < 2); l13 = (l13 + 1)) {
			fRec11[l13] = 0.0f;
		}
		for (int l14 = 0; (l14 < 2); l14 = (l14 + 1)) {
			fRec10[l14] = 0.0f;
		}
		for (int l15 = 0; (l15 < 2); l15 = (l15 + 1)) {
			fVec3[l15] = 0.0f;
		}
		for (int l16 = 0; (l16 < 4096); l16 = (l16 + 1)) {
			fVec4[l16] = 0.0f;
		}
		for (int l17 = 0; (l17 < 2); l17 = (l17 + 1)) {
			fRec13[l17] = 0.0f;
		}
		for (int l18 = 0; (l18 < 2); l18 = (l18 + 1)) {
			fRec12[l18] = 0.0f;
		}
		for (int l19 = 0; (l19 < 2); l19 = (l19 + 1)) {
			fVec5[l19] = 0.0f;
		}
		for (int l20 = 0; (l20 < 4096); l20 = (l20 + 1)) {
			fVec6[l20] = 0.0f;
		}
		for (int l21 = 0; (l21 < 2); l21 = (l21 + 1)) {
			fRec15[l21] = 0.0f;
		}
		for (int l22 = 0; (l22 < 2); l22 = (l22 + 1)) {
			fRec14[l22] = 0.0f;
		}
		for (int l23 = 0; (l23 < 2); l23 = (l23 + 1)) {
			fVec7[l23] = 0.0f;
		}
		for (int l24 = 0; (l24 < 4096); l24 = (l24 + 1)) {
			fVec8[l24] = 0.0f;
		}
		for (int l25 = 0; (l25 < 2); l25 = (l25 + 1)) {
			fRec16[l25] = 0.0f;
		}
		for (int l26 = 0; (l26 < 2); l26 = (l26 + 1)) {
			iRec20[l26] = 0;
		}
		for (int l27 = 0; (l27 < 3); l27 = (l27 + 1)) {
			fRec19[l27] = 0.0f;
		}
		for (int l28 = 0; (l28 < 3); l28 = (l28 + 1)) {
			fRec18[l28] = 0.0f;
		}
		for (int l29 = 0; (l29 < 2); l29 = (l29 + 1)) {
			iRec22[l29] = 0;
		}
		for (int l30 = 0; (l30 < 2); l30 = (l30 + 1)) {
			iVec9[l30] = 0;
		}
		for (int l31 = 0; (l31 < 2); l31 = (l31 + 1)) {
			iRec21[l31] = 0;
		}
		for (int l32 = 0; (l32 < 3); l32 = (l32 + 1)) {
			fRec17[l32] = 0.0f;
		}
		for (int l33 = 0; (l33 < 3); l33 = (l33 + 1)) {
			fRec23[l33] = 0.0f;
		}
		for (int l34 = 0; (l34 < 3); l34 = (l34 + 1)) {
			fRec24[l34] = 0.0f;
		}
		for (int l35 = 0; (l35 < 3); l35 = (l35 + 1)) {
			fRec25[l35] = 0.0f;
		}
		for (int l36 = 0; (l36 < 3); l36 = (l36 + 1)) {
			fRec26[l36] = 0.0f;
		}
		for (int l37 = 0; (l37 < 3); l37 = (l37 + 1)) {
			fRec27[l37] = 0.0f;
		}
		for (int l38 = 0; (l38 < 3); l38 = (l38 + 1)) {
			fRec28[l38] = 0.0f;
		}
		for (int l39 = 0; (l39 < 3); l39 = (l39 + 1)) {
			fRec29[l39] = 0.0f;
		}
		for (int l40 = 0; (l40 < 3); l40 = (l40 + 1)) {
			fRec30[l40] = 0.0f;
		}
		for (int l41 = 0; (l41 < 3); l41 = (l41 + 1)) {
			fRec31[l41] = 0.0f;
		}
		for (int l42 = 0; (l42 < 3); l42 = (l42 + 1)) {
			fRec32[l42] = 0.0f;
		}
		for (int l43 = 0; (l43 < 3); l43 = (l43 + 1)) {
			fRec33[l43] = 0.0f;
		}
		for (int l44 = 0; (l44 < 3); l44 = (l44 + 1)) {
			fRec34[l44] = 0.0f;
		}
		for (int l45 = 0; (l45 < 3); l45 = (l45 + 1)) {
			fRec35[l45] = 0.0f;
		}
		for (int l46 = 0; (l46 < 3); l46 = (l46 + 1)) {
			fRec36[l46] = 0.0f;
		}
		for (int l47 = 0; (l47 < 3); l47 = (l47 + 1)) {
			fRec37[l47] = 0.0f;
		}
		for (int l48 = 0; (l48 < 3); l48 = (l48 + 1)) {
			fRec38[l48] = 0.0f;
		}
		for (int l49 = 0; (l49 < 3); l49 = (l49 + 1)) {
			fRec39[l49] = 0.0f;
		}
		for (int l50 = 0; (l50 < 3); l50 = (l50 + 1)) {
			fRec40[l50] = 0.0f;
		}
		for (int l51 = 0; (l51 < 3); l51 = (l51 + 1)) {
			fRec41[l51] = 0.0f;
		}
		for (int l52 = 0; (l52 < 3); l52 = (l52 + 1)) {
			fRec42[l52] = 0.0f;
		}
		for (int l53 = 0; (l53 < 3); l53 = (l53 + 1)) {
			fRec43[l53] = 0.0f;
		}
		for (int l54 = 0; (l54 < 3); l54 = (l54 + 1)) {
			fRec44[l54] = 0.0f;
		}
		for (int l55 = 0; (l55 < 3); l55 = (l55 + 1)) {
			fRec45[l55] = 0.0f;
		}
		for (int l56 = 0; (l56 < 3); l56 = (l56 + 1)) {
			fRec46[l56] = 0.0f;
		}
		for (int l57 = 0; (l57 < 3); l57 = (l57 + 1)) {
			fRec47[l57] = 0.0f;
		}
		for (int l58 = 0; (l58 < 3); l58 = (l58 + 1)) {
			fRec48[l58] = 0.0f;
		}
		for (int l59 = 0; (l59 < 3); l59 = (l59 + 1)) {
			fRec49[l59] = 0.0f;
		}
		for (int l60 = 0; (l60 < 3); l60 = (l60 + 1)) {
			fRec50[l60] = 0.0f;
		}
		for (int l61 = 0; (l61 < 3); l61 = (l61 + 1)) {
			fRec51[l61] = 0.0f;
		}
		for (int l62 = 0; (l62 < 3); l62 = (l62 + 1)) {
			fRec52[l62] = 0.0f;
		}
		for (int l63 = 0; (l63 < 3); l63 = (l63 + 1)) {
			fRec53[l63] = 0.0f;
		}
		for (int l64 = 0; (l64 < 3); l64 = (l64 + 1)) {
			fRec54[l64] = 0.0f;
		}
		for (int l65 = 0; (l65 < 3); l65 = (l65 + 1)) {
			fRec55[l65] = 0.0f;
		}
		for (int l66 = 0; (l66 < 3); l66 = (l66 + 1)) {
			fRec56[l66] = 0.0f;
		}
		for (int l67 = 0; (l67 < 3); l67 = (l67 + 1)) {
			fRec57[l67] = 0.0f;
		}
		for (int l68 = 0; (l68 < 3); l68 = (l68 + 1)) {
			fRec58[l68] = 0.0f;
		}
		for (int l69 = 0; (l69 < 3); l69 = (l69 + 1)) {
			fRec59[l69] = 0.0f;
		}
		for (int l70 = 0; (l70 < 3); l70 = (l70 + 1)) {
			fRec60[l70] = 0.0f;
		}
		for (int l71 = 0; (l71 < 3); l71 = (l71 + 1)) {
			fRec61[l71] = 0.0f;
		}
		for (int l72 = 0; (l72 < 3); l72 = (l72 + 1)) {
			fRec62[l72] = 0.0f;
		}
		for (int l73 = 0; (l73 < 3); l73 = (l73 + 1)) {
			fRec63[l73] = 0.0f;
		}
		for (int l74 = 0; (l74 < 3); l74 = (l74 + 1)) {
			fRec64[l74] = 0.0f;
		}
		for (int l75 = 0; (l75 < 3); l75 = (l75 + 1)) {
			fRec65[l75] = 0.0f;
		}
		for (int l76 = 0; (l76 < 3); l76 = (l76 + 1)) {
			fRec66[l76] = 0.0f;
		}
		for (int l77 = 0; (l77 < 3); l77 = (l77 + 1)) {
			fRec67[l77] = 0.0f;
		}
		for (int l78 = 0; (l78 < 3); l78 = (l78 + 1)) {
			fRec68[l78] = 0.0f;
		}
		for (int l79 = 0; (l79 < 3); l79 = (l79 + 1)) {
			fRec69[l79] = 0.0f;
		}
		for (int l80 = 0; (l80 < 3); l80 = (l80 + 1)) {
			fRec70[l80] = 0.0f;
		}
		for (int l81 = 0; (l81 < 3); l81 = (l81 + 1)) {
			fRec71[l81] = 0.0f;
		}
		for (int l82 = 0; (l82 < 2); l82 = (l82 + 1)) {
			fRec83[l82] = 0.0f;
		}
		for (int l83 = 0; (l83 < 2); l83 = (l83 + 1)) {
			fRec82[l83] = 0.0f;
		}
		for (int l84 = 0; (l84 < 32768); l84 = (l84 + 1)) {
			fVec10[l84] = 0.0f;
		}
		for (int l85 = 0; (l85 < 2048); l85 = (l85 + 1)) {
			fVec11[l85] = 0.0f;
		}
		for (int l86 = 0; (l86 < 2048); l86 = (l86 + 1)) {
			fVec12[l86] = 0.0f;
		}
		for (int l87 = 0; (l87 < 2); l87 = (l87 + 1)) {
			fRec80[l87] = 0.0f;
		}
		for (int l88 = 0; (l88 < 2); l88 = (l88 + 1)) {
			fRec87[l88] = 0.0f;
		}
		for (int l89 = 0; (l89 < 2); l89 = (l89 + 1)) {
			fRec86[l89] = 0.0f;
		}
		for (int l90 = 0; (l90 < 32768); l90 = (l90 + 1)) {
			fVec13[l90] = 0.0f;
		}
		for (int l91 = 0; (l91 < 4096); l91 = (l91 + 1)) {
			fVec14[l91] = 0.0f;
		}
		for (int l92 = 0; (l92 < 2); l92 = (l92 + 1)) {
			fRec84[l92] = 0.0f;
		}
		for (int l93 = 0; (l93 < 2); l93 = (l93 + 1)) {
			fRec91[l93] = 0.0f;
		}
		for (int l94 = 0; (l94 < 2); l94 = (l94 + 1)) {
			fRec90[l94] = 0.0f;
		}
		for (int l95 = 0; (l95 < 16384); l95 = (l95 + 1)) {
			fVec15[l95] = 0.0f;
		}
		for (int l96 = 0; (l96 < 4096); l96 = (l96 + 1)) {
			fVec16[l96] = 0.0f;
		}
		for (int l97 = 0; (l97 < 2); l97 = (l97 + 1)) {
			fRec88[l97] = 0.0f;
		}
		for (int l98 = 0; (l98 < 2); l98 = (l98 + 1)) {
			fRec95[l98] = 0.0f;
		}
		for (int l99 = 0; (l99 < 2); l99 = (l99 + 1)) {
			fRec94[l99] = 0.0f;
		}
		for (int l100 = 0; (l100 < 32768); l100 = (l100 + 1)) {
			fVec17[l100] = 0.0f;
		}
		for (int l101 = 0; (l101 < 4096); l101 = (l101 + 1)) {
			fVec18[l101] = 0.0f;
		}
		for (int l102 = 0; (l102 < 2); l102 = (l102 + 1)) {
			fRec92[l102] = 0.0f;
		}
		for (int l103 = 0; (l103 < 2); l103 = (l103 + 1)) {
			fRec99[l103] = 0.0f;
		}
		for (int l104 = 0; (l104 < 2); l104 = (l104 + 1)) {
			fRec98[l104] = 0.0f;
		}
		for (int l105 = 0; (l105 < 16384); l105 = (l105 + 1)) {
			fVec19[l105] = 0.0f;
		}
		for (int l106 = 0; (l106 < 2048); l106 = (l106 + 1)) {
			fVec20[l106] = 0.0f;
		}
		for (int l107 = 0; (l107 < 2); l107 = (l107 + 1)) {
			fRec96[l107] = 0.0f;
		}
		for (int l108 = 0; (l108 < 2); l108 = (l108 + 1)) {
			fRec103[l108] = 0.0f;
		}
		for (int l109 = 0; (l109 < 2); l109 = (l109 + 1)) {
			fRec102[l109] = 0.0f;
		}
		for (int l110 = 0; (l110 < 16384); l110 = (l110 + 1)) {
			fVec21[l110] = 0.0f;
		}
		for (int l111 = 0; (l111 < 4096); l111 = (l111 + 1)) {
			fVec22[l111] = 0.0f;
		}
		for (int l112 = 0; (l112 < 2); l112 = (l112 + 1)) {
			fRec100[l112] = 0.0f;
		}
		for (int l113 = 0; (l113 < 2); l113 = (l113 + 1)) {
			fRec107[l113] = 0.0f;
		}
		for (int l114 = 0; (l114 < 2); l114 = (l114 + 1)) {
			fRec106[l114] = 0.0f;
		}
		for (int l115 = 0; (l115 < 16384); l115 = (l115 + 1)) {
			fVec23[l115] = 0.0f;
		}
		for (int l116 = 0; (l116 < 4096); l116 = (l116 + 1)) {
			fVec24[l116] = 0.0f;
		}
		for (int l117 = 0; (l117 < 2); l117 = (l117 + 1)) {
			fRec104[l117] = 0.0f;
		}
		for (int l118 = 0; (l118 < 2); l118 = (l118 + 1)) {
			fRec111[l118] = 0.0f;
		}
		for (int l119 = 0; (l119 < 2); l119 = (l119 + 1)) {
			fRec110[l119] = 0.0f;
		}
		for (int l120 = 0; (l120 < 16384); l120 = (l120 + 1)) {
			fVec25[l120] = 0.0f;
		}
		for (int l121 = 0; (l121 < 2048); l121 = (l121 + 1)) {
			fVec26[l121] = 0.0f;
		}
		for (int l122 = 0; (l122 < 2); l122 = (l122 + 1)) {
			fRec108[l122] = 0.0f;
		}
		for (int l123 = 0; (l123 < 3); l123 = (l123 + 1)) {
			fRec72[l123] = 0.0f;
		}
		for (int l124 = 0; (l124 < 3); l124 = (l124 + 1)) {
			fRec73[l124] = 0.0f;
		}
		for (int l125 = 0; (l125 < 3); l125 = (l125 + 1)) {
			fRec74[l125] = 0.0f;
		}
		for (int l126 = 0; (l126 < 3); l126 = (l126 + 1)) {
			fRec75[l126] = 0.0f;
		}
		for (int l127 = 0; (l127 < 3); l127 = (l127 + 1)) {
			fRec76[l127] = 0.0f;
		}
		for (int l128 = 0; (l128 < 3); l128 = (l128 + 1)) {
			fRec77[l128] = 0.0f;
		}
		for (int l129 = 0; (l129 < 3); l129 = (l129 + 1)) {
			fRec78[l129] = 0.0f;
		}
		for (int l130 = 0; (l130 < 3); l130 = (l130 + 1)) {
			fRec79[l130] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual mydsp* clone() {
		return new mydsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openTabBox("Mapping_Tabs");
		ui_interface->openHorizontalBox("Mixer_And_Master");
		ui_interface->openVerticalBox("Master_Gain");
		ui_interface->addVerticalSlider("Master Gain", &fVslider0, 0.0f, -96.0f, 12.0f, 0.00999999978f);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->openVerticalBox("Sonification_Control");
		ui_interface->openHorizontalBox("Soni 1");
		ui_interface->addHorizontalSlider("Control 1", &fHslider1, 660.0f, 20.0f, 2000.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("Control 2", &fHslider3, 880.0f, 20.0f, 2000.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("Control 3", &fHslider4, 1320.0f, 20.0f, 2000.0f, 0.00100000005f);
		ui_interface->addHorizontalSlider("Control 4", &fHslider5, 440.0f, 20.0f, 2000.0f, 0.00100000005f);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Soni 2");
		ui_interface->addHorizontalSlider("Control 0", &fHslider0, 0.0f, 0.0f, 1.0f, 0.00100000005f);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Soni 3");
		ui_interface->addHorizontalSlider("Control 0", &fHslider2, 0.0f, 0.0f, 1.0f, 0.00100000005f);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("Soni 4");
		ui_interface->addHorizontalSlider("Control 0", &fHslider6, 0.0f, 0.0f, 0.400000006f, 0.00100000005f);
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = std::pow(10.0f, (0.0500000007f * float(fVslider0)));
		float fSlow1 = (fConst2 * float(fHslider0));
		float fSlow2 = (fConst7 * float(fHslider1));
		float fSlow3 = (fConst2 * float(fHslider2));
		float fSlow4 = (fConst7 * float(fHslider3));
		float fSlow5 = (fConst7 * float(fHslider4));
		float fSlow6 = (fConst7 * float(fHslider5));
		float fSlow7 = (fConst2 * float(fHslider6));
		float fSlow8 = (0.600000024f * fSlow0);
		for (int i = 0; (i < count); i = (i + 1)) {
			iVec0[0] = 1;
			fRec1[0] = (fSlow1 + (fConst1 * fRec1[1]));
			float fTemp0 = (fRec3[1] + (fConst3 * ((4.5f * fRec1[0]) + 0.5f)));
			fRec3[0] = (fTemp0 - std::floor(fTemp0));
			fRec0[0] = ((0.999000013f * fRec0[1]) + (0.00100000005f * ((1.0f - (std::pow(fRec1[0], 0.400000006f) * std::fabs(ftbl0mydspSIG0[int((65536.0f * fRec3[0]))]))) * ((0.5f * fRec1[0]) + 1.0f))));
			float fTemp1 = float(iVec0[1]);
			fRec6[0] = (fSlow2 + (fConst6 * fRec6[1]));
			fRec7[0] = (fSlow3 + (fConst1 * fRec7[1]));
			fRec8[0] = (fConst8 + (fRec8[1] - std::floor((fConst8 + fRec8[1]))));
			float fTemp2 = (500.0f * (fRec7[0] * ftbl0mydspSIG0[int((65536.0f * fRec8[0]))]));
			float fTemp3 = (fRec6[0] + fTemp2);
			float fTemp4 = std::max<float>(fTemp3, 23.4489498f);
			float fTemp5 = std::max<float>(20.0f, std::fabs(fTemp4));
			float fTemp6 = (fRec5[1] + (fConst3 * fTemp5));
			fRec5[0] = (fTemp6 - std::floor(fTemp6));
			float fTemp7 = mydsp_faustpower2_f(((2.0f * fRec5[0]) + -1.0f));
			fVec1[0] = fTemp7;
			float fTemp8 = ((fTemp1 * (fTemp7 - fVec1[1])) / fTemp5);
			fVec2[(IOTA & 4095)] = fTemp8;
			float fTemp9 = std::max<float>(0.0f, std::min<float>(2047.0f, (fConst9 / fTemp4)));
			int iTemp10 = int(fTemp9);
			float fTemp11 = std::floor(fTemp9);
			fRec4[0] = ((0.999000013f * fRec4[1]) + (fConst5 * ((fTemp8 - (fVec2[((IOTA - iTemp10) & 4095)] * (fTemp11 + (1.0f - fTemp9)))) - ((fTemp9 - fTemp11) * fVec2[((IOTA - (iTemp10 + 1)) & 4095)]))));
			fRec9[0] = ((0.999000013f * fRec9[1]) + 0.000300000014f);
			fRec11[0] = (fSlow4 + (fConst6 * fRec11[1]));
			float fTemp12 = std::max<float>((fRec11[0] + fTemp2), 23.4489498f);
			float fTemp13 = std::max<float>(20.0f, std::fabs(fTemp12));
			float fTemp14 = (fRec10[1] + (fConst3 * fTemp13));
			fRec10[0] = (fTemp14 - std::floor(fTemp14));
			float fTemp15 = mydsp_faustpower2_f(((2.0f * fRec10[0]) + -1.0f));
			fVec3[0] = fTemp15;
			float fTemp16 = ((fTemp1 * (fTemp15 - fVec3[1])) / fTemp13);
			fVec4[(IOTA & 4095)] = fTemp16;
			float fTemp17 = std::max<float>(0.0f, std::min<float>(2047.0f, (fConst9 / fTemp12)));
			int iTemp18 = int(fTemp17);
			float fTemp19 = std::floor(fTemp17);
			fRec13[0] = (fSlow5 + (fConst6 * fRec13[1]));
			float fTemp20 = std::max<float>((fRec13[0] + fTemp2), 23.4489498f);
			float fTemp21 = std::max<float>(20.0f, std::fabs(fTemp20));
			float fTemp22 = (fRec12[1] + (fConst3 * fTemp21));
			fRec12[0] = (fTemp22 - std::floor(fTemp22));
			float fTemp23 = mydsp_faustpower2_f(((2.0f * fRec12[0]) + -1.0f));
			fVec5[0] = fTemp23;
			float fTemp24 = ((fTemp1 * (fTemp23 - fVec5[1])) / fTemp21);
			fVec6[(IOTA & 4095)] = fTemp24;
			float fTemp25 = std::max<float>(0.0f, std::min<float>(2047.0f, (fConst9 / fTemp20)));
			int iTemp26 = int(fTemp25);
			float fTemp27 = std::floor(fTemp25);
			fRec15[0] = (fSlow6 + (fConst6 * fRec15[1]));
			float fTemp28 = std::max<float>((fRec15[0] + fTemp2), 23.4489498f);
			float fTemp29 = std::max<float>(20.0f, std::fabs(fTemp28));
			float fTemp30 = (fRec14[1] + (fConst3 * fTemp29));
			fRec14[0] = (fTemp30 - std::floor(fTemp30));
			float fTemp31 = mydsp_faustpower2_f(((2.0f * fRec14[0]) + -1.0f));
			fVec7[0] = fTemp31;
			float fTemp32 = ((fTemp1 * (fTemp31 - fVec7[1])) / fTemp29);
			fVec8[(IOTA & 4095)] = fTemp32;
			float fTemp33 = std::max<float>(0.0f, std::min<float>(2047.0f, (fConst9 / fTemp28)));
			int iTemp34 = int(fTemp33);
			float fTemp35 = std::floor(fTemp33);
			fRec16[0] = (fSlow7 + (fConst1 * fRec16[1]));
			iRec20[0] = ((1103515245 * iRec20[1]) + 12345);
			fRec19[0] = ((4.65661287e-10f * float(iRec20[0])) - (fConst16 * ((fConst19 * fRec19[2]) + (fConst20 * fRec19[1]))));
			fRec18[0] = ((fConst16 * (((fConst18 * fRec19[0]) + (fConst21 * fRec19[1])) + (fConst18 * fRec19[2]))) - (fConst13 * ((fConst22 * fRec18[2]) + (fConst23 * fRec18[1]))));
			iRec22[0] = ((iVec0[1] + iRec22[1]) % 8000);
			int iTemp36 = (iRec22[0] == 0);
			iVec9[0] = iTemp36;
			iRec21[0] = (((iRec21[1] + (iRec21[1] > 0)) * (iTemp36 <= iVec9[1])) + (iTemp36 > iVec9[1]));
			float fTemp37 = float(iRec21[0]);
			float fTemp38 = (fConst24 * fTemp37);
			float fTemp39 = (fConst13 * ((fRec18[2] + (fRec18[0] + (2.0f * fRec18[1]))) * std::max<float>(0.0f, std::min<float>(fTemp38, (2.0f - fTemp38)))));
			fRec17[0] = (fTemp39 - ((fConst26 * fRec17[1]) + (fConst27 * fRec17[2])));
			fRec23[0] = (fTemp39 - ((fConst30 * fRec23[1]) + (fConst31 * fRec23[2])));
			fRec24[0] = (fTemp39 - ((fConst34 * fRec24[1]) + (fConst35 * fRec24[2])));
			fRec25[0] = (fTemp39 - ((fConst38 * fRec25[1]) + (fConst39 * fRec25[2])));
			fRec26[0] = (fTemp39 - ((fConst42 * fRec26[1]) + (fConst43 * fRec26[2])));
			fRec27[0] = (fTemp39 - ((fConst46 * fRec27[1]) + (fConst47 * fRec27[2])));
			fRec28[0] = (fTemp39 - ((fConst50 * fRec28[1]) + (fConst51 * fRec28[2])));
			fRec29[0] = (fTemp39 - ((fConst54 * fRec29[1]) + (fConst55 * fRec29[2])));
			fRec30[0] = (fTemp39 - ((fConst58 * fRec30[1]) + (fConst59 * fRec30[2])));
			fRec31[0] = (fTemp39 - ((fConst62 * fRec31[1]) + (fConst63 * fRec31[2])));
			fRec32[0] = (fTemp39 - ((fConst66 * fRec32[1]) + (fConst67 * fRec32[2])));
			fRec33[0] = (fTemp39 - ((fConst70 * fRec33[1]) + (fConst71 * fRec33[2])));
			fRec34[0] = (fTemp39 - ((fConst74 * fRec34[1]) + (fConst75 * fRec34[2])));
			fRec35[0] = (fTemp39 - ((fConst78 * fRec35[1]) + (fConst79 * fRec35[2])));
			fRec36[0] = (fTemp39 - ((fConst82 * fRec36[1]) + (fConst83 * fRec36[2])));
			fRec37[0] = (fTemp39 - ((fConst86 * fRec37[1]) + (fConst87 * fRec37[2])));
			fRec38[0] = (fTemp39 - ((fConst90 * fRec38[1]) + (fConst91 * fRec38[2])));
			fRec39[0] = (fTemp39 - ((fConst94 * fRec39[1]) + (fConst95 * fRec39[2])));
			fRec40[0] = (fTemp39 - ((fConst98 * fRec40[1]) + (fConst99 * fRec40[2])));
			fRec41[0] = (fTemp39 - ((fConst102 * fRec41[1]) + (fConst103 * fRec41[2])));
			fRec42[0] = (fTemp39 - ((fConst106 * fRec42[1]) + (fConst107 * fRec42[2])));
			fRec43[0] = (fTemp39 - ((fConst110 * fRec43[1]) + (fConst111 * fRec43[2])));
			fRec44[0] = (fTemp39 - ((fConst114 * fRec44[1]) + (fConst115 * fRec44[2])));
			fRec45[0] = (fTemp39 - ((fConst118 * fRec45[1]) + (fConst119 * fRec45[2])));
			fRec46[0] = (fTemp39 - ((fConst122 * fRec46[1]) + (fConst123 * fRec46[2])));
			fRec47[0] = (fTemp39 - ((fConst126 * fRec47[1]) + (fConst127 * fRec47[2])));
			fRec48[0] = (fTemp39 - ((fConst130 * fRec48[1]) + (fConst131 * fRec48[2])));
			fRec49[0] = (fTemp39 - ((fConst134 * fRec49[1]) + (fConst135 * fRec49[2])));
			fRec50[0] = (fTemp39 - ((fConst138 * fRec50[1]) + (fConst139 * fRec50[2])));
			fRec51[0] = (fTemp39 - ((fConst142 * fRec51[1]) + (fConst143 * fRec51[2])));
			fRec52[0] = (fTemp39 - ((fConst146 * fRec52[1]) + (fConst147 * fRec52[2])));
			fRec53[0] = (fTemp39 - ((fConst150 * fRec53[1]) + (fConst151 * fRec53[2])));
			fRec54[0] = (fTemp39 - ((fConst154 * fRec54[1]) + (fConst155 * fRec54[2])));
			fRec55[0] = (fTemp39 - ((fConst158 * fRec55[1]) + (fConst159 * fRec55[2])));
			fRec56[0] = (fTemp39 - ((fConst162 * fRec56[1]) + (fConst163 * fRec56[2])));
			fRec57[0] = (fTemp39 - ((fConst166 * fRec57[1]) + (fConst167 * fRec57[2])));
			fRec58[0] = (fTemp39 - ((fConst170 * fRec58[1]) + (fConst171 * fRec58[2])));
			fRec59[0] = (fTemp39 - ((fConst174 * fRec59[1]) + (fConst175 * fRec59[2])));
			fRec60[0] = (fTemp39 - ((fConst178 * fRec60[1]) + (fConst179 * fRec60[2])));
			fRec61[0] = (fTemp39 - ((fConst182 * fRec61[1]) + (fConst183 * fRec61[2])));
			fRec62[0] = (fTemp39 - ((fConst186 * fRec62[1]) + (fConst187 * fRec62[2])));
			fRec63[0] = (fTemp39 - ((fConst190 * fRec63[1]) + (fConst191 * fRec63[2])));
			fRec64[0] = (fTemp39 - ((fConst194 * fRec64[1]) + (fConst195 * fRec64[2])));
			fRec65[0] = (fTemp39 - ((fConst198 * fRec65[1]) + (fConst199 * fRec65[2])));
			fRec66[0] = (fTemp39 - ((fConst202 * fRec66[1]) + (fConst203 * fRec66[2])));
			fRec67[0] = (fTemp39 - ((fConst206 * fRec67[1]) + (fConst207 * fRec67[2])));
			fRec68[0] = (fTemp39 - ((fConst210 * fRec68[1]) + (fConst211 * fRec68[2])));
			fRec69[0] = (fTemp39 - ((fConst214 * fRec69[1]) + (fConst215 * fRec69[2])));
			fRec70[0] = (fTemp39 - ((fConst218 * fRec70[1]) + (fConst219 * fRec70[2])));
			fRec71[0] = (fTemp39 - ((fConst222 * fRec71[1]) + (fConst223 * fRec71[2])));
			float fTemp40 = ((0.100000001f * (mydsp_faustpower2_f(fRec0[0]) * ((fConst4 * (fRec4[0] * fTemp3)) + (fConst5 * ((fRec0[0] * fRec9[0]) * ((((fTemp16 - (fVec4[((IOTA - iTemp18) & 4095)] * (fTemp19 + (1.0f - fTemp17)))) - ((fTemp17 - fTemp19) * fVec4[((IOTA - (iTemp18 + 1)) & 4095)])) + ((fTemp24 - (fVec6[((IOTA - iTemp26) & 4095)] * (fTemp27 + (1.0f - fTemp25)))) - ((fTemp25 - fTemp27) * fVec6[((IOTA - (iTemp26 + 1)) & 4095)]))) + ((fTemp32 - (fVec8[((IOTA - iTemp34) & 4095)] * (fTemp35 + (1.0f - fTemp33)))) - ((fTemp33 - fTemp35) * fVec8[((IOTA - (iTemp34 + 1)) & 4095)])))))))) + (0.0199999996f * ((fRec16[0] * ((((((((((((((((((((((((((((((((((((((((((((((((((fConst10 * (fRec17[0] - fRec17[2])) + (fConst28 * (fRec23[0] - fRec23[2]))) + (fConst32 * (fRec24[0] - fRec24[2]))) + (fConst36 * (fRec25[0] - fRec25[2]))) + (fConst40 * (fRec26[0] - fRec26[2]))) + (fConst44 * (fRec27[0] - fRec27[2]))) + (fConst48 * (fRec28[0] - fRec28[2]))) + (fConst52 * (fRec29[0] - fRec29[2]))) + (fConst56 * (fRec30[0] - fRec30[2]))) + (fConst60 * (fRec31[0] - fRec31[2]))) + (fConst64 * (fRec32[0] - fRec32[2]))) + (fConst68 * (fRec33[0] - fRec33[2]))) + (fConst72 * (fRec34[0] - fRec34[2]))) + (fConst76 * (fRec35[0] - fRec35[2]))) + (fConst80 * (fRec36[0] - fRec36[2]))) + (fConst84 * (fRec37[0] - fRec37[2]))) + (fConst88 * (fRec38[0] - fRec38[2]))) + (fConst92 * (fRec39[0] - fRec39[2]))) + (fConst96 * (fRec40[0] - fRec40[2]))) + (fConst100 * (fRec41[0] - fRec41[2]))) + (fConst104 * (fRec42[0] - fRec42[2]))) + (fConst108 * (fRec43[0] - fRec43[2]))) + (fConst112 * (fRec44[0] - fRec44[2]))) + (fConst116 * (fRec45[0] - fRec45[2]))) + (fConst120 * (fRec46[0] - fRec46[2]))) + (fConst124 * (fRec47[0] - fRec47[2]))) + (fConst128 * (fRec48[0] - fRec48[2]))) + (fConst132 * (fRec49[0] - fRec49[2]))) + (fConst136 * (fRec50[0] - fRec50[2]))) + (fConst140 * (fRec51[0] - fRec51[2]))) + (fConst144 * (fRec52[0] - fRec52[2]))) + (fConst148 * (fRec53[0] - fRec53[2]))) + (fConst152 * (fRec54[0] - fRec54[2]))) + (fConst156 * (fRec55[0] - fRec55[2]))) + (fConst160 * (fRec56[0] - fRec56[2]))) + (fConst164 * (fRec57[0] - fRec57[2]))) + (fConst168 * (fRec58[0] - fRec58[2]))) + (fConst172 * (fRec59[0] - fRec59[2]))) + (fConst176 * (fRec60[0] - fRec60[2]))) + (fConst180 * (fRec61[0] - fRec61[2]))) + (fConst184 * (fRec62[0] - fRec62[2]))) + (fConst188 * (fRec63[0] - fRec63[2]))) + (fConst192 * (fRec64[0] - fRec64[2]))) + (fConst196 * (fRec65[0] - fRec65[2]))) + (fConst200 * (fRec66[0] - fRec66[2]))) + (fConst204 * (fRec67[0] - fRec67[2]))) + (fConst208 * (fRec68[0] - fRec68[2]))) + (fConst212 * (fRec69[0] - fRec69[2]))) + (fConst216 * (fRec70[0] - fRec70[2]))) + (fConst220 * (fRec71[0] - fRec71[2])))) * std::max<float>(0.0f, std::min<float>((fConst225 * fTemp37), ((fConst226 * (fConst224 - fTemp37)) + 1.0f))))));
			float fTemp41 = (fSlow0 * fTemp40);
			fRec83[0] = (0.0f - (fConst240 * ((fConst241 * fRec83[1]) - (fRec79[1] + fRec79[2]))));
			fRec82[0] = ((fConst236 * fRec82[1]) + (fConst237 * (fRec79[1] + (fConst238 * fRec83[0]))));
			fVec10[(IOTA & 32767)] = ((0.353553385f * fRec82[0]) + 9.99999968e-21f);
			fVec11[(IOTA & 2047)] = (fSlow8 * fTemp40);
			float fTemp42 = (0.300000012f * fVec11[((IOTA - iConst244) & 2047)]);
			float fTemp43 = (((0.600000024f * fRec80[1]) + fVec10[((IOTA - iConst243) & 32767)]) - fTemp42);
			fVec12[(IOTA & 2047)] = fTemp43;
			fRec80[0] = fVec12[((IOTA - iConst245) & 2047)];
			float fRec81 = (0.0f - (0.600000024f * fTemp43));
			fRec87[0] = (0.0f - (fConst240 * ((fConst241 * fRec87[1]) - (fRec75[1] + fRec75[2]))));
			fRec86[0] = ((fConst254 * fRec86[1]) + (fConst255 * (fRec75[1] + (fConst256 * fRec87[0]))));
			fVec13[(IOTA & 32767)] = ((0.353553385f * fRec86[0]) + 9.99999968e-21f);
			float fTemp44 = (((0.600000024f * fRec84[1]) + fVec13[((IOTA - iConst258) & 32767)]) - fTemp42);
			fVec14[(IOTA & 4095)] = fTemp44;
			fRec84[0] = fVec14[((IOTA - iConst259) & 4095)];
			float fRec85 = (0.0f - (0.600000024f * fTemp44));
			fRec91[0] = (0.0f - (fConst240 * ((fConst241 * fRec91[1]) - (fRec77[1] + fRec77[2]))));
			fRec90[0] = ((fConst268 * fRec90[1]) + (fConst269 * (fRec77[1] + (fConst270 * fRec91[0]))));
			fVec15[(IOTA & 16383)] = ((0.353553385f * fRec90[0]) + 9.99999968e-21f);
			float fTemp45 = (fVec15[((IOTA - iConst272) & 16383)] + (fTemp42 + (0.600000024f * fRec88[1])));
			fVec16[(IOTA & 4095)] = fTemp45;
			fRec88[0] = fVec16[((IOTA - iConst273) & 4095)];
			float fRec89 = (0.0f - (0.600000024f * fTemp45));
			fRec95[0] = (0.0f - (fConst240 * ((fConst241 * fRec95[1]) - (fRec73[1] + fRec73[2]))));
			fRec94[0] = ((fConst282 * fRec94[1]) + (fConst283 * (fRec73[1] + (fConst284 * fRec95[0]))));
			fVec17[(IOTA & 32767)] = ((0.353553385f * fRec94[0]) + 9.99999968e-21f);
			float fTemp46 = (fVec17[((IOTA - iConst286) & 32767)] + (fTemp42 + (0.600000024f * fRec92[1])));
			fVec18[(IOTA & 4095)] = fTemp46;
			fRec92[0] = fVec18[((IOTA - iConst287) & 4095)];
			float fRec93 = (0.0f - (0.600000024f * fTemp46));
			fRec99[0] = (0.0f - (fConst240 * ((fConst241 * fRec99[1]) - (fRec78[1] + fRec78[2]))));
			fRec98[0] = ((fConst296 * fRec98[1]) + (fConst297 * (fRec78[1] + (fConst298 * fRec99[0]))));
			fVec19[(IOTA & 16383)] = ((0.353553385f * fRec98[0]) + 9.99999968e-21f);
			float fTemp47 = (fVec19[((IOTA - iConst300) & 16383)] - (fTemp42 + (0.600000024f * fRec96[1])));
			fVec20[(IOTA & 2047)] = fTemp47;
			fRec96[0] = fVec20[((IOTA - iConst301) & 2047)];
			float fRec97 = (0.600000024f * fTemp47);
			fRec103[0] = (0.0f - (fConst240 * ((fConst241 * fRec103[1]) - (fRec74[1] + fRec74[2]))));
			fRec102[0] = ((fConst310 * fRec102[1]) + (fConst311 * (fRec74[1] + (fConst312 * fRec103[0]))));
			fVec21[(IOTA & 16383)] = ((0.353553385f * fRec102[0]) + 9.99999968e-21f);
			float fTemp48 = (fVec21[((IOTA - iConst314) & 16383)] - (fTemp42 + (0.600000024f * fRec100[1])));
			fVec22[(IOTA & 4095)] = fTemp48;
			fRec100[0] = fVec22[((IOTA - iConst315) & 4095)];
			float fRec101 = (0.600000024f * fTemp48);
			fRec107[0] = (0.0f - (fConst240 * ((fConst241 * fRec107[1]) - (fRec76[1] + fRec76[2]))));
			fRec106[0] = ((fConst324 * fRec106[1]) + (fConst325 * (fRec76[1] + (fConst326 * fRec107[0]))));
			fVec23[(IOTA & 16383)] = ((0.353553385f * fRec106[0]) + 9.99999968e-21f);
			float fTemp49 = ((fTemp42 + fVec23[((IOTA - iConst328) & 16383)]) - (0.600000024f * fRec104[1]));
			fVec24[(IOTA & 4095)] = fTemp49;
			fRec104[0] = fVec24[((IOTA - iConst329) & 4095)];
			float fRec105 = (0.600000024f * fTemp49);
			fRec111[0] = (0.0f - (fConst240 * ((fConst241 * fRec111[1]) - (fRec72[1] + fRec72[2]))));
			fRec110[0] = ((fConst338 * fRec110[1]) + (fConst339 * (fRec72[1] + (fConst340 * fRec111[0]))));
			fVec25[(IOTA & 16383)] = ((0.353553385f * fRec110[0]) + 9.99999968e-21f);
			float fTemp50 = ((fVec25[((IOTA - iConst342) & 16383)] + fTemp42) - (0.600000024f * fRec108[1]));
			fVec26[(IOTA & 2047)] = fTemp50;
			fRec108[0] = fVec26[((IOTA - iConst343) & 2047)];
			float fRec109 = (0.600000024f * fTemp50);
			float fTemp51 = (fRec109 + fRec105);
			float fTemp52 = (fRec97 + (fRec101 + fTemp51));
			fRec72[0] = (fRec80[1] + (fRec84[1] + (fRec88[1] + (fRec92[1] + (fRec96[1] + (fRec100[1] + (fRec104[1] + (fRec108[1] + (fRec81 + (fRec85 + (fRec89 + (fRec93 + fTemp52))))))))))));
			fRec73[0] = ((fRec96[1] + (fRec100[1] + (fRec104[1] + (fRec108[1] + fTemp52)))) - (fRec80[1] + (fRec84[1] + (fRec88[1] + (fRec92[1] + (fRec81 + (fRec85 + (fRec93 + fRec89))))))));
			float fTemp53 = (fRec101 + fRec97);
			fRec74[0] = ((fRec88[1] + (fRec92[1] + (fRec104[1] + (fRec108[1] + (fRec89 + (fRec93 + fTemp51)))))) - (fRec80[1] + (fRec84[1] + (fRec96[1] + (fRec100[1] + (fRec81 + (fRec85 + fTemp53)))))));
			fRec75[0] = ((fRec80[1] + (fRec84[1] + (fRec104[1] + (fRec108[1] + (fRec81 + (fRec85 + fTemp51)))))) - (fRec88[1] + (fRec92[1] + (fRec96[1] + (fRec100[1] + (fRec89 + (fRec93 + fTemp53)))))));
			float fTemp54 = (fRec109 + fRec101);
			float fTemp55 = (fRec105 + fRec97);
			fRec76[0] = ((fRec84[1] + (fRec92[1] + (fRec100[1] + (fRec108[1] + (fRec85 + (fRec93 + fTemp54)))))) - (fRec80[1] + (fRec88[1] + (fRec96[1] + (fRec104[1] + (fRec81 + (fRec89 + fTemp55)))))));
			fRec77[0] = ((fRec80[1] + (fRec88[1] + (fRec100[1] + (fRec108[1] + (fRec81 + (fRec89 + fTemp54)))))) - (fRec84[1] + (fRec92[1] + (fRec96[1] + (fRec104[1] + (fRec85 + (fRec93 + fTemp55)))))));
			float fTemp56 = (fRec109 + fRec97);
			float fTemp57 = (fRec105 + fRec101);
			fRec78[0] = ((fRec80[1] + (fRec92[1] + (fRec96[1] + (fRec108[1] + (fRec81 + (fRec93 + fTemp56)))))) - (fRec84[1] + (fRec88[1] + (fRec100[1] + (fRec104[1] + (fRec85 + (fRec89 + fTemp57)))))));
			fRec79[0] = ((fRec84[1] + (fRec88[1] + (fRec96[1] + (fRec108[1] + (fRec85 + (fRec89 + fTemp56)))))) - (fRec80[1] + (fRec92[1] + (fRec100[1] + (fRec104[1] + (fRec81 + (fRec93 + fTemp57)))))));
			output0[i] = FAUSTFLOAT((fTemp41 + (0.370000005f * (fRec73[0] + fRec74[0]))));
			output1[i] = FAUSTFLOAT((fTemp41 + (0.370000005f * (fRec73[0] - fRec74[0]))));
			iVec0[1] = iVec0[0];
			fRec1[1] = fRec1[0];
			fRec3[1] = fRec3[0];
			fRec0[1] = fRec0[0];
			fRec6[1] = fRec6[0];
			fRec7[1] = fRec7[0];
			fRec8[1] = fRec8[0];
			fRec5[1] = fRec5[0];
			fVec1[1] = fVec1[0];
			IOTA = (IOTA + 1);
			fRec4[1] = fRec4[0];
			fRec9[1] = fRec9[0];
			fRec11[1] = fRec11[0];
			fRec10[1] = fRec10[0];
			fVec3[1] = fVec3[0];
			fRec13[1] = fRec13[0];
			fRec12[1] = fRec12[0];
			fVec5[1] = fVec5[0];
			fRec15[1] = fRec15[0];
			fRec14[1] = fRec14[0];
			fVec7[1] = fVec7[0];
			fRec16[1] = fRec16[0];
			iRec20[1] = iRec20[0];
			fRec19[2] = fRec19[1];
			fRec19[1] = fRec19[0];
			fRec18[2] = fRec18[1];
			fRec18[1] = fRec18[0];
			iRec22[1] = iRec22[0];
			iVec9[1] = iVec9[0];
			iRec21[1] = iRec21[0];
			fRec17[2] = fRec17[1];
			fRec17[1] = fRec17[0];
			fRec23[2] = fRec23[1];
			fRec23[1] = fRec23[0];
			fRec24[2] = fRec24[1];
			fRec24[1] = fRec24[0];
			fRec25[2] = fRec25[1];
			fRec25[1] = fRec25[0];
			fRec26[2] = fRec26[1];
			fRec26[1] = fRec26[0];
			fRec27[2] = fRec27[1];
			fRec27[1] = fRec27[0];
			fRec28[2] = fRec28[1];
			fRec28[1] = fRec28[0];
			fRec29[2] = fRec29[1];
			fRec29[1] = fRec29[0];
			fRec30[2] = fRec30[1];
			fRec30[1] = fRec30[0];
			fRec31[2] = fRec31[1];
			fRec31[1] = fRec31[0];
			fRec32[2] = fRec32[1];
			fRec32[1] = fRec32[0];
			fRec33[2] = fRec33[1];
			fRec33[1] = fRec33[0];
			fRec34[2] = fRec34[1];
			fRec34[1] = fRec34[0];
			fRec35[2] = fRec35[1];
			fRec35[1] = fRec35[0];
			fRec36[2] = fRec36[1];
			fRec36[1] = fRec36[0];
			fRec37[2] = fRec37[1];
			fRec37[1] = fRec37[0];
			fRec38[2] = fRec38[1];
			fRec38[1] = fRec38[0];
			fRec39[2] = fRec39[1];
			fRec39[1] = fRec39[0];
			fRec40[2] = fRec40[1];
			fRec40[1] = fRec40[0];
			fRec41[2] = fRec41[1];
			fRec41[1] = fRec41[0];
			fRec42[2] = fRec42[1];
			fRec42[1] = fRec42[0];
			fRec43[2] = fRec43[1];
			fRec43[1] = fRec43[0];
			fRec44[2] = fRec44[1];
			fRec44[1] = fRec44[0];
			fRec45[2] = fRec45[1];
			fRec45[1] = fRec45[0];
			fRec46[2] = fRec46[1];
			fRec46[1] = fRec46[0];
			fRec47[2] = fRec47[1];
			fRec47[1] = fRec47[0];
			fRec48[2] = fRec48[1];
			fRec48[1] = fRec48[0];
			fRec49[2] = fRec49[1];
			fRec49[1] = fRec49[0];
			fRec50[2] = fRec50[1];
			fRec50[1] = fRec50[0];
			fRec51[2] = fRec51[1];
			fRec51[1] = fRec51[0];
			fRec52[2] = fRec52[1];
			fRec52[1] = fRec52[0];
			fRec53[2] = fRec53[1];
			fRec53[1] = fRec53[0];
			fRec54[2] = fRec54[1];
			fRec54[1] = fRec54[0];
			fRec55[2] = fRec55[1];
			fRec55[1] = fRec55[0];
			fRec56[2] = fRec56[1];
			fRec56[1] = fRec56[0];
			fRec57[2] = fRec57[1];
			fRec57[1] = fRec57[0];
			fRec58[2] = fRec58[1];
			fRec58[1] = fRec58[0];
			fRec59[2] = fRec59[1];
			fRec59[1] = fRec59[0];
			fRec60[2] = fRec60[1];
			fRec60[1] = fRec60[0];
			fRec61[2] = fRec61[1];
			fRec61[1] = fRec61[0];
			fRec62[2] = fRec62[1];
			fRec62[1] = fRec62[0];
			fRec63[2] = fRec63[1];
			fRec63[1] = fRec63[0];
			fRec64[2] = fRec64[1];
			fRec64[1] = fRec64[0];
			fRec65[2] = fRec65[1];
			fRec65[1] = fRec65[0];
			fRec66[2] = fRec66[1];
			fRec66[1] = fRec66[0];
			fRec67[2] = fRec67[1];
			fRec67[1] = fRec67[0];
			fRec68[2] = fRec68[1];
			fRec68[1] = fRec68[0];
			fRec69[2] = fRec69[1];
			fRec69[1] = fRec69[0];
			fRec70[2] = fRec70[1];
			fRec70[1] = fRec70[0];
			fRec71[2] = fRec71[1];
			fRec71[1] = fRec71[0];
			fRec83[1] = fRec83[0];
			fRec82[1] = fRec82[0];
			fRec80[1] = fRec80[0];
			fRec87[1] = fRec87[0];
			fRec86[1] = fRec86[0];
			fRec84[1] = fRec84[0];
			fRec91[1] = fRec91[0];
			fRec90[1] = fRec90[0];
			fRec88[1] = fRec88[0];
			fRec95[1] = fRec95[0];
			fRec94[1] = fRec94[0];
			fRec92[1] = fRec92[0];
			fRec99[1] = fRec99[0];
			fRec98[1] = fRec98[0];
			fRec96[1] = fRec96[0];
			fRec103[1] = fRec103[0];
			fRec102[1] = fRec102[0];
			fRec100[1] = fRec100[0];
			fRec107[1] = fRec107[0];
			fRec106[1] = fRec106[0];
			fRec104[1] = fRec104[0];
			fRec111[1] = fRec111[0];
			fRec110[1] = fRec110[0];
			fRec108[1] = fRec108[0];
			fRec72[2] = fRec72[1];
			fRec72[1] = fRec72[0];
			fRec73[2] = fRec73[1];
			fRec73[1] = fRec73[0];
			fRec74[2] = fRec74[1];
			fRec74[1] = fRec74[0];
			fRec75[2] = fRec75[1];
			fRec75[1] = fRec75[0];
			fRec76[2] = fRec76[1];
			fRec76[1] = fRec76[0];
			fRec77[2] = fRec77[1];
			fRec77[1] = fRec77[0];
			fRec78[2] = fRec78[1];
			fRec78[1] = fRec78[0];
			fRec79[2] = fRec79[1];
			fRec79[1] = fRec79[0];
		}
	}

};

#endif
