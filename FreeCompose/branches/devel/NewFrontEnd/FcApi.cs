using System.Runtime.InteropServices;

namespace NewFrontEnd {

    public static class FcApi {

        [StructLayout( LayoutKind.Sequential )]
        public struct KeySequence {
            public uint vkFirst;
            public uint vkSecond;
            public uint u32Composed;
        }

        public enum CapsLockToggleModes {
            Normal = 1,
            PressTwice,
            Disabled,
        }

        public enum CapsLockSwapModes {
            Swap = 1,
            Replace,
        }

        public const uint FCHOOKDLL_API_VERSION = 0x0028u;

        public static uint ApiVersion {
            get {
                return FcGetApiVersion( );
            }
        }

        public static bool SetKeySequences( KeySequence[ ] sequences ) {
            return FcSetComposeKeyEntries( sequences, (uint) sequences.GetLength( 0 ) );
        }

        public static bool Enabled {
            get {
                return FcIsHookEnabled( );
            }
            set {
                if ( value ) {
                    FcEnableHook( );
                } else {
                    FcDisableHook( );
                }
            }
        }

        public static CapsLockToggleModes CapsLockToggleMode {
            get {
                return FcGetCapsLockToggleMode( );
            }
            set {
                FcSetCapsLockToggleMode( value );
            }
        }

        public static CapsLockSwapModes CapsLockSwapMode {
            get {
                return FcGetCapsLockSwapMode( );
            }
            set {
                FcSetCapsLockSwapMode( value );
            }
        }

        public static bool SwapCapsLock {
            get {
                return FcIsSwapCapsLockEnabled( );
            }
            set {
                if ( value ) {
                    FcEnableSwapCapsLock( );
                } else {
                    FcDisableSwapCapsLock( );
                }
            }
        }

        public static uint ComposeKey {
            get {
                return FcGetComposeKey( );
            }
            set {
                FcSetComposeKey( value );
            }
        }

        public static uint SwapCapsLockKey {
            get {
                return FcGetSwapCapsLockKey( );
            }
            set {
                FcSetSwapCapsLockKey( value );
            }
        }

        #region DLL imports
        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.U4 )]
        private static extern uint FcGetApiVersion( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        private static extern bool FcSetComposeKeyEntries( [In, MarshalAs( UnmanagedType.LPArray, SizeParamIndex = 1 )] KeySequence[ ] rgEntries, [In, MarshalAs( UnmanagedType.U4 )] uint cEntries );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        private static extern bool FcEnableHook( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        private static extern bool FcDisableHook( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        private static extern bool FcIsHookEnabled( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        private static extern bool FcSetCapsLockToggleMode( [In, MarshalAs( UnmanagedType.U4 )] CapsLockToggleModes mode );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.U4 )]
        private static extern CapsLockToggleModes FcGetCapsLockToggleMode( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        private static extern bool FcSetCapsLockSwapMode( [In, MarshalAs( UnmanagedType.U4 )] CapsLockSwapModes mode );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.U4 )]
        private static extern CapsLockSwapModes FcGetCapsLockSwapMode( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        private static extern bool FcEnableSwapCapsLock( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        private static extern bool FcDisableSwapCapsLock( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        private static extern bool FcIsSwapCapsLockEnabled( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        private static extern bool FcSetComposeKey( [In, MarshalAs( UnmanagedType.U4 )] uint vkCompose );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.U4 )]
        private static extern uint FcGetComposeKey( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        private static extern bool FcSetSwapCapsLockKey( [In, MarshalAs( UnmanagedType.U4 )] uint vkCapsLockSwap );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.U4 )]
        private static extern uint FcGetSwapCapsLockKey( );
        #endregion

    }

}
