#pragma once

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

class CapsLockToggler: public KeyEventHandler { };

class CapsLockPressTwiceToggler: public CapsLockToggler {
public:
	CapsLockPressTwiceToggler( );

	virtual DISPOSITION KeyDown( KBDLLHOOKSTRUCT* pkb );
	virtual DISPOSITION KeyUp( KBDLLHOOKSTRUCT* pkb );

private:
	int downCount;
	int upCount;
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
