using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Shapeshifter.Ui
{
    public enum EffectModule
    {
        None = 0,

	    NoiseGate = 1,
	    Compressor,
	    InputFilter,
	    Boost,
	    Preamplifier,
	    OutputFilter,
	    OutputStage,
	    GraphicEq,
	    Cabinet,
	    Global
    }
}
