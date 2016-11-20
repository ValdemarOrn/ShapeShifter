using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using LowProfile.Core.Extensions;
using LowProfile.Core.Ui;
using SharpOSC;

namespace Shapeshifter.Ui
{
    public class MainViewModel : ViewModelBase
    {
        private string programName;
        private DisplayKnobState[] knobState;
        private Dictionary<int, double> allParameterValues;
        private Dictionary<int, string> allParameterDisplays;
        private OscTranceiver tranceiver;
        private readonly ConcurrentDictionary<string, OscMessage> sendMessages;
        private volatile bool disableOscSend;

        public MainViewModel()
        {
            ProgramName = "Program 1";
            allParameterValues = new Dictionary<int, double>();
            allParameterDisplays = new Dictionary<int, string>();
            tranceiver = new OscTranceiver(1800, 1801);
            sendMessages = new ConcurrentDictionary<string, OscMessage>();
            var oscThread = new Thread(ProcessOscMessages) { IsBackground = true };
            oscThread.Start();

            Action<ToggleState> updateSelectedModule = mod =>
            {
                foreach (var kvp in SelectedModule)
                {
                    if (kvp.Key != mod.Module)
                        kvp.Value.Unset();
                }

                SetModule(mod.Module);
            };

            SelectedModule = Enum.GetValues(typeof(EffectModule)).Cast<EffectModule>()
                .ToDictionary(x => x, x => new ToggleState(x, updateSelectedModule));

            SetModule(EffectModule.NoiseGate);
            RequestFullUpdate();
        }
        
        public string ProgramName
        {
            get { return programName; }
            set
            {
                programName = value;
                NotifyPropertyChanged();
            }
        }

        public DisplayKnobState[] KnobState
        {
            get { return knobState; }
            set { knobState = value; NotifyPropertyChanged(); }
        }

        public Dictionary<EffectModule, ToggleState> SelectedModule { get; set; }

        private void ProcessOscMessages()
        {
            while (true)
            {
                try
                {
                    while (true)
                    {
                        var msg = tranceiver.Receive();
                        if (msg == null)
                            break;

                        ProcessOscMessage(msg);
                    }

                    lock (sendMessages)
                    {
                        var keys = sendMessages.Keys.ToArray();
                        foreach (var key in keys)
                        {
                            try
                            {
                                var oscMsg = sendMessages[key];
                                OscMessage val;
                                sendMessages.TryRemove(key, out val);
                                var bytes = oscMsg.GetBytes();
                                tranceiver.Send(bytes);
                            }
                            catch (Exception ex)
                            {
                                Console.WriteLine(ex.GetTrace());
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.GetTrace());
                }

                Thread.Sleep(2);
            }
        }

        private void ProcessOscMessage(byte[] bytes)
        {
            var msg = OscPacket.GetPacket(bytes) as OscMessage;
            Console.WriteLine(msg.ToString());

            var parts = msg.Address.Split(new[] { '/' }, StringSplitOptions.RemoveEmptyEntries);

            if (parts[0] == "Control")
                ProcessControlMessage(msg);

            EffectModule module;
            var ok = Enum.TryParse<EffectModule>(parts[0], out module);
            if (ok)
            {
                var index = Convert.ToInt32(parts[1]);
                UpdateParameterFeedback(module, index, (float)msg.Arguments[0], (string)msg.Arguments[1]);
            }
        }

        private void ProcessControlMessage(OscMessage msg)
        {
            var parts = msg.Address.Split(new[] { '/' }, StringSplitOptions.RemoveEmptyEntries);
            if (parts[0] != "Control" || parts.Length < 2)
                return;

            if (parts[1] == "InputFilterResponse")
            {
                var points = msg.Arguments[0] as string;
                var values = points
                    .Split(',')
                    .Where(x => !string.IsNullOrWhiteSpace(x))
                    .Select(x => double.Parse(x, CultureInfo.InvariantCulture))
                    .ToArray();
            }

        }

        private void RequestFullUpdate()
        {
            var address = "/Control/RequestUpdate";
            sendMessages[address] = new OscMessage(address);
        }

        private void UpdateParameterFeedback(EffectModule module, int parameterIndex, float value, string display)
        {
            var idx = (((int)module) << 8) | parameterIndex;
            allParameterValues[idx] = value;
            allParameterDisplays[idx] = display;

            foreach (var kvp in KnobState)
            {
                if (kvp.Module == module && kvp.ParameterIndex == parameterIndex)
                {
                    // only update is received value is different from current value.
                    // otherwise this causes an infinite feedback loop
                    if (Math.Abs(kvp.Value - value) > 0.0001)
                    {
                        try
                        {
                            disableOscSend = true;
                            kvp.Value = value;
                        }
                        finally
                        {
                            disableOscSend = false;
                        }
                    }

                    kvp.DisplayValue = display;
                }
            }
        }

        private void SetModule(EffectModule module)
        {
            SelectedModule[module].IsSelected = true;

            var controls = ControlMap.Map
                .Where(x => x.Module == module && x.Page == 0)
                .OrderBy(x => x.ParameterIndex)
                .ToArray();

            var newStates = new DisplayKnobState[8];
            for (int i = 0; i < newStates.Length; i++)
            {
                if (controls.Length > i)
                {
                    var c = controls[i];
                    newStates[i] = new DisplayKnobState(c.Module, c.ParameterIndex)
                    {
                        IsVisible = true,
                        Name = c.Name,
                        Value = GetValue(c.Module, c.ParameterIndex) ?? 0.0,
                        DisplayValue  = GetDisplay(c.Module, c.ParameterIndex) ?? ""
                    };

                    newStates[i].PropertyChanged += KnobUpdateHandler;
                }
                else
                {
                    newStates[i] = new DisplayKnobState(0, 0)
                    {
                        IsVisible = false,
                        Name = "Not in use",
                    };
                }
            }

            KnobState = newStates;
        }

        private void KnobUpdateHandler(object s, PropertyChangedEventArgs e)
        {
            var state = (DisplayKnobState)s;
            if (e.PropertyName != "Value")
                return;

            var val = state.Value;
            SetValue(state.Module, state.ParameterIndex, val, true, false);
        }


        private double? GetValue(EffectModule module, int parameterIndex)
        {
            var idx = (((int)module) << 8) | parameterIndex;
            if (allParameterValues.ContainsKey(idx))
                return allParameterValues[idx];

            return null;
        }

        private string GetDisplay(EffectModule module, int parameterIndex)
        {
            var idx = (((int)module) << 8) | parameterIndex;
            if (allParameterDisplays.ContainsKey(idx))
                return allParameterDisplays[idx];

            return null;
        }

        private void SetValue(EffectModule module, int parameterIndex, double val,
            bool sendOscUpdate,
            bool updateGui)
        {
            var idx = (((int)module) << 8) | parameterIndex;
            allParameterValues[idx] = val;

            if (sendOscUpdate && !disableOscSend)
            {
                var address = $"/{module}/{parameterIndex}";
                sendMessages[address] = new OscMessage(address, (float)val);
            }

            if (updateGui)
            {
                foreach (var kvp in KnobState)
                {
                    if (kvp.Module == module && kvp.ParameterIndex == parameterIndex)
                    {
                        kvp.Value = val;
                        break;
                    }
                }
            }
        }
    }
}
