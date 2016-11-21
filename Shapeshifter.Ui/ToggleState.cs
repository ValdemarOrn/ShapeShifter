using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LowProfile.Core.Ui;

namespace Shapeshifter.Ui
{
    public class ToggleState : ViewModelBase
    {
        private Action<ToggleState> onUpdate;
        private bool isSelected;

        public ToggleState(EffectModule module, Action<ToggleState> onUpdate)
        {
            Module = module;
            this.onUpdate = onUpdate;
        }

        public EffectModule Module { get; set; }

        public bool IsSelected
        {
            get { return isSelected; }
            set
            {
                var last = IsSelected;
                isSelected = true;
                NotifyPropertyChanged();

                onUpdate?.Invoke(this);
            }
        }

        public void Unset()
        {
            isSelected = false;
            NotifyPropertyChanged(nameof(IsSelected));
        }
    }

}
