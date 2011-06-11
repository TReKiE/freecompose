using System.Runtime.InteropServices;

namespace NewFrontEnd {

    public static class FcApi {

        [StructLayout( LayoutKind.Sequential )]
        public struct COMPOSE_KEY_ENTRY {
            public uint vkFirst;
            public uint vkSecond;
            public uint u32Composed;
        }

        public enum CAPS_LOCK_TOGGLE_MODE {
            CLTM_NORMAL = 1,
            CLTM_PRESSTWICE,
            CLTM_DISABLED,
        }

        public enum CAPS_LOCK_SWAP_MODE {
            CLSM_SWAP = 1,
            CLSM_REPLACE,
        }

        public const uint FCHOOKDLL_API_VERSION = 0x0026u;

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.U4 )]
        public static extern uint FcGetApiVersion( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        public static extern bool FcSetComposeKeyEntries( [In, MarshalAs( UnmanagedType.LPArray, SizeParamIndex = 1 )] COMPOSE_KEY_ENTRY[ ] rgEntries, [In, MarshalAs( UnmanagedType.U4 )] uint cEntries );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        public static extern bool FcEnableHook( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        public static extern bool FcDisableHook( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        public static extern bool FcSetCapsLockToggleMode( [In, MarshalAs( UnmanagedType.U4 )] CAPS_LOCK_TOGGLE_MODE mode );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.U4 )]
        public static extern CAPS_LOCK_TOGGLE_MODE FcGetCapsLockToggleMode( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        public static extern bool FcSetCapsLockSwapMode( [In, MarshalAs( UnmanagedType.U4 )] CAPS_LOCK_SWAP_MODE mode );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.U4 )]
        public static extern CAPS_LOCK_SWAP_MODE FcGetCapsLockSwapMode( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        public static extern bool FcEnableSwapCapsLock( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        public static extern bool FcDisableSwapCapsLock( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        public static extern bool FcSetComposeKey( [In, MarshalAs( UnmanagedType.U4 )] uint vkCompose );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.U4 )]
        public static extern uint FcGetComposeKey( );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.Bool )]
        public static extern bool FcSetSwapCapsLockKey( [In, MarshalAs( UnmanagedType.U4 )] uint vkCapsLockSwap );

        [DllImport( "FCHookDll.dll", ExactSpelling = true )]
        [return: MarshalAs( UnmanagedType.U4 )]
        public static extern uint FcGetSwapCapsLockKey( );

    }

}
