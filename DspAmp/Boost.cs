using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AudioLib;
using AudioLib.Modules;
using AudioLib.SplineLut;
using AudioLib.TF;

namespace DspAmp
{
    public class Boost
    {
        public enum ClipperType
        {
            Diode,
            Zener,
            Led,
            Jfet
        }

        private double fs;
        private ClipperType clipper;
        private double tone;
        private SplineInterpolator spline;
        private TfUtil hpFilterGain;
        private TfUtil hpFilterOut;
        private TfUtil lpTone;
        private double toneFreq;
        private double drive;
        private double effectiveDrive;
        private int bufferSize;
        private double[] buffer;
        private double tightness;

        public Boost(double fs, int bufferSize)
        {
            this.bufferSize = bufferSize;
            this.buffer = new double[bufferSize];
            this.fs = fs;
            Clipper = ClipperType.Diode;
            hpFilterGain = new TfUtil(1);
            hpFilterOut = new TfUtil(1);
            lpTone = new TfUtil(1);

            var hpOut = Butterworth.ComputeCoefficients(fs, true, 70, 1);
            hpFilterOut.Update(hpOut.Item1, hpOut.Item2);

            Tightness = 0.4;
            Tone = 1;
        }

        public double Mix { get; set; }

        public double Drive
        {
            get { return drive; }
            set
            {
                drive = value;
                effectiveDrive = ValueTables.Get(Drive, ValueTables.Response2Dec) * 200;
            }
        }

        public double Tone
        {
            get { return tone; }
            set
            {
                this.toneFreq = 700 + ValueTables.Get(Tone, ValueTables.Response2Dec) * 4300;
                tone = value;
                var lp = Butterworth.ComputeCoefficients(fs, false, toneFreq, 1);
                lpTone.Update(lp.Item1, lp.Item2);
            }
        }

        public double Tightness
        {
            get { return tightness; }
            set
            {
                var freq = 200 + ValueTables.Get(Tone, ValueTables.Response2Oct) * 1800;
                tightness = value;
                var hpGain = Butterworth.ComputeCoefficients(fs, true, freq, 1);
                hpFilterGain.Update(hpGain.Item1, hpGain.Item2);
            }
        }

        public ClipperType Clipper
        {
            get { return clipper; }
            set
            {
                clipper = value;
                switch (clipper)
                {
                    case ClipperType.Diode:
                        spline = new SplineInterpolator(SplineInterpolator.Splines.D1N914Clipper, false);
                        break;
                    case ClipperType.Jfet:
                        spline = new SplineInterpolator(SplineInterpolator.Splines.JfetStageHighGain, false);
                        break;
                    case ClipperType.Led:
                        spline = new SplineInterpolator(SplineInterpolator.Splines.LedClipper, false);
                        break;
                    case ClipperType.Zener:
                        spline = new SplineInterpolator(SplineInterpolator.Splines.ZenerClipper, false);
                        break;
                }
            }
        }

        public double GetToneFreq()
        {
            return toneFreq;
        }

        public string GetClipperType()
        {
            return Clipper.ToString();
        }

        public void Process(double[] input, double[] output)
        {
            for (int i = 0; i < bufferSize; i++)
                buffer[i] = hpFilterGain.Process1(input[i]) * effectiveDrive;

            spline.ProcessInPlace(buffer);
            double mix = Mix;

            for (int i = 0; i < bufferSize; i++)
                buffer[i] = buffer[i] * mix + input[i];

            for (int i = 0; i < bufferSize; i++)
                output[i] = lpTone.Process1(hpFilterOut.Process1(buffer[i])) * 0.3;
        }

        public double Process(double input)
        {
            double buffer = 0.0;

            buffer = hpFilterGain.Process1(input) * effectiveDrive;
            buffer = spline.Process(buffer);
            double mix = Mix;

            buffer = buffer * mix + input;
            var output = lpTone.Process1(hpFilterOut.Process1(buffer));
            return output;
        }
    }
}
