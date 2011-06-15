using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace NewFrontEnd {

    /// <summary>
    /// Interaction logic for KeySequences.xaml
    /// </summary>
    public partial class KeySequences: Page {

        public ObservableCollection<FcApi.KeySequence> KeySequenceCollection { get; set; }

        private FcApi.KeySequence copy( FcApi.KeySequence ks ) {
            var _ = new FcApi.KeySequence( );
            _.vkFirst = ks.vkFirst;
            _.vkSecond = ks.vkSecond;
            _.u32Composed = ks.u32Composed;
            return _;
        }

        public KeySequences( ) {
            InitializeComponent( );
            KeySequenceCollection = new ObservableCollection<FcApi.KeySequence>( );
            foreach ( KeyValuePair<string,MappingGroup> pair in ( (App) Application.Current ).Configuration.MappingGroups ) {
                MappingGroup group = pair.Value;
                foreach ( FcApi.KeySequence keySequence in group.KeySequences ) {
                    KeySequenceCollection.Add( copy( keySequence ) );
                }
            }
        }

    }

}
