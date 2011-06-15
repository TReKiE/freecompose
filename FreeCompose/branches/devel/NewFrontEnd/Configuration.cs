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
            MappingGroups = new ObservableCollection<MappingGroup>( );
        }

        Dictionary< string, FcApi.CapsLockToggleModes > mapCltm = new Dictionary< string, FcApi.CapsLockToggleModes > {
            { "normal",     FcApi.CapsLockToggleModes.Normal     },
            { "pressTwice", FcApi.CapsLockToggleModes.PressTwice },
            { "disabled",   FcApi.CapsLockToggleModes.Disabled   },
        };

        Dictionary< string, FcApi.CapsLockSwapModes > mapClsm = new Dictionary< string, FcApi.CapsLockSwapModes > {
            { "swap",    FcApi.CapsLockSwapModes.Swap    },
            { "replace", FcApi.CapsLockSwapModes.Replace },
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

                    var group = new MappingGroup { Name = eltGroup.GetAttribute( "Name" ) };
                    foreach ( XmlNode nodeMapping in eltGroup.SelectNodes( "Mapping" ) ) {
                        var eltMapping = (XmlElement) nodeMapping;
                        var Mapping = new FcApi.KeySequence( );

                        var First = getString( eltMapping, "First" );
                        var FirstShifted = eltMapping.SelectSingleNode( "First/@Shifted" );
                        var Second = getString( eltMapping, "Second" );
                        var SecondShifted = eltMapping.SelectSingleNode( "Second/@Shifted" );
                        var Composed = getString( eltMapping, "Composed" );

                        Mapping.vkFirst = uint.Parse( First );
                        Mapping.vkSecond = uint.Parse( Second );
                        Mapping.u32Composed = uint.Parse( Composed );
                        if ( null != FirstShifted ) {
                            bool t = bool.Parse( ( (XmlAttribute) FirstShifted ).Value );
                            if ( t ) {
                                Mapping.vkFirst |= 0x80000000;
                            }
                        }
                        if ( null != SecondShifted ) {
                            bool t = bool.Parse( ( (XmlAttribute) SecondShifted ).Value );
                            if ( t ) {
                                Mapping.vkSecond |= 0x80000000;
                            }
                        }

                        group.KeySequences.Add( Mapping );
                    }

                    MappingGroups.Add( group );
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
        public FcApi.CapsLockToggleModes CapsLockToggleMode { get; set; }
        public FcApi.CapsLockSwapModes CapsLockSwapMode { get; set; }
        public uint ComposeKey { get; set; }
        public uint SwapCapsLockKey { get; set; }
        public ObservableCollection<MappingGroup> MappingGroups { get; set; }
    }

    public class MappingGroup {
        public string Name { get; set; }
        public ObservableCollection<FcApi.KeySequence> KeySequences { get; set; }

        public MappingGroup( ) {
            KeySequences = new ObservableCollection<FcApi.KeySequence>( );
        }
    }

}
