using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Shapeshifter.Ui
{
    public class ControlInstance
    {
        public EffectModule Module { get; set; }
        public int Page { get; set; }
        public int ParameterIndex { get; set; }
        public string Name { get; set; }
    }

    public class ControlMap
    {
        public static ControlInstance[] Map = 
        {
            new ControlInstance { Module = EffectModule.NoiseGate, Page = 0, ParameterIndex = 0, Name = "Detector\r\nGain" },
            new ControlInstance { Module = EffectModule.NoiseGate, Page = 0, ParameterIndex = 1, Name = "Reduction" },
            new ControlInstance { Module = EffectModule.NoiseGate, Page = 0, ParameterIndex = 2, Name = "Threshold" },
            new ControlInstance { Module = EffectModule.NoiseGate, Page = 0, ParameterIndex = 3, Name = "Slope" },
            new ControlInstance { Module = EffectModule.NoiseGate, Page = 0, ParameterIndex = 4, Name = "Release\r\nTime" },

            new ControlInstance { Module = EffectModule.Compressor, Page = 0, ParameterIndex = 0, Name = "Threshold" },
            new ControlInstance { Module = EffectModule.Compressor, Page = 0, ParameterIndex = 1, Name = "Rate" },
            new ControlInstance { Module = EffectModule.Compressor, Page = 0, ParameterIndex = 2, Name = "Attack" },
            new ControlInstance { Module = EffectModule.Compressor, Page = 0, ParameterIndex = 3, Name = "Release" },
            new ControlInstance { Module = EffectModule.Compressor, Page = 0, ParameterIndex = 4, Name = "Makeup\r\nGain" },

            new ControlInstance { Module = EffectModule.InputFilter, Page = 0, ParameterIndex = 0, Name = "Low\r\nType" },
            new ControlInstance { Module = EffectModule.InputFilter, Page = 0, ParameterIndex = 1, Name = "Low\r\nFreq" },
            new ControlInstance { Module = EffectModule.InputFilter, Page = 0, ParameterIndex = 2, Name = "Low\r\nGain" },
            new ControlInstance { Module = EffectModule.InputFilter, Page = 0, ParameterIndex = 3, Name = "High\r\nType" },
            new ControlInstance { Module = EffectModule.InputFilter, Page = 0, ParameterIndex = 4, Name = "High\r\nFreq" },
            new ControlInstance { Module = EffectModule.InputFilter, Page = 0, ParameterIndex = 5, Name = "High\r\nGain" },

            new ControlInstance { Module = EffectModule.InputFilter, Page = 1, ParameterIndex = 6, Name = "Peak 1\r\nGain" },
            new ControlInstance { Module = EffectModule.InputFilter, Page = 1, ParameterIndex = 7, Name = "Peak 1\r\nFreq" },
            new ControlInstance { Module = EffectModule.InputFilter, Page = 1, ParameterIndex = 8, Name = "Peak 1\r\nQ" },
            new ControlInstance { Module = EffectModule.InputFilter, Page = 1, ParameterIndex = 9, Name = "Peak 2\r\nGain" },
            new ControlInstance { Module = EffectModule.InputFilter, Page = 1, ParameterIndex = 10, Name = "Peak 2\r\nFreq" },
            new ControlInstance { Module = EffectModule.InputFilter, Page = 1, ParameterIndex = 11, Name = "Peak 2\r\nQ" },

            new ControlInstance { Module = EffectModule.Boost, Page = 0, ParameterIndex = 0, Name = "Drive" },
            new ControlInstance { Module = EffectModule.Boost, Page = 0, ParameterIndex = 1, Name = "Tone" },
            new ControlInstance { Module = EffectModule.Boost, Page = 0, ParameterIndex = 2, Name = "Mix" },
            new ControlInstance { Module = EffectModule.Boost, Page = 0, ParameterIndex = 3, Name = "Tightness" },
            new ControlInstance { Module = EffectModule.Boost, Page = 0, ParameterIndex = 4, Name = "Clipper\r\nType" },

        };

        
    }
}
