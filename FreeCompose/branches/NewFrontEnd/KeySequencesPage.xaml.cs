using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Globalization;
using System.Linq;
using System.Runtime.InteropServices;
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

    [ValueConversion( typeof( uint ), typeof( string ) )]
    public class ComposedCharacterConverter: IValueConverter {

        public object Convert( object value, Type targetType, object parameter, System.Globalization.CultureInfo culture ) {
            uint character = (uint) value;

            string result = string.Empty;
            if ( ( character <= 0xD7FF ) || ( character >= 0xE000 && character <= 0xFFFF ) ) {
                result += (char) character;
            } else if ( character >= 0x10000 && character <= 0x10FFFF ) {
                result += (char) ( 0xD800 + ( ( character - 0x10000 ) >> 10 ) );
                result += (char) ( 0xDC00 + ( ( character - 0x10000 ) & 0x3FF ) );
            } else {
                throw new InvalidOperationException( "value must be a valid UTF-32 code point" );
            }

            return string.Format( "U+{0:X4} '{1}'", character, result );
        }

        public object ConvertBack( object value, Type targetType, object parameter, System.Globalization.CultureInfo culture ) {
            return null;
        }

    }

    [ValueConversion( typeof( uint ), typeof( string ) )]
    public class VirtualKeyConverter: IValueConverter {

        public object Convert( object value, Type targetType, object parameter, System.Globalization.CultureInfo culture ) {
            int vk = (int) ((uint) value & ~( 1 << 31 ));
            bool shifted = ( (uint) value & ( 1 << 31 ) ) != 0;
            Key key = KeyInterop.KeyFromVirtualKey( vk );
            KeyConverter keyConverter = new KeyConverter( );
            string str = (string) keyConverter.ConvertTo( key, typeof( string ) );
            if ( shifted ) {
                str = "Shift+" + str;
            }
            return str;
        }

        public object ConvertBack( object value, Type targetType, object parameter, System.Globalization.CultureInfo culture ) {
            return null;
        }

    }

    /// <summary>
    /// Interaction logic for KeySequencesPage.xaml
    /// </summary>
    public partial class KeySequencesPage: Page {

        public KeySequencesPage( ) {
            InitializeComponent( );
        }

    }

}
