#pragma once

class KeyIsXAlnum {
public:
	KeyIsXAlnum( ) {
		for ( int n = '0';      n <= '9';      n++ ) map[n] = true;
		for ( int n = 'A';      n <= 'Z';      n++ ) map[n] = true;
		for ( int n = VK_OEM_1; n <= VK_OEM_3; n++ ) map[n] = true;
		for ( int n = VK_OEM_4; n <= VK_OEM_7; n++ ) map[n] = true;

		map[VK_OEM_102] = true;
	}

	static inline bool Test( const DWORD dw ) {
		if ( dw < 256 )
			return map[dw];
		return false;
	}

private:
	static bool map[256];
};

__declspec(selectany) bool KeyIsXAlnum::map[256];
__declspec(selectany) KeyIsXAlnum _keyIsXAlnum_staticinitializer;
