using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LowProfile.Visuals;

namespace DspAmp.Test
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            var pm = new OxyPlot.PlotModel();
            var fs = 48000.0;
            var stage = new GainStage(fs);

            stage.Bias = 0.0;
            stage.LowFeedback = 1.0;
            stage.HighFeedback = 1.0;
            stage.WideSpectrumFeedback = 0.0;
            stage.HighpassCutoff = 20000;
            stage.InputGain = 40.0;
            stage.LowpassCutoff = 10000.0;
            stage.StageGain = 100;

            var inputs = new List<double>();
            var outputs = new List<double>();
            for (int i = 0; i < 500; i++)
            {
                var input = Math.Sin(i * 500 / fs * 2 * Math.PI);
                var output = stage.Process(input);

                inputs.Add(input);
                outputs.Add(output);
            }

            pm.AddLine(inputs).Title = "Input";
            pm.AddLine(outputs).Title = "Output";
            pm.Show(asDialog:true);
            
        }
    }
}
