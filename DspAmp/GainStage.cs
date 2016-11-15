using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AudioLib;
using AudioLib.TF;

namespace DspAmp
{
    public class GainStage
    {
        private double fs;
        private double feedback;
        private double lowpassCutoff;
        private double highpassCutoff;
        private TfUtil lp, hp;
        private TfUtil inputHp;

        public GainStage(double fs)
        {
            this.fs = fs;

            lp = new TfUtil(new[] { 0.0, 0.0 }, new[] { 0.0, 0.0 });
            hp = new TfUtil(new[] { 0.0, 0.0 }, new[] { 0.0, 0.0 });
            inputHp = new TfUtil(new[] { 0.0, 0.0 }, new[] { 0.0, 0.0 });

            LowpassCutoff = 200;
            HighpassCutoff = 2000;
            InputHighpass = 40;
        }

        public double Output;

        /// <summary>
        /// This is used to tweak the gain transfer functions, and to set the initial training rate
        /// of feedback. It is NOT a necessary value, and some stages like a triode will have a fixed
        /// value based on the type of valve, e.g. mu = 100 for a 12ax7. mu = StageGain essentially
        /// </summary>
        public double StageGain = 10.0;
        public double InputGain = 1.0;
        public double LowFeedback = 0.99;
        public double HighFeedback = 0.99;
        public double WideSpectrumFeedback = 0.0;
        public double Bias = 0.0;

        // used for gain stages like a tub triode, where the output is >=0
        // between 0...Unipolar supply (voltage)
        public bool UnipolarMode;
        public double UnipolarSupply;
        private double inputHighpass;

        public double LowpassCutoff
        {
            get { return lowpassCutoff; }
            set
            {
                lowpassCutoff = value;
                var z = Butterworth.ComputeCoefficients(fs, false, lowpassCutoff, 1);
                lp.Update(z.Item1, z.Item2);
            }
        }

        public double HighpassCutoff
        {
            get { return highpassCutoff; }
            set
            {
                highpassCutoff = value;
                var z = Butterworth.ComputeCoefficients(fs, true, highpassCutoff, 1);
                hp.Update(z.Item1, z.Item2);
            }
        }

        public double InputHighpass
        {
            get { return inputHighpass; }
            set
            {
                inputHighpass = value;
                var z = Butterworth.ComputeCoefficients(fs, true, inputHighpass, 1);
                inputHp.Update(z.Item1, z.Item2);
            }
        }

        public double Process(double input)
        {
            input = inputHp.Process1(input);

            input = input * InputGain + Bias;
            
            int iters = 0;

            double prevOutput = -1000.0;
            double prevDiff = 0.0;
            double stageInput = input + feedback;
            double gTrain = 1.0 / StageGain;
            double stageOutput = 0.0;

            while (true)
            {
                iters++;
                var inputWithFeedback = input + feedback;
                var diff = inputWithFeedback - stageInput;

                if (diff != 0 && prevDiff != 0 && Math.Sign(diff) != Math.Sign(prevDiff))
                    gTrain = gTrain * 0.9;
                else
                    gTrain = gTrain * 1.1;

                stageInput = stageInput + gTrain * diff;
                //stageOutput = Stages.BipolarTanh(stageInput, StageGain);
                stageOutput = Stages.UnipolarTanh(stageInput, UnipolarSupply);
                double lowpassed;
                double wideSpectrum;
                double unipolarFeedbackSignal;

                if (UnipolarMode)
                {
                    unipolarFeedbackSignal = 100 - stageOutput;
                    lowpassed = -lp.Process1(unipolarFeedbackSignal, false);
                    wideSpectrum = -unipolarFeedbackSignal;
                }
                else
                {
                    unipolarFeedbackSignal = stageOutput;
                    lowpassed = lp.Process1(unipolarFeedbackSignal, false);
                    wideSpectrum = stageOutput;
                }
                
                var highpassed = hp.Process1(stageOutput, false);
                
                feedback = LowFeedback * lowpassed 
                    + HighFeedback * highpassed 
                    + WideSpectrumFeedback * wideSpectrum;

                // todo, move 2 lines up
                if (Math.Abs(stageOutput - prevOutput) < 0.0001 && Math.Abs(diff) < 0.0001)
                {
                    lp.Process1(unipolarFeedbackSignal);
                    hp.Process1(stageOutput);
                    break;
                }

                prevOutput = stageOutput;
                prevDiff = diff;

                if (iters > 200)
                {
                    feedback = 0.0;
                    return Output; // return same value as last time as a fallback
                }
            }

            Output = stageOutput;
            return Output;
        }

        public static class Stages
        {
            public static double HardClipper(double input, double stageGain)
            {
                var d = -input * stageGain;
                if (d < -100)
                    d = -100 + (d + 100) * 0.01;
                if (d > 100)
                    d = 100 + (d - 100) * 0.01;

                return d;
            }

            public static double BipolarTanh(double input, double stageGain)
            {
                return -Math.Tanh(input * Math.Sqrt(stageGain)) * Math.Sqrt(stageGain);
            }

            public static double UnipolarTanh(double input, double maxValue)
            {
                return maxValue - (1 + Math.Tanh((2 * input +2.5))) * 0.5 * 0.9 * maxValue;
            }
        }
    }
}
