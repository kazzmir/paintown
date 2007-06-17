package com.rafkind.paintown;

import javax.swing.*;

import com.rafkind.paintown.level.Character;
import com.rafkind.paintown.level.Level;

public class CharacterEditor implements PropertyEditor {
	public CharacterEditor( Character i ){
	}

	public JComponent createPane( Level level ){
		return new JPanel();
	}
}
