using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DspAmp.Ui
{
    public class ControlInstance
    {
        public EffectModule Module { get; set; }
        public int Page { get; set; }
        public int ParameterIndex { get; set; }
        public string Name { get; set; }
        public Func<double, string> Formatter { get; set; }
    }

    public class ControlMap
    {

        public static string DefaultFormatter(double x) => x.ToString("0.00");

        public static ControlInstance[] Map = 
        {
            new ControlInstance { Module = EffectModule.NoiseGate, Page = 0, ParameterIndex = 0, Name = "Detector\r\nGain", Formatter = DefaultFormatter },
            new ControlInstance { Module = EffectModule.NoiseGate, Page = 0, ParameterIndex = 1, Name = "Reduction", Formatter = DefaultFormatter },
            new ControlInstance { Module = EffectModule.NoiseGate, Page = 0, ParameterIndex = 2, Name = "Threshold", Formatter = DefaultFormatter },
            new ControlInstance { Module = EffectModule.NoiseGate, Page = 0, ParameterIndex = 3, Name = "Slope", Formatter = DefaultFormatter },
            new ControlInstance { Module = EffectModule.NoiseGate, Page = 0, ParameterIndex = 4, Name = "Release\r\nTime", Formatter = DefaultFormatter },

            new ControlInstance { Module = EffectModule.Compressor, Page = 0, ParameterIndex = 0, Name = "Threshold", Formatter = DefaultFormatter },
            new ControlInstance { Module = EffectModule.Compressor, Page = 0, ParameterIndex = 1, Name = "Rate", Formatter = DefaultFormatter },
            new ControlInstance { Module = EffectModule.Compressor, Page = 0, ParameterIndex = 2, Name = "Attack", Formatter = DefaultFormatter },
            new ControlInstance { Module = EffectModule.Compressor, Page = 0, ParameterIndex = 3, Name = "Release", Formatter = DefaultFormatter },
            new ControlInstance { Module = EffectModule.Compressor, Page = 0, ParameterIndex = 4, Name = "Makeup\r\nGain", Formatter = DefaultFormatter },

        };

        
    }
}
