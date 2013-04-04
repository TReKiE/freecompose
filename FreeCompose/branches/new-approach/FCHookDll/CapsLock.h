#pragma once

//
// Caps Lock mutators.
//

class CapsLockMutator: public KeyEventHandler { };

class CapsLockSwapper: public CapsLockMutator {
public:
	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* pkb );
	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* pkb );
};

class CapsLockReplacer: public CapsLockMutator {
public:
	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* pkb );
	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* pkb );
};

class CapsLockMutatorFactory {
public:
	static CapsLockMutator* Create( CAPS_LOCK_SWAP_MODE clSwapMode );

private:
	CapsLockMutatorFactory( );
	CapsLockMutatorFactory( CapsLockMutatorFactory& );
	~CapsLockMutatorFactory( );
	CapsLockMutatorFactory& operator=( CapsLockMutatorFactory& );
};

//
// Caps lock togglers.
//

class CapsLockToggler: public KeyEventHandler { };

class CapsLockPressTwiceToggler: public CapsLockToggler {
public:
	CapsLockPressTwiceToggler( ): allowDown ( false ), allowUp ( false ) { }

	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* pkb );
	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* pkb );

private:
	bool allowDown;
	bool allowUp;

	DISPOSITION _Implementation( KBDLLHOOKSTRUCT* pkb, bool& allow );
};

class CapsLockDisabledToggler: public CapsLockToggler {
public:
	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* pkb );
	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* pkb );
};

class CapsLockTogglerFactory {
public:
	static CapsLockToggler* Create( CAPS_LOCK_TOGGLE_MODE clToggleMode );

private:
	CapsLockTogglerFactory( );
	CapsLockTogglerFactory( CapsLockTogglerFactory& );
	~CapsLockTogglerFactory( );
	CapsLockTogglerFactory& operator=( CapsLockTogglerFactory& );
};

//
// Top-level caps lock handler.
//

class CapsLockKeyHandler: public KeyEventHandler {
public:

	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* pkb );
	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* pkb );

private:
	DISPOSITION _Implementation( KBDLLHOOKSTRUCT* pkb );
};
