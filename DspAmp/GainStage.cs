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

        public GainStage(double fs)
        {
            this.fs = fs;

            lp = new TfUtil(new[] { 0.0, 0.0 }, new[] { 0.0, 0.0 });
            hp = new TfUtil(new[] { 0.0, 0.0 }, new[] { 0.0, 0.0 });

            LowpassCutoff = 200;
            HighpassCutoff = 2000;
        }

        public double InputGain = 1.0;
        public double StageGain = 10.0;
        public double LowFeedback = 0.99;
        public double HighFeedback = 0.99;
        public double WideSpectrumFeedback = 0.0;
        public double Bias = 0.0;
        

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

        public double Process(double input)
        {
            input = input * InputGain + Bias;
            
            int iters = 0;
            //feedback = 0.0;

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
                stageOutput = GF(stageInput);

                var lowpassed = lp.Process1(stageOutput, false);
                var highpassed = hp.Process1(stageOutput, false);
                
                feedback = LowFeedback * lowpassed 
                    + HighFeedback * highpassed 
                    + WideSpectrumFeedback * stageOutput;

                // todo, move 2 lines up
                if (Math.Abs(stageOutput - prevOutput) < 0.0001 && Math.Abs(diff) < 0.0001)
                {
                    lp.Process1(stageOutput);
                    hp.Process1(stageOutput);
                    break;
                }

                prevOutput = stageOutput;
                prevDiff = diff;

                if (iters > 200)
                {
                    feedback = 0.0;
                    return 0.0;
                }
            }

            return stageOutput;
        }
        
        private double GF(double input)
        {
            /*var d = -input * StageGain;
            if (d < -100)
                d = -100 + (d + 100) * 0.01;
            if (d > 100)
                d = 100 + (d - 100) * 0.01;

            return d;*/


            return -Math.Tanh(input * Math.Sqrt(StageGain)) * Math.Sqrt(StageGain);
        }

        private double GF2(double input)
        {
            return Math.Sqrt(StageGain) - Math.Tanh((input + 1) * Math.Sqrt(StageGain)) * Math.Sqrt(StageGain);
        }
    }
}
