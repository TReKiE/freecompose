using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml;

namespace NewFrontEnd {

    public class Configuration {

        public Configuration( ) {
            KeySequenceCollection = new ObservableCollection<KeySequenceItem>( );
        }

        Dictionary< string, CapsLockToggleModes > mapCltm = new Dictionary< string, CapsLockToggleModes > {
            { "normal",     CapsLockToggleModes.Normal     },
            { "pressTwice", CapsLockToggleModes.PressTwice },
            { "disabled",   CapsLockToggleModes.Disabled   },
        };

        Dictionary< string, CapsLockSwapModes > mapClsm = new Dictionary< string, CapsLockSwapModes > {
            { "swap",    CapsLockSwapModes.Swap    },
            { "replace", CapsLockSwapModes.Replace },
        };

        private string getString( XmlElement elt, string name ) {
            return ( (XmlElement) elt.SelectSingleNode( name ) ).InnerText;
        }

        public void Load( string fileName ) {
            try {
                XmlDocument doc = new XmlDocument( );
                doc.Load( fileName );

                var root     = doc.SelectSingleNode( "FreeCompose" );
                var options  = root.SelectSingleNode( "Options" );
                var mappings = root.SelectSingleNode( "Mappings" );
                var startup  = (XmlElement) options.SelectSingleNode( "Startup" );
                var keyboard = (XmlElement) options.SelectSingleNode( "Keyboard" );
                
                StartActive        = bool.Parse( getString( startup,  "StartActive"        ) );
                StartWithWindows   = bool.Parse( getString( startup,  "StartWithWindows"   ) );

                SwapCapsLock       = bool.Parse( getString( keyboard, "SwapCapsLock"       ) );
                CapsLockToggleMode = mapCltm[    getString( keyboard, "CapsLockToggleMode" ) ];
                CapsLockSwapMode   = mapClsm[    getString( keyboard, "CapsLockSwapMode"   ) ];
                ComposeKey         = uint.Parse( getString( keyboard, "ComposeKey"         ) );
                SwapCapsLockKey    = uint.Parse( getString( keyboard, "SwapCapsLockKey"    ) );

                foreach ( XmlNode nodeGroup in mappings.SelectNodes( "Group" ) ) {
                    var eltGroup = nodeGroup as XmlElement;
                    if ( null == eltGroup ) {
                        continue;
                    }

                    string groupName = eltGroup.GetAttribute( "Name" );
                    if ( string.IsNullOrWhiteSpace( groupName ) ) {
                        groupName = "(default)";
                    }

                    foreach ( XmlNode nodeMapping in eltGroup.SelectNodes( "Mapping" ) ) {
                        var eltMapping = (XmlElement) nodeMapping;
                        var keySequence = new KeySequenceItem( );

                        var First = getString( eltMapping, "First" );
                        var FirstShifted = eltMapping.SelectSingleNode( "First/@Shifted" );
                        var Second = getString( eltMapping, "Second" );
                        var SecondShifted = eltMapping.SelectSingleNode( "Second/@Shifted" );
                        var Composed = getString( eltMapping, "Composed" );

                        keySequence.First = uint.Parse( First );
                        keySequence.Second = uint.Parse( Second );
                        keySequence.Composed = uint.Parse( Composed );
                        keySequence.Group = groupName;
                        if ( null != FirstShifted ) {
                            bool t = bool.Parse( ( (XmlAttribute) FirstShifted ).Value );
                            if ( t ) {
                                keySequence.First |= 0x80000000;
                            }
                        }
                        if ( null != SecondShifted ) {
                            bool t = bool.Parse( ( (XmlAttribute) SecondShifted ).Value );
                            if ( t ) {
                                keySequence.Second |= 0x80000000;
                            }
                        }

                        KeySequenceCollection.Add( keySequence );
                    }
                }
            }
            catch ( Exception e ) {
                Debug.Print( "Configuration.Load: Exception: {0}", e.ToString( ) );
                throw;
            }
        }

        public void Save( string fileName ) {
        }

        public bool StartActive { get; set; }
        public bool StartWithWindows { get; set; }
        public bool SwapCapsLock { get; set; }
        public CapsLockToggleModes CapsLockToggleMode { get; set; }
        public CapsLockSwapModes CapsLockSwapMode { get; set; }
        public uint ComposeKey { get; set; }
        public uint SwapCapsLockKey { get; set; }
        public ObservableCollection<KeySequenceItem> KeySequenceCollection { get; set; }
    }

    public struct KeySequenceItem {
        public string Group { get; set; }
        public uint First { get; set; }
        public uint Second { get; set; }
        public uint Composed { get; set; }
    }

}
