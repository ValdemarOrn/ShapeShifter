using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LowProfile.Core.Ui;

namespace Shapeshifter.Ui
{
    public class DisplayKnobState : ViewModelBase
    {
        private readonly EffectModule module;
        private readonly int parameterIndex;
        private double value;
        private string displayValue;

        public DisplayKnobState(
            EffectModule module,
            int parameterIndex)
        {
            this.module = module;
            this.parameterIndex = parameterIndex;
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
            }
        }

        public string DisplayValue
        {
            get { return displayValue; }
            set
            {
                displayValue = value;
                NotifyPropertyChanged();
            }
        }
    }

}
