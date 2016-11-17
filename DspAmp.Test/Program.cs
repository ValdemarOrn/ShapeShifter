using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AudioLib;
using AudioLib.TF;
using LowProfile.Visuals;

namespace DspAmp.Test
{
    class Program
    { 
        double fs = 48000.0;

        [STAThread]
        static void Main(string[] args)
        {
            var p = new Program();

            //p.TestUnipolarTanh();
            //p.TestUnipolarStage();
            //p.TestBoost();
            p.TestFreqz();
            //p.TestStage();
        }

        private void TestFreqz()
        {
            var cc = Butterworth.ComputeCoefficients(48000, false, 1000, 4);
            var res = Freqz.Compute(cc.Item1, cc.Item2, 2000);

            var mags = res.ToDictionary(x => x.W, x => Utils.Gain2DB(x.Magnitude));
            var angles = res.ToDictionary(x => x.W, x => x.Phase);

            var pm = new OxyPlot.PlotModel();
           // pm.AddLine(mags, x => x.Value, x => x.Key);
            pm.AddLine(angles, x => x.Value, x => x.Key);
            pm.Axes.Add(new OxyPlot.Axes.LogarithmicAxis() { Position = OxyPlot.Axes.AxisPosition.Bottom });
            pm.Show();
        }

        private void TestBoost()
        {
            var b = new Boost(fs, 1000);
            b.Drive = 1;
            b.Mix = 1.0;
            b.Tone = 1.0;

            var pm = new OxyPlot.PlotModel();

            var inputs = new List<double>();
            var outputs = new List<double>();
            for (int i = 0; i < 1000; i++)
            {
                var input = Math.Sin(i * 1000 / fs * 2 * Math.PI);
                var output = b.Process(input);

                inputs.Add(input);
                outputs.Add(output);
            }

            //pm.AddLine(inputs, outputs).Title = "Transfer";
            pm.AddLine(inputs).Title = "Inputs";
            pm.AddLine(outputs).Title = "Outputs";
            pm.Show(asDialog: true);
        }

        private void TestUnipolarTanh()
        {
            var pm = new OxyPlot.PlotModel();
            
            var inputs = new List<double>();
            var outputs = new List<double>();
            for (int i = 24; i < 500; i++)
            {
                var input = Math.Sin(i * 500 / fs * 2 * Math.PI) * 3 - 1;
                //var input = -5 + i / 50.0;
                var output = GainStage.Stages.UnipolarTanh(input, 100);

                inputs.Add(input);
                outputs.Add(output);
            }

            //pm.AddLine(inputs, outputs).Title = "Transfer";
            pm.AddLine(inputs).Title = "Inputs";
            pm.AddLine(outputs).Title = "Outputs"; 
            pm.Show(asDialog: true);
        }

        private void TestUnipolarStage()
        {
            var pm = new OxyPlot.PlotModel();
            var stage = new GainStage(fs);
            stage.Bias = 75.0;
            stage.LowFeedback = 0.03;
            stage.HighFeedback = 0.0;
            stage.WideSpectrumFeedback = 1;
            stage.HighpassCutoff = 20000;
            stage.InputGain = 2.0;
            stage.LowpassCutoff = 20.0;
            stage.StageGain = 100;
            stage.UnipolarMode = true;
            stage.UnipolarSupply = 100;

            var inputs = new List<double>();
            var outputs = new List<double>();
            for (int i = 0; i < 5000; i++)
            {
                var input = Math.Sin(i * 500 / fs * 2 * Math.PI);
                var output = stage.Process(input);

                inputs.Add(input);
                outputs.Add(output);
            }

            //pm.AddLine(inputs, outputs).Title = "Transfer";
            pm.AddLine(inputs.Skip(4500)).Title = "Inputs";
            pm.AddLine(outputs.Skip(4500)).Title = "Outputs";
            pm.Show(asDialog: true);
        }

        private void TestStage()
        {
            var pm = new OxyPlot.PlotModel();

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
            pm.Show(asDialog: true);
        }
    }
}
