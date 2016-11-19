using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LowProfile.Core.Ui;

namespace DspAmp.Ui
{
    public class DisplayKnobState : ViewModelBase
    {
        private readonly EffectModule module;
        private readonly int parameterIndex;
        private readonly Func<double, string> formatter;
        private double value;

        public DisplayKnobState(
            EffectModule module,
            int parameterIndex,
            Func<double, string> formatter)
        {
            this.module = module;
            this.parameterIndex = parameterIndex;
            this.formatter = formatter;
        }

        public EffectModule Module => module;
        public int ParameterIndex => parameterIndex;

        public string Name { get; set; }
        public bool IsVisible { get; set; }

        public double Value
        {
            get { return value; }
            set
            {
                this.value = value;
                NotifyPropertyChanged();
                NotifyPropertyChanged(nameof(DisplayValue));
            }
        }

        public string DisplayValue => formatter(value);
    }

}
