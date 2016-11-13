using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using AudioLib;
using AudioLib.TF;
using SharpSoundDevice;

namespace DspAmp
{
    public class TestbedPlugin : IAudioDevice
    {
        // --------------- IAudioDevice Properties ---------------

        DeviceInfo DevInfo;

        public int DeviceId { get; set; }
        public DeviceInfo DeviceInfo { get { return this.DevInfo; } }
        public Parameter[] ParameterInfo { get; private set; }
        public Port[] PortInfo { get; private set; }
        public int CurrentProgram { get; private set; }

        // --------------- Necessary Parameters ---------------

        public double Samplerate;
        private GainStage stage;
        private double outputGain;
        
        public TestbedPlugin()
        {
            Samplerate = 48000;
            DevInfo = new DeviceInfo();
            ParameterInfo = new Parameter[6];
            PortInfo = new Port[2];

            stage = new GainStage(Samplerate);
        }

        public void InitializeDevice()
        {
            DevInfo.DeviceID = "Low Profile - DSP Amp";
            DevInfo.Developer = "Valdemar Erlingsson";
            DevInfo.UnsafeProcessing = false;
            DevInfo.EditorHeight = 0;
            DevInfo.EditorWidth = 0;
            DevInfo.HasEditor = false;
            DevInfo.Name = "DSP Amp Test";
            DevInfo.ProgramCount = 1;
            DevInfo.Type = DeviceType.Effect;
            DevInfo.Version = 1000;
            DevInfo.VstId = DeviceUtilities.GenerateIntegerId(DevInfo.DeviceID);

            PortInfo[0].Direction = PortDirection.Input;
            PortInfo[0].Name = "Stereo Input";
            PortInfo[0].NumberOfChannels = 2;

            PortInfo[1].Direction = PortDirection.Output;
            PortInfo[1].Name = "Stereo Output";
            PortInfo[1].NumberOfChannels = 2;

            for (int i = 0; i < ParameterInfo.Length; i++)
            {
                var p = new Parameter();
                p.Index = (uint)i;
                p.Name = "Param " + i;
                p.Steps = 0;
                p.Display = "0.0";
                p.Value = 0.0;
                ParameterInfo[i] = p;
            }

            ParameterInfo[0].Name = "Input Gain";
            ParameterInfo[1].Name = "Stage Gain";
            ParameterInfo[2].Name = "Feedback";
            ParameterInfo[3].Name = "Output";
            ParameterInfo[4].Name = "Lowpass";
            ParameterInfo[5].Name = "Highpass";
            SetParameter(0, 0.03);
            SetParameter(1, 1);
            SetParameter(2, 1);
            SetParameter(3, 0.2);
            SetParameter(4, 0.2);
            SetParameter(5, 0.8);
        }

        public void DisposeDevice() { }

        public void Start() { }

        public void Stop() { }

        public void ProcessSample(double[][] inputs, double[][] outputs, uint bufferSize)
        {
            var input = inputs[0];
            
            for (int i = 0; i < bufferSize; i++)
            {
                var processed = stage.Process(input[i]) * outputGain;
                outputs[0][i] = processed;
                outputs[1][i] = processed;
            }
        }

        public void ProcessSample(IntPtr input, IntPtr output, uint inChannelCount, uint outChannelCount, uint bufferSize)
        {
            throw new NotImplementedException();
        }

        public void OpenEditor(IntPtr parentWindow) { }

        public void CloseEditor() { }

        public bool SendEvent(Event ev)
        {
            if (ev.Type == EventType.Parameter)
            {
                SetParameter(ev.EventIndex, (double)ev.Data);
                return true;
            }
            
            return false;
        }

        private void SetParameter(int index, double value)
        {
            if (index >= 0 && index < ParameterInfo.Length)
            {
                ParameterInfo[index].Value = value;

                if (index == 0)
                {
                    ParameterInfo[index].Display = (value * 200).ToString("f3");
                    stage.InputGain = value * 200;
                }
                else if (index == 1)
                {
                    ParameterInfo[index].Display = (value * 100).ToString("f3");
                    stage.StageGain = value * 100;
                }
                else if (index == 2)
                {
                    ParameterInfo[index].Display = (value).ToString("f3");
                    stage.LowFeedback= value;
                    stage.HighFeedback = value;

                }
                else if (index == 3)
                {
                    var disp = (value * 60 - 60);
                    ParameterInfo[index].Display = disp.ToString("f3") + " dB";
                    outputGain = Utils.DB2gain(disp);
                }
                else if (index == 4)
                {
                    var freq = 50 + value * 500;
                    ParameterInfo[index].Display = freq.ToString("f3") + " Hz";
                    stage.LowpassCutoff = freq;
                }
                else if (index == 5)
                {
                    var freq = 2000 + value * 20000;
                    ParameterInfo[index].Display = freq.ToString("f3") + " Hz";
                    stage.HighpassCutoff = freq;
                }
            }
        }

        public void SetProgramData(Program program, int index)
        {
            try
            {
                DeviceUtilities.DeserializeParameters(ParameterInfo, program.Data);
                for (int i = 0; i < ParameterInfo.Length; i++)
                    SetParameter(i, ParameterInfo[i].Value);
            }
            catch (Exception)
            {

            }
        }

        public Program GetProgramData(int index)
        {
            var output = new Program();
            output.Data = DeviceUtilities.SerializeParameters(ParameterInfo);
            output.Name = "";
            return output;
        }

        public void HostChanged()
        {
            Samplerate = HostInfo.SampleRate;
        }

        public IHostInfo HostInfo { get; set; }

    }
}
